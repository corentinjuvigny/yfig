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
 *      $Id: wid-grp-angle.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-grp-angle.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Composite widget for input/output of angle.                        =*/
/*=                                                                    =*/
/*= See the wid-grp.h headers for documentation.                       =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-grp.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-lvu.h"

#include "tools/math-macros.h"
#include "tools/lenunit.h"

/*======================================================================*/

typedef struct _Twid_io {
    WID_GRP_FIELDS();
    Twid_ilvu* angle;
#define        appCBvld appCB   /* appication callback */
    Twid_grpCB appCBchg;        /* appication callback */
} Twid_ioa;

Inline Twid_grp* GRP(Twid_ioa*self) { return (Twid_grp*)self; }

Inline double conv_App2Dis(Twid_ioa*self, double v) { return RadToDeg180(v); }
Inline double conv_Dis2App(Twid_ioa*self, double v) { return DegToRad(v);    }

#define Dis2App(v) conv_Dis2App(self,v)
#define App2Dis(v) conv_App2Dis(self,v)

/*======================================================================*/
/*= Services                                                           =*/

#define WIDEGS_none NULL

static void    ioa_free       (Twid_any* grp) { Twid_ioa* self=(Twid_ioa*)grp; wid_ilvu_free   (self->angle); }
static TwidEGS ioa_lioEGS     (Cwid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; return self->angle!=0 ? self->angle->lvu_labEGS : WIDEGS_none; }
static TwidEGS ioa_xioEGS     (Cwid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; return self->angle!=0 ? self->angle->lvu_valEGS : WIDEGS_none; }
static TwidEGS ioa_uioEGS     (Cwid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; return self->angle!=0 ? self->angle->lvu_uniEGS : WIDEGS_none; }
static void    ioa_clear      (Twid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; wid_ilvu_clear  (self->angle); }
static void    ioa_enable     (Twid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; wid_ilvu_enable (self->angle); }
static void    ioa_disable    (Twid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; wid_ilvu_disable(self->angle); }
static double  ioa_getDble10  (Twid_grp* grp) { Twid_ioa* self=(Twid_ioa*)grp; return Dis2App(wid_ilvu_getNum10(self->angle)); }
static void    ioa_setDble10  (Twid_grp* grp, double v)  { Twid_ioa* self=(Twid_ioa*)grp; wid_ilvu_setNum10(self->angle,App2Dis(v)); }

/*======================================================================*/
/*= The virtual tables                                                 =*/

Twid_grpVtable ioa_grpvtable = {
    .lioEGS      = ioa_lioEGS,
    .xioEGS      = ioa_xioEGS,
//  .yioEGS      = ioa_yioEGS,
    .uioEGS      = ioa_uioEGS,

    .clear       = ioa_clear,
    .enable      = ioa_enable,
    .disable     = ioa_disable,

    .getDble10   = ioa_getDble10,
//  .getDble01   = ioa_getDble01,
//  .getDblePt   = ioa_getDblePt,
    .setDble10   = ioa_setDble10,
//  .setDble01   = ioa_setDble01,
//  .setDblePt   = ioa_setDblePt,
//  .chgUsrUnit  = ioa_chgUsrUnit
};

Twid_anyVtable ioa_vtable = {
    WID_ANY_SCVT_GRP(sizeof(Twid_ioa),ioa_free,ioa_grpvtable)
};

/*======================================================================*/
/*= Interface                                                          =*/

static void    ioa_cb_vld(Tvgui_dw*vdw, Twid_ilvu*iw, double v);
static void    ioa_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v);

extern Twid_grp* wid_ioa180_nbf(Twid_box* box, CstrBrief label,
                                Twid_grpCB cbvld, Twid_grpCB cbchg, void* cbud,
                                double iv)
{
    Twid_cb_DId vld  = cbvld ? ioa_cb_vld : NULL;
    Twid_cb_DId chg  = cbchg ? ioa_cb_chg : NULL;
    Twid_ioa*  self = (Twid_ioa*)wid_grp_newByBox(&ioa_vtable, box);
    self->appCBvld   = cbvld;
    self->appCBchg   = cbchg;
    self->appCBud    = cbud;
    self->angle = wid_ilvu_newD(App2Dis(iv),"_999.99_",vld,chg,self,-180.0,180.0);
    wid_ilvu_addSpinInc(self->angle,5);
  if (label)
    wid_ilvu_addLabel  ( self->angle, label    , NULL);
    wid_ilvu_addUnitLab( self->angle, "degree" , NULL);
    widD_ilvu_dbf      ( self->angle, box );
    return GRP(self);
}

/*======================================================================*/
/*= Callbacks                                                          =*/

static void    ioa_cb_vld(Tvgui_dw*vdw, Twid_ilvu*iw, double v)
{
    Twid_ioa* self = iw->lvu_ud;
    if ( self->appCBvld ) self->appCBvld(GRP(self),self->appCBud);
}

static void    ioa_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v)
{
    Twid_ioa* self = iw->lvu_ud;
    if ( self->appCBchg ) self->appCBchg(GRP(self),self->appCBud);
}

/*======================================================================*/
