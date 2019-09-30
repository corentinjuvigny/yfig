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
 *      $Id: vgui.h 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top utilities of the Virtual GUI                                   =*/
/*= It provides:                                                       =*/
/*=  - The Tvgui data structure that contains all the parameters of    =*/
/*=           the Virtual GUI.                                         =*/
/*=  - The functions for creating and freeing a Virtual GUI.           =*/
/*=  - The definition of EGS functions used for creating and freeing   =*/
/*=    a Virtual GUI.                                                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_H
#define FILE_VGUI_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "outdev/types.h"

/*======================================================================*/

typedef struct _Tgui_main  Tgui_main;

/*======================================================================*/
/*= parameters for gui initialization                                  =*/
/*=   * only use in gui_open                                           =*/
/*=   * initializes in main.c only                                     =*/
/*=   * may be different for other systems                             =*/

#if TOS_MINGW == 1
typedef struct _Tvgui_openParam {
    void* op_hInstance;     // first arg of WinMain
    int   op_nCmdShow;      // 4th arg of WinMain
} Tvgui_openParam;
#elif defined(USE_X11) && USE_X11==1
typedef struct _Tvgui_openParam {
    const char* op_display;       // X11 display (eg: 256.12.67:1.0)
    const char* op_geometry;      // main window geometry (eg: 1000x700+20+20)
} Tvgui_openParam;
#else
typedef void Tvgui_openParam;
#endif

/*======================================================================*/

struct _Tvgui {
    TastrSet*     g_argv_org; // A copy of the command line arguments.
    int           g_argc;     // EGS must not use it but g_argc and g_argv
    TstrBrief*    g_argv;     // that it can altered without freing the str.

    void*         g_ud;     // Application user data 
    void*         g_vmw;    // a dummy Tvgui_mw, it can be used for
                            // instance to create dialog not attached
                            // to a main widget.
    Tvdw_console* g_console;     // a Twid_console object or null
    Tmsgdrv*      g_msgdrv;      // message driver

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= colors:                                                          =*/
    TcolorDriver*colorDriver; // data structure used by the color package
                              // that provides the interface to EGS.
#   include "vgui-colors.h"   // all the colors

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
    // dialog utilities
    TpixmapEGS  g_pmMenuArrow;   /* arrow for pull-down menus */
    int         g_pmMenuArrowDx;
    int         g_pmMenuArrowDy;
    TpixmapEGS  g_pmFold;        /* pixmap for folding */
    int         g_pmFoldDx;
    int         g_pmFoldDy;
    TpixmapEGS  g_pmUnfold;      /* pixmap for unfolding */
    int         g_pmUnfoldDx;
    int         g_pmUnfoldDy;

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= Parameters for output device (Toutdev base class)                =*/
  /*=                                                                  =*/
  /*= vg_odPxm is a small Tvgui_odPixmap. Its main rules are:          =*/
  /*=  - to hold the outdev resources (vg_odResources) that is shared  =*/
  /*=    by all the output devices.                                    =*/
  /*=  - to change the GC parameters especially at start time.         =*/
  /*=  - to get font parameters.                                       =*/
  /*=  - to get string extents.                                        =*/
  /*= It can also be used to generate small pixmap.                    =*/
  /*= It is created and freed in main_gc_init/free that call           =*/
  /*= gui_od_init and gui_od_free.                                     =*/

    void*           vg_odVtableCvs;   // Toudev_vt for canvas
    void*           vg_odVtablePxm;   // Toudev_vt for pixmap
    Tvgui_odpixmap* vg_odPxm;         // see comment above
    Toutdev*        vg_od;            // same as vg_odPxm but casted
#define             vg_odResources     vg_od->resources   // TodResources
#define             VG_fontdef(gui,fi) gui->vg_od->resources->fontUsed->eles[fi]
#define             VG_fontdefIndex(gui,fd) (fd - VG_fontdef(gui,0))
#define             VG_fontdefNb(gui)  gui->vg_od->resources->fontUsed->eleNb
#define             VG_odPPI(gui)      VG_ppiScreen(gui)
    // font used by VGUI in output device  pixmap or canvas such as
    // wid-mousefun).
    // Application does not use these fields. 
    double      g_devPtsStd;    // font size (1/72 inch) standard
    double      g_devPtsSmall;  // font size (1/72 inch) ~15% smaller
                                // than standard
    double      g_devPtsBig;    // font size (1/72 inch) ~15% bigger
                                // than standard
    int         g_devFid;       // font id (default is 0: times)

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= Parameter that are set by gui_open and freed by gui_close        =*/
  /*=                                                                  =*/

    Tgui_main*   g_main;      // private init of external system.

    int    g_externalSystem;  // see GUI_ExtSys_XXX
    int    g_pixScreenWidth;  // width of screen in pixel
    int    g_pixScreenHeight; // height of screen in pixel
    double g_ppiScreenWidth;  // pixel number by inch horizontally on the screen
    double g_ppiScreenHeight; // pixel number by inch vertically   on the screen
#define    VG_ppiScreen(gui) (((gui)->g_ppiScreenWidth+(gui)->g_ppiScreenHeight)/2.)

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= VGUI fonts:                                                      =*/
  /*= Must be set by gui_fonts_init and free by gui_font_free          =*/
  /*= font for the main windows and dialogs                            =*/
    double      g_mPts;         // font size (1/72 inch)
    Tegs_font   g_mFont;

#if 0 // FIXME:PRINTER
  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= VGUI printer:                                                    =*/
  /*= Must be set by gui_open                                          =*/
    void (*vg_printer)(Tvgui*gui);
#endif

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= VGUI Xor:                                                        =*/
  /*= g_unsprtXor : 1 if Xor unsupported, 0 otherwise                  =*/ //FIXME
  /*= g_unsprtEmbeddedFb : 1 if you must detach filebrowser from widget, else 0=*/
    int g_unsprtXor;
    int g_unsprtEmbeddedFb;

  /*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/
  /*= vtable of GUI object                                             =*/
  /*=  - They must be initialized in gui_open.                         =*/
  /*=  - Usually they are static table, in case of dynamic ones, they  =*/
  /*=    must be freed in gui_close.                                   =*/
    struct _Twidm_guiVt* vg_guiVtMenu;
};

extern Tvgui* vgui_newPass1(TastrSet*argv,Tvgui_openParam*op);
extern void   vgui_newPass2(Tvgui*gui);
extern void   vgui_free    (Tvgui*gui);

/*======================================================================*/
/*= open, run and close the EGS                                        =*/
/*=                                                                    =*/
/*=  gui_open                                                          =*/
/*=  gui_fonts_init                                                    =*/
/*=  (enable output device vg_odPxm)                                   =*/
/*=  gui_od_init                                                       =*/
/*=  gui_open_finish                                                   =*/
/*=  (build widget)                                                    =*/
/*=  gui_show                                                          =*/
/*=  gui_loop                                                          =*/
/*=  gui_close_start                                                   =*/
/*=  gui_od_free                                                       =*/
/*=  gui_fonts_free                                                    =*/
/*=  gui_close                                                         =*/

// The function opens the gui using the arguments given by gui->g_argc/argv
// It must create and initialize gui->g_main
// It must return argc/argv without the argumants that it used.
// It must not return if open the gui fails.
// guidata are extra parameters:
//   * in X11 case, it is the display or null.
extern void gui_open(Tvgui*gui, Tvgui_openParam*op);
extern void gui_close(Tvgui*gui);

// create the base pixmaps
extern void gui_open_finish(Tvgui*gui);
extern void gui_close_start(Tvgui*gui);

// create/free VGUI fonts
extern void gui_fonts_init(Tvgui*gui);
extern void gui_fonts_free(Tvgui*gui);

// get vtable for canvas and pixmap output device.
extern void* gui_od_getVtableCvs();
extern void* gui_od_getVtablePxm();

// enable/disable fonts for the vg_odPxm output device
extern void gui_od_init(Tvgui*gui);
extern void gui_od_free(Tvgui*gui);

// gui_show is called once when the GUI is built.
extern void gui_show(Tvgui*gui);

// gui_loop executes an infinite loop waiting for events.
// gui_loop_break breaks the infinite loop that gui_loop execute.
extern void gui_loop      (Tvgui*gui);
extern void gui_loop_break(Tvgui*gui);

extern void gui_gc_create(Tvgui* gui, Tod_GCdef gcd, Cstyle* sty, int _xor);
extern void gui_gc_free  (Tvgui* gui, Tod_GCdef gcd);

/*======================================================================*/
/*= Creation/Suppression of a console.                                 =*/

extern void vgui_msgdrv_addConsoleEL(Tvgui*gui, int errlvl);
extern void vgui_msgdrv_addConsole  (Tvgui*gui);
extern void vgui_msgdrv_delConsole  (Tvgui*gui);

/*======================================================================*/
/*= Direct access to output device fonts                               =*/
/*=                                                                    =*/

extern void gui_fonts_extents_DesUnit(Tvgui*gui, int fontid, double fontsize,
        CstrBrief str, int*asc, int*desc, int*dx, int*dy);

/**
 * The gui_fonts_lineinfo_DesUnit function returns for the fid font
 * at size pts of the gui EGS:
 *   - into *idy the max inked height of the characters (where pixels
 *     can be inked),
 *   - into *ldy the max logic height of the characters,
 *   - into *ils the inter-line space between lines.
 * The relation between these dimension is: ldy=idy+ils.
 *
 * The returned dimension are given in pixel.
 *
 * Parameters:
 *  - idy: where the inked height is returned or NULL.
 *  - ldy: where the logic height is returned or NULL.
 *  - ils: where the inter line space is returned or NULL.
**/
extern void gui_fonts_lineinfo_DesUnit(Tvgui*gui, int fontid, double fontsize,
        int*idy, int*ldy, int*ils); // old gui_font_pix_lineinfo

/*======================================================================*/
/*= Color interface to EGS.                                            =*/

extern void      gui_color_delete    (Tvgui*gui, TcolorEGS c);
extern TcolorEGS gui_color_dup       (Tvgui*gui, TcolorEGS c);
extern TcolorEGS gui_color_newByName (Tvgui*gui, CstrBrief name);
extern TcolorEGS gui_color_newByRGB08(Tvgui*gui, int r, int g, int b);
extern TcolorEGS gui_color_newByRGB16(Tvgui*gui, int r, int g, int b);
extern void      gui_color_getRGB08  (Tvgui*gui, TcolorEGS c,
                                      int*r, int*g, int*b);
extern void      gui_color_getRGB16(  Tvgui*gui, TcolorEGS c,
                                      int*r, int*g, int*b);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_H
/*======================================================================*/
