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
 *      $Id: grid.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/grid.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the drawing parameter:   =*/
/*=   background grid                                                  =*/
/*=  magnetic grid                                                     =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "dppanel.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-outdev.h"

#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tgridmag Tgridmag;
typedef const   Tgridmag Cgridmag;

struct _Tgridmag {
    Tdpp_l1l2base base;
    TpixmapEGS*   dt_pxms;    // the grid pixmaps 
    int           dt_pxmNb;   // the pixmap number in dt_pxms 
    int           dt_index;   // dt_step = dt_steps[dt_index]

    // dt_step gives the grid spacing that is presently shown
    double        dt_step;   
    double*       dt_steps; // dt_steps[i] corresponds to dt_pxms[i]
};

// configuration

// debug
#define DGM_FMT     "option: %s %s : ratio=%f : " 
#define DGM_PSE     self->dt_label1,self->dt_label2,self->dt_step
#define DGM_PDT     dt->ll_label1,dt->ll_label2,0.0

/*======================================================================*/
/*= Interface                                                          =*/

//static int          dpp_gridmag_pxm_dx();
//static int          dpp_gridmag_pxm_dy();
static double*      dpp_gridmag_pxm_ref();
static TpixmapEGS*  dpp_gridmag_pxm_pxm(Tvgui* gui,int dx, int dy);
static int          dpp_gridmag_pxm_nbe(Tvgui* gui);

static double*      dpp_gridshow_pxm_ref();
static TpixmapEGS*  dpp_gridshow_pxm_pxm(Tvgui* gui,int dx, int dy);
static int          dpp_gridshow_pxm_nbe(Tvgui* gui);

static void dpp_gridmag_free(Tdpp_l1l2base* _dt);
static void dpp_gridmag_cb1 (Tdpp_l1l2base* _dt, Tyfig_mw* ymw);
static void dpp_gridmag_inc (Tdpp_l1l2base* _dt);
static void dpp_gridmag_dec (Tdpp_l1l2base* _dt);
static void dpp_gridshow_inc(Tdpp_l1l2base* _dt);
static void dpp_gridshow_dec(Tdpp_l1l2base* _dt);
static void dpp_gridmag_draw(Tdpp_l1l2base* _dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* dpp_gridmag_new(Twid_buttonPanel* panel)
{
    Twid_button* btn;
    CstrBrief l1  = "Magnet";
    CstrBrief l2  = "Grid";
    Tvgui*    gui = panel->bp_gui;
    int       dyb = dpp_l1l2base_getMaxV3Dy(gui,1);
    int       dxb = dyb;
    l1l2_none_adjust(gui,&dxb,&dyb);

    Tgridmag* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_step   = 0.0;
    dt->dt_steps  = dpp_gridmag_pxm_ref();
    dt->dt_pxms   = dpp_gridmag_pxm_pxm(gui,dxb,dyb);
    dt->dt_pxmNb  = dpp_gridmag_pxm_nbe(gui);
    for ( dt->dt_index=0 ; dt->dt_steps[dt->dt_index]!=dt->dt_step
                         ; dt->dt_index+=1);

    dt->dt_free   = dpp_gridmag_free;
    dt->dt_cb1    = dpp_gridmag_cb1;
    dt->dt_inc    = dpp_gridmag_inc;
    dt->dt_dec    = dpp_gridmag_dec;
    dt->dt_redraw = dpp_gridmag_draw;
    btn = dpp_l1l2base_initBtnStdPXM3_V3VSep(&dt->base, panel, l1,l2, dxb,dyb, 1);

    return btn;
}

extern double dpp_gridmag_getVal(Twid_button* btn)
{ Tgridmag* self = btn->but_ud; return self->dt_step; }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* dpp_gridshow_new(Twid_buttonPanel* panel)
{
    Twid_button* btn;
    CstrBrief l1  = "Grid";
    CstrBrief l2  = "Mode";
    Tvgui*    gui = panel->bp_gui;
    int       v3dy = dpp_l1l2base_getMaxV3Dy(gui,1);
    int       v3dx = v3dy;
    l1l2_none_adjust(gui,&v3dx,&v3dy);

    Tgridmag* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_step   = 0.0;
    dt->dt_steps  = dpp_gridshow_pxm_ref();
    dt->dt_pxms   = dpp_gridshow_pxm_pxm(gui,v3dx,v3dy);
    dt->dt_pxmNb  = dpp_gridshow_pxm_nbe(gui);
    for ( dt->dt_index=0 ; dt->dt_steps[dt->dt_index]!=dt->dt_step
                         ; dt->dt_index+=1);

    dt->dt_free   = dpp_gridmag_free;
    dt->dt_cb1    = dpp_gridmag_cb1;
    dt->dt_inc    = dpp_gridshow_inc;
    dt->dt_dec    = dpp_gridshow_dec;
    dt->dt_redraw = dpp_gridmag_draw;
    btn = dpp_l1l2base_initBtnStdPXM3_V3VSep(&dt->base, panel, l1,l2, v3dx,v3dy, 1);

    return btn;
}

extern double dpp_gridshow_getVal(Twid_button* btn)
{ Tgridmag* self = btn->but_ud; return self->dt_step; }

/*======================================================================*/
/*= Private utilities                                                  =*/

#define NBE_M 6
static double gridmag_pxm_ref[NBE_M] = { 0.0, 0.1, 0.2, 0.5, 1.0, 2.0 };

#define NBE_S 5
static double gridshow_pxm_ref[NBE_S] = { 0.0, 0.2, 0.5, 1.0, 2.0 };

static TpixmapEGS* dpp_grid_pxm_pxm(Tvgui* gui,int dx, int dy,int nbe,int*steps)
{
    int     i;
    //int     nbe = dpp_gridmag_pxm_nbe(gui);
    Ccolor *fg, *bg;
    vgui_wn_getColor(gui,wn_mainBtnStd,&fg,&bg);
    TpixmapEGS* pxms = tool_malloc_0(nbe*sizeof(*pxms));

    Tvgui_odpixmap* pdev = vgui_odpixmap_new(gui,bg,dx,dy,NULL);
    Toutdev*        od   = &pdev->vv_od;

    // initialize the GC
    double     lw = 1;
    TjoinStyle js = DA_JS_Round;
    TcapStyle  cs = DA_CS_Round;
    od_ldaf_setGCsoP(od, OD_GC_SysDraw,lw,js,cs,fg);
    int     fontid   = L1L2_FontId;      
    int     fontsize = gui->g_devPtsStd;    // 1/72 inch
    od_str_setGC(od,OD_GC_SysDraw,fontid,fontsize,fg);

    for (i=0 ; i<nbe ; i+=1 ) {
        int x1,x2,y1,y2;
        int step = steps[i]==0 ? 0 : steps[i]==1 ? 3 : 2*steps[i] ;
        if ( step==0 ) {
            // none
            dpp_none_draw(&pdev->vv_vod,0,0,dx,dy);
        } else {
            // vertical lines
            y1=0; y2=dy-1;
            for (x1=step/2 ; x1<(dx-1) ; x1+=step ) {
                x2=x1;
                od_draw_lineXY(od,OD_GC_SysDraw,x1,y1,x2,y2);
            }
            // horizontal lines
            x1=0; x2=dx-1;
            for (y1=step/2 ; y1<(dy-1) ; y1+=step ) {
                y2=y1;
                od_draw_lineXY(od,OD_GC_SysDraw,x1,y1,x2,y2);
            }
        }
        pxms[i] = vgui_odpixmap_getPxm(pdev);
    }

    vgui_odpixmap_delete( pdev );
    return pxms;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static double*      dpp_gridmag_pxm_ref()
{ return gridmag_pxm_ref; }

static TpixmapEGS* dpp_gridmag_pxm_pxm(Tvgui* gui,int dx, int dy)
{
    int steps[NBE_M] = { 0, 1, 2, 3, 4, 5 };
    return dpp_grid_pxm_pxm(gui,dx,dy,dpp_gridmag_pxm_nbe(gui),steps);
}

static int          dpp_gridmag_pxm_nbe(Tvgui* gui)
{ return NBE_M; }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static double*      dpp_gridshow_pxm_ref()
{ return gridshow_pxm_ref; }

static TpixmapEGS* dpp_gridshow_pxm_pxm(Tvgui* gui,int dx, int dy)
{
    int steps[NBE_S] = { 0, 2, 3, 4, 5 };
    return dpp_grid_pxm_pxm(gui,dx,dy,dpp_gridshow_pxm_nbe(gui),steps);
}

static int          dpp_gridshow_pxm_nbe(Tvgui* gui)
{ return NBE_S; }

/*======================================================================*/
/*= Specialization                                                     =*/

static void dpp_gridmag_free(Tdpp_l1l2base* dt)
{
    Tgridmag* self  = (Tgridmag*)dt;
    Tvgui*    gui   = wid_button_getVGui( self->dt_btn );
    int i;

    for ( i=0 ; i<self->dt_pxmNb ; i+=1 ) {
        gui_pixmap_free (gui,self->dt_pxms[i] );
    }
    free ( self->dt_pxms );
}

static void dpp_gridmag_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{
    Tgridmag* self  = (Tgridmag*)dt;
    PERM_spe_NYI(0);
}

static void dpp_gridshow_inc (Tdpp_l1l2base* dt)
{
    Tgridmag* self = (Tgridmag*)dt;
    Tyfig_mw* ymw  = (Tyfig_mw*)wid_button_getMw(self->dt_btn);
    self->dt_index = self->dt_index==self->dt_pxmNb-1 ? 0 : self->dt_index+1;
    self->dt_step  = self->dt_steps[self->dt_index];
    yfig_cursce_redraw(ymw);
}
static void dpp_gridshow_dec (Tdpp_l1l2base* dt)
{
    Tgridmag* self = (Tgridmag*)dt;
    Tyfig_mw* ymw  = (Tyfig_mw*)wid_button_getMw(self->dt_btn);
    self->dt_index = self->dt_index==0 ? self->dt_pxmNb-1 : self->dt_index-1;
    self->dt_step  = self->dt_steps[self->dt_index];
    yfig_cursce_redraw(ymw);
}

static void dpp_gridmag_inc (Tdpp_l1l2base* dt)
{
    Tgridmag* self = (Tgridmag*)dt;
    Tyfig_mw* ymw  = (Tyfig_mw*)wid_button_getMw(self->dt_btn);
    self->dt_index = self->dt_index==self->dt_pxmNb-1 ? 0 : self->dt_index+1;
    self->dt_step  = self->dt_steps[self->dt_index];
    yfig_cursce_setMagnetGrid(ymw);
}

static void dpp_gridmag_dec (Tdpp_l1l2base* dt)
{
    Tgridmag* self = (Tgridmag*)dt;
    Tyfig_mw* ymw  = (Tyfig_mw*)wid_button_getMw(self->dt_btn);
    self->dt_index = self->dt_index==0 ? self->dt_pxmNb-1 : self->dt_index-1;
    self->dt_step = self->dt_steps[self->dt_index];
    yfig_cursce_setMagnetGrid(ymw);
}

static void dpp_gridmag_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev,
                                  double x, double y, double dx, double dy)
{
    Tgridmag* self = (Tgridmag*)dt;
    Toutdev*  od   = &vdev->v_od;
    gui_pixmap_drawPM(vdev,x,y,self->dt_pxms[self->dt_index],0,0,dx,dy);
}

/*======================================================================*/

