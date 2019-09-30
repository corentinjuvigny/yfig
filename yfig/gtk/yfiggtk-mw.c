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
 *$Software: YFIG (YFIG driver to GTK)
 *      $Id: yfiggtk-mw.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/gtk/yfiggtk-mw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* EGS driver for building and handling the top YFIG widget.           =*/
/*======================================================================*/

#include "../../config.h"
#include "guigtk/guigtk-top.h"

#include "vgui/wid-menu.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"

//TODO:REMOVE the include
#include "guigtk/guigtk-util.h"

#include "yfig-mw.h"

/*======================================================================*/

static gboolean gui_prevent_destroy(GtkWidget* widget, GdkEvent *event, gpointer ud)
{ gui_loop_break(ud); return TRUE; }

#if 0
static gboolean gui_window_change_state(GtkWidget* widget, GdkEvent* event, gpointer ud)
{
    Tvgui* gui = ud;
    GdkEventWindowState* ev = (GdkEventWindowState*)event;
    GdkEventConfigure event_configure;
    if ( ev->new_window_state & GDK_WINDOW_STATE_MAXIMIZED ) {
        int x, y, width, height;
        int ret;
        gtk_window_get_position(GTK_WINDOW(widget), &x, &y);
        gtk_window_get_size(GTK_WINDOW(widget), &width, &height);
        event_configure.type = GDK_CONFIGURE;
        event_configure.window = gtk_widget_get_window(widget);
        event_configure.send_event = FALSE;
        event_configure.x = x;
        event_configure.y = y;
        event_configure.width = width;
        event_configure.height = height;
        g_signal_emit_by_name(E_DrawingArea, "configure-event", E_DrawingArea, &event_configure, ret, &ret);
    } 

    return TRUE;
}
#endif
extern Tegs_widget gui_yfig_create(Tvgui* gui)
{
    Tgui_main*  main = gui->g_main;
    E_TopForm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_can_default(E_TopForm, TRUE);
    gtk_widget_grab_default(E_TopForm);
    g_signal_connect(E_TopForm, "delete_event", G_CALLBACK(gui_prevent_destroy), gui);
    //g_signal_connect(E_TopForm, "window-state-event", G_CALLBACK(gui_window_change_state), gui);
    
    return (Tegs_widget) E_TopForm;
}

extern void gui_yfig_createend(Tyfig_mw* ymw)
{
    Tvgui *gui = ymw->ymw_gui;

    GtkWidget *main_Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_container_add(GTK_CONTAINER(E_TopForm),main_Vbox);

    GtkWidget *top_Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    GtkWidget *mid_Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    GtkWidget *bot_Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);

    gtk_box_pack_start(GTK_BOX(main_Vbox),top_Hbox,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(main_Vbox),mid_Hbox,TRUE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(main_Vbox),bot_Hbox,FALSE,FALSE,0);
    
    GtkWidget *top_menu_Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget*  menuBarre  = (GtkWidget *)ymw->ymw_menuBar->mb_widEGS;
    GtkWidget*  message    = (GtkWidget *)ymw->ymw_message;
    GtkWidget*  mousefun   = (GtkWidget *)ymw->ymw_mouseFun->mf_widEGS;
    GtkWidget*  modePanel  = (GtkWidget *)ymw->ymw_modePanel->bp_widEGSext;
    GtkWidget*  canvas     = (GtkWidget *)ymw->ymw_canvas->vgcm_widEGS;
    GtkWidget*  update     = (GtkWidget *)vgui_dw_getWidEGS(ymw->ymw_update);
    GtkWidget*  dpPanel    = (GtkWidget *)ymw->ymw_dpPanel->bp_widEGSext;
    GtkWidget*  layers     = (GtkWidget *)vgui_dw_getWidEGS(ymw->ymw_layers);

    // BBBFF
    // GGGFF
    // MMCCL
    // MMCCL
    // UPPPP
    /*
    GdkRGBA red = {1, 0, 0, 0};
    GdkRGBA green = {0, 1, 0, 0};
    widget_setBorderThClr(main_Vbox, 5, &green);
    widget_setBorderThClr(top_Hbox, 5, &red);
    widget_setBorderThClr(mid_Hbox, 5, &red);
    widget_setBorderThClr(bot_Hbox, 5, &red);
    widget_setBorderThClr(top_menu_Vbox, 5, &green);
    */
#if 0
    widget_place_FF(grid,menuBarre,0,0,1,3);
    widget_place_FF(grid, message,   1,0 , 1,3);
    widget_place_FF(grid, mousefun,  0,3 , 2,2);
    widget_place_FE(grid, modePanel, 2,0 , 2,1);
    widget_place_EE(grid, canvas,    2,1 , 2,3);
    widget_place_FE(grid, layers,    2,4 , 2,1);
    widget_place_FF(grid, update,    4,0 , 1,1);
    widget_place_FF(grid, dpPanel,   4,1 , 1,4);
#else
    gtk_box_pack_start(GTK_BOX(top_Hbox),top_menu_Vbox,TRUE,TRUE,10);
    gtk_widget_set_vexpand(menuBarre,FALSE);
    gtk_box_pack_start(GTK_BOX(top_menu_Vbox),menuBarre,TRUE,TRUE,0);
    gtk_widget_set_vexpand(message,FALSE);
    gtk_box_pack_start(GTK_BOX(top_menu_Vbox),message,TRUE,TRUE,0);
    gtk_widget_set_hexpand(mousefun,FALSE);
    gtk_widget_set_vexpand(mousefun,FALSE);
    gtk_box_pack_start(GTK_BOX(top_Hbox),mousefun,FALSE,FALSE,0);
    
    gtk_box_pack_start(GTK_BOX(mid_Hbox),modePanel,FALSE,FALSE,10);
    gtk_box_pack_start(GTK_BOX(mid_Hbox),canvas,TRUE,TRUE,10);
    gtk_widget_set_hexpand(layers,FALSE);
    gtk_box_pack_start(GTK_BOX(mid_Hbox),layers,FALSE,FALSE,10);
    //gtk_box_pack_start(GTK_BOX(bot_Hbox),update_frame,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(bot_Hbox),update,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(bot_Hbox),dpPanel,TRUE,TRUE,0);
    
#endif
}

extern void gui_yfig_delete     (Tyfig_mw* ymw)
{
    GtkWidget* grid = (GtkWidget*)ymw->ymw_widEGS;
    ymw->ymw_widEGS = NULL;
    gtk_widget_destroy(grid);
}

extern void        gui_yfig_setTitle(Cyfig_mw* ymw, TstrBrief t)
{
    Tvgui*      gui  = ymw->ymw_gui;
    gtk_window_set_title(GTK_WINDOW(E_TopForm), t);
}

/*======================================================================*/
