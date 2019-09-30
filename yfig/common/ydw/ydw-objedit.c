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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-objedit.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities for the editing dialog.                     =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-dsds.h"
#include "yfig-mw.h"
#include "yfig-device.h"
#include "ydw/ydw-objedit.h"

#include "vgui/wid-filebrowser.h"
#include "vgui/wid-lvu.h"
#include "vgui/wid-button.h"

#include "outdev/string-hl.h"

#include "fig/obj-boxhv.h"
#include "tools/bitmap.h"

/*======================================================================*/
/*=  Interface to Tyoed_state.                                         =*/

// Interface for the creator of edit dialog.
// ydw_objedit_state_restoreInitialFig returns
//   0: the drawing area of ymw is unchanged
//   1: the drawing area of ymw is changed ( yfig_cursce_redraw must be called )

Inline Tyoed_state yoed_state_init(TobjAll obj) {
    Tyoed_state state;
    memset(&state,0,sizeof(state));
    state.oorg     = obj;
    state.preview = 1;
    return state;
}

Inline void yoed_state_free (Tyoed_state*state, Tyfig_mw* ymw) {
    if ( OBJ_IsDefined(state->onew) ) obj_dir_delObjQ( state->onew );
    if ( OBJ_IsDefined(state->odrw) ) obj_dir_delObjQ( state->odrw );
    if ( OBJ_IsDefined(state->opvw) ) obj_dir_delObjQ( state->opvw );
}

static int  yoed_state_restoreInitialFig(Tyoed_state*state, Tyfig_mw* ymw) {
    DBG_YOES(state,0);
    if ( OBJ_IsDefined(state->odrw) ) {
        obj_dir_remObjQ( state->odrw );
        obj_dir_addObj( fsod_getCurrObj(YMW_ydev(ymw)), state->oorg);
        DBG_YOES(state,1);
        return 1;
    } else if ( state->preview==1 && OBJ_IsDefined(state->opvw) ) {
        Toutdev* od = fsod_getOD( YMW_ydev(ymw) );
        od_ds_xorEraseStart_1( od );
          yfig_cursce_draw_obj(ymw,state->opvw.any,OD_GC_Preview);
        od_ds_xorEraseCommit_1( od );
        DBG_YOES(state,2);
        return 0;
    } else {
        return 0;
    }
}

// Interface for run time

static int yoed_state_draw(Tyoed_any*self)
{
    Tyoed_state*state = &self->dw_state;
    Tyfig_mw*   ymw   =  self->dw_ymw;
    Toutdev*    od    = fsod_getOD( YMW_ydev(ymw) );
    Tod_GCdef   gcd   = OD_GC_Preview;
    if ( OBJ_IsDefined(state->odrw) ) {
        yfig_cursce_redraw(ymw);
        od_ds_xorDrawStart_0( od );
        od_ds_xorDrawCommit_0( od );
    } else if ( OBJ_IsDefined(state->opvw) ) {
        if ( (self->dw_styleMask&(FDP_T_font|FDP_T_pts))!=0 )
            od_str_setGC( od, gcd, self->oed_dpt.stt_font, self->oed_dpt.stt_pts, NULL);
        od_ds_xorDrawStart_1( od );
          yfig_cursce_draw_obj(ymw,state->opvw.any,gcd);
        od_ds_xorDrawCommit_1( od );
    }
}

static void yoed_state_setNewObj(Tyoed_any*self, TobjAll obj)
{
    Tyoed_state*state = &self->dw_state;
    Tyfig_mw*   ymw   =  self->dw_ymw;
    DBG_YOES(state,0);
  /* clear  preview, draw and new object */
    // delete preview obj without forgetting to clear it (if needed)
    if ( OBJ_IsDefined(state->opvw) ) {
        Toutdev* od = fsod_getOD( YMW_ydev(ymw) );
        od_ds_xorEraseStart_1( od );
          yfig_cursce_draw_obj(ymw,state->opvw.any,OD_GC_Preview);
        od_ds_xorEraseCommit_1( od );
        obj_dir_delObjQN( &state->opvw );
    }
    // delete draw obj without forgetting to clear it (if needed)
    if ( OBJ_IsDefined(state->odrw) ) {
        obj_dir_delObjQN( &state->odrw );
    }
    // replace new objectby obj
    if ( OBJ_IsDefined(state->onew) )
        obj_dir_delObjQ ( state->onew );

  /* update  preview, draw and new object */
    state->onew = obj;
    if ( state->preview==1 ) {
        state->opvw = obj_newLazy( state->onew );
    }
    DBG_YOES(state,1);

  /* draw */
    yoed_state_draw(self);
}

/*======================================================================*/
/*= Constructor/destructors of base class of object edit dialogs.      =*/

static void yoed_update_action(Tyoed_any*self);
static void yoed_cb_depth     (Tvgui_dw*vdw, void* ud);
static void yoed_cb_unit      (Tvgui_dw*vdw, void* ud);
static void yoed_cb_pen       (void *ud, Csty_line* dpl);
static void yoed_cb_textpen   (Tydw_tpen*tpen,void *ud);
static void yoed_cb_fill      (void *ud, Csty_fill* dpf);
static void yoed_cb_translate (Twid_grp*grp, void* ud);
static void yoed_cb_file      (Twid_box*box, CstrBrief path);

extern Tyoed_any* yoed_any_new (Twid_anyVtable* vt, Tyfig_mw* ymw, TobjAll obj)
{
    Tyoed_any*  self;
    self =  (Tyoed_any*)wid_tbx_newT( vt, &ymw->any, "Object Edition");

    // initialisation of self data
    self->dw_ymw       = ymw;
    self->dw_xyfmt     = "_99999.99_";
    self->dw_uu        = ymw->ymw_userUnit.uu_id;
    self->dw_state     = yoed_state_init(obj);
    self->dw_kind      = obj_Kind(obj);
    self->dw_type      = obj_SubKind(obj);
    self->oed_depth    = obj.any->OA_depthmin; //obj_Depth(obj);
    Tfdp fdp_dummy;
    self->dw_styleMask = figdp_getFdpRaw(&fdp_dummy,obj); // must not be freed (see doc)

    // creation of widgets
    self->header    = yoed_any_header_nbf(self);
    self->depthUnit = ydw_objedit_du_nbfSub(SelfA,self->dw_uu,0,
                        yoed_cb_depth,yoed_cb_unit,self);

    if ( self->dw_kind==FIG_KD_Boxhv ) {
        CstrBrief path = obj.boxhv->OBHV_img->img_path;
        //CstrBrief dir  = "./";
        self->filebrowser = wid_filebrowser_new(SelfA,1,yoed_cb_file,NULL);
        self->filebrowser->embedded = 0;
        vgui_filebrowser_initSaveFilName(self->filebrowser,path);
        int i,kds[IMGFMT_RasNb];
        for (i=0;i<IMGFMT_RasNb ; i+=1) kds[i] = IMGFMT_RasFirst+i;
        vgui_filebrowser_initFileMaskT(self->filebrowser,kds,IMGFMT_RasNb);
        wid_filebrowser_dbf(self->filebrowser);
    } else {
        if ( (self->dw_styleMask&FDP_T_all)!=0 ) {
            self->oed_dpt = odsty_copyText(obj_StyText(obj));
            self->textpen = ydw_tpen_nbfSub(SelfA,ymw,&self->oed_dpt,self->oed_dir,yoed_cb_textpen, self);
        } //else PERM_spe_printf("%s", "no text style"); 

        if ( (self->dw_styleMask&FDP_L_all)!=0 ) {
            self->oed_dpl = odsty_copyLine(obj_StyLine(obj));
            self->pen = ydw_pen_nbfSub(SelfA,ymw,&self->oed_dpl, yoed_cb_pen, self, 0, obj);
        } //else PERM_spe_printf("%s", "no line style"); 

        if ( (self->dw_styleMask&FDP_F_all)!=0 ) {
            self->oed_dpf = odsty_copyFill(obj_StyFill(obj));
            self->fill = ydw_fill_nbf(SelfA,ymw,&self->oed_dpf, yoed_cb_fill, self);
        } //else PERM_spe_printf("%s", "no fill style"); 
//PERM_spe_printf("tc=%s", DBG_Color2str(self->oed_dpt.stt_color));
//PERM_spe_printf("lc=%s", DBG_Color2str(self->oed_dpl.stl_color));
//PERM_spe_printf("fc=%s", DBG_Color2str(self->oed_dpf.stf_color));
    }

  /* creation of the dialogs for the sub-class parameters */
    if ( oed_vtable(self)->op_size > 0 ) {
        self->objparams  = vgui_dw_newSub(oed_vtable(self)->op_size,SelfA,NULL);
        if ( oed_vtable(self)->op_center ) yoed_any_addCenter( self );
    }
    return self;
}

static void yoed_cb_preview(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tyoed_any*      self = (Tyoed_any*)vdw;
    Twid_button*    btn  = ud;
    self->dw_state.preview = (btn->but_onoff_state==WID_BUT_STATE_Off);
}

static void yoed_cb_apply  (Tvgui_dw*vdw, int btnId, void*ud)
{
    Tyoed_any*   self = (Tyoed_any*)vdw;
    Tyoed_state* state = &self->dw_state;
    Tyfig_mw*    ymw   = self->dw_ymw;

    DBG_YOES(state,0);
    if ( OBJ_IsNone(state->onew) || state->onew.any==state->odrw.any )
        return;
    if ( OBJ_IsDefined(state->odrw) ) {
        obj_dir_delObjQ( state->odrw );
    } else
        obj_dir_remObjQ( state->oorg );
    state->odrw = obj_newLazy( state->onew );
    obj_dir_addObj( fsod_getCurrObj(YMW_ydev(ymw)), state->odrw);
    DBG_YOES(state,1);

    yoed_state_draw(self);
}

extern void       yoed_any_dbf   (Tyoed_any* self)
{
    // add control buttons
    Twid_button* btn;
    btn = vgui_dw_addBtnOnOffLab(SelfA,
           "Enable/Disable drawing a preview of this object"
           " in the main canvas",
           yoed_cb_preview, "Disable preview", "Enable preview");
    btn->btn_option |= WID_BTNOPT_NHBP;
    //btn->but_ud      = ret.preview;
    btn = vgui_dw_addBtnStdLab(SelfA,
           "Draw this object in the main canvas",
            yoed_cb_apply, "Apply", NULL);
    vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_CancelX,NULL);
    vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_Ok,"Record\nchanges");
    
    // layout
#define HeadEGS self->header->D_widEGS
#define DeUnEGS self->depthUnit->D_widEGS
#define TPenEGS self->textpen     ? ydw_tpen_widEGS(self->textpen) : NULL
#define PenEGS  self->pen         ? ydw_pen_widEGS (self->pen)     : NULL
#define FillEGS self->fill        ? ydw_fill_widEGS(self->fill)    : NULL
#define FileEGS self->filebrowser ? self->filebrowser->widEGS      : NULL
#define ParEGS  self->objparams->D_widEGS

#define SEP1 VDWLF_SEP1
#define EH   VDWLF_EH
    vgui_dw_finishIC(SelfA, 
            0   |EH ,HeadEGS, 
            SEP1|EH ,DeUnEGS, 
            SEP1|EH ,TPenEGS, 
            SEP1|EH ,PenEGS, 
            SEP1|EH ,FillEGS, 
            SEP1|EH ,FileEGS, 
            SEP1|EH ,ParEGS, 
            -1);
    yoed_update_action(self);
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void       yoed_any_vt_free(Twid_any*any)
{
  Tyoed_any* self = (Tyoed_any*)any;
                       yoed_any_header_delete ( self->header    );
                       ydw_objedit_du_free    ( self->depthUnit );
  if ( self->textpen ) ydw_tpen_free          ( self->textpen       );
  if ( self->pen )     ydw_pen_free           ( self->pen       );
  if ( self->fill)     ydw_fill_free          ( self->fill      );
  if ( self->filebrowser )
                       wid_filebrowser_dbf    ( self->filebrowser );
  if ( oed_vtable(self)->op_center)
                       wid_grp_deleteN        (&self->center    );
                       vgui_dw_free           ( self->objparams );
                       yoed_state_free        (&self->dw_state , self->dw_ymw );
                       odsty_free             (&self->dw_dp.fdp_style         );
}

Twid_anyVtable yoed_any_vtable = { WID_ANY_SCVT_TBX(sizeof(Tyoed_any),&wid_anyVtable_tbx,yoed_any_vt_free, NULL) };

/*======================================================================*/
/*= Services of object edit dialogs.                                   =*/

extern void yoed_any_addCenter (Tyoed_any* self)
{
    TobjAll obj = yoed_any_currObj(self);
    self->center = wid_iopt_nbf((Tvgui_dw*)self->objparams,"center x/y",
            NULL,yoed_cb_translate,self,
            obj_center(obj), obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
}

extern void    yoed_any_update           (Tyoed_any* self) { yoed_update_action(self); }
static TobjAll yoed_any_newObj           (Tyoed_any* self) {
    Tyfig_mw*   ymw  = self->dw_ymw;
    Tyfig_dsds* dsds = yfig_mw_getDsds(ymw);
    TobjAll     obj  = oed_vtable(self)->newObj( self );
    if ( self->pen ) {
        CarrowDesc* ad;
        double PtsToFu = obj_Ppi(obj)/72.;
        double length; Tsty_fill*  pstf;
        if ( ydw_pen_arrowMode(self->pen,1)!=0 ) {
            ad = ydw_pen_arrowDesc(self->pen,1);
            length = ad->length * PtsToFu;
            void* vt =  obj_vtable[ad->vtidx];
            pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
            if ( ad->abslength )
                obj_arrow_chgAbs(obj,1,vt,pstf,length,ad->angle,ad->thickness);
            else
                obj_arrow_chgRel(obj,1,vt,pstf,length,ad->angle);
            }
        if ( ydw_pen_arrowMode(self->pen,0)!=0 ) {
            ad = ydw_pen_arrowDesc(self->pen,0);
            length = ad->length * PtsToFu;
            void* vt =  obj_vtable[ad->vtidx];
            pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
            if ( ad->abslength ) {
                obj_arrow_chgAbs(obj,0,vt,pstf,length,ad->angle,ad->thickness);
            } else {
                obj_arrow_chgRel(obj,0,vt,pstf,length,ad->angle);
            }
        }
    }
    return obj;
}
extern int    yoed_any_restoreInitialFig(Tyoed_any* self) { return yoed_state_restoreInitialFig(&self->dw_state,self->dw_ymw); }
extern void   yoed_any_setNewObj        (Tyoed_any* self) {
    TobjAll obj = yoed_any_newObj( self );
    if ( self->center!=0 )
        wid_grp_setDblePt( self->center, obj_center(obj) );
    yoed_state_setNewObj( self, obj);
}
extern void   yoed_any_setNewObjObjChg   (Tyoed_any* self) {
    TobjAll obj = yoed_any_newObj( self );
    if ( self->center!=0 )
        wid_grp_setDblePt( self->center, obj_center(obj) );
    yoed_state_setNewObj( self, obj);
    if ( oed_vtable(self)->objChg ) oed_vtable(self)->objChg(self);
}

/*======================================================================*/
/*= Default callbacks of object edit dialogs.                          =*/

static void yoed_any_cb_ilvu_I(Tvgui_dw*vdw, Twid_ilvu*iw)
{
    Tyoed_any* self   = iw->lvu_ud; if ( self->dw_inUpdate ) return;
    yoed_any_setNewObj( self );
}
extern void yoed_any_cb_ilvuD(Tvgui_dw*vdw, Twid_ilvu*iw, double v)    { yoed_any_cb_ilvu_I(vdw,iw); }
extern void yoed_any_cb_ilvuT(Tvgui_dw*vdw, Twid_ilvu*iw, CstrBrief s) { yoed_any_cb_ilvu_I(vdw,iw); }

extern void yoed_any_cb_grp (Twid_grp*grp, void* ud)
{
    Tyoed_any* self   = ud; if ( self->dw_inUpdate ) return;
    yoed_any_setNewObj( self );
}

/*======================================================================*/
/*= Actions of object edit dialogs.                                    =*/

static void yoed_update_action(Tyoed_any*self)
{
    if ( self->dw_inUpdate ) return;
    self->dw_inUpdate += 1;
    ydw_objedit_du_update(self->depthUnit,self->dw_uu,self->oed_depth,-1);
    if ( self->pen ) {
        TstyleMask mask = self->dw_styleMask;
        if ( obj_arrow_IsSupported( yoed_any_currObj(self) ) )
                mask |= FDP_A_beg|FDP_A_end ;
        ydw_pen_setStyleMask(self->pen, mask);
    }
    self->dw_inUpdate -= 1;
}

/*======================================================================*/
/*= Callbacks of object edit dialogs.                                  =*/

static void    yoed_cb_depth     (Tvgui_dw*vdw, void* ud)
{
    Tyoed_any* self  = ud; if ( self->dw_inUpdate ) return;
    self->dw_inUpdate += 1;
    int        depth = ydw_objedit_du_depth1(vdw);
    self->oed_depth = depth;
    TobjAll obj = oed_vtable(self)->newObj( self );
    yoed_any_setNewObj(self);
    self->dw_inUpdate -= 1;
}

static void    yoed_cb_unit      (Tvgui_dw*vdw, void* ud)
{
    Tyoed_any* self  = ud; if ( self->dw_inUpdate ) return;
    self->dw_uu = ydw_objedit_du_unit(self->depthUnit);
    if ( self->center!=0 ) {
        wid_grp_chgUsrUnit( self->center, self->dw_uu );
    }
    if ( oed_vtable(self)->unitChg!=0 ) {
        self->dw_inUpdate += 1;
        oed_vtable(self)->unitChg(self);
        self->dw_inUpdate -= 1;
    }
}

static void    yoed_cb_textpen   (Tydw_tpen*tpen,void *ud)
{
    Tyoed_any* self  = ud; if ( self->dw_inUpdate ) return;
                    self->dw_type = ydw_tpen_dirSTY(tpen);
                    self->oed_dir = ydw_tpen_dirSTY(tpen);
    odsty_upteText(&self->oed_dpt,  ydw_tpen_dpt(tpen));
    yoed_any_setNewObj(self);
}

static void    yoed_cb_pen       (void *ud, Csty_line* dpl)
{
    Tyoed_any* self  = ud; if ( self->dw_inUpdate ) return;
    odsty_upteLine(&self->oed_dpl, dpl);
    yoed_any_setNewObj(self);
}

static void    yoed_cb_fill      (void *ud, Csty_fill* dpf)
{
    Tyoed_any* self  = ud; if ( self->dw_inUpdate ) return;
    odsty_upteFill(&self->oed_dpf, dpf);
    yoed_any_setNewObj(self);
}

static void yoed_cb_translate (Twid_grp*grp, void* ud)
{
    Tyoed_any* self      = ud;
    Tdble2     curCenter = obj_center( yoed_any_currObj(self) );
    Tdble2     newCenter = wid_grp_getDblePt( self->center );
    Tdble2     transvect = dble2_initVab(curCenter,newCenter);
    if ( ! dble2_close0(transvect) ) {
        TobjAll obj = obj_newClone( yoed_any_currObj(self) );
        obj_translation(obj,transvect.x,transvect.y);
        yoed_state_setNewObj(self,obj);
        if ( oed_vtable(self)->objChg ) oed_vtable(self)->objChg(self);
    }
}

static void yoed_cb_file      (Twid_box*box, CstrBrief path)
{
    Tyoed_any* self = (Tyoed_any*)box;
    yoed_any_newObj(self);
}

/*======================================================================*/
