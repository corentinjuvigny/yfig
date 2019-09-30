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
/*= Implementation of dialog layouts.                                  =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/vgui-mw.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/
//#define SEP_BP_V 10

#define SEP_H     VDW_SEP_H
#define SEP_V     VDW_SEP_V

/*======================================================================*/

extern void gui_dw_finishMil(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,cc;
    int l=0,c=0;
    int sep_h=SEP_H;
    int sep_v=SEP_V;
    GtkWidget* parent = self->D_widEGS;
    GList *children;
#if 0
    if  ( VGUI_D_isTopDial(self) ) {
        children = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
    }
    GtkWidget* grid = children->data;
#endif
    GtkWidget* grid;
    if  ( VGUI_D_isTopDial(self) ) {
        GList* child = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
        children = gtk_container_get_children(GTK_CONTAINER(child->data));
        grid = gtk_bin_get_child(GTK_BIN(children->data));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
        grid = children->data;
    }

    /* place the title */
    if ( title )
        widget_place_EF(grid,(GtkWidget*)title,l++,0,1,nc);


    /* place the widget */
    for ( ll=0 ; ll<nl ; ll+=1 ) {
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);        
        gtk_grid_attach(GTK_GRID(grid),box,0,l++,1,1);
        for ( cc=0; cc<nc ; cc++ ) {
            gtk_box_set_spacing(GTK_BOX(box),sep_v+mtx[ll][c].sepHV);
            if ( mtx[ll][cc].widEGS==0 ) continue;
            GtkWidget* qw = (GtkWidget*)mtx[ll][cc].widEGS;
            if (  mtx[ll][cc].grow==0 ) {
                gtk_box_pack_start(GTK_BOX(box),qw,FALSE,FALSE,0);
            } else {
                gtk_box_pack_start(GTK_BOX(box),qw,TRUE,TRUE,0);
            }
        }
        GtkWidget* fill = gtk_label_new("");
        gtk_widget_set_hexpand(fill,TRUE);
        gtk_widget_set_vexpand(fill,TRUE);
        gtk_box_pack_start(GTK_BOX(box),fill,TRUE,TRUE,0);
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        Cvgui_dwTop* top = self->D_top;
        if ( top==0 )
            widget_place_FF(grid,(GtkWidget*)buttons,l,0,1,1);
        else switch ( (top->dt_btnLayout>1)&3 ) {
            case 1: // east
              widget_place_FF(grid,(GtkWidget*)buttons,0,1,l,1);
              break;
            default:
              widget_place_FF(grid,(GtkWidget*)buttons,l,0,1,1);
              break;
        }
    }
}

/*======================================================================*/

extern void gui_dw_finishMic(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,aboveSep;
    int l=0,c=0;
    int sep_h=self->D_hmargin;
    int sep_v=self->D_vspace;
    if (nl == 0) nl = 1;//FIXME
    if (nc == 0) nc = 1;

    GtkWidget* parent = self->D_widEGS;
    GList *children;
    GtkWidget* grid;
    if  ( VGUI_D_isTopDial(self) ) {
        GList* child = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
        children = gtk_container_get_children(GTK_CONTAINER(child->data));
        grid = gtk_bin_get_child(GTK_BIN(children->data));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
        grid = children->data;
    }

    /* place the title */
    if ( title )
        widget_place_FF(grid,(GtkWidget*)title,l++,0,1,nc);

    /* place the widget */
    for ( c=0 ; c<nc ; c+=1 ) {
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
        widget_place_FE(grid,box,l,c,1,1);
        gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
        int addFiller = 1;
        for ( ll=0; ll<nl ; ll++ ) {
            if ( mtx[ll][c].widEGS==0 ) continue;
            GtkWidget* qw = (GtkWidget*)mtx[ll][c].widEGS;
            gtk_box_set_spacing(GTK_BOX(box),sep_v+mtx[ll][c].sepHV);
            if (  (mtx[ll][c].flag&VDM_V2E_BGrow)==0 ) {
                gtk_box_pack_start(GTK_BOX(box),qw,FALSE,FALSE,0);
            } else if (  (mtx[ll][c].flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                gtk_box_pack_start(GTK_BOX(box),qw,FALSE,FALSE,0);
                addFiller = 0;
            } else if (  (mtx[ll][c].flag&VDM_V2E_HGrow)!=0 ) {
                gtk_box_pack_start(GTK_BOX(box),qw,FALSE,FALSE,0);
            } else if (  (mtx[ll][c].flag&VDM_V2E_VGrow)!=0 ) {
                gtk_box_pack_start(GTK_BOX(box),qw,TRUE,TRUE,0);
                addFiller = 0;
            }
        }
#if 1
        if ( addFiller ) {
            GtkWidget* fill = gtk_label_new("");;
                gtk_widget_set_hexpand(fill,FALSE);
                gtk_widget_set_vexpand(fill,TRUE);
            gtk_box_pack_start(GTK_BOX(box),fill,FALSE,FALSE,0);
        }
#endif
    }
    l += 1;
    /* place the buttons */
    if ( buttons!=0 ) {
        widget_place_FF(grid,(GtkWidget*)buttons,l,0,1,nc);
        gtk_widget_set_halign((GtkWidget*)buttons, GTK_ALIGN_END);
    }
    //gtk_widget_show_all(parent);FIXME S'affiche jamais sinon :)
}

/*======================================================================*/

extern void gui_dw_finishMfa(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,aboveSep;
    int l=0,c=0;
    int sep_h=SEP_H;
    int sep_v=SEP_V;
    GtkWidget* parent = self->D_widEGS;
    GList *children;
#if 0
    if  ( VGUI_D_isTopDial(self) ) {
        children = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
    }
    GtkWidget* grid = children->data;
#endif
    GtkWidget* grid;
    if  ( VGUI_D_isTopDial(self) ) {
        GList* child = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
        children = gtk_container_get_children(GTK_CONTAINER(child->data));
        grid = gtk_bin_get_child(GTK_BIN(children->data));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
        grid = children->data;
    }

    /* place the title */
    if ( title ) {
        widget_place_EF(grid,(GtkWidget*)title,l,0,1,nc);
        l += 1;
    }

    /* place the widget */
    for ( ll=0; ll<nl ; ll++,l+=1 ) {
        for ( c=0 ; c<nc ; c+=1 ) {
            Tvgui_dwMtxCell* p = &mtx[ll][c];
            if ( p->widEGS==0 ) continue;
            if ( p->xs==0 || p->ys==0 ) continue;
            if ( (p->flag&VDM_V2E_BGrow)==0 ) {
                widget_place_FF(grid,(GtkWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                widget_place_EE(grid,(GtkWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_HGrow)!=0 ) {
                widget_place_EF(grid,(GtkWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_VGrow)!=0 ) {
                widget_place_FE(grid,(GtkWidget*)p->widEGS,l,c,p->ys,p->xs);
            }
        }
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        widget_place_FF(grid,(GtkWidget*)buttons,l,0,1,nc);
        gtk_widget_set_halign((GtkWidget*)buttons, GTK_ALIGN_END);
    }
}

/*======================================================================*/

