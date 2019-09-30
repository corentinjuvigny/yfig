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
 *      $Id: yfigqt-mw.cc 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/qt/yfigqt-mw.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* EGS driver for building and handling the top YFIG widget.           =*/
/*======================================================================*/

#include "yfigqt.h"
#include "guiqt/guiqt-create.h"

#include "yfig-mw.h"
#include "vgui/wid-menu.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/

extern Tegs_widget gui_yfig_create   (Tvgui*    gui)
{
    Tgui_main*  main = gui->g_main;
    Tegs_widget form;
    form = create_form(gui, main->m_topForm, wn_mainTop,
            10, 0,-1,-1, EGSCOLOR_default,EGSCOLOR_default); 
    return form;
}

extern void gui_yfig_createend(Tyfig_mw* ymw)
{
    QGridLayout *grid      = (QGridLayout*)ymw->ymw_widEGS;
    QWidget     *menuBarre = (QWidget    *)ymw->ymw_menuBar->mb_widEGS;
    QWidget     *message   = (QWidget    *)ymw->ymw_message;
    QWidget     *mousefun  = (QWidget    *)ymw->ymw_mouseFun->mf_widEGS;
    QWidget     *modePanel = (QWidget    *)ymw->ymw_modePanel->bp_widEGSext;
    QWidget     *canvas    = (QWidget    *)ymw->ymw_canvas->vgcm_widEGS;
    QWidget     *update    = (QWidget    *)vgui_dw_getWidEGS(ymw->ymw_update);
    QWidget     *dpPanel   = (QWidget    *)ymw->ymw_dpPanel->bp_widEGSext;
    QWidget     *layers    = (QWidget    *)vgui_dw_getWidEGS(ymw->ymw_layers);

#if 1 // FIXME:REMOVE
    // BBBFF
    // GGGFF
    // MMCCL
    // MMCCL
    // UPPPP

    widget_place_EF(grid, menuBarre, 0,0 , 1,3);
    widget_place_EF(grid, message,   1,0 , 1,3);
    widget_place_FF(grid, mousefun,  0,3 , 2,2);
    widget_place_FE(grid, modePanel, 2,0 , 2,2);
    widget_place_EE(grid, canvas,    2,2 , 2,2);
    //grid->setColumnStretch(3,100);
    widget_place_FE(grid, layers,    2,4 , 2,1);
    widget_place_FF(grid, update,    4,0 , 1,1);
    widget_place_EF(grid, dpPanel,   4,1 , 1,4);
    grid->setColumnStretch(2,100);
    grid->setRowStretch(2,100);
#else
    // BBBFF
    // GGGFF
    // MMCCL
    // MMCCL
    // MMCCL
    // UPPPP

    widget_place_EF(grid, menuBarre, 0,0 , 1,3);
    widget_place_EF(grid, message,   1,0 , 1,3);
    widget_place_FF(grid, mousefun,  0,3 , 2,2);
    widget_place_FE(grid, modePanel, 2,0 , 3,2);
    widget_place_EE(grid, canvas,    2,2 , 3,2);
    grid->setColumnStretch(2,100);
    //grid->setColumnStretch(3,100);
    widget_place_FE(grid, layers,    2,4 , 3,1);
    widget_place_FF(grid, update,    5,0 , 1,1);
    widget_place_EF(grid, dpPanel,   5,1 , 1,4);
#endif
}

extern void gui_yfig_delete     (Tyfig_mw* ymw)
{
    QGridLayout *grid  = (QGridLayout*)ymw->ymw_widEGS;
    ymw->ymw_widEGS = 0;
    delete grid;
}

extern void        gui_yfig_setTitle(Cyfig_mw* ymw, TstrBrief t)
{
    Tvgui*      gui  = ymw->ymw_gui;
    Tgui_main*  main = gui->g_main;
    main->m_topForm->setWindowTitle(t);
}

/*======================================================================*/
