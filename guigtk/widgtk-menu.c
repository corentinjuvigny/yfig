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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: widgtk-menu.cc 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/widgtk-menu.cc $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets (EGS driver).                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-menu.h header for documentation.                  =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/wid-simple.h"
#include "vgui/wid-menu.h"
#include "vgui/wid-egs.h"

/*======================================================================*/
/*= bar menu widget                                                    =*/

//Callback function which serve as torchbearer between the menu which has just been selected
//and the function which will handle actions.
static void bar_cb_entrySelected(GtkMenuItem *menuitem, gpointer user_data) 
{
    Twidm_entry* me   = widget_getVguiData((GtkWidget*)menuitem);
    Twidm_any*   menu = me->me_menu;
    menu->ma_guiCB(me->me_menu, me);
}

static void gui_menu_createBySet(Tvgui* gui, Twidm_entrySet* s, Tegs_widget menu);

//Create a menu from an entrySet s
static void gui_menu_create_addEntry(Tvgui* gui, GtkWidget* menuWdg,
        Twidm_entry* me, GtkAccelGroup* accel_group)
{
    char label[1024];
    GtkWidget* menuitem = NULL;
    if ( me->me_kind == GUI_ME_ItemSep ) {
        menuitem = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(menuWdg), menuitem);
    } else if ( me->me_kind == GUI_ME_ItemStr ) {
        TaccelDef accel;
        accel_byAmpersand(label, me->me_label, &accel, NOT_COPY_IN_AMPERSAND);
        if ( accel==0 ) accel = me->me_accelerator;
        if ( me->me_checkable ) {
            menuitem = gtk_check_menu_item_new();
            gtk_check_menu_item_set_active( (GtkCheckMenuItem*)menuitem,  me->me_checked);
        } else
            menuitem = gtk_menu_item_new();
        widget_setVguiData(menuitem, me);

        if ( PXMEGS_IsDefined(me->me_pixmap) ) {
            GtkWidget* box   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
            GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(me->me_pixmap));
            gtk_container_add(GTK_CONTAINER(box), image);
            g_object_unref(image);
            if (me->me_label) {
                GtkWidget* wlabel = gtk_label_new(label);
                gtk_container_add(GTK_CONTAINER(box), wlabel);
            }
            gtk_container_add(GTK_CONTAINER(menuitem), box);
        } else {
            gtk_menu_item_set_label((GtkMenuItem*)menuitem,label);
        }

        if (accel_group && me->me_appCB && accel != 0) {
            GdkModifierType accel_mods; int accel_key;
            conv_accelerator2Gtk(accel, &accel_key, &accel_mods);
            TEMP_asf_printf(accel_key==0, "%s", "Unexpected Null Accel_key"); 
            gtk_widget_add_accelerator(menuitem, "activate", accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
        }

        if ( me->me_checkable )
        g_signal_connect(G_OBJECT(menuitem), "toggled", G_CALLBACK(bar_cb_entrySelected), NULL);
        else
        g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(bar_cb_entrySelected), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menuWdg), menuitem);
    } else if ( me->me_kind == GUI_ME_ItemSubmenu ) {
        if ( me->me_checkable ) {
            menuitem = gtk_check_menu_item_new();
            gtk_check_menu_item_set_active( (GtkCheckMenuItem*)menuitem,  me->me_checked);
        } else
            menuitem = gtk_menu_item_new();
        widget_setVguiData(menuitem, me);
        if ( PXMEGS_IsDefined(me->me_pixmap) ) {
            GtkWidget* box   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
            GtkWidget* image = gtk_image_new_from_pixbuf(PXMegs2gtk(me->me_pixmap));
            gtk_container_add(GTK_CONTAINER(box), image);
            g_object_unref(image);
            if (me->me_label) {
                GtkWidget* wlabel = gtk_label_new(me->me_label);
                gtk_container_add(GTK_CONTAINER(box), wlabel);
            }
            gtk_container_add(GTK_CONTAINER(menuitem), box);
        } else {
            gtk_menu_item_set_label((GtkMenuItem*)menuitem,me->me_label);
        }
#if 1
        Twidm_any* submenu  = me->me_submenu;
        submenu->ma_widEGS  = gtk_menu_new();
        submenu->ma_btnEGS = submenu->ma_widEGS;
        gui_menu_createBySet(gui,submenu->ma_entries,submenu->ma_widEGS);
        gtk_menu_item_set_submenu((GtkMenuItem*)menuitem,submenu->ma_widEGS);
#endif
        //g_signal_connect(G_OBJECT(menuitem), "activate", G_CALLBACK(bar_cb_entrySelected), NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menuWdg), menuitem);
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }        
    me->me_widEGS = menuitem;
}

//Create a menu from an entrySet s
static void gui_menu_createBySet(Tvgui* gui, Twidm_entrySet* s, Tegs_widget menu)
{
    GtkWidget* menuWdg = (GtkWidget*)menu;
    GtkAccelGroup* accel_group;
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(gui->g_main->m_topForm), accel_group);
    int i;
    for (i = 0; i < s->eleNb; i++) {
        gui_menu_create_addEntry(gui,menuWdg,s->eles[i],accel_group);
    }        
}

/*======================================================================*/
/*= DDL widget                                                         =*/

//Callback of guiXm_ddl_build while a change happens on combo_box
//set menu->m_selectedEntry to the # of the entry currently selected
static void guiXm_ddl_on_changed(GtkComboBox* widget, void* user_data)
{
    Twidm_ddl* menu = (Twidm_ddl*)user_data;
    menu->m_selectedEntry = gtk_combo_box_get_active(widget); 
    widget_setVguiData(menu->m_widEGS, menu->m_entries->eles[menu->m_selectedEntry]);
    Twidm_entry *entry = menu->m_entries->eles[menu->m_selectedEntry]; 
    menu->m_guiCB(&menu->any,entry);
}

static void guiXm_ddl_build(Twidm_ddl* menu,    Tegs_widget parent,
                            CstrBrief  menuRes, CstrBrief   btnRes)
{
    Tvgui* gui = menu->m_gui;
    GtkListStore* listStore;
    GtkWidget* self;
    GtkCellRenderer* column;
    GtkWidget* box;
    GtkWidget* button;
    GtkCellRenderer* renderer;
    GtkTreeIter iter;
    int i, test;

    Twidm_entry *entry = menu->m_entries->eles[0];
    if(PXMEGS_IsDefined(entry->me_pixmap)) {
        listStore = gtk_list_store_new(1, G_TYPE_OBJECT);
        test = 1;
    } else {
        test = 0;
        listStore = gtk_list_store_new(1, G_TYPE_STRING);
    }
    for (i = 0; i < menu->m_entries->eleNb; i++) {
        Twidm_entry* entry = menu->m_entries->eles[i];
        if ( (entry->me_target&GUI_ExtSys_GTK) == 0 ) continue;
        if ( entry->me_kind != GUI_ME_ItemStr ) continue;
        gtk_list_store_append(listStore, &iter);
        if ( PXMEGS_IsDefined(entry->me_pixmap) ) {
            gtk_list_store_set(listStore,&iter,0,PXMegs2gtk(entry->me_pixmap),-1);
        }
        else if ( entry->me_label ) {
            gtk_list_store_set(listStore,&iter,0,entry->me_label,-1);
        }
    }   
    self = gtk_combo_box_new_with_model(GTK_TREE_MODEL(listStore));
    menu->m_btnEGS = self;
    menu->m_widEGS = self;
    g_object_unref(listStore);
    if (test == 0) {
        renderer = gtk_cell_renderer_text_new();
        gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(self), renderer, TRUE);
        gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(self), renderer, "text", 0);
    } else {
        renderer = gtk_cell_renderer_pixbuf_new();
        gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(self), renderer, FALSE);
        gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(self), renderer, "pixbuf", 0);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(self), menu->m_selectedEntry);
    g_signal_connect(self, "changed", G_CALLBACK(guiXm_ddl_on_changed), (void*)menu);

    widget_setVguiData(self, menu->m_entries->eles);
}

/*======================================================================*/
/*= Interface: VGUI --> GUI Interface                                  =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Entry widget                                                       =*/

static void guigtkm_vt_entry_setEnablement(Cwidm_entry*me, int enabled)
{ Tegs_widget w=me->me_widEGS;
  if (enabled) gui_widegs_enable(w); else gui_widegs_disable(w); }

static void guigtkm_vt_entry_update(Cwidm_entry* me)
{
    TaccelDef accel;
    char label[1024];
    GtkMenuItem* menuitem = me->me_widEGS;
    if ( (me->me_target&GUI_ExtSys_GTK)==0 ) return;
    if ( me->me_kind == GUI_ME_ItemStr || me->me_kind == GUI_ME_ItemSubmenu ) {
        if ( me->me_label!=0 ) {
            accel_byAmpersand(label, me->me_label, &accel, NOT_COPY_IN_AMPERSAND);
            gtk_menu_item_set_label(menuitem,label);
        }
        if (me->me_checkable ) {
            gtk_check_menu_item_set_active( (GtkCheckMenuItem*)menuitem,  me->me_checked);
        }
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }
}

static void guigtkm_vt_lastEntryAdd(Twidm_any*self)
{
    Tvgui* gui = self->ma_gui;
    GtkWidget* menuWdg = (GtkWidget*)self->ma_widEGS;
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    gui_menu_create_addEntry(gui,menuWdg,me,NULL);
    gtk_widget_show_all(menuWdg);
}

static void guigtkm_vt_lastEntryDel(Twidm_any*self)
{
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    GtkWidget* menuitem = (GtkWidget*)me->me_widEGS;
    gtk_widget_destroy( menuitem );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= DDL widget                                                         =*/

static void guigtkm_vt_ddl_dbf(Twidm_ddl* menu,   Tegs_widget parent,
                              CstrBrief menuRes, CstrBrief btnRes)
{ guiXm_ddl_build(menu,parent, menuRes,btnRes); }

static void guigtkm_vt_ddl_setCurrentEntry(Twidm_ddl* menu)
{
    GtkWidget* self = (GtkWidget*)menu->m_widEGS;
    g_signal_handlers_block_by_func(G_OBJECT(self),G_CALLBACK(guiXm_ddl_on_changed),(void*)menu);
    gtk_combo_box_set_active(GTK_COMBO_BOX(self), menu->m_selectedEntry);
    g_signal_handlers_unblock_by_func(G_OBJECT(self),G_CALLBACK(guiXm_ddl_on_changed),(void*)menu);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Menu bar                                                           =*/

static Tegs_widget guigtkm_vt_tmb_build(Twidm_bar*tmb, Tegs_widget parent) { return gtk_menu_bar_new(); }

static Tegs_widget guigtkm_vt_tmb_sm_dbf(Twidm_bar*tmb, Twidm_popup*menu)
{
    Tvgui* gui = menu->m_gui;
    menu->m_btnEGS = gtk_menu_item_new_with_label(menu->m_label);
    menu->m_widEGS  = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->m_btnEGS), menu->m_widEGS);
    gui_menu_createBySet(gui, menu->m_entries, menu->m_widEGS);
    gtk_menu_shell_append(GTK_MENU_SHELL(tmb->mb_widEGS), menu->m_btnEGS);
    return menu->m_btnEGS;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= The GUI vtable.                                                    =*/

struct _Twidm_guiVt guigtk_VtMenu = {
    .entry_setEnablement = guigtkm_vt_entry_setEnablement,
    .entry_update        = guigtkm_vt_entry_update,
    .any_lastEntryAdd    = guigtkm_vt_lastEntryAdd,
    .any_lastEntryDel    = guigtkm_vt_lastEntryDel,
    .ddl_setCurrentEntry = guigtkm_vt_ddl_setCurrentEntry,
    .ddl_dbf             = guigtkm_vt_ddl_dbf,
    .tmb_build           = guigtkm_vt_tmb_build,
    .tmb_sm_dbf          = guigtkm_vt_tmb_sm_dbf,
//  .tmb_finish          = 0,
};

/*======================================================================*/
