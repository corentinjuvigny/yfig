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
 *      $Id: ydw-fill.c 371 2018-11-23 15:05:55Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-fill.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that handles filling area parameters.                   =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-pixmap.h"
#include "ydw/ydw.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-dw.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-lvu.h"

#include "tools/color.h"
#include "outdev/resources.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tydw_fill {
    Tvgui_dw          any;
    Twid_olvu*        dw_color;
    Tvgui_dw*         dw_color_d;
    Twid_olvu*        dw_slp;
#define DW_slpBtn(s)  ((Twid_button*)wid_olvu_getUniWid(self->dw_slp))
    Twid_buttonPanel* dw_slpPxm;
    Tsty_fill         dw_dpf;
    Tvgui_odpixmap*   dw_pod;   // used by the cb fill_act_update to preserve pixmap

    Tydw_fill_cb      dw_cbChg;
    void*             dw_cbUd;
};

// redraw the widget
static void fill_act_update(Tydw_fill* self);

// build the backgrounf pixmap panel
static Twid_buttonPanel* fill_slpPxm_nbf(Tvgui_dw* vdw, int dx, int dy,
                                       Ccolor*color, Twid_button_cbDw cb);

// callbacks when color changed and when bg pixmap changed.
static void fill_cb_color(Tvdw_colorSel*cs, Ccolor*c, void*ud);
static void fill_cb_slpPxm(Tvgui_dw* vdw, int uid, void*ud);

/*======================================================================*/
/*= Interface                                                          =*/

extern Tydw_fill* ydw_fill_nbf(Tvgui_dw* vdw,
                               Tyfig_mw* ymw,
                               Csty_fill* dpf, Tydw_fill_cb cb, void*ud)
{
    Tvgui*      gui        = vgui_dw_getVGui(vdw);
    char*       title      = "Filling area configuration";
    int i,nbe,dx,dy;
    TpixmapEGS   pxm;
    Twid_button* btn;

    Tydw_fill* self = (Tydw_fill*) vgui_dw_newSub(sizeof(*self), vdw,
                            tool_strdup(title));
    vgui_dw_titleAddLabTxt(SelfA, "none", YDW_VIEWER_WITH);
    vgui_dw_titleAddFufLabR(SelfA);

    self->dw_dpf   = *dpf;
    self->dw_cbChg = cb;
    self->dw_cbUd  = ud;

    // widgets for changing color 
    self->dw_color_d = ydw_colors_nbfWN(SelfA,ymw, NULL,
                                        fill_cb_color,self,NULL);
    self->dw_color = wid_olvu_newC("none", YDW_VAL_WITH,NULL,0,0);
                                   //dp->dpl_color,0,0);
    wid_olvu_addLabel  (self->dw_color, "color", YDW_LAB_WITH);
    btn = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    wid_olvu_addButton (self->dw_color, btn);
    widD_olvu_dbf      (self->dw_color, SelfA);
    //widD_button_fuf_addWidToCtl(btn,self->dw_color_d->D_widEGS,NULL);
    widD_button_fuf_addWidToIgn(btn,self->dw_color_d->D_widEGS,NULL);

    // widgets for changing bg tile 
    dx = YDW_FIL_PxmDx; 
    dy = YDW_FIL_PxmDy;
    Tcolor* color = color_newByStr(gui->colorDriver,"gray50");
    self->dw_slpPxm = fill_slpPxm_nbf(SelfA,dx,dy,color,fill_cb_slpPxm);
    color_delete(color);
    //self->dw_slp = wid_olvu_newC("none", YDW_VAL_WITH, NULL, dx,dy);
    self->dw_slp = wid_olvu_newC("none", YDW_VAL_WITH, NULL, 0,0);
    wid_olvu_addLabel  (self->dw_slp, "shade", YDW_LAB_WITH);
    btn = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    wid_olvu_addButton (self->dw_slp, btn);
    widD_olvu_dbf    (self->dw_slp, SelfA);
    //widD_button_fuf_addWidToIgn(btn,self->dw_slpPxm->bp_widEGS,NULL);
    widD_button_fuf_addWidToCtl(btn,self->dw_slpPxm->bp_widEGSext,NULL);

    // create output pixmap device
    self->dw_pod = vgui_odpixmap_new(gui,gui->vg_black,50,50,NULL);

    // layout
#define ColorL    self->dw_color->lvu_labEGS
#define ColorV    self->dw_color->lvu_valEGS
#define ColorU    self->dw_color->lvu_uniEGS
#define ColorD    self->dw_color_d->D_widEGS
#define ShadeL    self->dw_slp->lvu_labEGS
#define ShadeV    self->dw_slp->lvu_valEGS
#define ShadeU    self->dw_slp->lvu_uniEGS
#define SLbtns    self->dw_slpPxm->bp_widEGSext

#define LN(x) VDWLF_ROW(x)
    vgui_dw_finishMTX(SelfA
      ,LN(0) ,0,ColorL   ,1,ColorV    ,2,ColorU
      ,LN(1) ,0,ColorD   ,1,ColorD    ,2,ColorD    ,3,ColorD
      ,LN(2) ,0,ShadeL   ,1,ShadeV    ,2,ShadeU
      ,LN(3) ,0,SLbtns   ,1,SLbtns    ,2,SLbtns    ,3,SLbtns
      ,-1);
    
    //vgui_dw_titleFold(SelfA);
    //widD_button_fuf_setFolded(self->dw_slpBtn);
    fill_act_update(self);
    return self;
}

extern void       ydw_fill_free(Tydw_fill* self)
{
    wid_olvu_free       ( self->dw_color );
    ydw_colors_free     ( self->dw_color_d );
    wid_olvu_free       ( self->dw_slp );
    wid_buttonPanel_free( self->dw_slpPxm );
    vgui_odpixmap_delete( self->dw_pod );
    vgui_dw_free        ( SelfA );
}

extern Tegs_widget ydw_fill_widEGS     (Tydw_fill* self)
{ return self->dw_widEGS; }

extern Csty_fill* ydw_fill_dpf (Tydw_fill* self)
{ return &self->dw_dpf; }

extern void       ydw_fill_unfoldColor(Tydw_fill* self)
{
    Twid_button* btn;
    btn = (Twid_button*) wid_olvu_getUniWid(self->dw_color);
    vgui_dw_titleUnFold( SelfA );
    widD_button_fuf_setUnFolded( btn );
    fill_act_update(self);
}

extern void       ydw_fill_unfoldTile (Tydw_fill* self)
{
    //vgui_dw_titleUnFold( SelfA );
    widD_button_fuf_setUnFolded( DW_slpBtn(self) );
    fill_act_update(self);
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static Twid_buttonPanel* fill_slpPxm_nbf(Tvgui_dw* vdw, int dx, int dy,
                                       Ccolor*color, Twid_button_cbDw cb)
{
    int i,nbe;
    Twid_button*      btn;
    TpixmapEGS        pxm;
    Tvgui*            gui = vdw->D_gui;
    Twid_buttonPanel* self;

    self = widD_buttonPanel_newRaCa(vdw,dx,dy);
    self->bp_colnb = YDW_FIL_NbCol;
  //self->bp_rownb = (pxm_fill_shade_nbe()*2-3 +
  //                  ico_fill_pattern_n+YDW_FIL_NbCol-1)/YDW_FIL_NbCol;
    self->bp_buttonHVsep = 5;

    Tod_GCdef      gcd = OD_GC_SysFill;
    TodResources*  res = gui->vg_odResources;
    Tvgui_odpixmap*pod = vgui_odpixmap_new(gui,gui->vg_black,dx,dy,NULL);
    Toutdev*       od  = &pod->vv_od;
  od_style_pushTop(od,gcd);
    for ( i=1 ; i<res->shadBtms->eleNb ; i++ ) {
        od_ldaf_setGCll(od,gcd,1.0,DA_FS_ShadTile,color,i,0,0);
        od_fill_rectXYWH(od,gcd,0,0,dx,dy);
        pxm = vgui_odpixmap_getPxm(pod);
        btn = widD_button_std_newPxm3(vdw, NULL,cb,NULL,NULL, dx,dy,pxm);
        btn->but_uid = i;
        wid_buttonPanel_addButton(self,btn);
    }
    for ( i=0 ; i<(res->lighBtms->eleNb-1) ; i++ ) {
        od_ldaf_setGCll(od,gcd,1.0,DA_FS_LighTile,color,0,i,0);
        od_fill_rectXYWH(od,gcd,0,0,dx,dy);
        pxm = vgui_odpixmap_getPxm(pod);
        btn = widD_button_std_newPxm3(vdw, NULL,cb,NULL,NULL, dx,dy,pxm);
        btn->but_uid = i + 100;
        wid_buttonPanel_addButton(self,btn);
    }
    for ( i=0 ; i<res->pattBtms->eleNb ; i++ ) {
        od_ldaf_setGCll(od,gcd,1.0,DA_FS_PattTile,color,0,0,i);
        od_fill_rectXYWH(od,gcd,0,0,dx,dy);
        pxm = vgui_odpixmap_getPxm(pod);
        btn = widD_button_std_newPxm3(vdw, NULL,cb,NULL,NULL, dx,dy,pxm);
        btn->but_uid = i + 200;
        wid_buttonPanel_addButton(self,btn);
    }
  od_style_pop(od,gcd);
    vgui_odpixmap_delete(pod);

    btn = widD_button_std_newLab3(vdw, NULL,
                                  cb,NULL,NULL, "none", NULL);
    btn->but_uid = -1;
    wid_buttonPanel_addButton(self,btn);
    wid_buttonPanel_dbf( self , NULL);
    wid_button_setSize(btn,dx,dy);
    return self;
}

static void fill_act_update(Tydw_fill* self)
{
    Tvgui*       gui     = self->dw_gui;
    Csty_fill*   dpf     = &self->dw_dpf;
    Tegs_widget  mlabel  = vgui_dw_titleLab(SelfA);
    Twid_olvu*   cviewer = self->dw_color;
    Twid_olvu*   pviewer = self->dw_slp;
    Twid_button* slpbtn  = DW_slpBtn(self);

    if ( dpf->dpf_style==DA_FS_NoFill ) {
        Ccolor *mlabelBg = vgui_wn_getBgColor(gui, wn_dialLabel);
        wid_label_setBgColor(mlabel,mlabelBg);
        wid_label_setText   (mlabel,"none");
        wid_olvu_reset      (cviewer);
        wid_olvu_reset      (pviewer);
        widD_button_fuf_setFolded(slpbtn);
        wid_button_disable       (slpbtn);
        return;
    }
    wid_button_enable    (slpbtn);
    if ( dpf->dpf_style==DA_FS_Solid ) {
        wid_label_setBgColor (mlabel,dpf->dpf_color);
        wid_label_setText    (mlabel,"solid");
        wid_olvu_setBgClr    (cviewer,dpf->dpf_color);
        wid_olvu_setStr      (cviewer,"");
        wid_olvu_reset       (pviewer);
        return;
    }
    Tod_GCdef      gcd = OD_GC_SysFill;
    Tvgui_odpixmap*pod = self->dw_pod;
    Toutdev*       od  = &pod->vv_od;
  od_style_pushTop(od,gcd);
    od_ldaf_setGCll_byStyle(od, gcd, 1.0, dpf);
    od_fill_rectXYWH       (od, gcd, 0,0, Box_dx(od->boxSrc), Box_dy(od->boxSrc));
  od_style_pop(od,gcd);
    TpixmapEGS tileEGS = pod->vv_pixmap;
    wid_label_setBgPxm (mlabel,tileEGS);
    wid_label_setText  (mlabel,"");
    wid_olvu_setBgPxm  (cviewer,tileEGS);
    wid_olvu_setStr    (cviewer,"");
    wid_olvu_setBgPxm  (pviewer,tileEGS);
    wid_olvu_setStr    (pviewer,"");
}

/*======================================================================*/
/*= Callback                                                           =*/

static void fill_cb_slpPxm(Tvgui_dw* vdw, int uid, void*ud)
{
    Tydw_fill* self = (Tydw_fill*)vdw;
    Tsty_fill* dpf  = &self->dw_dpf;
    int        idx  = uid;
    DBG0_CdwE("uid=%d",uid);
    if ( idx==-1 ) {
        if ( dpf->stf_color==0 ) {
            dpf->stf_style = DA_FS_NoFill;
            dpf->stf_shade=dpf->stf_light=dpf->stf_pattern=0;
        } else {
            dpf->stf_style = DA_FS_Solid;
            dpf->stf_shade=dpf->stf_light=dpf->stf_pattern=0;
        }
    } else if ( idx<100 ) {
        dpf->stf_style = DA_FS_ShadTile;
        dpf->stf_shade = idx;
    } else if ( idx<200 ) {
        dpf->stf_style = DA_FS_LighTile;
        dpf->stf_light = idx - 100;
    } else if ( idx<300 ) {
        dpf->stf_style   = DA_FS_PattTile;
        dpf->stf_pattern = idx - 200;
    } else {
        TEMP_spe_printf("index %d out of range [-1:300[",idx);
    }
    DBG1_CdwM("uid=%d : sytle=0x%04x sidx=%d lidx=%d pidx=%d : color=%s",
        uid,dpf->dpf_style,dpf->dpf_shade,dpf->dpf_light,
        dpf->dpf_pattern,DBG_Color2str(dpf->dpf_color));
    fill_act_update(self);
    if ( self->dw_cbChg!=0 )
         self->dw_cbChg( self->dw_cbUd, &self->dw_dpf );
    DBG0_CdwL("uid=%d",uid);
}

static void fill_cb_color(Tvdw_colorSel*cs, Ccolor*c, void*ud)
{
    Tydw_fill* self = ud;
    Tsty_fill* dpf  = &self->dw_dpf;
    DBG0_CdwE("color=%s",DBG_Color2str(c));
    if ( c==0 ) {
        // the user selected the "none" button 
        // fig_fdp_initDPFnoFill(dpf);
        dpf->stf_style = DA_FS_NoFill;
        dpf->stf_shade=dpf->stf_light=dpf->stf_pattern=0;
        fill_act_update(self);
        return;
    }
    // the user selected a true color
    if ( dpf->dpf_style==DA_FS_NoFill ) {
        dpf->stf_style = DA_FS_Solid;
        dpf->stf_shade=dpf->stf_light=dpf->stf_pattern=0;
    }
//  color_delete( dpf->stf_color );
    dpf->stf_color = (Tcolor*)c;
    fill_act_update(self);
    if ( self->dw_cbChg!=0 )
        self->dw_cbChg( self->dw_cbUd, &self->dw_dpf );
    DBG0_CdwL("color=%s",DBG_Color2str(c));
}

/*======================================================================*/
