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
 *$Software: YFIG-GTK (VGUI driver to GTK)
 *      $Id: widgtk-button.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/widgtk-button.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons (EGS driver).                                =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for the type and function         =*/
/*= documentation.                                                     =*/
/*= See the guigtk/widgtk-button.h header for the button GTK classes.  =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/wid-button.h"
#include "vgui/wid-egs.h"
#include "vgui/vgui-accelkey.h"
#include "vgui/vgui-mw.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/
/*= The ornament button                                                =*/


extern Tegs_widget gui_button_orn_dbf(Twid_button* b, Tegs_widget pw)
{
    GtkWidget* button = gtk_label_new(b->but_orn_text);
    gtk_widget_set_hexpand(button, FALSE);
    widget_setPaddingTh(button, 1, 0);
    return button;
} 

/*======================================================================*/
/*= The standard button                                                =*/

static void gui_button_std_accel_cb(gpointer user_data)
{
    Twid_button* self = (Twid_button*)user_data;
    wid_button_std_cb(self, 1);
}

static void gui_button_std_clicked(GtkWidget* button, GdkEvent* event, gpointer user_data)
{
    Twid_button* self = (Twid_button*)user_data;
    GdkEventButton* event_button = (GdkEventButton*)event;
    wid_button_std_cb(self, event_button->button);
} 

static void gui_button_std_scroll(GtkWidget* button, GdkEvent* event, gpointer user_data)
{
    Twid_button* self = (Twid_button*)user_data;
    GdkScrollDirection direction;
    //TODO tenir compte de la vitesse de la molette (regarder gdk_get_scroll_delta())
    gdk_event_get_scroll_direction(event, &direction);
    switch (direction) {
        case GDK_SCROLL_UP :
            wid_button_std_cb(self, 4);
            break;
        case GDK_SCROLL_DOWN :
            wid_button_std_cb(self, 5);
            break;
        default :
            TEMP_ase_printf(1, "%s", "Unexpected scroll direction");
    }
}

extern Tegs_widget gui_button_std_dbf (Twid_button* b, Tegs_widget pw)
{
    TaccelDef accelerator1,accelerator2;

    //Creation of button
    GtkWidget* button = gtk_button_new();
    gtk_widget_set_hexpand(button, FALSE);

    // set button content
    if ( PXMEGS_IsDefined(b->but_std_pixmap) ) {
        accelerator1 = b->but_accelKey0;
        accelerator2 = b->but_accelKey1;
        GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(b->but_std_pixmap));
        gtk_button_set_image(GTK_BUTTON(button), image);
    } else if ( b->but_std_text ) {
        char label[1024];
        if ( b->but_accelKey0==0 ) {
            accel_byAmpersand(label, b->but_std_text, &accelerator1, COPY_IN_AMPERSAND);//TODO verifier utilité test
            accelerator2 = b->but_accelKey1;
        } else {
            accelerator1 = b->but_accelKey0;
            if ( b->but_accelKey1==0 )
                accel_byAmpersand(label, b->but_std_text, &accelerator2, COPY_IN_AMPERSAND);//TODO verifier utilité test
            else
                strcpy(label,b->but_std_text);
        }
        gtk_button_set_label(GTK_BUTTON(button), label);
    } // else { TEMP_spw_printf("Unexpected lack of label (%s) or pixbuf (%p)",b->but_std_text,b->but_std_pixmap); }

    //Add of signals
    gtk_widget_set_events(button, GDK_BUTTON_PRESS_MASK | GDK_SCROLL_MASK);
    g_signal_connect(button, "button-press-event", G_CALLBACK(gui_button_std_clicked), b);
    g_signal_connect(button, "scroll-event", G_CALLBACK(gui_button_std_scroll), b);

    //Add accel_keys
    if ( accelerator1!=0 || accelerator1!=0 ) {
        int             accel_key;
        GdkModifierType accel_mods;
        GClosure*       closure;
        GtkAccelGroup*  accel_group = gtk_accel_group_new();
        if ( accelerator1 != 0 ) {
            conv_accelerator2Gtk(accelerator1, &accel_key, &accel_mods);
            closure = g_cclosure_new_swap(G_CALLBACK(gui_button_std_accel_cb), b, NULL);
            //gtk_widget_add_accelerator(button, "button-press-event", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
            TEMP_asf_printf( accel_key<=0, "unexpected %d accel_key.",accel_key);
            gtk_accel_group_connect(accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE, closure);
        }
        if ( accelerator2 != 0 ) {
            conv_accelerator2Gtk(accelerator2, &accel_key, &accel_mods);
            closure = g_cclosure_new_swap(G_CALLBACK(gui_button_std_accel_cb), b, NULL);
            //gtk_widget_add_accelerator(button, "button-press-event", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
            TEMP_asf_printf( accel_key<=0, "unexpected %d accel_key.",accel_key);
            gtk_accel_group_connect(accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE, closure);
        }
    }

    //Add helpmessage
    gui_widegs_addTipAA(button, b->but_helpText, accelerator1, accelerator2);
    widget_setPaddingTh(button, 5, 0);

    return button;
}

/*======================================================================*/
/*= The On/Off button                                                  =*/

static void gui_button_onoff_accel_cb(gpointer user_data)
{
    Twid_button* self = (Twid_button*)user_data;
    wid_button_onoff_cb(self, 1);
}

static void gui_button_onoff_clicked(GtkWidget* button, GdkEvent* event, gpointer user_data)
{
    Twid_button* self = (Twid_button*)user_data;
    GdkEventButton* event_button = (GdkEventButton*)event;
    wid_button_onoff_cb(self, event_button->button);
} 

static GtkWidget* buttonOnOff_dbf(Twid_button* b, Tegs_widget pw)
{
    //Creation of button
    GtkWidget* button = gtk_button_new();
    gtk_widget_set_hexpand(button, FALSE);

    // set button content
    if ( b->but_onoff_usePixmap ) {
        GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(b->but_onoff_pixmapOff));
        gtk_button_set_image(GTK_BUTTON(button), image);
    } else {
        gtk_button_set_label(GTK_BUTTON(button), b->but_onoff_textOff);
    }

    //Add Callback
    gtk_widget_set_events(button, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(button, "button-press-event", G_CALLBACK(gui_button_onoff_clicked), b);

    //Add AccelKeys
    TaccelDef accelerator1=b->but_accelKey0;
    TaccelDef accelerator2=b->but_accelKey1;
    if ( accelerator1!=0 || accelerator1!=0 ) {
        int             accel_key;
        GdkModifierType accel_mods;
        GClosure*       closure;
        GtkAccelGroup*  accel_group = gtk_accel_group_new();
        if ( b->but_vmw ) {
            gtk_window_add_accel_group(GTK_WINDOW(b->but_vmw->gui->g_main->m_topForm), accel_group);
        } else if ( b->but_vdw ) {
            //gtk_window_add_accel_group(GTK_WINDOW(b->but_vdw->gui->g_main->m_topForm), accel_group);
        } else {
            TEMP_asf_printf(b->but_std_text == NULL, "%s", "Unexpected position of button");
        }
        if ( accelerator1 != 0 ) {
            conv_accelerator2Gtk(accelerator1, &accel_key, &accel_mods);
            closure = g_cclosure_new_swap(G_CALLBACK(gui_button_onoff_accel_cb), b, NULL);
            //gtk_widget_add_accelerator(button, "button-press-event", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
            TEMP_asf_printf( accel_key<=0, "unexpected %d accel_key.",accel_key);
            gtk_accel_group_connect(accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE, closure);
        }
        if ( accelerator2 != 0 ) {
            conv_accelerator2Gtk(accelerator2, &accel_key, &accel_mods);
            closure = g_cclosure_new_swap(G_CALLBACK(gui_button_onoff_accel_cb), b, NULL);
            //gtk_widget_add_accelerator(button, "button-press-event", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
            TEMP_asf_printf( accel_key<=0, "unexpected %d accel_key.",accel_key);
            gtk_accel_group_connect(accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE, closure);
        }
    }

    //Add helpmessage
    gui_widegs_addTipAA(button, b->but_helpText, accelerator1, accelerator2);

    return button;
}

#define NoAppCB "nocb"

static void buttonCheckBox_stateChanged(GtkWidget* check_button, gpointer data_user)
{

    Twid_button* self = (Twid_button*)data_user;
    if ( g_object_get_data(G_OBJECT(check_button),NoAppCB)==0 )
        wid_button_onoff_cb(self, 0);
}

static GtkWidget* buttonCheckBox_dbf(Twid_button* b, Tegs_widget pw)
{
    GtkWidget* button = gtk_check_button_new_with_label(b->but_onoff_textOff);
    g_signal_connect(button, "toggled", G_CALLBACK(buttonCheckBox_stateChanged), b);
    return button;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Tegs_widget gui_button_onoff_dbf(Twid_button* b, Tegs_widget pw)
{
    GtkWidget* button;
    if ( !b->but_onoff_checkBtn ) 
        button = buttonOnOff_dbf(b, pw);
    else
        button = buttonCheckBox_dbf(b, pw);
    widget_setPaddingTh(button, 1, 0);
    return button;
}

/*======================================================================*/
/*= Button operation                                                   =*/

static void button_onoff_redraw(Twid_button* b, int state)
{
    GtkWidget* button = (GtkWidget*)b->but_widEGS;
    if ( state ) {
        if ( b->but_onoff_usePixmap ) {
            GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(b->but_onoff_pixmapOn));
            gtk_button_set_image(GTK_BUTTON(button), image);
        } else {
            gtk_button_set_label(GTK_BUTTON(button), b->but_onoff_textOn);
        }
    }
    else {
        if ( b->but_onoff_usePixmap ) {
            GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(b->but_onoff_pixmapOff));
            gtk_button_set_image(GTK_BUTTON(button), image);
        } else {
            gtk_button_set_label(GTK_BUTTON(button), b->but_onoff_textOff);
        }
    }
}

static void button_onoff_setOn(Twid_button* b)
{
    GtkWidget* button = (GtkWidget*)b->but_widEGS;
    button_onoff_redraw(b, 1);
}

static void button_CheckBox_setOn(Twid_button* b)
{
    GtkToggleButton* button = (GtkToggleButton*)b->but_widEGS;
    g_object_set_data(G_OBJECT(button),NoAppCB,(void*)0x1);
    gtk_toggle_button_set_active(button, TRUE);
    g_object_set_data(G_OBJECT(button),NoAppCB,(void*)0x0);
}

static void button_onoff_setOff(Twid_button* b)
{
    GtkWidget* button = (GtkWidget*)b->but_widEGS;
    button_onoff_redraw(b, 0);
}

static void button_CheckBox_setOff(Twid_button* b)
{
    GtkToggleButton* button = (GtkToggleButton*)b->but_widEGS;
    g_object_set_data(G_OBJECT(button),NoAppCB,(void*)0x1);
    gtk_toggle_button_set_active(button, FALSE);
    g_object_set_data(G_OBJECT(button),NoAppCB,(void*)0x0);
}

static void button_std_redraw(Twid_button* b)
{
    GtkWidget* button = (GtkWidget*)b->but_widEGS;
    if ( PXMEGS_IsDefined(b->but_std_pixmap) ) {
        GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(b->but_std_pixmap));
        gtk_button_set_image(GTK_BUTTON(button), image);
    } else {
        gtk_button_set_label(GTK_BUTTON(button), b->but_std_text);
    }
}

static void button_CheckBox_redraw(Twid_button* b)
{
    GtkWidget* button = (GtkWidget*)b->but_widEGS;
    gtk_button_set_label(GTK_BUTTON(button),b->but_onoff_textOff);
}

extern void gui_button_setOn(Twid_button* button)
{
    if ( button->but_kind == WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn ) {
            button_CheckBox_setOn(button);
        } else {
            button_onoff_setOn(button);
        }
    } else {
        TEMP_asf_printf(TRUE, "%s", "Unexpected use of setOn function on an object which can't afford it");
    }
}

extern void gui_button_setOff(Twid_button* button)
{
    if ( button->but_kind == WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn ) {
            button_CheckBox_setOff(button);
        } else {
            button_onoff_setOff(button);
        }
    } else {
        TEMP_asf_printf(TRUE, "%s", "Unexpected use of setOff function on an object which can't afford it");
    }
}

extern void gui_button_redraw(Twid_button* button)
{
    if ( button->but_kind == WID_BUT_KD_Standard ) {
        GtkWidget* b = button->but_widEGS; 
        button_std_redraw(button);
    } else if ( button->but_kind == WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn ) button_CheckBox_redraw(button);
        else PERM_spe_printf("Invalid call this button kind (%d)", button->but_kind);
//button_onoff_redraw(button,button->but_onoff_state);
    } else PERM_spe_printf("Invalid call this button kind (%d)", button->but_kind);
}
