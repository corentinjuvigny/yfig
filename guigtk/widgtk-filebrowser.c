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
 *$Software: YFIG-GTK (VGUI driver to GTK)
 *      $Id: widqt-filebrowser.cc 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/widgtk-filebrowser.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= File browser widget (EGS driver).                                  =*/
/*=                                                                    =*/
/*= See the vgui/wid-filebrowser.h header for documentation.           =*/
/*======================================================================*/

#include "guigtk.h" 

#include "vgui/vgui-dw.h"
#include "vgui/wid-filebrowser.h"
#include "tools/file.h"

/*======================================================================*/

/**
 * The fbs->guiUserdata is
 *   - the GTK filebrowser dialog when unembedded (modal) dialog,
 *   - a GTK grid that contains the GTK filebrowser dialog 
 *     when embedded (modeless) dialog
 *
 * This macro gets the GTK filebrowser dialog taking into account
 * the formers cases.
**/
#define FileChooser(fbd) fbd->embedded \
    ? gtk_container_get_children(GTK_CONTAINER(fbd->guiUserdata))->next->data \
    : fbd->guiUserdata

Inline void filebrowser_dw_breakloop(Twid_filebrowser* fbd, Twid_dialStatus s) { vgui_dw_breakloop(fbd->pdialog,s); }

/*======================================================================*/
/*= callback for Modeless and modal file browser                       =*/

static void gui_filebrowser_currentChanged(GtkFileChooser *chooser, gpointer user_data)
{
    TstrEaten file;
    Twid_filebrowser* fbd = user_data;
    char *str = gtk_file_chooser_get_filename(chooser);
    Tvgui_dw *d = fbd->pdialog;
    if ( str==0 || tool_fileDirXXXCheck(str)==0 ) {
        fbd->guiFileIsSelectedCB(fbd,0);
    } else {
        fbd->guiFileIsSelectedCB(fbd,str);
    }
    tool_free (str);
}

/*======================================================================*/
/*= Modeless file browser                                              =*/

static void gui_filebrowser_cancelCB(GtkWidget *widget, gpointer user_data) { filebrowser_dw_breakloop(user_data,WID_DIALSTATUS_Cancel); }
static void gui_filebrowser_extraCB (GtkWidget *widget, gpointer user_data) { Twid_filebrowser* fbd = user_data; filebrowser_dw_breakloop(fbd,fbd->extraStatus); }

static TstrEaten gtk_filebrowser_getFilPath(Twid_filebrowser* fbd);
static void gui_filebrowser_okCB(GtkWidget *widget, gpointer user_data)
{
    Twid_filebrowser* fbd = user_data;
    Tvgui_dw *d = fbd->pdialog;
    TstrEaten file = gtk_filebrowser_getFilPath(fbd);
    if (file != 0) {
        filebrowser_dw_breakloop(fbd,WID_DIALSTATUS_Ok);
    } else {
        fbd->guiFileIsSelectedCB(fbd,0);
    }
    tool_free(file);
}

static GtkWidget* gtk_filebrowser_modeless_nbf(Twid_filebrowser* fbd, GtkFileChooserAction action)
{
    GtkWidget* dialog = gtk_file_chooser_widget_new(action);
    GtkWidget *box    = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,2);
    GtkWidget *grid   = gtk_grid_new();

    widget_place_EF( grid, dialog , 0,0,1,1);
    widget_place_EF( grid, box ,    1,0,1,1);
    g_signal_connect(dialog,"selection-changed",G_CALLBACK(gui_filebrowser_currentChanged),fbd);

    if ( fbd->addBtns ) {
        GtkWidget *okButton     = gtk_button_new_with_label(fbd->okText);
                                  gtk_box_pack_end(GTK_BOX(box),okButton,FALSE,FALSE,0);
        GtkWidget *extraButton  = NULL; 
      if ( fbd->extraText ) {
                   extraButton = gtk_button_new_with_label(fbd->extraText);
                                 gtk_box_pack_end(GTK_BOX(box),extraButton,FALSE,FALSE,0);
      }
        GtkWidget *cancelButton = gtk_button_new_with_label(fbd->cancelText);
                                  gtk_box_pack_end(GTK_BOX(box),cancelButton,FALSE,FALSE,0);
        g_signal_connect(cancelButton,"clicked",G_CALLBACK(gui_filebrowser_cancelCB),fbd);
        g_signal_connect(extraButton, "clicked",G_CALLBACK(gui_filebrowser_extraCB), fbd);
        g_signal_connect(okButton,    "clicked",G_CALLBACK(gui_filebrowser_okCB),    fbd);
    } 

    GtkFileFilter *filter = gtk_file_filter_new();
    char *tmp = strtok(fbd->filemask," ");
    while(tmp)
    {
        gtk_file_filter_add_pattern(filter,tmp);
        tmp = strtok(NULL," ");
    }
    gtk_file_filter_set_name   (filter,fbd->filemaskId);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
    
    GtkFileFilter *filter_any = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter_any,"*");
    gtk_file_filter_set_name(filter_any,"Any files");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter_any);

    return grid;
}

/*======================================================================*/
/*= Private C interfce interface to Tgui_fileBrowser class             =*/

static Tegs_widget gtk_filebrowser_dbf(Twid_filebrowser* fbd)
{
    GtkFileChooserAction action = fbd->existingPath ? GTK_FILE_CHOOSER_ACTION_OPEN :
                                                      GTK_FILE_CHOOSER_ACTION_SAVE;
    if ( fbd->embedded ) {
        fbd->guiUserdata = gtk_filebrowser_modeless_nbf(fbd,action);
    } else {
        GtkWidget* dialog = fbd->guiUserdata = gtk_file_chooser_dialog_new("", NULL, action,
                fbd->cancelText, GTK_RESPONSE_CANCEL,
                "Ok",            GTK_RESPONSE_ACCEPT,
                NULL);
        g_signal_connect(dialog,"selection-changed",G_CALLBACK(gui_filebrowser_currentChanged),fbd);
    }
    return fbd->guiUserdata;
}

static void      gtk_filebrowser_free       (Twid_filebrowser* fbd) { }
static TstrEaten gtk_filebrowser_getFilPath (Twid_filebrowser* fbd) { return gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileChooser(fbd))); }
static void      gtk_filebrowser_setBasName (Twid_filebrowser* fbd,
                                             CstrBrief bn)          { gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(FileChooser(fbd)), bn); }
static void gtk_filebrowser_modalDelete(Twid_filebrowser* fbd)      { gtk_widget_destroy((GtkWidget*)fbd->guiUserdata); }
static int  gtk_filebrowser_modalRun   (Twid_filebrowser*fbd)
{
    GtkFileChooser* gfb = fbd->guiUserdata;
    int res =  gtk_dialog_run (GTK_DIALOG (gfb));
    return res==GTK_RESPONSE_ACCEPT ? WID_DIALSTATUS_Ok : WID_DIALSTATUS_Cancel;
}

/*======================================================================*/
/*= Interface                                                          =*/

extern void gui_filebrowser_init(Twid_filebrowser* fbd )
{
    fbd->vt_dbf          = gtk_filebrowser_dbf;
    fbd->vt_free         = gtk_filebrowser_free;
    fbd->vt_getFilPath   = gtk_filebrowser_getFilPath;
  //fbd->vt_getDirPath   = gtk_filebrowser_getDirPath;
  //fbd->vt_getBasName   = gtk_filebrowser_getBasName;
    fbd->vt_setBasName   = gtk_filebrowser_setBasName;
    fbd->vt_modal_run    = gtk_filebrowser_modalRun;
    fbd->vt_modal_delete = gtk_filebrowser_modalDelete;
}

/*======================================================================*/
