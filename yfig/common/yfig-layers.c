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
 *      $Id: yfig-layers.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-layers.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the layer panel.                                       =*/
/*=                                                                    =*/
/*= It is the panel that allows the user to enable/disable layers.     =*/
/*=    - If the X layer is enabled, the objects with the X depth       =*/
/*=      are drawn on the canvas.                                      =*/                  
/*=    - If the X layer is disabled, the objects with the X depth      =*/
/*=      are not drawn on the canvas.                                  =*/                  
/*=                                                                    =*/
/*= The layout is a widget column:                                     =*/
/*=       Depth    (label title)                                       =*/
/*=       All On   (button to turn all the layers enable)              =*/
/*=       All Off  (button to turn all the layers disable)             =*/
/*=       All Tog  (button to toogle all the layers) i                 =*/
/*=       Dis-mode (DDL that indicates how disabled object are drawn)  =*/
/*=    SB layer-0  (check box for layer ...)                           =*/
/*=    SB layer-1  (check box for layer ...)                           =*/
/*=    SB layer-2  (check box for layer ...)                           =*/
/*=    SB ...                                                          =*/
/*=    SB layer-N  (check box for layer ...)                           =*/
/*= Only the layer that have corresponding object are shown.           =*/
/*= The SB scroll-bar is optional                                      =*/
/*=                                                                    =*/
/*= The widget is purely abstract, see the "yfig-mw.h" header for      =*/
/*= the iinterface documentation.                                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

#include "fig/fig-scene.h"
#include "fig/object.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tydw_layers {
    Tvgui_dw          any;
    Twid_button*      dw_allon;
    Twid_button*      dw_alloff;
    Twid_button*      dw_alltoggle;
  //Twidm_ddl*        dw_mode;
    Twid_buttonPanel* dw_layers;
#define dw_btnShToMinS dw_layers->bp_btns[0]
#define dw_btnShToMinB dw_layers->bp_btns[1]
#define DW_btnCBfirst  2
//#define DW_btnCBnb     VGUI_BP_BoutonNbMax-4 // maximum
#define DW_btnCBnb     40 // for debuging
//#define DW_btnCBnb       (VGUI_BP_BoutonNbMax-10) // secure
#define dw_btnShToMaxB dw_layers->bp_btns[DW_btnCBfirst+DW_btnCBnb]
#define dw_btnShToMaxS dw_layers->bp_btns[DW_btnCBfirst+DW_btnCBnb+1]
    Tscdepth    dw_depths[FIG_MAX_DEPTH]; // depths of figure
    int         dw_depthnb;               // # depths in dw_depths
    int         dw_depthfirst;  // index in dw_depth that is mapped
                                // in the first button of dw_layers
};

typedef struct _Tydw_layers Tydw_layers;
typedef const   Tydw_layers Cydw_layers;

/*======================================================================*/

static void  layers_cb_btnOn   (Tvgui_dw*vmw, int btnId, void*ud);
static void  layers_cb_btnOff  (Tvgui_dw*vmw, int btnId, void*ud);
static void  layers_cb_btnTog  (Tvgui_dw*vmw, int btnId, void*ud);
static void  layers_cb_btnShift(Tvgui_dw*vmw, int btnId, void*ud);
static void  layers_cb_btnDepth(Tvgui_dw*vmw, int btnId, void*ud);

/**
 * The layers_act_updateDepthFirst function sets the dw_depthfirst
 * field to a valid value. This valid value is computed to be as close
 * as possible to the initial dw_depthfirst value.
 *
 * The layers_act_updateBtns function updates the dw_layers button panel 
 * using the dw_depths, dw_depthnb and dw_depthfirst atrributes.
 * Those must be valid, no check is performed.
 *
 * The layers_act_updateDepths function sets the dw_depths/dw_depthnb
 * array with the layers that are used obj object.
 *
o * The layers_act_updateDepthsTopObj function performs
 *    layers_act_updateDepths(self,obj)
 * obj being the root dir of the figure that is displayed by the main
 * window of the self widget.
 * The layers_act_updateDepthsCurObj function performs
 *    layers_act_updateDepths(self,obj)
 * obj being the object that is currently displayed by the main window
 * of the self widget.
 *
 * Returns:
 * The layers_act_updateDepths/updateDepthsTopObj/updateDepthsCurObj
 * function return 1 when the dw_depths/dw_depthnb array changed otherwise
 * they return 0.
**/
static void layers_act_updateDepthFirst  (Tydw_layers* self);
static void layers_act_updateBtns        (Tydw_layers* self);
static int  layers_act_updateDepths      (Tydw_layers* self, TobjAll obj);
static int  layers_act_updateDepthsTopObj(Tydw_layers* self);
static int  layers_act_updateDepthsCurObj(Tydw_layers* self);

/*======================================================================*/
/*= Interface                                                          =*/

extern Tvgui_dw* yfig_layers_nbf(Tyfig_mw* ymw, Tegs_widget pw)
{
    char* btn_width = "M MMMMM";
    int i;
    Twid_button* btn;

    // top-dialog creation
    Tydw_layers* self = (Tydw_layers*) vguiM_dw_newTop(sizeof(*self),
                                        &ymw->any,NULL);

    // set title
    vgui_dw_titleSet(SelfA,NULL,"Depths");

    // add widgets
    self->dw_allon     = widD_button_std_nbfLab3(SelfA,
                           "Enable all the depths",
                           layers_cb_btnOn,self,NULL,
                           "All On", btn_width);
    self->dw_alloff    = widD_button_std_nbfLab3(SelfA,
                           "Disable all the depths",
                           layers_cb_btnOff,self,NULL,
                           "All Off", btn_width);
    self->dw_alltoggle = widD_button_std_nbfLab3(SelfA,
                           "Toggle displayed/hidden depths",
                           layers_cb_btnTog,self,NULL,
                           "Toggle", btn_width);
    self->dw_layers    = widD_buttonPanel_newRnC1(SelfA,0,0);
    btn =  widD_button_std_newLab3(SelfA,
                           "Shift shown depths about of 1/3",
                           layers_cb_btnShift,self,NULL,
                           "more +", btn_width);
    btn->but_uid  = -(DW_btnCBnb+2)/3;
    wid_buttonPanel_addButton(self->dw_layers,btn);
    btn =  widD_button_std_newLab3(SelfA,
                           "Shift shown depths of 1 depth",
                           layers_cb_btnShift,self,NULL,
                           "more", btn_width);
    btn->but_uid  = -1;
    wid_buttonPanel_addButton(self->dw_layers,btn);
    for ( i=0 ; i<DW_btnCBnb ; i+=1) {
        btn = widD_button_check_newLab(SelfA,
                           NULL,
                           layers_cb_btnDepth,NULL,NULL,
                           "_999", btn_width);
                           //NULL, "999");
        btn->btn_option |= WID_BTNOPT_NHBP;
        btn->but_ud   = btn;
        btn->but_uid  = -1;
        wid_buttonPanel_addButton(self->dw_layers,btn);
    }
    btn =  widD_button_std_newLab3(SelfA,
                           "Shift shown depths of 1 depth",
                           layers_cb_btnShift,self,NULL,
                           "more", btn_width);
    btn->but_uid  = +1;
    wid_buttonPanel_addButton(self->dw_layers,btn);
    btn =  widD_button_std_newLab3(SelfA,
                           "Shift shown depths about of 1/3",
                           layers_cb_btnShift,self,NULL,
                           "more +", btn_width);
    btn->but_uid  = +(DW_btnCBnb+2)/3;
    wid_buttonPanel_addButton(self->dw_layers,btn);

    self->dw_layers->bp_scroll = 3; // permanent scrollbar
    self->dw_layers->bp_buttonHVsep = 3; 
    wid_buttonPanel_dbf(self->dw_layers,NULL);

    // layout
    vgui_dw_finishIC(SelfA,
            VDWLF_EH  ,self->dw_allon    ->but_widEGS,
            VDWLF_EH  ,self->dw_alloff   ->but_widEGS,
            VDWLF_EH  ,self->dw_alltoggle->but_widEGS,
            VDWLF_EV  ,self->dw_layers   ->bp_widEGSext,
    -1);

    return SelfA;
}

extern void yfig_layers_delete (Tyfig_mw* ymw, Tvgui_dw* dw)
{
    Tydw_layers* self = (Tydw_layers*)dw;
    wid_button_delete( self->dw_allon );
    wid_button_delete( self->dw_alloff );
    wid_button_delete( self->dw_alltoggle );
  //widm_ddl_free    ( self->dw_mode );
    wid_buttonPanel_free( self->dw_layers );
    vgui_dw_delete( SelfA );
}

extern void yfig_layers_update(Tyfig_mw*ymw)
{
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tydw_layers* self   = (Tydw_layers*)ymw->ymw_layers;

    if ( layers_act_updateDepthsCurObj(self)==1 )
        // dw_depths/dw_depthnb array did not change
        return;
    layers_act_updateDepthFirst(self);
    layers_act_updateBtns(self);
}

extern void yfig_layers_updateFull(Tyfig_mw*ymw)
{
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tydw_layers* self   = (Tydw_layers*)ymw->ymw_layers;

    layers_act_updateDepthsCurObj(self);
    layers_act_updateDepthFirst(self);
    layers_act_updateBtns(self);
}

extern void yfig_layers_updateAddObj(Tyfig_mw*ymw, Tobject obj)
{
    // not optimized, we do not use obj
    yfig_layers_update(ymw);
}

extern void yfig_layers_updateSupObj(Tyfig_mw*ymw, Tobject obj)
{
    // not optimized, we do not use obj
    yfig_layers_update(ymw);
}

/*======================================================================*/
/*= Action                                                             =*/

static void layers_act_updateDepthFirst  (Tydw_layers* self)
{
    int newdepthfirst = self->dw_depthfirst;
    if ( (newdepthfirst+DW_btnCBnb)>=self->dw_depthnb )
        newdepthfirst=self->dw_depthnb-DW_btnCBnb;
    if ( newdepthfirst<0 )
        newdepthfirst=0;
    self->dw_depthfirst = newdepthfirst;
}

static void layers_act_updateBtns(Tydw_layers* self)
{
    int i,currdepth,newdepth;
    Twid_button* btn;
    Tyfig_mw*    ymw      = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tfig_scene*  scene    = yfig_cursce_scene(ymw);
    Tscdepth*    scdepths = scene->sc_depths.table;

    /* update button of active layers */
    for (i=0 ; i<DW_btnCBnb && i<self->dw_depthnb; i++ ) {
        btn    = self->dw_layers->bp_btns[DW_btnCBfirst+i];
        currdepth  = btn->but_uid;
        newdepth   = self->dw_depths[self->dw_depthfirst+i];
        if ( currdepth!=newdepth ) {
            char tmp[100];
            btn->but_uid = newdepth;
            sprintf(tmp," %3d ",newdepth);
            wid_button_chgText(btn,tmp);
        }
        wid_button_show( btn );
        if ( scdepths[newdepth]>0 )
            wid_button_setOn( btn );
        else
            wid_button_setOff( btn );
    }

    /* unmap other buttons if any. */
    for (i=i; i<DW_btnCBnb ; i++ ) {
        btn    = self->dw_layers->bp_btns[DW_btnCBfirst+i];
        wid_button_unshow(btn);
        btn->but_uid = -1;
    }

    /* handle the more buttons. */
    Twid_button* btnN = self->dw_layers->bp_btns[0];
    Twid_button* btn1 = self->dw_layers->bp_btns[1];
    if ( self->dw_depthfirst==0 ) {
        wid_button_unshow(btn1); 
        wid_button_unshow(btnN); 
    } else if ( self->dw_depthfirst<3 ) {
        wid_button_show  (btn1); 
        wid_button_unshow(btnN); 
    } else {
        wid_button_show  (btn1); 
        wid_button_show  (btnN); 
    }
    btn1 = self->dw_layers->bp_btns[DW_btnCBfirst+DW_btnCBnb];
    btnN = self->dw_layers->bp_btns[DW_btnCBfirst+DW_btnCBnb+1];
    int nafter = self->dw_depthnb-(self->dw_depthfirst+DW_btnCBnb);
    if ( nafter<=0 ) {
        wid_button_unshow(btn1); 
        wid_button_unshow(btnN); 
    } else if ( nafter<3 ) {
        wid_button_show  (btn1); 
        wid_button_unshow(btnN); 
    } else {
        wid_button_show  (btn1); 
        wid_button_show  (btnN); 
    }
}

static int layers_act_updateDepths      (Tydw_layers* self, TobjAll obj)
{
    int same = 1;

    // get the depths of the obj object
    int newdepths[FIG_MAX_DEPTH]; 
    int newnb = obj_usedDepths(obj.dir,newdepths);

    // compare the dw_depths/dw_depthnb and newdepths/newnb arrays
    if ( newnb!=self->dw_depthnb )
        same=0;
    else 
        same = memcmp(self->dw_depths,newdepths,newnb*sizeof(newdepths[0]))==0;

    // update  the dw_depths/dw_depthnb array if needed.
    if ( ! same ) {
        self->dw_depthnb = newnb;
        memcpy(self->dw_depths,newdepths,newnb*sizeof(newdepths[0]))==0;
    }
    return same;
}

static int layers_act_updateDepthsTopObj(Tydw_layers* self)
{
    Tyfig_mw*     ymw   = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    TobjDir       dir   = yfig_cursce_topDir(ymw);
    TobjAll       obj   = obj_ObjAll(dir);
    return layers_act_updateDepths(self,obj);
}

static int layers_act_updateDepthsCurObj(Tydw_layers* self)
{
    Tyfig_mw*     ymw   = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    TobjAll       obj   = yfig_cursce_curDir(ymw);
    return layers_act_updateDepths(self,obj);
}

/*======================================================================*/
/*= Callback                                                           =*/

static void  layers_cb_btnOn (Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_layers* self   = (Tydw_layers*)vdw;
    Tyfig_mw*    ymw    = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tscdepth*    depths = scene->sc_depths.table;
    Twid_button* btn;
    int i,depth;

    for (i=0 ; i<FIG_MAX_DEPTH ; i++ ) {
        if ( depths[i]<0 ) depths[i] = -depths[i];
    }
    for (i=0 ; i<DW_btnCBnb ; i++ ) {
        btn    = self->dw_layers->bp_btns[DW_btnCBfirst+i];
        depth  = btn->but_uid;
        if ( depths[depth]==0 ) continue; // button is not shown
        wid_button_setOn(btn);
    }
    yfig_cursce_redraw(ymw);
}

static void  layers_cb_btnOff(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_layers* self   = (Tydw_layers*)vdw;
    Tyfig_mw*    ymw    = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tscdepth*    depths = scene->sc_depths.table;
    Twid_button* btn;
    int i,depth;

    for (i=0 ; i<FIG_MAX_DEPTH ; i++ ) {
        if ( depths[i]>0 ) depths[i] = -depths[i];
    }
    for (i=0 ; i<DW_btnCBnb ; i++ ) {
        btn    = self->dw_layers->bp_btns[DW_btnCBfirst+i];
        depth  = btn->but_uid;
        if ( depths[depth]==0 ) continue; // button is not shown
        wid_button_setOff(btn);
    }
    yfig_cursce_redraw(ymw);
}

static void  layers_cb_btnTog(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_layers* self   = (Tydw_layers*)vdw;
    Tyfig_mw*    ymw    = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tscdepth*    depths = scene->sc_depths.table;
    Twid_button* btn;
    int i,depth;

    for (i=0 ; i<FIG_MAX_DEPTH ; i++ ) {
        depths[i] = -depths[i];
    }
    for (i=0 ; i<DW_btnCBnb ; i++ ) {
        btn    = self->dw_layers->bp_btns[DW_btnCBfirst+i];
        depth  = btn->but_uid;
        if ( depths[depth]==0 ) continue; // button is not shown
        if ( depths[depth]<0 )
            wid_button_setOff(btn);
        else
            wid_button_setOn(btn);
    }
    yfig_cursce_redraw(ymw);
}

static void  layers_cb_btnShift(Tvgui_dw*vdw, int shift, void*ud)
{
    Tydw_layers* self   = (Tydw_layers*)vdw;
    int olddepthfirst = self->dw_depthfirst;
    self->dw_depthfirst += shift;
    layers_act_updateDepthFirst(self);
    if ( olddepthfirst==self->dw_depthnb )
        return;
    layers_act_updateBtns(self);
}

static void  layers_cb_btnDepth(Tvgui_dw*vdw, int btnId, void*ud)
{
    Twid_button* btn    = ud;
    Tydw_layers* self   = (Tydw_layers*)vdw;
    Tyfig_mw*    ymw    = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tfig_scene*  scene  = yfig_cursce_scene(ymw);
    Tscdepth*    depths = scene->sc_depths.table;
    
    depths[btnId] = -depths[btnId];
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/
