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
 *      $Id: obj-arc.c 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for arcs (only circle arc, not elliptic).                   =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjArc type.                         =*/
/*=   - the constructors of the TobjArc type.                          =*/
/*= See obj-arc.h header for data structure of object.                 =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/obj-arc.h"
#include "fig/object-priv.h"
#include "fig/fig-scene.h"   // used for drawing it
#include "outdev/ldaf-hl.h"  // used for drawing it

#include "tools/color.h"
#include "tools/geo2d.h"

/*======================================================================*/

#define MVARS_Self(obj)    TobjArc *self = obj.arc

#define DBG0_E(fmt,...) DBG0_printf("arc","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("arc","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("arc","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("arc","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("arc","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("arc","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("arc","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("arc","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("arc","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Top information.                                                   =*/

static CstrStatic arc_nameF(TobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OAR_type==FIGOAR_T_PiePiece ) return "pie piece";
    return "arc";
}

static void arc_dump(TobjAll obj, int sh)
{
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); printf("\n");
    char tmp[100]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh+10);
    printf("%20s:%stype=%d--pts=(%f,%f) (%f %f) (%f %f) center=%f,%f",
        __func__,tmp,self->OAR_type,
        self->OAR_points[0].x, self->OAR_points[0].y,
        self->OAR_points[1].x, self->OAR_points[1].y,
        self->OAR_points[2].x, self->OAR_points[2].y,
        self->OAR_center   .x, self->OAR_center   .y);
    printf("--dir=%d\n",self->oar_direction);
}

/*======================================================================*/
/*= Utilities                                                          =*/

static double arc_angle(TobjAll obj)
{
    MVARS_Self(obj);
    double ax, ay, bx, by, cx, cy, a, b, c;
    ax = self->oar_points[0].x;
    ay = self->oar_points[0].y;
    bx = self->oar_points[2].x;
    by = self->oar_points[2].y;
    cx = self->oar_center.x;
    cy = self->oar_center.y;

    a = sqrt(pow(ax - bx,2) + pow(ay - by,2));
    b = sqrt(pow(bx - cx,2) + pow(by - cy,2));
    c = sqrt(pow(cx - ax,2) + pow(cy - ay,2));

    if (b < 0.000001 || c < 0.000001) return 0.f;
    return acos((pow(c, 2) + pow(b, 2) - pow(a, 2))/ (2 * b * c));
}

static double arc_length(TobjAll obj)
{
    MVARS_Self(obj);
    double length, radius, ax, ay, cx, cy;
    ax = self->oar_points[0].x;
    ay = self->oar_points[0].y;
    cx = self->oar_center.x;
    cy = self->oar_center.y;
    radius = sqrt(pow(ax - cx,2) + pow(ay - cy,2));

    length =  radius * arc_angle(obj);
    return length;
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

//static void    arc_free (Tobject obj);
//static Tobject arc_dup  (Cobject obj);
//static void    arc_clone(Tobject obj);

static TobjAll arc_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
    clone = obj_arc_newA(att,self->OAR_type, self->OAR_points);
    return clone;
}

static TboxDble arc_boundBox(CobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OAR_type==FIGOAR_T_PiePiece )
        return tool_boxDble_initByDble2T(self->OAR_points,4);
    else
        return tool_boxDble_initByDble2T(self->OAR_points,3);
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void arc_translation(TobjAll obj, double tx, double ty)
{
    MVARS_Self(obj);

    TRANSLATE_PXY(self->OAR_points[0]   ,tx,ty);
    TRANSLATE_PXY(self->OAR_points[1]   ,tx,ty);
    TRANSLATE_PXY(self->OAR_points[2]   ,tx,ty);
    TRANSLATE_PXY(self->OAR_center      ,tx,ty);
}

static void arc_homothety00(TobjAll obj, int wtcf, double f)
{
    MVARS_Self(obj);
    SCALE00_VXY(self->OAR_points[0] , f,f);
    SCALE00_VXY(self->OAR_points[1] , f,f);
    SCALE00_VXY(self->OAR_points[2] , f,f);
    SCALE00_VXY(self->OAR_center    , f,f);
}

static void arc_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{
    MVARS_Self(obj);
    SCALECC_PXY(self->OAR_points[0] , f,f,cx,cy);
    SCALECC_PXY(self->OAR_points[1] , f,f,cx,cy);
    SCALECC_PXY(self->OAR_points[2] , f,f,cx,cy);
    SCALECC_PXY(self->OAR_center    , f,f,cx,cy);
}

static void arc_scale00(TobjAll obj, int wtcf, double fx, double fy)
{
    MVARS_Self(obj);
    SCALE00_VXY(self->OAR_points[0] , fx,fy);
    SCALE00_VXY(self->OAR_points[1] , fx,fy);
    SCALE00_VXY(self->OAR_points[2] , fx,fy);
    if ( fx==fy ) {
        SCALE00_VXY(self->OAR_center, fx,fy);
    } else {
        int ret= tool_arcCenterOf3Points_PTD( &self->OAR_center,
            self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
        PERM_asf_printf(ret==0,"Can not scale arc; points are aligned",0);
        self->OAR_direction  = tool_are3PointsClockwise_PTD(
            self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
    }
}

static void arc_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{
    MVARS_Self(obj);
    SCALECC_PXY(self->OAR_points[0] , fx,fy,cx,cy);
    SCALECC_PXY(self->OAR_points[1] , fx,fy,cx,cy);
    SCALECC_PXY(self->OAR_points[2] , fx,fy,cx,cy);
    if ( fx==fy) {
        SCALECC_PXY(self->OAR_center, fx,fy,cx,cy);
    } else {
        int ret= tool_arcCenterOf3Points_PTD( &self->OAR_center,
            self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
        PERM_asf_printf(ret==0,"Can not scale arc; points are aligned",0);
        self->OAR_direction  = tool_are3PointsClockwise_PTD(
            self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
    }
}

static void arc_rotate(TobjAll obj, double a, double cx, double cy)
{
    MVARS_Self(obj);
    ROTCC_PAXY(self->OAR_points[0] , a, cx,cy);
    ROTCC_PAXY(self->OAR_points[1] , a, cx,cy);
    ROTCC_PAXY(self->OAR_points[2] , a, cx,cy);
    ROTCC_PAXY(self->OAR_points[3] , a, cx,cy);
}

static void arc_flip(TobjAll obj, int h, int v, int r, double cx, double cy)
{
    MVARS_Self(obj);
    Tdble2* points = self->OAR_points;
    FLIP_PHVRXY(points[0] , h,v,r, cx,cy);
    FLIP_PHVRXY(points[1] , h,v,r, cx,cy);
    FLIP_PHVRXY(points[2] , h,v,r, cx,cy);
    FLIP_PHVRXY(points[3] , h,v,r, cx,cy);
    self->OAR_direction  = tool_are3PointsClockwise_PTD(points+0,points+1,points+2);
}

/*======================================================================*/
/*= Drawing and filling.                                               =*/

static void arc_doDrawData(CobjAll obj)
{
    MVARS_Self(obj);

    memcpy(self->OAR_drawpts,self->OAR_points,sizeof(self->OAR_points));

    obj_arrow_doEndArc(self->OAR_endArrow,self->OAR_drawpts,self->OAR_direction);
    obj_arrow_doBegArc(self->OAR_begArrow,self->OAR_drawpts,self->OAR_direction);
}

static void arc_draw_sc(CobjAll   obj, Tfig_scene* sc,
                         Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);

    TellArc e;
    e.center =  self->OAR_center;
    if ( self->OAR_direction==0 ) {
        e.p1 = self->OAR_points[2];
        e.p2 = self->OAR_points[0];
    } else {
        e.p1 = self->OAR_points[0];
        e.p2 = self->OAR_points[2];
    }
    e.ra     =  VECTOR_Norme( self->OAR_points[0].x - self->OAR_center.x,
                              self->OAR_points[0].y - self->OAR_center.y);
    e.rb     = e.ra;
    e.pop    = (self->OAR_type&FIGOAR_T_PiePiece)!=0 ;
    if ( gcf!=OD_GC_None ) {
        TEMP_ase_printf(self->OAR_fill.dpf_style==DA_FS_NoFill,
                "NoFill (0x%x) invalid fill style",
                self->OAR_fill.dpf_style);
        od_fill_ellarc(sc->sc_device,gcf,&e);
    }
    if ( gcd!=OD_GC_None ) {
        TEMP_ase_printf(self->OAR_line.dpl_style==DA_LS_None,
                "None (0x%x) invalid line style",
                self->OAR_line.dpl_style);
        od_draw_ellarc(sc->sc_device,gcd,&e);
    }
}

/*======================================================================*/
/*= Arrow handler.                                                     =*/

/*======================================================================*/
/*= Localization.                                                      =*/

static int arc_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *shpt)
{
    int i;
    MVARS_Self(obj);
    DBG2_E(":1:      obj=%p d=%f x/y=%f/%f",self,d,pt.x,pt.y);
    Cdble2*     points = self->OAR_points;
    Tdble2      center = self->OAR_center;
    double      tolerance = d;

    // try the radius of the pie pieces
    if ( (self->OAR_type&FIGOAR_T_PiePiece)!=0 && (
          tool_closePtToSegmPt(pt,center,points[0],tolerance,shpt) ||
          tool_closePtToSegmPt(pt,center,points[2],tolerance,shpt)) ) {
        double dc = VECTOR_Norme2PT( dble2_initVab(center   ,*shpt) );
        double d0 = VECTOR_Norme2PT( dble2_initVab(points[0],*shpt) );
        double d2 = VECTOR_Norme2PT( dble2_initVab(points[2],*shpt) );
        int index = dc<d0 && dc<d2  ? 3
                  : d0<dc && d0<d2  ? 0 : 2;
        DBG2_L(":1:FOUND obj=%p d=%f x/y=%f/%f index=%d shx/y=%f/%f (on radius)",self,d,pt.x,pt.y,index,shpt->x,shpt->y);
        return index;
    }

    // try the arc; we get shpt such as center, PT & SHPT are aligned
    //     C ---------------PT-------------SHPT
    //     C -----SHPT----------PT
    double dx  = pt.x - self->OAR_center.x;
    double dy  = pt.y - self->OAR_center.y;
    double rad = VECTOR_NormePT( dble2_initVab(center ,points[0]) );
    double dis = VECTOR_NormeXY(dx,dy); // dis from pt to center
    if (  dis>(rad+tolerance) ||  // too far  of center
          dis<(rad-tolerance) ) { // too near of center
        DBG2_L(":1:MISS  obj=%p d=%f x/y=%f/%f (1)", self,d,pt.x,pt.y);
        return -1;
    }
    shpt->x = rad * dx / dis + self->OAR_center.x;
    shpt->y = rad * dy / dis + self->OAR_center.y;
    // now check the position of shpt on the arc.
    Tdble2 vc0  = dble2_initVab(center ,points[0]);
    Tdble2 vc1  = dble2_initVab(center ,points[1]);
    Tdble2 vc2  = dble2_initVab(center ,points[2]);
    Tdble2 vcsh = dble2_initVab(center ,*shpt    );
    double a0sh = tool_angleOfVectors_PTD( &vc0 , &vcsh );
    double a02  = tool_angleOfVectors_PTD( &vc0 , &vc2  );
    double ash1 = tool_angleOfVectors_PTD( &vcsh, &vc1  );
    double ash2 = tool_angleOfVectors_PTD( &vcsh, &vc2  );
    if ( (self->OAR_direction!=0 && a0sh>=a02) ||
         (self->OAR_direction==0 && a0sh<=a02) ) {
        // on the arc
        int index = abs(a0sh)<abs(ash1) && abs(a0sh)<abs(ash2)  ? 0
                  : abs(ash1)<abs(a0sh) && abs(ash1)<abs(ash2)  ? 1 : 2;
        DBG2_L(":1:FOUND obj=%p d=%f x/y=%f/%f index=%d shx/y=%f/%f",self,d,pt.x,pt.y,index,shpt->x,shpt->y);
        return index;
    } else {
        DBG2_L(":1:MISS  obj=%p d=%f x/y=%f/%f (2)", self,d,pt.x,pt.y);
        return -1;
    }
}

/*======================================================================*/
/*= Modification.                                                      =*/

static void arc_mkrOpeChange(TobjAll obj, int i, Tdble2 pt)
{
    MVARS_Self(obj);
    Tdble2*     points = self->OAR_points;
    Tdble2      center_save = self->OAR_center;
    Tdble2      chgpt_save  = points[i];

    points[i] = pt;
    int ret= tool_arcCenterOf3Points_PTD(&self->OAR_center,points+0,points+1,points+2);
    if ( ret==0 ) {
        // points are aligned, we abort the change by restauring initial arc
        self->OAR_center = center_save;
        points[i] = chgpt_save;
        return;
    }
    self->OAR_direction  = tool_are3PointsClockwise_PTD(points+0,points+1,points+2);
    self->O_tags &= ~FIG_TAG_BBoxUpToDate;
    self->O_tags &= ~FIG_TAG_DrawingDataReady;
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable arc_vtable = {
    .vt_kind          = FIG_KD_Arc,
    .vt_bytesz        = sizeof( TobjArc ),
    .vt_mask          = FIG_KDM_ARC,
    .vt_subKind       = offsetof(TobjArc,OAR_type),
//  .vt_fdc           = offsetof(TobjArc,OAR_curve),
    .vt_fdl           = offsetof(TobjArc,OAR_line),
    .vt_fdf           = offsetof(TobjArc,OAR_fill),
    .vt_begArrow      = offsetof(TobjArc,OAR_begArrow),
    .vt_endArrow      = offsetof(TobjArc,OAR_endArrow),
    .vt_name          = "arc",
    .vt_nameF         = arc_nameF,
    .vt_dump          = arc_dump,
    .vt_length        = arc_length,

//  .vt_free          = arc_free,
//  .vt_dup           = arc_dup,
//  .vt_clone         = arc_clone,
    .vt_dupNDP        = arc_dupNDP,
    .vt_boundBox      = arc_boundBox,

    .vt_translation   = arc_translation,
    .vt_homothety00   = arc_homothety00,
    .vt_homothetyCC   = arc_homothetyCC,
    .vt_scale00       = arc_scale00,
    .vt_scaleCC       = arc_scaleCC,
    .vt_rotate        = arc_rotate,
    .vt_flip          = arc_flip,

    .vt_doDrawData    = arc_doDrawData,
    .vt_draw_sc       = arc_draw_sc,

    .vt_markerNb      = 4,
    .vt_markerT       = offsetof(TobjArc,OAR_points[0]),
    .vt_centerMkr     = 1,
    .vt_centerMkrIdx  = 3,
    .vt_closeto       = arc_closeto,

    .vt_mkrOpeChange  = arc_mkrOpeChange
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void obj_arc_init3( TobjArc* self, int type, Tdble2 pt1, Tdble2 pt2, Tdble2 pt3)
{
    self->OAR_type = type==FIGOAR_T_PiePiece ? FIGOAR_T_PiePiece : FIGOAR_T_Open;
    self->OAR_line.dpl_join  = DA_JS_Bevel;
    self->OAR_points[0]  = pt1;
    self->OAR_points[1]  = pt2;
    self->OAR_points[2]  = pt3;
    int ret= tool_arcCenterOf3Points_PTD( &self->OAR_center,
        self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
    PERM_asf_printf(ret==0,"%s","Can not create arc; points are aligned");
    self->OAR_direction  = tool_are3PointsClockwise_PTD(
        self->OAR_points+0,self->OAR_points+1,self->OAR_points+2);
    if ( self->OAR_type==FIGOAR_T_Open )
        self->O_tags |= FIG_TAG_CanHaveArrow;
}

static void obj_arc_init( TobjArc* self, int type, Cdble2*pts)
{ obj_arc_init3(self,type,pts[0],pts[1],pts[2]); }


extern TobjAll obj_arc_newF(TobjVTT vtt, 
                        int depth, Csty_line* stl, Csty_fill* stf,
                        int type, Tdble2*pts)
{
    TobjAll self = obj_newByODS(FIG_KD_Arc,vtt,depth,stl,stf,NULL);
    obj_arc_init(self.arc,type,pts);
    return self;
}

extern TobjAll obj_arc_newA (Cfdp* att, int type, Cdble2*pts)
{
    TobjAll self = obj_newByFdp(FIG_KD_Arc,att);
    obj_arc_init(self.arc,type,pts);
    return self;
}

extern TobjAll obj_arc_newA3(Cfdp* att, int type,
                             Cdble2 spt, Cdble2 mpt, Cdble2 ept)
{
    TobjAll self = obj_newByFdp(FIG_KD_Arc,att);
    obj_arc_init3(self.arc,type,spt,mpt,ept);
    return self;
}

/*======================================================================*/
