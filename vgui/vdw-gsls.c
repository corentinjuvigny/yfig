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
 *      $Id: vdw-gsls.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-gsls.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= GSLS implementation (Get Single Line String).                      =*/
/*= See header wid-basewidget.h for function documentation.            =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-simple.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/

typedef struct _Tvdw_gsls {
    Tvgui_dw      any;
    Twid_slin*    slin;
    Twid_cb_Us    chgCB;
    Twid_cb_Us    valCB;
    void*         udCB;
} Tvdw_gsls;

/*======================================================================*/
/*= callbacks                                                          =*/

static void wid_gsls_cb_changed(Twid_slin* self, void* ud)
{
    Tvdw_gsls* d   = ud;
    TstrEaten  str = wid_slin_getText( self );
    DBG1_DwE("dial=%p str=%s",d,str);
    if ( d->chgCB!=0 )
        d->chgCB(d->udCB,str);
    DBG1_DwE("dial=%p str=%s",d,str);
    tool_free ( str );
}

static void wid_gsls_cb_validate(Twid_slin* self, void* ud)
{
    Tvdw_gsls* d    = ud;
    TstrEaten  str  = wid_slin_getText( self );
    DBG1_DwE("dial=%p str=%s",d,str);
    //vgui_dw_setSelBUT(&d->any,WID_DIALSTATUS_Ok);
    vgui_dw_breakloop( &d->any,WID_DIALSTATUS_Ok);
    DBG1_DwE("dial=%p str=%s",d,str);
    tool_free ( str );
}

/*======================================================================*/
/*= base function for creting and handling Tvdw_gsls structures        =*/

static Tvgui_dw* vdwM_gsls_new(
                                   Twid_dialStatus buttons,
                                   Tvgui_mw*vmw,   CstrBrief title, int hlayout,
                                   Twid_cb_Us chg, Twid_cb_Us val, void*ud)
{
    Tvdw_gsls* self = (Tvdw_gsls*)vgui_dw_newTop( sizeof(*self),
            vmw, tool_strdup(title), buttons);
    self->chgCB = chg;
    self->valCB = val;
    self->udCB  = ud;
    self->any.D_top->dt_btnLayout = hlayout!=0 ? 2 : 0; // 2&1=horizontal, 2&6=east

    return &self->any;
}

extern Tvgui_dw* vdwM_gsls_new_CancelOk(
                                   Tvgui_mw*vds,   CstrBrief title, int hlayout,
                                   Twid_cb_Us chg, Twid_cb_Us val, void*ud)
{
    Twid_dialStatus butts = WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok;
    return  vdwM_gsls_new(butts, vds, title, hlayout, chg, val, ud);
}

extern Tvgui_dw* vdwM_gsls_new_CancelContOk(
                                   Tvgui_mw*vds,   CstrBrief title, int hlayout,
                                   Twid_cb_Us chg, Twid_cb_Us val, void*ud)
{
    Twid_dialStatus butts = WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Cont
                          | WID_DIALSTATUS_Ok;
    return  vdwM_gsls_new(butts, vds, title, hlayout, chg, val, ud);
}

extern void         wid_gsls_bf(Tvgui_dw*_self, 
                                 CstrBrief text, CstrBrief width)
{
    Tvdw_gsls* self = (Tvdw_gsls*)_self;
    Tvgui*     gui  = vgui_dw_getVGui(_self);
    Tegs_widget pw = self->wd_widEGS;
    self->slin = widM_slin_nbf(gui, pw,
            text,width,
            wid_gsls_cb_changed,wid_gsls_cb_validate, self);
    self->any.top->dt_focus = self->slin->widEGS;
    vgui_dw_finishIL(_self,0,self->slin->widEGS,-1);
}

extern void wid_gsls_free(Tvgui_dw*_self)
{
    Tvdw_gsls* self = (Tvdw_gsls*)_self;
    wid_slin_free( self->slin );
    vgui_dw_free(_self);
}

extern Twid_dialStatus wid_gsls_run(Tvgui_dw*self)
{
    int status;
    status = vgui_dw_run(self);
    return status;
}

extern Twid_dialStatus wid_gsls_run2(Tvgui_dw*self,
        Tegs_widget pw, int x, int y)
{
    int status;
    status = vgui_dw_run2(self,pw,0,0);
    return status;
}

/*======================================================================*/
/*= high level interface                                               =*/

extern Twid_dialStatus vdwM_gsls_start_CancelOk(
                           Tvgui_mw*vds,   CstrBrief title,
                           CstrBrief text, CstrBrief width,
                           Twid_cb_Us chg, Twid_cb_Us val, void*ud,
                           Tegs_widget pw, int horiz)
{
    Tvgui_dw* self;
    int status;
    
    self = vdwM_gsls_new_CancelOk(vds, title, horiz, chg,val,ud);
    wid_gsls_bf(self,text,width);
    if ( pw==0 ) 
        status = wid_gsls_run(self);
    else
        status = wid_gsls_run2(self,pw,0,0);
    wid_gsls_free(self);
    return status;
}

extern Twid_dialStatus vdwM_gsls_start_CancelContOk(
                           Tvgui_mw*vds,   CstrBrief title,
                           CstrBrief text, CstrBrief width,
                           Twid_cb_Us chg, Twid_cb_Us val, void*ud,
                           Tegs_widget pw, int horiz)
{
    Tvgui_dw* self;
    int status;
    
    self = vdwM_gsls_new_CancelContOk(vds, title, horiz, chg,val,ud);
    wid_gsls_bf(self,text,width);
    if ( pw==0 ) 
        status = wid_gsls_run(self);
    else
        status = wid_gsls_run2(self,pw,0,0);
    wid_gsls_free(self);
    return status;
}

/*======================================================================*/
