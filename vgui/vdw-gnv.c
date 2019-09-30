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
 *      $Id: vdw-gnv.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-gnv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= GNV implementation (Get Numeric Value dialog).                     =*/
/*= See the wid-simple.h header for documentation.                     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-simple.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/

typedef struct _Tvdw_gnv {
    Tvgui_dw      any;
    Twid_spinner* spin;
} Tvdw_gnv;

/*======================================================================*/

static void vdw_gnv_cb_validate(Twid_spinner* spin, void* ud, double v)
{
    Tvdw_gnv* self = ud;
    DBG1_DwE("gnv=%p spin=%p v=%f",self,spin,v);
    TEMP_asf_printf(self->spin!=spin,"unexpected ud or spin",0);

    DBG1_DwL("gnv=%p spin=%p v=%f",self,spin,v);
}

/*======================================================================*/

extern Twid_dialStatus vdw_gnv_start(Tvgui_mw*vmw, CstrBrief title,
                                     Trange* r)
{
    Tvgui*    gui  = vgui_mw_getVGui(vmw);
    Tvdw_gnv* self = (Tvdw_gnv*)vgui_dw_newTop( sizeof(*self),
            vmw, tool_strdup(title),
            WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok);
    DBG1_DwE("gnv=%p title=%s v=%f",self,title,r->val);

    self->spin = wid_spinner_newRange(gui, r,
            vdw_gnv_cb_validate, 0, self);
    gui_spinner_dbf(self->spin,gui,self->dw_widEGS);

    vgui_dw_finishIC(&self->any,0,self->spin->self,-1);
    gui_spinner_setNum(self->spin,r->val);

    int status;
    status = vgui_dw_run(&self->any);
    if ( status==WID_DIALSTATUS_Ok ) {
        r->val = self->spin->range.val;
    }

    wid_spinner_free( self->spin );
    vgui_dw_free(&self->any);

    DBG1_DwL("gnv=%p title=%s v=%f : return %08x (%s)",
            self,title,r->val,status,
            status==WID_DIALSTATUS_Ok ? "Ok" : 
            status==WID_DIALSTATUS_Cancel ? "Cancel" : "???"
    );
    return status;
}

/*======================================================================*/
