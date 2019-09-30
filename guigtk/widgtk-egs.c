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
 *      $Id: widxt-egs.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-egs.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Operations on EGS widget (EGS driver).                             =*/
/*=                                                                    =*/
/*= See the vgui/wid-egs.h header for the function description and     =*/
/*= for documentation.                                                 =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/wid-egs.h"

/*======================================================================*/


void gui_widegs_addTip(Tegs_widget w, CstrBrief hs)
{
    if ( hs==0 ) return;

    GtkWidget* widget = (GtkWidget*)w;
    gtk_widget_set_tooltip_text(widget,hs);
}
void gui_widegs_addTipAA(Tegs_widget w, CstrBrief hs,
                         TaccelDef a1, TaccelDef a2)
{
    if ( hs==0 ) return;

    char* helpText = accel_appendToString(hs,a1,a2);
    gui_widegs_addTip(w,helpText);
    tool_free(helpText);
}

void  gui_widegs_putW1inNEofW2(Tvgui*gui,Tegs_widget w1,Tegs_widget w2)
{
    PERM_error_NYI();
}

extern void gui_widegs_disable(Tegs_widget w)
{
    GtkWidget* self = (GtkWidget*)w;
    gtk_widget_set_sensitive(self,FALSE);
}
extern void gui_widegs_enable (Tegs_widget w)
{
    GtkWidget* self = (GtkWidget*)w;
    gtk_widget_set_sensitive(self,TRUE);
}

extern void gui_widegs_unshow (Tegs_widget w)
{
    GtkWidget* self = (GtkWidget*)w;
    gtk_widget_hide(self);
}
extern void gui_widegs_show   (Tegs_widget w)
{
    GtkWidget* self = (GtkWidget*)w;
    gtk_widget_show(self);
}
extern int  gui_widegs_isShown(Tegs_widget w)
{
    GtkWidget* self = (GtkWidget*)w;
    return gtk_widget_is_visible(self);
}

extern void gui_widegs_children(Tegs_widget w, Tegs_widSet* set)
{
    GtkWidget* self = (GtkWidget*)w;
    GList *children = gtk_container_get_children(GTK_CONTAINER(self));
    GList *l = children;
    GtkWidget *child;
    while (l != NULL)
    {
        child = l->data;
        if(GTK_IS_GRID(child))
            gui_widegs_children(child,set);
        else
            egswidset_add(set,child);
        l = l->next;
    }
}

/*======================================================================*/

extern void gui_widegs_setFgColor (Tegs_widget w, TcolorEGS c)
{
    PERM_error_NYI();
#if 0
    GtkWidget* self = (GtkWidget*)w;
    QPalette pal = self->palette();
    pal.setColor( QPalette::WindowText,QtCOLOR(c) );
    pal.setColor( QPalette::Text,      QtCOLOR(c) );
    self->setPalette(pal);
    self->setAutoFillBackground(true);
#endif
}

extern void gui_widegs_setBgColor (Tegs_widget w, TcolorEGS c)
{
    GtkWidget *self = (GtkWidget*) w;
    GtkCssProvider* cssprov = gtk_css_provider_new();
    GError* err = NULL;
    char css[100];
    int r,g,b;
    gui_color_getRGB08(NULL,c,&r,&g,&b);
    sprintf(css, "* {background-image: none; background-color : #%02x%02x%02x; }",r,g,b);
    gtk_css_provider_load_from_data(cssprov, css, 100, &err); 
    gtk_style_context_add_provider(gtk_widget_get_style_context(self),
                                          (GtkStyleProvider*)cssprov,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
}

extern void gui_widegs_setBgPxm   (Tegs_widget w, TpixmapEGS pxm)
{
    GtkWidget *self = w;
    widget_setatt_bgPXM(self,PXMegs2gtk(pxm));
    int dx  = gtk_widget_get_allocated_width (self);
    int dy  = gtk_widget_get_allocated_height(self);
    gtk_widget_queue_draw_area(self,0,0,dx,dy);
}
extern gboolean widget_fctatt_bgPXM(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GdkPixbuf* pxm = widget_getatt_bgPXM(widget);
    gdk_cairo_set_source_pixbuf(cr,pxm,0,0);
    cairo_paint(cr);
    return TRUE;
}


extern int   gui_widegs_getGeometry(Tegs_widget w,
                int*x, int*y, int*dx, int*dy, int*bdt)
{
    GtkWidget* self = (GtkWidget *)w;
    if (x && y) gtk_widget_translate_coordinates(self, gtk_widget_get_toplevel(self),0,0,x,y);
    if (dx) *dx = gtk_widget_get_allocated_width(self);
    if (dy) *dy = gtk_widget_get_allocated_height(self);
    if (bdt) *bdt = 0;
}

extern void gui_widegs_setDY(Tegs_widget w, int dy)
{
    GtkWidget* self = (GtkWidget *)w;
    gtk_widget_set_size_request(self,-1,dy);
}
extern void gui_widegs_setDX(Tegs_widget w, int dx)
{
    GtkWidget* self = (GtkWidget *)w;
    gtk_widget_set_size_request(self,dx,-1);
}
extern void gui_widegs_setSize(Tegs_widget w, int dx, int dy)
{
    GtkWidget* self = (GtkWidget *)w;
    gtk_widget_set_size_request(self,dx,dy);
}

/*======================================================================*/

extern int gui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text)
{ 
    if ( text==0 ) return 0;
    int len = strlen(text);
    if ( len==0 ) return 0;
    return round( len*gui->g_mPts*0.75 ); // FIXME: we must use a font here
}

/*======================================================================*/
