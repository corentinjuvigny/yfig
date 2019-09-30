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
 *$Software: YFIG-XT (VGUI driver to Xt and Xawd or Xaw3d)
 *      $Id: widxt-button.c 130 2018-05-11 08:13:40Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-button.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons (EGS driver).                                =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for the type and function         =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt/guixt-create.h"

#include "vgui/wid-button.h"
#include "vgui/vgui-accelkey.h"
#include "vgui/wid-egs.h"

#define BMP
//#define BGPXM

/*======================================================================*/

static void gui_button_addHelp(Widget widget, Twid_button* b)
{
    gui_widegs_addTipAA(widget, b->but_helpText,
                        b->but_accelKey0,b->but_accelKey1);
}

static Widget gui_button_build(Twid_button* b, Tegs_widget parent,
        Cstr text, TpixmapEGS pixmap, XtEventHandler cb,
        int wcmb // With CheckMark Box
)
{
    Widget widget;
    //Twid_buttonPanel* panel  = b->but_panel;
    Tvgui*            gui    = wid_button_getVGui(b);

    int  dx   = b->but_dx;
    int  dy   = b->but_dy;
    Cstr wn   = b->but_wn;
    //widget = widget_button_create(gui,panel->vgbpm_widEGS,wn,
    if ( wcmb )
        widget = create_buttonWLPXM(gui,parent,wn,
            0,dx,dy, EGSCOLOR_default, EGSCOLOR_default,
            text, NULL, NULL,NULL, NULL, 0);
    else
        widget = create_button(gui,parent,wn,
            0,dx,dy, EGSCOLOR_default, EGSCOLOR_default,
            text, NULL, NULL,NULL, NULL);
    if ( ! PXMEGS_IsNone(pixmap) ) {
#ifdef  BMP
        XtVaSetValues(widget, XtNbitmap, PXMEGS2X11(pixmap), NULL);
#elif defined(BGPXM)
        XtVaSetValues(widget, XtNbackgroundPixmap, PXMEGS2X11(pixmap), NULL);
#else
        if ( dx<16 || dy<16 ) {
#if 1
            XtVaSetValues(widget, XtNbitmap, PXMEGS2X11(pixmap), NULL);
            //XtVaSetValues(widget, XtNbackgroundPixmap, PXMEGS2X11(pixmap), NULL);
#else
            //XtVaSetValues(widget, XtNborderWidth, (Dimension)8, NULL);
            XtVaSetValues(widget, XtNleftBitmap, PXMEGS2X11(pixmap), NULL);
#endif
        } else
            XtVaSetValues(widget, XtNbackgroundPixmap, PXMEGS2X11(pixmap), NULL);
#endif
    }

    XtAddEventHandler(widget, ButtonPressMask, False,
            cb, (XtPointer) b);
    if ( (b->btn_option&WID_BTNOPT_CCBWM)!=0 )
        widget_cb_addU(widget, b, YFIG_EVENT_Map, wid_button_map_cb, NULL);

    gui_button_addHelp(widget,b);

    return widget;
}

/*======================================================================*/
/*= The ornament button                                                =*/

extern Tegs_widget gui_button_orn_dbf(Twid_button*  b, Tegs_widget pw)
{
    Tvgui* gui  = wid_button_getVGui(b);
    Widget widget;

    Cstr wn   = b->but_wn;
    int  dx   = b->but_dx;
    int  dy   = b->but_dy;
    Cstr text = b->but_std_text;
    widget = create_label(gui,pw,wn,
            0,dx,dy,EGSCOLOR_default,EGSCOLOR_default,text);
    return widget;
}

/*======================================================================*/
/*= The standard button                                                =*/

static void gui_button_std_cb(Widget widget,
        XtPointer _button, XEvent *event, Boolean *continue_to_dispatch);

extern Tegs_widget gui_button_std_dbf(Twid_button*  b, Tegs_widget pw)
{
    Tvgui* gui  = wid_button_getVGui(b);
    Widget widget;

    Cstr           text = b->but_std_text;
    TpixmapEGS     pm   = b->but_std_pixmap;
    XtEventHandler cb   = gui_button_std_cb;

    widget = gui_button_build(b,pw,text,pm,cb,0);
    return widget;
}

static void gui_button_std_cb(Widget widget,
    XtPointer _button, XEvent *event, Boolean *continue_to_dispatch)
{
    XButtonEvent xbutton = event->xbutton;
    int          buttnum = xbutton.button;
    Twid_button* button  = (Twid_button*) _button;
    //Twid_buttonPanel*        panel   = button->but_panel;

    DBG0_ButE("butt=%p : bn=%d (%s)",button,buttnum,
      buttnum==1?"left":buttnum==2?"middle":buttnum==3?"right":"???");

    wid_button_std_cb(button,buttnum);

    DBG1_ButL("butt=%p : bn=%d",button,buttnum);
}

/*======================================================================*/
/*= The On/Off button                                                  =*/

extern void gui_button_onoff_cb(Widget widget, XtPointer button,
        XEvent *event, Boolean *continue_to_dispatch);

extern Tegs_widget gui_button_onoff_dbf(Twid_button*  b, Tegs_widget pw)
{
    Tvgui* gui  = wid_button_getVGui(b);
    Widget widget;

    Cstr           text = b->but_onoff_usePixmap ? 0 : b->but_onoff_textOff;
    TpixmapEGS     pm   = b->but_onoff_usePixmap ? b->but_onoff_pixmapOff
                                                 : PXMEGS_none;
    XtEventHandler cb   = gui_button_onoff_cb;

    widget = gui_button_build(b,pw,text,pm,cb,1);
    if ( b->but_onoff_checkBtn==0 && b->but_onoff_usePixmap!=0 ) {
        XtVaSetValues(widget,
                XtNinternalWidth,  (Dimension)0,
                XtNinternalHeight, (Dimension)0,
                NULL);
    } else if ( b->but_onoff_checkBtn!=0 ) {
        XtVaSetValues(widget,
                XtNinternalWidth,  (Dimension)5,
                //XtNinternalHeight, (Dimension)2,
                NULL);
        b->but_widEGS = widget;
        gui_button_setOff(b);
    }
    return widget;
}

// called with very restricted parameter, see the end of
// the gui_buttonPanel_accel function.
extern void gui_button_onoff_cb(Widget widget, XtPointer _button,
        XEvent *event, Boolean *continue_to_dispatch)
{
    // we are only sensitive too button click
    if ( event->type!=ButtonPress ) return;

    XButtonEvent*     xbutton = &event->xbutton;
    Twid_button*      button  = (Twid_button*) _button;
    int               buttnum = xbutton->button;

    DBG0_ButE("butt=%p : bn=%d (%s)",
            button,buttnum, DBGBut2Str(buttnum));

    wid_button_onoff_cb(button,buttnum);

    DBG1_ButL("butt=%p : bn=%d (%s)",
            button,buttnum, DBGBut2Str(buttnum));
}

/*======================================================================*/
/*= Button operation                                                   =*/

static void  gui_button_setOnOff_CheckBox(Twid_button* self, int on)
{
    Tvgui*     gui  = wid_button_getVGui(self);
    Tgui_main* main = gui->g_main;

    if ( self->but_onoff_usePixmap ) {
        PERM_spe_NYI(0);
    } else if (self->but_onoff_textOff) {
        XtVaSetValues(self->but_widEGS,
            XtNlabel, self->but_onoff_textOff, NULL);
    }
    Pixmap pxm = on ? main->m_bmCmBoxOnSm : main->m_bmCmBoxOffSm;
    XtVaSetValues( self->but_widEGS, XtNleftBitmap,pxm, NULL);
}

extern void  gui_button_setOn (Twid_button* button)
{
    TEMP_asf_printf(button->but_kind!=WID_BUT_KD_OnOff,
            "not an On/Off button",0);
    if ( button->but_onoff_checkBtn!=0 ) {
        gui_button_setOnOff_CheckBox(button,1);
        return;
    }
    if ( button->but_onoff_usePixmap ) {
        int  dx   = button->but_dx;
        int  dy   = button->but_dy;
#ifdef  BMP
        XtVaSetValues(button->but_widEGS, XtNbitmap, button->but_onoff_pixmapOn, NULL);
#elif defined(BGPXM)
        XtVaSetValues(button->but_widEGS, XtNbackgroundPixmap, button->but_onoff_pixmapOn, NULL);
#else
        if ( dx<16 || dy<16 ) {
            XtVaSetValues(button->but_widEGS,
                XtNbitmap, button->but_onoff_pixmapOn, NULL);
        } else {
            XtVaSetValues(button->but_widEGS,
                XtNbackgroundPixmap, button->but_onoff_pixmapOn, NULL);
            //XtVaSetValues(widget, XtNbackgroundPixmap, (Pixmap)pixmap, NULL);
        }
#endif
    } else {
        Tvgui*     gui = wid_button_getVGui(button);
        Tegs_color bg,fg; 
        vgui_wn_getColorEGS(gui,button->but_onoff_wnOn,&fg,&bg);
        XtVaSetValues(button->but_widEGS,
                XtNbackground,bg, XtNforeground,fg, NULL);
        if ( button->but_onoff_textOn!=0 )
            XtVaSetValues(button->but_widEGS,
                XtNlabel, button->but_onoff_textOn, NULL);
        else if ( button->but_onoff_textOff!=0 ) 
            XtVaSetValues(button->but_widEGS,
                XtNlabel, button->but_onoff_textOff, NULL);
    }
}

extern void  gui_button_setOff(Twid_button* button)
{
    TEMP_asf_printf(button->but_kind!=WID_BUT_KD_OnOff,
            "not an On/Off button",0);
    if ( button->but_onoff_checkBtn!=0 ) {
        gui_button_setOnOff_CheckBox(button,0);
        return;
    }
    if ( button->but_onoff_usePixmap ) {
        //XtVaSetValues(button->but_widEGS,
            //XtNbackgroundPixmap, button->but_onoff_pixmapOff, NULL);
#ifdef  BMP
        XtVaSetValues(button->but_widEGS, XtNbitmap, button->but_onoff_pixmapOff, NULL);
#elif defined(BGPXM)
        XtVaSetValues(button->but_widEGS, XtNbackgroundPixmap, button->but_onoff_pixmapOff, NULL);
#else
        int  dx   = button->but_dx;
        int  dy   = button->but_dy;
        if ( dx<16 || dy<16 ) {
            XtVaSetValues(button->but_widEGS,
                XtNbitmap, button->but_onoff_pixmapOff, NULL);
        } else {
            XtVaSetValues(button->but_widEGS,
                XtNbackgroundPixmap, button->but_onoff_pixmapOff, NULL);
            //XtVaSetValues(widget, XtNbackgroundPixmap, (Pixmap)pixmap, NULL);
        }
#endif
    } else {
        Tvgui*     gui = wid_button_getVGui(button);
        Tegs_color bg,fg; 
        vgui_wn_getColorEGS(gui,button->but_onoff_wnOff,&fg,&bg);
        XtVaSetValues(button->but_widEGS,
                XtNbackground,bg, XtNforeground,fg, NULL);
        if ( button->but_onoff_textOff!=0 )
            XtVaSetValues(button->but_widEGS,
                XtNlabel, button->but_onoff_textOff, NULL);
    }
}

extern void  gui_button_redraw(Twid_button* self)
{
    DBG0_ButE("butt=%p kd=%7s",self,DBGButKd2Str(self));
    int kind = self->but_kind;
    TEMP_ase_printf(
            kind!=WID_BUT_KD_Standard &&
          !(kind==WID_BUT_KD_OnOff &&  self->but_onoff_checkBtn!=0),
            "%d is an unexpected button kind",kind);
    if ( kind==WID_BUT_KD_Standard ) {
        if ( ! PXMEGS_IsNone(self->but_std_pixmap) ) {
#if USE_XAW==1
            DBG1_ButM("butt=%p : kd=%7s : clear pixmap",self,DBGButKd2Str(self));
            XtVaSetValues(self->but_widEGS,
                XtNbackgroundPixmap, (Pixmap)0, NULL);
#endif
            DBG1_ButM("butt=%p : kd=%7s : change pixmap",self,DBGButKd2Str(self));
            XtVaSetValues(self->but_widEGS,
                XtNbackgroundPixmap, PXMEGS2X11(self->but_std_pixmap), NULL);
        }
    } else if ( kind==WID_BUT_KD_OnOff &&  self->but_onoff_checkBtn!=0 ) {
        if ( self->but_onoff_usePixmap ) {
            PERM_spe_NYI(0);
        } else if (self->but_onoff_textOff) {
            XtVaSetValues(self->but_widEGS,
                XtNlabel, self->but_onoff_textOff, NULL);
        }

    }
    DBG1_ButL("butt=%p kd=%7s",self,DBGButKd2Str(self));
}

/*======================================================================*/
