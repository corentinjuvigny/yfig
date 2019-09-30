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
# $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-ellipse.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-ellipse.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for circle and ellipse.                                     =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjEllipse type.                     =*/
/*=   - the constructors of the TobjEllipse type.                      =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/obj-ellipse.h"
#include "fig/object-priv.h"
#include "tools/color.h"
#include "fig/fig-scene.h"   // used for drawing it
#include "outdev/ldaf-hl.h"  // used for drawing it

/*======================================================================*/

#define MVARS_Self(obj)  TobjEllipse *self = obj.ell

#define ZOOM_FACTOR 1.0  // FIXME (((double)PIX_PER_INCH)/((double)DISPLAY_PIX_PER_INCH))

#define DBG0_E(fmt,...) DBG0_printf("ellipse","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("ellipse","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("ellipse","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("ellipse","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("ellipse","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("ellipse","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("ellipse","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("ellipse","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("ellipse","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Top information.                                                   =*/

static CstrStatic ellipse_nameF(TobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OEL_type==FIGOELL_T_CirByRad ||
         self->OEL_type==FIGOELL_T_CirByDia ) return "circle";
    return "ellipse";
}

static void     ellipse_dump(TobjAll obj, int sh)
{
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); printf("\n");
    char tmp[100]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh+10);
    printf("%20s:%stype=%d--center=%f,%f--radiuses=%f,%f--angle=%f\n",
        __func__,tmp,
        self->OEL_type,
        self->OEL_center.x,   self->OEL_center.y,
        self->OEL_radiuses.x, self->OEL_radiuses.y,
        self->OEL_angle);
}

/*======================================================================*/
/*= Utilities                                                          =*/

static double ellipse_length(TobjAll obj)
{
    MVARS_Self(obj);
    double a, b;

    a = self->oel_radiuses.x;
    b = self->oel_radiuses.y;
    
    return M_PI * sqrt(2 * (pow(a, 2) + pow(b, 2)));
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

//static void    vt_free (Tobject obj);
//static Tobject vt_dup  (Cobject obj);
//static void    vt_clone(Tobject obj);

static TobjAll ellipse_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
    Tdble2 start = { self->OEL_start.x, self->OEL_start.y };
    Tdble2 end   = { self->OEL_end.x  , self->OEL_end.y };
    clone = obj_ellipse_newA(att,self->OEL_type,
                             start,end,self->OEL_angle);
    return clone;
}

static TboxDble ellipse_boundBox(CobjAll obj)
{
    MVARS_Self(obj);

    TboxDble bb;
    double xmin, ymin, xmax, ymax;
    int        half_wd;
    double     c1, c2, c3, c4, c5, c6, v1, cphi, sphi, cphisqr, sphisqr;
    double     xleft, xright, d, asqr, bsqr;
    int        yymax, yy=0;
    float        xcen, ycen, a, b;

    xcen = self->OEL_center.x;
    ycen = self->OEL_center.y;
    a = self->OEL_radiuses.x;
    b = self->OEL_radiuses.y;
    if (a==0 || b==0) {
        xmin = xmax = xcen;
        ymin = ymax = ycen;
        goto the_end;
    }

    /* don't adjust by line thickness if = 1 */
    if (self->OEL_thickness == 1)
        half_wd = 0;
    else
        half_wd = self->OEL_thickness / 2.0 * ZOOM_FACTOR;

    /* angle of 0 is easy */
    if (self->OEL_angle == 0) {
        xmin = xcen - a - half_wd;
        xmax = xcen + a + half_wd;
        ymin = ycen - b - half_wd;
        ymax = ycen + b + half_wd;
        goto the_end;
    }

    /* divide by ZOOM_FACTOR because we don't need such precision */
    a /= ZOOM_FACTOR;
    b /= ZOOM_FACTOR;
    xcen /= ZOOM_FACTOR;
    ycen /= ZOOM_FACTOR;

    cphi = cos((double)self->OEL_angle);
    sphi = sin((double)self->OEL_angle);
    cphisqr = cphi*cphi;
    sphisqr = sphi*sphi;
    asqr = a*a;
    bsqr = b*b;
    
    c1 = (cphisqr/asqr)+(sphisqr/bsqr);
    c2 = ((cphi*sphi/asqr)-(cphi*sphi/bsqr))/c1;
    c3 = (bsqr*cphisqr) + (asqr*sphisqr);
    yymax = sqrt(c3);
    c4 = a*b/c3;
    c5 = 0;
    v1 = c4*c4;
    c6 = 2*v1;
    c3 = c3*v1-v1;
    /* odd first points */
    xmin = ymin =  10000000;
    xmax = ymax = -10000000;
    if (yymax % 2) {
        d = sqrt(c3);
        xmin = min2(xmin,xcen-d);
        xmax = max2(xmax,xcen+d);
        ymin = min2(ymin,ycen);
        ymax = max2(ymax,ycen);
        c5 = c2;
        yy=1;
    }
    while (c3>=0) {
        d = sqrt(c3);
        xleft = c5-d;
        xright = c5+d;
        xmin = min2(xmin,xcen+xleft);
        xmax = max2(xmax,xcen+xleft);
        ymax = max2(ymax,ycen+yy);
        xmin = min2(xmin,xcen+xright);
        xmax = max2(xmax,xcen+xright);
        ymax = max2(ymax,ycen+yy);
        xmin = min2(xmin,xcen-xright);
        xmax = max2(xmax,xcen-xright);
        ymin = min2(ymin,ycen-yy);
        xmin = min2(xmin,xcen-xleft);
        xmax = max2(xmax,xcen-xleft);
        ymin = min2(ymin,ycen-yy);
        c5+=c2;
        v1+=c6;
        c3-=v1;
        yy=yy+1;
    }
    /* for simplicity, just add half the line thickness to xmax and ymax
       and subtract half from xmin and ymin */
    xmin = xmin * ZOOM_FACTOR + half_wd;
    ymin = ymin * ZOOM_FACTOR + half_wd;
    xmax = xmax * ZOOM_FACTOR + half_wd;
    ymax = ymax * ZOOM_FACTOR + half_wd;
the_end:
   Box_x(bb) = xmin;      Box_y(bb)=ymin;
   Box_dx(bb)=xmax-xmin;  Box_dy(bb)=ymax-ymin;
   return bb;
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void ellipse_translation(TobjAll obj, double tx, double ty)
{
    TobjEllipse* self = obj.ell;
    TRANSLATE_PXY(self->OEL_center   ,tx,ty);
    TRANSLATE_PXY(self->OEL_start    ,tx,ty);
    TRANSLATE_PXY(self->OEL_end      ,tx,ty);
}

// static void vt_homothety00(Tobject obj, double f);

static void ellipse_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{
    TobjEllipse* self = obj.ell;
    SCALECC_PXY(self->OEL_center   ,f,f,cx,cy);
    SCALECC_PXY(self->OEL_start    ,f,f,cx,cy);
    SCALECC_PXY(self->OEL_end      ,f,f,cx,cy);
    SCALE00_VXY(self->OEL_radiuses ,f,f);
}

static void ellipse_scale00(TobjAll obj, int wtcf, double fx, double fy)
{
    TobjEllipse* self = obj.ell;
    Tdble2 tmp;
    SCALE00_VXY(self->OEL_center   ,fx,fy);
    SCALE00_VXY(self->OEL_start    ,fx,fy);
    SCALE00_VXY(self->OEL_end      ,fx,fy);
    // the a axis
    tmp.x = cos(self->OEL_angle)*self->OEL_radiuses.x;
    tmp.y = sin(self->OEL_angle)*self->OEL_radiuses.x;
    SCALE00_VXY(tmp ,fx,fy);
    self->OEL_radiuses.x = sqrt(tmp.x*tmp.x+tmp.y*tmp.y);
    // the b axis
    tmp.x = cos(self->OEL_angle+TOOL_PI_2)*self->OEL_radiuses.y;
    tmp.y = sin(self->OEL_angle+TOOL_PI_2)*self->OEL_radiuses.y;
    SCALE00_VXY(tmp ,fx,fy);
    self->OEL_radiuses.y = sqrt(tmp.x*tmp.x+tmp.y*tmp.y);
}

static void ellipse_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{
    Tdble2 tmp;
    TobjEllipse* self = obj.ell;
    SCALECC_PXY(self->OEL_center   ,fx,fy,cx,cy);
    SCALECC_PXY(self->OEL_start    ,fx,fy,cx,cy);
    SCALECC_PXY(self->OEL_end      ,fx,fy,cx,cy);
    // the a axis
    tmp.x = cos(self->OEL_angle)*self->OEL_radiuses.x;
    tmp.y = sin(self->OEL_angle)*self->OEL_radiuses.x;
    SCALE00_VXY(tmp ,fx,fy);
    self->OEL_radiuses.x = sqrt(tmp.x*tmp.x+tmp.y*tmp.y);
    // the b axis
    tmp.x = cos(self->OEL_angle+TOOL_PI_2)*self->OEL_radiuses.y;
    tmp.y = sin(self->OEL_angle+TOOL_PI_2)*self->OEL_radiuses.y;
    SCALE00_VXY(tmp ,fx,fy);
    self->OEL_radiuses.y = sqrt(tmp.x*tmp.x+tmp.y*tmp.y);
}

static void ellipse_rotate(TobjAll obj, double a, double cx, double cy)
{
    MVARS_Self(obj);
    ROTCC_PAXY(self->OEL_center , a, cx,cy);
    ROTCC_PAXY(self->OEL_start  , a, cx,cy);
    ROTCC_PAXY(self->OEL_end    , a, cx,cy);
    self->OEL_angle += a;
}

static void ellipse_flip( TobjAll obj, int h, int v, int r, double cx, double cy)
{
    MVARS_Self(obj);
    FLIP_PHVRXY(self->OEL_center , h,v,r, cx,cy);
    FLIP_PHVRXY(self->OEL_start  , h,v,r, cx,cy);
    FLIP_PHVRXY(self->OEL_end    , h,v,r, cx,cy);
    if ( h ) self->OEL_angle  = TOOL_PI - self->OEL_angle;
    if ( v ) self->OEL_angle  = -self->OEL_angle;
    self->OEL_angle += r*TOOL_PI_2;
}

/*======================================================================*/
/*= Drawing and filling.                                               =*/

static void ellipse_draw_sc(CobjAll   obj, Tfig_scene* sc,
                         Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);

    Tellipse e;
    e.center.x =  self->OEL_center.x;
    e.center.y =  self->OEL_center.y;
    e.rx       =  self->OEL_radiuses.x;
    e.ry       =  self->OEL_radiuses.y;
    e.angle    =  self->OEL_angle;

    if ( gcf!=OD_GC_None ) {
        TEMP_ase_printf(self->OEL_fill.dpf_style==DA_FS_NoFill,
                "NoFill (0x%x) invalid fill style",
                self->OEL_fill.dpf_style);
        od_fill_ellipse(sc->sc_device,gcf,&e);
    }
    if ( gcd!=OD_GC_None ) {
        TEMP_ase_printf(self->OEL_style==DA_LS_None,
                "None (0x%x) invalid line style",
                self->OEL_style);
        od_draw_ellipse(sc->sc_device,gcd,&e);
    }
}

/*======================================================================*/
/*= Localization.                                                      =*/

static int    ellipse_markerNbF(TobjAll obj)
{
    MVARS_Self(obj);
    switch (self->OEL_type) {
        case FIGOELL_T_EllByRad: return 2;
        case FIGOELL_T_CirByRad: return 2;
        default:                 return 3;
    }
}

static Cdble2 ellipse_markerIF (TobjAll obj, int idx)
{
    MVARS_Self(obj);
    switch (self->OEL_type) {
        case FIGOELL_T_EllByRad:
        case FIGOELL_T_CirByRad:
            return idx==0 ? self->OEL_start : self->OEL_end;
        default:
            return idx==0 ? self->OEL_center : idx==1 ? self->OEL_start : self->OEL_end;
    }
}

static int ellipse_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *shpt)
{
    MVARS_Self(obj);
    DBG2_E(":1:      obj=%p d=%f x/y=%f/%f",self,d,pt.x,pt.y);
    double    a, b, dx, dy;
    double    dis, r;
    double    tolerance = d;

    // we rotate the pt point so axis are co-linear with i & j
    Tdble2  ptr = pt;
    if ( self->OEL_angle!=0 )
        ROTCC_PAP(ptr, self->OEL_angle,self->OEL_center);

    // we get shpt such as center, PT & SHPT are aligned
    //     C ---------------PT-------------SHPT
    //     C -----SHPT----------PT
    dx = ptr.x - self->OEL_center.x;
    dy = ptr.y - self->OEL_center.y;
    a  = self->OEL_radiuses.x;
    b  = self->OEL_radiuses.y;
    dis= VECTOR_NormeXY(dx,dy); // dis from pt to center
    if (  dis>(max2(a,b)+tolerance) ||  // too far  of center
          dis<(min2(a,b)-tolerance) ) { // too near of center
        DBG2_L(":1:MISS  obj=%p d=%f x/y=%f/%f (1)", self,d,pt.x,pt.y);
        return -1;
    }
    r = a * b * dis / sqrt(1.0 * b * b * dx * dx + 1.0 * a * a * dy * dy);
    if (fabs(dis - r) > tolerance) {
        DBG2_L(":1:MISS  obj=%p d=%f x/y=%f/%f (1)", self,d,pt.x,pt.y);
        return -1;
    }
    // found
    shpt->x = r * dx / dis + self->OEL_center.x;
    shpt->y = r * dy / dis + self->OEL_center.y;
    if ( self->OEL_angle!=0 )
        ROTCC_PAP(*shpt,-self->OEL_angle,self->OEL_center);
    // get marker index
    int index; double dc,ds,de;
    switch (self->OEL_type) {
        case FIGOELL_T_EllByRad:
        case FIGOELL_T_CirByRad:
            // start and center are the same points
#if 0
            ds = VECTOR_Norme2PT( dble2_initVab(self->OEL_start ,*shpt) );
            de = VECTOR_Norme2PT( dble2_initVab(self->OEL_end   ,*shpt) );
            index = ds<de ? 0 : 1;
#else
            index=1;
#endif
            break;
        default:
#if 0
            dc = VECTOR_Norme2PT( dble2_initVab(self->OEL_center,*shpt) );
            ds = VECTOR_Norme2PT( dble2_initVab(self->OEL_start ,*shpt) );
            de = VECTOR_Norme2PT( dble2_initVab(self->OEL_end   ,*shpt) );
            index = dc<ds && dc<de  ? 0 : ds<dc && ds<de  ? 1 : 2;
#else
            ds = VECTOR_Norme2PT( dble2_initVab(self->OEL_start ,*shpt) );
            de = VECTOR_Norme2PT( dble2_initVab(self->OEL_end   ,*shpt) );
            index = ds<de ? 1 : 2;
#endif
            break;
    }

    DBG2_L(":1:FOUND obj=%p d=%f x/y=%f/%f index=%d shx/y=%f/%f",self,d,pt.x,pt.y,index,shpt->x,shpt->y);
    return index;
}

/*======================================================================*/
/*= Modification.                                                      =*/

static void obj_ellipse_init( TobjEllipse* self, int type, Tdble2 pt1, Tdble2 pt2, double angle);

static void ellipse_mkrOpeChange(TobjAll obj, int idx, Tdble2 pt)
{
    MVARS_Self(obj);
    switch (self->OEL_type) {
      case FIGOELL_T_EllByRad:
      case FIGOELL_T_CirByRad:
        switch ( idx ) {
          case 0: self->OEL_start = pt; break;
          case 1: self->OEL_end   = pt; break;
          default:
            PERM_spe_printf("%d unexpected marker index (expected in [0:1]\n",idx);
            return;
        }
        break;
      default:
        switch ( idx ) {
          case 1: self->OEL_start = pt; break;
          case 2: self->OEL_end   = pt; break;
          default:
            PERM_spe_printf("%d unexpected marker index (expected in [1:2]\n",idx);
            return;
        }
        break;
    }
    obj_ellipse_init(self,self->OEL_type,self->OEL_start,self->OEL_end,self->OEL_angle);
    ellipse_boundBox(obj);
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable ellipse_vtable = {
    .vt_kind        = FIG_KD_Ellipse,
    .vt_bytesz      = sizeof( TobjEllipse ),
    .vt_mask        = FIG_KDM_ELL,
    .vt_subKind     = offsetof(TobjEllipse,OEL_type),
    .vt_fdc         = offsetof(TobjEllipse,OEL_curve),
    .vt_fdl         = offsetof(TobjEllipse,OEL_curve),
//  .vt_fdl         = offsetof(TobjEllipse,OEL_line),
    .vt_fdf         = offsetof(TobjEllipse,OEL_fill),
    .vt_name        = "ellipse",
    .vt_nameF       = ellipse_nameF,
    .vt_dump        = ellipse_dump,
    .vt_length      = ellipse_length,

    .vt_dupNDP      = ellipse_dupNDP,
    .vt_boundBox    = ellipse_boundBox,

    .vt_translation = ellipse_translation,
    .vt_homothetyCC = ellipse_homothetyCC,
    .vt_scale00     = ellipse_scale00,
    .vt_scaleCC     = ellipse_scaleCC,
    .vt_rotate      = ellipse_rotate,
    .vt_flip        = ellipse_flip,

    .vt_draw_sc     = ellipse_draw_sc,
    .vt_markerNbF   = ellipse_markerNbF,
    .vt_markerIF    = ellipse_markerIF,
    .vt_centerMkr   = 1,
    .vt_centerMkrIdx= 0,
    .vt_closeto     = ellipse_closeto,

    .vt_mkrOpeChange  = ellipse_mkrOpeChange
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void obj_ellipse_init( TobjEllipse* self,
     int type, Tdble2 pt1, Tdble2 pt2, double angle)
{
    double fix_x = pt1.x;
    double fix_y = pt1.y;
    double x     = pt2.x;
    double y     = pt2.y;

    self->OEL_type                 = type;
    self->OEL_curve.dpl_join       = DA_JS_Round;
    self->OEL_curve.dpl_cap        = DA_CS_Round;

    if (self->OEL_type==FIGOELL_T_EllByRad) {
        self->OEL_angle      = angle;
        self->OEL_center.x   = fix_x;
        self->OEL_center.y   = fix_y;
        self->OEL_radiuses.x = abs(x - fix_x);
        self->OEL_radiuses.y = abs(y - fix_y);
    } else if (self->OEL_type==FIGOELL_T_EllByDia) {
        self->OEL_angle      = angle;
        self->OEL_center.x = (fix_x + x) / 2;
        self->OEL_center.y = (fix_y + y) / 2;
        self->OEL_radiuses.x = abs(self->OEL_center.x - fix_x);
        self->OEL_radiuses.y = abs(self->OEL_center.y - fix_y);
    } else if (self->OEL_type==FIGOELL_T_CirByRad) {
        self->OEL_angle      = 0.0;
        self->OEL_center.x = fix_x;
        self->OEL_center.y = fix_y;
        double rx = fix_x - x;
        double ry = fix_y - y;
        self->OEL_radiuses.x = round(sqrt(rx * rx + ry * ry));
        self->OEL_radiuses.y = self->OEL_radiuses.x;
    } else { // self->OEL_type==FIGOELL_T_CirByDia
        if ( self->OEL_type!=FIGOELL_T_CirByDia ) {
          PERM_spe_printf(
            "%d is an invalid type (changed to FIGOELL_T_CirByDia)\n",
            self->OEL_type);
        }
        self->OEL_angle      = 0.0;
        self->OEL_center.x = round((fix_x + x) / 2);
        self->OEL_center.y = round((fix_y + y) / 2);
        double rx = x - self->OEL_center.x;
        double ry = y - self->OEL_center.y;
        self->OEL_radiuses.x = round(sqrt(rx * rx + ry * ry));
        self->OEL_radiuses.y = self->OEL_radiuses.x;
    }
    self->OEL_start.x    = fix_x;
    self->OEL_start.y    = fix_y;
    self->OEL_end.x      = x;
    self->OEL_end.y      = y;
}

extern TobjAll obj_ellipse_newF(TobjVTT vtt,
                 int depth, Csty_line* stl, Csty_fill* stf,
                 int type, Tdble2 pt1, Tdble2 pt2, double alpha)
{
    TobjAll self = obj_newByODS(FIG_KD_Ellipse,vtt,depth,stl,stf,NULL);
    obj_ellipse_init(self.ell,type,pt1,pt2,alpha);
    return self;
}

extern TobjAll obj_ellipse_newA(Cfdp* fdp,
                 int type, Tdble2 pt1, Tdble2 pt2, double alpha)
{
    TobjAll self = obj_newByFdp(FIG_KD_Ellipse,fdp);
    obj_ellipse_init(self.ell,type,pt1,pt2,alpha);
    return self;
}

extern TobjAll obj_ellipse_newAcra(Cfdp* fdp,
                 int type, Tdble2 ctr, Tdble2 rds, double alpha)
{
    TEMP_asf_printf(
        type!=FIGOELL_T_CirByRad && type!=FIGOELL_T_CirByDia &&
        type!=FIGOELL_T_EllByRad && type!=FIGOELL_T_EllByDia,
        "invalid ellipse type %d.",type);
    TboxDble box;
    Tdble2   pt1,pt2;
    if ( type==FIGOELL_T_CirByRad || type==FIGOELL_T_CirByDia ) {
        alpha=0;
#if 1 // 1: NW--SE axis
        double radius = (rds.x+rds.y)/2./sqrt(2.);
        box = tool_boxdble_initXY( ctr.x-radius,ctr.y-radius,
                                   ctr.x+radius,ctr.y+radius);
        if ( type==FIGOELL_T_EllByRad ) {
            pt1 = ctr;
            pt2 = tool_boxdble_pt3(&box);
        } else {
            pt1 = tool_boxdble_pt1(&box);
            pt2 = tool_boxdble_pt3(&box);
        }
#else // W--E axis
        double radius = (rds.x+rds.y)/2.;
        if ( type==FIGOELL_T_CirByRad ) {
            pt1 = ctr;
            pt2 = dble2_init( ctr.x+radius, ctr.y);
        } else {
            pt1 = dble2_init( ctr.x-radius, ctr.y);
            pt2 = dble2_init( ctr.x+radius, ctr.y);
        }
#endif
    } else /* type==FIGOELL_T_EllByRad || type==FIGOELL_T_EllByDia */ {
        if ( dble_close0(alpha) )
            alpha=0;
        box = tool_boxdble_initXY( ctr.x-rds.x,ctr.y-rds.y,
                                   ctr.x+rds.x,ctr.y+rds.y);
        if ( type==FIGOELL_T_EllByRad ) {
            pt1 = ctr;
            pt2 = tool_boxdble_pt3(&box);
        } else {
            pt1 = tool_boxdble_pt1(&box);
            pt2 = tool_boxdble_pt3(&box);
        }
    }
    return obj_ellipse_newA(fdp, type, pt1, pt2, alpha);
}

/*======================================================================*/
