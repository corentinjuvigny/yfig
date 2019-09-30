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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-efsm-points.c 335 2018-10-31 22:02:37Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-efsm-points.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= FSM definition for getting a point set.                            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-efsm-points.h header for the type and function   =*/
/*= documentation.                                                     =*/
/*=                                                                    =*/
/*= Implementation notes about drawing xor seccion                     =*/
/*=                                                                    =*/
/*=    -Section 0 : Last generated valid image                         =*/
/*=    -Section 1 : Last segments of skeleton                          =*/
/*=    -Section 2 : Cloising segment of skeleton                       =*/
/*=    -Section 3 : Preview shape                                      =*/
/*=    The order for drawing is : section 0, 1, 2, 3                   =*/
/*=    The order for erasing is : section 3, 2, 1, 0                   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-efsm-points.h"
#include "outdev/ldaf-hl.h"

#define DBG_ENTER(fsm,kd,x,y) DBG0_EfsmEkd(EFSM_FMTkd ", nb=%d ",\
                                fsm->fsm_state,kd,x,y,fsm->nb)

#define EFSMST_NewPos   2
#define EFSMST_TmpPos   3
#define EFSMST_HandFree 4

static void efsmpts_prev_draw(Cvgui_efsmPTS* fsm,int draw);
static void efsmpts_draft_drawSl (Cvgui_efsmPTS* fsm);
static void efsmpts_draft_drawSc (Cvgui_efsmPTS* fsm);
static void efsmpts_draft_drawSlc(Tvgui_efsmPTS* fsm);

#define dble2set_NB(points) ((points)->eleNb)
/*======================================================================*/
/*= points set interface                                               =*/
#include "outdev/outdev.h"

static void efsmpts_drawSc(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    if ( fsm->skeletonMode!=2 ) return;
    if ( points->eleNb < 3 ) return;
    od_ds_xorDrawStart_2(fsm->fsm_outdevice);
    efsmpts_draft_drawSc(fsm);
    od_ds_xorDrawCommit_2(fsm->fsm_outdevice);
}

static void efsmpts_eraseSc(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    if ( fsm->skeletonMode!=2 ) return;
    if ( points->eleNb < 3 ) return;
    od_ds_xorEraseStart_2(fsm->fsm_outdevice);
    efsmpts_draft_drawSc(fsm);
    od_ds_xorEraseCommit_2(fsm->fsm_outdevice);
}

static void efsmpts_drawSlc(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    if ( !( fsm->skeletonMode==0 && points->eleNb<2 ) ) {
        od_ds_xorDrawStart_1(fsm->fsm_outdevice);
        efsmpts_draft_drawSl(fsm);
        od_ds_xorDrawCommit_1(fsm->fsm_outdevice);
    }
    efsmpts_drawSc(fsm);
}

static void efsmpts_eraseSlc(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    efsmpts_eraseSc(fsm);
    if ( !( fsm->skeletonMode==0 && points->eleNb<2 ) ) {
        od_ds_xorEraseStart_1(fsm->fsm_outdevice);
        efsmpts_draft_drawSl(fsm);
        od_ds_xorEraseCommit_1(fsm->fsm_outdevice);
    }
}

/**
 * The efsmpts_pos_push function adds the (x,y) point at the end
 * of the a->point array.
 *
 * The efsmpts_pos_pop function suppresses the last point
 * of the a->point array.
 *
 * The efsmpts_pos_poppush function suppresses the last point
 * of the a->point array and then it adds the (x,y) point at the end
 * of the a->point array. So it chains:
 *   efsmpts_pos_pop(fsm);
 *   efsmpts_pos_push(fsm,x,y);
**/
static void efsmpts_pos_push(Tvgui_efsmPTS* fsm, double x, double y)
{
    dble2set_addXY(fsm->points,x,y);
}

static void efsmpts_pos_pop(Tvgui_efsmPTS* fsm)
{
    fsm->points->eleNb -= 1;
}

static void efsmpts_pos_poppush(Tvgui_efsmPTS* fsm, double x, double y)
{
    Tdble2* last = dble2set_last(fsm->points);
    last->x = x;
    last->y = y;
}

static void efsmpts_clear(Tvgui_efsmPTS* fsm)
{
    dble2set_clear(fsm->points);
}

static void efsmpts_free(Tvgui_efsmPTS* fsm)
{
    dble2set_delete(fsm->points);
}
#define efsmPTS_free ((void(*)(Tvgui_efsm*))efsmpts_free)

static int efsmpts_pos_lastEqXY(Cvgui_efsmPTS* fsm, double x, double y)
{
    Cdble2* last = dble2set_last(fsm->points);
    if ( last==0 ) return 0;
    return last->x==x && last->y==y;
}

/*======================================================================*/
/*= drawing the draft multi-line                                       =*/

static void efsmpts_prev_draw(Cvgui_efsmPTS* fsm, int draw)
{
    Tvgui_efsmPTS* wfsm = (Tvgui_efsmPTS*)fsm;
    if ( fsm->drawSetFunc!=0 && fsm->points->eleNb>=fsm->nb ) {
        if (draw) od_ds_xorDrawStart_3(fsm->fsm_outdevice); else od_ds_xorEraseStart_3(fsm->fsm_outdevice);
        fsm->drawSetFunc(wfsm, fsm->firstClick, fsm->points->eles, fsm->points->eleNb);
        if (draw) od_ds_xorDrawCommit_3(fsm->fsm_outdevice); else od_ds_xorEraseCommit_3(fsm->fsm_outdevice);
    }
}

/**
 * The efsmpts_draft_drawS function draws the segment (src,des) into
 * the device associated to the fsm FSM using the 
 * fsm->skeletonGcd GC.
 *
 * The function does not draw segments of 0 size (eg: src==des).
 **/
static void efsmpts_draft_drawS  (Cvgui_efsmPTS* fsm,
                                 Cdble2* src, Cdble2* des)
{
    if ( (src->x==des->x && src->y==des->y) )
            return;

    Coutdev* od  = fsm->fsm_outdevice;
    od_draw_linePT( od, fsm->skeletonGcd, src, des);
}

/**
 * The efsmpts_draft_drawSl function draws the last segment of the
 * fsm->points array.
 * If the last segment does not exist (eg: the size of fsm->points
 * array is null or 1), the function returns immediately.
 *
 * The efsmpts_draft_drawSc function draws the segment that closes
 * the polygon defined by the fsm->points  array (eg: the segment
 * from the last point to the first).
 * If the close segment does not exist (eg: the size of fsm->points
 * array is null or 1 or 2), the function returns immediately.
 *
 * The efsmpts_draft_drawSlc function draws both the last segment
 * and the segment that closes the polygon defined by the fsm->points
 * array. It chains the two former functions.
 *
 * These functions do not draw segments with null size.
 * They also check the fsm->skeletonMode flag and so they draw
 * only the segments (last or close) that the flag selects. 
 * They draw the segments into the device associated to the fsm FSM
 * using the fsm->skeletonGcd GC.
**/
static void efsmpts_draft_drawSl (Cvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    if ( fsm->skeletonMode==0 ) return;
    if ( points->eleNb < 2 ) return;
    Cdble2* src = dble2set_last(points);
    Cdble2* des = points->eles+points->eleNb-2;

    efsmpts_draft_drawS(fsm,src,des);
}

static void efsmpts_draft_drawSc (Cvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    if ( fsm->skeletonMode!=2 ) return;
    if ( points->eleNb < 3 ) return;
    Cdble2* src = dble2set_last (points);
    Cdble2* des = dble2set_first(points);

    efsmpts_draft_drawS(fsm,src,des);
}

static void efsmpts_draft_drawSlc(Tvgui_efsmPTS* a)
{
    efsmpts_draft_drawSl(a);
    efsmpts_draft_drawSc(a);
}

static void efsmpts_draft_redraw(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    int i;
    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
    od_ds_stdStart(fsm->fsm_outdevice);
    for (i=0; i<dble2set_NB(points)-2 ; i+=1) {
        // DBG(vgui_draw_linePT(dev,OD_GC_XorPAINT, a->points->eles+i, a->points->eles+i+1);)
        efsmpts_draft_drawS(fsm, points->eles+i, points->eles+i+1 );
    }
    od_ds_stdCommit(fsm->fsm_outdevice);
    efsmpts_drawSlc(fsm);
    efsmpts_prev_draw(fsm,1);
}

static void efsmpts_draft_erase(Tvgui_efsmPTS* fsm)
{
    Cdble2Set* points = fsm->points;
    int i;
    efsmpts_prev_draw(fsm,0);
    efsmpts_eraseSc(fsm);
    od_ds_xorEraseStart_1(fsm->fsm_outdevice);
    for (i=0; i<dble2set_NB(points)-1 ; i+=1) {
        // DBG(vgui_draw_linePT(dev,OD_GC_XorPAINT, a->points->eles+i, a->points->eles+i+1);)
        efsmpts_draft_drawS(fsm, points->eles+i, points->eles+i+1 );
    }
    od_ds_xorEraseCommit_1(fsm->fsm_outdevice);
    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
}

/*======================================================================*/
/*= exactly 1 point (nb>1)                                             =*/
/*=    button 1 or 2 for selecting the point                           =*/

static void efsmPTS_1exact(Tvgui_efsm* _a,
        Tvgui_efsm_in kind, int x, int y)
{
    Tvgui_efsmPTS* fsm  = (Tvgui_efsmPTS*)_a;
    DBG_ENTER(fsm,kind,x,y);

    if ( kind==EFSMIN_Quit ) {
        efsmpts_clear(fsm);
        EState = EFSMST_END;
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        return;
    }

    switch ( EState ) {
      case EFSMST_BEGIN:
        if ( kind == EFSMIN_But1Pressed ||
             kind == EFSMIN_But1Pressed ) {
            efsmpts_pos_push(fsm,x,y);
            vgui_efsm_runTreatment(&fsm->any);
            efsmpts_clear(fsm);
        }
        return;

      default: // can not happen
        TEMP_spe_printf("unexpected state %d.",EState);
        return;
    }
}

/*======================================================================*/
/*= exactly N points (nb>1)                                            =*/
/*=    button 1 or 2 for selecting points                              =*/
/*=    button 3 to abort                                               =*/

static void efsmPTS_Nexact(Tvgui_efsm* _a, Tvgui_efsm_in kind, int x, int y)
{
    Tvgui_efsmPTS* fsm  = (Tvgui_efsmPTS*)_a;
    DBG_ENTER(fsm,kind,x,y);
    if ( kind==EFSMIN_Init ) {
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        return;
    }
    if ( kind==EFSMIN_Quit ) {
        efsmpts_draft_erase(fsm);
        efsmpts_clear(fsm);
        EState = EFSMST_END;
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        efsmpts_draft_redraw(fsm);
        return;
    }

    switch ( EState ) {
      case EFSMST_BEGIN:
        if ( kind == EFSMIN_But1Pressed ||
             kind == EFSMIN_But2Pressed ) {
            od_ds_xorDrawStart_0(fsm->fsm_outdevice);
            od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
            efsmpts_pos_push(fsm,x,y);
            EState = EFSMST_NewPos;
            fsm->firstClick= kind == EFSMIN_But1Pressed ? 1 : 2;
        }
        return;
          
      case EFSMST_NewPos:
        if ( kind == EFSMIN_But1Pressed ||
             kind == EFSMIN_But2Pressed ) {
            if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                efsmpts_prev_draw(fsm,0);
                efsmpts_eraseSc(fsm);
                efsmpts_pos_push(fsm,x,y);
                efsmpts_drawSlc(fsm);
                efsmpts_prev_draw(fsm,1);
            } else printf("ST_NewPos:BUT2: ALREADY added (a).\n");
            if ( fsm->nb == dble2set_NB(fsm->points) ) {
                // at least nb and we have them
                efsmpts_draft_erase(fsm);
                vgui_efsm_runTreatment(&fsm->any);
                EState = EFSMST_BEGIN;
                efsmpts_clear(fsm);
                od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
            }
        } else if ( kind == EFSMIN_But3Pressed ) { // abort
            efsmpts_draft_erase(fsm);
            EState = EFSMST_BEGIN;
            efsmpts_clear(fsm);
        } else if ( kind == EFSMIN_MouseMotion ) {
            if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                efsmpts_prev_draw(fsm,0);
                efsmpts_eraseSc(fsm);
                efsmpts_pos_push(fsm,x,y);
                efsmpts_drawSlc(fsm);
                efsmpts_prev_draw(fsm,1);
                EState = EFSMST_TmpPos;
            } else printf("ST_NewPos:MOVE: ALREADY added (b).\n");
        }
        return;

      case EFSMST_TmpPos:
        if ( kind == EFSMIN_But1Pressed ||
             kind == EFSMIN_But2Pressed ) {
            if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                efsmpts_prev_draw(fsm,0);
                efsmpts_eraseSc(fsm);
                efsmpts_pos_push(fsm,x,y);
                efsmpts_drawSlc(fsm);
                efsmpts_prev_draw(fsm,1);
            } else printf("ST_TmpPos:BUT2: ALREADY added (c).\n");
            if ( fsm->nb == dble2set_NB(fsm->points) ) {
                // at least nb points and we have them
                efsmpts_draft_erase(fsm);
                vgui_efsm_runTreatment(&fsm->any);
                EState = EFSMST_BEGIN;
                efsmpts_clear(fsm);
                od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
            } else
                EState = EFSMST_NewPos;
        } else if ( kind == EFSMIN_But3Pressed ) { // abort
            efsmpts_draft_erase(fsm);
            EState = EFSMST_BEGIN;
            efsmpts_clear(fsm);
        } else if ( kind == EFSMIN_MouseMotion ) {
            if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                efsmpts_prev_draw(fsm,0);
                efsmpts_eraseSlc(fsm);
                efsmpts_pos_poppush(fsm,x,y);
                efsmpts_drawSlc(fsm);
                efsmpts_prev_draw(fsm,1);
            } else printf("ST_TmpPos:MOVE: ALREADY added (d).\n");
        }
        return;

      default: // can not happen
        TEMP_spe_printf("unexpected state %d.",EState);
        return;
    }
}

extern Tvgui_efsmPTS* vgui_efsmPTS_new_Nexact (
        int nbPoint, Twid_canvas* dev,
        Tvgui_efsm_treatFunc      treatment,
        Tvgui_efsmPTS_drawSetFunc preview,
        void*                     ud)
{
    Tvgui_efsmPTS* ret;

    if ( nbPoint==1 ) {
        ret = (Tvgui_efsmPTS*)vgui_efsm_new( sizeof(*ret), dev,
                efsmPTS_free,treatment, efsmPTS_1exact,ud);
    } else if ( nbPoint>1 ) {
        ret = (Tvgui_efsmPTS*)vgui_efsm_new( sizeof(*ret), dev,
                efsmPTS_free,treatment,efsmPTS_Nexact,ud);
    } else 
        TEMP_spf_printf(
           "%d is an invalid number of points (expected > 0).", nbPoint);
    ret->skeletonGcd  = OD_GC_Squeleton;
    ret->nb           = nbPoint;
    ret->drawSetFunc  = preview;
    ret->firstClick = 0;
    ret->points     = dble2set_new(20);
    return ret;
}

/*======================================================================*/
/*= 2 or more points                                                   =*/

// at least N points (N>=2):
//     button 1 for selecting points,
//     button 2 for the last point (disable before N)
//     button 3 to abort 
static void efsmPTS_NorMore(Tvgui_efsm* _a,
        Tvgui_efsm_in kind, int x, int y)
{
    Tvgui_efsmPTS* fsm  = (Tvgui_efsmPTS*)_a;
    DBG_ENTER(fsm,kind,x,y);

    if ( kind==EFSMIN_Init ) {
        od_ds_xorDrawStart_0(fsm->fsm_outdevice);
        od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        return;
    }
    if ( kind==EFSMIN_Quit ) {
        efsmpts_draft_erase(fsm);
        efsmpts_clear(fsm);
        EState = EFSMST_END;
        return;
    }
    if ( kind==EFSMIN_Redraw ) {
        efsmpts_draft_redraw(fsm);
        return;
    }

    switch ( EState ) {
        case EFSMST_BEGIN:
            if ( kind == EFSMIN_But1Pressed ) {
                od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                fsm->firstClick=1;
                efsmpts_pos_push(fsm,x,y);
                EState = EFSMST_NewPos;
            } else if ( kind == EFSMIN_But2Pressed ) {
                fsm->firstClick=2;
                efsmpts_pos_push(fsm,x,y);
                EState = EFSMST_HandFree;
            }
            return;
            
        case EFSMST_NewPos:
            if ( kind == EFSMIN_But1Pressed ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_prev_draw(fsm,0);
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                    efsmpts_prev_draw(fsm,1);
                } else printf("ST_NewPos:BUT1: ALREADY added (e).\n");
            } else if ( kind == EFSMIN_But2Pressed ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                } else printf("ST_NewPos:BUT2: ALREADY added (f).\n");
                if ( fsm->nb <= dble2set_NB(fsm->points) ) {
                    // at least nb and we have them
                    efsmpts_draft_erase(fsm);
                    vgui_efsm_runTreatment(&fsm->any);
                    EState = EFSMST_BEGIN;
                    efsmpts_clear(fsm);
    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                }
            } else if ( kind == EFSMIN_But3Pressed ) { // abort
                efsmpts_draft_erase(fsm);
                EState = EFSMST_BEGIN;
                efsmpts_clear(fsm);
            } else if ( kind == EFSMIN_MouseMotion ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_prev_draw(fsm,0);
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                    efsmpts_prev_draw(fsm,1);
                    EState = EFSMST_TmpPos;
                } else printf("ST_NewPos:MOVE: ALREADY added (g).\n");
            }
            return;

        case EFSMST_TmpPos:
            if ( kind == EFSMIN_But1Pressed ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_prev_draw(fsm,0);
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                    efsmpts_prev_draw(fsm,1);
                } else printf("ST_TmpPos:BUT1: ALREADY added (h).\n");
                EState = EFSMST_NewPos;
            } else if ( kind == EFSMIN_But2Pressed ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                } else printf("ST_TmpPos:BUT2: ALREADY added (i).\n");
                if ( fsm->nb <= dble2set_NB(fsm->points) ) {
                    // at least nb points and we have them
                    efsmpts_draft_erase(fsm);
                    vgui_efsm_runTreatment(&fsm->any);
                    EState = EFSMST_BEGIN;
                    efsmpts_clear(fsm);
    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                } else
                    EState = EFSMST_NewPos;
            } else if ( kind == EFSMIN_But3Pressed ) { // abort
                efsmpts_draft_erase(fsm);
                EState = EFSMST_BEGIN;
                efsmpts_clear(fsm);
            } else if ( kind == EFSMIN_MouseMotion ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_prev_draw(fsm,0);
                    efsmpts_eraseSlc(fsm);
                    efsmpts_pos_poppush(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                    efsmpts_prev_draw(fsm,1);
                } else printf("ST_TmpPos:MOVE: ALREADY added (j).\n");
            }
            return;

        case EFSMST_HandFree:
            if ( kind == EFSMIN_But1Pressed ||
                 kind == EFSMIN_But2Pressed ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                    efsmpts_eraseSc(fsm);
                    efsmpts_pos_push(fsm,x,y);
                    efsmpts_drawSlc(fsm);
                } else printf("ST_HandFree:BUT1/2: ALREADY added (k).\n");
                if ( fsm->nb <= dble2set_NB(fsm->points) ) {
                    // at least nb points and we have them
                    efsmpts_draft_erase(fsm);
                    vgui_efsm_runTreatment(&fsm->any);
                    EState = EFSMST_BEGIN;
                    efsmpts_clear(fsm);
                    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
                    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
                }
            } else if ( kind == EFSMIN_But3Pressed ) { // abort
                efsmpts_draft_erase(fsm);
                EState = EFSMST_BEGIN;
                efsmpts_clear(fsm);
            } else if ( kind == EFSMIN_MouseMotion ) {
                if ( !efsmpts_pos_lastEqXY(fsm,x,y) ) {
                efsmpts_eraseSc(fsm);
                efsmpts_pos_push(fsm,x,y);
                efsmpts_drawSlc(fsm);
                } else printf("ST_HandFree:Mouse: ALREADY added (l).\n");
            }
            return;

        default: // can not happen
            TEMP_spe_printf("unexpected state %d.",EState);
            return;
    }
}

extern Tvgui_efsmPTS* vgui_efsmPTS_new_NorMore (
        int nbPoint, Twid_canvas* dev,
        Tvgui_efsm_treatFunc      treatment,
        Tvgui_efsmPTS_drawSetFunc preview,
        void*                     ud)
{
    Tvgui_efsmPTS* ret;
    if ( nbPoint>=2 ) {
        ret = (Tvgui_efsmPTS*)vgui_efsm_new( sizeof(*ret), dev,
            efsmPTS_free,treatment,efsmPTS_NorMore,ud);
    } else 
        TEMP_spf_printf(
           "%d is an invalid number of points (expected >= 2).", nbPoint);
    ret->skeletonGcd  = OD_GC_Squeleton;
    ret->nb           = nbPoint;
    ret->drawSetFunc  = preview;
    ret->firstClick   = 0;
    ret->points       = dble2set_new(100);
    return ret;
}

/*======================================================================*/
