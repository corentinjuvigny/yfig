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
 *      $Id: line-arrow.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/line-arrow.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the end and begin arrow  =*/
/*= of a curve ar a line.                                              =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"
#include "yfig-device.h"
#include "yfig-mw.h"

#include "vgui/vgui.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tlinearrow Tlinearrow;
typedef const   Tlinearrow Clinearrow;

struct _Tlinearrow {
    Tdpp_l1l2base base;

    int           dt_index;   // 0: no arrow
                              // 1: end arrow
                              // 2: begin arrow
                              // 3: both begin and end arrows
    Tyfig_device* dt_fsod;    // same as base->dt_dev with a scene attached
                              // or null
    TobjAll       dt_line;
};

// configuration
#define BTN_DX DPP_V3STD_DX
#define BTN_DY DPP_V3STD_DY

/*======================================================================*/
/*= Interface                                                          =*/

static void dpp_linearrow_free(Tdpp_l1l2base* ll);
#define     dpp_linearrow_cb1 dpp_line_cb1
static void dpp_linearrow_inc (Tdpp_l1l2base* ll);
static void dpp_linearrow_dec (Tdpp_l1l2base* ll);
static void dpp_linearrow_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x, double y, double dx, double dy);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* dpp_linearrow_new(Twid_buttonPanel* panel, CcolorSet* clrs)
{
    Twid_button* btn;
    int       dxb = BTN_DX*3;
    int       dyb = BTN_DY*1.0;
    CstrBrief l1  = "Arrow";
    CstrBrief l2  = "Mode";
    Tvgui*    gui = panel->bp_gui;

    Tlinearrow* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_free   = dpp_linearrow_free;
    dt->dt_cb1    = dpp_linearrow_cb1;
    dt->dt_inc    = dpp_linearrow_inc;
    dt->dt_dec    = dpp_linearrow_dec;
    dt->dt_redraw = dpp_linearrow_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, dxb,dyb);

    return btn;
}

static void dpp_linearrow_free   (Tdpp_l1l2base* ll)
{
    Tlinearrow* self = (Tlinearrow*)ll;
    fsod_detach( self->dt_fsod );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void dpp_linearrow_update( Twid_button* btn, Tyfig_mw* ymw)
{
    Tdpp_l1l2base* ll   = btn->but_ud;
    Tlinearrow*    self = btn->but_ud;
    if ( ymw->ymw_arrowbeg.enabled )
        self->dt_index =  ymw->ymw_arrowend.enabled ? 3 : 2;
    else
        self->dt_index =  ymw->ymw_arrowend.enabled ? 1 : 0;
    dpp_l1l2base_redrawFull( ll );
}

/*======================================================================*/
/*= Specialization                                                     =*/

static void dpp_linearrow_incdec(Tdpp_l1l2base* ll, int v)
{
    Tlinearrow* self = (Tlinearrow*)ll;
    Tyfig_mw*   ymw  = self->dt_pod->vv_userdata;
    self->dt_index   = (self->dt_index+v+4)%4;
    ymw->ymw_arrowbeg.enabled = (self->dt_index&2)!=0;
    ymw->ymw_arrowend.enabled = (self->dt_index&1)!=0;
}
static void dpp_linearrow_inc (Tdpp_l1l2base* ll) { dpp_linearrow_incdec(ll,+1); }
static void dpp_linearrow_dec (Tdpp_l1l2base* ll) { dpp_linearrow_incdec(ll,-1); }

static void dpp_linearrow_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x, double y, double dx, double dy)
{
    Tlinearrow* self = (Tlinearrow*)ll;
    Tvgui*      gui  = vod->v_gui;
    Toutdev*    od   = &vod->v_od;
    Tod_GCdef   gcf  = OD_GC_SysFill;
    double      thickness = 2;

    if ( self->dt_fsod==0 ) {
        // Caution: this can not be done in constructor because
        // dpp_l1l2base_initBtnStdPXM3 calls this function.
        double hsh=5;
        self->dt_fsod = fsod_attach(vod);
        self->dt_line = fsod_addLineXY(self->dt_fsod,thickness,
                x+hsh,y+dy/2, x+dx-hsh,y+dy/2 );
    }

//od_style_pushTop(od, gcd);
  od_style_pushTop(od, gcf);

    // clear the drawing area where scene is printed
    od_ldaf_setGCll(od,gcf,1,DA_FS_Solid,gui->vg_white,0,0,0);
    od_fill_rectXYWH(od,gcf,x,y,dx,dy);

    // draw the figure
    TarrowDesc zad;
    Tyfig_mw* ymw  = od->userdata;
    if ( (self->dt_index&1)!=0 ) {
        zad  = ymw->ymw_arrowend;
        zad.length *= fsod_scaArrowRelDU(self->dt_fsod, &zad,thickness, dx/2.5, dy-7);
        fsod_chgArrowRel(self->dt_fsod,self->dt_line,0,&zad);
    } else
        obj_arrow_delete(self->dt_line,0);
    if ( (self->dt_index&2)!=0 ) {
        zad  = ymw->ymw_arrowbeg;
        zad.length *= fsod_scaArrowRelDU(self->dt_fsod, &zad,thickness, dx/2.5, dy-7);
        fsod_chgArrowRel(self->dt_fsod,self->dt_line,1,&zad);
    } else
        obj_arrow_delete(self->dt_line,1);

    od_draw(od);
//od_style_pop(od,gcd);
  od_style_pop(od,gcf);
}

/*======================================================================*/


