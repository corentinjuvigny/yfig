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
 *      $Id: wid-buttonPanel.h 331 2018-10-30 21:35:47Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-buttonPanel.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Line, row and matrix of buttons.                                   =*/
/*=   - Twid_buttonPanel: The button line, row or matrix.              =*/
/*=                                                                    =*/
/*= The Twid_buttonPanel_button type defines different button kinds.   =*/
/*=  - OrnLab:                                                         =*/
/*=     The ORNament LABel is just a label. Its rule is to separate    =*/
/*=     the other buttons.                                             =*/
/*=       OrnLab1 butt1 butt2 OrnLab2 butt3 butt4 butt5                =*/
/*=     So  OrnLab1 is like a title for the butt1 and butt2 buttons,   =*/
/*=     and OrnLab2 is like a title for the butt3/4/5 buttons.         =*/
/*=  - Standard:                                                       =*/
/*=     The standard button is a button that trigger a callback when   =*/
/*=     it is clicked.                                                 =*/
/*=     It render a label and/or a pixmap.                             =*/
/*=  - OnOff:                                                          =*/
/*=     The On/Off button is a button with 2 states (On/Off).          =*/
/*=     At start time all the On/Off buttons are off.                  =*/
/*=     In a panel, only a single button can be On, we call it the     =*/
/*=     the active button.                                             =*/
/*=     When it is clicked the following steps are performed.          =*/
/*=       - If it is already the active button, the click is ignored.  =*/
/*=       - The On/Off function of the panel is triggered.             =*/
/*=       - If the previous function returns 0, the click is ignored.  =*/
/*=       - If the previous function returns 1, the click is accepted. =*/
/*=         If there is an active button, it is turned Off.            =*/
/*=         The clicked button is turned On and becomes the active     =*/
/*=         button.                                                    =*/
/*=         The callback of the clicked button is triggered.           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_BUTTONPANEL_H
#define FILE_WID_BUTTONPANEL_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

#define VGUI_BP_BoutonNbMax 100  // the maximal number of buttons in panel

/*======================================================================*/
/*= The panel itself                                                   =*/

struct _Twid_buttonPanel {
    Tvgui*      bp_gui;    // shortcut to vgbpm_vmw or vgbpm_vdw gui
#define vgbpm_gui     bp_gui        // obsolete
    Tvgui_mw*   bp_vmw;    // vgbpm_vmw and vgbpm_vdw are exclusively
    Tvgui_dw*   bp_vdw;    // NULL.
#define vgbpm_vmw     bp_vmw        // obsolete
#define vgbpm_vdw     bp_vdw        // obsolete
    char        bp_type;      // 'P': panel
                              // 'V': vertical   (single column)
                              // 'H': horizontal (single row)
    char        bp_subpanel;  // 1: this panel is a sub-panel
    char        bp_scroll;    // 1: add 1 or 2 scrollbar when needed
                              // 3: add 1 or 2 scrollbar permanently.
    CstrStatic  bp_wn;        // widget resource name of the panel
#define vgbpm_wn   bp_wn
#define vgbpm_type bp_type

    int          bp_btnNb;   
    Twid_button* bp_btns[VGUI_BP_BoutonNbMax];
    Twid_button* bp_activeButton;
#define vgbpm_buttonNb     bp_btnNb        // obsolete
#define vgbpm_buttons      bp_btns         // obsolete
#define vgbpm_activeButton bp_activeButton // obsolete
    int       (*vgbpm_callback)(        // user selected the next button 
        Twid_buttonPanel* self,         // Returns 1 if next becomes active.
        Twid_button* next);             // otherwise 0.

    Tegs_widget bp_widEGSext; 
    Tegs_widget bp_widEGSint; 

    // Layout parameters
    int         bp_rownb;             // prefered line number or 0
    int         bp_colnb;             // prefered column number or 0
    int         bp_labelHeight;       // minimal height of labels (pixel)
    int         bp_buttonWidth;       // base width   of buttons  (pixel)
    int         bp_buttonHeight;      // base height  of buttons  (pixel)
    int         bp_buttonBorder;      // border width of buttons  (pixel)
    int         bp_buttonHVsep;       // space between to buttons (pixel)
    char        bp_adjustR1C1;        // R1Cn: all the btn with the same height
                                      // RnC1: all the btn with the same width
                                      // RaCa: ignored
#define vgbpm_labelHeight     bp_labelHeight   // obsolete
#define vgbpm_buttonWidth     bp_buttonWidth   // obsolete
#define vgbpm_buttonHeight    bp_buttonHeight  // obsolete
#define vgbpm_buttonBorder    bp_buttonBorder  // obsolete
#define vgbpm_buttonHVsep     bp_buttonHVsep   // obsolete
};

extern Twid_buttonPanel* widM_buttonPanel_newRaCa(Tvgui_mw*vmw,
                                                  int bwidth, int bheight);
extern Twid_buttonPanel* widM_buttonPanel_newR1Cn(Tvgui_mw*vmw,
                                                  int bwidth, int bheight);
extern Twid_buttonPanel* widM_buttonPanel_newRnC1(Tvgui_mw*vmw,
                                                  int bwidth, int bheight);

extern Twid_buttonPanel* widD_buttonPanel_newRaCa(Tvgui_dw*vdw,
                                                  int bwidth, int bheight);
extern Twid_buttonPanel* widD_buttonPanel_newR1Cn(Tvgui_dw*vdw,
                                                  int bwidth, int bheight);
extern Twid_buttonPanel* widD_buttonPanel_newRnC1(Tvgui_dw*vdw,
                                                  int bwidth, int bheight);

extern void wid_buttonPanel_build (Twid_buttonPanel* self, Tegs_widget pw);
extern void wid_buttonPanel_finish(Twid_buttonPanel* self);
extern void wid_buttonPanel_dbf   (Twid_buttonPanel* self, Tegs_widget pw);
extern void wid_buttonPanel_free  (Twid_buttonPanel* self);

extern void wid_buttonPanel_enable (Twid_buttonPanel* self);
extern void wid_buttonPanel_disable(Twid_buttonPanel* self);

extern Twid_button*
            wid_buttonPanel_addDecorativeLabel(Twid_buttonPanel* panel,
                                               CstrBrief label);
extern void wid_buttonPanel_addButton( Twid_buttonPanel* panel,
                                       Twid_button* button );

#define WID_BP_RaCa_IsAlc(self) ( (self)->bp_rownb==0 && (self)->bp_colnb==0 ) 
#define WID_BP_RaCa_IsFlc(self) ( (self)->bp_rownb!=0 || (self)->bp_colnb!=0 ) 

/**
 * The wid_buttonPanel_active function performs the next actions on the
 * self button panel:
 *   - unactive the current active button if it exists,
 *   - active the b th button of the panel.
**/
extern void wid_buttonPanel_active(Twid_buttonPanel* self,
                                   Twid_button*      b);
/**
 * The wid_buttonPanel_getBtnByUid function returns the first button of
 * the self panel the user identifier of which is uid.
 * If a such button does not exist in the panel, it returns NULL.
**/
extern Twid_button* wid_buttonPanel_getBtnByUid(Cwid_buttonPanel* self, int uid);

/*======================================================================*/
/*= tools for the external graphic system (only for panels RaCa)       =*/

/* returns the minimal height of a panel with rowNb rows */
extern int wid_buttonPanel_getHeight(Cwid_buttonPanel*panel, int rowNb);

/* returns the width of a panel with colNb columns */
extern int wid_buttonPanel_getWidth(Cwid_buttonPanel*panel, int colNb);

/* Returns the number of rows needed for a layout of colNb columns.
 * Fills up rows, cols and colspans arrays with the button layout. */
extern int wid_buttonPanel_getLayout(Cwid_buttonPanel*panel,int colNb,
        int rows[VGUI_BP_BoutonNbMax],     int cols[VGUI_BP_BoutonNbMax],
        int colspans[VGUI_BP_BoutonNbMax]);

/*======================================================================*/
/*= Interface to the external graphic system                           =*/

/**
 * To create the EGS panel, the VGUI calls in order:
 *   - firstly gui_buttonPanel_create (once only),
 *   - then gui_buttonPanel_add/DecorativeLabel/Button (several times),
 *   - and finally gui_buttonPanel_create_finish (once only).
**/
extern Tegs_widget gui_buttonPanel_build (Twid_buttonPanel* panel,Tegs_widget p);

extern void        gui_buttonPanel_finishRaCa (Twid_buttonPanel* panel);
extern void        gui_buttonPanel_finishR1Cn (Twid_buttonPanel* panel);
extern void        gui_buttonPanel_finishRnC1 (Twid_buttonPanel* panel);
extern void        gui_buttonPanel_update     (Twid_buttonPanel* panel);

/*======================================================================*/

#define DBG0_BP(fmt,...)  DBG0_printf("buttonpanel",fmt,__VA_ARGS__)
#define DBG1_BP(fmt,...)  DBG1_printf("buttonpanel",fmt,__VA_ARGS__)
#define DBG2_BP(fmt,...)  DBG2_printf("buttonpanel",fmt,__VA_ARGS__)

#define DBG0_BPE(fmt,...) DBG0_printf("buttonpanel","Enter : "fmt,__VA_ARGS__)
#define DBG0_BPM(fmt,...) DBG0_printf("buttonpanel","Middle: "fmt,__VA_ARGS__)
#define DBG0_BPL(fmt,...) DBG0_printf("buttonpanel","Leave : "fmt,__VA_ARGS__)
#define DBG1_BPE(fmt,...) DBG1_printf("buttonpanel","Enter : "fmt,__VA_ARGS__)
#define DBG1_BPM(fmt,...) DBG1_printf("buttonpanel","Middle: "fmt,__VA_ARGS__)
#define DBG1_BPL(fmt,...) DBG1_printf("buttonpanel","Leave : "fmt,__VA_ARGS__)
#define DBG2_BPE(fmt,...) DBG2_printf("buttonpanel","Enter : "fmt,__VA_ARGS__)
#define DBG2_BPM(fmt,...) DBG2_printf("buttonpanel","Middle: "fmt,__VA_ARGS__)
#define DBG2_BPL(fmt,...) DBG2_printf("buttonpanel","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_BUTTONPANEL_H
/*======================================================================*/
