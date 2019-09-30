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
 *      $Id: guigtk-init.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-init.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This files groups the functions that initializes and frees         =*/
/*= the XT Tvgui.                                                      =*/
/*=                                                                    =*/
/*= See the "vgui/vgui.h" header for types and function documentation. =*/
/*======================================================================*/

#include "guigtk.h"

#include "outdev/style.h" 

/*======================================================================*/
/*= This function is called indirectly by the WM when it destroys     =*/
/*= a windows.                                                         =*/


static void gui_quit(Tvgui* gui, GdkAtom a)
{
    gtk_main_quit();
}

/*======================================================================*/
/*= The gui initialization.                                            =*/

extern void gui_open(Tvgui*gui, Tvgui_openParam*op)
{
    gui->g_main = tool_malloc_0(sizeof(*gui->g_main));
    if (!gtk_init_check(&gui->g_argc,&gui->g_argv)) {
        tool_ftlerr_printf(NULL,"%s","Starting GTK failed");
    }

    gui->g_externalSystem  = GUI_ExtSys_GTK;
    GdkDisplay *display = gdk_display_get_default();
    E_Display = display;
    gui->g_unsprtXor = 1;
    //gui->g_unsprtEmbeddedFb = 1;
    GdkMonitor *monitor;
    GdkRectangle rect;
    int cnt = gdk_display_get_n_monitors(display);
    tool_inferr_printf(NULL,"count  Screen: %d",
            cnt);
    int width = 0;
    int height = 0;
    int widthm = 0;
    int heightm = 0;
    int nbpixel=0;
    int tmp = 0;
    int screenNumSelected = 0;
    for ( int i=0 ; i<cnt ; i+=1) {
        monitor = gdk_display_get_monitor(display,i);
        gdk_monitor_get_geometry(monitor,&rect);
        width = rect.width;
        height = rect.height;
        widthm = gdk_monitor_get_width_mm(monitor);
        heightm = gdk_monitor_get_height_mm(monitor);
        tool_inferr_printf(NULL,"screenGeometry %2d: %dx%d", i,width,height);
        tool_inferr_printf(NULL,"ppi   %2d: %.0fx%.0f", i,((double)width/widthm)*25.4,
                ((double)height/heightm)*25.4);
        // we select the screen that has more pixels
        tmp=width * height;
        if ( tmp>nbpixel ) {
            nbpixel = tmp;
            screenNumSelected = i;
        }
    }
    monitor = gdk_display_get_monitor(display,screenNumSelected);
    gdk_monitor_get_geometry(monitor, &rect);
    width = rect.width;
    height = rect.height;
    widthm = gdk_monitor_get_width_mm(monitor);
    heightm = gdk_monitor_get_height_mm(monitor);
    gui->g_pixScreenWidth  = width; 
    gui->g_pixScreenHeight = height;
    gui->g_ppiScreenWidth = ((double)width/widthm)*25.4;
    gui->g_ppiScreenHeight = ((double)height/heightm)*25.4;
    tool_wrnerr_printf(NULL,"screen selected: numero     = %d",screenNumSelected);
    tool_wrnerr_printf(NULL,"screen selected: geometry   = %dx%d  pix",
             gui->g_pixScreenWidth, gui->g_pixScreenHeight);
    tool_wrnerr_printf(NULL,"screen selected: resolution = %.0fx%.0f  ppi",
             gui->g_ppiScreenWidth, gui->g_ppiScreenHeight);

    // init. of vtable: VGUI --> GUI
    extern struct _Twidm_guiVt guigtk_VtMenu; gui->vg_guiVtMenu = &guigtk_VtMenu;
}

extern void gui_open_finish(Tvgui* gui)
{
    PERM_error_NYI();
    //gui_pixmaps_init(gui);
}

/*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/

extern void gui_close_start(Tvgui* gui)
{
    //gui_pixmaps_free(gui);
}

extern void gui_close(Tvgui* gui)
{
    free(gui->g_main);
}

/*======================================================================*/
/*= VGUI font initialization                                           =*/

static PangoFontDescription* gui_fonts_get(Tvgui*gui, Cstr fn0, int sz, Cstr fn1, Cstr fn2)
{
    PangoFontDescription* desc;
    char fontname[500];

    sprintf(fontname, fn0, sz);
    if ( desc = pango_font_description_from_string(fontname) ) {
        DBG0_Font("Found %s",fontname);
        return desc;
    }

    if ( fn1 == NULL ) return 0;
    sprintf(fontname,fn1,sz);
    if ( desc = pango_font_description_from_string(fontname) ) {
        DBG0_Font("Found %s",fontname);
        return desc;
    }
    DBG1_Font("NOT Found %s",fontname);
    DBG1_Font("NOT Found %s",fontname);

    if ( fn2==0 ) return 0;
    sprintf(fontname,fn2,sz);
    if ( desc = pango_font_description_from_string(fontname) ) {
        DBG0_Font("Found %s",fontname);
        return desc;
    }
    DBG1_Font("NOT Found %s",fontname);
    return NULL;
}

extern void gui_fonts_init(Tvgui*gui)
{
    int sz;
    PangoFontDescription* desc;
    gui->g_mPts = 12; 

    sz = gui->g_mPts;
    /*Initialisation of the font style normal*/
    desc = pango_font_description_from_string("times");
    pango_font_description_set_weight(desc, PANGO_WEIGHT_NORMAL);
    pango_font_description_set_size(desc, sz);
    gui->g_mFont                  = (Tegs_font) desc;
}

extern void   gui_fonts_free(Tvgui*gui)
{
    pango_font_description_free( (PangoFontDescription*)gui->g_mFont );
}

/*======================================================================*/
/*= show, the main loop, breaking the main loop                        =*/

extern void gui_show(Tvgui*gui)
{
    Tgui_main* main = gui->g_main;
    gtk_widget_show_all(main->m_topForm);
}

extern void gui_loop(Tvgui* gui)
{
    gtk_main();
}

extern void gui_loop_break(Tvgui*gui)
{ 
    gtk_main_quit();
}

/*======================================================================*/

extern void gui_gc_create(Tvgui* gui, Tod_GCdef gcd, Cstyle* sty, int xor)
{
}

extern void gui_gc_free(Tvgui*gui,  Tod_GCdef gcd)
{
    //On libère l'ancien pixbuf
    GdkPixbuf* pxb = PXMegs2gtk(E_PXM(gcd));
    if ( pxb ) g_object_unref((pxb));
}

/*======================================================================*/
