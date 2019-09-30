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
 *      $Id: wid-egs.h 134 2018-05-19 16:44:11Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-egs.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* Operations on EGS widget.                                           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_WIDGET_H
#define FILE_VGUI_WIDGET_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

/*======================================================================*/
/*= EGS widget set                                                     =*/

typedef struct _Tegs_widSet    Tegs_widSet;
typedef const   Tegs_widSet    Cegs_widSet;

struct _Tegs_widSet {
    int          eleMemNb; // element number of the memory block eles.
    int          eleNb;    // element number of set (eleNb<eleMemNb)
    Tegs_widget* eles;     // the set elements
};

extern Tegs_widSet*  egswidset_new  (int n);
extern void          egswidset_free (Tegs_widSet* s);
extern void          egswidset_add  (Tegs_widSet* s, Tegs_widget p);
extern Tegs_widget*  egswidset_first(Cegs_widSet* s);
extern Tegs_widget*  egswidset_last (Cegs_widSet* s);

/*======================================================================*/
/*= Interface to EGS widget                                            =*/

/**
 * The gui_widegs_addTip function enables the hs tip to the w widget.
 *
 * The gui_widegs_addTipAA appends to the hs string the a1 and a2
 * accelaration keys under an human format. Then it enables the
 * tip to the w widget with the new string
 *
 * If hs is the null pionter, the functions immediately returns.
 *
 * Parameters:
 *  - w  : the EGS widget.
 *  - hs : the help string.
 *  - a1 : the first accelaration key (0 means no accelaration key).
 *  - a2 : the second accelaration key (0 means no accelaration key).
**/
void gui_widegs_addTip  (Tegs_widget w, CstrBrief hs);
void gui_widegs_addTipAA(Tegs_widget w, CstrBrief hs,
                         TaccelDef a1, TaccelDef a2);

/**
 * The vgui_widegs_putW1inNEofW2 function places the w1 widget
 * in the north-East corner of the W2 widget.
**/
#define vgui_widegs_putW1inNEofW2(g,w,p) gui_widegs_putW1inNEofW2(g,w,p)
void  gui_widegs_putW1inNEofW2(Tvgui*gui,Tegs_widget w1,Tegs_widget w2);

/**
 * The gui_widegs_disable function disables the w widget:
 *   - makes the widget insensitive to keyboard and mouse.
 *   - redraws for showing this state (usually less lightly).
 * The gui_widegs_enable function enables again a disabled widget.
 * Calling the gui_widegs_disable (respectively: enable) function
 * on a w widget that is already disabled (respectively: enabled)
 * does nothing.
**/
extern void gui_widegs_disable(Tegs_widget w);
extern void gui_widegs_enable (Tegs_widget w);

/**
 * The gui_widegs_unshow function unshows the w widget and
 * all its children.
 * The gui_widegs_show function shows again an unshown widget.
 *
 * Calling the gui_widegs_unshow (respectively: show) function
 * on a w widget that is already unshown (respectively: shown)
 * does nothing.
**/
extern void gui_widegs_unshow (Tegs_widget w);
extern void gui_widegs_show   (Tegs_widget w);
extern int  gui_widegs_isShown(Tegs_widget w);

/**
 * Append to sed the children of w widget.
**/

extern void gui_widegs_children(Tegs_widget w, Tegs_widSet* set);

/*======================================================================*/

extern void gui_widegs_setFgColor (Tegs_widget w, TcolorEGS c);
extern void gui_widegs_setBgColor (Tegs_widget w, TcolorEGS c);
extern void gui_widegs_setBgPxm   (Tegs_widget w, TpixmapEGS pxm);

extern int  vgui_widegs_getX  (Tegs_widget w);
extern int  vgui_widegs_getY  (Tegs_widget w);
extern int  vgui_widegs_getXSE(Tegs_widget w);
extern int  vgui_widegs_getYSE(Tegs_widget w);
extern int  vgui_widegs_getBDT(Tegs_widget w);
extern int  vgui_widegs_getDX (Tegs_widget w);
extern int  vgui_widegs_getDY (Tegs_widget w);
extern int   gui_widegs_getGeometry(Tegs_widget w,
                int*x, int*y, int*dx, int*dy, int*bdt);

extern void gui_widegs_setDX  (Tegs_widget w, int dx);
extern void gui_widegs_setDY  (Tegs_widget w, int dy);
extern void gui_widegs_setSize(Tegs_widget w, int dx, int dy);

/*======================================================================*/

extern int   vgui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text);
extern int    gui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_WIDGET_H
/*======================================================================*/
