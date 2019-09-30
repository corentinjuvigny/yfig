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
 *      $Id: measurelength.c 307 2018-07-17 09:55:20Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/measurelength.c $
 *  $Author: Corentin Juvigny (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Mode panel FSM for creation and modification of text object.       =*/
/*======================================================================*/

#include "modepanel.h"
#include "vgui/vgui.h"
#include "yfig-efsmSelObj.h"
#include "fig/object.h"
#include "fig/obj-any.h"
#include "fig/obj-vtable.h"

/*======================================================================*/
/*= Interface                                                          =*/

static Twid_mousefun3S mp_measurelength_mess[];
static void mp_measurelength_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos);

extern void mp_measurelength_cb(Tvgui_mw* vmv, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmv;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj* fsm = yfig_efsmSel1Obj_new(cvs, FSC_MM_ALL,
            mp_measurelength_sel, ymw);

    vgui_efsm_mousefun_enable(&fsm->any, ymw->ymw_mouseFun, mp_measurelength_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static Twid_mousefun3S mp_measurelength_mess[] = {
    { "Select object", "Select and Add"  , 0           }, // Start
    { 0              , 0                 , 0           }, // End
    { 0              , 0                 , 0           }, 
    { 0              , 0                 , 0           }, 
};

static void mp_measurelength_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw* ymw       = (Tyfig_mw*)fsm->fsm_userdata;
    Tyfig_device* fsod  = (Tyfig_device*)fsm->fsm_outdevice;
    VARS_SeVt(obj);

    switch ( self->OA_vtable->vt_kind ) {
        case FIG_KD_Compound:
        case FIG_KD_Boxhv:
        case FIG_KD_Text:
            yfig_mw_usridt_printf(ymw,
                                  "Length undefined on this object");
            return;
        default:
            break;
    }
    double length       = obj_getLength(obj);
    CuserUnit* usrunit  = &ymw->ymw_userUnit;
    double srcppi       = fsod_getPpiSrc(fsod);
    char tmp[100];

    yfig_mw_usridt_printf(ymw,
                          "Length = %s",
                          lu_LenPpiToUsrStr(usrunit, length, srcppi, tmp));
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_measurelength_small[] = {
0x00,0x00,0xc0,0x00,0x00,0xc0,0x00,0x00,0xc0,0xf0,0x00,0xc0,0x8c,0xc0,0xcc,
0x84,0xb0,0xc8,0x82,0x10,0xc9,0xc2,0x10,0xc9,0x60,0x08,0xc9,0x18,0x0c,0xcf,
0x08,0x02,0xc0,0xf0,0x01,0xc0,0x00,0x00,0xc0,0x00,0x00,0xc0,0x00,0x00,0xc0,
0x00,0x00,0xc0,0x24,0x49,0xd2,0xfc,0xff,0xdf,0x00,0x00,0xc0,0x00,0x00,0xc0,
0x00,0x00,0xc0,0x00,0x00,0xc0};

CUstr mp_measurelength_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x80,0x13,0x00,0x40,
0x00,0x70,0x10,0x80,0xc1,0x00,0x18,0x10,0xe0,0x83,0x01,0x08,
0x10,0x30,0x06,0x01,0x0c,0x10,0x10,0x0c,0x01,0x04,0x18,0x18,
0x08,0x01,0x04,0x08,0x08,0x0c,0x01,0x04,0x04,0x08,0x04,0x01,
0x00,0x03,0x0c,0x84,0x01,0xc0,0x00,0x06,0xdc,0x00,0x60,0x00,
0x03,0x70,0x00,0x60,0xc0,0x00,0x00,0x00,0xc0,0x3f,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x21,0x84,0x10,0x02,
0x08,0x21,0x84,0x10,0x02,0x08,0x21,0x84,0x10,0x02,0xf8,0xff,
0xff,0xff,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00};
