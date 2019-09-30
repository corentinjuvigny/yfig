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
 *      $Id: vdw-console.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-console.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* A dialog implementing an output console                              */
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vdw-console.h"
#include "vgui/vgui-dw.h"
#include "vgui/vgui.h"

#include "tools/messagedriver.h"
#include "tools/globalvars.h"

/*======================================================================*/
/*= data structure                                                     =*/

struct _Tvdw_console {
    Tvgui_dw       any;
    Twid_mlout*    console;
    Tmsgdrv        msgdrv;
    Tvdw_console** pToRef;  // reference to set to NULL when
                            // widget is deleted.
    Tmsgdrv**      pToRef2; // reference to the msgdrv list that
                            // may contain &self->msgdrv.
                            // Must be suppressed of list when
                            // widget is deleted.
} ;

static void vdw_console_usrmsg_print(Cmsgdrv*drv,CstrBrief msg);
static void vdw_console_errmsg_print(Cmsgdrv*drv,CstrBrief msg,int lvl);
static void vdw_console_cb( Tvgui_dw*_d, Twid_dialStatus s, void* ud);

/*======================================================================*/
/*= Interface                                                          =*/

extern Tvdw_console* vdw_console_nbf(Tvgui_mw* vmw, Tvdw_console** pref,
                                                    Tmsgdrv**      pref2)
{
    Tvdw_console* self = (Tvdw_console*) vgui_dw_newTop(
            sizeof(*self), vmw, strdup( "message console" ),0);
    self->pToRef = pref;
    self->pToRef2= pref2;

    Tvgui_dw* wd = &self->any;

    self->console = widD_mlout_nbf(wd,40,10,0);
    vgui_dw_addCtlButt(wd, WID_DIALSTATUS_CancelX,"Dismiss");
    vgui_dw_addCtlButt(wd, WID_DIALSTATUS_New,    "Clear");
    vgui_dw_finishIC(&self->any, VDWLF_EB,self->console->widEGS, -1);

    wd->top->dt_cb = vdw_console_cb;

  //self->msgdrv.usridt       = 0;
    self->msgdrv.usrmsg       = 1;
  //self->msgdrv.addapp       = 0;
    self->msgdrv.addlvl       = 1;
    self->msgdrv.errlvl       = gl_msgdrv_default.errlvl;
    self->msgdrv.usrmsg_print = vdw_console_usrmsg_print;
    self->msgdrv.errmsg_print = vdw_console_errmsg_print;
  //self->msgdrv.exit         = 0;
    self->msgdrv.userdata.ptr = self;
  //self->msgdrv.next         = 0;

    vgui_dw_start(wd);

    return self;
}

extern void vdw_console_delete(Tvdw_console*self)
{
    Tmsgdrv* p;
    if ( self->pToRef!=0 ) *self->pToRef = 0;
    if ( self->pToRef2!=0 ) {
        if ( *self->pToRef2==&self->msgdrv )
            *self->pToRef2 = self->msgdrv.next;
        else
            for (p=*self->pToRef2 ; p->next!=0 ; p=p->next) {
                if ( p->next==&self->msgdrv ) {
                    p->next = self->msgdrv.next;
                    break;
                }
            }
    }
    wid_mlout_free( self->console );
    vgui_dw_free( &self->any );
}

/*= == == == == == == == == == == ==  == == == == == == == == == == == =*/

extern Tmsgdrv* vdw_console_getMsgDrv(Tvdw_console*self) { return &self->msgdrv; }

/*======================================================================*/
/*= Local utilities                                                    =*/

static void vdw_console_usrmsg_print(Cmsgdrv*drv,CstrBrief msg)
{
    Tvdw_console* self = drv->userdata.ptr;
    Twid_mlout* mlout  = self->console;
    wid_mlout_printf(mlout,"%s\n",msg);
}

static void vdw_console_errmsg_print(Cmsgdrv*drv,CstrBrief msg,int lvl)
{
    Tvdw_console* self = drv->userdata.ptr;
    Twid_mlout* mlout  = self->console;
    switch (lvl) {
      case  3: wid_mlout_printf   (mlout,"%s\n",msg); break;
      case  2: wid_mlout_printfWrn(mlout,"%s\n",msg); break;
      default: wid_mlout_printfErr(mlout,"%s\n",msg); break;
    }
}

static void vdw_console_cb(
        Tvgui_dw*_d, Twid_dialStatus s, void* ud)
{
    Tvdw_console* self  = (Tvdw_console*)_d;
    Twid_mlout*   mlout = self->console;
    switch (s) {
        case WID_DIALSTATUS_Cancel: // dismiss
            vdw_console_delete(self);
            return;
        case WID_DIALSTATUS_New:    // clear
            wid_mlout_setText(mlout,"");
            return;
        default:
            PERM_spe_printf("%d status not recognized.",s);
    }
}

/*======================================================================*/
