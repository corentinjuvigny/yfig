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
 *      $Id: edit.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/edit.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: This mode selects an object and then starts a dialog   =*/
/*= that edits it.                                                     =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj.h"
#include "vgui/vgui-dw.h"
#include "ydw/ydw-objedit.h"

#include "fig/fig-scene.h"

/*======================================================================*/
/*= Interface                                                          =*/

// This function is performed when an object is selected.
static void mp_edit_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker);

static Twid_mousefun3S mp_edit_mess[] = {
//{ "Edit object" , "Edit main comment"  , "Edit point" }, // BEGIN
    { "Edit object" , "Edit object"  , 0 },       // BEGIN
    { 0             , 0              , 0 },       // END
    { 0             , 0              , 0 },       // OBJSEL
    { 0             , 0              , 0 },       // OBJNEW
};

extern void mp_edit_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj*  fsm = yfig_efsmSel1Obj_new(cvs,
            FSC_MM_ALL,mp_edit_objSel,ymw);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_edit_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Zoom);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

// This function is performed when an object is selected.
static void mp_edit_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyoed_any*     dialog;

    int kind = obj_Kind( obj );
    switch ( kind ) {
        case FIG_KD_Arc     : dialog = yoed_arc_nbf    (ymw, obj); break;
        case FIG_KD_Boxhv   : dialog = yoed_boxhv_nbf  (ymw, obj); break;
        case FIG_KD_Ellipse : dialog = yoed_ellipse_nbf(ymw, obj); break;
        case FIG_KD_Line    :
        case FIG_KD_Spline  : dialog = yoed_line_nbf   (ymw, obj); break;
        case FIG_KD_Text    : dialog = yoed_text_nbf   (ymw, obj); break;
        case FIG_KD_Compound:
            PERM_spe_printf("edit of %d object kind not yet implemented",kind);
            return;
        default:
            TEMP_spe_printf("unrecognized object kind %d",kind);
            return;
    }
    
    Toutdev* od = fsod_getOD( YMW_ydev(ymw) );
    yfig_cursce_redraw(ymw);    // FIXME bug in TefsmSel1Obj
    od_ds_xorDrawStart_0( od );
    od_ds_xorDrawCommit_0( od );

    // start dialog
    int status = vgui_dw_run((Tvgui_dw*)dialog);

    // treat the dialog status
    int          redraw = yoed_any_restoreInitialFig(dialog);
    Tyoed_state* state  = &dialog->dw_state;
    if ( status!=WID_DIALSTATUS_Ok ||  OBJ_IsNone(state->onew) ) {
        wid_box_delete( WIDBOX(dialog) );
        if ( redraw ) yfig_cursce_redraw( ymw );
        DBG_YOES(state,0);
    } else /* WID_DIALSTATUS_Ok  && OBJ_IsDefined(state.chg) */ {
        TobjAll nobj = obj_newLazy(state->onew); // wid_box_delete delete it
        DBG_YOES(state,1);
        wid_box_delete( WIDBOX(dialog) );
        yfig_cursce_chgObjWHis(ymw,obj,nobj); // nobj is eaten
        yfig_cursce_redraw( ymw );
        DBG_YOES(state,2);
        yfig_layers_updateFull(ymw);
    }
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_edit_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa7,
0xaa,0xff,0x00,0x60,0x0c,0x80,0x80,0x00,0x20,0x18,0x80,
0x8e,0x00,0x30,0x30,0x80,0x80,0x00,0x10,0x20,0x80,0x9e,
0x00,0x30,0x20,0x80,0x80,0x00,0x60,0x30,0x80,0x86,0x00,
0xc0,0x18,0x80,0x80,0x00,0x80,0x0f,0x80,0xbe,0x00,0x00,
0x20,0x80,0x80,0x00,0x00,0x80,0x80,0xbe,0x00,0x00,0x00,
0x82,0x80,0x00,0x00,0x00,0x88,0x80,0x00,0x00,0x00,0xa0,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x1f,0x04,0x00,0x00,0x00,0x01,0x44,0x04,0x00,
0x00,0x01,0x04,0x04,0x00,0x00,0x81,0x65,0x0e,0x00,0x00,
0x4f,0x46,0x04,0x00,0x00,0x41,0x44,0x04,0x00,0x00,0x41,
0x44,0x04,0x00,0x00,0x41,0x46,0x14,0x00,0x00,0x9f,0xe5,
0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

CUstr mp_edit_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0x0f,0x80,0x10,0x08,
0x38,0x50,0x0b,0x28,0x10,0x08,0x64,0xd0,0x0b,0x4c,0x10,0x08,
0x68,0xd0,0x08,0x38,0x10,0x08,0x80,0xd0,0x08,0x00,0x14,0x08,
0x00,0xf0,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xa8,0x00,
0x20,0x88,0x01,0x60,0xae,0x00,0x20,0xaa,0x00,0xe0,0x2e,0x01,
0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/
