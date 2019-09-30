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
 *      $Id: guimingw-init.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guimingw/guimingw-init.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This files groups the functions that initializes and frees         =*/
/*= the WIN32 Tvgui.                                                   =*/
/*=                                                                    =*/
/*= See the "vgui/vgui.h" header for types and function documentation. =*/
/*======================================================================*/

#include "guimingw.h"

#include "tools/font.h"   // for DBG0/1_Font
#include "outdev/style.h" // for gui_gc_create

/*======================================================================*/
/*= The gui initialization.                                            =*/

extern void gui_open(Tvgui*gui, Tvgui_openParam* param)
{
    gui->g_main = malloc(sizeof(*gui->g_main));
    E_hInstance = param->op_hInstance;
    E_nCmdShow  = param->op_nCmdShow;

    gui->g_externalSystem = GUI_ExtSys_MinGw;

    // screen resolution
    HDC hdc = GetDC(NULL);
    gui->g_pixScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
    gui->g_pixScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    int  mmScreenWidth     = GetDeviceCaps(hdc,HORZSIZE);
    int  mmScreenHeight    = GetDeviceCaps(hdc,VERTSIZE);
    gui->g_ppiScreenWidth  = ((double)gui->g_pixScreenWidth)  / ((double)mmScreenWidth)*25.4;
    gui->g_ppiScreenHeight = ((double)gui->g_pixScreenHeight) / ((double)mmScreenHeight)*25.4;
    ReleaseDC(NULL,hdc);

    // FIXME gui_global_init(gui);
}

extern void gui_open_finish(Tvgui* gui)
{
    // FIXME gui_pixmaps_init(gui);
}

/*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/

extern void gui_close_start(Tvgui* gui)
{
    // FIXME gui_pixmaps_free(gui);
}

extern void gui_close(Tvgui* gui)
{
#if 0 // FIXME
    while ( XtAppPending( gui->g_main->m_appContext ) ) {
        XEvent	    event;
        XtAppNextEvent(gui->g_main->m_appContext,&event);
	    XtDispatchEvent(&event);
    }
    XtDestroyApplicationContext(gui->g_main->m_appContext);
    free( gui->g_main );
#endif
}

/*======================================================================*/
/*= VGUI font initialization                                           =*/

extern void   gui_fonts_init(Tvgui*gui)
{
#if 0 // FIXME
#define D0_MFONT        "-*-times-medium-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D0_MFONTBOLD    "-*-times-bold-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D1_MFONT        "-*-fixed-medium-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D1_MFONTBOLD    "-*-fixed-bold-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D2_MFONT        "8x13"
#define D2_MFONTBOLD    "8x13bold"

#define D0_CFONT        "-*-times-medium-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D0_CFONTBOLD    "-*-times-bold-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D1_CFONT        "-*-fixed-medium-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D1_CFONTBOLD    "-*-fixed-bold-r-normal--%d-0-*-*-*-*-ISO8859-*"
#define D2_CFONT        "8x13"
#define D2_CFONTBOLD    "8x13bold"

    Font font;
    int  sz=16; gui->g_mPts = sz;

    // the default font and the fonts for the main windows and dialogs
    if ( (font=gui_fonts_get(gui,D0_MFONT,sz,D1_MFONT,D2_MFONT))==0 ) {
        vgui_errerr_printf(gui,"can not found a default font");
        vgui_errerr_printf(gui,"  I tried %s",D0_MFONT);
        vgui_errerr_printf(gui,"  I tried %s",D1_MFONT);
        vgui_errerr_printf(gui,"  I tried %s",D2_MFONT);
        vgui_errerr_printf(gui,"  with %d point size.",sz);
        exit(1);
    }
    gui->g_fonts[gui->g_fontNb++] = (Tegs_font) font;
    gui->g_mFont                  = (Tegs_font) font;

    if ( (font=gui_fonts_get(gui,D0_MFONTBOLD,sz,
                    D1_MFONTBOLD,D2_MFONTBOLD))==0 ) {
        vgui_errerr_printf(gui,"can not found a default bold font");
        gui->g_mFontBold              = gui->g_mFont;
    } else {
        gui->g_fonts[gui->g_fontNb++] = (Tegs_font) font;
        gui->g_mFontBold              = (Tegs_font) font;
    }
#endif
}

extern void   gui_fonts_free(Tvgui*gui)
{
#if 0 // FIXME
    int i;
    for ( i=0 ; i<gui->g_fontNb ; i+=1 ) {
        Font font = (Font) gui->g_fonts[i];
        XUnloadFont(E_Display, font );
    }
#endif
}

/*======================================================================*/
/*= show, the main loop, breaking the main loop                        =*/

extern void gui_show(Tvgui*gui)
{
    // FIXME
    Tgui_main* main = gui->g_main;
}

extern void gui_loop(Tvgui*gui)
{
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

extern void gui_loop_break(Tvgui*gui)
{
    // FIXME
}

/*======================================================================*/

extern void gui_gc_create(Tvgui*gui,  Tod_GCdef gcd, Cstyle*sty, int xor)
{
#if 0 // FIXME
    GC             ngc;
    XGCValues      gcv;
    unsigned long  gcmask=0;

    Csty_text* stt = &sty->sty_text;
    Csty_line* stl = &sty->sty_line;
    Csty_fill* stf = &sty->sty_fill;

    gcmask |= GCFont       ; gcv.font       = (Font)gui->g_mFont;
    gcmask |= GCLineWidth  ; gcv.line_width = stl->stl_thickness<=0
                                            ? 1
                                            : stl->stl_thickness;
    gcmask |= GCLineStyle  ; gcv.line_style = stl->stl_style==DA_LS_None 
                                            ? LineSolid
                                            : conv_lineStyle_Od2X11(stl->stl_style);
    gcmask |= GCJoinStyle  ; gcv.join_style = stl->stl_join==DA_JS_None
                                            ? JoinMiter
                                            : conv_joinStyle_Od2X11(stl->stl_join);
    gcmask |= GCCapStyle   ; gcv.cap_style  = stl->stl_cap==DA_CS_None
                                            ? CapRound
                                            : conv_capStyle_Od2X11(stl->stl_cap);
    gcmask |= GCForeground ; gcv.foreground = EGS_COLOR(stl->stl_color);
    gcmask |= GCFillStyle  ; gcv.fill_style = FillSolid ; // FIXME always set to DA_FS_Solid
    if ( stf->stf_color!=0 ) {
        gcmask |= GCBackground ; gcv.background = EGS_COLOR(stf->stf_color);
    } else {
        gcmask |= GCBackground ; gcv.background = EGS_COLOR(gui->vg_black);
    }
    gcmask |= GCFunction   ; gcv.function   = xor==0 ? GXcopy : GXxor;
    gcmask |= GCFillRule   ; gcv.fill_rule  = EvenOddRule;
    ngc = XCreateGC(E_Display, RootWindow(E_Display,E_ScreenNum), gcmask, &gcv);

    E_GCcache(gcd) = ngc;

    gui_gc_create_font(gui,gcd,stt->stt_font,stt->stt_pts,stt->stt_color,NULL);
#endif
}

extern void gui_gc_free(Tvgui*gui,  Tod_GCdef gcd)
{
    // FIXME
}

/*======================================================================*/
