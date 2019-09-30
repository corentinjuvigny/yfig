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
 *      $Id: yfig-efsmSelObj+move.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-efsmSelObj+move.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A EFSM specialization that selects an object within a marker point
 * and then gives the new position of the marker.
 *
 * See the yfig-efsmSelObj+move.h header for documentation.
**======================================================================*/

/*======================================================================*/
/*= Implementation notes about drawing xor seccion                     =*/
/*=                                                                    =*/
/*=    -Section 0 : Original image without the element selected        =*/
/*=    -Section 1 : Current image without markers                      =*/
/*= Caution : it's to the responsibility of the sel function to launch =*/
/*=           the drawing sequence due to the fact that most of these  =*/
/*=           functions handle both erasing the former selected object =*/
/*=           and redrawing this object elsewhere                      =*/
/*======================================================================*/



#include "yfig-config.h"
#include "yfig-efsmSelObj+move.h"
#include "yfig-device.h"

#include "fig/fig-scene.h"
#include "fig/object.h"

#define EFSMST_NewPos (EFSMST_NFS+0) // New Position

#define FSOD_UpdateMarker(fsm) fsod_objmarker_toggleCur2(ydev, \
        fsm->fsm_objMask,fsm->fsm_noCtrMkr,fsm->fsm_mkrOpeMask)

/*======================================================================*/

static void efsmSel1ObjMv_trans(Tvgui_efsm* f, Tvgui_efsm_in kind, int x, int y)
{
    TefsmSel1ObjMv* fsm   = (TefsmSel1ObjMv*)f;
    Tyfig_device*   ydev  = (Tyfig_device*)fsm->any.outdevice;
    Tfig_scene*     scene = fsod_getScene(ydev);
    Tdble2          marker;

    DBG0_EfsmEkd(EFSM_FMTkd,EFSM_ARGkd);
    if ( kind==EFSMIN_Init ) {
        scene->sc_selectableObjectMask = fsm->fsm_objMask;
        od_ds_xorDrawStart_1(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
        FSOD_UpdateMarker(fsm);
        DBG0_EfsmLkd(EFSM_FMTkd,EFSM_ARGkd);
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        switch ( fsm->fsm_state ) {
          case EFSMST_NewPos:
            if ( OBJ_IsDefined(fsm->fsm_prevObj) )
                ydev_draw_obj(ydev,fsm->fsm_prevObj.any,OD_GC_Preview);
          case EFSMST_END:
            DBG0_EfsmLkd(EFSM_FMTkd,EFSM_ARGkd);
            return;
          default:
            od_ds_xorDrawStart_1(fsm->fsm_outdevice);
            od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
            FSOD_UpdateMarker(fsm);
            DBG0_EfsmLkd(EFSM_FMTkd,EFSM_ARGkd);
            return;
        }
    }

    int click;
    switch ( fsm->fsm_state ) {
        case EFSMST_BEGIN:
            if ( kind == EFSMIN_Quit ) {
                FSOD_UpdateMarker(fsm);
                od_ds_xorEraseStart_1(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_1(fsm->fsm_outdevice);
                scene->sc_selectableObjectMask = 0;
                fsm->fsm_state = EFSMST_END;
            } else if ( kind == EFSMIN_But1Pressed ) {
                TobjCloseToPt tmp;
                click = 1;
button_clicked: 
                tmp = fsod_searchFirstM2XY(ydev,x,y,fsm->fsm_objMask,fsm->fsm_mkrOpeMask);
                fsm->fsm_selObj = tmp.obj;
                if ( OBJ_IsDefined(fsm->fsm_selObj) ) {
                    fsm->fsm_selClick   = click;
                    fsm->fsm_selShpt    = tmp.shpt;
                    fsm->fsm_selMark    = tmp.mkr;
                    fsm->fsm_selMarkIdx = tmp.idx;
                    od_ds_xorDrawStart_1(fsm->fsm_outdevice);
                    od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
                    FSOD_UpdateMarker(fsm);
                    fsm->fsm_selAbort = 0;
                    if ( fsm->fsm_selfunc )
                        fsm->fsm_selfunc(fsm,click,fsm->fsm_selObj,fsm->fsm_selMark); //have to handle the save of the section 0
                    if ( fsm->fsm_selAbort==0 ) {
                        fsm->fsm_state = EFSMST_NewPos;
                        fsm->any.magnet = 1;
                    }
                }
            } else if ( kind == EFSMIN_But2Pressed ) {
                click=2;
                goto button_clicked;
            }
            break;
        case EFSMST_NewPos:
            if ( kind == EFSMIN_Quit ) {
                od_ds_xorDrawStart_1(fsm->fsm_outdevice);
                od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
                FSOD_UpdateMarker(fsm);
                scene->sc_selectableObjectMask = 0;
                fsm->fsm_state = EFSMST_END;
                if ( fsm->fsm_finfunc ) {
                    marker.x = x; marker.y = y;
                    fsm->fsm_finfunc(fsm,0,marker);
                }
            } else if ( kind == EFSMIN_But1Pressed ) {
                click = 1;
button_clicked_2:
                if ( fsm->fsm_finfunc ) {
                    marker.x = x; marker.y = y;
                    fsm->fsm_finfunc(fsm,click,marker);
                }
                od_ds_xorDrawStart_1(fsm->fsm_outdevice);
                od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
                FSOD_UpdateMarker(fsm);
                fsm->fsm_state = EFSMST_BEGIN;
                fsm->any.magnet = 0;
            } else if ( kind == EFSMIN_But2Pressed ) {
                click = 2; goto button_clicked_2;
            } else if ( kind == EFSMIN_But3Pressed ) {
                click = 3; goto button_clicked_2;
            } else if ( kind == EFSMIN_MouseMotion ) {
                if ( fsm->fsm_posfunc ) {
                    marker.x = x; marker.y = y;
                    od_ds_xorEraseStart_1(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_1(fsm->fsm_outdevice);
                    fsm->fsm_posfunc(fsm,marker);
                }
            }
            break;
        case EFSMST_END:
            break;
    }
    DBG0_EfsmLkd(EFSM_FMTkd": ret=1",EFSM_ARGkd);
    return;
}

extern TefsmSel1ObjMv* yfig_efsmSel1ObjMv_new(Twid_canvas* dev,
              int mask, void* ud,
              TefsmSel1ObjMv_selFunc sel,
              TefsmSel1ObjMv_posFunc move,
              TefsmSel1ObjMv_finFunc finish)
{
    TefsmSel1ObjMv* ret = (TefsmSel1ObjMv*)vgui_efsm_new( sizeof(*ret), dev,
                0,0, efsmSel1ObjMv_trans,ud);
    ret -> fsm_selfunc = sel;
    ret -> fsm_posfunc = move;
    ret -> fsm_finfunc = finish;
    ret -> fsm_objMask = mask;

    return ret;
}

/*======================================================================*/
