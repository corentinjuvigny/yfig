/*======================================================================
 *  Copyright 2018 Corentin Juvigny
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
 *$Software: YFIG-GTK (Interface to Gtk)
 *      $Id: widgtk-buttonPanel.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/widgtk-buttonPanel.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Lines, rows and matrix of buttons (EGS driver).                    =*/
/*=                                                                    =*/
/*= See the vgui/wid-buttonPanel.h header for the type and function    =*/
/*= documentation.                                                     =*/
/*= See the guigtk/widgtk-buttonPanel.h header for the panel GTK classes. =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

/*======================================================================*/
/*= Panels                                                             =*/

/*Function which return the maximum number of column*/
static int maxCoAlc() {return 7;}

static void TGUI_buttonPanel_resize(GtkWidget       *widget,
                                    GtkAllocation   *allocation,
                                    gpointer        user_data);

GtkWidget* global_local; //Used for tests

static void TGUI_buttonPanel_mappingAlc(Twid_buttonPanel* panel, int colnb)
{
    int i, colNb, rowNb;
    int rows   [VGUI_BP_BoutonNbMax];
    int cols   [VGUI_BP_BoutonNbMax];
    int colspan[VGUI_BP_BoutonNbMax];

    colNb = colnb;
    rowNb = wid_buttonPanel_getLayout(panel, colNb, rows, cols, colspan);
            gtk_grid_set_row_homogeneous(panel->bp_widEGSext,FALSE);
            gtk_grid_set_column_homogeneous(panel->bp_widEGSext,FALSE);
    for (i = 0; i < panel->vgbpm_buttonNb; i++) {
        Twid_button* b = panel->vgbpm_buttons[i];
        GtkWidget* w = (GtkWidget*)b->but_widEGS;
        widget_place_FF(panel->bp_widEGSext, w, rows[i], cols[i],1, colspan[i]);
    }
}

static void TGUI_buttonPanel_resize(GtkWidget      *widget,
                                    GtkAllocation  *allocation,
                                    gpointer       user_data)
{

    Twid_buttonPanel* panel = (Twid_buttonPanel*)user_data;
    if WID_BP_RaCa_IsFlc(panel) return;

    int i, col, row;
    int rows   [VGUI_BP_BoutonNbMax];
    int cols   [VGUI_BP_BoutonNbMax];
    int colspan[VGUI_BP_BoutonNbMax];
    //GdkEventConfigure configure = event->configure;
    //int dx = configure.width;
    int dy = allocation->height;
    int eDx, eDy;
    

    for (col = 1; col < maxCoAlc(); col++) {
        row = wid_buttonPanel_getLayout(panel, col, rows, cols, colspan);
        eDx = wid_buttonPanel_getWidth(panel, col) + 30;
        eDy = wid_buttonPanel_getHeight(panel, row);
        if ( eDy < dy ) {
            if ( row != panel->bp_rownb || col != panel->bp_colnb ) { //FIXME must be checked
                //g_object_ref(GTK_CONTAINER(panel->bp_widEGSext));
                for (i = 0; i < panel->vgbpm_buttonNb; i++) {
                    g_object_ref(panel->bp_btns[i]->but_widEGS);
                    gtk_container_remove(GTK_CONTAINER(panel->bp_widEGSext), panel->bp_btns[i]->but_widEGS);
                    //FIXME semble supprimer le signal avec ou alors désactive les boutons
                }
                //gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID(panel->bp_widEGSext), 0, )); //TODO ne va probablement pas marcher
                gtk_container_remove(GTK_CONTAINER(panel->bp_widEGSext), global_local);
                TGUI_buttonPanel_mappingAlc(panel, col);
            }
            //allocation->width = eDx;
            //int minimum_width;
            //gtk_widget_get_preferred_width(widget, &minimum_width, NULL);
            //gtk_widget_size_allocate(widget, allocation);
            return;
        }
    }
}

static void TGUI_buttonPanel_finishAlc(Twid_buttonPanel* panel)
{
    TGUI_buttonPanel_mappingAlc(panel, 3);
}

static void TGUI_buttonPanel_finishFlc(Twid_buttonPanel* panel)
{
    int colnb, rownb;
    if ( panel->bp_colnb != 0 && panel->bp_rownb == 0) {
        colnb = panel->bp_colnb;
        rownb = (panel->bp_btnNb + colnb - 1) / colnb;
    } else if ( panel->bp_colnb==0 && panel->bp_rownb!=0 ) {
        rownb = panel->bp_rownb;
        colnb = (panel->bp_btnNb+rownb-1)/rownb;
    } else if ( panel->bp_colnb!=0 && panel->bp_rownb!=0 ) {
        colnb = panel->bp_colnb;
        rownb = panel->bp_rownb;
    } else {
        PERM_spf_printf("either col or row number must be specified",0);
    }
    int l, c, idx;
    GtkWidget* w;
    for ( l = 0, idx = 0; l < rownb; l++) {
        for (c = 0; c < colnb; c++) {
            if ( idx < panel->bp_btnNb ) {
                w = panel->bp_btns[idx++]->but_widEGS;
                gtk_widget_unparent(w);
                widget_place_FF(panel->bp_widEGSext, w, l, c, 1, 1);
            }
        }
    }
    gtk_widget_show_all(panel->bp_widEGSext);
}

static GtkWidget* buttonPannel_create_scrollBar(GtkOrientation orientation)
{
    GtkWidget* sw;
    GtkAdjustment* adj;

    adj = gtk_adjustment_new(0, 0, 100, 1, 1, 5);
    if ( orientation == GTK_ORIENTATION_HORIZONTAL ) {
        sw = gtk_scrolled_window_new(adj, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    } else if ( orientation == GTK_ORIENTATION_VERTICAL ) {
        sw = gtk_scrolled_window_new(NULL, adj);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_hexpand(sw,FALSE);
        //gtk_window_set_interactive_debugging(TRUE); //#FIXME cool option if debug is needed
     } else
        TEMP_ase_printf(1, "%s", "unexpected orientation");
    return sw;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void gui_buttonPanel_finishRaCa(Twid_buttonPanel* panel)
{
    if (WID_BP_RaCa_IsFlc(panel)) TGUI_buttonPanel_finishFlc(panel);
    else  {
        TEMP_ase_printf(
            panel->bp_buttonWidth==0 || panel->bp_buttonHeight==0,
           "bp_buttonWidth=%d bp_buttonHVsep=%d : must be both !=0",
            panel->bp_buttonWidth, panel->bp_buttonHeight);
        TGUI_buttonPanel_finishAlc(panel);
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void gui_buttonPanel_finishRnC1(Twid_buttonPanel* panel)
{
    int i;
    GtkWidget* button_panel = panel->bp_scroll ?
                                        gtk_box_new(GTK_ORIENTATION_VERTICAL, 1) :
                                        panel->bp_widEGSext;
    gtk_widget_set_hexpand(button_panel,FALSE);
    for (i = 0; i < panel->vgbpm_buttonNb; i++) {
        Twid_button* b = panel->vgbpm_buttons[i];
        GtkWidget* button = (GtkWidget*)b->but_widEGS;
        gtk_box_pack_start(GTK_BOX(button_panel), button, FALSE, FALSE, 1);
    }
    if (panel->bp_scroll)
        gtk_container_add(GTK_CONTAINER(panel->bp_widEGSext), button_panel);
    panel->bp_widEGSint = button_panel;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void gui_buttonPanel_finishR1Cn(Twid_buttonPanel* panel)
{
    int i;
    GtkWidget* button_panel = panel->bp_scroll ?
                                gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1) :
                                panel->bp_widEGSext;
    for (i = 0; i < panel->vgbpm_buttonNb; i++) {
        Twid_button* b = panel->vgbpm_buttons[i];
        GtkWidget* button = (GtkWidget*)b->but_widEGS;
        gtk_container_add(GTK_CONTAINER(button_panel), button);
    }
    if (panel->bp_scroll)
        gtk_container_add(GTK_CONTAINER(panel->bp_widEGSext), button_panel);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void gui_buttonPanel_update(Twid_buttonPanel* panel)
{
    int i;
    for (i = 0; i < panel->bp_btnNb; i++) {
        g_object_ref(panel->bp_btns[i]->but_widEGS);
        gtk_container_remove(GTK_CONTAINER(panel->bp_widEGSext), panel->bp_btns[i]->but_widEGS);
    }
    gui_buttonPanel_finishRaCa(panel);
}    

extern Tegs_widget gui_buttonPanel_build(Twid_buttonPanel* panel, Tegs_widget p)
{
    GtkWidget* button_panel;

    if ( panel->vgbpm_type == 'P' ) {
        button_panel = gtk_grid_new();
        gtk_grid_set_column_spacing(GTK_GRID(button_panel), panel->bp_buttonHVsep);
        gtk_grid_set_row_spacing(GTK_GRID(button_panel), panel->bp_buttonHVsep);
        gtk_widget_set_hexpand(button_panel, FALSE);
        gtk_grid_set_row_homogeneous(GTK_GRID(button_panel),TRUE);
        gtk_grid_set_column_homogeneous(GTK_GRID(button_panel),TRUE);
    } else if ( panel->vgbpm_type == 'H' ) {
        if (panel->bp_scroll) {
            button_panel = buttonPannel_create_scrollBar(GTK_ORIENTATION_HORIZONTAL);
        } else {
            button_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
        }
    } else if ( panel->vgbpm_type == 'V' ) {
        if (panel->bp_scroll) {
            button_panel = buttonPannel_create_scrollBar(GTK_ORIENTATION_VERTICAL);
        } else {
            button_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
        }
    } else
        TEMP_spf_printf("0x%02x panel type not recognized.", panel->vgbpm_type);
    return (Tegs_widget) button_panel;
}
