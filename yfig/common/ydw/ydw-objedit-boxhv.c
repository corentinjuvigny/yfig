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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-objedit-boxhv.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-boxhv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top dialog for editing horizontal/vertical box objects.            =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"

#include "vgui/vgui-pixmap.h"
#include "vgui/wid-lvu.h"
#include "vgui/wid-filebrowser.h"
#include "fig/obj-boxhv.h"
#include "tools/bitmap.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Toed_boxhv Toed_boxhv;
typedef const   Toed_boxhv Coed_boxhv;

struct _Toed_boxhv { YOED_FIELDS();
    Twid_grp*  points[4];
    Twid_ilvu* imgopWid;
    TpixmapEGS imgopOrgPxm;  // the pxm without transformation
    TwidEGS    imgopOrgWid;  // the label that shows imgopOrgPxm
};

Inline Tyoed_any* ANY(Toed_boxhv* self) { return (Tyoed_any*)self; }

static Twid_anyVtable widany_vt_boxhv;

static int imgops[8] = {
    IMG_OP_CAN_FX0FY0R000, IMG_OP_CAN_FX1FY0R000,
    IMG_OP_CAN_FX0FY1R000, IMG_OP_CAN_FX1FY1R000,
    IMG_OP_CAN_FX0FY0R090, IMG_OP_CAN_FX1FY0R090,
    IMG_OP_CAN_FX0FY1R090, IMG_OP_CAN_FX1FY1R090,
};
#define imgopn TOOL_TABLE_NbEle(imgops)

static int Imgop2Idx(int imgop) { int i; for ( i=0 ; i<imgopn ; i+=1) if (imgops[i]==imgop) return i; return 0; }
Inline int Idx2Imgop(int idx)   { return imgops[idx]; }

/*======================================================================*/
/*= Interface                                                          =*/

static TpixmapEGS oedboxhv_genImgopPxm(Tvgui*gui, CcolorSet*clrs, int imgop);

static void oedbox_cb_pt0123(Toed_boxhv*self, int ipt, int ifw, int ibw)
{
   //      order 1  order 2
   // pt0:  x0 y0    x0 y0
   // pt1:  x1 y0    x0 y1
   // pt2:  x1 y1    x1 y1
   // pt3:  x0 y1    x1 y0
    Tdble2 pt = wid_grp_getDblePt(self->points[ipt]);
    Tdble2 fw = wid_grp_getDblePt(self->points[ifw]);
    Tdble2 bw = wid_grp_getDblePt(self->points[ibw]);
    if        ( pt.x == fw.x ) { // order 2 & pt.y changed
        wid_grp_setDble01(self->points[ibw], pt.y);
    } else if ( pt.y == bw.y ) { // order 2 & pt.x changed
        wid_grp_setDble10(self->points[ifw], pt.x);
    } else if ( pt.y == fw.y ) { // order 1 & pt.x changed
        wid_grp_setDble10(self->points[ibw], pt.x);
    } else if ( pt.x == bw.x ) { // order 1 & pt.y changed
        wid_grp_setDble01(self->points[ifw], pt.y);
    }
    yoed_any_setNewObj(ANY(self));
}

static void oedboxhv_cb_imgop(void* ud, ...)          { yoed_any_setNewObjObjChg( ud ); }
static void oedboxhv_cb_pt0  (Twid_grp*grp, void* ud) { oedbox_cb_pt0123(ud,0,1,3); }
static void oedboxhv_cb_pt1  (Twid_grp*grp, void* ud) { oedbox_cb_pt0123(ud,1,2,0); }
static void oedboxhv_cb_pt2  (Twid_grp*grp, void* ud) { oedbox_cb_pt0123(ud,2,3,1); }
static void oedboxhv_cb_pt3  (Twid_grp*grp, void* ud) { oedbox_cb_pt0123(ud,3,0,2); }

extern Tyoed_any* yoed_boxhv_nbf(Tyfig_mw* ymw, TobjAll obj)
{
    int i;
    Toed_boxhv*  self;
    int          kind = obj_Kind(obj);
    Tvgui*       gui  = yfig_mw_getVGui(ymw);

    TEMP_asf_printf( kind!=FIG_KD_Boxhv, "%d is an unexpected object kind",kind);

    self = (Toed_boxhv*)yoed_any_new(&widany_vt_boxhv,ymw,obj);

    // rectangle point
    self->points[0] = wid_iopt_nbf(self->objparams,"point 1 x/y", NULL,oedboxhv_cb_pt0,self, obj.boxhv->OBHV_points[0], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->points[1] = wid_iopt_nbf(self->objparams,"point 2 x/y", NULL,oedboxhv_cb_pt1,self, obj.boxhv->OBHV_points[1], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->points[2] = wid_iopt_nbf(self->objparams,"point 3 x/y", NULL,oedboxhv_cb_pt2,self, obj.boxhv->OBHV_points[2], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->points[3] = wid_iopt_nbf(self->objparams,"point 4 x/y", NULL,oedboxhv_cb_pt3,self, obj.boxhv->OBHV_points[3], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);

    // symetry and rotation of box
    int imgop= obj.boxhv->OBHV_img->img_ope;
#define IOPPxm_DX 50
#define IOPPxm_DY 30
    CcolorSet* clrs = yfig_mw_getClrs(ymw);
    TpixmapEGS*pxms = tool_malloc(sizeof(*pxms)*imgopn); // eaten by dw_dirWid.
    for ( i=0 ; i<imgopn ; i+=1) pxms[i]=oedboxhv_genImgopPxm(gui,clrs,imgops[i]);
    self->imgopWid = wid_ilvu_newDdlPxm( Imgop2Idx(imgop), pxms,imgopn, IOPPxm_DX,IOPPxm_DY, oedboxhv_cb_imgop,self);
    wid_ilvu_addLabel (self->imgopWid, "image operation", YDW_LAB_WITH);
    widD_ilvu_dbf     (self->imgopWid, SelfA);
    self->imgopOrgPxm = oedboxhv_genImgopPxm(gui,clrs,imgops[0]);
    self->imgopOrgWid = widD_label_nbfBox(SelfA,0,IOPPxm_DX,IOPPxm_DY);
    wid_label_setBgPxm(self->imgopOrgWid,self->imgopOrgPxm);

    // layout
#define IopLab      self->imgopWid->lvu_labEGS
#define IopVal      self->imgopWid->lvu_valEGS
#define IopOrg      self->imgopOrgWid
    vgui_dw_finishMTX(self->objparams
        ,VDWLF_ROW(0) ,MTX_IOPT_VXYx(0,self->center)
        ,VDWLF_ROW(1) ,MTX_IOPT_VXYx(0,self->points[0])
        ,VDWLF_ROW(2) ,MTX_IOPT_VXYx(0,self->points[1])
        ,VDWLF_ROW(3) ,MTX_IOPT_VXYx(0,self->points[2])
        ,VDWLF_ROW(4) ,MTX_IOPT_VXYx(0,self->points[3])
        ,VDWLF_ROW(5) ,0,IopLab  ,1,IopOrg  ,2,IopVal
        ,-1);

    yoed_any_dbf((Tyoed_any*)self);
    return (Tyoed_any*)self;
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void oedboxhv_vt_objChg(void*oed)
{
    int i; Tdble2 pt;
    Toed_boxhv* self = oed;
    for ( i=0; i<4 ; i+=1) {
        pt = yoed_any_currObj(oed).boxhv->OBHV_points[i];
        wid_grp_setDblePt(self->points[i],pt);
    }
}

static void oedboxhv_vt_unitChg(void*oed)
{
    int i;
    Toed_boxhv* self = oed;
    for ( i=0 ; i<4 ; i+=1) wid_grp_chgUsrUnit( self->points[i], self->dw_uu );
}

static TobjAll oedboxhv_vt_newObj(void*oed)
{
    Toed_boxhv* self = oed;
    int       idx      = wid_ilvu_getNum10(self->imgopWid);
    int       imgop    = Idx2Imgop(idx);
    int       oldimgop = yoed_any_currObj(oed).boxhv->OBHV_img->img_ope;
    Tdble2    pt1      = wid_grp_getDblePt( self->points[0] );
    Tdble2    pt3      = wid_grp_getDblePt( self->points[2] );
    if ( (oldimgop&IMG_OP_ROTA) != (imgop&IMG_OP_ROTA) ) {
        int       dx2 = abs(pt1.x-pt3.x)/2;
        int       dy2 = abs(pt1.y-pt3.y)/2;
        Tdble2    ctr = dble2_init( (pt1.x+pt3.x)/2, (pt1.y+pt3.y)/2 );
        pt1 = dble2_init( ctr.x-dy2 , ctr.y-dx2);
        pt3 = dble2_init( ctr.x+dy2 , ctr.y+dx2);
    }

    Twid_filebrowser* fb = self->filebrowser;
    Timage* img  = image_new( vgui_filebrowser_getFilPath(fb), imgop);
    return obj_image_newAbyPTPT(&self->dw_dp, pt1,pt3, img);
}

static Tyoed_vtable yoed_vt_boxhv = {
    .op_size=sizeof(Twid_box), .op_center=1,
    .objChg  = oedboxhv_vt_objChg,
    .unitChg = oedboxhv_vt_unitChg,
    .newObj  = oedboxhv_vt_newObj,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oedboxhv_vt_free(Twid_any*oed)
{
    int i;
    Toed_boxhv* self = (void*)oed;
    Tvgui*      gui  = vgui_dw_getVGui( SelfA );
    for ( i=0 ; i<4 ; i+=1) wid_grp_delete ( self->points[i] );
    wid_ilvu_free( self->imgopWid );
    gui_pixmap_free(gui,self->imgopOrgPxm);
    // imgopOrgWid (EGS frees labels autmaically)
}
static Twid_anyVtable widany_vt_boxhv = { WID_ANY_SCVT_TBX(sizeof(Toed_boxhv),&yoed_any_vtable,oedboxhv_vt_free, &yoed_vt_boxhv) };

/*======================================================================*/

#include "yfig-device.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-outdev.h"
#include "outdev/ldaf-hl.h"
#include "fig/object.h"

static TpixmapEGS oedboxhv_genImgopPxm(Tvgui*gui, CcolorSet*clrs, int imgop)
{
    Ccolor         *fg,*bg; vgui_wn_getColor(gui,wn_dialSub,&fg,&bg);
    Tvgui_odpixmap* odpxm = vgui_odpixmap_new(gui,bg, IOPPxm_DX,IOPPxm_DY,NULL);
    Tyfig_device*   fsod  = fsod_attach(&odpxm->v_od);
    Toutdev*        od    = &odpxm->vv_od;
    Tod_GCdef       gcd   = OD_GC_UsrDraw;
    TobjAll         dir   = obj_dir_mkdir ( fsod_getCurrObj(fsod) );
    double          ppi   = fsod_getPpiSrc(fsod);
    TobjAll         obj;
    Tfdp            fdp;

    memset(&fdp,0,sizeof(fdp));
    fdp.fdp_style.sty_line = odsty_copyLine( od_style_topLine(od,OD_GC_SysDraw) );
    fdp.fdp_style.sty_fill = odsty_copyFill( od_style_topFill(od,OD_GC_SysFill) );
    fdp.fdp_depth                    = 50;
    fdp.fdp_style.sty_fill.stf_style = DA_FS_Solid;
    
    double sep = 2;
    double dx  = min2(IOPPxm_DX,IOPPxm_DY)-sep*2;
    double dy  = min2(IOPPxm_DX,IOPPxm_DY)*3./4.-sep*2;
    double x   = (IOPPxm_DX-dx)/2;
    double y   = (IOPPxm_DY-dy)/2;

    color_chgLazy(&fdp.fdp_style.sty_fill.stf_color, fig_colors_getCyan(clrs) );
    obj = obj_sqrrect_newAXYWH(&fdp,x,y,dx,dy);
    obj_setPpi(obj,ppi);
    obj_dir_addObj(dir,obj);
    double r   = 2.5;
    Tdble2 ctr = dble2_init( x+dx-(r+sep*.66) , y+dy-(r+sep*.66) );
    Tdble2 r2  = dble2_init( r , r );
    fdp.fdp_depth                    = 40;
    color_chgLazy(&fdp.fdp_style.sty_fill.stf_color, fig_colors_getRed(clrs) );
    obj = obj_ellipse_newAcra(&fdp,FIGOELL_T_CirByRad,ctr,r2,0.0);
    obj_setPpi(obj,ppi);
    obj_dir_addObj(dir,obj);

    int fx,fy,rot; image_canonical_hvr(imgop,&fx,&fy,&rot);
    od_style_pushTop(od,gcd);
        ctr = obj_center(dir);
        obj_flip(dir,fx,fy,rot,ctr.x,ctr.y);
        fig_scene_draw( fsod_getScene(fsod) );
    od_style_pop(od,gcd);

    fsod_detach(fsod);
    odsty_free( &fdp.fdp_style );
    return vgui_odpixmap_delete2(odpxm);
}

/*======================================================================*/
