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
 *$Software: YFIG-GTK (Interface to GTK)
 *      $Id: guigtk-top.h 347 2018-11-09 23:05:47Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guigtk/guigtk-top.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups the EGS resources and basic utilities that are =*/
/*= required everywhere.                                               =*/
/*=                                                                    =*/
/*= The EGS resources are grouped in the Tgui_main structure.          =*/
/*=                                                                    =*/
/*= The basic utilities are mainly:                                    =*/
/*=    - macros thay cast VGUI egs type to the true EGS types          =*/
/*=      (eg: Tcolor to QtColor).                                      =*/
/*=    - some initialization and free functions.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIGTK_TOP_H
#define FILE_GUIGTK_TOP_H
/*======================================================================*/

#include "guigtk-config.h"
#include "vgui/vgui.h"
#include "tools/font.h"
#include "tools/color.h"
#include "vgui/vgui-names.h"

Inline GdkPixbuf* PXMegs2gtk(TpixmapEGS egs) {
    if ( PXMEGS_IsNone(egs) ) return 0;
    else                      return (GdkPixbuf*)egs.ptr; }
Inline TpixmapEGS PXMgtk2egs(GdkPixbuf*qp) {
    if ( qp==0 ) return PXMEGS_none;
    TpixmapEGS ep; ep.ptr=qp; return ep; }

Inline cairo_surface_t* SURFegs2gtk(TimageEGS egs) {
    if ( IMGEGS_IsNone(egs) ) return 0;
    else                      return (cairo_surface_t*)egs.egs.ptr; }
Inline Tany   SURFgtk2egs(cairo_surface_t*qp) {
    if ( qp==0 ) return IMGEGS_none.egs;
    Tany   ep; ep.ptr=qp; return ep; }
Inline PangoFontDescription* FONTegs2gtk(TfontEGS egs) {
    if ( FONTEGS_IsNone(egs) ) return 0;
    else                       return (PangoFontDescription*)egs.ptr; }
Inline TfontEGS FONTgtk2egs(PangoFontDescription*qf) {
    if ( qf==0 ) return FONTEGS_none;
    TfontEGS ef; ef.ptr=qf; return ef; }

//#include "guigtk-gc.h"

/*======================================================================*/
/*= EGS resources and shortcut
                                         =*/
typedef struct _Tegs_context Tegs_context;
struct _Tegs_context {
    cairo_t *gc;
};

typedef struct _Tgui_pango Tgui_pango;
struct _Tgui_pango {
    PangoContext * context;
    double         contextangle; // the angle currently set in context matrix
    PangoLayout  * layout;
    PangoFontDescription
                 * font;         // the font presently set within the layout
};
    
typedef struct _Tegs_pxe Tegs_pxe;
struct _Tegs_pxe {
    TpixmapEGS            m_pxm;
    PangoFontDescription* m_fegs;
};

struct _Tgui_main {
    // initialized in gui_open
    GMainLoop*      m_mainLoop;
    GtkWidget*      m_topForm;         // the top container
    GdkDisplay*     m_display;

    //Pango fontconfig
    Tgui_pango*     m_pango;
    // Graphic Context
    Tegs_pxe        m_gcpxe[OD_GC_NB];
    //Drawing area
    GtkWidget*      m_drawing_area;
    //TGUI_gc         m_gcs[OD_GC_NB];
};

#define E_Application        (gui->g_main->m_application)
#define E_mainLoop           (gui->g_main->m_mainLoop)
#define E_TopForm            (gui->g_main->m_topForm)
#define E_PXM(gcd)           (gui->g_main->m_gcpxe[gcd].m_pxm)
#define E_Display            (gui->g_main->m_display)
#define E_Pango              (gui->g_main->m_pango)
#define E_PangoFontDesc(gcd) (gui->g_main->m_gcpxe[gcd].m_fegs)
#define E_DrawingArea        (gui->g_main->m_drawing_area)
//#define E_GC(gcd)       (gui->g_main->m_gcs[gcd])

/*======================================================================*/
/*= some macros for colors                                             =*/

#undef  COLOR
#undef  GtkCOLOR

#define GUI_RGB(r,g,b) "#" #r #g #b

#define GtkCOLOR(x) ((GdkRGBA*)((x).ptr))
#define COLOR(x) GtkCOLOR((x)->egs)

/*======================================================================*/
/*= some converters from VGUI enum to GTK enum                          =*/

extern void conv_accelDef2str(char* buf, TaccelDef accelDef);

/*======================================================================*/
/*= Some utilities internal to GTK                                      =*/

/*======================================================================*/
#endif // FILE_GUIGTK_TOP_H
/*======================================================================*/
