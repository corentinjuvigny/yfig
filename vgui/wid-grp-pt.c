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
 *      $Id: wid-grp-pt.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-grp-pt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Composite widget for input/output of 2d points.                    =*/
/*=                                                                    =*/
/*= See the wid-grp.h headers for documentation.                        =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-grp.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-lvu.h"

#include "tools/math-macros.h"
#include "tools/lenunit.h"

/*======================================================================*/

typedef struct _Twid_inpt {
    WID_GRP_FIELDS();
    int        usrUnit;    /* user unit for displaying (LUNIT_ID_ ... macros) */
    double     inchToApp;  /* unit to use for exchanging with app (~ ppi).
                            *   1         app. unit --> 1/inchToApp inch
                            *   inchToApp app. unit --> 1           inch */
    Twid_ilvu* widx;
    Twid_ilvu* widy;
#define        appCBvld appCB   /* appication callback */
    Twid_grpCB appCBchg;        /* appication callback */
} Twid_inpt;

Inline Twid_grp* GRP(Twid_inpt*self) { return (Twid_grp*)self; }

Inline double conv_App2Dis(Twid_inpt*self, double v) {
    int    uu  = self->usrUnit;
    double ppi = self->inchToApp;
    if ( ! dble_close0(ppi) ) v = lu_convToFuPpi(v,ppi);
    if ( uu!=LUNIT_ID_None  ) v = lu_convFromFu(v,uu);
    return v;
}
Inline double conv_Dis2App(Twid_inpt*self, double v) {
    int    uu  = self->usrUnit;
    double ppi = self->inchToApp;
    if ( uu!=LUNIT_ID_None  ) v = lu_convToFu(v,uu);
    if ( ! dble_close0(ppi) ) v = lu_convFromFuPpi(v,ppi);
    return v;
}

#define Dis2App(v) conv_Dis2App(self,v)
#define App2Dis(v) conv_App2Dis(self,v)

/*======================================================================*/
/*= Services                                                           =*/

#define EgsCbOff() Twid_grpCB cbvld=self->appCBvld,cbchg=self->appCBchg; self->appCBvld=0; self->appCBchg=0
#define EgsCbOn()  self->appCBvld=cbvld; self->appCBchg=cbchg

static void    inpt_free       (Twid_any* any)            { Twid_inpt* self=(Twid_inpt*)any; wid_ilvu_free   (self->widx); wid_ilvu_free   (self->widy); }

static TwidEGS inpt_lioEGS     (Cwid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return self->widx!=0 ? self->widx->lvu_labEGS : WIDEGS_none; }
static TwidEGS inpt_xioEGS     (Cwid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return self->widx!=0 ? self->widx->lvu_valEGS : WIDEGS_none; }
static TwidEGS inpt_yioEGS     (Cwid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return self->widy!=0 ? self->widy->lvu_valEGS : WIDEGS_none; }
static TwidEGS inpt_uioEGS     (Cwid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return self->widy!=0 ? self->widy->lvu_uniEGS : WIDEGS_none; }
static void    inpt_clear      (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; wid_ilvu_clear    (self->widx);   wid_ilvu_clear    (self->widy); }
static void    inpt_enable     (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; wid_ilvu_enable   (self->widx);   wid_ilvu_enable   (self->widy); }
static void    inpt_disable    (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; wid_ilvu_disable  (self->widx);   wid_ilvu_disable  (self->widy); }
static void    inpt_updMin     (Twid_grp* grp, double v)  { Twid_inpt* self=(Twid_inpt*)grp; wid_ilvu_chgValMin(self->widx,v); wid_ilvu_chgValMin(self->widy,v); }
static void    inpt_updMax     (Twid_grp* grp, double v)  { Twid_inpt* self=(Twid_inpt*)grp; wid_ilvu_chgValMax(self->widx,v); wid_ilvu_chgValMax(self->widy,v); }
static double  inpt_getDble10  (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return Dis2App(wid_ilvu_getNum10(self->widx)); }
static double  inpt_getDble01  (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return Dis2App(wid_ilvu_getNum10(self->widy)); }
static Tdble2  inpt_getDblePt  (Twid_grp* grp)            { Twid_inpt* self=(Twid_inpt*)grp; return dble2_init( inpt_getDble10(grp), inpt_getDble01(grp) ); }
static void    inpt_setDble10  (Twid_grp* grp, double v)  { Twid_inpt* self=(Twid_inpt*)grp; EgsCbOff(); wid_ilvu_setNum10(self->widx,App2Dis(v)); EgsCbOn(); }
static void    inpt_setDble01  (Twid_grp* grp, double v)  { Twid_inpt* self=(Twid_inpt*)grp; EgsCbOff(); wid_ilvu_setNum10(self->widy,App2Dis(v)); EgsCbOn(); }
static void    inpt_setDblePt  (Twid_grp* grp, Tdble2 pt) { Twid_inpt* self=(Twid_inpt*)grp; inpt_setDble10(grp,pt.x), inpt_setDble01(grp,pt.y); }
static void    inpt_chgUsrUnit (Twid_grp* grp, int uu)
{
    Tdble2 pt;
    Twid_inpt* self=(Twid_inpt*)grp;
    if ( self->usrUnit==uu ) return;
    pt = wid_grp_getDblePt(grp);
    self->usrUnit = uu;
    wid_grp_setDblePt(grp,pt);
}

/*======================================================================*/
/*= The virtual tables                                                 =*/

Twid_grpVtable inpt_grpvtable = {
    .lioEGS      = inpt_lioEGS,
    .xioEGS      = inpt_xioEGS,
    .yioEGS      = inpt_yioEGS,
    .uioEGS      = inpt_uioEGS,

    .clear       = inpt_clear,
    .enable      = inpt_enable,
    .disable     = inpt_disable,

    .updMin      = inpt_updMin,
    .updMax      = inpt_updMax,
//  .updInc      = inpt_updInc,
    .getDble10   = inpt_getDble10,
    .getDble01   = inpt_getDble01,
    .getDblePt   = inpt_getDblePt,
    .setDble10   = inpt_setDble10,
    .setDble01   = inpt_setDble01,
    .setDblePt   = inpt_setDblePt,
    .chgUsrUnit  = inpt_chgUsrUnit
};

Twid_anyVtable inpt_vtable = {
    WID_ANY_SCVT_GRP(sizeof(Twid_inpt),inpt_free,inpt_grpvtable)
};

/*======================================================================*/
/*= Interface                                                          =*/

static void    iopt_cb_vld(Tvgui_dw*vdw, Twid_ilvu*iw, double v);
static void    iopt_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v);

extern Twid_grp* wid_iopt_nbf  (Twid_box* box, CstrBrief label,
                                Twid_grpCB cbvld, Twid_grpCB cbchg, void* cbud,
                                Tdble2 iv, double au, int uu, CstrBrief fmt)
{
    Twid_cb_DId vld  = cbvld ? iopt_cb_vld : NULL;
    Twid_cb_DId chg  = cbchg ? iopt_cb_chg : NULL;
    Twid_inpt*  self = (Twid_inpt*)wid_grp_newByBox(&inpt_vtable, box);
    self->appCBvld   = cbvld;
    self->appCBchg   = cbchg;
    self->appCBud    = cbud;
    self->usrUnit    = uu;
    self->inchToApp  = au;
    self->widx = wid_ilvu_newD(App2Dis(iv.x),fmt,vld,chg,self,0.0,0.0);
    self->widy = wid_ilvu_newD(App2Dis(iv.y),fmt,vld,chg,self,0.0,0.0);
    wid_ilvu_addLabel( self->widx, label,NULL);
    widD_ilvu_dbf    ( self->widx, box );
    widD_ilvu_dbf    ( self->widy, box );
    return GRP(self);
}

/*======================================================================*/
/*= Callbacks                                                          =*/

static void    iopt_cb_vld(Tvgui_dw*vdw, Twid_ilvu*iw, double v)
{
    Twid_inpt* self = iw->lvu_ud;
    if ( self->appCBvld ) self->appCBvld(GRP(self),self->appCBud);
}

static void    iopt_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v)
{
    Twid_inpt* self = iw->lvu_ud;
    if ( self->appCBchg ) self->appCBchg(GRP(self),self->appCBud);
}

/*======================================================================*/

