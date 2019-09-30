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
 *      $Id: zmcd.c 307 2018-07-17 09:55:20Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/zmcd.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: zmcd --> Zoom, Move, Copy, Delete objects              =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj+move.h"
#include "yfig-efsmSelNObj.h"

#include "fig/fig-scene.h"
#include "tools/geo2d.h"

/*======================================================================*/
/*= Interface                                                          =*/
/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_zoom_sel(TefsmSel1ObjMv*fsm, int click, TobjAll obj, Tdble2 pos);
// This function is performed when the mouse pointer changes.
static void mp_zoom_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the zoom action.
static void mp_zoom_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);

static Twid_mousefun3S mp_zoom_mess[] = {
    { "Zoom object"  , "Scale object" , 0        }, // Begin
    { 0              , 0              , 0        }, // End
    { "New size"     , "New size"     , "Cancel" }, // NewPos
    { 0              , 0              , 0 },
};

extern void mp_zoom_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1ObjMv*  fsm = yfig_efsmSel1ObjMv_new(cvs, FSC_MM_ALL, ymw,
            mp_zoom_sel,mp_zoom_newPos,mp_zoom_finish);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_zoom_mess);
    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    fsm->fsm_noCtrMkr = 1;
    *ret = &fsm->any;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_move_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos);
// This function is performed when the mouse pointer changes.
static void mp_move_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the move action.
static void mp_move_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);

static Twid_mousefun3S mp_move_mess[] = {
    { "Move object"  , "Hor/Vertical move" , 0        }, // Begin
    { 0              , 0                   , 0        }, // End
    { "Place object" , "Place object"      , "Cancel" }, // NewPos
    { 0              , 0                   , 0 },
};

extern void mp_move_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1ObjMv*  fsm = yfig_efsmSel1ObjMv_new(cvs, FSC_MM_ALL, ymw,
            mp_move_sel,mp_move_newPos,mp_move_finish);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_move_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    *ret = &fsm->any;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_copy_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos);
// This function is performed when the mouse pointer changes.
static void mp_copy_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos);
// This function is performed when the user finishes the copy action.
static void mp_copy_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos);

static Twid_mousefun3S mp_copy_mess[] = {
    { "Copy object"  , "Hor/Vertical copy" , "Copy to cut buffer" },
    { 0              , 0                   , 0        }, // End
    { "Place object" , "Place object"      , "Cancel" }, // NewPos
    { 0              , 0                   , 0 },
};

extern void mp_copy_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1ObjMv*  fsm = yfig_efsmSel1ObjMv_new(cvs, FSC_MM_ALL, ymw,
            mp_copy_sel,mp_copy_newPos,mp_copy_finish);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_copy_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    *ret = &fsm->any;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
// This function is performed when an object is selected.
static void mp_delete_sel1(TefsmSelNObj*fsm, TobjAll obj);
static void mp_delete_selN(TefsmSelNObj*fsm, CobjAllSet* obj);

static Twid_mousefun3S mp_delete_mess[] = {
    { "Delete object" , "Clear area"   , 0        },
    { 0               , 0              , 0        }, // End
    { "Final corner"  , "Final corner" , "Cancel" }, // EFSMST_REG1
    { "Final corner"  , "Final corner" , "Cancel" }, // EFSMST_REG2
    { 0               , 0              , 0        },
};

extern void mp_delete_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSelNObj*  fsm = yfig_efsmSelNObj_new(cvs, FSC_MM_ALL, ymw,
            mp_delete_sel1,mp_delete_selN);

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_delete_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Std);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
#include "fig/obj-text.h"
#include "outdev/string-hl.h"

static void draw_previewObject(TefsmSel1ObjMv*fsm)
{
    Tyfig_device* fsod = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*     ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tod_GCdef     gcd  = OD_GC_Preview;
    if ( obj_Kind(fsm->fsm_prevObj)==FIG_KD_Text ) {
      od_style_pushTop(fsod_getOD(fsod),gcd);
        od_str_setGC(fsod_getOD(fsod),gcd,
              fsm->fsm_prevObj.text->OTE_font,
              fsm->fsm_prevObj.text->OTE_pts,
              NULL);
        ydev_draw_obj(fsod,fsm->fsm_prevObj.any,gcd);
      od_style_pop(fsod_getOD(fsod),gcd);
    } else
        ydev_draw_obj(fsod,fsm->fsm_prevObj.any,gcd);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_zoom_usrmsg(TefsmSel1ObjMv*fsm, Tdble2 center, double fx, double fy)
{
    Tyfig_mw*      ymw     = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  fsod    = (Tyfig_device*) fsm->fsm_outdevice;
    Tdble2*        tv      = &fsm->fsm_prevTv; 
    CuserUnit*     usrunit = &ymw->ymw_userUnit;
    double         srcppi  = fsod_getPpiSrc(fsod);
    char tmp1[100],tmp2[100];
    char*      fmt     = fsm->fsm_selClick==1 
                       ? "center: %s %s ; zoom: %6.2f"
                       : "center: %s %s ; scale: %6.2f %6.2f";
    yfig_mw_usridt_printf(ymw, fmt,
            lu_LenPpiToUsrStrNU(usrunit,center.x,srcppi,tmp1),
            lu_LenPpiToUsrStr  (usrunit,center.y,srcppi,tmp2),
            fx,fy);
}

static void mp_zoom_sel(TefsmSel1ObjMv*fsm, int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    
    if ( click!=1 ) {
        if ( obj_Kind(obj)==FIG_KD_Text ) {
            yfig_mw_errerr_printf(ymw,
                "no uniform scale is not available for %s objects",
                obj_Name(obj));
            fsm->fsm_selAbort = 1;
            return ;
        }
    }

    obj = obj_dir_remObjQ( obj );
    yfig_cursce_redraw( ymw );
    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
    fsm->fsm_prevObj  = obj_newClone( obj );
    draw_previewObject( fsm );
    mp_zoom_usrmsg(fsm,obj_center(obj),1,1);
}

Inline double fixe_zoom(double zoom)
{
#define ZMIN  0.05
#define ZMAX 50.00
    if ( zoom<0 ) {
        if ( zoom < -ZMAX ) return -ZMAX;
        if ( zoom > -ZMIN ) return -ZMIN;
    } else {
        if ( zoom > +ZMAX ) return +ZMAX;
        if ( zoom < +ZMIN ) return +ZMIN;
    }
    return zoom;
}

static void mp_zoom_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos)
{
    Tyfig_device* fsod = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*     ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    int wtcf = FIGO_SA_AllDim;   // What To Change (Flag) when scaling
    if ( ymw_drawopt_scaleText(ymw)  )             wtcf |= FIGO_SA_Text;
    if ( ymw_drawopt_scaleStyle(ymw) )             wtcf |= FIGO_SA_Thickness;
    if ( ymw_drawopt_scaleStyle(ymw) )             wtcf |= FIGO_SA_Arrow;
    if ( obj_Kind(fsm->fsm_prevObj)==FIG_KD_Text ) wtcf |= FIGO_SA_Text;

    // erase preview and get a new preview object
    draw_previewObject(fsm);
    obj_dir_delObjQ( fsm->fsm_prevObj );
    fsm->fsm_prevObj = obj_newClone( fsm->fsm_selObj );

    double srx,sry;
    Tdble2 center = obj_center( fsm->fsm_prevObj );
    if ( fsm->fsm_selClick==1 ) {
        // compute the zoom ratio
        srx=sry = tool_projPtToLineRa(pos,center,fsm->fsm_selShpt);
    } else {
        // compute the scale ratios
#if 1
        srx = (pos.x-center.x)/(fsm->fsm_selShpt.x-center.x);
        sry = (pos.y-center.y)/(fsm->fsm_selShpt.y-center.y);
#else
        srx = (pos.x-center.x)/(fsm->fsm_selMark.x-center.x);
        sry = (pos.y-center.y)/(fsm->fsm_selMark.y-center.y);
#endif
    }
    mp_zoom_usrmsg(fsm,center,srx,sry);

    // scale the preview object and draw it
    srx=fixe_zoom(srx);
    sry=fixe_zoom(sry);
    obj_scaleCC(fsm->fsm_prevObj,wtcf,srx,sry,center.x,center.y);
    draw_previewObject(fsm);
}

static void mp_zm_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos,
        void (*newPos)(TefsmSel1ObjMv*,Tdble2))
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;

    if ( click!=1 && click!=2 ) {
        // zoom canceled
        draw_previewObject( fsm );
        obj_dir_delObjQ( fsm->fsm_prevObj );
        yfig_cursce_addObjNHis(ymw,fsm->fsm_selObj);
    } else {
        // zoom accepted
        newPos(fsm,pos);
        draw_previewObject(fsm);
        yfig_cursce_chgObjWHis(ymw,fsm->fsm_selObj,fsm->fsm_prevObj);
    }
    fsm->fsm_prevObj=OBJ_none; // otherwise the efsm draws it again
    yfig_cursce_redraw(ymw);
}

static void mp_zoom_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos)
{ mp_zm_finish(fsm,click,pos,mp_zoom_newPos); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
static void mp_move_usrmsg(TefsmSel1ObjMv*fsm, Tdble2 pos)
{
    Tyfig_mw*      ymw     = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  fsod    = (Tyfig_device*) fsm->fsm_outdevice;
    Tdble2*        tv      = &fsm->fsm_prevTv; 
    CuserUnit*     usrunit = &ymw->ymw_userUnit;
    double         srcppi  = fsod_getPpiSrc(fsod);

    char tmp1[100],tmp2[100],tmp3[100],tmp4[100];
    yfig_mw_usridt_printf(ymw, 
            "position: %s %s ; trans: %s %s",
            lu_LenPpiToUsrStrNU(usrunit,pos.x,srcppi,tmp1),
            lu_LenPpiToUsrStr  (usrunit,pos.y,srcppi,tmp2),
            lu_LenPpiToUsrStrNU(usrunit,tv->x,srcppi,tmp3),
            lu_LenPpiToUsrStr  (usrunit,tv->y,srcppi,tmp4));
}

static void mp_move_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    
    obj = obj_dir_remObjQ( obj );
    yfig_cursce_redraw( ymw );

    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);

    fsm->fsm_prevObj  = obj_newClone( obj );
    fsm->fsm_prevTv   = dble2_init(0,0);
    draw_previewObject( fsm );
    mp_move_usrmsg(fsm,pos);
}

static void mp_move_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos)
{
    Tyfig_device*  fsod = (Tyfig_device*) fsm->fsm_outdevice;
    Tdble2*        tv   = &fsm->fsm_prevTv; 

    // erase preview and reset the previous translation
    draw_previewObject( fsm );
    obj_translation(fsm->fsm_prevObj,-tv->x,-tv->y);

    // compute translation vector
    *tv = pos;
    tv->x -= fsm->fsm_selShpt.x ;
    tv->y -= fsm->fsm_selShpt.y;
    if ( fsm->fsm_selClick==2 ) {
        if ( abs(tv->x)>=abs(tv->y) ) tv->y=0;
        else                          tv->x=0;
    }
    mp_move_usrmsg(fsm,pos);

    // translate the preview object and draw it
    obj_translation(fsm->fsm_prevObj,tv->x,tv->y);
    draw_previewObject( fsm );
}

static void mp_move_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos)
{ mp_zm_finish(fsm,click,pos,mp_move_newPos); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_copy_sel(TefsmSel1ObjMv*fsm,
                        int click, TobjAll obj, Tdble2 pos)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    
    if ( click==3 ) {
        yfig_mw_errerr_printf(ymw,"%s","copy to cut buffer not yet implemented");
            fsm->fsm_selAbort = 1;
            return ;
    }

    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);

    fsm->fsm_prevObj  = obj_newClone( obj );
    fsm->fsm_prevTv   = dble2_init(0,0);
    draw_previewObject( fsm );
    mp_move_usrmsg(fsm,pos);
}

static void mp_copy_newPos(TefsmSel1ObjMv*fsm, Tdble2 pos)
{ mp_move_newPos(fsm,pos); }

static void mp_copy_finish(TefsmSel1ObjMv*fsm, int click, Tdble2 pos)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;

    if ( click!=1 && click!=2 ) {
        // cancel copy
        draw_previewObject( fsm );
        yfig_cursce_supObjNHis(ymw, fsm->fsm_prevObj );
    } else {
        // copy accepted
        mp_copy_newPos(fsm,pos);
        draw_previewObject( fsm );
        yfig_cursce_addObjWHis(ymw, fsm->fsm_prevObj );
    }
    fsm->fsm_prevObj=OBJ_none; // otherwise the efsm draws it again
    yfig_cursce_redraw( ymw );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void mp_delete_sel1(TefsmSelNObj*fsm, TobjAll obj)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    yfig_cursce_supObjWHis(ymw, obj);
    yfig_cursce_redraw_objDeleted( ymw, obj );
}

static void mp_delete_selN(TefsmSelNObj*fsm, CobjAllSet* set)
{
    Tyfig_mw* ymw  = (Tyfig_mw*)fsm->fsm_userdata;
    yfig_cursce_supObjSetWHis(ymw,set->eles,set->eleNb);
    yfig_cursce_redraw_update( ymw );
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_zoom_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x04,0x00,0x00,0x00,0x00,0x0a,0x00,0x40,0x20,0x04,
0x0a,0x00,0x40,0x10,0x08,0x11,0x00,0xa0,0xf8,0x1f,0x11,
0x00,0xa0,0x10,0x88,0x20,0x00,0x10,0x21,0x84,0x20,0x00,
0xf0,0x01,0x40,0x40,0x00,0x00,0x00,0x40,0x40,0x00,0x00,
0x00,0x20,0x80,0x00,0x00,0x00,0x20,0x80,0x00,0x00,0x00,
0x10,0x00,0x01,0x00,0x00,0xf0,0xff,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xe0,0x00,0xc0,0x00,0x00,0x10,0x01,0x80,0x00,0x00,
0x10,0x00,0x80,0x00,0x00,0x10,0x38,0x8e,0x38,0x00,0xe0,
0x44,0x90,0x44,0x00,0x00,0x05,0x9e,0x7c,0x00,0x00,0x05,
0x91,0x04,0x00,0x10,0x45,0x99,0x44,0x00,0xe0,0x38,0xd6,
0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
CUstr mp_zoom_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x01,0x10,0x11,0x01,
0x90,0xbf,0x02,0x28,0x91,0x02,0x28,0x4a,0x04,0x7c,0x40,0x04,
0x00,0xe0,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x80,0x00,
0x24,0x80,0x00,0x04,0x80,0x00,0x08,0x9b,0x04,0xb0,0xa0,0x0a,
0xa0,0xb8,0x0e,0xa4,0xa8,0x02,0x18,0xb3,0x0c,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

CUstr mp_move_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x02,0x00,0x1c,0x00,0x20,0x00,
0x00,0x3e,0x00,0x10,0x04,0x00,0x63,0x00,0x00,0x00,0x80,
0x61,0x00,0x10,0x04,0x80,0x61,0x00,0x00,0x08,0x80,0x61,
0x00,0x10,0x00,0x88,0xc1,0x00,0x00,0x08,0x90,0xc1,0x00,
0x10,0xc0,0xbf,0xc1,0x00,0x00,0x08,0x90,0xc1,0x00,0x10,
0x00,0x88,0xc1,0x00,0x00,0x04,0x80,0x61,0x00,0x20,0x00,
0x00,0x63,0x00,0x00,0x04,0x00,0x66,0x00,0x80,0x02,0x00,
0x7c,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x04,0x00,0x00,0x00,
0xc0,0x06,0x00,0x00,0x00,0xc0,0xc6,0x89,0x1c,0x00,0x40,
0x25,0x8a,0x22,0x00,0x40,0x25,0x8a,0x3e,0x00,0x40,0x24,
0x52,0x02,0x00,0x40,0x24,0x52,0x22,0x00,0x40,0xc4,0x21,
0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
CUstr mp_move_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x80,0x03,0x40,0x80,0x0e,
0x88,0xc0,0x08,0x00,0x48,0x08,0x84,0x5e,0x08,0x00,0x48,0x08,
0x84,0xc0,0x08,0x08,0x80,0x09,0xa0,0x00,0x0f,0x40,0x00,0x06,
0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x6c,0x00,0x00,
0x54,0x52,0x03,0x44,0x55,0x05,0x44,0x55,0x03,0x44,0x22,0x06,
0x00,0x00,0x00,0x00,0x00,0x00};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

CUstr mp_copy_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x03,
0x00,0x1c,0x00,0xc0,0x07,0x00,0x3e,0x00,0x60,0x0c,0x00,0x63,
0x00,0x30,0x0c,0x80,0x61,0x00,0x30,0x0c,0x80,0x61,0x00,0x30,
0x0c,0x80,0x61,0x00,0x30,0x18,0x88,0xc1,0x00,0x30,0x18,0x90,
0xc1,0x00,0x30,0xd8,0xbf,0xc1,0x00,0x30,0x18,0x90,0xc1,0x00,
0x30,0x18,0x88,0xc1,0x00,0x30,0x0c,0x80,0x61,0x00,0x60,0x0c,
0x00,0x63,0x00,0xc0,0x0c,0x00,0x66,0x00,0x80,0x0f,0x00,0x7c,
0x00,0x00,0x07,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
0x03,0x00,0x00,0x00,0x40,0x04,0x00,0x00,0x00,0x40,0x00,0x00,
0x00,0x00,0x40,0xe0,0x34,0x11,0x00,0x40,0x10,0x4d,0x11,0x00,
0x40,0x10,0x45,0x11,0x00,0x40,0x14,0x4d,0x19,0x00,0x80,0xe3,
0x34,0x16,0x00,0x00,0x00,0x04,0x10,0x00,0x00,0x00,0x04,0x0f,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,};
CUstr mp_copy_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x80,0x03,0xe8,0x80,0x0e,
0x8c,0xc0,0x08,0x84,0x48,0x08,0x84,0x5e,0x08,0x84,0x48,0x08,
0x8c,0xc0,0x08,0x98,0x80,0x09,0xf0,0x00,0x0f,0x60,0x00,0x06,
0x00,0x00,0x00,0x38,0x00,0x00,0x44,0x00,0x00,0x04,0x32,0x05,
0x04,0x55,0x05,0x44,0x35,0x07,0x38,0x12,0x04,0x00,0x10,0x03,
0x00,0x00,0x00,0x00,0x00,0x00};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

CUstr mp_delete_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x07,0x00,0x00,0x00,0xc0,
0x0f,0x00,0x00,0x00,0xe0,0x1f,0x00,0x00,0x00,0x30,0x33,
0x00,0x00,0x00,0x30,0x33,0x00,0x00,0x00,0xe0,0x1f,0x00,
0x00,0x00,0xc0,0x0f,0x00,0x00,0x00,0x80,0x07,0x00,0x00,
0x00,0x84,0x87,0x00,0x00,0x00,0x84,0x87,0x01,0x00,0x00,
0x08,0x43,0x00,0x00,0x00,0x70,0x38,0x00,0x00,0x00,0x80,
0x07,0x00,0x00,0x00,0xe0,0x1f,0x01,0x00,0x00,0x3c,0xf0,
0x01,0x00,0x00,0x04,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0xc0,0x00,0x02,0x00,
0x48,0x80,0x00,0x07,0x00,0x48,0x8e,0x38,0xc2,0x01,0x48,
0x91,0x44,0x22,0x02,0x48,0x9f,0x7c,0xe2,0x03,0x48,0x81,
0x04,0x22,0x00,0x48,0x91,0x44,0x2a,0x02,0x3c,0xce,0x39,
0xc4,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
CUstr mp_delete_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x80,0x3f,0x00,
0x80,0x3f,0x00,0xc0,0x64,0x00,0xc0,0x7f,0x00,0x80,0x3f,0x00,
0x00,0x0e,0x00,0x00,0x0a,0x00,0x10,0x00,0x02,0xf8,0xe0,0x07,
0x00,0x1f,0x00,0xf8,0xe0,0x07,0x10,0x00,0x02,0x00,0x00,0x00,
0x46,0x92,0x08,0xaa,0xaa,0x15,0x6a,0x9a,0x0c,0xc6,0xb2,0x18,
0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/
