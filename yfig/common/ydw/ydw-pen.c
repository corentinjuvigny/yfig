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
 *      $Id: ydw-pen.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-pen.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that handles pen parameters.                            =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-pixmap.h"
#include "ydw/ydw-arrow.h"
#include "ydw/ydw.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-button.h"

#include "fig/figure.h"
#include "fig/object.h"

/*======================================================================*/
/*= data structure                                                     =*/

struct _Tydw_pen {
    Tvgui_dw    any;
    Twid_ilvu*  dw_thickness;
    Twid_olvu*  dw_color;
    Tvgui_dw*   dw_color_d;
    Twid_ilvu*  dw_style;
    Twid_ilvu*  dw_join;
    Twid_ilvu*  dw_cap;

    TarrowDesc  dw_arrowbegDesc;
    TarrowDesc  dw_arrowendDesc;
    Twid_ilvu*  dw_arrowmod;
    Twid_ilvu*  dw_arrowbeg;
    Twid_ilvu*  dw_arrowend;
    Tydw_arrow2*dw_arrowParam;
    Twid_button*dw_arrowBtnFuf;

    TstyleMask  dw_styleMask;
    Tsty_line   dw_pen;
    Tydw_pen_cb dw_cb;
    void*       dw_ud;
} ;

/*======================================================================*/
/*= Interface                                                          =*/

// build the pen widget
static void ydw_pen_nbf_I(Tydw_pen*self, 
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_line*dpl, Tydw_pen_cb cb, void*ud,
        int curveonly, TobjAll obj);

// redraw the widget
static void pen_act_update(Tydw_pen* self);

static void pen_cb_thickness(Tvgui_dw* dw, Twid_ilvu* iw, int v);
static void pen_cb_color      (Tvdw_colorSel*cs, Ccolor*c, void*ud);
static void pen_cb_style      (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_join       (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_cap        (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_arrowbeg   (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_arrowend   (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_arrowmod   (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void pen_cb_arrowparams(Tvgui_dw* dw);

extern Tvgui_dw* ydw_pen_nbfTop(
        Tvgui_dw* vdw,
        Tyfig_mw* ymw,
        Csty_line*dpl, Tydw_pen_cb cb, void*ud,
        int curveonly)
{
    char*    title = "Pen configuration";
    Tydw_pen* self = (Tydw_pen*) vgui_dw_newTop(sizeof(*self), &ymw->ymw_vmw,
                            tool_strdup(title),
                            WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok);

    ydw_pen_nbf_I(self,vdw,ymw,dpl,cb,ud,curveonly,OBJ_none);
    ydw_pen_hideClr(self);
    widD_button_fuf_setFolded(self->dw_arrowBtnFuf);
    return SelfA;
}

extern Tydw_pen* ydw_pen_nbfSub(
        Tvgui_dw* vdw,
        Tyfig_mw* ymw,
        Csty_line*dpl, Tydw_pen_cb cb, void*ud,
        int curveonly, TobjAll obj)
{
    char*     title  = "Pen configuration";
    Tydw_pen* self  = (Tydw_pen*) vgui_dw_newSub(sizeof(*self), vdw,
                            tool_strdup(title));
    vgui_dw_titleAddLabTxt(SelfA, "none", YDW_VIEWER_WITH);
    vgui_dw_titleAddFufLabR(SelfA);

    ydw_pen_nbf_I(self,vdw,ymw,dpl,cb,ud,curveonly,obj);

    ydw_pen_hideClr(self);
    widD_button_fuf_setFolded(self->dw_arrowBtnFuf);
    vgui_dw_titleUnFold(SelfA);
    pen_act_update(self);
    return self;
}

extern void       ydw_pen_free(Tydw_pen* self)
{
    wid_ilvu_free( self->dw_thickness );
    wid_olvu_free( self->dw_color );
    ydw_colors_free( self->dw_color_d );
    wid_ilvu_free( self->dw_style );
    wid_ilvu_free( self->dw_join );
    wid_ilvu_free( self->dw_cap );

    wid_ilvu_free( self->dw_arrowmod );
    wid_ilvu_free( self->dw_arrowbeg );
    wid_ilvu_free( self->dw_arrowend );
    ydw_arrow2_delete( self->dw_arrowParam );
    wid_button_delete( self->dw_arrowBtnFuf );
    vgui_dw_free(SelfA);
}

extern Tegs_widget ydw_pen_widEGS    (Tydw_pen* self) { return self->dw_widEGS; }
extern int         ydw_pen_arrowMode (Tydw_pen* self, int atBeg)
{
    int idx = wid_ilvu_getNum10(self->dw_arrowmod);
    if ( idx==3 ) return 1;
    if ( idx==1 && atBeg==0 ) return 1;
    if ( idx==2 && atBeg==1 ) return 1;
    return 0;
}
extern Csty_line*  ydw_pen_dpl       (Tydw_pen* self) { return &self->dw_pen; }
extern CarrowDesc* ydw_pen_arrowDesc (Tydw_pen* self, int atBeg)
{ return atBeg ? &self->dw_arrowbegDesc :  &self->dw_arrowendDesc; }

extern void       ydw_pen_showClr(Tydw_pen* self)
{
    Twid_button* btn = wid_olvu_getUniWid(self->dw_color);
    widD_button_fuf_setUnFolded(btn);
}

extern void       ydw_pen_hideClr(Tydw_pen* self)
{
    Twid_button* btn = wid_olvu_getUniWid(self->dw_color);
    widD_button_fuf_setFolded(btn);
}

extern void        ydw_pen_setStyleMask(Tydw_pen* self, TstyleMask mask)
{
    TstyleMask maskARW = FDP_A_beg|FDP_A_end;
    if ( self->dw_styleMask!=mask ) {
        if ( (mask&FDP_L_join)!=0 ) wid_ilvu_enable( self->dw_join     ); else  wid_ilvu_disable( self->dw_join     );
        if ( (mask&FDP_L_cap )!=0 ) wid_ilvu_enable( self->dw_cap      ); else  wid_ilvu_disable( self->dw_cap      );
        if ( (mask&maskARW   )!=0 ) wid_ilvu_enable( self->dw_arrowmod ); else  wid_ilvu_disable( self->dw_arrowmod );
        if ( (mask&maskARW   )!=0 ) wid_ilvu_enable( self->dw_arrowbeg ); else  wid_ilvu_disable( self->dw_arrowbeg );
        if ( (mask&maskARW   )!=0 ) wid_ilvu_enable( self->dw_arrowend ); else  wid_ilvu_disable( self->dw_arrowend );
        self->dw_styleMask = mask;
    }
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void ydw_pen_initArrowDesc(TarrowDesc*ad,TobjAll ao)
{
    CobjVtable*avtable;
    int abs; double len,angle,thickness; Csty_fill*fill;
    obj_arrow_getParams(ao,(void*)&avtable,&abs,&fill,&len,&angle,&thickness);
    ad->enabled   = 1;
    ad->abslength = abs;
    ad->length    = LUNIT_FuToPt(len);
    ad->angle     = angle;
    ad->vtidx     = obj_Kind(ao);
    switch (  ad->vtidx ) {
        case FIG_KD_ArrowT00 :
            ad->flag = FARROW_FillNone;
            break;
        case FIG_KD_ArrowT01 :
        case FIG_KD_ArrowT02 : {
            CcolorRGB*rgb = &fill->stf_color->rgb;
            TcolorRGB white = { 255,255,255 };
            if ( color_cmpRGB(rgb,&white)==0 ) {
                ad->flag = FARROW_FillWhite;
            } else {
                ad->flag = FARROW_FillParent;
            }
            break;
            }
        default:
            PERM_spe_printf("%d is an unknown object kind (use %d instead)",ad->vtidx,FIG_KD_ArrowT00);
            ad->vtidx = FIG_KD_ArrowT00;
            ad->flag = FARROW_FillNone;
    }
    ad->thickness = ad->abslength ? thickness : 0;
}

static int ydw_pen_getArrowDllIdx(TarrowDesc*ad)
{
    int i;
    TarrowDesc* t = pxm_arrow_ref();
    for (i=0 ; i<pxm_arrow_nbe() ; i+=1) {
        if ( t[i].flag==ad->flag && t[i].vtidx==ad->vtidx ) return i;
    }
    PERM_spe_printf("unexpected arrow description flag=%d kind=%d",ad->flag,ad->vtidx);
    return 0;
}

static void ydw_pen_nbf_I(Tydw_pen*self, 
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_line*dpl, Tydw_pen_cb cb, void*ud,
        int curveonly, TobjAll obj)
{
    int          i;
    Tvgui*       gui        = vdw!=0 ? vgui_dw_getVGui(vdw)
                                     : vgui_mw_getVGui(&ymw->ymw_vmw);
    Twid_button* btn;

    self->dw_pen = *dpl;
    self->dw_cb  = cb;
    self->dw_ud  = ud;

    // thickness
    self->dw_thickness = wid_ilvu_newI(dpl->dpl_thickness, "999.9 ",
                           NULL,pen_cb_thickness,self,
                           FIG_THICKNESS_MIN, FIG_THICKNESS_MAX);
    wid_ilvu_addLabel  (self->dw_thickness, "thickness", YDW_LAB_WITH);
    wid_ilvu_addSpinInc(self->dw_thickness,1);
    widD_ilvu_dbf      (self->dw_thickness, SelfA);

    // widgets for changing color 
    self->dw_color_d = ydw_colors_nbf(SelfA,ymw, NULL,
                                        pen_cb_color,self);
    self->dw_color = wid_olvu_newC(NULL,YDW_VAL_WITH,
                                   dpl->dpl_color,0,0);
    wid_olvu_addLabel  (self->dw_color, "color", YDW_LAB_WITH);
    btn = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    wid_olvu_addButton (self->dw_color, btn);
    widD_olvu_dbf      (self->dw_color, SelfA);
    widD_button_fuf_addWidToIgn(btn,self->dw_color_d->D_widEGS,NULL);

#define GetIdx(ref,nbe,val) \
    for (i=0 ; i<nbe(gui) && ref()[i]!=val ; i++); \
    if ( i==nbe(gui) ) i -=1
    // style
    GetIdx(pxm_lineStyle_ref,pxm_lineStyle_nbe,dpl->dpl_style);
    self->dw_style = wid_ilvu_newDdlPxm(i, 
            pxm_lineStyle_pxm(gui,0,0,NULL), pxm_lineStyle_nbe(gui),
            pxm_lineStyle_dx(),              pxm_lineStyle_dy(),
            (Twid_cb_Uap)pen_cb_style,self);
    wid_ilvu_addLabel  (self->dw_style, "style", YDW_LAB_WITH);
    widD_ilvu_dbf      (self->dw_style, SelfA);

    // join
    GetIdx(pxm_joinStyle_ref,pxm_joinStyle_nbe,dpl->dpl_join);
    self->dw_join = wid_ilvu_newDdlPxm(i,
            pxm_joinStyle_pxm(gui), pxm_joinStyle_nbe(),
            pxm_joinStyle_dx(),     pxm_joinStyle_dy(),
            (Twid_cb_Uap)pen_cb_join,self);
    wid_ilvu_addLabel  (self->dw_join, "join", YDW_LAB_WITH);
    widD_ilvu_dbf      (self->dw_join, SelfA);

    // cap
    GetIdx(pxm_capStyle_ref,pxm_capStyle_nbe,dpl->dpl_cap);
    self->dw_cap = wid_ilvu_newDdlPxm(i,
            pxm_capStyle_pxm(gui), pxm_capStyle_nbe(),
            pxm_capStyle_dx(),     pxm_capStyle_dy(),
            (Twid_cb_Uap)pen_cb_cap,self);
    wid_ilvu_addLabel  (self->dw_cap, "cap", YDW_LAB_WITH);
    widD_ilvu_dbf      (self->dw_cap, SelfA);

    // begin arrow
    TobjAll* arrow;
    self->dw_arrowbegDesc = ymw->ymw_arrowbeg;
    if ( OBJ_IsDefined(obj) && (arrow=obj_arrow_BegPtr(obj))!=0 && OBJ_IsDefined(*arrow) ) 
        ydw_pen_initArrowDesc(&self->dw_arrowbegDesc,*arrow);
    self->dw_arrowbeg =
    wid_ilvu_newDdlPxm(ydw_pen_getArrowDllIdx(&self->dw_arrowbegDesc),
            pxm_arrow_pxm(gui,1), pxm_arrow_nbe(),
            pxm_arrow_dx(),       pxm_arrow_dy(),
            (Twid_cb_Uap)pen_cb_arrowbeg,self);
    wid_ilvu_addLabel   (self->dw_arrowbeg, "arrow at begin", YDW_LAB_WITH);
  //wid_ilvu_dontFreePxm(self->dw_arrowbeg );
    widD_ilvu_dbf       (self->dw_arrowbeg, SelfA);

    // end arrow
    self->dw_arrowendDesc = ymw->ymw_arrowend;
    if ( OBJ_IsDefined(obj) && (arrow=obj_arrow_EndPtr(obj))!=0 && OBJ_IsDefined(*arrow) ) 
        ydw_pen_initArrowDesc(&self->dw_arrowendDesc,*arrow);
    self->dw_arrowend =
    wid_ilvu_newDdlPxm(ydw_pen_getArrowDllIdx(&self->dw_arrowendDesc),
            pxm_arrow_pxm(gui,0), pxm_arrow_nbe(),
            pxm_arrow_dx(),       pxm_arrow_dy(),
            (Twid_cb_Uap)pen_cb_arrowend,self);
    wid_ilvu_addLabel   (self->dw_arrowend, "arrow at end", YDW_LAB_WITH);
  //wid_ilvu_dontFreePxm(self->dw_arrowend );
    widD_ilvu_dbf       (self->dw_arrowend, SelfA);

    // arrow mode
#define AOW_IDX(s) s->dw_arrowendDesc.enabled!=0 \
                     ? (s->dw_arrowbegDesc.enabled!=0 ? 3 : 1) \
                     : (s->dw_arrowbegDesc.enabled!=0 ? 2 : 0)
    self->dw_arrowmod = wid_ilvu_newDdlPxm(AOW_IDX(self), 
            pxm_arrowmode_pxm(gui,4), pxm_arrowmode_nbe(),
            pxm_arrowmode_dx(),     pxm_arrowmode_dy(),
            (Twid_cb_Uap)pen_cb_arrowmod,self);
    wid_ilvu_addLabel   (self->dw_arrowmod, "arrow mode", YDW_LAB_WITH);
    wid_ilvu_dontFreePxm(self->dw_arrowmod );
    widD_ilvu_dbf       (self->dw_arrowmod, SelfA);

    // arrow paramters
    self->dw_arrowParam  = ydw_arrow2_nbfSub(SelfA,ymw,
            &self->dw_arrowbegDesc,&self->dw_arrowendDesc,
            pen_cb_arrowparams, self);
    self->dw_arrowBtnFuf = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    widD_button_fuf_addWidToIgn(self->dw_arrowBtnFuf,self->dw_arrowParam->any.D_widEGS,NULL);

#define ThicknessL self->dw_thickness->lvu_labEGS
#define ThicknessV self->dw_thickness->lvu_valEGS
#define ThicknessU self->dw_thickness->lvu_uniEGS
#define ColorL    self->dw_color->lvu_labEGS
#define ColorV    self->dw_color->lvu_valEGS
#define ColorU    self->dw_color->lvu_uniEGS
#define ColorD    self->dw_color_d->D_widEGS
#define StyleL    self->dw_style->lvu_labEGS
#define StyleV    self->dw_style->lvu_valEGS
#define StyleU    self->dw_style->lvu_uniEGS
#define JoinL     self->dw_join->lvu_labEGS
#define JoinV     self->dw_join->lvu_valEGS
#define JoinU     self->dw_join->lvu_uniEGS
#define CapL      self->dw_cap->lvu_labEGS
#define CapV      self->dw_cap->lvu_valEGS
#define CapU      self->dw_cap->lvu_uniEGS
#define AwbL      self->dw_arrowbeg->lvu_labEGS
#define AwbV      self->dw_arrowbeg->lvu_valEGS
#define AweL      self->dw_arrowend->lvu_labEGS
#define AweV      self->dw_arrowend->lvu_valEGS
#define AwModL    self->dw_arrowmod->lvu_labEGS
#define AwModV    self->dw_arrowmod->lvu_valEGS
#define AwFuf     self->dw_arrowBtnFuf->but_widEGS
#define AwPar     self->dw_arrowParam->any.D_widEGS

#define LN(x) VDWLF_ROW(x)
    vgui_dw_finishMTX(SelfA
     ,LN(0)   ,0,ThicknessL       ,1,ThicknessV  //,2,ThicknessU
     ,LN(1)   ,0,ColorL           ,1,ColorV     ,2,ColorU
     ,LN(2)   ,0|VDWLF_EH,ColorD  ,1,ColorD     ,2|VDWLF_EH,ColorD  ,3,ColorD ,4,ColorD
     ,LN(3)   ,0,StyleL           ,1,StyleV                         ,3,AwModV ,4,AwModL
     ,LN(4)   ,0,JoinL            ,1,JoinV                          ,3,CapV   ,4,CapL
     ,LN(5)   ,0,AwbL             ,1,AwbV       ,2,AwFuf            ,3,AweV   ,4,AweL  
     ,LN(6)   ,0,AwPar            ,1,AwPar      ,2,AwPar            ,3,AwPar  ,4,AwPar
     ,-1);
}

static void pen_act_update(Tydw_pen* self)
{
    Tvgui*       gui     = self->dw_gui;
    Tsty_line*   pen     = &self->dw_pen;
    Twid_olvu*   cviewer = self->dw_color;

    wid_olvu_setBgClr    (cviewer,pen->dpl_color);
    TEMP_asf_printf(pen->dpl_style==DA_LS_None,
            "invalid line style %d (DA_LS_None)",DA_LS_None);
    if ( self->dw_title ) {
        Tegs_widget  mlabel  = vgui_dw_titleLab(SelfA);
        if ( pen->dpl_style==DA_LS_NoLine ) {
            Ccolor *mlabelBg = vgui_wn_getBgColor(gui, wn_dialLabel);
            wid_label_setBgColor(mlabel,mlabelBg);
            wid_label_setText   (mlabel,"none");
        } else {
            int thickness_save = pen->dpl_thickness;
            if ( pen->dpl_thickness>10 ) 
                pen->dpl_thickness = 10;
            wid_label_setBgColor (mlabel,pen->dpl_color);
            wid_label_setText    (mlabel,"");
            pen->dpl_thickness = thickness_save;
        }
    }
}

/*======================================================================*/
/*= Callback                                                           =*/

#define CALL_CB() if ( self->dw_cb ) self->dw_cb(self->dw_ud,&self->dw_pen)

static void pen_cb_thickness(Tvgui_dw* dw, Twid_ilvu* iw, int v)
{
    Tydw_pen* self = (Tydw_pen*)dw;
    if ( self->dw_pen.dpl_thickness != v ) {
        self->dw_pen.dpl_thickness = v;
        CALL_CB();
    }
}

static void pen_cb_color(Tvdw_colorSel*cs, Ccolor*c, void*ud)
{
    Tydw_pen* self = (Tydw_pen*)ud;
    if ( color_cmp(self->dw_pen.dpl_color,c)!=0 ) {
        self->dw_pen.dpl_color = (Tcolor*)c;
        pen_act_update(self);
        CALL_CB();
    }
}

//static void pen_cb_style(void* ud, ...);
static void pen_cb_style(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen* self = (Tydw_pen*)dw;
    int       idx  = v;
    TlineStyle ls  =  pxm_lineStyle_ref()[idx];
    if ( self->dw_pen.dpl_style != ls ) {
        self->dw_pen.dpl_style = ls;
        CALL_CB();
    }
}

static void pen_cb_join (Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen*  self = (Tydw_pen*)dw;
    int        idx  = v;
    TjoinStyle js  =  pxm_joinStyle_ref()[idx];
    if ( self->dw_pen.dpl_join != js ) {
        self->dw_pen.dpl_join = js;
        CALL_CB();
    }
}

static void pen_cb_cap  (Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen* self = (Tydw_pen*)dw;
    int       idx  = v;
    TcapStyle cs  =  pxm_capStyle_ref()[idx];
    if ( self->dw_pen.dpl_cap != cs ) {
        self->dw_pen.dpl_cap = cs;
        CALL_CB();
    }
}

static void pen_cb_arrowmod(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen* self = (Tydw_pen*)dw;
    int       idx  = v;
    int       enabledBeg = (idx&2)!=0;
    int       enabledEnd = (idx&1)!=0;
    if ( self->dw_arrowbegDesc.enabled != enabledBeg ||
         self->dw_arrowendDesc.enabled != enabledEnd ) {
        self->dw_arrowbegDesc.enabled = enabledBeg ;
        self->dw_arrowendDesc.enabled = enabledEnd;
        CALL_CB();
    }
}

static void pen_cb_arrowbeg(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen*   self = (Tydw_pen*)dw;
    int         idx  = v;
    TarrowDesc* ad   = pxm_arrow_ref()+idx;
    if ( self->dw_arrowbegDesc.vtidx != ad->vtidx ||
         self->dw_arrowbegDesc.flag != ad->flag ) {
        self->dw_arrowbegDesc.vtidx = ad->vtidx;
        self->dw_arrowbegDesc.flag  = ad->flag ;
        CALL_CB();
    }
}

static void pen_cb_arrowend(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_pen*   self = (Tydw_pen*)dw;
    int         idx  = v;
    TarrowDesc* ad   = pxm_arrow_ref()+idx;
    if ( self->dw_arrowendDesc.vtidx != ad->vtidx ||
         self->dw_arrowendDesc.flag != ad->flag ) {
        self->dw_arrowendDesc.vtidx = ad->vtidx;
        self->dw_arrowendDesc.flag  = ad->flag ;
        CALL_CB();
    }
}

static void pen_cb_arrowparams(Tvgui_dw* dw)
{
    Tydw_pen* self = (Tydw_pen*)dw;
    CALL_CB();
}

/*======================================================================*/
