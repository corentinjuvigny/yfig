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
 *      $Id: marker.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/marker.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: move / add / suppress object markers                   =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj.h"
#include "yfig-efsmSelObj+move.h"

#include "fig/fig-scene.h"
#include "tools/geo2d.h"

//#define FSC_MM_ALL_NoTex (FSC_MM_ALL&(~FSC_MM_TEX))

/*======================================================================*/
/*= Interface                                                          =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_mkrmov_sel(TefsmSel1ObjMv*fsm, int click, TobjAll obj, Tdble2 pos);
// This function is performed when the mouse pointer changes.
static void mp_mkrmov_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the zoom action.
static void mp_mkrmov_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);

static Twid_mousefun3S mp_mkrmov_mess[] = {
    { "Move point"   , "Horiz./Vert. move" , 0        }, // Begin
    { 0              , 0                   , 0        }, // End
    { "New position" , "New position"      , "Cancel" }, // NewPos
    { 0              , 0                   , 0 },
};

extern void mp_mkrmov_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1ObjMv*  fsm = yfig_efsmSel1ObjMv_new(cvs, FSC_MM_ALL, ymw,
            mp_mkrmov_sel,mp_mkrmov_newPos,mp_mkrmov_finish);
    fsm->fsm_mkrOpeMask = FIGO_MkrOpe_Chg;
    vgui_efsm_mousefun_enable(&fsm->any, ymw->ymw_mouseFun, mp_mkrmov_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    fsm->fsm_noCtrMkr = 1;
    *ret = &fsm->any;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_mkradd_sel(TefsmSel1ObjMv*fsm, int click, TobjAll obj, Tdble2 pos);
// This function is performed when the mouse pointer changes.
static void mp_mkradd_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the zoom action.
static void mp_mkradd_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);

static Twid_mousefun3S mp_mkradd_mess[] = {
    { "Break & add here"  , "Append a point"   , 0        }, // Begin
    { 0                   , 0                  , 0        }, // End
    { "Place new poinr"   , "Place new point"  , "Cancel" }, // NewPos
    { 0                   , 0                  , 0        },
};

extern void mp_mkradd_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1ObjMv*  fsm = yfig_efsmSel1ObjMv_new(cvs, FSC_MM_ALL, ymw,
            mp_mkradd_sel,mp_mkradd_newPos,mp_mkradd_finish);
    fsm->fsm_mkrOpeMask = FIGO_MkrOpe_Ins;
    vgui_efsm_mousefun_enable(&fsm->any, ymw->ymw_mouseFun, mp_mkradd_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    fsm->fsm_noCtrMkr = 1;
    *ret = &fsm->any;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_mkrdel_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos);
#if 0
// This function is performed when the mouse pointer changes.
static void mp_mkrdel_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the zoom action.
static void mp_mkrdel_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);
#endif

static Twid_mousefun3S mp_mkrdel_mess[] = {
    { "Delete point" , "Delete point"      , 0        }, // Begin
    { 0              , 0                   , 0        }, // End
    { 0              , 0                   , 0 },
    { 0              , 0                   , 0 },
};

extern void mp_mkrdel_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;
    TefsmSel1Obj*    fsm;
    
    fsm = yfig_efsmSel1Obj_new(cvs, FSC_MM_ALL, mp_mkrdel_sel,ymw);
    fsm->fsm_mkrOpeMask = FIGO_MkrOpe_Del;
    vgui_efsm_mousefun_enable(&fsm->any,ymw->ymw_mouseFun, mp_mkrdel_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    fsm->fsm_noCtrMkr = 1;
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_mkrmov_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;

    obj = obj_dir_remObjQ( obj );
    yfig_cursce_redraw( ymw );

    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);

    fsm->fsm_prevObj  = obj_newClone( obj );
    ydev_draw_obj(ydev,fsm->fsm_prevObj.any,OD_GC_Preview);
}

static void mp_mkrmov_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos)
{
    Tyfig_device*  fsod = (Tyfig_device*) fsm->fsm_outdevice;

    // erase preview and get a new preview object
    ydev_draw_obj(fsod,fsm->fsm_prevObj.any,OD_GC_Preview);

    // move object marker
    if ( fsm->fsm_selClick==2 ) {
        // horizontal/vertical move
        Tdble2 v = dble2_initVab(pos,fsm->fsm_selMark);
        if ( abs(v.x)>=abs(v.y) ) pos.y=fsm->fsm_selMark.y;
        else                      pos.x=fsm->fsm_selMark.x;
    }
    obj_markerChg(fsm->fsm_prevObj,fsm->fsm_selMarkIdx,pos);

#if 0
    char tmp1[100],tmp2[100];
    Tyfig_mw*  ymw     = (Tyfig_mw*)     fsm->fsm_userdata;
    CuserUnit* usrunit = &ymw->ymw_userUnit;
    double     srcppi  = fsod_getPpiSrc(fsod);
    char*      fmt     = fsm->fsm_selClick==1 
                       ? "center: %s %s ; zoom: %6.2f"
                       : "center: %s %s ; scale: %6.2f %6.2f";
    yfig_mw_usridt_printf(ymw, fmt,
            lu_LenPpiToUsrStrNU(usrunit,center.x,srcppi,tmp1),
            lu_LenPpiToUsrStr  (usrunit,center.y,srcppi,tmp2),
            srx,sry);
#endif

    // draw preview object
    ydev_draw_obj(fsod,fsm->fsm_prevObj.any,OD_GC_Preview);
}

static void mp_marker_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos,
        void (*newPos)(TefsmSel1ObjMv*,Tdble2))
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;

    if ( click!=1 && click!=2 ) {
        // change was canceled
        ydev_draw_obj(ydev,fsm->fsm_prevObj.any,OD_GC_Preview);
        obj_dir_delObjQ( fsm->fsm_prevObj );
        yfig_cursce_addObjNHis(ymw,fsm->fsm_selObj);
    } else {
        // change was accepted
        newPos(fsm,pos);
        ydev_draw_obj(ydev,fsm->fsm_prevObj.any,OD_GC_Preview);
        yfig_cursce_chgObjWHis(ymw,fsm->fsm_selObj,fsm->fsm_prevObj);
    }
    fsm->fsm_prevObj=OBJ_none; // otherwise the efsm draws it again
    yfig_cursce_redraw(ymw);
}

static void mp_mkrmov_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos)
{ mp_marker_finish(fsm,click,pos,mp_mkrmov_newPos); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_mkradd_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    
    obj = obj_dir_remObjQ( obj );
    yfig_cursce_redraw( ymw );

    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);

    fsm->fsm_prevObj  = obj_newClone( obj );
    if ( click==2 ) {
        fsm->fsm_selMarkIdx = obj_markerNb(fsm->fsm_prevObj);
        obj_markerIns(fsm->fsm_prevObj, fsm->fsm_selMarkIdx, NULL);
    } else
        obj_markerIns(fsm->fsm_prevObj, fsm->fsm_selMarkIdx, &fsm->fsm_selShpt);
    ydev_draw_obj(ydev,fsm->fsm_prevObj.any,OD_GC_Preview);
}

static void mp_mkradd_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos)
{
    Tyfig_device*  fsod = (Tyfig_device*) fsm->fsm_outdevice;

    // erase preview and get a new preview object
    ydev_draw_obj(fsod,fsm->fsm_prevObj.any,OD_GC_Preview);

    // move object marker
    obj_markerChg(fsm->fsm_prevObj,fsm->fsm_selMarkIdx,pos);

#if 0
    char tmp1[100],tmp2[100];
    Tyfig_mw*  ymw     = (Tyfig_mw*)     fsm->fsm_userdata;
    CuserUnit* usrunit = &ymw->ymw_userUnit;
    double     srcppi  = fsod_getPpiSrc(fsod);
    char*      fmt     = fsm->fsm_selClick==1 
                       ? "center: %s %s ; zoom: %6.2f"
                       : "center: %s %s ; scale: %6.2f %6.2f";
    yfig_mw_usridt_printf(ymw, fmt,
            lu_LenPpiToUsrStrNU(usrunit,center.x,srcppi,tmp1),
            lu_LenPpiToUsrStr  (usrunit,center.y,srcppi,tmp2),
            srx,sry);
#endif

    // draw preview object
    ydev_draw_obj(fsod,fsm->fsm_prevObj.any,OD_GC_Preview);
}

static void mp_mkradd_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos)
{ mp_marker_finish(fsm,click,pos,mp_mkradd_newPos); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_mkrdel_sel(TefsmSel1Obj*fsm, int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;

    if ( click!=1 && click!=2 ) return;

    TobjAll clone  = obj_newClone( obj );
    obj_markerDel(clone,fsm->fsm_selMarkIdx);

    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);

    yfig_cursce_chgObjWHis(ymw,fsm->fsm_selObj,clone);
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_mkrmov_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0xa0,0x02,
0x08,0x7e,0x00,0x00,0x00,0x10,0x66,0x00,0x20,0xe2,0x3f,
0x66,0x00,0x00,0x00,0x10,0x7e,0x00,0xa0,0x02,0x08,0x7e,
0x00,0x00,0x00,0x00,0x1c,0x00,0x80,0x02,0x00,0x1e,0x00,
0x00,0x00,0x00,0x1b,0x00,0x80,0x08,0x80,0x19,0x00,0x00,
0x00,0xc0,0x18,0x00,0x80,0x20,0x60,0x18,0x00,0x00,0x00,
0x30,0x18,0x00,0x80,0x80,0x18,0x18,0x00,0x00,0x00,0x0c,
0x18,0x00,0x80,0x00,0x06,0x18,0x00,0x00,0x00,0x03,0x18,
0x00,0x80,0x80,0x09,0x18,0x00,0x00,0xc0,0x00,0x18,0x00,
0x80,0x60,0x20,0x18,0x00,0x00,0x30,0x00,0x18,0x00,0x80,
0x18,0x80,0x18,0x00,0x00,0x0c,0x00,0x18,0x00,0x80,0x06,
0x00,0x1a,0x00,0x00,0x03,0x00,0x18,0x00,0x80,0x01,0x00,
0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
CUstr mp_mkrmov_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x1c,0x10,0x0e,
0x1c,0x3f,0x0e,0x3c,0x10,0x0e,0x00,0x08,0x05,0x88,0x80,0x04,
0x08,0x40,0x04,0x00,0x22,0x04,0x08,0x10,0x04,0x08,0x08,0x04,
0x00,0x08,0x04,0x08,0x04,0x04,0x08,0x22,0x04,0x00,0x01,0x04,
0x88,0x80,0x04,0x48,0x00,0x04,0x20,0x00,0x06,0x18,0x00,0x04,
0x00,0x00,0x00,0x00,0x00,0x00};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

CUstr mp_mkradd_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x1f,0x00,0x00,0x00,0x80,0x1f,
0x00,0x00,0x00,0x80,0x19,0x00,0x00,0x00,0x80,0x19,0x00,
0x00,0x00,0x80,0x1f,0x00,0x00,0x00,0xc0,0x3f,0x00,0x00,
0x00,0x60,0x60,0x00,0x00,0x00,0x30,0xc0,0x00,0x00,0x00,
0x18,0x80,0x01,0x00,0x00,0x0c,0x02,0x03,0x00,0x00,0x06,
0x07,0x06,0x00,0x00,0x83,0x0a,0x0c,0x00,0x80,0x01,0x02,
0x18,0x00,0xc0,0x00,0x02,0x30,0x00,0x60,0x00,0x02,0x60,
0x00,0x30,0x00,0x02,0xc0,0x00,0x18,0x00,0x02,0x80,0x01,
0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xa8,0xaa,0xaa,0xaa,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
CUstr mp_mkradd_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x00,0x00,0x0e,0x00,
0x00,0x0e,0x00,0x00,0x1f,0x00,0x00,0x11,0x00,0x00,0x11,0x00,
0x80,0x20,0x00,0x80,0x20,0x00,0x40,0x44,0x00,0x40,0x4e,0x00,
0x20,0x95,0x00,0x20,0x84,0x00,0x10,0x04,0x01,0x10,0x04,0x01,
0x08,0x04,0x02,0x08,0x00,0x02,0x04,0x00,0x04,0x54,0x55,0x05,
0x00,0x00,0x00,0x00,0x00,0x00};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

CUstr mp_mkrdel_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xc0,0x01,0x00,0x00,0x00,0x40,0x01,0x00,
0x00,0x00,0xc0,0x01,0x00,0x00,0x80,0xc7,0x00,0x00,0x00,0x00,
0xa9,0x00,0x00,0x00,0x00,0xb2,0x00,0x00,0x00,0x00,0xa4,0x00,
0x00,0x00,0x00,0xc8,0x00,0x00,0x00,0x10,0x98,0x00,0x00,0x00,
0x70,0x28,0x01,0x00,0x00,0x90,0x45,0x02,0x00,0x00,0x30,0x86,
0x04,0x00,0x00,0xc0,0x9a,0x09,0x00,0x00,0x00,0xe3,0x12,0x00,
0x00,0x00,0x8d,0x25,0x00,0x00,0x00,0xb1,0x4e,0x00,0x00,0x80,
0xc0,0x58,0x38,0x00,0x80,0x80,0xe3,0x7c,0x00,0x40,0x80,0xfe,
0xcf,0x00,0x20,0x80,0x80,0x9d,0x00,0x20,0x80,0xc0,0xf3,0x00,
0x10,0x80,0xc0,0x66,0x00,0x10,0x80,0xc0,0x0c,0x00,0x00,0x80,
0x80,0x09,0x00,0x00,0x80,0x00,0x0b,0x00,0x00,0x80,0x00,0x0e,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,};
CUstr mp_mkrdel_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x70,0x00,0x00,
0xe0,0x00,0x00,0xd8,0x01,0x07,0x98,0x83,0x0f,0x20,0x87,0x08,
0xac,0xff,0x08,0x58,0xff,0x0f,0x40,0x30,0x07,0x88,0x70,0x00,
0x88,0xd0,0x00,0x08,0x91,0x01,0x08,0x11,0x03,0x08,0x62,0x03,
0x08,0xc2,0x01,0x08,0x04,0x00,0x08,0x04,0x00,0x08,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/
