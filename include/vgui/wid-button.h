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
 *      $Id: wid-button.h 339 2018-11-06 14:08:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-button.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons.                                             =*/
/*=                                                                    =*/
/*= All the button kinds are grouped under the single Twid_button      =*/
/*= type. It offers an abstract view of the buttons.                   =*/
/*=                                                                    =*/
/*= A button is attached to either a main widget or a dialog widget.   =*/
/*= If it is attached to a main widget, its callback prototype is:     =*/  
/*=  void callback( Tvgui_mw* vmw, void* ud);                          =*/
/*= If it is attached to a dialog widget, its callback prototype is:   =*/  
/*=  void callback( Tvgui_dw* vdw, void* ud);                          =*/
/*=                                                                    =*/
/*= The Twid_button type integrates the following button kinds:        =*/
/*=                                                                    =*/
/*=  - OrnLab:                                                         =*/
/*=     The ORNament LABel is just a label. Its rule is to separate    =*/
/*=     other buttons. It has only sens in a button panel.             =*/
/*=                                                                    =*/
/*=  - Standard:                                                       =*/
/*=     The standard button is a button that triggers a callback when  =*/
/*=     it is clicked.                                                 =*/
/*=     It render a label and/or a pixmap.                             =*/
/*=                                                                    =*/
/*=  - OnOff:                                                          =*/
/*=     The On/Off button is a button with 2 states (On/Off).          =*/
/*=     It looks differently depending of its state.                   =*/ 
/*=     At start time the On/Off button is off.                        =*/
/*=     When it is in Off state the following behavior occurs:         =*/
/*=       - on the first click, it becomes On, it is rendered on the   =*/
/*=         screen as On, it triggers the callback.                    =*/
/*=       - on the second click, it becomes Off again, it is rendered  =*/
/*=         as Off. The callback is triggered again.                   =*/
/*=                                                                    =*/
/*=  - Panel:                                                          =*/
/*=     The Panel button is not a button. It is a container based on   =*/
/**     Twid_modePanel tyoe.                                           =*/
/*=     It allows to make hierarchy of button groups.                  =*/
/*=     See the wid-buttonPanel.h header for information about the     =*/
/*=     button panel.                                                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_BUTTON_H
#define FILE_WID_BUTTON_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================*/
/*= macros and base types                                              =*/

// the different buttons
#define WID_BUT_KD_OrnLab                1 // ornament button, no action,
                                           // just decorative
#define WID_BUT_KD_Standard              2 // active an action when pressed
#define WID_BUT_KD_OnOff                 3 // Is either on or off
#define WID_BUT_KD_Panel                 4 // not a button but a panel 
                                           // grouping buttons

// state of On/Off button
#define WID_BUT_STATE_On                1 
#define WID_BUT_STATE_Off               0 
// #define WID_BUT_STATE_None              0  // no state

/* Define the mouse buttons that are active */
#define WID_BUT_AB_1                    1 
#define WID_BUT_AB_2                    2 
#define WID_BUT_AB_3                    4 
#define WID_BUT_AB_123                  7 
#define WID_BUT_AB_BnInFl(n,fl) ((1<<((n)-1))&(fl)) 

/* Some option */
#define WID_BTNOPT_CCBWM   0x0001 // Call CallBack When Mapped.
#define WID_BTNOPT_NHBP    0x0002 // Not Handle By Panel (onoff button only)
#define WID_BTNOPT_PubPxm  0x0004 // PUBlic PiXMap (destructor does not free pixmap)



typedef struct _Twid_button_orn     Twid_button_orn;
typedef const   Twid_button_orn     Cwid_button_orn;
typedef struct _Twid_button_std     Twid_button_std;
typedef const   Twid_button_std     Cwid_button_std;
typedef struct _Twid_button_onOff   Twid_button_onOff;
typedef const   Twid_button_onOff   Cwid_button_onOff;
typedef struct _Twid_button_spanel  Twid_button_spanel;
typedef const   Twid_button_spanel  Cwid_button_spanel;

/**
 * When a b button widget of a p panel widget is clicked with the n th
 * mouse button the function "cbVGUI(p,b,n)" is performed.
 * Note: Usually n is 1 for the left button, 2 for the middle button and
 * 3 for the right button.
**/
typedef void (*Twid_button_cbVgui)(Twid_buttonPanel*  panel,
                                   Twid_button*       button,
                                   int                buttonNum);

/*======================================================================*/
/*= The ornament button                                                =*/

struct _Twid_button_orn {
    TstrEaten    text;    // written centered in button
#define but_orn        u.orn
#define but_orn_text   but_orn.text
};

extern Twid_button* widM_button_orn_new(Tvgui_mw*vmw,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_orn_new(Tvgui_dw*vdw,
                        CstrBrief text, CstrBrief width);

/*======================================================================*/
/*= The standard button                                                =*/

struct _Twid_button_std {
    TstrEaten    text;    // written centered in button
    TpixmapEGS   pixmap;  // drawn centered in button (not freed)
#define but_std        u.std
#define but_std_text   but_std.text
#define but_std_pixmap but_std.pixmap
};


extern Twid_button* widM_button_std_newLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widM_button_std_newPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);
extern Twid_button* widM_button_std_newLab3(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widM_button_std_newPxm3(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);

extern Twid_button* widD_button_std_newLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_std_newPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);
extern Twid_button* widD_button_std_newLab3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_std_newPxm3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);

extern Twid_button* widD_button_std_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_std_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);
extern Twid_button* widD_button_std_nbfLab3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_std_nbfPxm3(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pm);

/**
 * The EGS must call this callback when a standard button is clicked.
 * self is the clicked button and bn is the mouse button with 1 for
 * the left mouse button, 2 and 3 for the middle and right ones.
 * This function does the checkings such as if the button is active,
 * so the EGS can call it directly. 
**/
extern void wid_button_std_cb(Twid_button*self, int bn);

/*======================================================================*/
/*= The On/Off button                                                  =*/

struct _Twid_button_onOff {
    char        state;      // state of button, see WID_BUT_STATE_...
    char        usePixmap;  // 0: draw the u.string strings
                            // 1: draw the u.pixmap pixmaps
    char        isCheckBtn; // 0: btn is a standard on/off button
                            // 1: btn is a check button (WID_BUT_STATE_On
                            //    means checked).
    CstrStatic  wnOn;       // widget resource for on state
    union {
      struct {
        TstrEaten textOff;      // drawn when the button state is off
        TstrEaten textOn;       // drawn when the button state is on
      } string;
      struct {
         TpixmapEGS  pixmapOff; // drawn when the button state is off
         TpixmapEGS  pixmapOn;  // drawn when the button state is on
      } pixmap;
    } u;
#define but_onoff           u.onOff
#define but_onoff_state     but_onoff.state
#define but_onoff_usePixmap but_onoff.usePixmap
#define but_onoff_checkBtn  but_onoff.isCheckBtn
#define but_onoff_wnOff     but_wn
#define but_onoff_wnOn      but_onoff.wnOn
#define but_onoff_textOff   but_onoff.u.string.textOff
#define but_onoff_textOn    but_onoff.u.string.textOn
#define but_onoff_pixmapOff but_onoff.u.pixmap.pixmapOff
#define but_onoff_pixmapOn  but_onoff.u.pixmap.pixmapOn
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* widM_button_onoff_newLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on);
extern Twid_button* widM_button_onoff_newPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);
extern Twid_button* widM_button_onoff_newIco(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon);
extern Twid_button* widM_button_onoff_nbfLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on);
extern Twid_button* widM_button_onoff_nbfPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);
extern Twid_button* widM_button_onoff_nbfIco(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* widD_button_onoff_newLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on);
extern Twid_button* widD_button_onoff_newPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);
extern Twid_button* widD_button_onoff_newIco(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon);
extern Twid_button* widD_button_onoff_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief off, CstrBrief on);
extern Twid_button* widD_button_onoff_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);
extern Twid_button* widD_button_onoff_nbfIco(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, Cvgui_icon* icon);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Fold UnFold button                                                 =*/

extern Twid_button* widD_button_fuf_nbflab(Tvgui_dw*vdw,
                        CstrBrief off, CstrBrief on);
extern Twid_button* widD_button_fuf_nbfPxm(Tvgui_dw*vdw,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);
extern Twid_button* widD_button_fuf_nbfPXM(Tvgui_dw*vdw);
// map/unmap all the widget  specified in ...
extern Twid_button* widD_button_fuf_addWidToCtl(Twid_button*self, ...);
// map/unmap all the child of pw except of those specified in ...
extern Twid_button* widD_button_fuf_addWidToIgn(Twid_button*self, 
                                                Tegs_widget pw, ...);

extern Twid_button* widD_button_fuf_setFolded(Twid_button*self);
extern Twid_button* widD_button_fuf_setUnFolded(Twid_button*self);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Check button                                                       =*/

extern Twid_button* widM_button_check_newLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widM_button_check_newPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm);
extern Twid_button* widM_button_check_nbfLab(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widM_button_check_nbfPxm(Tvgui_mw*vmw,
                        CstrBrief helpText,
                        Twid_button_cbMw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm);

extern Twid_button* widD_button_check_newLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_check_newPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm);
extern Twid_button* widD_button_check_nbfLab(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        CstrBrief text, CstrBrief width);
extern Twid_button* widD_button_check_nbfPxm(Tvgui_dw*vdw,
                        CstrBrief helpText,
                        Twid_button_cbDw cb, void* ud,
                        Twid_button_free free,
                        int dx, int dy, TpixmapEGS pxm);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/**
 * The EGS must call this callback when the iself On/off button is clicked.
 * This function does the checkings such as if the button is active,
 * so the EGS can call it directly. 
**/
extern void wid_button_onoff_cb(Twid_button*self, int bn);

/*======================================================================*/
/*= The button that groups buttons (hierarchical structure)            =*/

struct _Twid_button_spanel {
    Twid_buttonPanel* panel;
#define but_spanel_panel u.spanel.panel
};

extern Twid_button*
            widM_button_subpanel_new(Twid_buttonPanel* panel);
extern Twid_button*
            widD_button_subpanel_new(Twid_buttonPanel* panel);

/*======================================================================*/
/*= The buttons                                                        =*/

struct _Twid_button {
    char         but_kind;        // the button kind, see VGUI_BP_KIND_xxx
    char         but_actButt;     // active buttons (OR of VGUI_BP_AB_1/2/3)
    Tuint16      btn_option;      // see WID_BTNOPT_...
    CstrStatic   but_wn;          // widget resource name of the button
    TstrEaten    but_helpText;    // if not null helptext must be displayed
                                  // when cursor enter the button
    TaccelDef    but_accelKey0;   // 2 accelaration keys sequence for
    TaccelDef    but_accelKey1;   // this button, see GL_ACCEL_Xxx

    int          but_dx;    // minimal width and height of button
    int          but_dy;    // if <=0, adjust to content

    Tvgui_mw*    but_vmw;   // back link to the main widget or null
    Tvgui_dw*    but_vdw;   // back link to the dialog widget or null

    union {
        Twid_button_orn      orn;
        Twid_button_std      std;
        Twid_button_onOff    onOff;
        Twid_button_spanel   spanel;
    } u;

    union {
        void*            cbUsr;
        Twid_button_cbMw cbMw;
        Twid_button_cbDw cbDw;
    } cb;
#define               but_cbUsr cb.cbUsr
#define               but_cbBt  cb.cbBt
#define               but_cbMw  cb.cbMw
#define               but_cbDw  cb.cbDw
    int               but_uid;
    void*             but_ud;

    // if not null, but_free(gui,but_ud) is called when the button
    // is deleted
    Twid_button_free   but_free;

    Tegs_widget        but_widEGS;   // the EGS widget of the button

    Twid_button_cbVgui but_cbVgui;
    Twid_buttonPanel*  but_panel;    // back link to the panel or null 
};

extern Tvgui* wid_button_getVGui(Cwid_button* self);
extern void   wid_button_dbf(Twid_button*  b, Tegs_widget pw);
// free the memory used by self except of pixmap in std button
extern void   wid_button_delete (Twid_button* self);

extern void   wid_button_chgText(Twid_button* self, CstrBrief str);
extern void   wid_button_show   (Twid_button* self);
extern void   wid_button_unshow (Twid_button* self);
extern void   wid_button_enable (Twid_button* self);
extern void   wid_button_disable(Twid_button* self);
extern void   wid_button_setOn  (Twid_button* self);
extern void   wid_button_setOff (Twid_button* self);
extern void   wid_button_redraw (Twid_button* self);
extern void   wid_button_setSize(Twid_button* self, int dx, int dy);


/*======================================================================*/
/*= Interface to the external graphic system                           =*/

extern void        wid_button_map_cb(Twid_button*self);

extern Tegs_widget gui_button_orn_dbf  (Twid_button*  b, Tegs_widget pw);
extern Tegs_widget gui_button_std_dbf  (Twid_button*  b, Tegs_widget pw);
extern Tegs_widget gui_button_onoff_dbf(Twid_button*  b, Tegs_widget pw);

extern void  gui_button_setOn (Twid_button* button);
extern void  gui_button_setOff(Twid_button* button);
extern void  gui_button_redraw(Twid_button* button);

/*======================================================================*/

#define DBGButKd2Str(b) ((b)->but_kind==WID_BUT_KD_OrnLab ? "ornlab" :\
                         (b)->but_kind==WID_BUT_KD_Standard ? "std" :\
                         (b)->but_kind==WID_BUT_KD_OnOff  ? "onoff" :\
                         (b)->but_kind==WID_BUT_KD_Panel  ? "panel" :\
                        "???")
#define DBGBut2Str(n) ((n)==1?"left":(n)==2?"middle":(n)==3?"right":"???")

#define DBG0_But(fmt,...)  DBG0_printf("button",fmt,__VA_ARGS__)
#define DBG1_But(fmt,...)  DBG1_printf("button",fmt,__VA_ARGS__)
#define DBG2_But(fmt,...)  DBG2_printf("button",fmt,__VA_ARGS__)

#define DBG0_ButE(fmt,...) DBG0_printf("button","Enter : " fmt,__VA_ARGS__)
#define DBG0_ButM(fmt,...) DBG0_printf("button","Middle: " fmt,__VA_ARGS__)
#define DBG0_ButL(fmt,...) DBG0_printf("button","Leave : " fmt,__VA_ARGS__)
#define DBG1_ButE(fmt,...) DBG1_printf("button","Enter : " fmt,__VA_ARGS__)
#define DBG1_ButM(fmt,...) DBG1_printf("button","Middle: " fmt,__VA_ARGS__)
#define DBG1_ButL(fmt,...) DBG1_printf("button","Leave : " fmt,__VA_ARGS__)
#define DBG2_ButE(fmt,...) DBG2_printf("button","Enter : " fmt,__VA_ARGS__)
#define DBG2_ButM(fmt,...) DBG2_printf("button","Middle: " fmt,__VA_ARGS__)
#define DBG2_ButL(fmt,...) DBG2_printf("button","Leave : " fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_BUTTON_H
/*======================================================================*/
