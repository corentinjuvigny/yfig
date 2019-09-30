/*======================================================================
 *  Copyright 2017 Corentin Juvigny
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
 *      $Id: guixt-util.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-util.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/vgui-names.h"

#include "outdev/style.h"

/*======================================================================*/
/*= miscellaneous converters                                           =*/

extern cairo_line_cap_t conv_capStyle_OdGtk(TcapStyle cs) {
    switch(cs) {
        default:
            PERM_spe_printf(
                    "%d is an invalid cap style (set to line-solid).", cs);
        case DA_CS_Butt  : return CAIRO_LINE_CAP_BUTT;
        case DA_CS_Round : return CAIRO_LINE_CAP_ROUND;
        case DA_CS_Proj  : return CAIRO_LINE_CAP_SQUARE;
        case DA_CS_None  :
                           PERM_spe_printf(
                                   "None (%d) cap style is unexpected (set to cat-butt).", cs);
                       return CAIRO_LINE_CAP_BUTT;
    }
}

extern cairo_line_join_t conv_joinStyle_Od2Gtk(TjoinStyle js) {
    switch (js) {
        default :
           PERM_spe_printf(
               "%d is an invalid join style (set to join-miter).", js);
        case DA_JS_Miter : return CAIRO_LINE_JOIN_MITER;
        case DA_JS_Round : return CAIRO_LINE_JOIN_ROUND;
        case DA_JS_Bevel : return CAIRO_LINE_JOIN_BEVEL;
        case DA_JS_None  :
            PERM_spe_printf("None (%d) join style is unexpected (set to join-miter).", js);
                 return CAIRO_LINE_JOIN_MITER;
    }
}

extern void conv_accelerator2Gtk(TaccelDef accelerator,
                                 int* accel_key,
                                 GdkModifierType* accel_mods)
{
    *accel_key = GL_ACCEL_GetKey(accelerator);
    if (accelerator&GL_ACCEL_Shift) {
        *accel_mods = GDK_SHIFT_MASK;
    } else if (accelerator&GL_ACCEL_Control) {
        *accel_mods = GDK_CONTROL_MASK;
    } else if (accelerator&GL_ACCEL_Meta) {
        *accel_mods = GDK_META_MASK;
    } else {
        *accel_mods = 2;
    }
}

extern TaccelDef conv_KeyMod_Gtk2Vgui(int state)
{
    TaccelDef kmod=0;
    if ( state & GDK_SHIFT_MASK )    kmod |= GL_ACCEL_Shift;
    if ( state & GDK_CONTROL_MASK )  kmod |= GL_ACCEL_Control;
    if ( state & GDK_META_MASK )     kmod |= GL_ACCEL_Meta;
    if ( state & GDK_MOD1_MASK )     kmod |= GL_ACCEL_Meta;
    return kmod;
}
extern int       conv_BtnNum_Gtk2Vgui(GdkEventButton *event)
{
    switch ( event->button ) {
        case 1:   return 1;
        case 2:   return 2;
        case 3:   return 3;
        default:
            TEMP_spw_printf("unexpected GTK button identifier %04x",
                event->button);
            return 0;
    }
}

extern int       conv_wheel_Gtk2Vgui(GdkEvent *event, int*times, int horizontal)
{
    GdkEventScroll *e = (GdkEventScroll*) event;
    //FIXME : calcul de times ?
    GdkScrollDirection direction;
    gdk_event_get_scroll_direction(event,&direction);
    double delta;
    if (horizontal) delta = e->delta_x;
    else delta = e->delta_y;
    int numDegrees = delta / 8;
    int numSteps = numDegrees / 15;

    if(direction == GDK_SCROLL_UP) {
        if (times)
            *times =  numSteps<1 ? 1 : numSteps;    
        return 4;
    } else {
        if (times)
            *times = numSteps>-1 ? 1 : -numSteps;
        return 5;
    }
}

/*======================================================================*/
/*= miscellaneous utilities                                            =*/

extern void widget_getColors(Tvgui* gui, CstrBrief wn,
                      TcolorEGS*bg, TcolorEGS*fg)
{
     vgui_wn_getColorEGS(gui,wn, fg, bg);
}

extern void widget_setColors(Tvgui* gui, GtkWidget* self, CstrBrief wn, 
                      TcolorEGS bg, TcolorEGS fg)
{
    GtkCssProvider* cssprov = gtk_css_provider_new();
    GError* err = NULL;
    char css[100];
    int rf, gf, bf, rg, gg, bgg;

    if ( COLOREGS_IsDefault(bg) || COLOREGS_IsDefault(fg) ) {
        TcolorEGS bg2,fg2;
        widget_getColors(gui, wn, &bg2, &fg2);

        if ( COLOREGS_IsDefault(bg) ) bg = bg2;
        if ( COLOREGS_IsDefault(fg) ) fg = fg2;
    }
    gui_color_getRGB08(gui, fg, &rf, &gf, &bf);
    gui_color_getRGB08(gui, bg, &rg, &gg, &bgg); 

    if (          COLOREGS_IsNone(bg) && ! COLOREGS_IsNone(fg) ) {
        gui_color_getRGB08(gui, fg, &rf, &gf, &bf); 
        sprintf(css, "* {color : #%02x%02x%02x; }", rf, gf, bf);
    } else if ( ! COLOREGS_IsNone(bg) &&   COLOREGS_IsNone(fg) ) {
        gui_color_getRGB08(gui, bg, &rg, &gg, &bgg); 
        sprintf(css, "* {background-color : #%02x%02x%02x; }", rg, gg, bgg);
    } else if ( ! COLOREGS_IsNone(bg) && ! COLOREGS_IsNone(fg) ) {
        gui_color_getRGB08(gui, fg, &rf, &gf, &bf); 
        gui_color_getRGB08(gui, bg, &rg, &gg, &bgg); 
        sprintf(css, "* {color : #%02x%02x%02x; background-color : #%02x%02x%02x; }",
                       rf, gf, bf, rg, gg, bgg);
    }
    if ( ! COLOREGS_IsNone(bg) || ! COLOREGS_IsNone(fg) ) {
        gtk_css_provider_load_from_data(cssprov, css, 100, &err); 
        gtk_style_context_add_provider(gtk_widget_get_style_context(self),
                                          (GtkStyleProvider*)cssprov,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
   }
}

extern void widget_setBorderThClr(GtkWidget *gtkwid, int th, GdkRGBA *clr)
{
    if (clr != NULL) {
        GtkCssProvider* cssprov = gtk_css_provider_new();
        GError* err = NULL;
        char css[100];
        int r = round(clr->red * 255);
        int g = round(clr->green * 255);
        int b = round(clr->blue * 255);
        sprintf(css ,"* { border: %dpx solid; border-color: #%02x%02x%02x; }",th,r,g,b); 
        gtk_css_provider_load_from_data(cssprov,css,100,&err);
        gtk_style_context_add_provider(gtk_widget_get_style_context(gtkwid),
                                          (GtkStyleProvider*)cssprov,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

extern void widget_setPaddingTh(GtkWidget* wid, int padth, int bordth)
{
    GtkCssProvider* cssprov = gtk_css_provider_new();
    GError* err = NULL;
    char css[100];
    sprintf(css, "* { padding : %dpx; border-width : %dpx; }", padth, bordth);
    gtk_css_provider_load_from_data(cssprov, css, 100, &err);
    gtk_style_context_add_provider(gtk_widget_get_style_context(wid),
                                      (GtkStyleProvider*)cssprov,
                                      GTK_STYLE_PROVIDER_PRIORITY_USER);
}

/*======================================================================*/
/*= place widget in container                                          =*/

void widget_place   (GtkWidget *g, GtkWidget *w,int li, int ci, int ls, int cs,gboolean hxpand,gboolean vxpand)
{
    gtk_grid_attach(GTK_GRID(g),w,li,ci,ls,cs);
    gtk_widget_set_hexpand(w,hxpand);
    gtk_widget_set_vexpand(w,vxpand);
}
void widget_place_FF(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs) { widget_place(g,w,ci,li,cs,ls,FALSE,FALSE); }
void widget_place_EE(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs) { widget_place(g,w,ci,li,cs,ls,TRUE ,TRUE ); }
void widget_place_EF(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs) { widget_place(g,w,ci,li,cs,ls,TRUE ,FALSE); }
void widget_place_FE(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs) { widget_place(g,w,ci,li,cs,ls,FALSE,TRUE ); }

/*======================================================================*/
