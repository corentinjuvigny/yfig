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
 *$Software: YFIG (YFIG driver to Qt)
 *      $Id: tbgtk-file-OMSF.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/gtk/tbgtk-file-OMSF.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widget for open/merge/save-as YFIG files (EGS driver).      =*/
/*======================================================================*/

#include "../../config.h"
#include "ydw/tb-file-OMSF.h"

#include "guigtk/guigtk-top.h"
#include "guigtk/guigtk-util.h"

#include "vgui/wid-lvu.h"
#include "vgui/wid-filebrowser.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/

extern void gui_OMSFprev_dbf(Tdial_OMSFprev* dp)
{
    Tvgui_dw* self = &dp->any;
    GtkWidget* parent = self->D_widEGS;
    GList *children;
    if  ( VGUI_D_isTopDial(self) ) {
        children = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
    }
    GtkWidget* grid = children->data;


    // line 1, all the columns
    widget_place_FF(grid, (GtkWidget*)dp->df_titleLAB,0,0,1,1);
    widget_place_EF(grid, (GtkWidget*)dp->df_fileNameLAB,0,1,1,3);

    // line2 column 1
    widget_place_FF(grid, (GtkWidget*)dp->df_objnb->lvu_labEGS,1,0,1,1);
    widget_place_FF(grid, (GtkWidget*)dp->df_objnb->lvu_valEGS,1,1,1,1);
    widget_place_FF(grid, (GtkWidget*)dp->df_size->lvu_labEGS, 2,0,1,1);
    widget_place_FF(grid, (GtkWidget*)dp->df_size->lvu_valEGS, 2,1,1,1);
    widget_place_FF(grid, (GtkWidget*)dp->df_size->lvu_uniEGS, 2,2,1,1);
    widget_place_FF(grid, (GtkWidget*)dp->df_commTitleLAB,     3,0,1,1);
    widget_place_EE(grid, (GtkWidget*)dp->df_commValueTOU->widEGS, 3,1, 1,2);

    // line 2, column 2
    widget_place_EE(grid, (GtkWidget*)dp->df_canvas->vgcm_widEGS, 1,3,4,1);
}

#define df_xLAB      df_xINP ->lvu_labEGS
#define df_xWestWid  df_xINP ->lvu_valEGS
#define df_xEastWid  df_xINP ->lvu_uniEGS
#define df_yLAB      df_yINP ->lvu_labEGS
#define df_yWestWid  df_yINP ->lvu_valEGS
#define df_yEastWid  df_yINP ->lvu_uniEGS
#define df_dxWestWid df_dxLWU->lwu_westWidget
#define df_dxEastWid df_dxLWU->lwu_eastWidget
#define df_dyWestWid df_dyLWU->lwu_westWidget
#define df_dyEastWid df_dyLWU->lwu_eastWidget
#define df_scaleLAB  df_scaleDIN->lvu_labEGS
#define df_scaleWid  df_scaleDIN->lvu_valEGS

extern void gui_OMSFpara_dbf(Tdial_OMSFpara* dp)
{
    Tvgui_dw* self = &dp->any;
    GtkWidget* parent = self->D_widEGS;
    GList *children;
    if  ( VGUI_D_isTopDial(self) ) {
        children = gtk_container_get_children(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(parent))));
    } else {
        children = gtk_container_get_children(GTK_CONTAINER(parent));
    }
    GtkWidget* grid = children->data;
    // line 1, 
    widget_place_FF(grid,(GtkWidget*)dp->df_xLAB      , 0,0 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_xWestWid  , 0,1 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_xEastWid  , 0,2 , 1,1);
                                                       
    // line 2,                                         
    widget_place_FF(grid,(GtkWidget*)dp->df_yLAB      , 1,0 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_yWestWid  , 1,1 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_yEastWid  , 1,2 , 1,1);
                                                       
    // line 3,                                         
    widget_place_FF(grid,(GtkWidget*)dp->df_dx->lvu_labEGS , 2,0 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_dx->lvu_valEGS , 2,1 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_dx->lvu_uniEGS , 2,2 , 1,1);
                                                       
    // line 4,                                         
    widget_place_FF(grid,(GtkWidget*)dp->df_dy->lvu_labEGS , 3,0 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_dy->lvu_valEGS , 3,1 , 1,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_dy->lvu_uniEGS , 3,2 , 1,1);
                                                       
    // line 3-4, col 4-5                               
    widget_place_FF(grid,(GtkWidget*)dp->df_scaleLAB  , 2,3,  2,1);
    widget_place_FF(grid,(GtkWidget*)dp->df_scaleWid  , 2,4,  2,1);

    //GtkWidget* fill = gtk_label_new("fill");
    //widget_place_FF(grid,fill,0,5, 4,1);
}

/*======================================================================*/

extern void gui_OMSF_newFinish(Tdial_OMSF*df) { }

/*======================================================================*/
