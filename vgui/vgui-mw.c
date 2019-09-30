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
 *      $Id: vgui-mw.c 127 2018-04-14 10:30:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-mw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Main widgets implementation.                                       =*/
/*= See vgui-mw.h header for data structure and documentation.         =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-mw.h"
#include "vgui/vgui.h"
#include "vgui/wid-simple.h"

#include "tools/messagedriver.h"

/*======================================================================*/
/*= abstract accessors                                                 =*/

extern Tvgui*      vgui_mw_getVGui   (Cvgui_mw*vmw) { return vmw->MW_gui; }
extern Tegs_widget vgui_mw_getWidEGS (Cvgui_mw*vmw) { return vmw->MW_widEGS; }
extern Tmsgdrv*    vgui_mw_getMsgDrv(Cvgui_mw*vmw)  { return vmw->MW_msgdrv!=0 ? vmw->MW_msgdrv : vmw->MW_gui->g_msgdrv; }
extern Tvgui_mw*   vgui_mw_newDummy  (Tvgui*   gui)
{
    Tvgui_mw* self = tool_malloc_0( sizeof(*self) );
    self->gui = gui;
    return self;
}
extern void vgui_mw_delDummy(Tvgui_mw* mw) { tool_free( mw ); }

/*======================================================================*/
/*= Interface for printing messages                                    =*/     

static void vgui_mw_msgdrv_usrmsg_print(Cmsgdrv*drv, CstrBrief msg)
{
    Tvgui_mw* vmw = drv->userdata.ptr;
    wid_slout_setText(vmw->MW_message,msg);
}

static void vgui_mw_msgdrv_errmsg_print(Cmsgdrv*drv, CstrBrief msg, int lvl)
{
    Tvgui_mw* vmw = drv->userdata.ptr;
    switch (lvl) {
        case  3: wid_slout_setText   (vmw->MW_message,msg); break;
        case  2: wid_slout_setTextWrn(vmw->MW_message,msg); break;
        default: wid_slout_setTextErr(vmw->MW_message,msg); break;
    }
}

extern void vgui_mw_msgdrv_initWidLvl(Tvgui_mw* vmw, Tegs_widget msgwid, int lvl)
{
    Tmsgdrv* md = vmw->MW_msgdrv;
    if ( md==0 )
        vmw->MW_msgdrv = md = tool_malloc_0(sizeof(*md));
    md->usridt       = 1;
    md->usrmsg       = 1;
  //md->addapp       = 0;
    md->addlvl       = 1;
    md->errlvl       = lvl;
    md->usrmsg_print = vgui_mw_msgdrv_usrmsg_print;
    md->errmsg_print = vgui_mw_msgdrv_errmsg_print;
  //md->exit         = 0;
    md->userdata.ptr = vmw;
    md->next         = vmw->MW_gui->g_msgdrv;
    
    vmw->MW_message = msgwid;
}

extern void vgui_mw_msgdrv_free(Tvgui_mw* vmw)
{
    tool_free( vmw->MW_msgdrv );
    vmw->MW_msgdrv = 0;
}

/*======================================================================*/
