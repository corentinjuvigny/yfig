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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: l1l2v3.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/l1l2v3.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

#include "vgui/vgui-outdev.h"
#include "vgui/wid-simple.h"

#include "outdev/string-hl.h"
#include "tools/range.h"
#include "tools/math-macros.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tl1l2v3 Tl1l2v3;
typedef const   Tl1l2v3 Cl1l2v3;

struct _Tl1l2v3 {
    Tdpp_l1l2base base;
    Trange        dt_range;
    void        (*dt_action)(Tyfig_mw*ymw,Tany any);
};

/*======================================================================*/
/*= the utility                                                        =*/

//static void dpp_l1l2v3_free(Tdpp_l1l2base* dt);
static void dpp_l1l2v3_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw);
static void dpp_l1l2v3_inc (Tdpp_l1l2base* dt);
static void dpp_l1l2v3_dec (Tdpp_l1l2base* dt);
static void dpp_l1l2v3_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vod, double x, double y, double dx, double dy);

static Twid_button* dpp_l1l2v3_new(
        Twid_buttonPanel* panel, Cdpp_l1l2v3* l1l2v3, void*piv, int isdble)
{
    Tvgui* gui    = panel->vgbpm_gui;
    Twid_button* btn;

    Tl1l2v3* dt = tool_malloc_0( sizeof(*dt) );

    double initval = piv==0 ? l1l2v3->initval
                   : isdble ? *((double*)piv) : *((int*)piv);
    if ( l1l2v3->fact==0 ) {
        range_initIMM(&dt->dt_range,
            initval, l1l2v3->fmt,
            l1l2v3->minval, l1l2v3->maxval);
            if ( l1l2v3->incr!=0 )
                dt->dt_range.iof = l1l2v3->incr;
    } else {
        range_initFMM(&dt->dt_range,
            initval, l1l2v3->fmt,
            l1l2v3->minval, l1l2v3->maxval);
            dt->dt_range.iof = l1l2v3->fact;
    }
    if ( piv!=0 && isdble!=0) 
        range_synDble(&dt->dt_range,piv);
    else if ( piv!=0 && isdble==0) 
        range_synInt(&dt->dt_range,piv);

    int dxb=30,dyb=20;
    //dt->dt_free   = dpp_l1l2v3_free;
    dt->dt_cb1    = dpp_l1l2v3_cb1;
    dt->dt_inc    = dpp_l1l2v3_inc;
    dt->dt_dec    = dpp_l1l2v3_dec;
    dt->dt_redraw = dpp_l1l2v3_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel,
            l1l2v3->label1, l1l2v3->label2, dxb,dyb);
    return btn;
}

extern Twid_button* dpp_l1l2v3_newDble(
        Twid_buttonPanel* panel, Cdpp_l1l2v3* l1l2v3, double*piv)
{ return dpp_l1l2v3_new(panel,l1l2v3,piv,1); }

extern Twid_button* dpp_l1l2v3_newInt(
        Twid_buttonPanel* panel, Cdpp_l1l2v3* l1l2v3, int*piv)
{ return dpp_l1l2v3_new(panel,l1l2v3,piv,0); }

extern void dpp_l1l2l3_setAction(Twid_button* btn,
                                 void(*cb)(Tyfig_mw*ymw, Tany av))
{
    Tl1l2v3* self   = btn->but_ud;
    self->dt_action = cb;
}

extern double dpp_l1l2v3_getVal( Cwid_button* self)
{
    Tl1l2v3* dt  = self->but_ud;
    return dt->dt_range.val;
}

extern void dpp_l1l2v3_setVal( Cwid_button* btn, double val)
{
    Tl1l2v3* self = btn->but_ud;
    Trange*  range= &self->dt_range;
    if ( dble_closeAtP(range->val,val,0.01) ) return;

    range_setVal( range, val );
    dpp_l1l2base_redrawFull(&self->base);
}

extern void   dpp_l1l2v3_update( Twid_button* btn, Tyfig_mw* ymw)
{
    Tl1l2v3* self = btn->but_ud;
    double oldval = self->dt_range.val;
    range_update( &self->dt_range );

    if ( oldval!=self->dt_range.val ) {
        dpp_l1l2base_redrawFull(&self->base);
    }
}

/*======================================================================*/
/*= internal utilities                                                 =*/

static void dpp_l1l2v3_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{
    Tl1l2v3*  self  = (Tl1l2v3*)dt;
    Trange*   range = &self->dt_range;
    double   oldval = range->val;
    char title[1000];
    sprintf(title,"change %s %s",self->dt_label1,self->dt_label2);
    vdw_gnv_start((Tvgui_mw*)ymw,title,range);
    if ( oldval!=range->val ) {
        dpp_l1l2base_redrawFull(dt);
        if ( self->dt_action )
            self->dt_action(ymw,range_any(range));
    }
}

static void dpp_l1l2v3_inc (Tdpp_l1l2base* dt)
{
    Tl1l2v3* self   = (Tl1l2v3*)dt;
    Trange*  range  = &self->dt_range;
    double   oldval = range->val;
    range_inc(range);
    if ( oldval!=range->val && self->dt_action ) {
        Tyfig_mw* ymw = self->dt_pod->vv_userdata;
        self->dt_action(ymw,range_any(range));
    }
}

static void dpp_l1l2v3_dec (Tdpp_l1l2base* dt)
{
    Tl1l2v3* self   = (Tl1l2v3*)dt;
    Trange*  range  = &self->dt_range;
    double   oldval = range->val;
    range_dec(range);
    if ( oldval!=range->val && self->dt_action ) {
        Tyfig_mw* ymw = self->dt_pod->vv_userdata;
        self->dt_action(ymw,range_any(range));
    }
}

static void dpp_l1l2v3_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vod, double x, double y, double dx, double dy)
{
    Tl1l2v3* self = (Tl1l2v3*)dt;
    Tvgui*   gui  = vod->v_gui;

    char tmp[100];
    char* str = range_dble(tmp,&self->dt_range);
    DBG2_DpM("dev=%p %s %s ; drawing %s at (%d %d)",
            vod,self->dt_label1,self->dt_label2, str,x,y);
    od_str_draw_LR(&vod->v_od, OD_GC_SysDraw, str, x,y+16); 
}

/*======================================================================*/
