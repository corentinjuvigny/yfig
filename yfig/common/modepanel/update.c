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
 *      $Id: update.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/update.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: This mode selects an object and then                   =*/
/*=   - either sets its drawing parameters to the values of the DPP    =*/
/*=     (Drawing Parameter Panel).                                     =*/
/*=   - or updates the values of the DDP from its drawing parameters.  =*/
/*=                                                                    =*/
/*= The parameters that are changed in both directions are controled   =*/
/*= by a flag called upfl (UPdate FLag).                               =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj.h"
#include "yfig-mw.h"
#include "ydw/ydw-update.h"

#include "fig/obj-any.h"
#include "fig/object-priv.h"
#include "fig/fig-scene.h"

//#define FSC_MM_update (FSC_MM_ALL&(~FSC_MM_BHV)
#define FSC_MM_update (FSC_MM_ALL)

/*======================================================================*/
/*= Interface                                                          =*/

// This function is performed when an object is selected.
static void mp_update_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker);

static Twid_mousefun3S mp_update_mess[] = {
    { "Update object" , "Update setting"  , 0 }, // BEGIN
    { 0               , 0                 , 0 }, // END
    { 0               , 0                 , 0 }, // OBJSEL
    { 0               , 0                 , 0 }, // OBJNEW
};

extern void mp_update_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj*  fsm = yfig_efsmSel1Obj_new(cvs,
            FSC_MM_update,mp_update_objSel,ymw);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_update_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Update    | DPP_SHOW_Depth
                         |DPP_SHOW_Angle     | DPP_SHOW_LineFillWA
                         |DPP_SHOW_Text);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

// select the arrow operations that must be done
static TstyleMask mp_update_cmpMaskArrow(TstyleMask m, Tyfig_mw* ymw, TobjAll obj)
{
    TstyleMask ret=0;
    m &= FDP_A_all; 
    if ( (m&FDP_A_all)==0 )            return 0;
    if ( !obj_arrow_IsSupported(obj) ) return 0;

    if ( (m&FDP_A_mode)!=0 ) {
        if ( ((ymw->ymw_arrowbeg.enabled!=0) == (obj_arrow_Beg(obj)!=0)) &&
             ((ymw->ymw_arrowend.enabled!=0) == (obj_arrow_End(obj)!=0)) ) {
            m &= ~FDP_A_mode;
        }
    }
    return m;
}

// perform the the arrow operations selected in m
// assuming obj has arrow support.
static void mp_update_objArrow(TstyleMask m, Tyfig_mw* ymw, TobjAll obj)
{
#define ENABLED_IN_STY(beg,end)  ymw->ymw_arrowbeg.enabled==beg &&  ymw->ymw_arrowend.enabled==end
#define ENABLED_IN_OBJ(beg,end) (obj_arrow_Beg(obj)!=0)==beg && (obj_arrow_End(obj)!=0)==end
    if ( (m&FDP_A_mode)==0 ) return;
    if ( ENABLED_IN_STY(0,0) ) {
        obj_arrow_delete(obj,0);
        obj_arrow_delete(obj,1);
    } else if ( ENABLED_IN_STY(0,1) ) {
        if ( ENABLED_IN_OBJ(0,0) ) 
            yfig_cursce_chgObjArrowEnd(ymw,obj);
        else if ( ENABLED_IN_OBJ(1,0) ) 
            obj_arrow_permute(obj);
        else if ( ENABLED_IN_OBJ(1,1) ) 
            obj_arrow_delete(obj,1);
    } else if ( ENABLED_IN_STY(1,0) ) {
        if ( ENABLED_IN_OBJ(0,0) ) 
            yfig_cursce_chgObjArrowBeg(ymw,obj);
        else if ( ENABLED_IN_OBJ(0,1) ) 
            obj_arrow_permute(obj);
        else if ( ENABLED_IN_OBJ(1,1) ) 
            obj_arrow_delete(obj,0);
    } else if ( ENABLED_IN_STY(1,1) ) {
        if ( ENABLED_IN_OBJ(0,0) ) {
            yfig_cursce_chgObjArrowBegEnd(ymw,obj);
        } else if ( ENABLED_IN_OBJ(0,1) )  {
            yfig_cursce_chgObjArrowBeg(ymw,obj);
        } else if ( ENABLED_IN_OBJ(1,0) )  {
            yfig_cursce_chgObjArrowEnd(ymw,obj);
        }
    }
}

// The mp_update_objSel function is performed when an object is selected.
// The mp_update_obj_do function updates a single object that is not a directoty.
static TobjAll mp_update_obj_do(TefsmSel1Obj*fsm, TobjAll obj);

static void mp_update_objSel(TefsmSel1Obj*fsm, int click,
                           TobjAll obj, Tdble2 marker)
{
//    Tyfig_device*  ydev   = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw    = (Tyfig_mw*)     fsm->fsm_userdata;
//   Tfdp        dp_obj,  *dp_curr, dp_res;
//   TstyleMask mask_obj, mask_curr, status;

    if ( click==2 ) {
        // setting update from object
        if ( obj_Kind(obj)==FIG_KD_Compound ) {
            yfig_mw_errerr_printf(ymw,"%s",
                "compound objects can not been used for setting update parameters.");
            return;
        }
        // update the drawing parameters staying in the ymw->ymw_currDP
        // structure with the values found in the obj object.
        Tfdp       dp_obj, *dp_curr, dp_res;
        TstyleMask mask_curr, mask_obj;
        mask_curr = ydw_update_getMask(ymw);
        dp_curr   = &ymw->ymw_currDP;
        mask_obj = figdp_getFdpRaw(&dp_obj,obj); // must not be freed (see doc)
        dp_res = figdp_merge1(mask_curr,dp_curr, mask_obj,&dp_obj);
        figdp_free(dp_curr);
        *dp_curr = dp_res;
        yfig_dppanel_update(ymw);
    } else if ( click==1 && obj_Kind(obj)!=FIG_KD_Compound ) {
        TobjAll newobj = mp_update_obj_do(fsm,obj);
        if ( OBJ_IsDefined(newobj) ) {
            yfig_cursce_chgObjWHis(ymw,obj,newobj);
            yfig_cursce_redraw_update( ymw );
            yfig_mw_inferr_printf(ymw,"%s object updated.",obj_Name(obj));
        } else {
            yfig_mw_errerr_printf(ymw,"%s object not updated.",obj_Name(obj));
        }
    } else if ( click==1 && obj_Kind(obj)==FIG_KD_Compound ) {
        // update all sub-object from current setting
        TobjAll     newobj  = obj_newClone(obj);
        TobjAllSet* objs    = obj_getChildrenD(newobj,YMW_scene(ymw)->sc_depths.table);
        if ( objs->eleNb==0 ) {
            yfig_mw_errerr_printf(ymw,"%s does not contain any updatable object.",obj_Name(obj));
        } else {
            int depthSave     = ymw->ymw_currDP.fdp_depth;
            int depthCompound = obj.any->OA_depthmin;
            int depthDiff     = depthSave-depthCompound;
            int i,found=0;
            for (i=0; i<objs->eleNb ; i+=1) {
                TobjAll oldobj = objs->eles[i];
                ymw->ymw_currDP.fdp_depth = oldobj.any->OA_depthmin + depthDiff;
                if ( ymw->ymw_currDP.fdp_depth< 0             ) ymw->ymw_currDP.fdp_depth=0;
                if ( ymw->ymw_currDP.fdp_depth>=FIG_MAX_DEPTH ) ymw->ymw_currDP.fdp_depth=FIG_MAX_DEPTH-1;
                TobjAll updobj = mp_update_obj_do(fsm,oldobj);
                if ( OBJ_IsDefined(updobj) ) {
                    found += 1;
                    obj_dir_addObj(obj_ObjAll(oldobj.any->OA_parent),updobj);
                    obj_dir_delObjQ(oldobj);
                }
            }
            if ( found!=0 ) {
                yfig_cursce_chgObjWHis(ymw,obj,newobj);
                yfig_cursce_redraw_update( ymw );
                yfig_mw_inferr_printf(ymw,"%d object%s updated.",found,found>1?"s":"");
            } else {
                obj_delete( newobj );
                yfig_mw_errerr_printf(ymw,"no object updated.");
            }
            ymw->ymw_currDP.fdp_depth = depthSave;
        }
        objAllSet_delete(objs);
    }
}

// returns a clone of obj that is updated.
// if no update is done return OBJ_none.
static TobjAll mp_update_obj_do(TefsmSel1Obj*fsm, TobjAll obj)
{
    TobjAll        newobj;
    Tyfig_device*  fsod   = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw    = (Tyfig_mw*)     fsm->fsm_userdata;
    Tfdp       dp_obj,  *dp_curr, dp_res;
    TstyleMask mask_obj, mask_curr, status;

    mask_curr = ydw_update_getMask(ymw);
    dp_curr   = &ymw->ymw_currDP;

    mask_obj = figdp_getFdpRaw(&dp_obj,obj); // must not be freed (see doc)

    // update the drawing parameters of the obj object
    // with the values found in the ydm->ydm_currDP structure
    dp_res = figdp_merge2(mask_curr,dp_curr, mask_obj,&dp_obj);
    status = figdp_cmpMask(mask_obj,&dp_res,&dp_obj);
    status |= mp_update_cmpMaskArrow(mask_curr,ymw,obj);
    if ( status==0 ) {
        newobj = OBJ_none;
    } else {
        if ( (status&(~FDP_A_all))!=0 ) {
            dp_res.fdp_text.dpt_angle = DegToRad( dp_res.fdp_text.dpt_angle );
            newobj = obj_newChangingDP(obj,&dp_res,(Toutdev*)fsod);
        } else {
            newobj = obj_newClone(obj); 
        }
        if ( (status&FDP_A_all)!=0 )
            mp_update_objArrow(status, ymw, newobj);
//          yfig_cursce_chgObjWHis(ymw,obj,newobj);
//          yfig_cursce_redraw_update( ymw );
    }
    figdp_free(&dp_res);
    return newobj;
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_update_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xc0,0x07,0xff,0xff,0x01,0x60,0x0d,0x01,0x00,
0x01,0xb0,0x1a,0x01,0x3e,0x01,0x50,0x35,0x3d,0x2a,0x01,0xb0,
0x2a,0x01,0x36,0x01,0x50,0x35,0x7d,0x2a,0x01,0xb0,0x2a,0x01,
0x3e,0x01,0x60,0x35,0x01,0x00,0x01,0xc0,0x1f,0xff,0xff,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x80,0x03,
0x80,0x03,0x00,0x40,0x05,0x40,0x05,0x00,0x00,0x01,0x00,0x01,
0x00,0x00,0xff,0xff,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x48,
0x00,0x02,0x02,0x00,0x48,0x00,0x02,0x02,0x00,0x48,0x8d,0x32,
0xc7,0x00,0x48,0x53,0x43,0x22,0x01,0x48,0x51,0x7a,0xe2,0x01,
0x48,0x53,0x4a,0x22,0x00,0x48,0x4d,0x4b,0x2a,0x01,0x30,0x81,
0x72,0xc4,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,};

CUstr mp_update_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x00,0x00,
0x48,0x00,0x00,0x48,0x00,0x00,0x48,0x07,0x00,0x48,0xe5,0x00,
0x78,0x07,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
0x40,0x00,0x00,0x40,0x23,0x00,0x70,0x64,0x02,0x48,0x27,0x05,
0x48,0x25,0x03,0x70,0x4b,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};


/*======================================================================*/
