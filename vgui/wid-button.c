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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: wid-button.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-button.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons.                                             =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for the type and function         =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

/*======================================================================*/
/*= abstract accessors                                                 =*/

extern Tvgui* wid_button_getVGui(Cwid_button* self)
{
    if ( self->but_panel ) return wid_buttonPanel_getVGui(self->but_panel);
    if ( self->but_vdw )   return vgui_dw_getVGui(self->but_vdw);
    if ( self->but_vmw )   return vgui_mw_getVGui(self->but_vmw);
    PERM_spf_printf("No gui found",0);
    return 0;
}

extern Tegs_widget wid_button_getWidEGS(Cwid_button*self)
{ return self->but_widEGS; }

extern Tvgui_mw*   wid_button_getMw    (Cwid_button*self)
{
    Tvgui_mw* vmw=0;

    if ( self->but_vmw )
        vmw = self->but_vmw;
    else if ( self->but_vdw )
        vmw = vgui_dw_getMw(self->but_vdw);
    TEMP_ase_printf(vmw==0, "No main widget found",0);
    return vmw;
}
extern Tvgui_dw*   wid_button_getDw    (Cwid_button*self);

/*======================================================================*/
/*= The ornament button                                                =*/

static void         wid_button_orn_free(Twid_button*self)
{
    Twid_button_orn* proper = &self->but_orn;
    tool_free( proper->text );
}

static Twid_button* wid_button_orn_new(
    Tvgui_mw*vmw, Tvgui_dw*vdw,
    CstrStatic wn,  // widget resource name
    CstrBrief text)
{
    Twid_button* self  = tool_malloc_0(sizeof(*self));
    self->but_kind     = WID_BUT_KD_OrnLab;
    self->but_wn       = wn;
    self->but_vdw      = vdw ;
    self->but_vmw      = vmw ;
    self->but_orn_text = tool_strdup(text);
    return self;
}

extern Twid_button* widM_button_orn_new(Tvgui_mw*vmw,
                        CstrBrief text, CstrBrief width)
{ return wid_button_orn_new(vmw,NULL,wn_mainBtnOrn,text); }

extern Twid_button* widD_button_orn_new(Tvgui_dw*vdw,
                        CstrBrief text, CstrBrief width)
{ return wid_button_orn_new(NULL,vdw,wn_dialBtnOrn,text); }

/*======================================================================*/
/*= The standard button                                                =*/

static void         wid_button_std_free(Twid_button*self)
{
    Tvgui* gui = wid_button_getVGui(self);
    Twid_button_std* proper = &self->but_std;
    tool_free( proper->text );
    if ( (self->btn_option&WID_BTNOPT_PubPxm)==0 )
        gui_pixmap_free( gui, proper->pixmap );
}

static Twid_button* wid_button_std_new(
    Tvgui_mw*vmw, Tvgui_dw*vdw,
    CstrBrief text,
    int dx, int dy, TpixmapEGS pm,
    CstrBrief helpText,
    void* cb, void* ud,
    Twid_button_free  free,
    CstrStatic wn,  // widget resource name
    int ab)         // active buttons
{
    Twid_button* self  = tool_malloc_0(sizeof(*self));
    self->but_kind     = WID_BUT_KD_Standard;
    self->but_actButt  = ab;
    self->but_wn       = wn;
    self->but_helpText = tool_strdup(helpText);

    if ( ! PXMEGS_IsNone(pm) ) {
        self->but_dx         = dx;
        self->but_dy         = dy;
        self->but_std_pixmap = pm;
    } else {
        self->but_std_pixmap = PXMEGS_none;
        self->but_std_text   = tool_strdup(text);
    }

    self->but_vdw      = vdw ;
    self->but_vmw      = vmw ;
    self->but_cbUsr    = cb ;
    self->but_ud       = ud ;
    self->but_free     = free ;
    return self;
}

#define BUTSTD_newLab(fn,Ta1,Ta3,wn,ab,a1,a2) \
  extern Twid_button* fn(Ta1*w, \
     CstrBrief help, Twid_button_cb##Ta3 cb, void* ud, \
     Twid_button_free free, CstrBrief text, CstrBrief width) \
{ return wid_button_std_new(a1,a2,text,0,0,PXMEGS_none,help,cb,ud,free,wn,ab); }

BUTSTD_newLab(widM_button_std_newLab, Tvgui_mw,Mw,wn_mainBtnStd,WID_BUT_AB_1,w,NULL)
BUTSTD_newLab(widM_button_std_newLab3,Tvgui_mw,Mw,wn_mainBtnStd,WID_BUT_AB_123,w,NULL)
BUTSTD_newLab(widD_button_std_newLab, Tvgui_dw,Dw,wn_dialBtnStd,WID_BUT_AB_1,NULL,w)
BUTSTD_newLab(widD_button_std_newLab3,Tvgui_dw,Dw,wn_dialBtnStd,WID_BUT_AB_123,NULL,w)

#define BUTSTD_newPxm(fn,Ta1,Ta3,wn,ab,a1,a2) \
  extern Twid_button* fn(Ta1*w, \
     CstrBrief help, Twid_button_cb##Ta3 cb, void* ud, \
     Twid_button_free free, int dx, int dy, TpixmapEGS pm) \
{ return wid_button_std_new(a1,a2,0,dx,dy,pm,help,cb,ud,free,wn,ab); }

BUTSTD_newPxm(widM_button_std_newPxm, Tvgui_mw,Mw,wn_mainBtnStd,WID_BUT_AB_1,w,NULL)
BUTSTD_newPxm(widM_button_std_newPxm3,Tvgui_mw,Mw,wn_mainBtnStd,WID_BUT_AB_123,w,NULL)
BUTSTD_newPxm(widD_button_std_newPxm, Tvgui_dw,Dw,wn_dialBtnStd,WID_BUT_AB_1,NULL,w)
BUTSTD_newPxm(widD_button_std_newPxm3,Tvgui_dw,Dw,wn_dialBtnStd,WID_BUT_AB_123,NULL,w)

extern Twid_button* widD_button_std_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self = widD_button_std_newLab(
            vdw, helpText,cb,ud,free,text,width);
    self->but_widEGS = gui_button_std_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_std_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm)
{
    Twid_button* self = widD_button_std_newPxm(
            vdw, helpText,cb,ud,free,dx,dy,pm);
    self->but_widEGS = gui_button_std_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_std_nbfLab3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self = widD_button_std_newLab3(
            vdw, helpText,cb,ud,free,text,width);
    self->but_widEGS = gui_button_std_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_std_nbfPxm3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm)
{
    Twid_button* self = widD_button_std_newPxm3(
            vdw, helpText,cb,ud,free,dx,dy,pm);
    self->but_widEGS = gui_button_std_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}

extern void wid_button_std_cb(Twid_button*self, int bn)
{
    DBG0_ButE("butt=%p : butt=%d (%s)",self,bn,DBGBut2Str(bn));

    if ( bn==5 ) {
        bn=3;
        DBG0_ButM("butt=%p : butt=%d (%s) : roulette",self,bn,DBGBut2Str(bn));
    } else if ( bn==4 ) {
        bn=2;
        DBG0_ButM("butt=%p : butt=%d (%s) : roulette",self,bn,DBGBut2Str(bn));
    }

    if ( ! WID_BUT_AB_BnInFl(bn,self->but_actButt) ) {
        DBG1_ButL("butt=%p : butt=%d : not active (actButt=%d",
                self,bn,self->but_actButt);
        return;
    }

    if ( self->but_cbVgui ) {
        DBG2_ButM("butt=%p : butt=%d : call cbVgui",self,bn);
        self->but_cbVgui(self->but_panel,self,bn);
    } else if ( self->but_cbDw && self->but_vdw ) {
        DBG2_ButM("butt=%p : butt=%d : call cbDw",self,bn);
        self->but_cbDw(self->but_vdw,self->but_uid,self->but_ud);
    } else if ( self->but_cbMw && self->but_vmw ) {
        DBG2_ButM("butt=%p : butt=%d : call cbMw",self,bn);
        self->but_cbMw(self->but_vmw,self->but_uid,self->but_ud);
    } else {
        DBG2_ButM("butt=%p : butt=%d : no callback",self,bn);
    }
    DBG1_ButL("butt=%p : butt=%d",self,bn);
}

/*======================================================================*/
/*= The On/Off button                                                  =*/

static Twid_button* wid_button_onoff_newLab(
                Tvgui_mw*vmw,Tvgui_dw*vdw,
                CstrStatic wnOff, CstrStatic wnOn, int ab,
                CstrBrief helpText,
                void* cb, void* ud,
                Twid_button_free free,
                CstrBrief off, CstrBrief on)
{
    Twid_button* self    = tool_malloc_0(sizeof(*self));
    self->but_kind       = WID_BUT_KD_OnOff;
    self->but_actButt    = ab;
    self->but_wn         = wnOff;
    self->but_onoff_wnOn = wnOn;
    self->but_helpText   = tool_strdup(helpText);

    self->but_onoff_textOff = tool_strdup( off );
    self->but_onoff_textOn  = tool_strdup( on  );

    self->but_vdw      = vdw ;
    self->but_vmw      = vmw ;
    self->but_cbUsr    = cb ;
    self->but_ud       = ud ;
    self->but_free     = free ;
    return self;
}

static Twid_button* wid_button_onoff_newPxm(
                Tvgui_mw*vmw,Tvgui_dw*vdw,
                CstrStatic wnOff,CstrStatic wnOn,int ab,
                CstrBrief helpText,
                void* cb, void* ud,
                Twid_button_free free,
                int dx, int dy,
                TpixmapEGS off, TpixmapEGS on)
{
    Twid_button* self    = tool_malloc_0(sizeof(*self));
    self->but_kind       = WID_BUT_KD_OnOff;
    self->but_actButt    = ab;
    self->but_wn         = wnOff;
    self->but_onoff_wnOn = wnOn;
    self->but_helpText   = tool_strdup(helpText);
    self->but_dx         = dx;
    self->but_dy         = dy;

    self->but_onoff_usePixmap = 1;
    self->but_onoff_pixmapOff = off;
    self->but_onoff_pixmapOn  = on;

    self->but_vdw      = vdw ;
    self->but_vmw      = vmw ;
    self->but_cbUsr    = cb ;
    self->but_ud       = ud ;
    self->but_free     = free ;
    return self;
}

static void         wid_button_onoff_free(Twid_button*self)
{
    Tvgui* gui = wid_button_getVGui(self);
    if ( self->but_onoff_usePixmap ) {
        if ( (self->btn_option&WID_BTNOPT_PubPxm)==0 ) {
            gui_pixmap_free( gui, self->but_onoff_pixmapOff );
            gui_pixmap_free( gui, self->but_onoff_pixmapOn  );
        }
    } else {
        tool_free( self->but_onoff_textOff );
        tool_free( self->but_onoff_textOn  );
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* widM_button_onoff_newLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on)
{ return wid_button_onoff_newLab(vmw,NULL,wn_mainBtnOff,wn_mainBtnOn,
                        WID_BUT_AB_123, helpText,cb,ud,free,off,on); }
extern Twid_button* widM_button_onoff_newPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on)
{ return wid_button_onoff_newPxm(vmw,NULL,wn_mainBtnOff,wn_mainBtnOn,
                        WID_BUT_AB_123, helpText,cb,ud,free,dx,dy,off,on); }
extern Twid_button* widM_button_onoff_newIco(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon)
{
    Tvgui* gui      = vgui_mw_getVGui(vmw);
    TpixmapEGS  off = vgui_pixmap_create(gui,
        gui->vg_mainForegroundBtnOff, gui->vg_mainBackgroundBtnOff,icon);
    TpixmapEGS on  = vgui_pixmap_create(gui,
        gui->vg_mainForegroundBtnOn,  gui->vg_mainBackgroundBtnOn,icon);
    return wid_button_onoff_newPxm(vmw,NULL,wn_mainBtnOff,wn_mainBtnOn,WID_BUT_AB_123,
                                  helpText,cb,ud,free,dx,dy,off,on);
}
extern Twid_button* widM_button_onoff_nbfLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on)
{
    Twid_button* self;
    self = widM_button_onoff_newLab(vmw,helpText,cb,ud,free,off,on);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_mw_getWidEGS(vmw));
    return self;
}
extern Twid_button* widM_button_onoff_nbfPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on)
{
    Twid_button* self;
    self = widM_button_onoff_newPxm(vmw,helpText,cb,ud,free,dx,dy,off,on);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_mw_getWidEGS(vmw));
    return self;
}
extern Twid_button* widM_button_onoff_nbfIco(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon)
{
    Twid_button* self;
    self = widM_button_onoff_newIco(vmw,helpText,cb,ud,free,dx,dy,icon);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_mw_getWidEGS(vmw));
    return self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* widD_button_onoff_newLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on)
{ return wid_button_onoff_newLab(NULL,vdw,wn_dialBtnOff,wn_dialBtnOn,WID_BUT_AB_123,
                                  helpText,cb,ud,free,off,on); }
extern Twid_button* widD_button_onoff_newPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on)
{ return wid_button_onoff_newPxm(NULL,vdw,wn_dialBtnOff,wn_dialBtnOn,WID_BUT_AB_123,
                                  helpText,cb,ud,free,dx,dy,off,on); }
extern Twid_button* widD_button_onoff_newIco(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon)
{
    Tvgui* gui      = vgui_dw_getVGui(vdw);
    TpixmapEGS  off = vgui_pixmap_create(gui,
        gui->vg_mainForegroundBtnOff, gui->vg_mainBackgroundBtnOff,icon);
    TpixmapEGS  on = vgui_pixmap_create(gui,
        gui->vg_mainForegroundBtnOn,  gui->vg_mainBackgroundBtnOn,icon);
    return wid_button_onoff_newPxm(NULL,vdw,wn_dialBtnOff,wn_dialBtnOn,WID_BUT_AB_123,
                                  helpText,cb,ud,free,dx,dy,off,on);
}

extern Twid_button* widD_button_onoff_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on)
{
    Twid_button* self = widD_button_onoff_newLab(vdw, helpText,cb,ud,free,off,on);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_onoff_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on)
{
    Twid_button* self = widD_button_onoff_newPxm(
            vdw, helpText,cb,ud,free,dx,dy,off,on);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_onoff_nbfIco(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon)
{
    Twid_button* self = widD_button_onoff_newIco(
            vdw, helpText,cb,ud,free,dx,dy,icon);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Fold UnFold button                                                 =*/

// done in wid-button-fuf.c

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Check button                                                       =*/

extern Twid_button* widM_button_check_newLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self;
    self = widM_button_onoff_newLab(vmw, helpText,cb,ud,free,text,width);
    self->but_onoff_checkBtn = 1;
    return self;
}
extern Twid_button* widM_button_check_newPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm)
{
    Twid_button* self;
    self = widM_button_onoff_newPxm(vmw,helpText,cb,ud,free,dx,dy,pxm,PXMEGS_none);
    self->but_onoff_checkBtn = 1;
    return self;
}
extern Twid_button* widM_button_check_nbfLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self;
    self = widM_button_check_newLab(vmw, helpText,cb,ud,free,text,width);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_mw_getWidEGS(vmw));
    return self;
}
extern Twid_button* widM_button_check_nbfPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm)
{
    Twid_button* self;
    self = widM_button_check_newPxm(vmw, helpText,cb,ud,free,dx,dy,pxm);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_mw_getWidEGS(vmw));
    return self;
}

extern Twid_button* widD_button_check_newLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self;
    self = widD_button_onoff_newLab(vdw, helpText,cb,ud,free,text,width);
    self->but_onoff_checkBtn = 1;
    return self;
}
extern Twid_button* widD_button_check_newPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm)
{
    Twid_button* self;
    self = widD_button_onoff_newPxm(vdw, helpText,cb,ud,free,dx,dy,pxm,PXMEGS_none);
    self->but_onoff_checkBtn = 1;
    return self;
}
extern Twid_button* widD_button_check_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width)
{
    Twid_button* self;
    self = widD_button_check_newLab(vdw, helpText,cb,ud,free,text,width);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}
extern Twid_button* widD_button_check_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm)
{
    Twid_button* self;
    self = widD_button_check_newPxm(vdw, helpText,cb,ud,free,dx,dy,pxm);
    self->but_widEGS = gui_button_onoff_dbf(self,vgui_dw_getWidEGS(vdw));
    return self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void wid_button_onoff_cb(Twid_button*self,int bn)
{
    DBG0_ButE("butt=%p : butt=%d (%s)",self,bn,DBGBut2Str(bn));

    if ( self->but_panel==0 || (self->btn_option&WID_BTNOPT_NHBP)!=0 ) {
        DBG2_ButM("butt=%p : butt=%d : On/Off button out of a panel",self,bn);
#define btn_state self->but_onoff_state
        if ( btn_state==WID_BUT_STATE_Off ) {
            btn_state = WID_BUT_STATE_On;
            wid_button_setOn(self);
        } else {
            btn_state = WID_BUT_STATE_Off;
            wid_button_setOff(self);
        }
        if ( self->but_cbDw && self->but_vdw ) {
            DBG2_ButM("butt=%p : butt=%d : call cbDw",self,bn);
            self->but_cbDw(self->but_vdw,self->but_uid,self->but_ud);
        } else if ( self->but_cbMw && self->but_vmw ) {
            DBG2_ButM("butt=%p : butt=%d : call cbMw",self,bn);
            self->but_cbMw(self->but_vmw,self->but_uid,self->but_ud);
        }
        DBG1_ButL("butt=%p : butt=%d (%s)",self,bn,DBGBut2Str(bn));
        return;
    }

    Twid_buttonPanel* panel = self->but_panel;
#define ActiveButton  panel->vgbpm_activeButton

    // the selected button is already active --> nothing to do
    if ( ActiveButton == self ) {
        return;
    }

    // ask to the panel to perform the transition to new button.
    // A 0 return means that it does not accept.
    if ( panel->vgbpm_callback!=0 &&
         panel->vgbpm_callback(panel,self) == 0 ) {
        return;
    }

    // turn off active button and turn on next button
    wid_buttonPanel_active(panel,self);

    DBG1_ButL("butt=%p : butt=%d (%s)",self,bn,DBGBut2Str(bn));
}

/*======================================================================*/
/*= The button that groups buttons (hierarchical structure)            =*/

static Twid_button* wid_button_subpanel_free(Twid_button* self)
{ wid_buttonPanel_free( self->but_spanel_panel ); }

extern Twid_button*
            widM_button_subpanel_new(Twid_buttonPanel* panel)
{
    Twid_button* self      = tool_malloc_0(sizeof(*self));
    self->but_kind         = WID_BUT_KD_Panel;
    self->but_wn           = wn_mainPanel;
  //self->but_wm           = ???
  //self->but_dm           = ???
    self->but_spanel_panel = panel;
    panel->bp_subpanel     = 1;
    return self;
}

extern Twid_button*
            widD_button_subpanel_new(Twid_buttonPanel* panel)
{
    Twid_button* self      = tool_malloc_0(sizeof(*self));
    self->but_kind         = WID_BUT_KD_Panel;
    self->but_wn           = wn_dialPanel;
  //self->but_wm           = ???
  //self->but_dm           = ???
    self->but_spanel_panel = panel;
    panel->bp_subpanel     = 1;
    return self;
}

/*======================================================================*/
/*= The buttons                                                        =*/

extern void   wid_button_dbf(Twid_button*  self, Tegs_widget pw)
{
    if ( pw==0 ) {
        if ( self->but_vdw )
            pw = vgui_dw_getWidEGS(self->but_vdw);
    }
    TEMP_asf_printf(pw==0,
        "pw parameter and self->but_vdw can not been null both",0);
    if ( self->but_kind==WID_BUT_KD_OrnLab )
        self->but_widEGS = gui_button_orn_dbf(self,pw);
    else if ( self->but_kind==WID_BUT_KD_Standard )
        self->but_widEGS = gui_button_std_dbf(self,pw);
    else if ( self->but_kind==WID_BUT_KD_OnOff )
        self->but_widEGS = gui_button_onoff_dbf(self,pw);
    //else if ( self->but_kind==WID_BUT_KD_Panel )
    //    self->but_widEGS = gui_button_subpanel_dbf(self,pw);
    else
        PERM_spf_printf("%d is an unexpected kind",self->but_kind);
}

extern void   wid_button_delete (Twid_button* self)
{
    if ( self==0 ) return;

    tool_free( self->but_helpText );
    switch ( self->but_kind ) {
      case WID_BUT_KD_OrnLab   : wid_button_orn_free(self); break;
      case WID_BUT_KD_Standard : wid_button_std_free(self); break;
      case WID_BUT_KD_OnOff    : wid_button_onoff_free(self); break;
      case WID_BUT_KD_Panel    : wid_button_subpanel_free(self); break;
      default:
        PERM_spe_printf("%d is an unknown button kind.",self->but_kind);
    }
    if ( self->but_free )
        self->but_free( wid_button_getVGui(self), self->but_ud );
    free( self );
}

extern void   wid_button_chgText(Twid_button* self, CstrBrief str)
{
    switch ( self->but_kind ) {
      case WID_BUT_KD_Standard :
        tool_free( self->but_std_text );
        self->but_std_text = tool_strdup(str);
        wid_button_redraw(self);
        return;
      case WID_BUT_KD_OnOff    :
        if ( self->but_onoff_checkBtn!=0 ) {
            tool_free( self->but_onoff_textOff );
            self->but_onoff_textOff = tool_strdup(str);
            wid_button_redraw(self);
            return;
        }
      default:
        PERM_spe_printf("%d is an unknown button kind.",self->but_kind);
    }
}

extern void   wid_button_show   (Twid_button* self)
{ gui_widegs_show( self->but_widEGS); }
extern void   wid_button_unshow (Twid_button* self)
{ gui_widegs_unshow( self->but_widEGS); }

extern void   wid_button_enable (Twid_button* self)
{ gui_widegs_enable( self->but_widEGS); }
extern void   wid_button_disable(Twid_button* self)
{ gui_widegs_disable( self->but_widEGS); }

extern void  wid_button_setOn (Twid_button* self)
{
    int kind = self->but_kind;
    TEMP_ase_printf( kind!=WID_BUT_KD_OnOff,
            "unexpected call for self->but_kind=%d (ignored).",kind);
    if ( kind!=WID_BUT_KD_OnOff ) return;
    self->but_onoff_state = 1;
    gui_button_setOn(self);
}

extern void  wid_button_setOff(Twid_button* self)
{
    int kind = self->but_kind;
    TEMP_ase_printf( kind!=WID_BUT_KD_OnOff,
            "unexpected call for self->but_kind=%d (ignored).",kind);
    if ( kind!=WID_BUT_KD_OnOff ) return;
    self->but_onoff_state = 0;
    gui_button_setOff(self);
}
extern void  wid_button_redraw(Twid_button* self)
{
    DBG0_ButE("butt=%p kd=%7s",self,DBGButKd2Str(self));
    int kind = self->but_kind;
#define REDRAW_ReturnCond \
            kind!=WID_BUT_KD_Standard && \
           (kind!=WID_BUT_KD_OnOff || self->but_onoff_checkBtn==0)
    TEMP_ase_printf( REDRAW_ReturnCond,
            "unexpected call for self->but_kind=%d (ignored).",kind);
    if ( REDRAW_ReturnCond ) return;
    gui_button_redraw(self);
    DBG1_ButL("butt=%p kd=%7s",self,DBGButKd2Str(self));
}

extern void  wid_button_setSize(Twid_button* self, int dx, int dy)
{ gui_widegs_setSize(self->but_widEGS,dx,dy); }

/*======================================================================*/

extern void        wid_button_map_cb(Twid_button*self)
{
    DBG0_ButE("btn=%p : uid=%d",self,self->but_uid);
    if (  (self->btn_option&WID_BTNOPT_CCBWM)!=0 ) {
      if ( self->but_cbDw && self->but_vdw ) {
        DBG2_ButM("btn=%p : uid=%d : call cbDw",self,self->but_uid);
        self->but_cbDw(self->but_vdw,self->but_uid,self->but_ud);
      } else if ( self->but_cbMw && self->but_vmw ) {
        DBG2_ButM("btn=%p : uid=%d : call cbMw",self,self->but_uid);
        self->but_cbMw(self->but_vmw,self->but_uid,self->but_ud);
      } else {
        DBG2_ButM("btn=%p : uid=%d : no callback",self,self->but_uid);
      }
    }
    DBG2_ButL("btn=%p : uid=%d",self,self->but_uid);
}

/*======================================================================*/
