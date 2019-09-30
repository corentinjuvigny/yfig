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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-text.c 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Object for text.                                                   =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjText type.                        =*/
/*=   - the constructors of the TobjText type.                         =*/
/*= See obj-text.h header for data structure of object.                =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/object-priv.h"
#include "fig/obj-text.h"

#include "outdev/string-hl.h"
#include "tools/geo2d.h"

/*======================================================================*/

#define MVARS_Self(obj) TobjText *self = obj.text

#define DBG0_E(fmt,...) DBG0_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("text","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("text","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("text","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= dump and figure                                                    =*/

static void text_dump(TobjAll obj, int sh)
{
    MVARS_Self(obj);

    obj_dump_any(obj,sh,__func__); 
    printf("\n");

    char tmp[1000]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh+10);
    printf("%20s:%stype=%d--pt=(%f %f)--angle=%f--flag=%d--str=%s\n",
        __func__,tmp,self->OTE_type,
        self->OTE_base.x,self->OTE_base.y,
        self->OTE_angle,self->OTE_flag,self->OTE_str);
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

static void    text_free (TobjAll obj)
{
    MVARS_Self(obj);
    DBG1_E("obj=%p str=%s",obj,self->OTE_str);
    tool_strfreeS0( &self->OTE_str );
    DBG2_L("obj=%p",obj);
}

//static TobjAll text_dup  (Cobject obj);

static void    text_clone(TobjAll obj)
{
    MVARS_Self(obj);
    DBG1_E("obj=%p str=%s",obj,self->OTE_str);

    self->OTE_str = tool_strdup( self->OTE_str );

    DBG1_L("obj=%p str=%s",obj,self->OTE_str);
}

static TobjAll text_dupNDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    MVARS_Self(obj);
    TobjAll clone;
    clone = obj_newByFdp(FIG_KD_Text,att);
    clone.text->OTE_base   = self->OTE_base;
    clone.text->OTE_str    = tool_strdup(self->OTE_str);
    clone.text->OTE_type   = att->fdp_dir;
    clone.text->OTE_flag   = self->OTE_flag; // not used

    // update the bounding box OTE_length, OTE_height
    Csty_text* clone_stt = &clone.text->OTE_text;
    if ( clone_stt->stt_angle==self->OTE_text.stt_angle &&
         clone_stt->stt_font ==self->OTE_text.stt_font  &&
         clone_stt->stt_pts  ==self->OTE_text.stt_pts   ) {
        clone.text->O_boundBox = self->O_boundBox;
        clone.text->OTE_geom   = self->OTE_geom;
        clone.text->O_tags |= FIG_TAG_BBoxUpToDate;
    } else {
        double dx,dy,asc;
        if ( od==0 ) {
            dx=0; dy=0; asc=-1;
        } else {
            od_str_extents_SrcUnit(od,
                clone_stt->stt_font,clone_stt->stt_pts,clone.text->OTE_str,
                &asc,NULL,&dx,&dy);
        }
        obj_text_updateGeom(clone.text,dx,dy,asc);
    }
    return clone;
}

static TboxDble text_boundBox(CobjAll obj)
{
    PERM_spf_printf( "%s",
       "must never be called, must be set at creation and"
       " be updated immediately at every change.");
    return tool_boxDble_undef;
}

/*======================================================================*/
/*= geometric transformation.                                          =*/

static void text_translation(TobjAll obj, double tx, double ty)
{ MVARS_Self(obj); TRANSLATE_PXY(self->OTE_base   ,tx,ty); }

static void text_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{
    MVARS_Self(obj);
    SCALECC_PXY(self->OTE_base     ,f,f, cx,cy);
    if ( (wtcf&FIGO_SA_Text)!=0 ) {
        if ( f<0 ) {
            self->OTE_angle += TOOL_PI;
            f=abs(f);
        }
        SCALE00_Dim(self->OTE_pts      ,f);
        SCALE00_Dim(self->OTE_length   ,f);
        SCALE00_Dim(self->OTE_height   ,f);
        SCALE00_Dim(self->OTE_ascent   ,f);
        SCALE00_Dim(self->OTE_geom.nl  ,f);
        SCALE00_Dim(self->OTE_geom.pts ,f);
    }
}

static void text_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{
    MVARS_Self(obj);
    SCALECC_PXY(self->OTE_base ,fx,fy,cx,cy);
    if ( (wtcf&FIGO_SA_Text)!=0 ) {
        double f = min2(abs(fx),abs(fy));
        if ( (fx<0 && fy<0) ||
             (fx<0 && fx==-f) || 
             (fy<0 && fy==-f) ) {
            self->OTE_angle += TOOL_PI;
            f=abs(f);
        }
        SCALE00_Dim(self->OTE_pts      ,f);
        SCALE00_Dim(self->OTE_length   ,f);
        SCALE00_Dim(self->OTE_height   ,f);
        SCALE00_Dim(self->OTE_ascent   ,f);
        SCALE00_Dim(self->OTE_geom.nl  ,f);
        SCALE00_Dim(self->OTE_geom.pts ,f);
    }
}

static void text_rotate(TobjAll obj, double a, double cx, double cy)
{
    MVARS_Self(obj);
    self->OTE_angle -= a;
    ROTCC_PAXY(self->OTE_base, a,  cx,cy);
    obj_text_updateGeom(self,self->OTE_length,self->OTE_height,self->OTE_ascent);
}

static void text_flip(TobjAll obj, int h,int v,int r, double cx, double cy)
{
    MVARS_Self(obj);
    if ( h!=0 ) {
        switch ( self->OTE_type ) {
          case FIGOTE_T_LEFT:
            self->OTE_type = FIGOTE_T_RIGHT;
            //self->OTE_base.x += self->OTE_length;
            FLIP_PHVRXY(self->OTE_base, h,0,0, cx,cy);
            self->OTE_angle = -self->OTE_angle;
            break;
          case FIGOTE_T_RIGHT:
            self->OTE_type = FIGOTE_T_LEFT;
            //self->OTE_base.x -= self->OTE_length;
            FLIP_PHVRXY(self->OTE_base, h,0,0, cx,cy);
            self->OTE_angle = -self->OTE_angle;
            break;
          case FIGOTE_T_CENTER:
            FLIP_PHVRXY(self->OTE_base, h,0,0, cx,cy);
            self->OTE_angle = -self->OTE_angle;
            break;
        }
    }
    if ( v!=0 ) {
        FLIP_PHVRXY(self->OTE_base, 0,v,0, cx,cy);
            self->OTE_angle = -self->OTE_angle;
    }
    if ( r!=0 ) {
        text_rotate(obj,r*TOOL_PI_2,cx,cy);
    } else
        obj_text_updateGeom(self,self->OTE_length,self->OTE_height,self->OTE_ascent);
}

/*======================================================================*/
/*= Drawing and filling.                                               =*/

static void text_draw   (CobjAll obj, Toutdev* od, Tod_GCdef gcd)
{
    MVARS_Self(obj);
    Cstr   text = self->OTE_str;
    DBG0_TextE("obj=%p : text=%s",self,text);

#if 1 // FIXME:REMOVE
    if ( (self->O_tags&FIG_TAG_FontGeomUpToDate)==0 ) {
        double ascent,dx,dy;
        Todstrgeo* geo  = &self->OTE_geom;
        if ( od_str_extents_SrcUnit(od, geo->font,geo->pts,text,
                    &ascent,NULL,&dx,&dy)==0 ) {
            // set the EGS value
            geo->ascent = ascent;
            geo->dx     = dx;
            geo->dy     = dy;
        }
        self->O_tags |= FIG_TAG_FontGeomUpToDate;
    }
#endif
    od_str_draw(od, gcd, &self->OTE_geom, self->OTE_str, self->OTE_angle);
    DBG1_TextL("obj=%p : text=%s",self,text);
}

//static void text_draw_sc(CobjAll   obj, Tfig_scene* sc,
//                         Tod_GCdef gcd, Tod_GCdef gcf);

/*======================================================================*/
/*= Localization.                                                      =*/

static int text_markerNbF   (TobjAll obj)
{
    MVARS_Self(obj);
    switch ( self->OTE_type ) {
        case FIGOTE_T_CENTER:              return 5;
        default: /* FIGOTE_T_LEFT/RIGHT */ return 3;
    }
}
static int    text_markerNbCstF(TobjAll obj) { return 1; }
static Cdble2 text_markerIF    (TobjAll obj, int idx)
{
    MVARS_Self(obj);
    int    type = self->OTE_type;
    Tdble2 base = self->OTE_base;
    double dx   = self->OTE_length;
    double dy   = self->OTE_height;
    double as   = self->OTE_ascent;
    Tdble2 nw,mkr;
    if (idx==0) return self->OTE_base;
    switch ( type ) {
      default:
          TEMP_spe_printf("%d is an invalid type for text object",type);
      case FIGOTE_T_LEFT: 
        nw = dble2_init( base.x+ 0, base.y-as);
        switch ( idx) {
          case 1: mkr = dble2_init(nw.x+dx , nw.y+ 0); break;
          case 2: mkr = dble2_init(nw.x+dx , nw.y+dy); break;
        }
        break;
      case FIGOTE_T_CENTER: 
        nw = dble2_init( base.x-dx/2, base.y-as);
        switch ( idx) {
          case 1: mkr = dble2_init(nw.x+ 0 , nw.y+ 0); break;
          case 2: mkr = dble2_init(nw.x+dx , nw.y+ 0); break;
          case 3: mkr = dble2_init(nw.x+dx , nw.y+dy); break;
          case 4: mkr = dble2_init(nw.x+ 0 , nw.y+dy); break;
        }
        break;
      case FIGOTE_T_RIGHT: 
        nw = dble2_init( base.x-dx, base.y-as);
        switch ( idx) {
          case 1: mkr = dble2_init(nw.x+ 0 , nw.y+ 0); break;
          case 2: mkr = dble2_init(nw.x+ 0 , nw.y+dy); break;
        }
        break;
    }
    if ( ! dble_close0(self->OTE_angle) ) {
        ROTCC_PAXY(mkr,-self->OTE_angle,base.x,base.y);
    }
    return mkr;
}

static Cdble2 text_centerF (TobjAll obj)
{
    MVARS_Self(obj);
    TboxDble bb = self->O_boundBox;
    return dble2_init(Box_x(bb)+Box_dx(bb)/2,Box_y(bb)+Box_dy(bb)/2);
}

static int text_closeto(TobjAll obj, double d, Tdble2  pt, Tdble2 *mkr)
{
    double tolerance = d;
    MVARS_Self(obj);
    DBG2_E(":1:     obj=%p str=%s base=%f/%f d=%f x/y=%f/%f dx/dy=%f/%f",
            self,self->OTE_str, self->OTE_base.x,self->OTE_base.y,
            d,pt.x,pt.y,
            abs(self->OTE_base.x-pt.x), abs(self->OTE_base.y-pt.y));

    if ( dble_close0(self->OTE_angle) ) {
        CboxDble* bb = &self->O_boundBox;

        DBG2_M("obj=%p str=%s : nwx=%f x=%f sex=%f",self,self->OTE_str,
            Box_nwx(*bb),pt.x,Box_sex(*bb));
        DBG2_M("obj=%p str=%s : nwy=%f y=%f sey=%f",self,self->OTE_str,
            Box_nwy(*bb),pt.y,Box_sey(*bb));

        if ( pt.x<=Box_nwx(*bb) || Box_sex(*bb)<=pt.x ||
             pt.y<=Box_nwy(*bb) || Box_sey(*bb)<=pt.y ) {
            DBG2_L(":1:MISS  obj=%p",self);
            return -1;
        } else {
            int i; double dist;
            // we get the markers
            int mkrnb = text_markerNbF(obj);
            Tdble2 mkrs[10];
            for (i=0; i<mkrnb ; i+=1) mkrs[i] = text_markerIF(obj,i);
            // select the marker that is the nearest of pt
            int idx=0;
            dist = VECTOR_Norme2PT( dble2_initVab(pt,mkrs[0]) );
            for (i=1; i<mkrnb ; i+=1) {
                double dist2 = VECTOR_Norme2PT( dble2_initVab(pt,mkrs[i]) );
                if ( dist2<dist ) { idx=i; dist=dist2; }
            }
            *mkr = pt; //mkrs[0];
            DBG2_L(":1:FOUND obj=%p mkr=%f/%f", self,mkr->x,mkr->y);
            return idx;
        }
    } else { // angle != 0
        double angle = self->OTE_angle;
        Tdble2 xyrot = pt;
        ROTCC_PAP(xyrot,+angle,self->OTE_base);
        self->OTE_angle=0;
        TboxDble bbsave = self->O_boundBox;
        Box_x (self->O_boundBox) = self->OTE_base.x;
        Box_y (self->O_boundBox) = self->OTE_base.y - self->OTE_ascent;
        Box_dx(self->O_boundBox) = self->OTE_length;
        Box_dy(self->O_boundBox) = self->OTE_height;
        switch ( self->OTE_type ) {
          default:
            PERM_spf_printf("%d is an invalid kind (set it to left)",
                    self->OTE_type);
            self->OTE_type = FIGOTE_T_LEFT;
          case FIGOTE_T_LEFT: break;
          case FIGOTE_T_CENTER: Box_x (self->O_boundBox) -= self->OTE_length/2; break;
          case FIGOTE_T_RIGHT:  Box_x (self->O_boundBox) -= self->OTE_length; break;
        }
        int idx=text_closeto(obj,tolerance,xyrot,mkr);
        self->OTE_angle=angle;
        self->O_boundBox = bbsave;
        if ( idx>=0 ) {
            *mkr = pt;
            DBG2_L(":1:FOUND obj=%p mkr=%f/%f", self,mkr->x,mkr->y);
            return idx;
        }
    }
    DBG2_L(":1:MISS  obj=%p",obj);
    return -1;
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable text_vtable = {
    .vt_kind          = FIG_KD_Text,
    .vt_bytesz        = sizeof( TobjText ),
    .vt_mask          = FIG_KDM_TEX,
    .vt_subKind       = offsetof(TobjText,OTE_type),
    .vt_fdt           = offsetof(TobjText,OTE_text),
    .vt_name          = "text",
//  .vt_nameF         = text_nameF,
    .vt_dump          = text_dump,

    .vt_free          = text_free,
//  .vt_dup           = text_dup,
    .vt_clone         = text_clone,
    .vt_dupNDP        = text_dupNDP,
    .vt_boundBox      = text_boundBox,

    .vt_translation   = text_translation,
  //.vt_homothety00   = text_homothety00,
    .vt_homothetyCC   = text_homothetyCC,
  //.vt_scale00       = text_scale00,
    .vt_scaleCC       = text_scaleCC,
    .vt_rotate        = text_rotate,
    .vt_flip          = text_flip,

    .vt_draw          = text_draw,
  //.vt_draw_sc       = text_draw_sc,
    .vt_markerNbF     = text_markerNbF,
    .vt_markerNbCstF  = text_markerNbCstF,
    .vt_markerIF      = text_markerIF,
    .vt_centerF       = text_centerF,
    .vt_closeto       = text_closeto,
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void obj_text_init( TobjText* self,
               int type,  Tdble2 pt, double l, double h,
               CstrBrief str, int flag)
{
    self->OTE_base   = pt;
    tool_strchgClone(&self->OTE_str,str);
    self->OTE_type   = type;
    self->OTE_flag   = flag;

    // update the bounding box OTE_length, OTE_height
    obj_text_updateGeom(self,l,h,-1);
}

extern TobjAll obj_text_newF(TobjVTT vtt, int depth, Csty_text* stt,
                    int type,  Tdble2 pt, double l, double h,
                    CstrBrief str, int flag)
{
    TobjAll self = obj_newByODS(FIG_KD_Text,vtt,depth,NULL,NULL,stt);
    obj_text_init(self.text, type,pt,l,h,str,flag);
    return self;
}

extern TobjAll obj_text_newA(Cfdp* fdp,
                    int type,  Tdble2 pt, double l, double h,
                    CstrBrief str, int flag)
{
    TobjAll self = obj_newByFdp(FIG_KD_Text,fdp);
    obj_text_init(self.text, type,pt,l,h,str,flag);
    return self;
}

/*======================================================================*/
/*= others                                                             =*/

extern void obj_text_setTextFU(TobjText* self, CstrBrief str, double l, double h);
extern void obj_text_setTextOU(TobjText* self, CstrBrief str, double l, double h)
{
    tool_strchgClone(&self->OTE_str,str);
    obj_text_updateGeom(self,l,h,-1);
}

extern void obj_text_updateGeom(TobjText* self, double l, double h, double ascent)
{
    if (ascent<0 ) ascent = (3./4.)*h;
    self->OTE_ascent = ascent;
    self->OTE_length = l;
    self->OTE_height = h;
    TboxDble* bb    = &self->O_boundBox;
    double    angle = self->OTE_text.dpt_angle;
    double    x     = self->OTE_base.x;
    double    y     = self->OTE_base.y - ascent;
    if ( angle==0 ) {
        // easy case
        switch ( self->OTE_type ) {
          default:
            PERM_spf_printf("%d is an invalid kind (set it to left)",
                    self->OTE_type);
            self->OTE_type = FIGOTE_T_LEFT;
          case FIGOTE_T_LEFT: break;
          case FIGOTE_T_CENTER: x -= l/2; break;
          case FIGOTE_T_RIGHT:  x -= l; break;
        }
        Box_x(*bb)=x; Box_y(*bb)=y; Box_dx(*bb)=l; Box_dy(*bb)=h;
    } else {
        Tdble2 pt1,pt2,pt3,pt4;
        // get coordinates of 4 rectangle corners
        switch ( self->OTE_type ) {
          default:
            PERM_spf_printf("%d is an invalid kind (set it to left)",
                    self->OTE_type);
            self->OTE_type = FIGOTE_T_LEFT;
          case FIGOTE_T_LEFT:
            pt1.x=x;   pt1.y=y;
            pt2.x=x+l; pt2.y=y;
            pt3.x=x+l; pt3.y=y+h;
            pt4.x=x;   pt4.y=y+h;
            break;
          case FIGOTE_T_CENTER:
            pt1.x=x-l/2; pt1.y=y;
            pt2.x=x+l/2; pt2.y=y;
            pt3.x=x+l/2; pt3.y=y+h;
            pt4.x=x-l/2; pt4.y=y+h;
            break;
          case FIGOTE_T_RIGHT:
            pt1.x=x-l;   pt1.y=y;
            pt2.x=x;     pt2.y=y;
            pt3.x=x;     pt3.y=y+h;
            pt4.x=x-l;   pt4.y=y+h;
            break;
        }
        // rotate the 4 corners: angle radian, center OTE_base
        ROTCC_PAP(pt1,-angle,self->OTE_base);
        ROTCC_PAP(pt2,-angle,self->OTE_base);
        ROTCC_PAP(pt3,-angle,self->OTE_base);
        ROTCC_PAP(pt4,-angle,self->OTE_base);

        // (xmin,ymin) and (xmax,ymax) are the 2 corners (opposite on
        // the nw-->se diagonal of the BB.
        double xmin = min4(pt1.x, pt2.x, pt3.x, pt4.x);
        double ymin = min4(pt1.y, pt2.y, pt3.y, pt4.y);
        double xmax = max4(pt1.x, pt2.x, pt3.x, pt4.x);
        double ymax = max4(pt1.y, pt2.y, pt3.y, pt4.y);
        tool_boxDble_initXY(bb,xmin,ymin,xmax,ymax);
    }
    self->O_tags |= FIG_TAG_BBoxUpToDate;

    self->OTE_geom.nl     = self->OTE_height;
    self->OTE_geom.of     = self->OTE_type;
    self->OTE_geom.font   = self->OTE_font;
    self->OTE_geom.pts    = self->OTE_pts;
}

/*======================================================================*/
