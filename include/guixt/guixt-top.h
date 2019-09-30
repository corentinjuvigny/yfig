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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-top.h 342 2018-11-08 00:14:55Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guixt/guixt-top.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups the EGS resources and basic utilities that are =*/
/*= required everywhere.                                               =*/
/*=                                                                    =*/
/*= The EGS resources are grouped in the Tgui_main structure.          =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_TOP_H
#define FILE_GUIXT_TOP_H
/*======================================================================*/

#include "guixt-config.h"

#include "vgui/vgui.h"
#include "vgui/vgui-names.h"

#include "tools/color.h"

/*======================================================================*/
/*= Macros for transcribing types                                      =*/

Inline long      CLRegs2xt (TcolorEGS c) { return c.slong; }
Inline TcolorEGS CLRxt2egs (long c)      { return any_initBylong(c); }
Inline long      CLRvgui2xt(Ccolor*   c) { return CLRegs2xt(c->egs); }

/*======================================================================*/
/*= EGS resources and shortcut                                         =*/

#define E_Display      gui->g_main->m_display
#define E_Screen       gui->g_main->m_screen
#define E_ScreenNum    gui->g_main->m_screenNum
#define E_Visual       gui->g_main->m_visual
#define E_Depth        gui->g_main->m_depth
#define E_ColorMap     gui->g_main->m_colorMap
#define E_GCcache(gcd) gui->g_main->m_gccache[gcd]
#define E_Pango        ((Tgui_pango*)gui->g_main->m_pango)

#define E_AppContext   gui->g_main->m_appContext
#define E_TopWidget    gui->g_main->m_topWidget

struct _Tgui_main {
    XtAccelerators modePanelTable;
    // initialized in gui_open
    Widget       m_topWidget;    // top widget ORG:tool created by XtGetApplicationResources
                                 // top widget ORG:tool created by XtAppCreateShell
    XtAppContext m_appContext;      // ORG:tool_app
    Display*     m_display;         // ORG:tool_d
    Screen*      m_screen;          // ORG:tool_s
    int          m_screenNum;       // ORG:tool_sn
    Visual*      m_visual;          // ORG:tool_v
#define m_visualClass m_visual->class  // ORG: tool_vclass
    int          m_depth;           // ORG:tool_dpth
    Colormap     m_colorMap;        // ORG:tool_cm

    Atom         m_wm_delete_window;
    Atom         m_wm_save_yourself;

    // opaque pango xft fontconfig
    void*        m_pango;

    // Graphic Context
    GC m_gccache[OD_GC_NB];

    // Bitmap & Pixmap
    Pixmap      m_pmMenuArrow;       // added to button that triggers DD list
    Pixmap      m_pmMenuEntryArrow;  // added to DD List entry that triggers
                                     // sub menu 
    Pixmap      m_pmSpinup;          // spinner pm to increase
    Pixmap      m_pmSpindown;        // spinner pm to decreases

    Pixmap      m_bmCmBoxOnSm;       // small checkmark box on ( 13x13 )
    Pixmap      m_bmCmBoxOffSm;      // small checkmark box off ( 13x13 )
} ;

/*======================================================================*/
/*= some macros for colors                                             =*/

#define GUI_RGB(r,g,b) "rgb:"#r"/"#g"/"#b

/*======================================================================*/
/**
 * Macros for using XtSetArg easily:
 *
 *  void blah()
 *  {
 *  XT_DeclareArgs(10);
 *    ...
 *    XT_FirstArg(XmNx, 100);
 *    XT_NextArg(XmNy, 80);
 *    button = XmCreatePushButton(parent, name, Args, ArgCount);
 *  }
**/

#define ArgCount    _ArgCount
#define Args        _ArgList
#define ArgCountMax _ArgCountMax

#define XT_DeclareArgs(n)   Arg Args[n]; int ArgCountMax = n; int ArgCount=0

#define XT_DeclareArgsStatic(n)  static Arg Args[n]; \
                                 static int ArgCountMax = n; \
                                 static int ArgCount=0

#define XT_FirstArg(name, val) \
    do { XtSetArg(Args[0], (name), (val)); ArgCount=1;} while (0)
#define XT_NextArg(name, val) \
    do { TEMP_asf_printf(ArgCount >= ArgCountMax,\
         "too many args in XT_NextArg (max=%d)",ArgCountMax ); \
      XtSetArg(Args[ArgCount], (name), (val)); ArgCount++;} while (0)

#define XT_GetValues(n) XtGetValues(n, Args, ArgCount)
#define XT_SetValues(n) XtSetValues(n, Args, ArgCount)

/*======================================================================*/
#endif // FILE_GUIXT_TOP_H
/*======================================================================*/
