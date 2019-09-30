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
 *      $Id: obj-spline.c 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-spline.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for opened and closed spline curves.                        =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjSpline type.                      =*/
/*=   - the constructors of the TobjSpline type.                       =*/
/*=                                                                    =*/
/*= See the object.h header for the function documentation.            =*/
/*= See the obj-spline.h header for the spline structure.              =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/object-priv.h"
#include "fig/obj-spline.h"
#include "fig/fig-scene.h"   // used for drawing it
#include "outdev/ldaf-hl.h"  // used for drawing it

/*======================================================================*/

#define MVARS_Self(obj)  TobjSpline *self = obj.spline

#define DBG0_E(fmt,...) DBG0_printf("spline","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("spline","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("spline","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("spline","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("spline","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("spline","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("spline","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("spline","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("spline","Leave : "fmt,__VA_ARGS__)

// initialize or reinitialize the spline nodes using the spline points.
static void default_nodes(TobjSpline *self);

/*======================================================================*/
/*= Top information.                                                   =*/

static CstrStatic spline_nameF(TobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OSP_type==FIGOSP_T_OpenApprox )  return "approximated open spline";
    if ( self->OSP_type==FIGOSP_T_OpenInterp )  return "interpreted open spline";
    if ( self->OSP_type==FIGOSP_T_ClosedApprox) return "approximated closed spline";
    return "approximated open spline";
}

static void     spline_dump(TobjAll obj, int sh)
{
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); 
    printf("--type=%d--nbpts=%d\n",
        self->OSP_type, self->OSP_points->eleNb
    );
    obj_dump_dble2set(self->OSP_points,sh+10,__func__);
}

/*======================================================================*/
/*= Utilities                                                          =*/
static Tdble3Set* od_splineO_getDble3set(Cdble2Set* ctls, CdbleSet* nodes)
{
    int i=0;
    Tdble3Set* spline = dble3set_new( ctls->eleNb + 10);
    dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    for (i+=1 ; i<ctls->eleNb ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    // add a dupplication of last point
    int last = spline->eleNb-1;
    dble3set_addPT (spline, &spline->eles[last]);
    return spline;
}

static Tdble3Set* od_splineC_getDble3set(Cdble2Set* ctls, CdbleSet* nodes)
{
    int i;
    Tdble3Set* spline = dble3set_new( ctls->eleNb + 10);
    for (i=0 ; i<ctls->eleNb ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    // add a dupplication of first points
    for (i=0 ; i<3 ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    return spline;
}

static double spline_length(TobjAll obj)
{
    MVARS_Self(obj);
    int i;
    double x, y, length = 0.f;

    Tdble3Set* spline = closed_spline(self) ? od_splineC_getDble3set(self->osp_points, self->osp_nodes)
                                            : od_splineO_getDble3set(self->osp_points, self->osp_nodes);

    Tdble2Set* line_spline = tool_splineToMLine(spline, 1);
    
    x = line_spline->eles[0].x;
    y = line_spline->eles[0].y;

    for (i = 1; i < line_spline->eleNb; i++) {
        length += sqrt(pow(x - line_spline->eles[i].x, 2) + pow(y - line_spline->eles[i].y, 2));
        x = line_spline->eles[i].x;
        y = line_spline->eles[i].y;
    }

    dble2set_delete(line_spline);
    dble3set_delete(spline);

    return length;
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

static void    spline_free (TobjAll obj)
{
    MVARS_Self(obj);
    dble2set_deleteS0( &self->OSP_points );
    dbleset_deleteS0 ( &self->OSP_nodes );
    dble2set_deleteS0( &self->OSP_drawctls );
    dbleset_deleteS0 ( &self->OSP_drawndes );
}

//static Tobject spline_dup  (Cobject obj);

static void    spline_clone(TobjAll obj)
{
    MVARS_Self(obj);
    self->OSP_points   = dble2set_newClone( self->OSP_points );
    self->OSP_nodes    = dbleset_newClone ( self->OSP_nodes  );
    self->OSP_drawctls = 0; 
    self->OSP_drawndes = 0; 
    self->O_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

static TobjAll spline_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
    clone = obj_spline_newA(att,self->OSP_type,
                            dble2set_newClone(self->OSP_points));
    return clone;
}

/*======================================================================*/
/*= Bounding box                                                       =*/

//static TboxDble spline_boundBoxApprox(F_point* p);

static TboxDble spline_boundBoxGeneral(Cdble2Set* pts, CdbleSet* sfs)
{
    TboxDble bb;
    int xmin, ymin, xmax, ymax;
    int x0, y0, x1, y1, x2, y2 ,x ,y;

    int n=0;

#define NEXT(s,n) ((n+1)<(s)->eleNb ? pts->eles+(n)+1 : 0)
    Tdble2*   cur_point   = pts->eles+n;
    Tdble2*   next_point  = NEXT(pts,n);
    double    cur_sfactor = sfs->eles[n];

    xmin = xmax = x0 = x1 = cur_point->x;
    ymin = ymax = y0 = y1 = cur_point->y;
    x2 = next_point->x;
    y2 = next_point->y;
  
    n += 1;
    while (1) {
        cur_point  = next_point;
        next_point = NEXT(pts,n);
        if (next_point == NULL)
          next_point = pts->eles+0;  /* usefull for closed splines,
                                      * no consequences on open splines */
        cur_sfactor = sfs->eles[n];

        x0 = x1;
        y0 = y1;
        x1 = x2;
        y1 = y2;
        x2 = next_point->x;
        y2 = next_point->y;
        
        if (cur_sfactor < 0) {
          x = abs(x2 - x0)>>2;
          y = abs(y2 - y0)>>2;
        } else {
          x = y = 0;
        }
        
        xmax = max2(xmax, x1+x);
        xmin = min2(xmin, x1-x);
        ymax = max2(ymax, y1+y);
        ymin = min2(ymin, y1-y);
        n += 1;
        if (n==pts->eleNb)
          break;
    }
    Box_x(bb)  = xmin;
    Box_y(bb)  = ymin;
    Box_dx(bb) = xmax-xmin;
    Box_dy(bb) = ymax-ymin;
    return bb;
}

static TboxDble spline_boundBox(CobjAll obj)
{
    MVARS_Self(obj);
    TboxDble bb;
    if (approx_spline(self))
        bb = tool_boxDble_initByDble2S(self->OSP_points);
    else
        bb = spline_boundBoxGeneral(self->OSP_points,self->OSP_nodes);
    return bb;
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void spline_translation(TobjAll obj, double tx, double ty)           { MVARS_Self(obj); dble2set_translation(self->OSP_points, tx,ty); }
static void spline_homothety00(TobjAll obj, int wtcf, double f)             { MVARS_Self(obj); dble2set_homothety00(self->OSP_points, f); }
static void spline_homothetyCC(TobjAll obj, int wtcf, double f,
                                                      double cx, double cy) { MVARS_Self(obj); dble2set_homothety  (self->OSP_points, f,cx,cy); }
static void spline_scale00    (TobjAll obj, int wtcf, double fx, double fy) { MVARS_Self(obj); dble2set_scale00    (self->OSP_points, fx,fy); }
static void spline_scaleCC    (TobjAll obj, int wtcf, double fx, double fy,
                                                      double cx, double cy) { MVARS_Self(obj); dble2set_scale      (self->OSP_points, fx,fy, cx,cy); }
static void spline_rotate     (TobjAll obj, double a, double cx, double cy) { MVARS_Self(obj); dble2set_rotate     (self->OSP_points, a, cx,cy); }
static void spline_flip       (TobjAll obj, int h, int v,int r,
                                                      double cx, double cy) { MVARS_Self(obj); dble2set_flip       (self->OSP_points, h,v,r, cx,cy); }

/*======================================================================*/
/*= Drawing and filling                                                =*/

static void spline_doDrawData(CobjAll   obj)
{
    MVARS_Self(obj);
    dble2set_newORclr( &self->OSP_drawctls, 10 );
    dbleset_newORclr ( &self->OSP_drawndes, 10 );
    dble2set_copy    (self->OSP_drawctls,self->OSP_points);
    dbleset_copy     (self->OSP_drawndes,self->OSP_nodes);
    obj_arrow_doEndSpline(self->OSP_endArrow,self->OSP_drawctls,self->OSP_drawndes);
    obj_arrow_doBegSpline(self->OSP_begArrow,self->OSP_drawctls,self->OSP_drawndes);
}

static void spline_draw_sc(CobjAll   obj, Tfig_scene* sc,
                         Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);

    Tdble2Set* ctls  =  self->OSP_drawctls;
    TdbleSet*  nodes =  self->OSP_drawndes;

    if ( gcf!=OD_GC_None ) {
        TEMP_ase_printf(self->OSP_fill.dpf_style==DA_FS_NoFill,
                "NoFill (0x%x) invalid fill style",
                self->OSP_fill.dpf_style);
        if ( closed_spline(self)!=0 )
            od_fill_splineC(sc->sc_device, gcf, ctls,nodes);
        else
            od_fill_splineO(sc->sc_device, gcf, ctls,nodes);
    }
    if ( gcd!=OD_GC_None ) {
        TEMP_ase_printf(self->OSP_line.dpl_style==DA_LS_None,
                "None (0x%x) invalid line style",
                self->OSP_line.dpl_style);
        if ( closed_spline(self)!=0 )
            od_draw_splineC(sc->sc_device, gcd, ctls,nodes);
        else
            od_draw_splineO(sc->sc_device, gcd, ctls,nodes);
    }
}

/*======================================================================*/
/*= Localization.                                                      =*/

static int spline_markerNbF   (TobjAll obj)
{ MVARS_Self(obj); return self->OSP_points->eleNb; }

static int spline_markerNbMinF(TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OSP_type ) {
        case FIGOSP_T_OpenApprox:  return 2;
        case FIGOSP_T_OpenInterp:  return 2;
        case FIGOSP_T_ClosedApprox:return 3;
        default:                   return 3; // FIGOSP_T_ClosedInterp
    }
}

static int spline_markerNbCstF(TobjAll obj) { return 0; }

//static int spline_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *mkr);

/*======================================================================*/
/*= Modification.                                                      =*/

static Tdble2 spline_mkrOpeInsert   (TobjAll obj, int idx, Cdble2*pos)
{
    MVARS_Self(obj);
    Tdble2Set* points = self->OSP_points;
    Tdble2     mkr;
    
    if      ( pos!=0 )              mkr = *pos;
    else if ( idx==points->eleNb )  mkr = points->eles[idx-1];
    else                            mkr = points->eles[idx];

    if ( idx==points->eleNb )
        dble2set_addPT(points,mkr);
    else
        dble2set_addBeforePT(points,idx,mkr);
    default_nodes(self);
    self->O_tags &= ~FIG_TAG_BBoxUpToDate;
    self->O_tags &= ~FIG_TAG_DrawingDataReady;
    return mkr;
}

static void   spline_mkrOpeChange   (TobjAll obj, int idx, Tdble2 pt)
{
    MVARS_Self(obj);
    self->OSP_points->eles[idx] = pt;
    self->O_tags &= ~FIG_TAG_BBoxUpToDate;
    self->O_tags &= ~FIG_TAG_DrawingDataReady;
}

static void   spline_mkrOpeDelete   (TobjAll obj, int idx)
{
    MVARS_Self(obj);
    dble2set_supShift(self->OSP_points,idx);
    default_nodes(self);
    self->O_tags &= ~FIG_TAG_BBoxUpToDate;
    self->O_tags &= ~FIG_TAG_DrawingDataReady;
}

static int    spline_mkrOpeEnabled  (TobjAll obj)
{
    MVARS_Self(obj);
    int ret = FIGO_MkrOpe_Chg;
    ret |= FIGO_MkrOpe_Ins;
    if ( self->OSP_points->eleNb > spline_markerNbMinF(obj) ) ret |= FIGO_MkrOpe_Del;
    return ret;
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable spline_vtable = {
    .vt_kind        = FIG_KD_Spline,
    .vt_bytesz      = sizeof( TobjSpline ),
    .vt_mask        = FIG_KDM_SPL,
    .vt_subKind     = offsetof(TobjSpline,OSP_type),
//  .vt_fdc         = offsetof(TobjSpline,OSP_curve),
    .vt_fdl         = offsetof(TobjSpline,OSP_line),
    .vt_fdf         = offsetof(TobjSpline,OSP_fill),
    .vt_begArrow    = offsetof(TobjSpline,OSP_begArrow),
    .vt_endArrow    = offsetof(TobjSpline,OSP_endArrow),
    .vt_name        = "spline",
    .vt_nameF       = spline_nameF,
    .vt_dump        = spline_dump,
    .vt_length      = spline_length,

    .vt_free        = spline_free,
//  .vt_dup         = spline_dup,
    .vt_clone       = spline_clone,
    .vt_dupNDP      = spline_dupNDP,
    .vt_boundBox    = spline_boundBox,

    .vt_translation = spline_translation,
    .vt_homothety00 = spline_homothety00,
    .vt_homothetyCC = spline_homothetyCC,
    .vt_scale00     = spline_scale00,
    .vt_scaleCC     = spline_scaleCC,
    .vt_rotate      = spline_rotate,
    .vt_flip        = spline_flip,

    .vt_doDrawData  = spline_doDrawData,
    .vt_draw_sc     = spline_draw_sc,

    .vt_markerNbF     = spline_markerNbF,
    .vt_markerNbMinF  = spline_markerNbMinF,
    .vt_markerNbCstF  = spline_markerNbCstF,
    .vt_markerS       = offsetof(TobjSpline,OSP_points),
//  .vt_closeto       = spline_closeto,

    .vt_mkrOpeInsert  = spline_mkrOpeInsert,
    .vt_mkrOpeChange  = spline_mkrOpeChange,
    .vt_mkrOpeDelete  = spline_mkrOpeDelete,
    .vt_mkrOpeEnabled = spline_mkrOpeEnabled,
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void default_nodes(TobjSpline *self)
{
    int i;
    dbleset_newORclr(&self->OSP_nodes,self->OSP_points->eleNb);
    TdbleSet* ndes = self->OSP_nodes;
    double    type_s = approx_spline(self) ? S_SPLINE_APPROX : S_SPLINE_INTERP;
    
    dbleset_add(ndes, closed_spline(self) ? type_s : S_SPLINE_ANGULAR);
    
    for(i=2; i<self->OSP_points->eleNb ; i+=1)
        dbleset_add(ndes, type_s);
    
    dbleset_add(ndes, closed_spline(self) ? type_s : S_SPLINE_ANGULAR);
}

// assuming that self fields are to low value
static void obj_spline_init( TobjSpline* self,
              int type, Tdble2Set* ctls)
{
    self->OSP_type   = type;
    self->OSP_points = ctls;
    default_nodes(self);

    switch (type) {
      case FIGOSP_T_OpenApprox:
      case FIGOSP_T_OpenInterp:
        self->O_tags |= FIG_TAG_CanHaveArrow;
        break;
      case FIGOSP_T_ClosedApprox:
      case FIGOSP_T_ClosedInterp:
        break;
      default:
        PERM_spe_printf("%d is an unexpected kind.",type);
        PERM_spf_printf("recognized are: "
                "OpApp=%d ClApp=%d OpInt=%d ClInt=%d",
                FIGOSP_T_OpenApprox,   FIGOSP_T_ClosedApprox,
                FIGOSP_T_OpenInterp,   FIGOSP_T_ClosedInterp);
    }
}

extern TobjAll obj_spline_newF(TobjVTT vtt,
                        int depth, Csty_line* stl, Csty_fill* stf,
                        int type, Tdble2Set* ctls)
{
    TobjAll self;
    self=obj_newByODS(FIG_KD_Spline,vtt,depth,stl,stf,NULL);
    obj_spline_init(self.spline,type,ctls);
    return self;
}

extern TobjAll obj_spline_newA(Cfdp* fdp, int type, Tdble2Set* ctls)
{
    TobjAll self;
    self=obj_newByFdp(FIG_KD_Spline,fdp);
    obj_spline_init(self.spline,type,ctls);
    return self;
}

/*======================================================================*/
