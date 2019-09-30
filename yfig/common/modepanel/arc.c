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
 *      $Id: arc.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/arc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Mode panel FSM for creating arc curves.                            =*/
/*======================================================================*/

#include "modepanel.h"
#include "vgui/vgui-efsm-points.h"
#include "fig/obj-arc.h"
#include "tools/geo2d.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void mp_arc_create(Tvgui_efsm* a);
static void mp_arc_preview(Tvgui_efsmPTS*a, int click1, Cdble2* pts, int ptNb);

static Twid_mousefun3S mp_arcopen_mess[] = {
    { "First point"   , 0               , 0 },       // BEGIN
    { "Final point"   , "Final point"   , "Cancel"}, // END
    { "Next  point"   , "Next point"    , "Cancel"}, // NewPos
    { "Final point"   , "Final point"   , "Cancel"}, // TmpPos
    { 0               , 0               , 0 },       // HandFree
};

extern void mp_arcopen_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**   ret  = (Tvgui_efsm**)ud;

    Tvgui_efsmPTS* fsm = vgui_efsmPTS_new_Nexact(
            3, ymw->ymw_canvas,
            mp_arc_create, mp_arc_preview, ymw);
    fsm->any.magnet = 1;
    fsm->iflag1 = FIGOAR_T_Open;
    fsm->skeletonMode     = 1;
    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_arcopen_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_StdLineFillWA);
    *ret = &fsm->any;
}

#define mp_arcpop_mess mp_arcopen_mess

extern void mp_arcpop_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*      ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**   ret  = (Tvgui_efsm**)ud;

    Tvgui_efsmPTS* fsm = vgui_efsmPTS_new_Nexact(
            3, ymw->ymw_canvas,
            mp_arc_create, mp_arc_preview, ymw);
    fsm->any.magnet = 1;
    fsm->iflag1 = FIGOAR_T_PiePiece;
    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_arcpop_mess);
    fsm->skeletonMode     = 1;

    yfig_dpp_showonly(ymw,DPP_SHOW_StdLineFillNA);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= Private utilities                                                  =*/
#include "fig/obj-compound.h"

static void  mp_arc_create(Tvgui_efsm* a)
{
    int i;
    Tvgui_efsmPTS* fsm = (Tvgui_efsmPTS*)a;
    Tyfig_device*  dev = (Tyfig_device*) a->outdevice;
    Tyfig_mw*      ymw = (Tyfig_mw*)     a->userdata;

    TobjAll obj = obj_arc_newA(&ymw->ymw_currDP,
        fsm->iflag1,fsm->points->eles);

    yfig_cursce_addObjWHis(ymw,obj);
    yfig_cursce_redraw_objAdded(ymw,obj);
}

static void mp_arc_preview(Tvgui_efsmPTS*a, int click1, Cdble2* pts, int ptNb)
{
    Tvgui_efsmPTS* fsm = (Tvgui_efsmPTS*)a;
    Tyfig_device*  dev = (Tyfig_device*) a->any.outdevice;
    Tyfig_mw*      ymw = (Tyfig_mw*)     a->any.userdata;

    TEMP_asf_printf(ptNb!=3,"ptNb is %d instead of 3",ptNb);
#if 0
    printf("pts= (%f %f) (%f %f) (%f %f)\n", pts[0].x,pts[0].y,
            pts[1].x,pts[1].y, pts[2].x,pts[2].y);
#endif

    Tdble2 center;
    if ( tool_arcCenterOf3Points_PTD(&center,pts+0,pts+1,pts+2)==0 ) {
        // points are co-linear, we can not draw an arc
        // but a multi-line. Those is already drawn.
        return;
    }

    TobjAll obj = obj_arc_newA3(&ymw->ymw_currDP,
        fsm->iflag1,pts[0],pts[1],pts[2]);

    ydev_draw_obj(dev,obj.any,OD_GC_Preview);
    obj_dir_delObjQ( obj );
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_arcopen_big[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x38,0x00,0x00,0x00,0xc0,0xf8,
0x00,0x00,0x00,0x80,0x38,0x03,0x00,0x00,0x80,0x00,0x0c,
0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x80,0x00,0x20,0x00,
0x00,0xc0,0x01,0x20,0x00,0x00,0x00,0x00,0x40,0x00,0x00,
0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x80,0x30,0x00,0x00,
0x00,0x80,0x48,0x00,0x00,0x00,0xc0,0x41,0x00,0x00,0x00,
0xc0,0x21,0x00,0x00,0x00,0xc0,0x11,0x00,0x00,0x00,0x80,
0x08,0x00,0x00,0x00,0x80,0x78,0x00,0x00,0x00,0x40,0x00,
0x00,0x00,0x00,0x40,0x00,0x00,0xc0,0x00,0x20,0x00,0x00,
0x20,0x01,0x20,0x00,0x00,0x00,0x01,0x10,0x00,0x00,0xc0,
0x00,0x0c,0x00,0x00,0x00,0x39,0x03,0x00,0x00,0x20,0xf9,
0x00,0x00,0x00,0xc0,0x38,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

CUstr mp_arcopen_small[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x03,0x00,0x90,0x0d,0x00,
0x10,0x10,0x00,0x10,0x20,0x00,0x00,0x20,0x00,0x00,0x40,0x00,
0x00,0x80,0x0c,0x00,0x80,0x08,0x00,0x80,0x0d,0x00,0x80,0x05,
0x00,0x80,0x0c,0x00,0x80,0x00,0x00,0x40,0x00,0x18,0x20,0x00,
0x10,0x20,0x00,0x08,0x10,0x00,0x90,0x0d,0x00,0x98,0x03,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/

CUstr mp_arcpop_big[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x03,
0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00,
0x00, 0x00, 0xc2, 0x07, 0x00, 0x00, 0x00, 0xc2, 0x19, 0x00, 0x00, 0x00,
0x27, 0x60, 0x00, 0x00, 0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x08, 0x00,
0x01, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00,
0x80, 0x00, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x84, 0x01, 0x20, 0x00,
0x00, 0x44, 0x02, 0x1c, 0x00, 0x00, 0x0e, 0x02, 0x1c, 0x00, 0x00, 0x0e,
0x01, 0x1c, 0x00, 0x00, 0x8e, 0x00, 0x20, 0x00, 0x00, 0x44, 0x00, 0x40,
0x00, 0x00, 0xc4, 0x03, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00,
0x02, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00, 0x01, 0x00,
0x00, 0x13, 0x80, 0x00, 0x00, 0x80, 0x24, 0x60, 0x00, 0x00, 0x00, 0xc4,
0x19, 0x00, 0x00, 0x00, 0xc3, 0x07, 0x00, 0x00, 0x00, 0xc4, 0x01, 0x00,
0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00};

CUstr mp_arcpop_small[] = {
0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x07, 0x00, 0x10, 0x1b, 0x00,
0x90, 0x20, 0x00, 0x40, 0x40, 0x00, 0x40, 0x40, 0x00, 0x20, 0x80, 0x00,
0x10, 0x00, 0x19, 0x08, 0x00, 0x11, 0x06, 0x00, 0x1b, 0x06, 0x00, 0x0b,
0x08, 0x00, 0x19, 0x10, 0x00, 0x01, 0x20, 0x80, 0x00, 0x40, 0x40, 0x00,
0x58, 0x40, 0x00, 0x90, 0x20, 0x00, 0x08, 0x1b, 0x00, 0x10, 0x07, 0x00,
0x18, 0x00, 0x00, 0x00, 0x00, 0x00};

/*======================================================================*/