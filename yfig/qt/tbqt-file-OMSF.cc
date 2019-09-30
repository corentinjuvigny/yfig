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
 *$Software: YFIG (YFIG driver to Qt)
 *      $Id: tbqt-file-OMSF.cc 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/qt/tbqt-file-OMSF.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widget for open/merge/save-as YFIG files (EGS driver).      =*/
/*======================================================================*/

#include "yfigqt.h"
#include "ydw/tb-file-OMSF.h"
#include "../../guiqt/widqt-filebrowser.h"

#include "vgui/wid-lvu.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-button.h"

/*======================================================================*/
/*=Tools for control buttons                                           =*/
/*======================================================================*/

/*======================================================================*/

extern void gui_OMSFprev_dbf(Tdial_OMSFprev* dp)
{
    //QGridLayout* grid = (QGridLayout*) dp->dw_widEGS;
    Tvgui_dw* self = &dp->any;
    QGridLayout* grid = VGUI_D_isTopDial(self)
                      ? ((TGUI_dialog*)self->D_widEGS)->grid
                      : ((TGUI_subdialog*)self->D_widEGS)->grid;

    // line 1, all the columns
    widget_place_FF(grid,(QWidget*)dp->df_titleLAB   ,0,0);
    widget_place_EF(grid,(QWidget*)dp->df_fileNameLAB,0,1,1,3);
    
    // line 2, column 1
    widget_place_FF(grid, (QWidget*)dp->df_objnb->lvu_labEGS,1,0);
    widget_place_FF(grid, (QWidget*)dp->df_objnb->lvu_valEGS,1,1);
    widget_place_FF(grid, (QWidget*)dp->df_size->lvu_labEGS, 2,0);
    widget_place_FF(grid, (QWidget*)dp->df_size->lvu_valEGS, 2,1);
    widget_place_FF(grid, (QWidget*)dp->df_size->lvu_uniEGS,  2,2);
    widget_place_FF(grid, (QWidget*)dp->df_commTitleLAB, 3,0);
    widget_place_EE(grid, (QWidget*)dp->df_commValueTOU->widEGS, 3,1, 1,2);

    // line 2, column 2
    widget_place_EE(grid, (QWidget*)dp->df_canvas->vgcm_widEGS, 1,3,4,1);
}

/*======================================================================*/

extern void        gui_OMSFpara_dbf(Tdial_OMSFpara* dp)
{
    //QGridLayout* grid = (QGridLayout*) dp->dw_widEGS;
    Tvgui_dw* self = &dp->any;
    QGridLayout* grid = VGUI_D_isTopDial(self)
                      ? ((TGUI_dialog*)self->D_widEGS)->grid
                      : ((TGUI_subdialog*)self->D_widEGS)->grid;

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

    // line 1, 
    widget_place_FF(grid,(QWidget*)dp->df_xLAB      , 0,0 , 1,1);
    widget_place_FF(grid,(QWidget*)dp->df_xWestWid  , 0,1 , 1,1);
    widget_place_FF(grid,(QWidget*)dp->df_xEastWid  , 0,2 , 1,1);
                                                       
    // line 2,                                         
    grid->addWidget((QWidget*)dp->df_yLAB      , 1,0 , 1,1);
    grid->addWidget((QWidget*)dp->df_yWestWid  , 1,1 , 1,1);
    grid->addWidget((QWidget*)dp->df_yEastWid  , 1,2 , 1,1);
                                                       
    // line 3,                                         
    grid->addWidget((QWidget*)dp->df_dx->lvu_labEGS , 2,0 , 1,1);
    grid->addWidget((QWidget*)dp->df_dx->lvu_valEGS , 2,1 , 1,1);
    grid->addWidget((QWidget*)dp->df_dx->lvu_uniEGS , 2,2 , 1,1);
                                                       
    // line 4,                                         
    grid->addWidget((QWidget*)dp->df_dy->lvu_labEGS , 3,0 , 1,1);
    grid->addWidget((QWidget*)dp->df_dy->lvu_valEGS , 3,1 , 1,1);
    grid->addWidget((QWidget*)dp->df_dy->lvu_uniEGS , 3,2 , 1,1);
                                                       
    // line 3-4, col 4-5                               
    grid->addWidget((QWidget*)dp->df_scaleLAB  , 2,3,  2,1);
    grid->addWidget((QWidget*)dp->df_scaleWid  , 2,4,  2,1);

    QLabel* fill = new QLabel();
    fill->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grid->addWidget(fill,                        0,5, 4,1);
}

/*======================================================================*/

extern void gui_OMSF_newFinish(Tdial_OMSF*df)
{
//{ QLayout* g = ((QWidget*)self->D_widEGS)->layout();
//  //g->setSizeConstraint(QLayout::SetDefaultConstraint);
//  g->setSizeConstraint(QLayout::SetMinimumSize); }
}

/*======================================================================*/
