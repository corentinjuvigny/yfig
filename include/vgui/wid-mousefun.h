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
 *      $Id: wid-mousefun.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-mousefun.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A widget that shows an help about mouse buttons.                   =*/
/*=                                                                    =*/
/*= - The button and texts are drawn using the VGUI_GC_MWpaint GC.     =*/
/*= - The size of the drawing area is automatically computed:          =*/
/*=    * the width  is set to 3.0*<width  of MOUSEFUN_WD_STR string>   =*/
/*=      plus 20%.                                                     =*/
/*=    * the height is set to 2.5*<height of MOUSEFUN_WD_STR string>.  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_MOUSEFUN_H
#define FILE_WID_MOUSEFUN_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

#define MOUSEFUN_WD_STR   "Mouse Buttons"

/*======================================================================*/
/*= The mouse fun widget                                               =*/

struct _Twid_mousefun {
    Twid_canvas *mf_canvas;
#define mf_widEGS mf_canvas->vgcm_widEGS
#define mf_device mf_canvas->vgcm_drawingArea.device
#define mf_outdev mf_canvas->vgcm_outdev
    TstrEaten    mf_leftTitle;
    TstrEaten    mf_midTitle;
    TstrEaten    mf_rightTitle;
};

extern Twid_mousefun* wid_mfun_create(Tvgui_mw* vmw, Tegs_widget parent);
extern void           wid_mfun_delete(Twid_mousefun* mf);

extern void           wid_mfun_clear(Twid_mousefun* mf);
extern void           wid_mfun_show (Twid_mousefun* mf,
                            CstrBrief l, CstrBrief m, CstrBrief r);

/*======================================================================*/
/*= Quick interface to mouse fun widget.                               =*/

struct _Twid_mousefunDt {
    Twid_mousefun*    widget;
    Cwid_mousefun3S*  messages;
};

/**
 * The wid_mfun_showDt function is a shortcut to:
 *    wid_mfun_show(dt->widget, dt->messages[n].str[0],
 *      dt->messages[n].str[1], dt->messages[n].str[2]);
 *
 * if dt or dt->widget is null the function does nothing. 
**/
extern void wid_mfun_showDt(Twid_mousefunDt* dt, int n);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_MOUSEFUN_H
/*======================================================================*/
