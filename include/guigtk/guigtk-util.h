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
 *      $Id: guigtk-util.h 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guixt/guixt-util.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIGTK_UTIL_H
#define FILE_GUIGTK_UTIL_H
/*======================================================================*/
/*======================================================================*/
/*= miscellaneous converters                                           =*/

#include "vgui/vgui-accelkey.h"

#if 0
// return 0:unknown button; 1/2/3/4/5:left/middle/right/wheel button
extern int       conv_BtnNum_Xt2Vgui(int  xnb);
extern TaccelDef conv_KeyMod_Xt2Vgui(int  xkmod);
#endif
extern cairo_line_cap_t conv_capStyle_OdGtk(TcapStyle cs);
extern cairo_line_join_t conv_joinStyle_Od2Gtk(TjoinStyle js);
extern void conv_accelerator2Gtk(TaccelDef accelerator,
                                 int* accel_key,
                                 GdkModifierType* accel_mods);

//extern TaccelDef conv_KeyMod_Gtk2Vgui(GdkEventKey *event);
extern TaccelDef conv_KeyMod_Gtk2Vgui(int state);//FIXME REMOVE ?
extern int       conv_BtnNum_Gtk2Vgui(GdkEventButton *event);
extern int       conv_wheel_Gtk2Vgui(GdkEvent *event, int*times, int horizontal);

/*======================================================================*/
/*= miscellaneous utilities                                            =*/
 
extern void widget_getColors(Tvgui* gui, CstrBrief wn, TcolorEGS*bg, TcolorEGS*fg);
extern void widget_setColors(Tvgui* gui, GtkWidget* self, CstrBrief wn, TcolorEGS bg, TcolorEGS fg);
// Set the thickness and the color of the widget border to th and clr.
// If th <= 0, no border, and if clr is NULL, the gtk default color is preserved. 
extern void widget_setBorderThClr(GtkWidget *gtkwid, int th, GdkRGBA *clr);
extern void widget_setPaddingTh(GtkWidget* wid, int padth, int bordth);

// Store and retrieve the vgui widget data into (from) a gtk widget. 
Inline void widget_setVguiData(GtkWidget *gtkwid, void *vguiwid) { g_object_set_data(G_OBJECT(gtkwid),"vguidt",vguiwid);}
Inline void*widget_getVguiData(GtkWidget *gtkwid) { return g_object_get_data(G_OBJECT(gtkwid),"vguidt"); }

/*======================================================================*/
/*= Setting widget background to a pixmap                              =*/
/*=                                                                    =*/
/*= You must connect the "draw" signal to widget_fctatt_bgPXM.         =*/

Inline void       widget_setatt_bgPXM(GtkWidget*w, GdkPixbuf *pxm) { g_object_set_data(G_OBJECT(w),"bgPXM",(void*)pxm); }
Inline GdkPixbuf* widget_getatt_bgPXM(GtkWidget*w) { return g_object_get_data(G_OBJECT(w),"bgPXM"); }
extern gboolean   widget_fctatt_bgPXM(GtkWidget *widget, cairo_t *cr, gpointer data);

/*======================================================================*/
/*= place widget in container                                          =*/

void widget_place   (GtkWidget *g, GtkWidget *w,int li, int ci, int ls, int cs,gboolean hxpand,gboolean vxpand);

void widget_place_FF(GtkWidget *g, GtkWidget*w, int li, int ci, int ls, int cs);

void widget_place_EF(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs);

void widget_place_EE(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs);

void widget_place_FE(GtkWidget*g, GtkWidget*w, int li, int ci, int ls, int cs);

/*======================================================================*/
#endif // FILE_GUIGTK_UTIL_H
/*======================================================================*/
