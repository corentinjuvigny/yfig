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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vdw-console.h 127 2018-04-14 10:30:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vdw-console.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* A dialog implementing an output console                             =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VDW_CONSOLE_H
#define FILE_VDW_CONSOLE_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================*/
/*= function                                                           =*/

extern Tvdw_console* vdw_console_nbf     (Tvgui_mw* vds,
                                          Tvdw_console** pref,
                                          Tmsgdrv**      pref2);
extern void          vdw_console_delete   (Tvdw_console*self);
extern Tmsgdrv*      vdw_console_getMsgDrv(Tvdw_console*self);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VDW_CONSOLE_H
/*======================================================================*/

