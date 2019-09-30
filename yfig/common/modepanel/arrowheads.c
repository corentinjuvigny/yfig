/*======================================================================
 *  Copyright 2018 Corentin Juvigny
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
 *$Software: YFIG-MP (Mode Panel)
 *      $Id: arrowheads.c 307 2018-07-17 09:55:20Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/arrowheads.c $
 *  $Author: Corentin Juvigny (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Mode panel FSM for creation and modification of text object.       =*/
/*======================================================================*/

#include "modepanel.h"
#include "vgui/vgui.h"
#include "yfig-efsmSelObj.h"

/*======================================================================*/
/*= Interface                                                          =*/
static Twid_mousefun3S mp_arrowheads_mess[];
static void            mp_arrowheads_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos);

extern void mp_arrowheads_cb(Tvgui_mw* vmv, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmv;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj* fsm = yfig_efsmSel1Obj_new(cvs, FSC_MM_ALL,
            mp_arrowheads_sel, ymw);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_arrowheads_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static Twid_mousefun3S mp_arrowheads_mess[] = {
    { "Add arrow"    , "Remove arrow"    , 0        }, // Start
    { 0              , 0                 , 0        }, // End
    { 0              , 0                 , 0        }, 
    { 0              , 0                 , 0        }, 
};


static void mp_arrowheads_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw* ymw = (Tyfig_mw*)fsm->fsm_userdata;
    //FIXME : any submit after the first will trigger the modification of the end arrow
    TarrowDesc* ad = !fsm->fsm_selMarkIdx ? &ymw->ymw_arrowbeg :
                                            &ymw->ymw_arrowend;
    int save = ad->enabled;
    ad->enabled = click == 1 ? 1 : 0;
    TobjAll clone = obj_newClone( obj );
    if ( !fsm->fsm_selMarkIdx ) {
        yfig_cursce_chgObjArrowBeg(ymw,clone); 
        yfig_cursce_chgObjWHis(ymw,obj,clone);
    }
    else {
        yfig_cursce_chgObjArrowEnd(ymw,clone); 
        yfig_cursce_chgObjWHis(ymw,obj,clone);
    }
    yfig_cursce_redraw(ymw);
    ad->enabled = save;
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_arrowheads_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,
0x00,0x0a,0x00,0x00,0x0a,0x00,0x00,0x11,0x00,0x00,0x11,0x00,
0x80,0x20,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,
0xc0,0x7f,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,
0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

CUstr mp_arrowheads_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x02,
0x00,0x00,0x04,0x00,0x02,0x00,0x00,0x0e,0x08,0x87,0x00,0x00,
0x0a,0x10,0x45,0x00,0x00,0x0a,0x10,0x45,0x00,0x00,0x11,0xa0,
0x28,0x00,0x00,0x11,0xa0,0x28,0x00,0x00,0x11,0xc0,0x18,0x00,
0x80,0x20,0xc0,0x18,0x00,0x80,0x20,0xc0,0x18,0x00,0x80,0x20,
0x40,0x15,0x00,0x40,0x40,0x20,0x22,0x00,0x40,0x40,0x20,0x25,
0x00,0x40,0x40,0x20,0x25,0x00,0xe0,0xff,0xf0,0x7f,0x00,0x00,
0x04,0x40,0x12,0x00,0x00,0x04,0x40,0x12,0x00,0x00,0x04,0x20,
0x22,0x00,0x00,0x04,0x20,0x22,0x00,0x00,0x04,0x10,0x42,0x00,
0x00,0x04,0x10,0x42,0x00,0x00,0x04,0x00,0x02,0x00,0x00,0x04,
0x00,0x02,0x00,0x00,0x04,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00};
