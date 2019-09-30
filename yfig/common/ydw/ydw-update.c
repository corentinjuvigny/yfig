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
 *      $Id: ydw-update.c 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-update.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that control the update mode.                           =*/
/*=                                                                    =*/
/*= See the "ydw/ydw-updade.h" header for documentation                =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "ydw/ydw-update.h"
#include "vgui/vgui.h"
#include "vgui/vgui-dw.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

#include "tools/color.h"
#include "outdev/ldaf-hl.h"

#define BTNL_WH 14
#define BTNS_WH 11

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _TupdateEle {
    Twid_button* dpp;   // The button in DPP
    Twid_button* onoff; // The button attached to dpp
    TstyleMask   mask;  // 1<<uid
    int          uid;   // index in dw_eles
} TupdateEle;

#define NB_MAX 25

struct _Tydw_update {
    Tvgui_dw      any;

    // widgets
    Tegs_widget       dw_label;
    Twid_buttonPanel* dw_btns;

    // proper data
    TpixmapEGS    dw_pxmOff,dw_pxmOn;
    TupdateEle    dw_eles[NB_MAX];
    int           dw_eleNb; // 0: not initialized
    int           dw_mask;  // active buttons
} ;

typedef struct _Tydw_update Tydw_update;
typedef const   Tydw_update Cydw_update;

static void ydw_update_nbf_initPXM(Tydw_update* self);
static void ydw_update_nbf_initEle(Tydw_update* self);

/*======================================================================*/
/*= Interface                                                          =*/

static void ydw_update_cb_btn(Tvgui_mw*vmw, int btnId, void*ud);
//static void ydw_update_cb_unit (Twid_lenunit* lu, int u, void*ud);

extern Tvgui_dw* ydw_update_nbfTop(Tyfig_mw* ymw)
{
    //Tvgui*      gui   = vgui_dw_getVGui(vdw);
    const char*   title = "Update\nControl";
    Twid_button*  btn;

    // top-dialog creation
    Tydw_update* self = (Tydw_update*) vguiM_dw_newTop(sizeof(*self),
                                        &ymw->any,NULL);
    // dw_pxmOff/On
    ydw_update_nbf_initPXM(self);

    // dw_eles/eleNb; not do here because dpp may be not created.
    // dw_eleNb is null --> not initialized
    // They will be initialized at the first mapping of dialog.
    //ydw_update_nbf_initEle(self);

    // create proper widgets
    self->dw_label = widD_label_nbf(SelfA,title,NULL);
    self->dw_btns  = widM_buttonPanel_newR1Cn(&ymw->any, BTNL_WH, BTNL_WH);
    btn = widM_button_std_newPxm3(&ymw->any,
            "Turn on all settings for update",
            ydw_update_cb_btn, self, NULL,
            BTNL_WH, BTNL_WH, self->dw_pxmOn);
    btn->but_uid = 2;
    btn->btn_option |= WID_BTNOPT_PubPxm;
    wid_buttonPanel_addButton(self->dw_btns,btn);
    btn = widM_button_std_newPxm3(&ymw->any,
            "Turn off all settings for update",
            ydw_update_cb_btn, self, NULL,
            BTNL_WH, BTNL_WH, self->dw_pxmOff);
    btn->but_uid = 1;
    btn->btn_option |= WID_BTNOPT_PubPxm;
    wid_buttonPanel_addButton(self->dw_btns,btn);
    wid_buttonPanel_dbf(self->dw_btns,self->dw_widEGS);

    // vidget layout
    self->dw_vspace  = 0;
    self->dw_hmargin = 0;
#   define TITLE self->dw_label
#   define BTNS  self->dw_btns->bp_widEGSext
    vgui_dw_finishIC(SelfA
      ,0          ,TITLE
      ,VDWLF_CB   ,BTNS
    ,-1);

    return SelfA;
}

extern void       ydw_update_delete(Tyfig_mw* ymw, Tvgui_dw* vdw)
{
    Tydw_update* self = (Tydw_update*)vdw;
    wid_buttonPanel_free( self->dw_btns );
    gui_pixmap_free( ymw->ymw_gui, self->dw_pxmOff );
    gui_pixmap_free( ymw->ymw_gui, self->dw_pxmOn  );
    vgui_dw_delete( SelfA );
}

extern void      ydw_update_enable(Cyfig_mw* ymw)
{
    Tydw_update* self = (Tydw_update*)ymw->ymw_currYDP.dp_update;
    int i;
    ydw_update_nbf_initEle(self);

    for (i=0 ; i<self->dw_eleNb ; i+=1) {
        gui_widegs_show(self->dw_eles[i].onoff->but_widEGS);
        if ( (self->dw_mask&self->dw_eles[i].mask)!=0 )
            wid_button_setOn(self->dw_eles[i].onoff);
        else 
            wid_button_setOff(self->dw_eles[i].onoff);
    }
}

extern void      ydw_update_disable(Cyfig_mw* ymw)
{
    Tydw_update* self = (Tydw_update*)ymw->ymw_currYDP.dp_update;
    //gui_widegs_unshow(self->dw_widEGS);
    int i;

    for (i=0 ; i<self->dw_eleNb ; i+=1) {
        gui_widegs_unshow(self->dw_eles[i].onoff->but_widEGS);
    }
}

extern TstyleMask ydw_update_getMask(Cyfig_mw* ymw)
{
    Tydw_update* self = (Tydw_update*)ymw->ymw_currYDP.dp_update;
    return self->dw_mask;
}

/*======================================================================*/
/*= Callbacks                                                          =*/

static void ydw_update_cb_btn(Tvgui_mw*vmw, int btnId, void*ud)
{
    Tydw_update* self = ud;
    Tyfig_mw*    ymw  = (Tyfig_mw*) vmw;
    printf("%s: btnId=%d\n",__func__,btnId);
    int i;
    for (i=0 ; i<self->dw_eleNb ; i+=1) {
        if ( btnId==2 ) {
            self->dw_mask |= self->dw_eles[i].mask;
            wid_button_setOn(self->dw_eles[i].onoff);
        } else {
            self->dw_mask &= ~self->dw_eles[i].mask;
            wid_button_setOff(self->dw_eles[i].onoff);
        }
    }
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void ydw_update_nbf_initPXM(Tydw_update* self)
{
    Ccolor *fg, *bg;
    Tvgui* gui = vgui_dw_getVGui(SelfA);
    int dx=BTNL_WH;
    int dy=BTNL_WH;

    vgui_wn_getColor(gui,wn_mainBtnStd,&fg,&bg);
    Tcolor*          red   = color_newByRGB(gui->colorDriver,0xFF,0x30,0x30);
    Tcolor*          green = color_newByRGB(gui->colorDriver,0x30,0xFF,0x30);
    Tvgui_odpixmap*  pdev  = vgui_odpixmap_new(gui,bg,dx,dy,NULL);
    Toutdev*         od    = &pdev->vv_od;

    od_ldaf_setGCll (od, OD_GC_SysFill,0.0,DA_FS_Solid, red,  0,0,0);
    od_fill_rectXYWH(od, OD_GC_SysFill,1,1,dx-2,dy-2);
    self->dw_pxmOff =  pdev->vv_pixmap;
    pdev->vv_pixmap = gui_pixmap_create(gui,dx,dy);

    od_ldaf_setGCll (od, OD_GC_SysFill,0.0,DA_FS_Solid, bg,   0,0,0);
    od_fill_rectXYWH(od, OD_GC_SysFill,0,0,dx,dy);
    od_ldaf_setGCll (od, OD_GC_SysFill,0.0,DA_FS_Solid, green,0,0,0);
    od_fill_rectXYWH(od, OD_GC_SysFill,1,1,dx-2,dy-2);
    self->dw_pxmOn =  vgui_odpixmap_delete2( pdev );

    color_delete(red);
    color_delete(green);
}

static void ydw_update_cb_btnToggle(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_update*     self = ud;
    TupdateEle*      p    = self->dw_eles+btnId;
    Twid_button*     btn  = p->onoff;
    if ( btn->but_onoff_state==WID_BUT_STATE_On ) 
        self->dw_mask |= p->mask;
    else
        self->dw_mask &= ~p->mask;
}

static void ydw_update_nbf_initEle(Tydw_update* self)
{
    if ( self->dw_eleNb!=0 )
        // already initialized
        return;

    Tvgui*           gui = vgui_dw_getVGui(SelfA);
    Tyfig_mw*        ymw = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tyfig_drawparam* ydp = &ymw->ymw_currYDP;
    TupdateEle*      p;
    int i=0;

#define INIT_ELE(i,dp_field,m) \
    p = self->dw_eles+i; \
    p->dpp   = ydp->dp_field; \
    p->onoff = widD_button_onoff_nbfPxm(SelfA, \
            "Enable/disable this setting for updating", \
            ydw_update_cb_btnToggle,self,NULL, \
            BTNS_WH,BTNS_WH,self->dw_pxmOff,self->dw_pxmOn); \
    p->onoff->btn_option |= WID_BTNOPT_PubPxm; \
    p->mask = m; i += 1

    INIT_ELE(i,dp_depth,FDP_G_depth);
  //INIT_ELE(i,dp_angle,FDP_G_angle);

    // line
    INIT_ELE(i,dp_linewidth,FDP_L_thick);
    INIT_ELE(i,dp_linecolor,FDP_L_color);
    INIT_ELE(i,dp_linestyle,FDP_L_style);
    INIT_ELE(i,dp_linearrow,FDP_A_mode);

    // fill
    INIT_ELE(i,dp_fillcolor,FDP_F_style|FDP_F_color);
    INIT_ELE(i,dp_filltile ,FDP_F_style|FDP_F_slp);

    // text
    INIT_ELE(i,dp_textdir   , FDP_G_dir);
    INIT_ELE(i,dp_textcolor , FDP_T_color);
    INIT_ELE(i,dp_textsize  , FDP_T_pts);
    INIT_ELE(i,dp_textfont  , FDP_T_font);
    INIT_ELE(i,dp_textangle , FDP_T_angle);
  //INIT_ELE(i,dp_textLSF   , FDP_T_???);

    self->dw_eleNb = i;

    for (i=0 ; i<self->dw_eleNb ; i+=1) {
        self->dw_eles[i].onoff->but_uid = i;
        vgui_widegs_putW1inNEofW2(gui,self->dw_eles[i].onoff->but_widEGS,
                                      self->dw_eles[i].dpp->but_widEGS);
    }
}

/*======================================================================*/
