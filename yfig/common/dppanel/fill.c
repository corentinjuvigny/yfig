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
 *      $Id: fill.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/fill.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the filling area         =*/
/*= parameter: color, bg tile.                                         =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "ydw/ydw.h"
#include "dppanel.h"

#include "vgui/vgui.h"
#include "vgui/vgui-outdev.h"

#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= Fill Color                                                         =*/

extern Twid_button* dpp_fillcolor_new  (
        Twid_buttonPanel* panel, Tsty_fill*syn)
{
    Twid_button* button;
    button = dpp_l1l2color_newDPF(panel,"Fill","Color", syn);
    return button;
}

/*======================================================================*/
/*= Fill tile                                                          =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= data structure                                                     =*/

typedef struct _Tfilltile {
    Tdpp_l1l2base base;
    Tsty_fill     dt_dpf;   // the background tile that is presently
                            // drawn in fs_dev pixmap.
    Tsty_fill*    dt_sync;  // if !=0, dt_curr and *syn are synchronized 
} Tfilltile;

#define BTN_DX DPP_V3STD_DX
#define BTN_DY DPP_V3STD_DY

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= the utility                                                        =*/

//static void dpp_filltile_free(Tdpp_l1l2base* dt);
static void dpp_filltile_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw);
static void dpp_filltile_inc (Tdpp_l1l2base* dt);
static void dpp_filltile_dec (Tdpp_l1l2base* dt);
static void dpp_filltile_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);

extern Twid_button* dpp_filltile_new (
        Twid_buttonPanel* panel, Tsty_fill*syn)
{
    Tvgui* gui = panel->vgbpm_gui;
    int v3dx = BTN_DX;
    int v3dy = BTN_DY;
    l1l2_none_adjust(gui,&v3dx,&v3dy);

    // build the button
    Twid_button* btn;
    Tfilltile* self = tool_malloc_0( sizeof(*self) );

    self->dt_dpf  = *syn;
    self->dt_sync = syn;

  //self->dt_free   = dpp_filltile_free;
    self->dt_cb1    = dpp_filltile_cb1;
    self->dt_inc    = dpp_filltile_inc;
    self->dt_dec    = dpp_filltile_dec;
    self->dt_redraw = dpp_filltile_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&self->base, panel, "Fill","Tile", v3dx,v3dy);

    return btn;
}

extern void dpp_filltile_update(Twid_button* btn, Tyfig_mw* ymw)
{
    Tfilltile* self = btn->but_ud;
    if ( self->dt_sync )
        self->dt_dpf = *self->dt_sync ;
    dpp_l1l2base_redrawFull( &self->base );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Private utilities                                                  =*/

typedef struct _Tydw_filltile {
    Tvgui_dw   any;
    Tydw_fill* dw_fill;
} Tydw_filltile;

static Tydw_filltile* ydw_filltile_nbf(Tyfig_mw* ymw, Csty_fill*dpf)
{
    Tydw_filltile* self = (Tydw_filltile*)
                      vgui_dw_newTop(sizeof(*self), &ymw->any,
                        tool_strdup("background tile"),
                        WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok);
    self->dw_fill = ydw_fill_nbf(SelfA,ymw,dpf,NULL,0);
    vgui_dw_finishIC(SelfA, 0,ydw_fill_widEGS(self->dw_fill), -1);
    ydw_fill_unfoldColor( self->dw_fill ); 
    ydw_fill_unfoldTile( self->dw_fill ); 
    return self;
}
static void ydw_filltile_free(Tydw_filltile*self)
{
    ydw_fill_free( self->dw_fill );
    vgui_dw_free( SelfA );
}


/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Specialization of Tdpp_l1l2base                                    =*/

//static void dpp_filltile_free(Tdpp_l1l2base* dt);

static void dpp_filltile_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{
    Tfilltile*     self    = (Tfilltile*)dt;
    Tydw_filltile* dialog;
    int            status;
    
    dialog = ydw_filltile_nbf(ymw,&self->dt_dpf);
    status = vgui_dw_run(&dialog->any);

    if ( status==WID_DIALSTATUS_Ok ) {
       Csty_fill* ndpf = ydw_fill_dpf( dialog->dw_fill );
       DBG0_Dp(DOPT_FMT"dialog OK: %s",DOPT_PSE,DBG_STF2str(ndpf)); 
       self->dt_dpf = *ndpf;
       if ( self->dt_sync!=0 )
            *self->dt_sync = *ndpf;
       yfig_dppanel_update(ymw);
    } else
       DBG0_Dp(DOPT_FMT"dialog Canceled",DOPT_PSE); 

    ydw_filltile_free(dialog);
    
}

static void dpp_filltile_inc (Tdpp_l1l2base* dt)
{
    PERM_spe_NYI(0);
}

static void dpp_filltile_dec (Tdpp_l1l2base* dt)
{
    PERM_spe_NYI(0);
}

static void dpp_filltile_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy)
{
    Tod_GCdef  gcd  = OD_GC_SysDraw;
    Tfilltile* self = (Tfilltile*)dt;
    Toutdev*   od   = &vdev->v_od;

    if ( self->dt_dpf.dpf_style==DA_FS_NoFill ||
         self->dt_dpf.dpf_style==DA_FS_Solid  ) {
        dpp_none_draw(vdev,x,y,dx,dy);
        DBG2_Dp(DOPT_FMT"draw %s",DOPT_PSE, dpp_none);
    } else {
      od_style_pushTop(od, gcd);
        Tsty_fill* dpf = &self->dt_dpf; // shortcut
        DBG2_Dp(DOPT_FMT"fill rect %dx%d : %s",DOPT_PSE, dx,dy,DBG_STF2str(dpf));
        od_ldaf_setGCll(od,gcd,1.0,
                dpf->dpf_style, dpf->dpf_color,
                dpf->dpf_shade,dpf->dpf_light,dpf->dpf_pattern);
        od_fill_rectXYWH(od,gcd,x,y,dx,dy);
      od_style_pop(od,gcd);
    }
}

/*======================================================================*/
