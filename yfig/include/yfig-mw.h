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
 *$Software: YFIG
 *      $Id: yfig-mw.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-mw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data structure for top YFIG window.                                =*/
/*=                                                                    =*/
/*= Provide:                                                           =*/
/*=  - Tyfig_mw:     groups all the data required for the top YFIG     =*/
/*=                  window.                                           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_MW_H
#define FILE_YFIG_MW_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-types.h"
#include "yfig-drawparam.h"

#include "vgui/vgui-mw.h"

#include "outdev/style.h"
#include "tools/lenunit.h"

/*======================================================================*/

struct _Tyfig_mw {
    Tvgui_mw       any;
#define ymw_vmw    any
#define ymw_gui    any.gui
#define ymw_widEGS any.widEGS
#define ymw_dsds   any.gui->g_ud

    Twidm_bar*         ymw_menuBar;   // The top bar within the menus: file, edit, view, ....
#define ymw_message    vmw_message    // Where information messages are printed.
    Twid_mousefun*     ymw_mouseFun;
    Twid_buttonPanel*  ymw_modePanel; // The buttons that drive the creation or the modification
                                      // of figure objects.
    Twid_canvas*       ymw_canvas;    // The area for figure drawing.
    Tvgui_dw*          ymw_update;    // the update buttons
    Twid_buttonPanel*  ymw_dpPanel;   // The buttons that specify the parameters for creating or
                                      // for modifying the figure objects.
    Tvgui_dw*          ymw_layers;    // the layers dialog widget

    // the current scene
    int         ymw_currHistSave;  // pointer in history when the current figure was loaded or saved.
    int         ymw_currHistCurr;  // current pointer in history (can be negative).
    Tfdp        ymw_currDP;    // current parameters for user object creation
    Tyfig_drawparam            // widget of current parameters for user object
                ymw_currYDP;   // creation.
    TarrowDesc  ymw_arrowbeg;  // param. for adding arrows at begin of line/curve
    TarrowDesc  ymw_arrowend;  // param. for adding arrows at end of line/curve
    TuserUnit   ymw_userUnit; // the user unit used in the current scene
                              // it is used only for diplaying lengths,
                              // and canvas grid and scales.

    // parameter of the YFIG gui
    TstrEaten   ymw_OMSFdir;   // the start directory for OMSF (Open/Merge/Save file).
    TstrEaten   ymw_expDir;    // the start directory for exporting figures.
    int ymw_do_useFigUnit; // 1: use the unit set in current figure. ymw_userUnit is updated
                           //    when a file is loaded.
                           // 0: don't update ymw_userUnit when file is loaded.
    int ymw_do_scaleText;  // 1: scale text when scaling object
                           // O: do not scale text when scaling object
    int ymw_do_scaleStyle; // 1: scale style (thickness) when scaling object
                           // O: do not scale style (thickness) when scaling object
    int ymw_do_symrotCtr;  // 1: do object symmetry and rotation from object center
                           // O: do object symmetry and rotation from selected point


    // message driver
#define ymw_msgdrv vmw_msgdrv
};

// shortcut 
#define ymw_odcvs      ymw_canvas->vgcm_outdev
#define ymw_fsod(ymw) ((Tyfig_device*)ymw->ymw_odcvs)
#define ymw_vdev      ymw_canvas->vgcm_outdev
#define ymw_ydev(ymw) ((Tyfig_device*)ymw->ymw_vdev)
#define YMW_vdev(ymw) wid_canvas_getVDev(ymw->ymw_canvas)
#define YMW_ydev(ymw) ((Tyfig_device*)YMW_vdev(ymw))
#define ymw_efsm      ymw_canvas->vgcm_efsm
extern Tfig_scene* YMW_scene (Cyfig_mw* ymw);
extern Tfigure*    YMW_figure(Cyfig_mw* ymw);

extern Tyfig_mw* yfig_mw_nbf    (Tvgui*    gui);
extern int       yfig_mw_delete (Tyfig_mw* ymw);

extern Tegs_widget gui_yfig_create   (Tvgui*    gui);
extern void        gui_yfig_createend(Tyfig_mw* ymw);
extern void        gui_yfig_delete   (Tyfig_mw* ymw);
extern void        gui_yfig_setTitle (Cyfig_mw* ymw, TstrBrief t);

extern void      yfig_mw_titleUpdate(Cyfig_mw* ymw);

/*======================================================================*/
/*= changing/handling current scene.                                   =*/

extern void yfig_cursce_chgFigPath(Tyfig_mw*ymw, CstrBrief path);

/**
 * Return:
 *   The yfig_cursce_isModified function returns 1, if the current scene
 *   of the ymw YFIG-top-window has been modified since its creation or
 *   the last back-up.
 *   The function returns 0 in the other cases.
**/
extern int yfig_cursce_isModified(Tyfig_mw*ymw);

/**
 * The yfig_cursce_new function changes the current scene of the ymw
 * YFIG-top-window to a new scene within an empty figure.
 * 
 * The yfig_cursce_newByPath function changes the current scene of the ymw
 * YFIG-top-window to a new scene within the path figure.
 *
 * The yfig_cursce_delete function deletes the current scene of the ymw
 * YFIG-top-window. If the file of the current scene is not up to date
 * (never saved or modified since the last save), it pops up a dialog
 * for saving it.
 * Notice that a YFIG-top-window requires a current scene for working,
 * so this function must only be used when deleting a YFIG-top-window.
 *
 * Return:
 *  The yfig_cursce_newByPath returns 0 on success and -1 on failure.
 *  The yfig_cursce_delete returns 0 when the user canceled the quit
 *  process otherwise it returns 1.
**/
extern void  yfig_cursce_new      (Tyfig_mw*ymw);
extern int   yfig_cursce_newByPath(Tyfig_mw*ymw, CstrBrief path);
extern int   yfig_cursce_newByPathScaleAndTrans(Tyfig_mw*ymw,
                CstrBrief path, double sf, double tx, double ty);
extern int   yfig_cursce_delete   (Tyfig_mw*ymw);

/**
 * The yfig_cursce_save function saves the figure of the current scene
 * of the ymw YFIG-top-window.
 * The back-up is always performed even if yfig_cursce_isModified(ymw)
 * is true.
 * If the silent flag is set to 0, the function works silently.
 * If the silent flag is set to 1 and a problem occurs, the function
 * prints and/or popups error message. 
 *
 * The yfig_cursce_saveFilename function returns the file name that the
 * yfig_cursce_saveFilename uses t save the current scene.
 *
 * Return:
 *  The function returns 0 if the current figure is saved successfully,
 *  otherwise it returns -1.
**/
extern int  yfig_cursce_save(Tyfig_mw*ymw, int silent);
extern Cstr yfig_cursce_saveFilename( Cyfig_mw*ymw );

/*======================================================================*/
/*= Getters of the current scene.                                      =*/
/*=                                                                    =*/
/*= The yfig_cursce_scene function returns the figure scene that is    =*/
/*= currently drawn in the ymw window.                                 =*/
/*=                                                                    =*/
/*= The yfig_cursce_fig function returns the figure of current scene.  =*/
/*= The yfig_cursce_histo function returns the history of the current  =*/
/*= scene. If the figure has no history, it returns the NULL pointer.  =*/
/*=                                                                    =*/
/*= The yfig_cursce_topDir function returns the top directory object   =*/
/*= of the yfig_cursce_fig(ymw) figure.                                =*/
/*=                                                                    =*/
/*= The yfig_cursce_curDir function returns the directory object of    =*/
/*= yfig_cursce_scene(ymw) scene that is currently  drawn.             =*/

extern Tfig_scene*yfig_cursce_scene (Cyfig_mw* ymw);
extern Tfigure*   yfig_cursce_fig   (Cyfig_mw* ymw);
extern Tfig_histo*yfig_cursce_histo (Cyfig_mw* ymw);
extern TobjDir    yfig_cursce_topDir(Cyfig_mw* ymw);
extern TobjAll    yfig_cursce_curDir(Cyfig_mw* ymw);

/*======================================================================*/
/*= drawing the current scene.                                         =*/

/**
 * The yfig_cursce_clear clears the drawing area of the ymw YFIG-top-window.
 * The yfig_cursce_draw draws into the drawing area of ymw YFIG-top-window
 * the current scene.
 * The yfig_cursce_redraw chains the yfig_cursce_clear/draw functions.
**/
//extern void yfig_cursce_clear            (Tyfig_mw*ymw);
//extern void yfig_cursce_draw             (Tyfig_mw*ymw);
extern void yfig_cursce_redraw           (Tyfig_mw*ymw);
extern void yfig_cursce_redraw_objAdded  (Tyfig_mw*ymw, TobjAll obj);
extern void yfig_cursce_redraw_objDeleted(Tyfig_mw*ymw, TobjAll obj);
extern void yfig_cursce_redraw_update    (Tyfig_mw*ymw);
extern void yfig_cursce_setMagnetGrid    (Tyfig_mw*ymw);
extern void yfig_cursce_draw_obj(Tyfig_mw*ymw, Cobject o, Tod_GCdef gcd);

/*======================================================================*/
/*= Modifying picture of the current scene.                            =*/

extern void    yfig_cursce_movObjNHis   (Tyfig_mw* ymw, TobjAll obj, Tdble2 tv);
extern void    yfig_cursce_movObjWHis   (Tyfig_mw* ymw, TobjAll obj, Tdble2 tv);
extern void    yfig_cursce_supObjNHis   (Tyfig_mw* ymw, TobjAll obj);
extern void    yfig_cursce_supObjWHis   (Tyfig_mw* ymw, TobjAll obj);
extern void    yfig_cursce_supObjSetNHis(Tyfig_mw* ymw, TobjAll*objs, int objnb);
extern void    yfig_cursce_supObjSetWHis(Tyfig_mw* ymw, TobjAll*objs, int objnb);
extern void    yfig_cursce_addObjNHis   (Tyfig_mw* ymw, TobjAll obj);
extern void    yfig_cursce_addObjWHis   (Tyfig_mw* ymw, TobjAll obj);
extern void    yfig_cursce_chgObjNHis   (Tyfig_mw* ymw, TobjAll obj, TobjAll newobj);
extern void    yfig_cursce_chgObjWHis   (Tyfig_mw* ymw, TobjAll obj, TobjAll newobj);
extern void    yfig_cursce_fltObjNHis   (Tyfig_mw* ymw, TobjAll obj); // flat dir
extern void    yfig_cursce_fltObjWHis   (Tyfig_mw* ymw, TobjAll obj);
extern TobjAll yfig_cursce_mkdObjNHis   (Tyfig_mw* ymw, TobjAll*objs, int objnb);
extern TobjAll yfig_cursce_mkdObjWHis   (Tyfig_mw* ymw, TobjAll*objs, int objnb);

/*======================================================================*/
/*= Setting arrows of an object of the current scene.                  =*/

extern void yfig_cursce_chgObjArrowBeg   (Tyfig_mw* ymw, TobjAll obj);
extern void yfig_cursce_chgObjArrowEnd   (Tyfig_mw* ymw, TobjAll obj);
extern void yfig_cursce_chgObjArrowBegEnd(Tyfig_mw* ymw, TobjAll obj);

/*======================================================================*/

extern Twidm_bar*        yfig_topbar_nbf    (Tyfig_mw* ymw);
extern  void             yfig_topbar_delete (Tyfig_mw* ymw, Twidm_bar* tb);
extern Twid_buttonPanel* yfig_modePanel_nbf (Tyfig_mw* ymw);
extern Twid_buttonPanel* yfig_dppanel_nbf   (Tyfig_mw* ymw);
extern Tvgui_dw*         yfig_layers_nbf    (Tyfig_mw* ymw, Tegs_widget pw);
extern void              yfig_layers_delete (Tyfig_mw* ymw, Tvgui_dw* dw);

/*======================================================================*/
/*= GUI updating                                                       =*/

// Update the drawing parameters (ymw_currDP) and the dppanel widget
// (ymw_currYDP) to the ymw_scene scene.
// So it must be called each time a new scene/figure is attached.
extern void yfig_dppanel_update(Tyfig_mw* ymw);

/**
 * The yfig_layers_upateFull function initializes fully the layer dialog
 * of the ymw YFIG widget with the depths found in its scene (ymw->ymw_scene).
 *
 * The yfig_layers_updateAddObj function updates the layer dialog when
 * obj object is added to the current figure.
 *
 * The yfig_layers_updateSupObj function updates the layer dialog when
 * the obj object is suppressed from current figure.
**/ 
extern void yfig_layers_updateFull(Tyfig_mw*ymw);
extern void yfig_layers_update(Tyfig_mw*ymw);
extern void yfig_layers_updateAddObj(Tyfig_mw*ymw, Tobject obj);
extern void yfig_layers_updateSupObj(Tyfig_mw*ymw, Tobject obj);

extern void yfig_canvas_updateUnit          (Tyfig_mw*ymw, CuserUnit* uu);
extern void yfig_canvas_updateMinorGridTick (Tyfig_mw*ymw, double stn);
extern void yfig_canvas_updateMagnetGridTick(Tyfig_mw*ymw, double stn);
extern void yfig_canvas_update              (Tyfig_mw*ymw, double zf);

extern void yfig_canvas_enableAxis          (Tyfig_mw*ymw, int on);
extern void yfig_canvas_enableGrid          (Tyfig_mw*ymw, int on);
extern void yfig_canvas_enableMagnet        (Tyfig_mw*ymw, int on);

/*======================================================================*/

#define YFIG_TB_FILE 0x01
#define YFIG_TB_EDIT 0x02
#define YFIG_TB_ALL  0x03
extern void yfig_topbar_update(Cyfig_mw* ymw, int mask);

/*======================================================================*/
/*= Drawing options                                                    =*/

Inline int ymw_drawopt_scaleText   (Cyfig_mw*ymw) { return ymw->ymw_do_scaleText; }
Inline int ymw_drawopt_scaleStyle  (Cyfig_mw*ymw) { return ymw->ymw_do_scaleStyle; }
Inline int ymw_drawopt_symrotCenter(Cyfig_mw*ymw) { return ymw->ymw_do_symrotCtr; }

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_YFIG_MW_H
/*======================================================================*/
// vim: set textwidth=100:
