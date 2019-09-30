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
 *      $Id: compound-open.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/compound-open.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= mode panel: open compound object.                                  =*/
/*=                                                                    =*/
/*= This file is included by compound.c                                =*/
/*======================================================================*/

#include "vgui/vgui-dw.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

/*======================================================================*/
/*= Data structucture                                                  =*/

/*= the data for restoring an open compound.                           =*/

typedef struct _TcompoundOpenData {
    TobjAll  dt_objdir;  /* The compound object shows before the open
                          * (the parent of the one presently drawn). */
    TboxDble dt_boxDS;   /* the figure box drawn before the open */
} TcompoundOpenData;

/*= a stack of open compound.                                          =*/

typedef struct _TcompoundOpenStack TcompoundOpenStack;

struct _TcompoundOpenStack {
    int     eleMemNb; // element number of the memory block eles.
    int     eleNb;    // element number of set (eleNb<eleMemNb)
    TcompoundOpenData* eles;     // the set elements
};

static TcompoundOpenStack* compoundOpenStack_new();
static void compoundOpenStack_free(TcompoundOpenStack* s);
static void compoundOpenStack_push(TcompoundOpenStack* s,
                                   CboxDble*box, TobjAll  dir);
static void compoundOpenStack_pop(TcompoundOpenStack* s);
static void compoundOpenStack_popAll(TcompoundOpenStack* s);

/*= the dialog widget that allows to the user to close the open        =*/
/*= compounds. It consists                                             =*/
/*=   - of a TcompoundOpenData stack,                                  =*/
/*=   - of a button to close the top open,                             =*/
/*=   - of a button to close all the open compound.                    =*/

typedef struct _TcompoundOpenDial {
    Tvgui_dw            any;
    TcompoundOpenStack* stack;    
    Tyfig_mw*           ymw;
} TcompoundOpenDial;

static TcompoundOpenDial* compoundOpenDial_nbf (Tyfig_mw*ymw);
static void               compoundOpenDial_free(TcompoundOpenDial*self);

/*======================================================================*/
/*= Interface                                                          =*/

// the functions that open and close a compound.
static void cpdOpen_do(TefsmSel1Obj*fsm, int click,
                                TobjAll obj, Tdble2 marker)
{
    Tyfig_device*  ydev  = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw   = (Tyfig_mw*)     fsm->fsm_userdata;
    Tfig_scene*    scene = fsod_getScene(ydev);

    if ( click==3 ) { return; }

    if ( click==2 ) {
        PERM_spe_printf("click of middle button not yet implemented",0);
        return;
    }

    // if dialog is not present (first open), we create it
    if ( scene->sc_compoundData==0 ) {
        scene->sc_compoundData = compoundOpenDial_nbf(ymw);
    }

    // save the current state in dialog
    TcompoundOpenDial* dialog = scene->sc_compoundData;
    compoundOpenStack_push(dialog->stack,
            fsod_getBoxSrc(ydev), fsod_getCurrObj(ydev));

    // open the compound
    fsod_setCurrObj(  ydev, obj);
    vgui_dw_start(&dialog->any);
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void compoundOpenDial_cb(Tvgui_dw*_d, Twid_dialStatus s, void* ud)
{
    int i;
    TcompoundOpenDial* d     = (TcompoundOpenDial*)_d;
    Tyfig_mw*          ymw   = d->ymw;
    Tvgui_odcanvas*    vdev  = ymw->ymw_canvas->vgcm_outdev;
    Tyfig_device*      ydev  = (Tyfig_device*)vdev;
    Tfig_scene*        scene = fsod_getScene(ydev); //ymw->ymw_scene;
    Twid_buttonPanel*  mp    = ymw->ymw_modePanel;

    // search the open compound button in the mode panel
    Twid_button* button=0;
    for ( i=0 ; i<mp->vgbpm_buttonNb ; i+=1 ) {
        if ( mp->vgbpm_buttons[i]->but_cbMw == mp_cpdOpen_cb ) {
            button= mp->vgbpm_buttons[i];
            break;
        }
    }
    TEMP_asf_printf(button==0,"%s", "no open button found in the mode panel");

    // pop the state from the stack
    TcompoundOpenData state;
    if ( s==WID_DIALSTATUS_Do ) {
        state = d->stack->eles[d->stack->eleNb-1];
        compoundOpenStack_pop(d->stack);
    } else {
        state = d->stack->eles[0];
        compoundOpenStack_popAll(d->stack);
    }
    if ( d->stack->eleNb==0 ) {
        // dialog is no more needed
        compoundOpenDial_free(d);
        scene->sc_compoundData = 0;
    }

    // restore the state:
    fsod_setCurrObj(  ydev, state.dt_objdir);
    od_updateSrcBB_toBox(&vdev->vv_od, &state.dt_boxDS);

    // restore the open compound mode.
    wid_buttonPanel_active(mp,button );
    yfig_modePanel_chgMode(mp,button);
    yfig_cursce_redraw(ymw);
}

static TcompoundOpenDial* compoundOpenDial_nbf(Tyfig_mw*ymw)
{
    TcompoundOpenDial* d;
    d = (TcompoundOpenDial*) vgui_dw_newTop(
            sizeof(*d),&ymw->any,strdup("Close compound"),0);

    d->stack = compoundOpenStack_new();
    d->ymw   = ymw;

    Tvgui_dw* wd = &d->any;

    vgui_dw_addCtlButt(wd,
            WID_DIALSTATUS_Do,"Close current compound");
    vgui_dw_addCtlButt(wd,
            WID_DIALSTATUS_CancelX,"Close all the compounds");

    vgui_dw_finishIC(wd,-1);

    wd->top->dt_cb = compoundOpenDial_cb;

    return d;
}

static void compoundOpenDial_free(TcompoundOpenDial*self)
{
    compoundOpenStack_free( self->stack );
    vgui_dw_free(SelfA);
}

/*======================================================================*/
/*= Compound stack                                                     =*/

static TcompoundOpenStack* compoundOpenStack_new()
{
    TcompoundOpenStack* ret= tool_malloc( sizeof(*ret) );
    ret->eleMemNb  = 5;
    ret->eleNb     = 0;
    ret->eles      = tool_malloc(sizeof(*ret->eles)*ret->eleMemNb);
    return ret;
}

static void compoundOpenStack_free(TcompoundOpenStack* s)
{
    if ( s==0 ) return;
    tool_free(s->eles);
    tool_free(s);
}

static void compoundOpenStack_push(TcompoundOpenStack* s,
                                       CboxDble*box,
                                       TobjAll  dir)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 5;
        s->eles = realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    s->eles[s->eleNb].dt_boxDS  = *box;
    s->eles[s->eleNb].dt_objdir = dir;
    s->eleNb += 1;
}

static void compoundOpenStack_pop(TcompoundOpenStack* s) { s->eleNb -= 1; }

static void compoundOpenStack_popAll(TcompoundOpenStack* s) { s->eleNb = 0; }

/*======================================================================*/
