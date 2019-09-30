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
 *$Software: YFIG-MP (Mode Panel)
 *      $Id: align.c 251 2018-06-25 11:00:05Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/align.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: This mode aligns objects within a compound or canvas.  =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj.h"
//#include "vgui/vgui-dw.h"

//#include "fig/fig-scene.h"

/*======================================================================*/
/*= Interface                                                          =*/

// This function is performed when an object is selected.
static void mp_align_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker);

static Twid_mousefun3S mp_align_mess[] = {
//{ "Edit object" , "Edit main comment"  , "Edit point" }, // BEGIN
    { "Align compound" , "Align canvas" , 0 },       // BEGIN
    { 0                , 0              , 0 },       // END
    { 0                , 0              , 0 },       // OBJSEL
    { 0                , 0              , 0 },       // OBJNEW
};

extern void mp_align_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj*  fsm = yfig_efsmSel1Obj_new(cvs,
            FSC_MM_DIR,mp_align_objSel,ymw);

    vgui_efsm_mousefun_enable(&fsm->any,ymw->ymw_mouseFun, mp_align_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Zoom|DPP_SHOW_Align);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

// This function is performed when an object is selected.
static void mp_align_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;

    TobjAll clone = obj_newClone(obj);
    obj_alignD(clone.dir,yfig_mw_getAlignOpe(ymw));
    yfig_cursce_chgObjWHis(ymw,obj,clone);
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_align_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0xc0,0x60,
0x00,0x00,0x00,0x20,0x80,0x00,0x00,0x00,0x20,0x80,0x00,0x00,
0x00,0x10,0x00,0x01,0x00,0x01,0x10,0x00,0x01,0x00,0xc1,0x13,
0x00,0x01,0x80,0x42,0x12,0x00,0x01,0x80,0x42,0x12,0x00,0x01,
0x40,0x44,0x22,0x80,0x00,0x40,0x44,0x22,0x80,0x00,0x20,0x48,
0xc2,0x60,0x00,0xe0,0xcf,0x03,0x1f,0x00,0x54,0x55,0x55,0x55,
0x01,0x00,0x00,0x00,0x00,0x00,0x80,0x31,0x00,0x00,0x00,0x40,
0x22,0x02,0x00,0x00,0x20,0x24,0x00,0x00,0x00,0x20,0x24,0x63,
0x35,0x00,0x20,0x24,0x92,0x4c,0x00,0xe0,0x27,0x92,0x44,0x00,
0x20,0x24,0x62,0x44,0x00,0x20,0x24,0x12,0x44,0x00,0x20,0x74,
0xe7,0x44,0x00,0x00,0x00,0x10,0x01,0x00,0x00,0x00,0xe0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,};

CUstr mp_align_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8f,0x03,0x10,0x49,0x04,
0x10,0x29,0x08,0x28,0x29,0x08,0x28,0x29,0x08,0x44,0x49,0x04,
0x7c,0x8f,0x03,0xa8,0xaa,0x0a,0x00,0x00,0x00,0x98,0x00,0x00,
0xa4,0x00,0x00,0xa4,0x02,0x00,0xa4,0x30,0x07,0xbc,0x4a,0x09,
0xa4,0x4a,0x09,0xa4,0x72,0x09,0xa4,0x42,0x09,0x00,0x38,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/
