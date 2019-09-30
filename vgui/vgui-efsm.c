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
 *      $Id: vgui-efsm.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-efsm.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A canvas Event FSM definition.                                     =*/
/*=                                                                    =*/
/*= See the vgui/vgui-efsm.h header for the type and function          =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "../config.h"
#include "vgui/vgui-efsm.h"

#include "vgui-config.h"
#include "vgui/vgui-efsm.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"
#include "tools/grid.h"

/*======================================================================*/

extern Tvgui_efsm* vgui_efsm_new(int bs, Twid_canvas* dev,
                                 Tvgui_efsm_freeFunc  free,
                                 Tvgui_efsm_treatFunc treatment,
                                 Tvgui_efsm_transFunc trans,
                                 void*                ud)
{
    Tvgui_efsm* ret = tool_malloc_0( bs );
    ret->indevice       = dev;
    ret->outdevice      = wid_canvas_getOD(dev);
    ret->freeFunc       = free;
    ret->treatmentFunc  = treatment;
    ret->transitionFunc = trans;
    ret->userdata       = ud;;
    ret->state          = EFSMST_BEGIN;

    return ret;
}

extern void    vgui_efsm_delete (Tvgui_efsm* fsm)
{
    if (fsm==0) return;
    if (fsm->freeFunc) fsm->freeFunc(fsm);
    free(fsm);
}

/*======================================================================*/

extern void vgui_efsm_transition(Tvgui_efsm* fsm,
                                 Tvgui_efsm_in kind, int x, int y)
{
    if ( fsm==0 ) return;

    // for _EnterCanvas and LeaveCanvas we can:
    //    return 0 that is not very good
    //    return 1 that is worst
    // so we call the transitionFunc that knows the return value.
    if ( fsm->state!=EFSMST_END ) {
        if ( kind==EFSMIN_EnterCanvas ) 
            vgui_efsm_mousefun_show(fsm);
        else if ( kind==EFSMIN_LeaveCanvas )
            vgui_efsm_mousefun_clear(fsm);
    }

    if ( fsm->state==EFSMST_END )
        return;

    if ( fsm->locked )
        return;

    if ( fsm->transitionFunc==0 ) return;

    int old_state = fsm->state;
#if 1
    Twid_canvas* canvas = fsm->indevice;
    Tgrid*       grid   = canvas->cvs_grid;
    if ( fsm->magnet && grid_isMagnetEnabled(grid)!=0 ) {
        double xm=x,ym=y;
        xm = GRID_ToMagnet(grid,xm);
        ym = GRID_ToMagnet(grid,ym);
        fsm->transitionFunc(fsm,kind,xm,ym);
    } else 
        fsm->transitionFunc(fsm,kind,x,y);
#else
    fsm->transitionFunc(fsm,kind,x,y);
#endif
    if ( old_state!=fsm->state )
        vgui_efsm_mousefun_show(fsm);
}

extern void vgui_efsm_runTreatment(Tvgui_efsm* fsm)
{
    if ( fsm->treatmentFunc!=0 ) {
        fsm->locked = 1;
        fsm->treatmentFunc(fsm);
        fsm->locked = 0;
    }
}

/*======================================================================*/

extern void vgui_efsm_mousefun_show(Tvgui_efsm* fsm)
{
    wid_mfun_showDt(fsm->mfun,fsm->state);
}

extern void vgui_efsm_mousefun_clear(Tvgui_efsm* fsm)
{
    if ( fsm->mfun==0 || fsm->mfun->widget==0 ) return;
    wid_mfun_show(fsm->mfun->widget,0,0,0);
}

extern void vgui_efsm_mousefun_enable(Tvgui_efsm* fsm,
                                      Twid_mousefun*  w,
                                      Cwid_mousefun3S m[])
{
    if ( fsm->mfun==0 )
        fsm->mfun = tool_malloc_0(sizeof(*fsm->mfun));
    fsm->mfun->widget   = w;
    fsm->mfun->messages = m;
}
extern void vgui_efsm_mousefun_disable(Tvgui_efsm* fsm)
{ tool_free(fsm->mfun); }

/*======================================================================*/
