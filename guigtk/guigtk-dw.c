/*======================================================================
 *  Copyright 2018 Timothee Denoux
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
 *$Software: YFIG-GTK (VGUI driver to GTK)
 *      $Id: guiqt-dw.cc 97 2017-11-11 17:20:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-dw.cc $
 *  $Author: Timothee Denoux (Email: timothee.denoux@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/vgui-mw.h"
#include "vgui/vgui-dw.h"

//TODO remove
#include "guigtk/guigtk-util.h"

/*======================================================================*/
/*= Running dialog                                                     =*/

static Twid_dialStatus vt_modal_run(Tvgui_dw*d)
{
    Tvgui_dwTop* top = d->top;
    GtkWidget *widget = (GtkWidget *) d->D_widEGS;
    gtk_widget_show_all(widget);
    gtk_dialog_run(GTK_DIALOG(widget));
    return top->dt_buttSelected;
}

static void vt_modal_run2_conf_cb(GtkWindow *window, GdkEvent *event, gpointer data)
{
    if (!GTK_IS_WIDGET(data)) return;
    int wx,wy;
    gtk_window_get_size(window,&wx,&wy);
    int x = event->configure.x;
    int y = event->configure.y + wy - gtk_widget_get_allocated_height(data);
    gtk_window_move(GTK_WINDOW(data), x, y);
}

static Twid_dialStatus vt_modal_run2(Tvgui_dw*d,
                                   Tegs_widget inw, int x, int y)
{
    if ( inw == 0) return vt_modal_run(d);

    Tvgui_dwTop* top = d->top;
    GtkWidget *widget = (GtkWidget *) d->D_widEGS;
    
    GtkWidget* gtkinw = (GtkWidget*)inw;
    GtkWidget *mw = d->vmw->widEGS;
    int wx,wy;
    gdk_window_get_origin (gtk_widget_get_window (mw), &wx, &wy);
    int mw_x,mw_y;
    gtk_window_get_size(GTK_WINDOW(mw),&mw_x,&mw_y);
    int nx = wx;
    int ny = wy + mw_y - gtk_widget_get_allocated_height(inw);
    gtk_window_move(GTK_WINDOW(widget), nx, ny);

    g_signal_connect(G_OBJECT(GTK_WINDOW(mw)),"configure-event",G_CALLBACK(vt_modal_run2_conf_cb),widget);

    gtk_window_set_decorated(GTK_WINDOW(widget),FALSE);
    gtk_widget_show_all(widget);
    gtk_dialog_run(GTK_DIALOG(widget));

    return top->dt_buttSelected;
}

static void vt_modeless_start(Tvgui_dw* d) { gtk_widget_show_all((GtkWidget *)d->D_widEGS); }
static void vt_modal_breakloop(Tvgui_dw*d, Twid_dialStatus s) { gtk_dialog_response(GTK_DIALOG(d->D_widEGS),s); }

static void dw_vtInit(Tvgui_dw* d)
{
    if ( d->top==0 ) return;
    d->top->vt_modal_run       = vt_modal_run;
    d->top->vt_modal_run2      = vt_modal_run2;
    d->top->vt_modal_breakloop = vt_modal_breakloop;
    d->top->vt_modeless_start  = vt_modeless_start;
}

/*======================================================================*/
/*= Constructors and destructor                                        =*/

extern Tegs_widget gui_dw_create(Tvgui_dw* d, CstrBrief wn,
                                int bd, int dx, int dy, int resizable)
{
    Tvgui*      gui = d->D_gui;
    GtkWidget*    parent;

    dw_vtInit(d);
    if  ( VGUI_D_isTopDial(d) ) {
        parent = gtk_dialog_new();
        //parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_transient_for(GTK_WINDOW(parent),GTK_WINDOW(d->vmw->widEGS));
    } else {
        parent = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type(GTK_FRAME(parent),GTK_SHADOW_NONE);
    }
    d->D_widEGS = parent;
    widget_setColors(gui,parent,wn,EGSCOLOR_default,EGSCOLOR_default);

    // now we create the form
    GtkWidget* grid = gtk_grid_new();
     
    if  ( VGUI_D_isTopDial(d) ) {
        GtkWidget *box = gtk_dialog_get_content_area(GTK_DIALOG(parent));
        //gtk_container_add(GTK_CONTAINER(GTK_DIALOG(parent)),grid);
        //gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 1);
    
        GtkAdjustment* adj = gtk_adjustment_new(0, 0, 100, 1, 1, 5);
        GtkWidget* sw = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(sw), TRUE);
        gtk_scrolled_window_set_propagate_natural_width(GTK_SCROLLED_WINDOW(sw), TRUE);
        
        gtk_box_pack_start(GTK_BOX(box), sw, FALSE, FALSE, 1);
        gtk_container_add(GTK_CONTAINER(sw), grid);
        
        //gtk_box_set_baseline_position(GTK_BOX(box), GTK_BASELINE_POSITION_CENTER);
        //gtk_window_set_interactive_debugging(TRUE); //#FIXME cool option if debug is needed
    } else {
        gtk_container_add(GTK_CONTAINER(parent),grid);
    }
    
    int hvsep = d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 2 : 0;
    gtk_grid_set_row_spacing(GTK_GRID(grid),hvsep);
    gtk_grid_set_column_spacing(GTK_GRID(grid),hvsep);
    gtk_widget_set_size_request(grid,dx,dy);
    if  ( VGUI_D_isTopDial(d) )
        gtk_window_set_resizable(GTK_WINDOW(parent),resizable);

    widget_setColors(d->D_gui,grid,wn_dialShell,EGSCOLOR_default,EGSCOLOR_default);
    
    if ( d->pDialog==0 ) {
        gtk_widget_set_margin_end(grid,5);
        gtk_widget_set_margin_bottom(grid,5);
    }
    else if ( d->pDialog->pDialog==0 ) {
        gtk_widget_set_margin_start(grid,5);
        gtk_widget_set_margin_top(grid,5);
        gtk_widget_set_margin_end(grid,2);
        gtk_widget_set_margin_bottom(grid,2);
    }

    if ( bd>0 || d->pDialog==0 || d->pDialog->pDialog==0 ) {
        GtkWidget *frame = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
        //frame->setLineWidth(bd);
        gtk_grid_attach(GTK_GRID(grid),frame,0,0,1,1);
    }
    return d->D_widEGS;
}

extern Tegs_widget gui_dw_create2(Tvgui_dw* d, Tegs_widget parent, CstrBrief wn,
                                int bd, int dx, int dy, int resizable)
{ return gui_dw_create(d,wn,bd,dx,dy,resizable); }

extern void gui_dw_free(Tvgui_dw* d)
{
    if ( VGUI_D_isTopDial(d) ) 
        gtk_widget_destroy((GtkWidget *)d->D_widEGS);
}

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/

extern void            gui_dw_setTitle(Cvgui_dw* d, TstrBrief t)
{
    GtkWidget * widget = (GtkWidget*) d->D_widEGS;
    if  ( VGUI_D_isTopDial(d) )
        gtk_window_set_title(GTK_WINDOW(widget),t);
}

extern void gui_dw_finishSdx(Tvgui_dw*vdw, Tegs_widget*ws, int wn)
{
    PERM_error_NYI();
}

/*======================================================================*/
