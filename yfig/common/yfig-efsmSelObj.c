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
 *$Software: YFIG-YGUI (YFIG top gui)
 *      $Id: yfig-efsmSelObj.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-efsmSelObj.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*=A EFSM specialization that selects an object within a marker point. =*/
/*======================================================================*/

/*======================================================================*/
/*= Implementation notes about drawing xor section                     =*/
/*=                                                                    =*/
/*=    -Section 0 : Original image without any mark                    =*/
/*= Caution : in case of text drawing,it's to the responsibility       =*/
/*=           of the pos function to launch the drawing sequence due   =*/
/*=           to the fact that most of these functions handle both     =*/
/*=           erasing the former selected object and redrawing         =*/
/*=           this object elsewhere                                    =*/
/*= Summary of Draw session :                                          =*/
/*= each time a button is pressed a save of the state of drawing area  =*/
/*= is saved in the section 0                                          =*/
/*= Then when continue button is pressed another save is done          =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-efsmSelObj.h"
#include "yfig-device.h"

#include "fig/fig-scene.h"
#include "fig/object.h"
#include "outdev/ldaf-hl.h"

#define EFSMST_OBJSEL (EFSMST_NFS+0) // fsm_selfunc runs
#define EFSMST_OBJNEW (EFSMST_NFS+1) // fsm_posfunc runs

Inline void efsmSel1Obj_draw(TefsmSel1Obj* fsm)
{
    Tyfig_device* fsod  = (Tyfig_device*)fsm->any.outdevice;
    Toutdev*      od    = fsod_getOD(fsod);
    CboxDble*     box   = fsod_getBoxSrc(fsod);
    Tod_GCdef     gcd   = OD_GC_Preview;
    double x  = fsm->fsm_AxisPos.x;
    double y  = fsm->fsm_AxisPos.y;
    double x0 = Box_x ( *box );
    double y0 = Box_y ( *box );
    double dx = Box_dx( *box );
    double dy = Box_dy( *box );
    double delta = 10 / od->homothetySrc2Des;
    //fsod_objmarker_toggleCur(fsod,fsm->fsm_objMask,0);
    fsod_objmarker_toggleCur2(fsod, fsm->fsm_objMask,
            fsm->fsm_noCtrMkr,fsm->fsm_mkrOpeMask);
    if ( fsm->fsm_AxisHLine ) od_draw_lineXY(od,gcd, x0,y  , x0+dx,y);
    if ( fsm->fsm_AxisVLine ) od_draw_lineXY(od,gcd, x, y0 , x,y0+dy);
    if ( fsm->fsm_AxisDiag ) {
        od_draw_lineXY(od,gcd, x-delta, y-delta , x+delta,y+delta);
        od_draw_lineXY(od,gcd, x-delta, y+delta , x+delta,y-delta);
    }
}

Inline void efsmSel1Obj_erase(TefsmSel1Obj* fsm)
{
    efsmSel1Obj_draw(fsm);
}

/*======================================================================*/

static void efsmSel1Obj_trans(Tvgui_efsm* f, Tvgui_efsm_in kind, int x, int y)
{
    TefsmSel1Obj* fsm   = (TefsmSel1Obj*)f;
    Tyfig_device* ydev  = (Tyfig_device*)fsm->any.outdevice;
    Tfig_scene*   scene = fsod_getScene(ydev);

    DBG0_EfsmEkd(EFSM_FMTkd,EFSM_ARGkd);

    if ( kind==EFSMIN_Init ) {
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        scene->sc_selectableObjectMask = fsm->fsm_objMask;
        efsmSel1Obj_draw(fsm);
        DBG0_EfsmLkd(EFSM_FMTkd": ret=1",EFSM_ARGkd);
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        switch ( fsm->fsm_state ) {
          case EFSMST_OBJSEL:
          case EFSMST_OBJNEW:
            DBG0_EfsmLkd(EFSM_FMTkd": ret=0",EFSM_ARGkd);
            return;
          default:
            efsmSel1Obj_draw(fsm);
            DBG0_EfsmLkd(EFSM_FMTkd": ret=1",EFSM_ARGkd);
            return;
        }
    }

    int click;
    switch ( fsm->fsm_state ) {
        case EFSMST_BEGIN:
            if ( kind == EFSMIN_Quit ) {
                efsmSel1Obj_erase(fsm);
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                scene->sc_selectableObjectMask = 0;
                fsm->fsm_state = EFSMST_END;
            } else if ( kind == EFSMIN_But1Pressed ) {
                click = 1;
button_clicked:
                if ( fsm->fsm_selfunc ) {
                    TobjCloseToPt info = fsod_searchFirstM2XY(ydev,x,y,fsm->fsm_objMask,fsm->fsm_mkrOpeMask);
                    fsm->fsm_selObj = info.obj;
                    if ( OBJ_IsDefined(fsm->fsm_selObj) ) {
                        int index           = info.idx;
                        fsm->fsm_selClick   = click;
                        fsm->fsm_selShpt    = info.shpt;
                        fsm->fsm_selMark    = info.mkr;
                        fsm->fsm_selMarkIdx = info.idx;
                        efsmSel1Obj_erase(fsm);
                        od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                        od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                        vgui_efsm_mousefun_clear(&fsm->any);
                        fsm->fsm_selMarkIdx = index;
                        fsm->fsm_state = EFSMST_OBJSEL;
                        fsm->fsm_selfunc(fsm,click,info.obj,info.mkr);
                        fsm->fsm_state = EFSMST_BEGIN;
                        vgui_efsm_mousefun_show(&fsm->any);
                        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                        efsmSel1Obj_draw(fsm);
                        break;
                    }
                }
                if ( fsm->fsm_posfunc ) { // selectedObj==0
                    Tdble2 pos = { x, y };
                    efsmSel1Obj_erase(fsm);
                    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                    vgui_efsm_mousefun_clear(&fsm->any);
                    fsm->fsm_state = EFSMST_OBJNEW;
                    fsm->fsm_posfunc(fsm,click,pos);
                    fsm->fsm_state = EFSMST_BEGIN;
                    vgui_efsm_mousefun_show(&fsm->any);
                    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                    efsmSel1Obj_draw(fsm);
                }
            } else if ( kind == EFSMIN_But2Pressed ) {
                click=2;
                goto button_clicked;
            } else if ( kind == EFSMIN_But3Pressed ) {
                if ( fsm->fsm_posfunc ) {
                    Tdble2 pos = { x, y };
                    efsmSel1Obj_erase(fsm);
                    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                    fsm->fsm_posfunc(fsm,3,pos);
                    efsmSel1Obj_draw(fsm);
                    vgui_efsm_mousefun_show(&fsm->any);
                }
            }
            break;
        case EFSMST_END:
            break;
    }
    DBG0_EfsmLkd(EFSM_FMTkd": ret=1",EFSM_ARGkd);
    return;
}

extern TefsmSel1Obj* yfig_efsmSel1Obj_new(Twid_canvas* cvs,
              int mask, TefsmSel1Obj_selFunc func, void* ud)
{
    TefsmSel1Obj* ret = (TefsmSel1Obj*)vgui_efsm_new( sizeof(*ret), cvs,
                0,0, efsmSel1Obj_trans,ud);
    ret -> fsm_selfunc = func;
    ret -> fsm_objMask = mask;

    return ret;
}

/*======================================================================*/
