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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: l1l2base.c 277 2018-07-05 08:42:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/l1l2base.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*=                                                                    =*/
/*= Implementation of Tdpp_l1l2base class that is the top class for    =*/
/*= miscelaneous drawing parameters.                                   =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"
#include "yfig-mw.h"

#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-outdev.h"

#include "outdev/string-hl.h"

/*======================================================================*/

const char* dpp_none = "None";

extern void dpp_none_adjust(Tvgui*gui, int fi, double pts, int*dx, int*dy)
{
    int dxcr,dycr;
    gui_fonts_extents_DesUnit(gui,fi,pts,dpp_none,NULL,NULL,&dxcr,&dycr); 
    dycr += -(dycr*25)/100; // ~ space between lines
    if ( dxcr>*dx ) *dx = dxcr;
    if ( dycr>*dy ) *dy = dycr;
}

extern void dpp_none_draw  (Tvgui_outdev*vod, double x, double y, double dx, double dy)
{
    Tvgui*   gui    =  vod->v_gui;
    Toutdev* od     = &vod->v_od;
    double xpos = x;
    double ypos = y;
    int nasc,ndx;
    gui_fonts_extents_DesUnit(gui,gui->g_devFid,gui->g_devPtsStd,
            dpp_none,&nasc,NULL,&ndx,NULL);
    nasc = CONV_D_D2S(od,nasc);
    ndx  = CONV_D_D2S(od,ndx);
    if ( dx>ndx )
        xpos += (dx-ndx)/2 ;
    od_str_draw_LR(od,OD_GC_SysDraw,dpp_none,xpos,ypos+nasc);
}

extern void l1l2_none_adjust(Tvgui*gui, int*dx, int*dy)
{ dpp_none_adjust(gui,L1L2_FontId,L1L2_FontSizeV3,dx,dy); }

/*======================================================================*/

static void dpp_l1l2base_setsize(Tdpp_l1l2base* self, Cvgui* _gui,
        int dt_mindx, int dt_mindy, int dt_vsep)
{
    Tvgui* gui = (Tvgui*)_gui;
    Cstr  str;
    int   asc,dx1,dy1,dx2,dy2;
    if ( dt_vsep<=0 ) dt_vsep=3;

    int     fontid   = L1L2_FontId;      
    double  fontsize = L1L2_FontSize;    // 1/72 inch
    // left, middle and right space
    int     lsep     = LeftHSep;
    int     msep     = MidHSep;
    int     rsep     = RightHSep;

    /* get the size (ldx, ldy) for the left labels */
    str = self->ll_label1;
    gui_fonts_extents_DesUnit(gui,fontid,fontsize,str,&asc,NULL,&dx1,&dy1); 
    str = self->ll_label2;
    gui_fonts_extents_DesUnit(gui,fontid,fontsize,str,NULL,NULL,&dx2,&dy2); 
    int ldx = max2(dx1,dx2);
    int ldy = 0 + L1L2_VertShif + dy1 + L1L2_VertShif + dy2;

    /* get the size (wdx,wdy) of label + data */
    int wdx = lsep + ldx + msep + dt_mindx + rsep;
    int wdy = max2(ldy,dt_vsep+dt_mindy+dt_vsep);

    /* update the self positions and sizes */
    self->ll_dx   = wdx;
    self->ll_dy   = wdy;
    self->ll_xl   = lsep;
    self->ll_yl1  = 0 + L1L2_VertShif + asc;
    self->ll_yl2  = wdy - dy2 + L1L2_VertShif + asc;
    self->ll_xdt  = lsep + ldx + msep;
    self->ll_ydt  = (wdy - dt_mindy) / 2;
    self->ll_dxdt = dt_mindx;
    self->ll_dydt = dt_mindy;
}

extern int  dpp_l1l2base_getMaxV3Dy(Cvgui* gui, int v3sep)
{
    Tdpp_l1l2base self; memset(&self,0,sizeof(self));
    self.ll_label1 = "Apj"; // we hope that no string are highter
    self.ll_label2 = "Apj";
    dpp_l1l2base_setsize(&self,gui,0,0,0);
    if ( v3sep<=0 ) v3sep=3;
    return self.ll_dy - 2*v3sep;
}

static void dpp_l1l2base_setsize_lv(Tdpp_l1l2base* self,
        Cvgui* _gui, int dt_mindx, int dt_mindy)
{
    Tvgui* gui = (Tvgui*)_gui;
    Cstr  str;
    int   dx1,dy1;

    int     fontid   = L1L2_FontId;      
    int     fontsize = L1L2_FontSize;    // 1/72 inch
    // left and right space
    int     lsep     = LeftHSep;
    int     rsep     = RightHSep;
    int     vsep     = 3; // minimal space above and under data

    /* get the size (ldx, ldy) for the top label */
    int lasc,ldx,ldy;
    str = self->ll_label1;
    gui_fonts_extents_DesUnit(gui,fontid,fontsize,str,&lasc,NULL,&ldx,&ldy); 

    /* get the size (wdx,wdy) of label + data */
    int wdx = lsep + max2(ldx,dt_mindx) + rsep;
    int wdy = L1L2_VertShif+ldy+2*L1L2_VertShif+dt_mindy+vsep;

    /* update the self positions and sizes */
    self->ll_dx   = wdx;
    self->ll_dy   = wdy;
    self->ll_xl   = lsep;
    self->ll_yl1  = 0 + L1L2_VertShif ;
    self->ll_yl2  = 0; // not used
    self->ll_xdt  = lsep;
    self->ll_ydt  = self->ll_yl1 + ldy + 2*L1L2_VertShif;
    self->ll_dxdt = dt_mindx;
    self->ll_dydt = dt_mindy;
    self->ll_yl1 += lasc;
}

extern void dpp_l1l2base_init_V3VSep(Tdpp_l1l2base*self, Tvgui_mw* vmw,
        CstrStatic l1, CstrStatic l2, int dx, int dy, int v3_vsep)
{
    Tvgui* gui = vmw->gui;
    Ccolor *fg,*bg;
    vgui_wn_getColor(gui, wn_mainBtnStd, &fg, &bg);

    self->ll_label1 = l1;
    self->ll_label2 = l2;
    dpp_l1l2base_setsize(self,gui,dx,dy,v3_vsep);
//printf("INIT l1=%-15ss l2=%-15s x=%d/%d y=%d/%d xydt=%d/%d dxdydt=%d/%d pxm=%d/%d\n",
//self->ll_label1,self->ll_label2,self->ll_xl,self->ll_xl,
//self->ll_yl1,self->ll_yl2,
//self->ll_xdt,self->ll_ydt,
//self->ll_dxdt,self->ll_dydt,
//self->ll_dx,self->ll_dy
//);
    self->ll_dev = vgui_odpixmap_new(gui, bg, self->ll_dx,self->ll_dy, vmw);
}
extern void dpp_l1l2base_init(Tdpp_l1l2base*self, Tvgui_mw* vmw,
        CstrStatic l1, CstrStatic l2, int dx, int dy)
{ dpp_l1l2base_init_V3VSep(self,vmw,l1,l2,dx,dy,0); }

extern void dpp_l1l2base_init_lv(Tdpp_l1l2base*self, Tvgui_mw* vmw,
        CstrStatic l, int dx, int dy)
{
    Tvgui* gui = vmw->gui;
    Ccolor *fg,*bg;
    vgui_wn_getColor(gui, wn_mainBtnStd, &fg, &bg);

    self->ll_label1 = l;
    self->ll_label2 = 0;
    dpp_l1l2base_setsize_lv(self,gui,dx,dy);

    self->ll_dev = vgui_odpixmap_new(gui, bg,
            self->ll_dx,self->ll_dy, vmw);
}

/*======================================================================*/

static void dpp_l1l2base_cb( Twid_buttonPanel* p, Twid_button* b, int bn)
{
    Tyfig_mw*  ymw  = (Tyfig_mw*) p->vgbpm_vmw;
    Tdpp_l1l2base* self = b->but_ud;

#define DCB1_FMT DOPT_FMT"btn=%d : " 
#define DCB1_PAR DOPT_PDT(self),bn

    DBG0_DpE(DCB1_FMT,DCB1_PAR);
    if ( bn==1 && self->ll_cb1 ) {
            DBG0_DpE(DCB1_FMT"starting specialized CB cb1",DCB1_PAR);
            self->ll_cb1(self,ymw);
    } else if ( bn==2 && self->ll_dec ) {
        DBG0_DpE(DCB1_FMT"starting specialized CB inc",DCB1_PAR);
        self->ll_dec(self);
        dpp_l1l2base_redrawFull(self);
    } else if ( bn==3 && self->ll_inc ) {
        DBG0_DpE(DCB1_FMT"starting specialized CB dec",DCB1_PAR);
        self->ll_inc(self);
        dpp_l1l2base_redrawFull(self);
    } else {
        PERM_spe_printf("%d is an unexpected button number",bn);
    }

    DBG1_DpL(DCB1_FMT,DCB1_PAR);
}

static void dpp_l1l2base_btndelete(Tvgui*gui, void *ud)
{ dpp_l1l2base_delete( ud ); }

extern void dpp_l1l2base_delete(Tdpp_l1l2base*self)
{
        if ( self->ll_free )
            self->ll_free(self);
        dpp_l1l2base_free( self );
        free( self );
}

extern Twid_button* dpp_l1l2base_initBtnStdPXM3_V3VSep(Tdpp_l1l2base*self,
        Twid_buttonPanel* panel,
        CstrStatic l1, CstrStatic l2, int dx, int dy, int v3_vsep)
{
    Tvgui*       gui = panel->vgbpm_gui;
    Tvgui_mw*    ymw = panel->vgbpm_vmw;

    if ( l2!=0 )
        dpp_l1l2base_init_V3VSep(self,ymw,l1,l2,dx,dy,v3_vsep);
    else
        dpp_l1l2base_init_lv(self,ymw,l1,dx,dy);
//printf("NEW:00:%10s: ymw=%p od->ymw=%p\n",l1,ymw,self->ll_dev->vv_userdata);
    dpp_l1l2base_redrawFull(self);
    
    self->ll_btn = widM_button_std_newPxm3(ymw,
            NULL, // help text
            0, self,
            dpp_l1l2base_btndelete,
            self->ll_dx,self->ll_dy, self->ll_pixmap);
    self->ll_btn->btn_option |= WID_BTNOPT_PubPxm;
    self->ll_btn->but_cbVgui = dpp_l1l2base_cb;
    wid_buttonPanel_addButton(panel,self->ll_btn);
    return self->ll_btn;
}
extern Twid_button* dpp_l1l2base_initBtnStdPXM3(Tdpp_l1l2base*self,
        Twid_buttonPanel* panel,
        CstrStatic l1, CstrStatic l2, int dx, int dy)
{ return dpp_l1l2base_initBtnStdPXM3_V3VSep(self,panel,l1,l2,dx,dy,0); }

extern void dpp_l1l2base_free(Tdpp_l1l2base*self)
{
    vgui_odpixmap_delete( self->ll_dev );
}

extern void dpp_l1l2base_redrawBase(Tdpp_l1l2base*self)
{
    Tod_GCdef       gcd      = OD_GC_SysDraw;
    Tvgui_odpixmap* vdev     = self->ll_dev;
    Toutdev*        od       = &vdev->vv_od;
    Tvgui*          gui      = vdev->vv_gui;
    int             fontid   = L1L2_FontId;      
    double          fontsize = L1L2_FontSize;    // 1/72 inch

    Ccolor *fg,*bg;
    vgui_wn_getColor(gui, wn_mainBtnStd, &fg, &bg);

    DBG2_DpE("dev=%p %s %s",vdev,self->ll_label1,self->ll_label2);

  od_style_pushTop(od,gcd);
    double xl  = CONV_X_D2S(od,self->ll_xl);
    double yl1 = CONV_Y_D2S(od,self->ll_yl1);
    double yl2 = CONV_Y_D2S(od,self->ll_yl2);
    od_clear(od);
    od_str_setGC(od,gcd,fontid,fontsize,fg);
    od_str_draw_LR(od,gcd, self->ll_label1, xl, yl1);
    if ( self->ll_label2!=0 ) {
    od_str_draw_LR(od,gcd, self->ll_label2, xl, yl2);
    }
  od_style_pop(od,gcd);

    DBG2_DpL("dev=%p %s %s",vdev,self->ll_label1,self->ll_label2);
}

extern void dpp_l1l2base_redrawFull(Tdpp_l1l2base*self)
{
    dpp_l1l2base_redrawBase(self);
    if ( self->ll_redraw ) {
        Tvgui_odpixmap* vdev = self->ll_dev;
        Toutdev*        od   = &vdev->vv_od;
        self->ll_redraw(self, &self->ll_dev->vv_vdev,
                CONV_X_D2S(od,self->ll_xdt)  , CONV_Y_D2S(od,self->ll_ydt),
                CONV_D_D2S(od,self->ll_dxdt) , CONV_D_D2S(od,self->ll_dydt)
        );
    }
    if ( self->ll_btn )
        wid_button_redraw( self->ll_btn );
}

/*======================================================================*/

