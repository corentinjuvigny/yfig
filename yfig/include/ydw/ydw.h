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
 *      $Id: ydw.h 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/ydw/ydw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous dialog and sub-dialog widget.                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YDW_YDW_H
#define FILE_YDW_YDW_H
/*======================================================================*/

#include "yfig-config.h"
#include "vgui/vdw-color.h"

/*======================================================================*/

#define YDW_LAB_WITH    "MMMMMMMMMM"
#define YDW_VAL_WITH    "MMMMMM"
#define YDW_VIEWER_WITH "XXXXX"

#define YDW_CLR_NbCol 13
#define YDW_CLR_BtnDx 20
#define YDW_CLR_BtnDy 15

#define YDW_FIL_NbCol YDW_CLR_NbCol
#define YDW_FIL_PxmDx 30
#define YDW_FIL_PxmDy 25

/*======================================================================*/
/*=  Dialog that selects colors.                                      =*/

/**
 * Performs cb(vdw,c,ud) every time that the user selects a color.
 *   vdw is a VGUI widget (Tvdw_colorSel) and has no interest.
 *   c is the selected color.
 *   ud is the parameter given in the constructor.
**/
extern Tvgui_dw* ydw_colors_nbf(
        Tvgui_dw* vdw, // the parent dialog
        Tyfig_mw* ymw, // ymw->ymw_figure->colors
        Ccolor*   ic,  // init color
        Tvdw_colorSel_cb cb, void*ud);

extern Tvgui_dw* ydw_colors_nbfWN(
        Tvgui_dw* vdw, // the parent dialog
        Tyfig_mw* ymw, // ymw->ymw_figure->colors
        Ccolor*   ic,  // init color
        Tvdw_colorSel_cb cb, void*ud,
        CstrBrief none); // none=NULL --> "none" 

extern void    ydw_colors_free  (Tvgui_dw* vdw);
extern Ccolor* ydw_colors_color (Tvgui_dw* vdw);
extern void    ydw_colors_addClr(Tvgui_dw* vdw, Ccolor*c);
extern void    ydw_colors_delClr(Tvgui_dw* vdw, Ccolor*c);

/**
 * Full dialog that allows to the user to select the pen, fill and
 * text colors.
**/

Twid_dialStatus ydw_clrPFT_start(Tyfig_mw* ymw,
                   Tfig_colors* figcolors,
                   int isel, // initial selected color: P, F or T
                   Tcolor**pen,Tcolor**fill,Tcolor**text);

/*======================================================================*/
/*=  Sub-dialog that handles pen parameters for text object.           =*/

typedef struct _Tydw_tpen Tydw_tpen;
typedef const   Tydw_tpen Cydw_tpen;
typedef         void    (*Tydw_tpen_cb) (Tydw_tpen*tpen, void *ud);

extern Tvgui_dw*  ydw_tpen_nbfTop(Tvgui_dw* vdw, // the parent dialog
                                  Tyfig_mw* ymw, // ymw->ymw_figure->colors
                                  Csty_text*dpt, int dir, // dir: OD_SORG_...
                                  Tydw_tpen_cb cb, void*ud);
extern Tydw_tpen* ydw_tpen_nbfSub(Tvgui_dw* vdw, // the parent dialog
                                  Tyfig_mw* ymw, // ymw->ymw_figure->colors
                                  Csty_text*dpt, int dir, // dir: OD_SORG_...
                                  Tydw_tpen_cb cb, void*ud);
extern void       ydw_tpen_free(Tydw_tpen* self);

extern Tegs_widget ydw_tpen_widEGS      (Tydw_tpen* self);
extern int         ydw_tpen_dirSTY      (Tydw_tpen* self); // return STY_T_...  macros
extern Csty_text*  ydw_tpen_dpt         (Tydw_tpen* self);
extern void        ydw_tpen_showClr     (Tydw_tpen* self);
extern void        ydw_tpen_hideClr     (Tydw_tpen* self);

/*======================================================================*/
/*=  Sub-dialog that handles pen parameters.                           =*/

typedef void  (*Tydw_pen_cb) (void *ud, Csty_line* dpl);
typedef struct _Tydw_pen Tydw_pen;
typedef const   Tydw_pen Cydw_pen;

extern Tvgui_dw* ydw_pen_nbfTop(Tvgui_dw* vdw, // the parent dialog
                                Tyfig_mw* ymw, // ymw->ymw_figure->colors
                                Csty_line*dpl, Tydw_pen_cb cb, void*ud,
                                int curveonly);
extern Tydw_pen* ydw_pen_nbfSub(Tvgui_dw* vdw, // the parent dialog
                                Tyfig_mw* ymw, // ymw->ymw_figure->colors
                                Csty_line*dpl, Tydw_pen_cb cb, void*ud,
                                int curveonly, TobjAll obj);
extern void      ydw_pen_free(Tydw_pen* self);

extern Tegs_widget ydw_pen_widEGS      (Tydw_pen* self);
extern Csty_line*  ydw_pen_dpl         (Tydw_pen* self);
extern int         ydw_pen_arrowMode   (Tydw_pen* self, int atBeg);
extern CarrowDesc* ydw_pen_arrowDesc   (Tydw_pen* self, int atBeg);
extern void        ydw_pen_showClr     (Tydw_pen* self);
extern void        ydw_pen_hideClr     (Tydw_pen* self);
extern void        ydw_pen_setStyleMask(Tydw_pen* self, TstyleMask mask);

/*======================================================================*/
/*=  Sub-dialog that handles filling area parameters.                  =*/

typedef void  (*Tydw_fill_cb) (void *ud, Csty_fill* dpf);

typedef struct _Tydw_fill Tydw_fill;
typedef const   Tydw_fill Cydw_fill;

extern Tydw_fill* ydw_fill_nbf (Tvgui_dw* vdw, // the parent dialog
                                Tyfig_mw* ymw, // ymw->ymw_figure->colors
                                Csty_fill*dpf, Tydw_fill_cb cb, void*ud);
extern void       ydw_fill_free(Tydw_fill* self);

extern Tegs_widget ydw_fill_widEGS     (Tydw_fill* self);
extern Csty_fill*  ydw_fill_dpf        (Tydw_fill* self);
extern void        ydw_fill_unfoldColor(Tydw_fill* self);
extern void        ydw_fill_unfoldTile (Tydw_fill* self);

/*======================================================================*/
/*=  Dialog that handles parameters for exporting/printing figure.     =*/

typedef struct _Tydw_expopt Tydw_expopt;
typedef const   Tydw_expopt Cydw_expopt;

extern Tydw_expopt* ydw_expopt_nbf   (Tyfig_mw* ymw, CexportOption* eo,
                                      int flag, // 0:export ; 1:print ; ?:print
                                      Twid_cb_DU cb, void* ud);
extern void         ydw_expopt_delete(Tydw_expopt* self);

extern Twid_dialStatus ydw_expopt_run (Tydw_expopt* self);
extern CexportOption*  ydw_expopt_get (Tydw_expopt* self);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Interface of the export/printing dialog.                           =*/
/*=                                                                    =*/
/*= The ydw_export_nbf function creates the dialog that export the     =*/
/*= the figure that the ymw main window draws currently.               =*/
/*= The ydw_export_delete function frees all the memory that the self  =*/
/*= dialog allocated.                                                  =*/
/*= The ydw_export_run function runs the self dialog in modal mode.    =*/
/*= The ydw_export_updateOption function backups the export image      =*/
/*= format, options, ouput directory in YFIG data structure.           =*/
/*= The ydw_export_do function performs the export of the current      =*/
/*= figure using the parameters stored in the dialog.                  =*/
/*=                                                                    =*/
/*= The standard usage of these functions is:                          =*/
/*=     Tydw_export* dialog = ydw_export_nbf(ymw,0);                   =*/
/*=     if ( ydw_export_run(dialog)==WID_DIALSTATUS_Ok ) {             =*/
/*=        // CAUTION: the dialog does not perform the export.         =*/
/*=        // it just allows the user to choose the options.           =*/
/*=        ydw_export_do(dialog);           // do the export           =*/
/*=        ydw_export_updateOption(dialog); // optional                =*/
/*=     }                                                              =*/
/*=     ydw_export_delete(dialog);                                     =*/
/*=                                                                    =*/
/*= Return:                                                            =*/
/*=  - ydw_export_nbf always returns a pointer to the created dialog.  =*/
/*=  - ydw_export_run returns WID_DIALSTATUS_Ok when user asked for    =*/
/*=    exporting otherwise an other value.                             =*/
/*=  - On success, ydw_export_do returns 0 otherwise it returns 1.     =*/

typedef struct _Tydw_export Tydw_export;
typedef const   Tydw_export Cydw_export;

extern Tydw_export* ydw_export_nbf   (Tyfig_mw* ymw, 
                                      int flag); // 0:export ; 1:print ; ?:print
extern void         ydw_export_delete(Tydw_export* self);

extern Twid_dialStatus ydw_export_run         (Tydw_export* self);
extern void            ydw_export_updateOption(Tydw_export* self);
extern int             ydw_export_do          (Tydw_export* self);

/*======================================================================*/
#endif  // FILE_YDW_YDW_H
/*======================================================================*/
