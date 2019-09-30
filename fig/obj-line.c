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
 *      $Id: obj-line.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-line.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for opened and closed multi-lines, rectangular boxes        =*/
/*= and pictures.                                                      =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjLine type.                        =*/
/*=   - the constructors of the TobjLine type.                         =*/
/*= See obj-line.h header for data structure of object.                =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/object-priv.h"
#include "fig/obj-line.h"
#include "fig/fig-scene.h"   // used for drawing it
#include "outdev/ldaf-hl.h"  // used for drawing it

#include "tools/lenunit.h"  // used for drawing it
#include "math.h"

/*======================================================================*/

#define MVARS_Self(obj)  TobjLine *self = obj.line

#define DBG0_E(fmt,...) DBG0_printf("line","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("line","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("line","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("line","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("line","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("line","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("line","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("line","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("line","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Top information.                                                   =*/

static CstrStatic line_nameF(TobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OLI_type==FIGOLI_T_Box )      return "rectangle";
    if ( self->OLI_type==FIGOLI_T_RoundBox ) return "rounded box";
    if ( self->OLI_type==FIGOLI_T_Polygon  ) return "polygon";
    return "multi-line";
}

static void     line_dump(TobjAll obj, int sh)
{
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); 
    printf("--type=%d--nbpts=%d\n",
        self->OLI_type, self->OLI_points->eleNb
    );
    obj_dump_dble2set(self->OLI_points,sh+10,__func__);
}

/*======================================================================*/
/*= Utilities                                                          =*/

static double line_length(TobjAll obj)
{
    MVARS_Self(obj);
    int i;
    double x, y;
    double length = 0.f;
    x = self->OLI_points->eles[0].x;
    y = self->OLI_points->eles[0].y;
    for (i = 1; i < self->OLI_points->eleNb; i++) {
        length += sqrt(pow(x - self->OLI_points->eles[i].x,2) + pow(y - self->OLI_points->eles[i].y,2));
        x = self->OLI_points->eles[i].x;
        y = self->OLI_points->eles[i].y;
    }
    return length;
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

static void    line_free (TobjAll obj)
{
    MVARS_Self(obj);
    dble2set_deleteS0( &self->OLI_points );
    dble2set_deleteS0( &self->OLI_drawpts );
}

//static Tobject line_dup  (Cobject obj);
static void    line_clone(TobjAll obj)
{
    MVARS_Self(obj);
    self->OLI_points  = dble2set_newClone( self->OLI_points );
    self->OLI_drawpts = 0; 
    self->O_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);
}

static TobjAll line_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
    clone = obj_line_newA(att,self->OLI_type,
                          dble2set_newClone(self->OLI_points));
    return clone;
}

static TboxDble line_boundBox(CobjAll obj)
{
    MVARS_Self(obj);
    double   width; // in object unit
    TboxDble box   = tool_boxDble_initByDble2S(self->OLI_points);
    width = self->OLI_line.stl_thickness; // in pt
    width = LUNIT_PtToFu(width) ; // in fu;
    width = lu_convToFuPpi(width,self->O_ppi);
    Box_x(box)  -= width/2;
    Box_y(box)  -= width/2;
    Box_dx(box) += width;
    Box_dy(box) += width;
    return box;
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void line_translation(TobjAll obj, double tx, double ty)           { MVARS_Self(obj); dble2set_translation(self->OLI_points, tx,ty); }
static void line_homothety00(TobjAll obj, int wtcf, double f)             { MVARS_Self(obj); dble2set_homothety00(self->OLI_points, f); }
static void line_homothetyCC(TobjAll obj, int wtcf, double f,
                                                    double cx, double cy) { MVARS_Self(obj); dble2set_homothety  (self->OLI_points, f,cx,cy); }
static void line_scale00    (TobjAll obj, int wtcf, double fx, double fy) { MVARS_Self(obj); dble2set_scale00    (self->OLI_points, fx,fy); }
static void line_scaleCC    (TobjAll obj, int wtcf, double fx, double fy,
                                                    double cx, double cy) { MVARS_Self(obj); dble2set_scale      (self->OLI_points, fx,fy, cx,cy); }
static void line_rotate     (TobjAll obj, double a, double cx, double cy) { MVARS_Self(obj); dble2set_rotate     (self->OLI_points, a, cx,cy); }
static void line_flip       (TobjAll obj, int h, int v,int r,
                                                    double cx, double cy) { MVARS_Self(obj); dble2set_flip       (self->OLI_points, h,v,r, cx,cy); }

/*======================================================================*/
/*= Drawing and filling.                                               =*/

// The arcs are approximated by a multi-line with nb points
static void arcrect_doDrawData(CobjAll   obj, int nb)
{
    int i;
    double cost[100],sint[100];
    if (nb>100) nb=100;
    MVARS_Self(obj);
    Tdble2*pts       = self->OLI_points->eles;
    double objppi    = obj_Ppi(obj); 
    double minradius = LUNIT_PtToIn( 3)*objppi;
    double radius    = LUNIT_PtToIn(20)*objppi;
    double xmin      = min2(pts[2].x,pts[0].x);
    double xmax      = max2(pts[2].x,pts[0].x);
    double ymin      = min2(pts[2].y,pts[0].y);
    double ymax      = max2(pts[2].y,pts[0].y);
    double mindxy    = min2(xmax-xmin,ymax-ymin);
    if ( radius>(mindxy/5) ) radius=mindxy/5;
    if ( radius<minradius ) {
        // no round corner a simple rectangle
        dble2set_copy(self->OLI_drawpts,self->OLI_points);
        return;
    }
    // adjust nb
    double arclen = TOOL_PI_2 * radius;
    int    nbmax  = arclen/(LUNIT_PtToIn( 1)*objppi);
    if (nb>nbmax) nb=nbmax;
    // fill in the cos and sin table
    for (i=0 ; i<nb ; i+=1) {
        double alpha = (TOOL_PI_2/nb)*i;
        cost[i] = cos(alpha);
        sint[i] = sin(alpha);
    }
    // now create the multi-line
    double cx,cy;
    cx = xmin + radius; cy = ymin + radius;
    for (i=0 ; i<nb ; i++) 
        dble2set_addXY(self->OLI_drawpts,cx-radius*cost[i],cy-radius*sint[i]);

    cx = xmax - radius; cy = ymin + radius;
    for (i=0 ; i<nb ; i++) 
        dble2set_addXY(self->OLI_drawpts,cx+radius*sint[i],cy-radius*cost[i]);

    cx = xmax - radius; cy = ymax - radius;
    for (i=0 ; i<nb ; i++) 
        dble2set_addXY(self->OLI_drawpts,cx+radius*cost[i],cy+radius*sint[i]);

    cx = xmin + radius; cy = ymax - radius;
    for (i=0 ; i<nb ; i++) 
        dble2set_addXY(self->OLI_drawpts,cx-radius*sint[i],cy+radius*cost[i]);
    dble2set_addXY(self->OLI_drawpts,  xmin , cy=ymin + radius);
}

static void line_doDrawData(CobjAll   obj)
{
    MVARS_Self(obj);
    dble2set_newORclr( &self->OLI_drawpts, 10 );
    if ( self->OLI_type==FIGOLI_T_RoundBox) {
        arcrect_doDrawData(obj,8);
    } else {
        dble2set_copy(self->OLI_drawpts,self->OLI_points);
        // do nothing when arrows are not defined.
        obj_arrow_doEndLine(self->OLI_endArrow,self->OLI_drawpts);
        obj_arrow_doBegLine(self->OLI_begArrow,self->OLI_drawpts);
    }
}


static void line_draw_sc(CobjAll   obj, Tfig_scene* sc,
                         Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);

    if ( gcf!=OD_GC_None ) {
        TEMP_ase_printf(self->OLI_fill.dpf_style==DA_FS_NoFill,
                "NoFill (0x%x) invalid fill style",
                self->OLI_fill.dpf_style);
        od_fill_mlineS(sc->sc_device,gcf,self->OLI_drawpts);
    }
    if ( gcd!=OD_GC_None ) {
        TEMP_ase_printf(self->OLI_line.dpl_style==DA_LS_None,
                "None (0x%x) invalid line style",
                self->OLI_line.dpl_style);
        od_draw_mlineS(sc->sc_device,gcd,self->OLI_drawpts);
    }
}

/*======================================================================*/
/*= Localization.                                                      =*/

static int line_markerNbF   (TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OLI_type ) {
        case FIGOLI_T_Box:      return 4;
        case FIGOLI_T_RoundBox: return 4;
        case FIGOLI_T_Polygon:  return self->OLI_points->eleNb-1;
        case FIGOLI_T_MLine:
        default:                return self->OLI_points->eleNb;
    }
}

static int line_markerNbMinF(TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OLI_type ) {
        case FIGOLI_T_Box:      return 4;
        case FIGOLI_T_RoundBox: return 4;
        case FIGOLI_T_Polygon:  return 3;
        default:                return 2; // FIGOLI_T_MLine
    }
}

static int line_markerNbCstF(TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OLI_type ) {
        case FIGOLI_T_Box:      return 1;
        case FIGOLI_T_RoundBox: return 1;
        case FIGOLI_T_Polygon:  return 0;
        default:                return 0; // FIGOLI_T_MLine
    }
}

static int line_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *shpt)
{
    MVARS_Self(obj);
    double  tolerance = d;
    int     idx;
    DBG2_E(":1:     obj=%p d=%f x/y=%f/%f",self,d,pt.x,pt.y);
    for ( idx=0 ; idx<self->OLI_points->eleNb-1 ; idx+=1) {
        Tdble2 mkr1 = self->OLI_points->eles[idx];
        Tdble2 mkr2 = self->OLI_points->eles[idx+1];
        if ( tool_closePtToSegmPt(pt,mkr1,mkr2,tolerance,shpt) ) {
            double d1 = VECTOR_Norme2PT( dble2_initVab(mkr1   ,*shpt) );
            double d2 = VECTOR_Norme2PT( dble2_initVab(mkr2   ,*shpt) );
            if ( d2<d1 ) idx=idx+1;
            DBG2_L(":1:FOUND obj=%p idx=%d mkr=%f/%f", self,idx,pt.x,pt.y);
            return idx;
        }
    }
    DBG2_L(":1:MISS  obj=%p",self);
    return -1;
}

/*======================================================================*/
/*= Modification.                                                      =*/

static Tdble2 line_mkrOpeInsert   (TobjAll obj, int idx, Cdble2*pos)
{
    MVARS_Self(obj);
    Tdble2Set* points = self->OLI_points;
    Tdble2     mkr;
    
    if      ( pos!=0 )                      mkr = *pos;
    else if ( idx==line_markerNbF(obj) )  mkr = points->eles[idx-1];
    else                                    mkr = points->eles[idx];
    switch ( self->OLI_type ) {
      case      FIGOLI_T_Polygon:
        if ( idx==0 )
            dble2set_addBeforePT(points,idx,points->eles[0]);
        else if ( idx>=(points->eleNb-1) )
            dble2set_addPT(points,points->eles[0]);
        else
            dble2set_addBeforePT(points,idx,mkr);
        self->O_tags &= ~(FIG_TAG_DrawingDataReady);
        self->O_boundBox = tool_boxDble_initByDble2S(self->OLI_points);
        break;
      case      FIGOLI_T_MLine:
        if ( idx==points->eleNb )
            dble2set_addPT(points,mkr);
        else
            dble2set_addBeforePT(points,idx,mkr);
        self->O_tags &= ~(FIG_TAG_DrawingDataReady);
        self->O_tags &= ~(FIG_TAG_BBoxUpToDate);
        break;
      default:
        break;
    }
    return mkr;
}

static void   line_mkrOpeChange   (TobjAll obj, int idx, Tdble2 pt)
{
    MVARS_Self(obj);
    switch ( self->OLI_type ) {
        case      FIGOLI_T_Box:
        case      FIGOLI_T_RoundBox:
            switch ( idx ) {
              case 0:
                self->OLI_points->eles[0]   = pt;
                self->OLI_points->eles[1].y = pt.y;
                self->OLI_points->eles[3].x = pt.x;
                break;
              case 1:
                self->OLI_points->eles[0].y   = pt.y;
                self->OLI_points->eles[1]     = pt;
                self->OLI_points->eles[2].x   = pt.x;
                break;
              case 2:
                self->OLI_points->eles[1].x = pt.x;
                self->OLI_points->eles[2]   = pt;
                self->OLI_points->eles[3].y = pt.y;
                break;
              case 3:
                self->OLI_points->eles[0].x   = pt.x;
                self->OLI_points->eles[2].y   = pt.y;
                self->OLI_points->eles[3]     = pt;
                break;
            }
            self->OLI_points->eles[4]   = self->OLI_points->eles[0];
            self->O_tags &= ~(FIG_TAG_DrawingDataReady);
            self->O_boundBox = tool_boxDble_initByDble2S(self->OLI_points);
            break;
        case      FIGOLI_T_Polygon: 
            self->OLI_points->eles[idx] = pt;
            if (idx==0)
                self->OLI_points->eles[self->OLI_points->eleNb-1] = self->OLI_points->eles[0];
            self->O_tags &= ~(FIG_TAG_DrawingDataReady);
            self->O_boundBox = tool_boxDble_initByDble2S(self->OLI_points);
            break;
        default://FIGOLI_T_MLine
            self->OLI_points->eles[idx] = pt;
            self->O_tags &= ~(FIG_TAG_DrawingDataReady);
            self->O_tags &= ~(FIG_TAG_BBoxUpToDate);
            break;
    }
}

static void line_mkrOpeDelete   (TobjAll obj, int idx)
{
    MVARS_Self(obj);
    switch ( self->OLI_type ) {
        case FIGOLI_T_Box:
        case FIGOLI_T_RoundBox:
            TEMP_spe_printf("invalid call for %s object (ignored)",obj_Name(obj));
            break;
        case FIGOLI_T_Polygon: 
            dble2set_supShift(self->OLI_points,idx);
            if (idx==0)
              self->OLI_points->eles[self->OLI_points->eleNb-1]=self->OLI_points->eles[0];
            self->O_tags &= ~(FIG_TAG_DrawingDataReady);
            self->O_boundBox = tool_boxDble_initByDble2S(self->OLI_points);
            break;
        default://FIGOLI_T_MLine
            dble2set_supShift(self->OLI_points,idx);
            self->O_tags &= ~(FIG_TAG_DrawingDataReady);
            self->O_tags &= ~(FIG_TAG_BBoxUpToDate);
            break;
    }
}

static int line_mkrOpeEnabled  (TobjAll obj)
{
    MVARS_Self(obj);
    int ret = FIGO_MkrOpe_Chg;
    switch ( self->OLI_type ) {
        case FIGOLI_T_Box:      break;
        case FIGOLI_T_RoundBox: break;
        case FIGOLI_T_Polygon:
        case FIGOLI_T_MLine: 
            ret |= FIGO_MkrOpe_Ins;
            if ( self->OLI_points->eleNb > line_markerNbMinF(obj) ) ret |= FIGO_MkrOpe_Del;
            break;
        default:                break;
    }
    return ret;
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable line_vtable = {
    .vt_kind          = FIG_KD_Line,
    .vt_bytesz        = sizeof( TobjLine ),
    .vt_mask          = FIG_KDM_LIN,
    .vt_subKind       = offsetof(TobjLine,OLI_type),
//  .vt_fdc           = offsetof(TobjLine,OLI_curve),
    .vt_fdl           = offsetof(TobjLine,OLI_line),
    .vt_fdf           = offsetof(TobjLine,OLI_fill),
    .vt_begArrow      = offsetof(TobjLine,OLI_begArrow),
    .vt_endArrow      = offsetof(TobjLine,OLI_endArrow),
    .vt_name          = "line",
    .vt_nameF         = line_nameF,
    .vt_dump          = line_dump,
    .vt_length        = line_length,

    .vt_free          = line_free,
//  .vt_dup           = line_dup,
    .vt_clone         = line_clone,
    .vt_dupNDP        = line_dupNDP,
    .vt_boundBox      = line_boundBox,

    .vt_translation   = line_translation,
    .vt_homothety00   = line_homothety00,
    .vt_homothetyCC   = line_homothetyCC,
    .vt_scale00       = line_scale00,
    .vt_scaleCC       = line_scaleCC,
    .vt_rotate        = line_rotate,
    .vt_flip          = line_flip,
    
    .vt_doDrawData    = line_doDrawData,
    .vt_draw_sc       = line_draw_sc,

    .vt_markerNbF     = line_markerNbF,
    .vt_markerNbMinF  = line_markerNbMinF,
    .vt_markerNbCstF  = line_markerNbCstF,
    .vt_markerS       = offsetof(TobjLine,OLI_points),
    .vt_closeto       = line_closeto,

    .vt_mkrOpeInsert  = line_mkrOpeInsert,
    .vt_mkrOpeChange  = line_mkrOpeChange,
    .vt_mkrOpeDelete  = line_mkrOpeDelete,
    .vt_mkrOpeEnabled = line_mkrOpeEnabled,
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void obj_line_init( TobjLine* self, int type, Tdble2Set* pts)
{
    PERM_asf_printf(pts->eleNb<2,
        "Multi-line with %d points, at least 2 is required",pts->eleNb);
    self->OLI_type   = type;
    self->OLI_points = pts;

    // If it is needed:
    //    - set the FIG_TAG_CanHaveArrow flag
    //    - close the multi-line
    switch (type) {
      case FIGOLI_T_MLine:
        self->O_tags |= FIG_TAG_CanHaveArrow;
        break;
      case FIGOLI_T_Box:
      case FIGOLI_T_RoundBox:
        if ( self->OLI_points->eleNb == 5 ) {
          Tdble2* first = dble2set_first(self->OLI_points);
          Tdble2* last  = dble2set_last (self->OLI_points);
          TEMP_asf_printf( dble2_cmpPT(first,last), "%s","invalid rectangle points (last!=first).");
        } else if ( self->OLI_points->eleNb == 4 ) {
          Tdble2* first = dble2set_first(self->OLI_points);
          dble2set_addPT(self->OLI_points,*first);
        } else {
          TEMP_spf_printf( "invalid rectangle points (%d points instead of 4 or 5).",self->OLI_points->eleNb);
        }
        break;
      case FIGOLI_T_Polygon: {
          Tdble2* first = dble2set_first(self->OLI_points);
          Tdble2* last  = dble2set_last (self->OLI_points);
          if ( dble2_cmpPT(first,last)!=0 )
            dble2set_addPT(self->OLI_points,*first);
        }
        break;
      default:
        PERM_spe_printf("%d is an unexpected kind.",type);
        PERM_spf_printf("recognized are: "
                "MLine=%d Box=%d Polygon=%d RoundBox=%d",
                FIGOLI_T_MLine,   FIGOLI_T_Box,
                FIGOLI_T_Polygon,  FIGOLI_T_RoundBox);
    }
    
    // Waiting for update vt_boundBox and/or vt_doDrawData for
    // updating dynamic data
    // Notice that the FIG_TAG_BBoxUpToDate/DrawingDisabled tags
    // are not set.
}

extern TobjAll obj_line_newF(TobjVTT vtt,
     int depth, Csty_line* stl, Csty_fill* stf,
     int type, Tdble2Set* pts)
{
    TobjAll self;
    self=obj_newByODS(FIG_KD_Line,vtt,depth,stl,stf,NULL);
    obj_line_init(self.line,type,pts);
    return self;
}

extern TobjAll obj_line_newA(Cfdp* fdp, int type, Tdble2Set* pts)
{
    TobjAll self;
    self=obj_newByFdp(FIG_KD_Line,fdp);
    obj_line_init(self.line,type,pts);
    return self;
}

extern TobjAll    obj_sqrrect_newAPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2)
{
    TboxDble box = tool_boxdble_initPT(pt1,pt2);
    Tdble2Set* pts = dble2set_new(5);
    dble2set_addPT(pts,tool_boxdble_pt1(&box));
    dble2set_addPT(pts,tool_boxdble_pt2(&box));
    dble2set_addPT(pts,tool_boxdble_pt3(&box));
    dble2set_addPT(pts,tool_boxdble_pt4(&box));
    return obj_line_newA(fdp, FIGOLI_T_Box, pts);
}

extern TobjAll    obj_sqrrect_newAXYWH(Cfdp* fdp, double x,  double y,
                                                  double dx, double dy)
{ Tdble2 pt1=dble2_init(x,y),pt2=dble2_init(x+dx,y+dy);
  return obj_sqrrect_newAPTPT(fdp,pt1,pt2); }

extern TobjAll    obj_arcrect_newAPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2)
{
    TboxDble box = tool_boxdble_initPT(pt1,pt2);
    Tdble2Set* pts = dble2set_new(5);
    dble2set_addPT(pts,tool_boxdble_pt1(&box));
    dble2set_addPT(pts,tool_boxdble_pt2(&box));
    dble2set_addPT(pts,tool_boxdble_pt3(&box));
    dble2set_addPT(pts,tool_boxdble_pt4(&box));
    return obj_line_newA(fdp, FIGOLI_T_RoundBox, pts);
}

extern TobjAll    obj_arcrect_newAXYWH(Cfdp* fdp, double x,  double y,
                                                  double dx, double dy)
{ Tdble2 pt1=dble2_init(x,y),pt2=dble2_init(x+dx,y+dy);
  return obj_arcrect_newAPTPT(fdp,pt1,pt2); }

/*======================================================================*/
