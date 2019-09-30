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
 *$Software: YFIG-WIN32 (Interface to WIN32 EGS under MinGw)
 *      $Id: guimingw-top.h 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guimingw/guimingw-top.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups the EGS resources and basic utilities that are =*/
/*= required everywhere.                                               =*/
/*=                                                                    =*/
/*= The EGS resources are grouped in the Tgui_main structure.          =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIMINGW_TOP_H
#define FILE_GUIMINGW_TOP_H
/*======================================================================*/

#include "guimingw-config.h"

#include "vgui/vgui.h"
#include "vgui/vgui-names.h"

#include "tools/color.h"

/*======================================================================*/
/*= EGS resources and shortcut                                         =*/

#define E_hInstance   gui->g_main->m_hInstance
#define E_nCmdShow    gui->g_main->m_nCmdShow

struct _Tgui_main {
    // initialized in gui_open
    HINSTANCE m_hInstance;
    int       m_nCmdShow;
    ATOM      m_AtomSpinner;  // spinner class
#if 0
    //Widget       m_topLevel;   // top widget ORG:tool created by XtGetApplicationResources
    Widget       m_topWidget;    // top widget ORG:tool created by XtGetApplicationResources
                                 // top widget ORG:tool created by XtAppCreateShell
    XtAppContext m_appContext;      // ORG:tool_app
    Display*     m_display;         // ORG:tool_d
    Screen*      m_screen;          // ORG:tool_s
    int          m_screenNum;       // ORG:tool_sn
    //int          m_screenWidth;     // ORG:screen_wd
    //int          m_screenHeight;    // ORG:screen_ht
    Visual*      m_visual;          // ORG:tool_v
#define m_visualClass m_visual->class  // ORG: tool_vclass
    int          m_depth;           // ORG:tool_dpth
    Colormap     m_colorMap;        // ORG:tool_cm
    int          m_bytePerPixel;    // ORG:image_bpp

    Atom         m_wm_delete_window;
    Atom         m_wm_save_yourself;

    // opaque pango xft fontconfig
    void*        m_pango;

    // the xfig windows
    Tgui_widgetGeometry             // geometries of main widgets
                m_widgetGeometry;

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
#endif
} ;

/*======================================================================*/
/*= Macros for transcribing types                                      =*/

Inline COLORREF  CLRegs2mingw(TcolorEGS c) { return c.uint; }
Inline TcolorEGS CLRmingw2egs(COLORREF  c) { return any_initBySInt(c); }

#if 0
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
#endif

/*======================================================================*/
#endif // FILE_GUIMINGW_TOP_H
/*======================================================================*/
