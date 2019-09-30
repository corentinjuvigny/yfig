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
 *      $Id: compound-do.c 310 2018-07-17 10:01:05Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/compound-do.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: The EFSM for creating compound object.                 =*/
/*=                                                                    =*/
/*= This file is included by compound.c                                =*/
/*======================================================================*/

/*======================================================================*/
/*= Implementation notes about drawing xor seccion                     =*/
/*=                                                                    =*/
/*=    -Section 0 : Original image without any mark                    =*/
/*======================================================================*/
#include "fig/fig-scene.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= Data structucture                                                  =*/

typedef struct _TefsmCOMP {
    Tvgui_efsm  any;
    TobjAllSet* selectedObjects;
    Tdble2      regPt1;   // first corner of region
    Tdble2      regPt2;   // second corner of region
} TefsmCOMP;

#define EFSMST_REG1      (EFSMST_NFS+0)
#define EFSMST_REG2      (EFSMST_NFS+1)

#define DEF_NoCtrMkr 0 // draw makkers that are also object center

/*======================================================================*/
/*= Interface                                                          =*/

// the transition function of the EFSM creating compounds.
static void cpdDo_efsm(Tvgui_efsm*fsm, Tvgui_efsm_in kind, int x,int y);

static Tvgui_efsm* cpdDo_newEFSM( Twid_canvas*cvs, Tyfig_mw*ymw)
{
    Tvgui_efsm* fsm = vgui_efsm_new( sizeof(TefsmCOMP),cvs,
            0,0,cpdDo_efsm,ymw);
    return fsm;
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void automata_compounDo_draw_region(Tyfig_device*dev, Tdble2 pt1, Tdble2 pt2)
{
    TboxDble box; tool_boxDble_initPT(&box, pt1, pt2);
    od_draw_rectXYWH(fsod_getOD(dev),OD_GC_SelArea,
            Box_x (box), Box_y (box),
            Box_dx(box), Box_dy(box)
    );
}

// draw or erase the markers of the selcted objects.
static void automata_compounDo_mark(TefsmCOMP* fsm)
{
    int i;
    Tyfig_device* ydev = (Tyfig_device*)fsm->any.outdevice;

    for ( i=0 ; i<fsm->selectedObjects->eleNb ; i+=1 ) {
      TobjAll o = fsm->selectedObjects->eles[i];
      fsod_objmarker_toggle(ydev,o,DEF_NoCtrMkr);
    }
}

// highlight or unhighlight the selcted objects.
static void automata_compounDo_highlight(TefsmCOMP* fsm)
{
    int i;
    Tyfig_device* ydev = (Tyfig_device*)fsm->any.outdevice;

    for ( i=0 ; i<fsm->selectedObjects->eleNb ; i+=1 ) {
      TobjAll o = fsm->selectedObjects->eles[i];
      fsod_objmarker_toggleHL(ydev,o,DEF_NoCtrMkr);
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void cpdDo_efsm(Tvgui_efsm*a, Tvgui_efsm_in kind, int x,int y)
{
    TefsmCOMP*    fsm   = (TefsmCOMP*)   a;
    Tyfig_device* ydev  = (Tyfig_device*)fsm->any.outdevice;
    Tyfig_mw*     ymw   = (Tyfig_mw*)    a->userdata;
    Tfig_scene*   scene = fsod_getScene(ydev); //ymw->ymw_scene;

    if ( kind==EFSMIN_Init ) {
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        fsm->selectedObjects = objAllSet_new(100);
        scene->sc_selectableObjectMask = FSC_MM_ALL;
        fsod_objmarker_toggleCur(ydev,FSC_MM_ALL,DEF_NoCtrMkr);
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        //Tmarkers* settings = &sc->state.settings;
        //settings->cur_objmask = 0;
        fsod_objmarker_toggleCur(ydev,FSC_MM_ALL,DEF_NoCtrMkr);
        automata_compounDo_highlight(fsm);
        return;
    }

#define FSM_return() return
    int prev_state=a->state;
    switch ( a->state ) {
        case EFSMST_BEGIN:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind == EFSMIN_But1Pressed ) {
                TobjAll selectedObj = fsod_searchFirstXY(ydev,x,y).obj;
                if ( OBJ_IsDefined(selectedObj) ) {
                    // highlight or unhighlight the selected object
                    fsod_objmarker_toggleHL(ydev,selectedObj,DEF_NoCtrMkr);
                    if ( objAllSet_existObj(fsm->selectedObjects,selectedObj) )
                        objAllSet_supObjQuick(fsm->selectedObjects,selectedObj);
                    else
                        objAllSet_add(fsm->selectedObjects,selectedObj);
                }
            } else if ( kind == EFSMIN_But2Pressed ) {
                fsm->regPt1.x = x; 
                fsm->regPt1.y = y; 
                a->state = EFSMST_REG1;
            } else if ( kind == EFSMIN_But3Pressed ) {
                TobjAll dir = yfig_cursce_mkdObjWHis(ymw,
                                fsm->selectedObjects->eles,
                                fsm->selectedObjects->eleNb);
                // unhighlight + unmark slected object 
                automata_compounDo_highlight(fsm);
                automata_compounDo_mark(fsm);
                objAllSet_clear(fsm->selectedObjects);
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                // mark the compound object 
                fsod_objmarker_toggle(ydev,dir,DEF_NoCtrMkr);
            }
            FSM_return();

        case EFSMST_REG1:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind==EFSMIN_But1Pressed ||
                        kind==EFSMIN_But2Pressed ) {
                if ( dble2_cmpXY(&fsm->regPt1,x,y)!=0 ) {
                    a->state = EFSMST_BEGIN;
                    fsm->regPt2.x = x; 
                    fsm->regPt2.y = y; 
                    goto add_object_of_region;
                }
                goto add_object_of_region;
            } else if ( kind==EFSMIN_But3Pressed ) {
                a->state = EFSMST_BEGIN;
            } else if ( kind==EFSMIN_MouseMotion ) {
                a->state = EFSMST_REG2;
                goto draw_first_reg;
            }
            FSM_return();

        case EFSMST_REG2:
            if ( kind == EFSMIN_Quit ) {
                a->state = EFSMST_END;
                goto clean;
            } else if ( kind==EFSMIN_But1Pressed ||
                        kind==EFSMIN_But2Pressed ) {
                if ( dble2_cmpXY(&fsm->regPt1,x,y)!=0 ) {
                    a->state = EFSMST_BEGIN;
                    automata_compounDo_draw_region(ydev,fsm->regPt1,fsm->regPt2);
                    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                    goto add_object_of_region;
                }
            } else if ( kind==EFSMIN_But3Pressed ) {
                automata_compounDo_draw_region(ydev,fsm->regPt1,fsm->regPt2);
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                a->state = EFSMST_BEGIN;
            } else if ( kind==EFSMIN_MouseMotion ) {
                if ( dble2_cmpXY(&fsm->regPt2,x,y)!=0 )
                    goto draw_clear_reg;
            }
            FSM_return();

        case EFSMST_END:
            FSM_return();
    }
clean:
    automata_compounDo_highlight(fsm);
    objAllSet_delete(fsm->selectedObjects); fsm->selectedObjects = 0;
    fsod_objmarker_toggleCur(ydev,FSC_MM_ALL,DEF_NoCtrMkr);
    scene->sc_selectableObjectMask = 0;
    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
    FSM_return();
draw_clear_reg:
    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
    automata_compounDo_draw_region(ydev,fsm->regPt1,fsm->regPt2);
draw_first_reg:
    fsm->regPt2.x = x; 
    fsm->regPt2.y = y; 
    automata_compounDo_draw_region(ydev,fsm->regPt1,fsm->regPt2);
    FSM_return();
add_object_of_region: {
    int i;
    TboxDble region; tool_boxDble_initPT(&region, fsm->regPt1, fsm->regPt2);
    TobjAllSet* s = fsod_getChildrenB(ydev,&region);
    for ( i=0 ; i<s->eleNb ; i+=1 ) {
        TobjAll o=s->eles[i];
        if ( objAllSet_existObj(fsm->selectedObjects,o) ) {
            // o is already selected and so highlighted, we unselect it.
            fsod_objmarker_toggleHL(ydev,o,DEF_NoCtrMkr);    // unhighlight
            objAllSet_supObjQuick(fsm->selectedObjects,o);
        } else {
            // we add o to the selcted object.
            fsod_objmarker_toggleHL(ydev,o,DEF_NoCtrMkr);
            objAllSet_add(fsm->selectedObjects,o);
        }
    }
    objAllSet_delete(s);
    FSM_return();
    }
}

/*======================================================================*/
