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
 *      $Id: yfig-efsmSelNObj.c 316 2018-07-19 15:29:19Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-efsmSelNObj.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A EFSM specialization that selects either one object or all the
 * objects inside a rectangular area.
 *
 * See the yfig-efsmSelNObj.h header for documentation.
**======================================================================*/

/*======================================================================*/
/*= Implementation notes about drawing xor seccion                     =*/
/*=                                                                    =*/
/*=    -Section 0 : Original image without any mark                    =*/
/*=    -Section 1 : Current image without markers when selecting       =*/
/*=                 a region.                                          =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-efsmSelNObj.h"
#include "yfig-device.h"

#include "fig/fig-scene.h"
#include "fig/object.h"

#include "outdev/ldaf-hl.h"

/*======================================================================*/

#define EFSMST_REG1      (EFSMST_NFS+0)
#define EFSMST_REG2      (EFSMST_NFS+1)

/*======================================================================*/
/*= Interface                                                          =*/

// the transition function of the EFSM creating compounds.
static void efsmSelNObj_trans(Tvgui_efsm*fsm, Tvgui_efsm_in kind, int x,int y);

extern TefsmSelNObj* yfig_efsmSelNObj_new(Twid_canvas* cvs,
              int mask, void* ud,
              TefsmSelNObj_sel1Func f1, TefsmSelNObj_selNFunc fn)
{
    TefsmSelNObj* ret = (TefsmSelNObj*)vgui_efsm_new( sizeof(*ret), cvs,
                0,0, efsmSelNObj_trans,ud);
    ret -> fsm_sel1func = f1;
    ret -> fsm_selNfunc = fn;
    ret -> fsm_objMask = mask;

    return ret;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void efsmSelNObj_draw_region(Tyfig_device*dev, Tdble2 pt1, Tdble2 pt2)
{
    TboxDble box; tool_boxDble_initPT(&box, pt1, pt2);
    od_draw_rectXYWH(fsod_getOD(dev),OD_GC_SelArea,
            Box_x (box), Box_y (box),
            Box_dx(box), Box_dy(box)
    );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void efsmSelNObj_trans(Tvgui_efsm*a, Tvgui_efsm_in kind, int x,int y)
{
    TefsmSelNObj* fsm   = (TefsmSelNObj*)a;
    Tyfig_device* ydev  = (Tyfig_device*)fsm->any.outdevice;
    Tyfig_mw*     ymw   = (Tyfig_mw*)    a->userdata;
    Tfig_scene*   scene = fsod_getScene(ydev);

    if ( kind==EFSMIN_Init ) {
        scene->sc_selectableObjectMask = fsm->fsm_objMask;
        fsod_objmarker_toggleCur(ydev,fsm->fsm_objMask,0);
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        switch ( fsm->fsm_state ) {
          case EFSMST_BEGIN:
          case EFSMST_REG1:
          case EFSMST_REG2:
            od_ds_xorDrawStart_1(fsm->fsm_outdevice);
            od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
            fsod_objmarker_toggleCur(ydev,fsm->fsm_objMask,0);
        }
        return;
    }

#define FSM_return() return
    switch ( a->state ) {
        case EFSMST_BEGIN:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind == EFSMIN_But1Pressed ) {
                TobjCloseToPt info=fsod_searchFirstXY(ydev, x,y);
                if ( OBJ_IsDefined(info.obj) && fsm->fsm_sel1func!=0 ) { 
                    fsm->fsm_sel1func( fsm, info.obj );
                    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                }
            } else if ( kind == EFSMIN_But2Pressed ) {
                fsm->fsm_regPt1.x = x; 
                fsm->fsm_regPt1.y = y; 
                a->state = EFSMST_REG1;
            }
            FSM_return();

        case EFSMST_REG1:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind==EFSMIN_But1Pressed ||
                        kind==EFSMIN_But2Pressed ) {
                if ( dble2_cmpXY(&fsm->fsm_regPt1,x,y)!=0 ) {
                    a->state = EFSMST_BEGIN;
                    fsm->fsm_regPt2.x = x; 
                    fsm->fsm_regPt2.y = y; 
                    goto object_of_region;
                }
            } else if ( kind==EFSMIN_But3Pressed ) {
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                a->state = EFSMST_BEGIN;
            } else if ( kind==EFSMIN_MouseMotion ) {
                a->state = EFSMST_REG2;
                fsm->fsm_regPt2.x = x; 
                fsm->fsm_regPt2.y = y; 
                efsmSelNObj_draw_region(ydev,fsm->fsm_regPt1,fsm->fsm_regPt2);
            }
            FSM_return();

        case EFSMST_REG2:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind==EFSMIN_But1Pressed ||
                        kind==EFSMIN_But2Pressed ) {
                if ( dble2_cmpXY(&fsm->fsm_regPt1,x,y)!=0 ) {
                    a->state = EFSMST_BEGIN;
                    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                    efsmSelNObj_draw_region(ydev,fsm->fsm_regPt1,fsm->fsm_regPt2);
                    goto object_of_region;
                }
            } else if ( kind==EFSMIN_But3Pressed ) {
                efsmSelNObj_draw_region(ydev,fsm->fsm_regPt1,fsm->fsm_regPt2);
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                a->state = EFSMST_BEGIN;
            } else if ( kind==EFSMIN_MouseMotion ) {
                if ( dble2_cmpXY(&fsm->fsm_regPt2,x,y)!=0 )
                    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                    efsmSelNObj_draw_region(ydev,fsm->fsm_regPt1,fsm->fsm_regPt2);
                    fsm->fsm_regPt2.x = x; 
                    fsm->fsm_regPt2.y = y; 
                    efsmSelNObj_draw_region(ydev,fsm->fsm_regPt1,fsm->fsm_regPt2);
            }
            FSM_return();

        case EFSMST_END:
            FSM_return();
    }

clean:
    fsod_objmarker_toggleCur(ydev,fsm->fsm_objMask,0);
    od_ds_xorEraseStart_1(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_1(fsm->fsm_outdevice);
    scene->sc_selectableObjectMask = 0;
    FSM_return();

object_of_region: {
        TboxDble region;
        int i;
        tool_boxDble_initPT(&region, fsm->fsm_regPt1, fsm->fsm_regPt2);
        TobjAllSet* s = fsod_getChildrenB(ydev,&region);
        TobjAllSet* s2 = objAllSet_new(s ? s->eleNb : 0);
        for ( i=0 ; s!=0 && i<s->eleNb ; i++ ) {
            CboxDble* obb = obj_BBox(s->eles[i]);
            if ( tool_boxDble_isInside(obb, &region)!=0 )
                objAllSet_add(s2, s->eles[i]);
        }
        if ( fsm->fsm_selNfunc ) {
            fsm->fsm_selNfunc(fsm,s2);
        } else if ( fsm->fsm_sel1func ) {
            for ( i=0 ; i<s2->eleNb ; i+=1 )
                fsm->fsm_sel1func(fsm,s2->eles[i]);
        }
        objAllSet_delete(s2);
        objAllSet_delete(s);
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
    }
    FSM_return();
}

/*======================================================================*/
