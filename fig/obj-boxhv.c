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
 *      $Id: obj-boxhv.c 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-boxhv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object(s) container the shape of which is a rectangular boxes      =*/
/*= without any rotation.                                              =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjBoxhv type.                       =*/
/*=   - the constructors of the TobjBoxhv type.                        =*/
/*= See obj-boxhv.h header for data structure of object.               =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/obj-boxhv.h"
#include "fig/object-priv.h"
#include "tools/bitmap.h"
#include "outdev/ldaf-hl.h"  // used for drawing it
#include "outdev/pxm-hl.h"   // used for drawing it

/*======================================================================*/

#define MVARS_Self(obj)    TobjBoxhv *self = obj.boxhv

#define DBG0_E(fmt,...) DBG0_printf("boxhv","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("boxhv","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("boxhv","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("boxhv","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("boxhv","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("boxhv","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("boxhv","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("boxhv","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("boxhv","Leave : "fmt,__VA_ARGS__)

#define UnRecognizedTypeArgs(func) func(\
        "%d sub-type not recognized, known are: Image=%d\n", \
          self->OBHV_type,FIGOBHV_T_Image)

/*======================================================================*/
/*= Top information.                                                   =*/

static CstrStatic boxhv_nameF(TobjAll obj)
{
    MVARS_Self(obj);
    if ( self->OBHV_type==FIGOBHV_T_Image ) return "image";
    return "box-hv";
}

static void     boxhv_dump(TobjAll obj, int sh)
{
    int i;
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); 
    printf("--type=%d\n", self->OBHV_type);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image : 
        printf("%20s:",__func__);
        for (i=0;i<sh+10;i+=1) printf(" ");
        printf("rect=%f/%f<->%f/%f img=<<%s>>\n", OBHV_diagonal(self),DBG_image(self->OBHV_img));
        break;
      default : UnRecognizedTypeArgs(PERM_spw_printf);
    }
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

static void    boxhv_free (TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image: image_delete( self->OBHV_img ); break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

static void    boxhv_clone(TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image : self->OBHV_img = image_newClone( self->OBHV_img ); break;
      default : UnRecognizedTypeArgs(PERM_spf_printf);
    }
}

static TobjAll boxhv_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
TEMP_spe_printf("Called",0);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image :
          clone = obj_image_newAbyPTPT(att,
                  self->OBHV_points[0],self->OBHV_points[2],
                  image_newClone(self->OBHV_img) );
          break;
      default : UnRecognizedTypeArgs(PERM_spf_printf);
    }
    return clone;
}

static TboxDble boxhv_boundBox(CobjAll obj)
{
    MVARS_Self(obj);
    return tool_boxDble_initByDble2T(self->OBHV_points,4);
}

/*======================================================================*/
/*= geometric transformation                                           =*/
static void boxhv_flip(TobjAll obj, int h, int v,int r, double cx, double cy);

static void boxhv_translation(TobjAll obj, double tx, double ty)
{ MVARS_Self(obj); dble2_translation(self->OBHV_points,4,tx,ty); }

//static void boxhv_homothety00(TobjAll obj, int wtcf, double f);
//static void boxhv_homothetyCC(TobjAll obj, int wtcf, double f, double cx,double cy);
//static void boxhv_scale00    (TobjAll obj, int wtcf, double fx, double fy);
static void boxhv_scaleCC(TobjAll obj, int wtcf, double fx,double fy, double cx,double cy)
{
    MVARS_Self(obj);
    dble2_scale (self->OBHV_points,4, fx,fy, cx,cy);
    if (fx<0) boxhv_flip(obj,1,0,0,cx,cy);
    if (fy<0) boxhv_flip(obj,0,1,0,cx,cy);
}

//static void boxhv_rotate   (TobjAll obj, double a, double cx, double cy);
static void boxhv_flip(TobjAll obj, int h, int v,int r, double cx, double cy)
{
    MVARS_Self(obj);
    CboxDble* box = &self->O_boundBox;  // updaded by caller obj_flip
    obj_boxhv_setRectXYWH(obj,BoxArg_XYWH(*box));

    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image :
          if ( (self->OBHV_img->img_ope&IMG_OP_ROTA_P090)!=0 ||
               (self->OBHV_img->img_ope&IMG_OP_ROTA_P270)!=0 ) {
              int tmp=h; h=v; v=tmp;
          }
          if (h) {
              if ( (self->OBHV_img->img_ope&IMG_OP_FLIP_H)!=0 )
                self->OBHV_img->img_ope &= ~IMG_OP_FLIP_H;
              else
                self->OBHV_img->img_ope |=  IMG_OP_FLIP_H;
          }
          if (v) {
              if ( (self->OBHV_img->img_ope&IMG_OP_FLIP_V)!=0 )
                self->OBHV_img->img_ope &= ~IMG_OP_FLIP_V;
              else
                self->OBHV_img->img_ope |=  IMG_OP_FLIP_V;
          }
          if ( (r%4)!=0 ) {
            if ( (self->OBHV_img->img_ope&IMG_OP_ROTA_P090)) {
              self->OBHV_img->img_ope &= ~IMG_OP_ROTA_P090;
              switch (r%4) {
                  case 1: self->OBHV_img->img_ope |= IMG_OP_ROTA_P180; break;
                  case 2: self->OBHV_img->img_ope |= IMG_OP_ROTA_P270; break;
                  case 3: self->OBHV_img->img_ope |= IMG_OP_ROTA_P000; break;
              }
            } else if ( (self->OBHV_img->img_ope&IMG_OP_ROTA_P180)) {
              self->OBHV_img->img_ope &= ~IMG_OP_ROTA_P180;
              switch (r%4) {
                  case 1: self->OBHV_img->img_ope |= IMG_OP_ROTA_P270; break;
                  case 2: self->OBHV_img->img_ope |= IMG_OP_ROTA_P000; break;
                  case 3: self->OBHV_img->img_ope |= IMG_OP_ROTA_P090; break;
              }
            } else if ( (self->OBHV_img->img_ope&IMG_OP_ROTA_P270)) {
              self->OBHV_img->img_ope &= ~IMG_OP_ROTA_P180;
              switch (r%4) {
                  case 1: self->OBHV_img->img_ope |= IMG_OP_ROTA_P000; break;
                  case 2: self->OBHV_img->img_ope |= IMG_OP_ROTA_P180; break;
                  case 3: self->OBHV_img->img_ope |= IMG_OP_ROTA_P090; break;
              }
            } else /* self->OBHV_img->img_ope is IMG_OP_ROTA_P000 */ {
              self->OBHV_img->img_ope &= ~IMG_OP_ROTA_P000;
              switch (r%4) {
                  case 1: self->OBHV_img->img_ope |= IMG_OP_ROTA_P090; break;
                  case 2: self->OBHV_img->img_ope |= IMG_OP_ROTA_P180; break;
                  case 3: self->OBHV_img->img_ope |= IMG_OP_ROTA_P270; break;
              }
            }
          }
          self->OBHV_img->img_ope = image_canonical(self->OBHV_img->img_ope);
          break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

/*======================================================================*/
/*= Drawing and filling                                                =*/

static void boxhv_doDrawData(CobjAll   obj)
{
    MVARS_Self(obj);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image : break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

static void boxhv_draw(CobjAll obj, Toutdev* od, Tod_GCdef gcd)
{
    MVARS_Self(obj);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image : 
        switch ( gcd ) {
            case OD_GC_UsrDraw: case OD_GC_UsrFill:
                od_bpi_img_drawXYHW(od,gcd,self->OBHV_img,OBHV_xywh(self));
                break;
            default:
                od_draw_rectBB(od,gcd,*obj_BBox(obj));
        }
        break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

static void boxhv_clrEGScache(CobjAll   obj, Toutdev* od)
{
    MVARS_Self(obj);
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image : 
        od_bpi_img_free(od,self->OBHV_img);
        break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

/*======================================================================*/
/*= Localization.                                                      =*/

static int boxhv_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *shpt)
{
    MVARS_Self(obj);
    double  tolerance = d;
    int     idx;
    DBG2_E(":1:     obj=%p d=%f x/y=%f/%f",self,d,pt.x,pt.y);
    for ( idx=0 ; idx<(4-1) ; idx+=1) {
        Tdble2 mkr1 = self->OBHV_points[idx];
        Tdble2 mkr2 = self->OBHV_points[idx+1];
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

static void   boxhv_mkrOpeChange   (TobjAll obj, int idx, Tdble2 pt)
{
    MVARS_Self(obj);
    Tdble2* points = self->OBHV_points;
    switch ( self->OBHV_type ) {
      case FIGOBHV_T_Image :
        //od_bpi_img_free(od,self->OBHV_img);
        switch ( idx ) {
          case 0:
            points[0]   = pt;
            points[1].y = pt.y;
            points[3].x = pt.x;
            break;
          case 1:
            points[0].y = pt.y;
            points[1]   = pt;
            points[2].x = pt.x;
            break;
          case 2:
            points[1].x = pt.x;
            points[2]   = pt;
            points[3].y = pt.y;
            break;
          case 3:
            points[0].x = pt.x;
            points[2].y = pt.y;
            points[3]   = pt;
            break;
        }
        self->O_boundBox = tool_boxDble_initByDble2T(points,4);
        break;
      default : UnRecognizedTypeArgs(PERM_spe_printf);
    }
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable boxhv_vtable = {
    .vt_kind          = FIG_KD_Boxhv,
    .vt_bytesz        = sizeof( TobjBoxhv ),
    .vt_mask          = FIG_KDM_BHV,
    .vt_subKind       = offsetof(TobjBoxhv,OBHV_type),
//  .vt_colors        = { },
//  .vt_fdc           = offsetof(TobjBoxhv,OBHV_curve),
//  .vt_fdl           = offsetof(TobjBoxhv,OBHV_line),
//  .vt_fdf           = offsetof(TobjBoxhv,OBHV_fill),
//  .vt_begArrow      = offsetof(TobjBoxhv,OBHV_begArrow),
//  .vt_endArrow      = offsetof(TobjBoxhv,OBHV_endArrow),
    .vt_name          = "boxhv",
    .vt_nameF         = boxhv_nameF,
    .vt_dump          = boxhv_dump,

    .vt_free          = boxhv_free,
//  .vt_dup           = boxhv_dup,
    .vt_clone         = boxhv_clone,
    .vt_dupNDP        = boxhv_dupNDP,
    .vt_boundBox      = boxhv_boundBox,

    .vt_translation   = boxhv_translation,
//    .vt_homothety00   = boxhv_homothety00,
//    .vt_homothetyCC   = boxhv_homothetyCC,
//    .vt_scale00       = boxhv_scale00,
    .vt_scaleCC       = boxhv_scaleCC,
  //.vt_rotate        = boxhv_rotate,
    .vt_flip          = boxhv_flip,

    .vt_doDrawData    = boxhv_doDrawData,
    .vt_draw          = boxhv_draw,
//  .vt_draw_sc       = boxhv_draw_sc,
    .vt_clrEGScache   = boxhv_clrEGScache,

    .vt_markerNb      = 4,
    .vt_markerT       = offsetof(TobjBoxhv,OBHV_points),
    .vt_closeto       = boxhv_closeto,

    .vt_mkrOpeChange  = boxhv_mkrOpeChange,
};

/*======================================================================*/
/*= Constructors                                                       =*/

extern void    obj_boxhv_setRectXYWH(TobjAll obj,
                    double x, double y, double dx, double dy)
{
    MVARS_Self(obj);
    self->OBHV_points[0] = dble2_init( x    , y    );
    self->OBHV_points[1] = dble2_init( x+dx , y    );
    self->OBHV_points[2] = dble2_init( x+dx , y+dy );
    self->OBHV_points[3] = dble2_init( x    , y+dy );
}

extern TobjAll obj_image_newFbyXYWH(TobjVTT vtt, int depth,
                    double x, double y, double dx, double dy, Timage* img)
{
    TobjAll self;
    self=obj_newByODS(FIG_KD_Boxhv,vtt,depth,NULL,NULL,NULL);
    self.boxhv->OBHV_type = FIGOBHV_T_Image;
    obj_boxhv_setRectXYWH(self,x,y,dx,dy);
    self.boxhv->OBHV_img  = img;
    return self;
}

extern TobjAll obj_image_newAbyXYWH(Cfdp* fdp,
                    double x, double y, double dx, double dy, Timage* img)
{ return obj_image_newFbyXYWH(fdp->fdp_vtt,fdp->fdp_depth,x,y,dx,dy,img); }
extern TobjAll obj_image_newAbyPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2, Timage* img)
{   TboxDble box = tool_boxdble_initPT(pt1,pt2);
    return obj_image_newFbyXYWH(fdp->fdp_vtt,fdp->fdp_depth,BoxArg_XYWH(box),img); }

/*======================================================================*/
