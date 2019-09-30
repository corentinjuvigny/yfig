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
 *      $Id: vgui-dw.h 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-dw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_DW_H
#define FILE_VGUI_DW_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "wid.h"
#include "wid-simple.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Tvgui_dwTop Tvgui_dwTop;
typedef const   Tvgui_dwTop Cvgui_dwTop;

typedef struct _Tvgui_dwSet Tvgui_dwSet;
typedef const   Tvgui_dwSet Cvgui_dwSet;

struct _Tvgui_dw {
    WID_BOX_FIELDS();
    Tvgui_mw*    vmw;
    Tvgui_dwTop *top;       // private top dialog data
                            // must be null in sub dialog
//    Tegs_widget  widEGS;    // container widget of dialog
    Tvgui_dw*    title;
    Tvgui_dw    *pDialog;   // parent dialog
    void        *userdata;  // user data not used by the vgui API.
    void        *guiUserdata; // reserved to gui interface
#define VGUI_D_isTopDial(d)  ((d)->D_topDialog!=0)
#define VGUI_DW_isTopDial(d) ((d)->dw_topDialog!=0)

    char topDialog;  // 1:top modeless or modial popup dialog
    // The vgui_dw_newTop, vgui_dw_newSub and vguiM_dw_newTop
    // functions initialize the following fields to -1 (default).
    // The 0 and -1 value are interpreted by the vgui_dw_finishIC,
    // vgui_dw_finishIL and vgui_dw_finishMTX functions.
    // So one can set these field between the new and finish
    // function.
    char hspace;  // horizontal/vertical space between items
    char vspace;  // in pixel (-1:default, 0:items are pasted)
    char hmargin; // horizontal/vertical space before the 1st
    char vmargin; // items in pixel (-1:default, 0:paste to border)

    CstrStatic wcn; // widget configuration name, used to get default
                    // colors, ...

#define D_vmw         vmw
#define D_gui         vmw->gui
#define D_widEGS      ww_widEGS
#define D_title       title
#define D_printer     printer
#define D_pDialog     pDialog
#define D_top         top
#define D_userdata    userdata
#define D_guiUserdata guiUserdata
#define D_topDialog   topDialog
#define D_hspace      hspace
#define D_vspace      vspace
#define D_hmargin     hmargin
#define D_vmargin     vmargin
#define D_wcn         wcn

#define dw_vmw       any.D_vmw
#define dw_gui       any.D_gui
#define dw_title     any.D_title
#define dw_pDialog   any.D_pDialog
#define dw_pWidget   any.D_pDialog->D_widEGS
#define dw_userdata  any.D_userdata
#define dw_widEGS    any.D_widEGS
#define dw_topDialog any.D_topDialog
#define dw_hspace    any.D_hspace
#define dw_vspace    any.D_vspace
#define dw_hmargin   any.D_hmargin
#define dw_vmargin   any.D_vmargin
#define dw_wcn       any.D_wcn

#define wd_vmw     dw_vmw        // obsolete
#define wd_gui     dw_gui        // obsolete
#define wd_title   dw_title      // obsolete
#define wd_pDialog dw_pDialog    // obsolete
#define wd_pWidget dw_pWidget    // obsolete
#define wd_widEGS  dw_widEGS     // obsolete
};

#define SelfA &self->any
#define VdwA  &vdw->any

#define VDWLF_ROW(x)   (0x40000000|(x))
#define VDWLF_SEP1      0x00020000
#define VDWLF_SEP2      0x00040000
#define VDWLF_SEP3      0x00060000     // SEP1 + SEP2
//#define VDWLF_SSZ       0x00080000
//#define VDWLF_CH        0x00100000   // Center
//#define VDWLF_CV        0x00200000
#define VDWLF_CB        0x00300000
#define VDWLF_EH        0x01000000   // Expand
#define VDWLF_EV        0x02000000
#define VDWLF_EB        0x03000000

#define VDWLF_IsRow(x)  (0x40000000&(x))
#define VDWLF_RowNum(x) (0x000000FF&(x))

// value in pixel of widget spacing in dialog
#define VDW_SEP_H    5
#define VDW_SEP_V    5
#define VDW_SEP_HV   5
#define VDW_SEP1_H   10
#define VDW_SEP1_V   10
#define VDW_SEP1_HV  10
#define VDW_SEP2_H   15
#define VDW_SEP2_V   15
#define VDW_SEP2_HV  15

/*======================================================================*/
/*= Constructors and destructor                                        =*/

/**
 * The vgui_dw_newTop function creates a popup dialog window, that
 * can be popup by the gui_dw_run and gui_dw_start function.
 * The title of the dialog window is set to "title" string.
 * The bs parameter is a bitwise-or of Twid_dialStatus constants,
 * it defines the control buttons of the dialog window.
 * These buttons are automatically added to the dialog window.
 *
 * Return:
 *  The function returns the created dialog.
 *
 * Notes:
 *  - You can set the bs parameter to WID_DIALSTATUS_None.
 *  - You can add control buttons to the dialog window later with
 *    vgui_dw_addCtlButtXXX functions.
 *  - Setting WID_DIALSTATUS_Cancel in bs activates the cancel button.
 *  - Setting WID_DIALSTATUS_CancelX in bs activates both the cancel
 *    button and the X button set by the window manager.
 *    If one clicks the cancel button or the VM X button, the
 *    gui_dw_run function returns WID_DIALSTATUS_Cancel (AND NOT
 *    WID_DIALSTATUS_CancelX).
 *    So WID_DIALSTATUS_Cancel|WID_DIALSTATUS_CancelX is equivalent to
 *    WID_DIALSTATUS_CancelX alone.
**/
extern Tvgui_dw * vgui_dw_newTop(int bytesz, Tvgui_mw*vmw,
                                 TstrEaten title, Twid_dialStatus bs);

/**
 * The vguiM_dw_newTop function creates and initializes a top widget
 * container as a child of the pw widget.
 * If the byte size specified by the bytesz parameter is greater than
 * the size of the Tvgui_dw type, the extra byte are set to zero.
 * If the pw parent widget is NULL, the container is built as a child
 * of the vmw widget.
 *
 * Return:
 *  The function returns the created widget container.
 *
 * Parameters:
 *  - bytesz: The byte size of the allocated structure. Insure that
 *            it is greater than sizeof(Tvgui_dw).
 *  - vmw:    The new container widget is attached this main widget.
 *  - pw:     The parent of created widget. A null pointer means
 *            the vmw widget.
 * Insure:
 *  - bytesz >=  sizeof(Tvgui_dw).
 *  - pw==NULL or pw is in child hierarchy of the vmw widget.
**/
extern Tvgui_dw* vguiM_dw_newTop(int bytesz, Tvgui_mw*vmw, Tegs_widget pw);

/**
 * The vgui_dw_newSub function creates and initializes a widget
 * container as a child of the vdw widget.
 * If the byte size specified by the bytesz parameter is greater than
 * the size of the Tvgui_dw type, the extra byte are set to zero.
 * If the title parameter is not null, a top title bar is added to
 * the widget container showing the title string.
 *
 * Parameters:
 *  - bytesz: The byte size of the allocated structure.
 *  - vdw:    The parent widget.
 *  - title:  The container title.
 *            A null value means no title.
 *
 * Insure:
 *  - bytesz >=  sizeof(Tvgui_dw).
**/
extern Tvgui_dw * vgui_dw_newSub(int bytesz, Tvgui_dw*vdw,
                                 TstrEaten title);

/**
 * The vgui_dw_free function frees all the memory used by the
 * vdw widget including the vdw structure itself. So the standard
 * free function of a Tmy_vdw_widget that is a specialization of 
 * the Tvgui_dw dialog is:
 *   void my_vdw_widget(Tvgui_dw* vdw)
 *   {
 *      Tmy_vdw_widget* self = (Tmy_vdw_widget*)vdw;
 *      // free self->field1
 *      // free self->field2
 *      ...
 *      vgui_dw_free(vdw); // LAST ACTION
 *   }
 * If the vdw widget is a top popup dialog, the function also destroys
 * all the EGS widget of the dialog.
**/
#define vgui_dw_delete vgui_dw_free // obsolete
extern void vgui_dw_delete(Tvgui_dw*vdw);

/**
 * The vgui_dw_finishIC function fills columns from top to bottom with
 * the widgets specified by the variable argument list.
 * The variable argument list is a sequence of (int,Tegs_widget)
 * terminated by a single -1 integer. For instance, this invocation
 *   vgui_dw_finishIC(dialog,
 *     0, w1,     1, w2,   2, w3,
 *     0, w4,     1, w5,   2, w6,
 *                1, w7,   2, w8,
 *                1, w9,
 *    -1);
 * creates a 3 columns layout and places the w1, w4 widgets in the
 * first column, w2, w5, w7, w9 in the second column and w3, w5, w8
 * in the third column. Notice that there is no horizontal alignment,
 * so in the former example, w8 and w9 may be in the same row if the
 * height of (w2+w5+w7) is equal to the height of (w3+w6).
 * 
 * You can tune the layout by oring the following flags to the column
 * number.
 *   - VDWLF_SEP1 adds a small vertical space
 *   - WIDLF_SEP2 adds a larger vertical space
 *   - VDWLF_SSZ  resizes to the same width all the widget of the
 *     column.
 *
 * If the vdw dialog has a title sub dialog, it is placed at the widget
 * top. If the vdw dialog is a top dialog, the control button bar is
 * added at the widget bottom.
 *
 * The vgui_dw_finishIL function is similar to the vgui_dw_finishIC
 * function but number are interpreted as row number instead of column
 * number. So the former example generates the layout
 *   w1 w4
 *   w2 w5 w7 w9
 *   w3 w6 w8
 * Notice that is more readable to present the code as following:
 *   vgui_dw_finishIL(dialog,
 *      0,w1,  0,w4,
 *      1,w2,  1,w5,  1,w7,  1,w9,
 *      2,w3,  2,w6,  2,w8,
 *   -1)
 *    
**/
extern void vgui_dw_finishIC (Tvgui_dw*vdw, ...);
extern void vgui_dw_finishIL (Tvgui_dw*vdw, ...);

extern void vgui_dw_finishMTX(Tvgui_dw*vdw, ...);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Interface to EGS                                                   =*/

typedef enum {
    VDM_V2E_HGrow   = 0x001,
    VDM_V2E_VGrow   = 0x002,
    VDM_V2E_BGrow   = 0x003,
    VDM_V2E_HCenter = 0x010,
    VDM_V2E_VCenter = 0x020,
    VDM_V2E_BCenter = 0x030,
} Tvgui_dwMtxFlag;
#define VGUI_DW_MTX_SZ 25
typedef struct _Tvgui_dwMtxCell {
    Tegs_widget  widEGS;    // NULL:empty cell or spaned cell
    char         xs,ys;     // cell spans cell[i,j]: x<=i<x+xs
                            //                    && y<=j<y+ys
    int          grow;      //  0 : no growing 
                            // 'H': grow horizontally
                            // 'V': grow vertically
                            // 'B': grow horizontally & vertically both
    int          sepHV;     // add sepHV pixel space at the top and
                            // at the left hand of the widget.
    Tvgui_dwMtxFlag flag;   // miscellaneous options
} Tvgui_dwMtxCell;
typedef Tvgui_dwMtxCell Tvgui_dwMTX[VGUI_DW_MTX_SZ][VGUI_DW_MTX_SZ];

extern Tegs_widget gui_dw_create(Tvgui_dw*vdw, CstrBrief wn,
                                int bd, int dx, int dy, int resizable);
extern Tegs_widget gui_dw_create2(Tvgui_dw*vdw, Tegs_widget pw, CstrBrief wn,
                                int bd, int dx, int dy, int resizable);
extern void        gui_dw_free(Tvgui_dw*vdw);

// align fully the mtx[i][j] 
// layout:
//       ----title---title---title----       
//       mtx[0][0] mtx[0][1] mtx[0][2]
//       mtx[1][0] mtx[1][1] mtx[1][2]
//       mtx[2][0] mtx[2][1] mtx[2][2]
//       ---button--button--button----
// Notes:
//  - spans are available
//  - sepHV indicates the spaces
//        * between the upper widget and the current
//        * between the left hand widget and the current
extern void gui_dw_finishMfa(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget btns,
                             Tvgui_dwMTX mtx, int nl, int nc);

// align the columns independently
//         mtx[0][j]  mtx[0][j+1] mtx[0][j+2] ...
//         mtx[1][j]  mtx[1][j+1] mtx[1][j+2] ...
//         ...
// layout:
//       ----title---title---title----       
//       mtx[0][0] mtx[0][1] mtx[0][1]
//       mtx[0][0] mtx[1][1] 
//       mtx[1][0] mtx[2][1] 
//       ---button--button--button----
// Notes:
//   No span,
//   incomplete column are filed with null widget
//   sepHV indicates the space between the upper widget and current.
extern void gui_dw_finishMic(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget btns,
                             Tvgui_dwMTX mtx, int nl, int nc);
extern void gui_dw_finishMil(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget btns,
                             Tvgui_dwMTX mtx, int nl, int nc);


extern void gui_dw_finishSdx(Tvgui_dw*vdw, Tegs_widget*ws, int wn);
extern void gui_dw_finishSdy(Tvgui_dw*vdw, Tegs_widget*ws, int wn);

/*======================================================================*/
/*= Configuring Title dialog                                           =*/


// Set to str the title that the title sub-dialog of the vdw dialog shows.
// If the vdw dialog has no title sub-dialog, it is created.
// If the str string is empty the function does nothing.
extern void vgui_dw_titleSet(Tvgui_dw* vdw, CstrStatic wcn, CstrBrief str);

// fold/unfold button
extern void vgui_dw_titleAddFufLabL(Tvgui_dw* vdw);
extern void vgui_dw_titleAddFufLabR(Tvgui_dw* vdw);
extern void vgui_dw_titleAddFufPxmL(Tvgui_dw* vdw);
extern void vgui_dw_titleAddFufPxmR(Tvgui_dw* vdw);
extern void vgui_dw_titleFold      (Tvgui_dw* vdw);
extern void vgui_dw_titleUnFold    (Tvgui_dw* vdw);

// extra label at the right hand of the title
extern void vgui_dw_titleAddLabTxt (Tvgui_dw* vdw,
                                    CstrBrief text, CstrBrief width);
extern void vgui_dw_titleAddLabBox (Tvgui_dw* vdw,
                                    int bdw, int dx, int dy);
extern void vgui_dw_titleLabSetText (Tvgui_dw* vdw, CstrBrief text);
extern void vgui_dw_titleLabSetBgClr(Tvgui_dw* vdw, Ccolor* c);
extern Tegs_widget vgui_dw_titleLab(Tvgui_dw* vdw);

/*======================================================================*/
/*= Running dialog                                                     =*/

/**
 * The vgui_dw_run and vgui_dw_run2 functions run the vdw dialog in
 * modal mode.
 * The vgui_dw_breakloop function breaks the event loop started by
 * the vgui_dw_run or vgui_dw_run2 functions.
 * The vgui_dw_run and vgui_dw_run2 functions return the s state
 * argument of the vgui_dw_breakloop function. 
**/
extern Twid_dialStatus  vgui_dw_run (Tvgui_dw*vdw);
extern Twid_dialStatus  vgui_dw_run2(Tvgui_dw*vdw,
                                     Tegs_widget inw, int x, int y);
extern void             vgui_dw_breakloop(Tvgui_dw*vdw, Twid_dialStatus s);
#define vgui_dw_enableBUTorg(d,b)  (vgui_dw_getTopDialog(d))->top->dt_buttsMaskOrg |= b
#define vgui_dw_disableBUTorg(d,b) (vgui_dw_getTopDialog(d))->top->dt_buttsMaskOrg &= ~(b)
#define vgui_dw_enableBUTcur(d,b)  (vgui_dw_getTopDialog(d))->top->dt_buttsMaskCur |= b
#define vgui_dw_disableBUTcur(d,b) (vgui_dw_getTopDialog(d))->top->dt_buttsMaskCur &= ~(b)
#define vgui_dw_setSelBUT(d,b)     (vgui_dw_getTopDialog(d))->top->dt_buttSelected  = (b)

/**
 * The vgui_dw_start function starts the vdw dialog in modeless mode.
**/
extern void vgui_dw_start(Tvgui_dw*vdw);

/*======================================================================*/
/*= Miscellaneous utilities                                              =*/

extern void      vgui_dw_titleUpdate(Tvgui_dw*vdw, TstrEaten t);
extern void      vgui_dw_infmess (Cvgui_dw*vdw, CstrBrief fmt, ...);
extern void      vgui_dw_wrnmess (Cvgui_dw*vdw, CstrBrief fmt, ...);
extern void      vgui_dw_errmess (Cvgui_dw*vdw, CstrBrief fmt, ...);
extern void      vgui_dw_vinfmess(Cvgui_dw*vdw, CstrBrief fmt, va_list ap);
extern void      vgui_dw_vwrnmess(Cvgui_dw*vdw, CstrBrief fmt, va_list ap);
extern void      vgui_dw_verrmess(Cvgui_dw*vdw, CstrBrief fmt, va_list ap);

// Inerface to EGS
extern void        gui_dw_setTitle(Cvgui_dw*vdw, TstrBrief t);

/*======================================================================*/
/*= Top dialog features                                                =*/

struct _Tvgui_dwTop {
    TstrEaten    dt_title;     // the dialog title

    // buttons that control the dialog
    int               dt_wmX;  // accept cancel from WM (window Manager)
    Twid_buttonPanel* dt_ctlButtons;
    Twid_dialStatus dt_buttsMaskOrg; // active buttons when dialog starts
    Twid_dialStatus dt_buttsMaskCur; // active buttons when dialog runs
    Twid_dialStatus dt_buttSelected; // button that was clicked (single)
    int             dt_btnLayout;    // x&1=1: btn are placed vertically
                                     // x&1=0: btns are placed horizontally
                                     // x&6=0: btns are placed at dialog bottom
                                     // x&6=1: btns are placed at dialog left hand

#if 0
    // layout
    int         dt_layout; // 0: place the widget vertically,
                           //    buttons line at dialog bottom
                           // 1: place the widget horizontally
                           //    buttons column at dialog east
                           // 2: place the widget horizontally
                           //    buttons row at dialog east
#endif

    // set the focus to this widget when started
    Tegs_widget dt_focus;

    // for printing messages into the dialog 
    // Tegs_widget dt_messSelf;
    Tegs_widget  dt_printer;
    Tmsgdrv*     dt_msgdrv;

  /**********************************************************************
   * For modal dialog
   *   - vt_modal_run  : It starts the vdw dialog in modal mode. 
   *   - vt_modal_run2 : It starts the vdw dialog in modal mode,
   *                     placing vdw at (x,y) position in inw.
   *   - vt_modal_break: It breaks the event loop started by the
   *                     vt_modal_run/run2 functions.
  **/
    Twid_dialStatus (*vt_modal_run      )(Tvgui_dw*vdw);
    Twid_dialStatus (*vt_modal_run2     )(Tvgui_dw*vdw, Tegs_widget inw,
                                                        int x, int y);
    void            (*vt_modal_breakloop)(Tvgui_dw*vdw, Twid_dialStatus s);
    

  /**********************************************************************
   * For modeless dialog
   *   - vt_modal_run  : It starts the vdw dialog in modeless mode. 
   *   - dt_cb         : The callback that is performed when a control
   *                     button is clicked. Its format is:
   *                       dt_cb(vdw, status, cb_ud)
  **/
    void      (*vt_modeless_start)(Tvgui_dw*vdw);
    Twid_cb_DSU dt_cb;
    void*       dt_cb_ud;
} ;

extern Tvgui_dw*    vgui_dw_getTopDialog(Tvgui_dw*d);
extern Tvgui_dwTop* vgui_dw_getTopData  (Cvgui_dw*vdw);

extern void vgui_dw_addBtnCtl(Tvgui_dw*vdw, Twid_dialStatus b,
                                             CstrBrief      n);
#define vgui_dw_addCtlButt vgui_dw_addBtnCtl
extern void vgui_dw_addCtlButtsDN(Tvgui_dw*vdw, Twid_dialStatus bs);
extern void vgui_dw_addCtlButts  (Tvgui_dw*vdw, ...);
extern void vgui_dw_addCtlButtsVa(Tvgui_dw*vdw, va_list ap);
extern Twid_button* vgui_dw_addBtnStdLab (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        CstrBrief text, CstrBrief width);
extern Twid_button* vgui_dw_addBtnStdPxm (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        int dx, int dy, TpixmapEGS pm);
extern Twid_button* vgui_dw_addBtnOnOffLab (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        CstrBrief off, CstrBrief on);
extern Twid_button* vgui_dw_addBtnOnOffPxm (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);

//extern void vgui_dw_addCtlButtsVp(Tvgui_dw*vdw, va_list ap);

#if 0
/*======================================================================*/
/*= A Tvgui_dw set                                                     =*/

struct _Tvgui_dwSet {
    int       eleMemNb; // element number of the eles memory block
    int       eleNb;    // element number of set (eleNb<eleMemNb)
    Tvgui_dw**eles;     // the set elements
};

#define              vgui_dwset_NB(s)    (s)->eleNb
#define              vgui_dwset_clear(s) (s)->eleNb=0
extern Tvgui_dwSet*  vgui_dwset_new  (int n);
extern void          vgui_dwset_free (Tvgui_dwSet* s);
extern void          vgui_dwset_add  (Tvgui_dwSet* s, Tvgui_dw* p);
extern Tvgui_dw*     vgui_dwset_first(Cvgui_dwSet* s);
extern Tvgui_dw*     vgui_dwset_last (Cvgui_dwSet* s);
#endif

/*======================================================================*/
/*= debug                                                              =*/

#define DBG0_Dw(fmt,...)  DBG0_printf("dialog",fmt,__VA_ARGS__)
#define DBG1_Dw(fmt,...)  DBG1_printf("dialog",fmt,__VA_ARGS__)
#define DBG2_Dw(fmt,...)  DBG2_printf("dialog",fmt,__VA_ARGS__)

#define DBG0_DwE(fmt,...) DBG0_printf("dialog","Enter : "fmt,__VA_ARGS__)
#define DBG0_DwM(fmt,...) DBG0_printf("dialog","Middle: "fmt,__VA_ARGS__)
#define DBG0_DwL(fmt,...) DBG0_printf("dialog","Leave : "fmt,__VA_ARGS__)
#define DBG1_DwE(fmt,...) DBG1_printf("dialog","Enter : "fmt,__VA_ARGS__)
#define DBG1_DwM(fmt,...) DBG1_printf("dialog","Middle: "fmt,__VA_ARGS__)
#define DBG1_DwL(fmt,...) DBG1_printf("dialog","Leave : "fmt,__VA_ARGS__)
#define DBG2_DwE(fmt,...) DBG2_printf("dialog","Enter : "fmt,__VA_ARGS__)
#define DBG2_DwM(fmt,...) DBG2_printf("dialog","Middle: "fmt,__VA_ARGS__)
#define DBG2_DwL(fmt,...) DBG2_printf("dialog","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_DW_H
/*======================================================================*/
