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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-dw.h 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-dw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= We attach some XT private data to the Tvgui_dw                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_DW_H
#define FILE_GUIXT_DW_H
/*======================================================================*/

#include "vgui/vgui-dw.h"

/*======================================================================*/

typedef struct _TGUI_dialog {
    Tvgui_dw*        dialog;
    Widget           shell;
    Txt_dw_cb_resize cbResize;
    int              nbVisbilyNotify;
    Widget           widgets      [20];
    Widget           widgetsBellow[20];  // widgets[i} is above  widgetsBellow[i]
    Tvgui_dwMtxFlag  flags        [20];
    int              lengths      [20];
    int              widgetNb;  // size of widgets and flags
} TGUI_dialog;

/*======================================================================*/
#endif // FILE_GUIXT_DW_H
/*======================================================================*/
