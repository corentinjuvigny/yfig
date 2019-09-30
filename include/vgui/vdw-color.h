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
 *      $Id: vdw-color.h 297 2018-07-12 15:26:53Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vdw-color.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widgets for selecting, creating colors.                     =*/
/*=                                                                    =*/
/*= Tvgui_dw_colorSel: This class allows to user to select a           =*/
/*=   predefined color. The widget layout is:                          =*/
/*=       title                                                        =*/
/*=       C1 C2 C3 ....                                                =*/
/*=       Ci Cj Ck ....                                                =*/
/*=       ...                                                          =*/
/*=                                                                    =*/
/*= Tvgui_dw_colorCte: This class allows to user to create new         =*/
/*=   color. The widget layout is:                                     =*/
/*=       title                                                        =*/
/*=       R G B lock H S V                                             =*/
/*=       S S S   S  S S S                                             =*/
/*=       S S S   S  S S S                                             =*/
/*=       S S S   S  S S S                                             =*/
/*=        viewver-viewer                                              =*/

/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_DW_COLOR_H
#define FILE_VGUI_DW_COLOR_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-dw.h"

/*======================================================================*/
/*= Color selector                                                     =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Data structure                                                     =*/

typedef struct _Tvdw_colorSel Tvdw_colorSel;
typedef const   Tvdw_colorSel Cvdw_colorSel;

typedef void (*Tvdw_colorSel_cb)(Tvdw_colorSel*self, Ccolor*c, void*ud);

struct _Tvdw_colorSel {
    Tvgui_dw           any;
    Twid_buttonPanel*  btns;    // color panel
    Tvdw_colorSel_cb   cb;
    void*              ud;
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= VGUI interface                                                     =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Constructors & destructor                                          =*/

/**
 * The vdwD_colorsel_new function allocates a color selector dialog
 * and returns it. The dialog title is set to str, and it shows
 * the nb first colors defined in the cs array.
 *
 * The vdw_colorsel_dbf function builds and does the layout of
 * the self color selector widget. The widget is built as child
 * of the pw widget or of the vdw dialog if the pw pointer is NULL. 
 *
 * The vdw_colorsel_free function frees all the memory that the
 * self widget allocated.
 *
 * Parameter:
 *  - vdm:     The parent widget
 *  - str:     The widget title. A NULL value means no title.
 *  - cs,nb:   The nb colors that can be selected.
 *             Both can be null.
 *  - cb,ud:   When the user clicks a color, the widget triggers the
 *             cb callaback: cb(vdw,c,ud)
 *             Both can be null.
 *  - pw:      the parent widget or NULL.
**/
extern Tvdw_colorSel*
               vdwD_colorsel_new (Tvgui_dw* vdw, TstrEaten str,
                                  Tvdw_colorSel_cb cb, void* ud,
                                  Tcolor**cs, int nb);
extern Tvdw_colorSel*
               vdwD_colorsel_newWN(Tvgui_dw* vdw, TstrEaten str,
                                  Tvdw_colorSel_cb cb, void* ud,
                                  CstrBrief nonefirst,
                                  Tcolor**cs, int nb,
                                  CstrBrief nonelast);
extern void    vdw_colorsel_dbf  (Tvdw_colorSel* self, Tegs_widget pw);
extern void    vdw_colorsel_free (Tvdw_colorSel* self);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Operations                                                         =*/

/**
 * The vdw_colorsel_set function fixes the layout parameters
 * of the self widget.
 * It must be invocated before vdwD_colorsel_dbf.
 * 
 * Parameter:
 *  - self:    The widget to configure.
 *  - col,row: The numbers of rows and columns of the color panel.
 *  - bdx,bdy: The width and height of the color buttons.
 * Note: Negative or null values mean either to use default or to
 * compute it automatically.
**/
extern void    vdw_colorsel_set  (Tvdw_colorSel* self,
                                  int col, int row, int bdx, int bdy);

extern void    vdw_colorsel_addC (Tvdw_colorSel* self, Ccolor* c, int finish);
extern void    vdw_colorsel_supC (Tvdw_colorSel* self, Ccolor* c);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Quick constructors                                                 =*/

/**
 * The vdwD_colorsel_nbf function chains the functions
 * vdwD_colorsel_new, vdwD_colorsel_set and vdwD_colorsel_dbf
 * for building a complete widget.
 *
 * The number of rows is adjusted to show all the colors in the
 * widget.
 *
 * The vdwD_colorsel_nbfWN function is similar to the vdwD_colorsel_nbf
 * function, but it adds pseudo color button containing the none string.
 * The pos parameter indicates where the pseudo button is placed.
 *   - 0: No pseudo button.
 *   - 1: The pseudo button is placed as the first button.
 *   - 2: The pseudo button is placed as the last button.
 *   - 3: Union of 1 and 2 (2 pseud buttons).
**/
extern Tvdw_colorSel*
               vdwD_colorsel_nbf  (Tvgui_dw* vdw, TstrEaten str,
                                   Tcolor**cs, int nb,
                                   Tvdw_colorSel_cb cb, void* ud,
                                   int col, int row);

extern Tvdw_colorSel*
               vdwD_colorsel_nbfWN(Tvgui_dw* vdw, TstrEaten str,
                                   Tcolor**cs, int nb,
                                   Tvdw_colorSel_cb cb, void* ud,
                                   int col, int row,
                                   int pos, CstrBrief none);

/*======================================================================*/
/*= Color creator                                                      =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Data structure                                                     =*/

typedef struct _Tvdw_colorCre Tvdw_colorCre;
typedef const   Tvdw_colorCre Cvdw_colorCre;

typedef void (*Tvdw_colorCre_cb)(Tvdw_colorCre*self, Ccolor*c, void*ud);

struct _Tvdw_colorCre {
    Tvgui_dw           any;
    Tcolor*            color;       // the current color (eated)
    Tvdw_colorCre_cb   cb;
    void*              ud;
    Twid_button*       boxes[7];    // label
    Twid_slider*       sliders[7];
    Tegs_widget        viewer;      // label
    int                lockmask;    // 1:red 2:green 4:blue
    int                lock255max;  // the maximal value of locked RGB
    int                lock255min;  // the minimal value of lock allowed
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= VGUI interface                                                     =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Constructors & destructor                                          =*/

/**
 * The vdwD_colorcre_nbf function creates a color creator sub-dialog
 * and retuns it. The dialog title is set to str, and it shows the
 * ic colors. Its sliders have a wu pixel width and a hu pixel height. 
 *
 * Parameters:
 *  - vdw   : The parent dialog.
 *  - str   : The widget title. A NULL value means no title.
 *  - cb,ud : When the user changes a color element, the widget triggers
 *            the cb callaback: cb(vdw,c,ud)
 *  - ic    : The initial color. A Null value disables the dialog.
 *  - wu,hu : The size of the sliders in pixel.
**/
extern Tvdw_colorCre* vdwD_colorcre_nbf  (Tvgui_dw* vdw, TstrEaten str,
                                   Tvdw_colorCre_cb cb, void* ud,
                                   Tcolor*ic, int wu, int hu);
extern void    vdw_colorcre_free (Tvdw_colorCre* self);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Operations                                                         =*/

extern void    vdw_colorcre_enable  (Tvdw_colorCre* self, Ccolor* ic);
extern void    vdw_colorcre_viewer  (Tvdw_colorCre* self, Ccolor* ic);
extern void    vdw_colorcre_disable (Tvdw_colorCre* self);

/*======================================================================*/
/*= Debug                                                              =*/

#define DBG0_Cdw(fmt,...)  DBG0_printf("dwcolor",fmt,__VA_ARGS__)
#define DBG1_Cdw(fmt,...)  DBG1_printf("dwcolor",fmt,__VA_ARGS__)
#define DBG2_Cdw(fmt,...)  DBG2_printf("dwcolor",fmt,__VA_ARGS__)

#define DBG0_CdwE(fmt,...) DBG0_printf("dwcolor","Enter : "fmt,__VA_ARGS__)
#define DBG0_CdwM(fmt,...) DBG0_printf("dwcolor","Middle: "fmt,__VA_ARGS__)
#define DBG0_CdwL(fmt,...) DBG0_printf("dwcolor","Leave : "fmt,__VA_ARGS__)
#define DBG1_CdwE(fmt,...) DBG1_printf("dwcolor","Enter : "fmt,__VA_ARGS__)
#define DBG1_CdwM(fmt,...) DBG1_printf("dwcolor","Middle: "fmt,__VA_ARGS__)
#define DBG1_CdwL(fmt,...) DBG1_printf("dwcolor","Leave : "fmt,__VA_ARGS__)
#define DBG2_CdwE(fmt,...) DBG2_printf("dwcolor","Enter : "fmt,__VA_ARGS__)
#define DBG2_CdwM(fmt,...) DBG2_printf("dwcolor","Middle: "fmt,__VA_ARGS__)
#define DBG2_CdwL(fmt,...) DBG2_printf("dwcolor","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_DW_COLOR_H
/*======================================================================*/
