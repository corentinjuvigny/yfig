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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-update.h 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/ydw/ydw-update.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top-dialog that control the update mode.                           =*/
/*= It is not really a popup dialog, it is shown and unshow in the     =*/
/*= YFIG main window when the update mode is enables/disables.         =*/ 
/*=                                                                    =*/
/*= It also handles the enable/disable indicators of the DDP buttons.  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YDW_YDW_UPDATE_H
#define FILE_YDW_YDW_UPDATE_H
/*======================================================================*/

#include "yfig-config.h"

/*======================================================================*/

extern Tvgui_dw*  ydw_update_nbfTop(Tyfig_mw* ymw);
extern void       ydw_update_delete(Tyfig_mw* ymw, Tvgui_dw* vdw);

extern void       ydw_update_enable (Cyfig_mw* ymw);
extern void       ydw_update_disable(Cyfig_mw* ymw);
extern TstyleMask ydw_update_getMask(Cyfig_mw* ymw);

/*======================================================================*/
#endif // FILE_YDW_YDW_UPDATE_H
/*======================================================================*/
