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
 *      $Id: vgui-print.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-print.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Implementation of the functions that write into the gui console    =*/
/*=                                                                    =*/
/*= See the vgui.h header for the type and function documentation.     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vdw-console.h"

#include "tools/globalvars.h"
#include "tools/messagedriver.h"

/*======================================================================*/

extern void vgui_msgdrv_addConsoleEL(Tvgui*gui, int errlvl)
{
    if ( gui->g_console!=0 ) {
        PERM_spe_printf("%s","console already exist (call is ignored).");
        return;
    }
    gui->g_console = vdw_console_nbf( gui->g_vmw, &gui->g_console, &gui->g_msgdrv);
    Tmsgdrv* md    = vdw_console_getMsgDrv(gui->g_console);
    gui->g_msgdrv->next = md;
    md->errlvl = errlvl;
}
extern void vgui_msgdrv_addConsole(Tvgui*gui) { vgui_msgdrv_addConsoleEL(gui,gl_msgdrv_default.errlvl); }

extern void vgui_msgdrv_delConsole(Tvgui*gui)
{
    if ( gui->g_console==0 )
        return;
    //gui->g_msgdrv->next = NULL;
    vdw_console_delete( gui->g_console );
}

/*======================================================================*/

extern Tmsgdrv* vgui_getMsgDrv(Cvgui*gui) { return gui->g_msgdrv; }

/*======================================================================*/
