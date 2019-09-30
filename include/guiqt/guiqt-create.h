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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: guiqt-create.h 63 2017-04-22 08:03:26Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-create.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for creating widgets.                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_CREATE_H
#define FILE_GUIQT_CREATE_H
/*======================================================================*/

#include "vgui/wid-simple.h"

#if 0
/*======================================================================*/
/*= label widget                                                       =*/

extern Tegs_widget create_label(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text);

#define create_label_short(gui,pw,wn,text) create_label(\
        gui,pw,wn,0,-1,-1,EGSCOLOR_DEFAULT,EGSCOLOR_DEFAULT,text)

extern Widget create_label_place(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn, CstrBrief text,
        Tegs_widget lw, Tegs_widget tw);

/*======================================================================*/
/*= button widget                                                      =*/

extern Tegs_widget create_button(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, CstrBrief event, void*cb, void*ud1, void*ud2);

extern Tegs_widget create_menuButton(
        Tvgui* gui, Widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, CstrBrief menuname);

/*======================================================================*/
/*= text input widget                                                  =*/

extern Twid_slin* create_slin(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, Twid_cb_TU chg, Twid_cb_TU val, void*ud);

extern Twid_slin* create_slin_place(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn, int dx, int dy,
        CstrBrief text, Twid_cb_TU chg, Twid_cb_TU val, void*ud,
        Tegs_widget lw, Tegs_widget tw);

/*======================================================================*/
/*= container widgets                                                  =*/

extern Tegs_widget create_shell(
        Tvgui* gui, const char* wn,
        int bw, int x, int y, TcolorEGS bg, TcolorEGS fg,
        const char* _title);
#endif

extern Tegs_widget create_form(
        Tvgui* gui, Tegs_widget parent, const char* wn,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg);

#if 0
extern Tvgui_dw* create_dwform(Tvgui_dw* vdw,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg);

/*======================================================================*/
/*= string list widget                                                 =*/

extern Tegs_widget create_listString(Tegs_widget*viewport,
        Tvgui* gui, Tegs_widget parent, const char* wn,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        XtCallbackProc cb, void* ud);

// nb can be null if strs is null terminated
extern void xt_listString_chgStrs(Tegs_widget list, char** strs, int nb);
#endif

/*======================================================================*/
#endif // FILE_GUIQT_CREATE_H
/*======================================================================*/
