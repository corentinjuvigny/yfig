/*======================================================================
 *  Copyright 2017 Ivan Augé
 *
 *  This file is part of the YFIG software.
 * 
 *  YFIG is free  software;  you  can redistribute it and/or modify it
 *  under the terms of the GNU  General Public License as published by
 *  the  Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  YFIG software is distributed  in the  hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the  implied  warranty  of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  the GNU
 *  General Public License for more details.
 * 
 *  You should have received  a copy of the GNU General Public License
 *  along with YFIG. If not, see <http://www.gnu.org/licenses/>.
**======================================================================
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-arrow.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arrow.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Arrows of types 00 and 01.                                         =*/
/*=                                                                    =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjAny type.                         =*/
/*=   - the constructors of the TobjArrow type.                        =*/
/*=                                                                    =*/
/*= See obj-arrow.h header for data structure of object.               =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "fig/object-priv.h"
#include "fig/obj-arrow.h"

#include "outdev/ldaf-hl.h"  // used for drawing it
#include "tools/color.h"
#include "tools/geo2d.h"
#include "tools/lenunit.h"

/*======================================================================*/

#define MVARS_Self(obj) TobjArrow *self = obj.arrow
#define MVARS_SeVt(obj) MVARS_Self(obj); CobjVtable* vtable =  self->O_vtable

#define DBG0_E(fmt,...) DBG0_printf("arrow","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("arrow","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("arrow","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("arrow","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("arrow","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("arrow","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("arrow","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("arrow","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("arrow","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Arrow head reference points.                                       =*/
/*=                                                                    =*/
/*=   4 points arrow head                5 points arrow head           =*/
/*=      pt1                                pt1                        =*/
/*=       x....                            ..x....                     =*/
/*=       |    ....                      ..  |    ....                 =*/
/*=       |        ....                ..    |        ....             =*/
/*=       x pts     pte x..      pts .x      x ptm     pte x..         =*/
/*=       |        ....                ..    |        ....             =*/
/*=       |    ....                      ..  |    ....                 =*/
/*=       x....                            ..x....                     =*/
/*=      pt2                                pt2                        =*/
/*=                                                                    =*/
/*=  - pts,ptm,pte mean start, middle, end.                            =*/
/*=  - pte is not the real end of arrow, the real end is the tip due   =*/
/*=    to the join style.                                              =*/
/*=  - pts is not the real start of arrow the real start is:           =*/
/*       * at half line thickness for 4 points arrow head.             =*/
/*       * the tip due to the join style for 5 points arrow head.      =*/

typedef struct _Tarrowhead {
    Tdble2 ah_pte, ah_pts, ah_ptm;
    Tdble2 ah_pt1, ah_pt2;
} Tarrowhead;

static Tarrowhead arrowhead4_init(
        Cdble2* pte, CobjArrow* oa, double cosa, double sina)
{
    Tdble2 pt;
    double angle = oa->OAW_baseangle;
    double len   = oa->OAW_currlength;
    Tarrowhead ret;
    ret.ah_pte = *pte;
    ret.ah_pts = dble2_init(pte->x + len * cosa, pte->y + len * sina);
    ret.ah_ptm = dble2_init(0,0);
    len        = len/cos(angle);
    pt         = dble2_init(pte->x + len * cosa, pte->y + len * sina);
    ret.ah_pt1 = dble2_initRot(&pt,-angle,pte);
    ret.ah_pt2 = dble2_initRot(&pt,+angle,pte);
    return ret;
}

/**
 * Fills in the ah structure and updates the pts line/splie points
 * for arrow with 4 reference points.
 * Return 0 on success otherwise -1
**/
static int arrow_doAH4Line(Tarrowhead*ah, int atBeg,
        CobjAll obj, Tdble2Set* pts, double deltaL, double deltaA)
{
    int i;
    MVARS_Self(obj);

    if ( pts==0 || pts->eleNb<2 ) return -1; // not enough points

    // For atBeg==0:
    //   We search ptL on the line starting from line end (pts[N-1])
    //   and going to the line beginning (pts[0]) that verifies:
    //    - ptL != pts[N-1]
    //    - distance(ptL,pts[N-1])>delta
    //   pts0   pts1  pts2  pts3  pts4  pts5  pts6 
    //                            ptL        pts[N-1]
    //
    // For atBeg!=0:
    //   It similar but we search from line beginning to line end
    //   pts0   pts1  pts2  pts3  pts4  pts5  pts6 
    //   pts[0]       ptL
    int    ptLidx,ptFoundIdx;
    Tdble2 ptL,   ptFound;
    if ( atBeg==0 ) {
        ptLidx = pts->eleNb-1;
        ptL    = pts->eles[ptLidx];
        for ( ptFoundIdx=ptLidx-1 ; 1 ; ptFoundIdx-=1) {
            if (ptFoundIdx<0) return -1; // no 2 enough different points
            ptFound =  pts->eles[ptFoundIdx];
            if ( dble2_cmpPT(&ptFound,&ptL)==0 ) continue;
            if ( ! dble2_closeAtP(&ptFound,&ptL,deltaL) ) break;
        }
        for (i=ptLidx-1 ; i>ptFoundIdx ; i-=1 )
            dble2set_supShift(pts,ptFoundIdx+1);
        ptLidx = pts->eleNb-1;
    } else {
        ptLidx = 0;
        ptL    = pts->eles[ptLidx];
        for ( ptFoundIdx=ptLidx+1 ; 1 ; ptFoundIdx+=1) {
            if (ptFoundIdx>=pts->eleNb) return -1; // no 2 enough different points
            ptFound = pts->eles[ptFoundIdx];
            if ( dble2_cmpPT(&ptL,&ptFound)==0 ) continue;
            if ( ! dble2_closeAtP(&ptL,&ptFound,deltaL) ) break;
        }
        // FIXME very slow if ptFoundIdx is large
        for (i=ptLidx+1 ; i<ptFoundIdx ; i+=1 )
            dble2set_supShift(pts,1);
    }

    // compute the factors lfx, lfy of eptL------sptL------ line
    // that are useful to get other line points
    Tdble2 vLF = dble2_initVab(ptL,ptFound);
    double d   = VECTOR_NormePT(vLF);
    double lfx = dble_close0AtP(vLF.x,1e-2) ? 0 : vLF.x/d; // line factor for x.
    double lfy = dble_close0AtP(vLF.y,1e-2) ? 0 : vLF.y/d; // line factor for y.

    // update pts[ptLidx] by moving it of deltaL on line ptL----ptFound
    // note distance(ptL,ptFound)>deltaL
    if ( deltaL!=0 ) {
        pts->eles[ptLidx].x = ptL.x + deltaL * lfx;
        pts->eles[ptLidx].y = ptL.y + deltaL * lfy;
    }

    // by moving ptL of deltaA on line ptL----ptFound,
    // we get the arrow head reference point pte
    // and then the others.
    Tdble2 pte;
    pte.x = ptL.x + deltaA * lfx;
    pte.y = ptL.y + deltaA * lfy;
    *ah = arrowhead4_init(&pte, self, lfx, lfy);

    return 0;
}

/**
 * Fills in the ah structure and updates the pts arcs points
 * for arrow with 4 reference points.
 * Return 0 on success otherwise -1
**/
static int arrow_doAH4Arc(Tarrowhead*ah, int atBeg,
        CobjAll obj, Tdble2 pts[4], int dir,
        double deltaL, double deltaA)
{
    MVARS_SeVt(obj);
    Tdble2 v;
    Tdble2 center  = pts[3];

    // enough arc for arrow?
    Tdble2 vcb     = dble2_initVab(center,pts[0]);
    Tdble2 vce     = dble2_initVab(center,pts[2]);
    double arcradius  = VECTOR_NormePT(vcb);
    double deltaangle = deltaL/(TOOL_2PI*arcradius);
    double arcangle   = dir==0
                      ? tool_angleOfVectors_PTD(&vcb,&vce) // clockwise
                      : tool_angleOfVectors_PTD(&vce,&vcb);
    if ( (abs(arcangle)-deltaangle)<DegToRad(1) ) 
        return -1;

    // We search for spL the real arc beginning (atBeg==1) or thof
    // For atBeg==1:
    //   We search for ptL on the arc starting from pts[0] and
    //   going to arc end that verifies:
    //    - ptL != pts[0]
    //    - distance(ptL,pts[0])=delta
    //
    // For atBeg==0:
    //   It is similar but for  arc end (pts[2] to pts[0])
    Tdble2 ptL;
    if ( deltaL!=0 ) {
        if ( atBeg==0 ) {
            ptL = pts[2];
        } else {
            ptL = pts[0];
        }
    } else {
        if ( atBeg==0 ) {
            if (dir==0) // clockwide
                ptL = dble2_initRot(pts+2,+deltaangle,&center);
            else
                ptL = dble2_initRot(pts+2,-deltaangle,&center);
            pts[2] = ptL;
        } else {
            if (dir==0) // clockwide
                ptL = dble2_initRot(pts+0,-deltaangle,&center);
            else
                ptL = dble2_initRot(pts+0,+deltaangle,&center);
            pts[0] = ptL;
        }
        // adjust pts[1]
        double angle_vcb, angle_vcm, angle_vce;
        v=dble2_initVab(center,pts[0]); angle_vcb = tool_angleOfVector_PTD( &v );
        v=dble2_initVab(center,pts[1]); angle_vcm = tool_angleOfVector_PTD( &v );
        v=dble2_initVab(center,pts[2]); angle_vce = tool_angleOfVector_PTD( &v );
        if ( !(angle_vcb<angle_vcm && angle_vcm<angle_vce) &&
             !(angle_vcb>angle_vcm && angle_vcm>angle_vce) ) {
            // we must adjust the middle point, we place it in the middle
            angle_vcm = (angle_vcb - angle_vce)/2;
            pts[1] = dble2_initRot(pts+0,angle_vcm,&center);
        }
    }

    // get the reference points of the arrow head
    //  - angle gives the arrow direction (base line of arrow)
    v=dble2_initVab(center,ptL);
    double angle = tool_angleOfVector_PTD( &v );
    angle += atBeg ? (dir==0 ? +TOOL_PI_2 : -TOOL_PI_2)
                   : (dir==0 ? -TOOL_PI_2 : +TOOL_PI_2);
    *ah = arrowhead4_init(&ptL, self,cos(angle), sin(angle));

    return 0;
}

/**
 * Fills in the ah structure and updates the pts line/splie points
 * for arrow with 5 reference points.
 * Return 0 on success otherwise -1
**/
static int arrow_doAH5Line(Tarrowhead*ah, int atBeg,
        CobjAll obj, Tdble2Set* pts,
        double deltaL, double deltaA, double coef)
{
    if ( arrow_doAH4Line(ah,atBeg,obj,pts,deltaL,deltaA)!=0 )
        return -1;
    ah->ah_ptm = ah->ah_pts;
    // ES = coef EM
    ah->ah_pts.x = ah->ah_pte.x + coef * ( ah->ah_ptm.x - ah->ah_pte.x);
    ah->ah_pts.y = ah->ah_pte.y + coef * ( ah->ah_ptm.y - ah->ah_pte.y);
    return 0;
}

/**
 * Fills in the ah structure and updates the pts arcs points
 * for arrow with 5 reference points.
 * Return 0 on success otherwise -1
**/
static int arrow_doAH5Arc(Tarrowhead*ah, int atBeg,
        CobjAll obj, Tdble2 pts[4], int dir,
        double deltaL, double deltaA, double coef)
{
    if ( arrow_doAH4Arc(ah,atBeg,obj,pts,dir,deltaL,deltaA)!=0 )
        return -1;
    ah->ah_ptm = ah->ah_pts;
    // ES = coef EM
    ah->ah_pts.x = ah->ah_pte.x + coef * ( ah->ah_ptm.x - ah->ah_pte.x);
    ah->ah_pts.y = ah->ah_pte.y + coef * ( ah->ah_ptm.y - ah->ah_pte.y);
    return 0;
}

/*======================================================================*/
/*= dump and figure                                                    =*/

static void arrow_dump(TobjAll obj, int sh)
{
    int i;
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); 
    printf("\n%20s:",__func__);
    for (i=0;i<sh+10;i++) printf(" ");
    printf("--abs=%s--basedx/angle=%f/%f--curdx=%f\n",
            self->OAW_baseabs ? "Abs-" : "Prop",
            self->OAW_baselength, self->OAW_baseangle,
            self->OAW_currlength
    );
}

/*======================================================================*/
/*= free, dup and clone                                                =*/

static void    arrow_free (TobjAll obj)
{
    MVARS_Self(obj);
    dble2set_delete( self->OAW_pts1 );
    dble2set_delete( self->OAW_pts2 );
    dble2set_delete( self->OAW_pts3 );
}

static void    arrow_clone(TobjAll obj)
{
    MVARS_Self(obj);
    self->OAW_pts1 = dble2set_newClone( self->OAW_pts1 );
    self->OAW_pts2 = dble2set_newClone( self->OAW_pts2 );
    self->OAW_pts3 = dble2set_newClone( self->OAW_pts3 );
}

static TboxDble arrow_boundBox(CobjAll obj)
{
    TboxDble ret,tmp;
    MVARS_Self(obj);
    if ( (self->O_tags&FIG_TAG_DrawingDisabled)!=0 || 
         (self->O_tags&FIG_TAG_DrawingDataReady)==0 )
         return tool_boxDble_undef;
    ret = tool_boxDble_initByDble2S(self->OAW_pts1);
    if ( self->OAW_pts2 )
        tool_boxDble_union(&ret,tool_boxDble_initByDble2Sptr(self->OAW_pts2,&tmp));
    if ( self->OAW_pts3 )
        tool_boxDble_union(&ret,tool_boxDble_initByDble2Sptr(self->OAW_pts3,&tmp));
    return ret;
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void arrow_translation(TobjAll obj, double tx, double ty)
{
    // nothing to do because the arrow will be computed again before drawing.
}

static void arrow_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{
    MVARS_Self(obj);
    // we don't scale legnth that are relative to line thickness because
    // the line thickness is scaled
    if ( (wtcf&FIGO_SA_AllDim)!=0 && self->OAW_baseabs!=0 )
        SCALE00_Dim(self->OAW_baselength,abs(f));
}

static void arrow_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{
    MVARS_Self(obj);
    // we don't scale legnth that are relative to line thickness because
    // the line thickness is scaled
    if ( (wtcf&FIGO_SA_AllDim)!=0 && self->OAW_baseabs!=0 ) {
        SCALE00_Dim(self->OAW_baselength,(abs(fx)+abs(fy))/2);
    }
}

static void arrow_rotate(TobjAll obj, double a, double cx, double cy)
{
    // nothing to do because the arrow will be computed again before drawing.
    // but arrow_rotate must be present to disable the default rotation.
}

/*======================================================================*/
/*= Drawing and filling                                                =*/

#if 0
static void arrow_draw_sc(CobjAll   obj, Tfig_scene* sc,
                          Tod_GCdef gcd, Tod_GCdef gcf)
{
    CDEF_STD_VARS_OA(obj);

    if ( gcd!=OD_GC_None ) {
        if ( self->OAW_pts1 && self->OAW_pts1->eleNb )
            od_draw_mlineS(sc->sc_device,gcd,self->OAW_pts1);
        if ( self->OAW_pts2 && self->OAW_pts2->eleNb )
            od_draw_mlineS(sc->sc_device,gcd,self->OAW_pts2);
    }
}
#endif

/*======================================================================*/
/*= The tables                                                         =*/

#include "fig/obj-vtable.h"

#include "obj-arrows/00.c"
#include "obj-arrows/01.c"
#include "obj-arrows/02to04.c"

/*======================================================================*/
/*= Constructors                                                       =*/

static TobjArrow* obj_arrow_new(TobjAll pobj, void*_vt, Csty_fill* stf,
                             double len, double a, int abs, double th)
{
    TobjVtable*    vt = _vt;
    TobjArrow     *self = obj_new( _vt ).arrow;
    self->O_partof = pobj;

    self->OAW_stymask = FDP_C_all ; // all except join and cap
    self->OAW_line = odsty_copyLine( obj_StyLine(pobj) );
    if ( abs!=0 ) {
        self->OAW_thickness = th;
        self->OAW_stymask  &= ~FDP_L_thick;
    }
    self->OAW_join = DA_JS_Miter;
    self->OAW_cap  = DA_CS_Butt;

    if ( vt->vt_fdf!=0 ) {
        if ( stf==0 ) {
            self->OAW_fill_style = DA_FS_Solid;
            self->OAW_fill_color = color_newLazy( self->OAW_pen_color );
            self->OAW_stymask   |= FDP_F_color;
        } else {
            self->OAW_fill     = odsty_copyFill( stf );
        }
    } else
        self->OAW_fill_style       = DA_FS_NoFill;
    self->OAW_baseabs    = abs;
    self->OAW_baselength = len;
    self->OAW_baseangle  = a;
    self->O_ppi = pobj.any->OA_ppi;

    return self;
}

extern void obj_arrow_chgRel(TobjAll op, int atBeg, void* vt,
                             Csty_fill* stf, double len, double a)
{
    TobjAll* opArrow = atBeg ? obj_arrow_BegPtr(op) : obj_arrow_EndPtr(op);
    if (opArrow==0) return;
    obj_delete( *opArrow );
    opArrow->arrow = obj_arrow_new(op, vt, stf, len, a, 0, 0.0);
    op.any->OA_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

extern void obj_arrow_chgAbs(TobjAll op, int atBeg, void* vt,
                             Csty_fill* stf, double len, double a, double th)
{
    TobjAll* opArrow = atBeg ? obj_arrow_BegPtr(op) : obj_arrow_EndPtr(op);
    if (opArrow==0) return;
    obj_delete( *opArrow );
    opArrow->arrow = obj_arrow_new(op, vt, stf, len, a, 1, th);
    op.any->OA_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

extern void obj_arrow_permute(TobjAll op)
{
    TobjAll* begArrow = obj_arrow_BegPtr(op);
    TobjAll* endArrow = obj_arrow_EndPtr(op);
    if ( begArrow==0 || endArrow==0 ) return;
    TobjAll tmp = *begArrow;
    *begArrow = *endArrow;
    *endArrow = tmp;
    op.any->OA_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

extern void obj_arrow_delete(TobjAll op, int atBeg)
{
    TobjAll* opArrow = atBeg ? obj_arrow_BegPtr(op) : obj_arrow_EndPtr(op);
    obj_deleteS0( opArrow );
    op.any->OA_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

extern void obj_arrow_getParams(TobjAll ao, CobjVtable**vtable, int*abs, Csty_fill**fill,
                                double*len, double*angle, double*th )
{
    TobjArrow* self = ao.arrow;
    *vtable =  self->O_vtable;
    *abs    =  self->OAW_baseabs;
    *fill   = &self->OAW_fill;
    *len    =  self->OAW_baselength;
    *angle  =  self->OAW_baseangle;
    *th     =  self->OAW_thickness;
}
/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void obj_arrow_doBegEndLine(TobjAll oa, Tdble2Set* pts,
    void (*vt_arrowDoBegEndLine)(CobjAll obj, Tdble2Set* pts) )
{
    MVARS_SeVt(oa);
    TobjAll poa = self->O_partof;

    // update drawing parameters 
    odsty_upteLineM(&self->OAW_line,obj_StyLine(poa),self->OAW_stymask);
    if ( (self->OAW_stymask&FDP_F_color)!=0 )
        color_chgLazy(&self->OAW_fill_color,self->OAW_pen_color);

    self->OAW_currlength = lu_convFromFuPpi(self->OAW_baselength,self->O_ppi);
    if ( self->OAW_baseabs==0 ) self->OAW_currlength *= self->OAW_thickness;

    // clear drawing data
    dble2set_deleteS0( &self->OAW_pts1 );
    dble2set_deleteS0( &self->OAW_pts2 );
    dble2set_deleteS0( &self->OAW_pts3 );

    // call driver that builds drawing data
    if ( vt_arrowDoBegEndLine ) vt_arrowDoBegEndLine(oa,pts);
}

extern void obj_arrow_doBegLine(TobjAll oa, Tdble2Set* pts)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndLine(oa,pts,vtable->vt_arrowDoBegLine);
}

extern void obj_arrow_doEndLine(TobjAll oa, Tdble2Set* pts)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndLine(oa,pts,vtable->vt_arrowDoEndLine);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void obj_arrow_doBegEndSpline(TobjAll oa, Tdble2Set* ctls, TdbleSet* ndes,
    void (*vt_arrowDoBegEndSpline)(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes) )
{
    MVARS_SeVt(oa);
    TobjAll poa = self->O_partof;

    // update drawing parameters 
    odsty_upteLineM(&self->OAW_line,obj_StyLine(poa),self->OAW_stymask);
    if ( (self->OAW_stymask&FDP_F_color)!=0 )
        color_chgLazy(&self->OAW_fill_color,self->OAW_pen_color);

    self->OAW_currlength = lu_convFromFuPpi(self->OAW_baselength,self->O_ppi);
    if ( self->OAW_baseabs==0 ) self->OAW_currlength *= self->OAW_thickness;

    // clear drawing data
    dble2set_deleteS0( &self->OAW_pts1 );
    dble2set_deleteS0( &self->OAW_pts2 );
    dble2set_deleteS0( &self->OAW_pts3 );

    // call driver that builds drawing data
    if ( vt_arrowDoBegEndSpline ) vt_arrowDoBegEndSpline(oa,ctls,ndes);
}

extern void obj_arrow_doBegSpline(TobjAll oa, Tdble2Set* ctls, TdbleSet*ndes)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndSpline(oa,ctls,ndes,vtable->vt_arrowDoBegSpline);
}

extern void obj_arrow_doEndSpline(TobjAll oa, Tdble2Set* ctls, TdbleSet*ndes)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndSpline(oa,ctls,ndes,vtable->vt_arrowDoEndSpline);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void obj_arrow_doBegEndArc(TobjAll oa, Tdble2 pts[4], int dir,
    void (*vt_arrowDoBegEndArc)(CobjAll obj, Tdble2 pts[4], int dir) )
{
    MVARS_SeVt(oa);
    TobjAll poa = self->O_partof;

    // update drawing parameters 
    odsty_upteLineM(&self->OAW_line,obj_StyLine(poa),self->OAW_stymask);
    if ( (self->OAW_stymask&FDP_F_color)!=0 )
        color_chgLazy(&self->OAW_fill_color,self->OAW_pen_color);

    self->OAW_currlength = lu_convFromFuPpi(self->OAW_baselength,self->O_ppi);
    if ( self->OAW_baseabs==0 ) self->OAW_currlength *= self->OAW_thickness;

    // clear drawing data
    dble2set_deleteS0( &self->OAW_pts1 );
    dble2set_deleteS0( &self->OAW_pts2 );
    dble2set_deleteS0( &self->OAW_pts3 );

    // call driver that builds drawing data
    if ( vt_arrowDoBegEndArc ) vt_arrowDoBegEndArc(oa,pts,dir);
}

extern void obj_arrow_doBegArc(TobjAll oa, Tdble2 pts[4], int dir)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndArc(oa,pts,dir,vtable->vt_arrowDoBegArc);
}

extern void obj_arrow_doEndArc(TobjAll oa, Tdble2 pts[4], int dir)
{
    if ( OBJ_IsNone(oa) ) return;
    MVARS_SeVt(oa);
    obj_arrow_doBegEndArc(oa,pts,dir,vtable->vt_arrowDoEndArc);
}

/*======================================================================*/
