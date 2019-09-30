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
 *      $Id: wid-buttonPanel.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-buttonPanel.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Lines, rows and matrix of buttons.                                 =*/
/*=                                                                    =*/
/*= See the vgui/wid-buttonPanel.h header for the type and function    =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

/*======================================================================*/
/*= abstract accessors                                                 =*/

extern Tvgui*      wid_buttonPanel_getVGui   (Cwid_buttonPanel*self)
{ return self->vgbpm_gui; }
extern Tegs_widget wid_buttonPanel_getWidEGS(Cwid_buttonPanel* self)
{ return self->bp_widEGSext; }
//extern Tegs_widget wid_buttonPanel_getPWidEGS(Cwid_buttonPanel*self);
extern Tvgui_mw*   wid_buttonPanel_getMw     (Cwid_buttonPanel*self)
{ return self->bp_vmw; }

/*======================================================================*/
/*= The panel itself                                                   =*/

static Twid_buttonPanel*
            widM_buttonPanel_new(
            Tvgui_mw*vmw, int type, // 'P' or 'V' or 'H'
            int bwidth,   int bheight)
{
    Twid_buttonPanel* ret = tool_malloc_0(sizeof(*ret));
    ret->vgbpm_gui          = vgui_mw_getVGui(vmw);
    ret->vgbpm_vmw          = vmw;
    ret->vgbpm_type         = type;
    ret->vgbpm_wn           = wn_mainPanel;
    ret->vgbpm_buttonWidth  = bwidth;
    ret->vgbpm_buttonHeight = bheight;
    return ret;
}

static Twid_buttonPanel*
            widD_buttonPanel_new(
            Tvgui_dw*vdw, int type, // 'P' or 'V' or 'H'
            int bwidth,   int bheight)
{
    Twid_buttonPanel* ret = tool_malloc_0(sizeof(*ret));
    ret->vgbpm_gui          = vgui_dw_getVGui(vdw);
    ret->vgbpm_vdw          = vdw;
    ret->vgbpm_type         = type;
    ret->vgbpm_wn           = wn_mainPanel;
    ret->vgbpm_buttonWidth  = bwidth;
    ret->vgbpm_buttonHeight = bheight;
    return ret;
}

static void wid_buttonPanel_build_I (Twid_buttonPanel* self,
                                     Tegs_widget parent)
{
    int i;
    if ( self->vgbpm_type=='H' ) {
        // fixe height of row ( max of button height).
        int maxdy=0;
        for (i = 0; i < self->vgbpm_buttonNb ; ++i) {
            Twid_button* button =  self->vgbpm_buttons[i];
            if ( button->but_dy > 0 && button->but_dy > maxdy ) {
                maxdy= button->but_dy;
            }
        }
        if ( maxdy>0 && maxdy>self->vgbpm_buttonHeight )
            self->vgbpm_buttonHeight = maxdy;
    }
    if ( self->bp_widEGSint==0 )
        self->bp_widEGSint = gui_buttonPanel_build(self,parent);
    if ( self->bp_widEGSext==0 )
        self->bp_widEGSext = self->bp_widEGSint;

    // create the buttons
    Tegs_widget widEGS = self->bp_widEGSint;
    for (i = 0; i < self->vgbpm_buttonNb ; ++i) {
        Twid_button* button =  self->vgbpm_buttons[i];
        if ( button->but_widEGS!=0 ) continue;
        switch ( button->but_kind ) {
          case WID_BUT_KD_OrnLab:
            button->but_widEGS = gui_button_orn_dbf(button,widEGS);
            break;
          case WID_BUT_KD_Standard:
            button->but_widEGS = gui_button_std_dbf(button,widEGS);
            break;
          case WID_BUT_KD_OnOff:
            button->but_widEGS = gui_button_onoff_dbf(button,widEGS);
            break;
          case WID_BUT_KD_Panel:
            wid_buttonPanel_build_I( button->but_spanel_panel, widEGS);
            wid_buttonPanel_finish( button->but_spanel_panel );
            button->but_widEGS = button->but_spanel_panel->bp_widEGSext;
            break;
        }
        TEMP_asf_printf(button->but_widEGS==0,
                "%d button kind not recognized.",button->but_kind);
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Interface                                                          =*/

extern Twid_buttonPanel* widM_buttonPanel_newRaCa(
                            Tvgui_mw*vmw, int bwidth, int bheight)
{ return widM_buttonPanel_new(vmw,'P',bwidth,bheight); }

extern Twid_buttonPanel* widM_buttonPanel_newR1Cn (
                            Tvgui_mw*vmw, int bwidth, int bheight)
{ return widM_buttonPanel_new(vmw,'H',bwidth,bheight); }

extern Twid_buttonPanel* widM_buttonPanel_newRnC1 (
                            Tvgui_mw*vmw, int bwidth, int bheight)
{ return widM_buttonPanel_new(vmw,'V',bwidth,bheight); }

extern Twid_buttonPanel* widD_buttonPanel_newRaCa(
                            Tvgui_dw*vmw, int bwidth, int bheight)
{ return widD_buttonPanel_new(vmw,'P',bwidth,bheight); }

extern Twid_buttonPanel* widD_buttonPanel_newR1Cn (
                            Tvgui_dw*vmw, int bwidth, int bheight)
{ return widD_buttonPanel_new(vmw,'H',bwidth,bheight); }

extern Twid_buttonPanel* widD_buttonPanel_newRnC1 (
                            Tvgui_dw*vmw, int bwidth, int bheight)
{ return widD_buttonPanel_new(vmw,'V',bwidth,bheight); }

extern void wid_buttonPanel_build  (Twid_buttonPanel* self, Tegs_widget pw)
{
    wid_buttonPanel_build_I(self,pw);
}
extern void wid_buttonPanel_finish(Twid_buttonPanel* self)
{
    if ( self->vgbpm_type=='V' ) {
        gui_buttonPanel_finishRnC1(self);
    } else if ( self->vgbpm_type=='H' ) {
        gui_buttonPanel_finishR1Cn(self);
    } else if ( self->vgbpm_type=='P' ) {
        gui_buttonPanel_finishRaCa(self);
    } else {
        TEMP_spf_printf( "0x%02x panel type not recognized.",
                self->vgbpm_type);
    }
}
extern void wid_buttonPanel_dbf  (Twid_buttonPanel* self, Tegs_widget pw)
{
    Tegs_widget parent
        = pw              ? pw 
        : self->vgbpm_vmw ? vgui_mw_getWidEGS(self->vgbpm_vmw)
        :                   vgui_dw_getWidEGS(self->vgbpm_vdw);
    wid_buttonPanel_build_I(self,parent);
    wid_buttonPanel_finish(self);
}

extern void wid_buttonPanel_free (Twid_buttonPanel* self)
{
    int i;
    for ( i=0 ; i<self->bp_btnNb ; i+=1)
        wid_button_delete( self->bp_btns[i] );
    free( self );
}

extern void wid_buttonPanel_enable (Twid_buttonPanel* self)
{ gui_widegs_enable(self->bp_widEGSext); }
extern void wid_buttonPanel_disable(Twid_buttonPanel* self)
{ gui_widegs_disable(self->bp_widEGSext); }

extern Twid_button* wid_buttonPanel_addDecorativeLabel(
        Twid_buttonPanel* panel, CstrBrief label)
{
    Twid_button* ret;
    if ( panel->vgbpm_vdw )
        ret = widD_button_orn_new(panel->vgbpm_vdw,label,NULL);
    else
        ret = widM_button_orn_new(panel->vgbpm_vmw,label,NULL);
    wid_buttonPanel_addButton(panel,ret);
    return ret;
}

extern void wid_buttonPanel_addButton(
        Twid_buttonPanel* panel, Twid_button* button)
{
    button->but_panel = panel;
    panel->vgbpm_buttons[panel->vgbpm_buttonNb++] = button;
}

extern void wid_buttonPanel_active(Twid_buttonPanel* self,
        Twid_button* button)
{
    if ( self->vgbpm_activeButton!=0 ) {
        gui_button_setOff(self->vgbpm_activeButton);
        self->vgbpm_activeButton=0;
    }

    if ( button!=0 ) {
        self->vgbpm_activeButton = button;
        gui_button_setOn(button);
    }
}

extern Twid_button* wid_buttonPanel_getBtnByUid(Cwid_buttonPanel* self, int uid)
{
    int i;
    for ( i=0 ; i<self->bp_btnNb ; i+=1) {
        if ( self->bp_btns[i]->but_uid==uid )
            return self->bp_btns[i];
    }
    return NULL;
}

/*======================================================================*/
/*= tools for the external graphic system                              =*/

extern int wid_buttonPanel_getHeight(Cwid_buttonPanel*panel, int rowNb)
{
    int borderWidth = panel->vgbpm_buttonBorder;
    int HVsep       = panel->vgbpm_buttonHVsep;
    int labelHeight = panel->vgbpm_labelHeight;
    return HVsep + 2 * ( labelHeight+HVsep )
#if 0
                 + (rowNb-2) * (   panel->vgbpm_buttonHeight
#else
                 + (rowNb) * (   panel->vgbpm_buttonHeight
#endif
                                 + 2*borderWidth
                                 + HVsep);
}

extern int wid_buttonPanel_getWidth(Cwid_buttonPanel*panel, int colNb)
{
    int borderWidth = panel->vgbpm_buttonBorder;
    int HVsep       = panel->vgbpm_buttonHVsep;
    return HVsep + colNb * (   panel->vgbpm_buttonWidth
                             + 2*borderWidth
                             + HVsep
                            );
}

extern int wid_buttonPanel_getLayout(Cwid_buttonPanel*panel,int colNb,
        int rows[VGUI_BP_BoutonNbMax],     int cols[VGUI_BP_BoutonNbMax],
        int colspans[VGUI_BP_BoutonNbMax])
{
    int dummy[VGUI_BP_BoutonNbMax];
    if ( rows    ==0 ) rows     = dummy;
    if ( cols    ==0 ) cols     = dummy;
    if ( colspans==0 ) colspans = dummy;


    Twid_button* b;
    int borderWidth = panel->vgbpm_buttonBorder;
    int HVsep       = panel->vgbpm_buttonHVsep;
    int maxDx = wid_buttonPanel_getWidth(panel,colNb);
    int colDx = panel->vgbpm_buttonWidth+2*borderWidth+HVsep;
    int i,bDx;

    int curDx=HVsep, row=0, col=0;
    for (i=0 ; i!=panel->vgbpm_buttonNb ; i+=1 ) {
        b = panel->vgbpm_buttons[i];
        if ( b->but_kind==WID_BUT_KD_OrnLab ) {
            if (col!=0) row+=1;
            rows[i] = row;
            cols[i] = 0;
            colspans[i] = colNb;
//printf("%s:%20s:%2d: row=%d col=%d span=%d\n",__PRETTY_FUNCTION__,"label",i,rows[i],cols[i],colspans[i]);
            row+=1; col=0;
        } else {
            bDx = b->but_dx + 2*borderWidth + HVsep;
            if ( !((curDx+bDx)<=maxDx) ) {
                row+=1; col=0;
            }
//printf("%s:%20s:%2d: row=%d col=%d codDx=%d\n",__PRETTY_FUNCTION__,"button",i,row,col,colDx);
            rows[i] = row; cols[i]=col;
            colspans[i] = (bDx+colDx-1)/colDx;
//printf("%s:%20s:%2d: row=%d col=%d span=%d\n",__PRETTY_FUNCTION__,"button",i,rows[i],cols[i],colspans[i]);
            if ( colspans[i] > colNb ) colspans[i]=colNb;
            col+=colspans[i];
            if (col>=colNb) {
                row+=1; col=0;
            }
        }
    }
    return row+(col!=0);
}

/*======================================================================*/
