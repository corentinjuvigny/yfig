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
 *      $Id: line-style.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/line-style.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the drawing parameter:   =*/
/*=  line style                                                        =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-pixmap.h"
#include "yfig-mw.h"
#include "ydw/ydw.h"
#include "dppanel.h"
#include "vgui/vgui-outdev.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tlinestyle Tlinestyle;
typedef const   Tlinestyle Clinestyle;

struct _Tlinestyle {
    Tdpp_l1l2base base;

    // dt_style gives the line style that is presently shown
    TlineStyle    dt_style;   

    TlineStyle*   dt_styles;  // dt_styles[i] corresponds to dt_pxms[i]
    TpixmapEGS*   dt_pxms;    // the style pixmaps 
    int           dt_pxmNb;   // the pixmap number in dt_pxms 
    int           dt_index;   // dt_style = dt_styles[dt_index]

    // the next field gives the external line style that the widget
    // must update automatically.
    // it can be null.
    TlineStyle*   dt_sync;   
};

// configuration
#define BTN_DX DPP_V3STD_DX
#define BTN_DY DPP_V3STD_DY

// debug
#define DLS_FMT     "option: %s %s : style=%s : " 
#define DLS_PSE     self->dt_label1,self->dt_label2,DBG_LS2str(self->dt_style)
#define DLS_PSES(s) DLS_PSE,DBG_LS2str(s)
#define DLS_PDT     dt->ll_label1,dt->ll_label2, ""
#define DLS_PDTS(s) DLS_PDT,DBG_LS2str(s)

/*======================================================================*/
/*= Interface                                                          =*/

static void dpp_linestyle_free (Tdpp_l1l2base* _dt);
#define     dpp_linestyle_cb1   dpp_line_cb1
static void dpp_linestyle_inc  (Tdpp_l1l2base* _dt);
static void dpp_linestyle_dec  (Tdpp_l1l2base* _dt);
static void dpp_linestyle_draw (Tdpp_l1l2base* _dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* dpp_linestyle_new(Twid_buttonPanel* panel,
                                      TlineStyle*sync)
{
    Twid_button* btn;
    int       v3dx = pxm_lineStyle_dx();
    int       v3dy = BTN_DY;
    CstrBrief l1  = "Pen";
    CstrBrief l2  = "Style";
    Tvgui*    gui = panel->bp_gui;

    Tlinestyle* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_style = sync ? *sync : DA_LS_Solid;
    dt->dt_styles = pxm_lineStyle_ref();
    dt->dt_pxms   = pxm_lineStyle_pxm(gui,0,v3dy,NULL);
    dt->dt_pxmNb  = pxm_lineStyle_nbe(gui);
    for ( dt->dt_index=0 ; dt->dt_styles[dt->dt_index]!=dt->dt_style
                         ; dt->dt_index+=1);
    dt->dt_sync = sync;

    dt->dt_free   = dpp_linestyle_free;
    dt->dt_cb1    = dpp_linestyle_cb1;
    dt->dt_inc    = dpp_linestyle_inc;
    dt->dt_dec    = dpp_linestyle_dec;
    dt->dt_redraw = dpp_linestyle_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, v3dx,v3dy);

    return btn;
}

static void dpp_linestyle_free (Tdpp_l1l2base* dt)
{
    Tlinestyle* self = (Tlinestyle*)dt;
    Tvgui*       gui = self->dt_pod->vv_gui;
    int i;
    for ( i=0 ; i<self->dt_pxmNb ; i+=1) {
        gui_pixmap_free( gui, self->dt_pxms[i] );
    }
    free ( self->dt_pxms );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void     dpp_linestyle_update( Twid_button* btn, Tyfig_mw* ymw)
{
    Tlinestyle* self = btn->but_ud;

    if ( self->dt_sync==0 )
        return;

    self->dt_style = *self->dt_sync;
    for ( self->dt_index=0 ; self->dt_styles[self->dt_index]!=self->dt_style
                           ; self->dt_index+=1);
    dpp_l1l2base_redrawFull( &self->base );
}

/*======================================================================*/
/*= Private utilities                                                  =*/

/*======================================================================*/
/*= Specialization                                                     =*/

extern void dpp_line_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{
    Tvgui_dw* dialog = ydw_pen_nbfTop(NULL,ymw,&ymw->ymw_currDP.fdp_line,
            NULL,0,0);
    Twid_dialStatus status = vgui_dw_run( dialog );
    if ( status==WID_DIALSTATUS_Ok ) {
        Csty_line* pen = ydw_pen_dpl( (Tydw_pen*)dialog );
        DBG2_Dp(DLS_FMT "status=%08x(%s) : pen=%s",DLS_PDT,status, vgui_dw_status2buttLab(status), DBG_STL2str(pen));
        ymw->ymw_currDP.fdp_line = *pen;
        CarrowDesc* ad;
        ad = ydw_pen_arrowDesc( (Tydw_pen*)dialog, 1 ); ymw->ymw_arrowbeg = *ad;
        ad = ydw_pen_arrowDesc( (Tydw_pen*)dialog, 0 ); ymw->ymw_arrowend = *ad;

        Tyfig_drawparam* param = &ymw->ymw_currYDP;
        dpp_linewidth_update( param->dp_linewidth, ymw );
        dpp_linecolor_update( param->dp_linecolor, ymw );
        dpp_linestyle_update( param->dp_linestyle, ymw );
        dpp_linearrow_update( param->dp_linearrow, ymw );
    }
    ydw_pen_free( (Tydw_pen*)dialog );
}

static void dpp_linestyle_inc (Tdpp_l1l2base* dt)
{
    Tlinestyle* self = (Tlinestyle*)dt;
    self->dt_index = self->dt_index==self->dt_pxmNb-1 ? 0 : self->dt_index+1;
    self->dt_style = self->dt_styles[self->dt_index];
    if ( self->dt_sync )
        *self->dt_sync = self->dt_style;
}

static void dpp_linestyle_dec (Tdpp_l1l2base* dt)
{
    Tlinestyle* self = (Tlinestyle*)dt;
    self->dt_index = self->dt_index==0 ? self->dt_pxmNb-1 : self->dt_index-1;
    self->dt_style = self->dt_styles[self->dt_index];
    if ( self->dt_sync )
        *self->dt_sync = self->dt_style;
}

static void dpp_linestyle_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy)
{
    Tlinestyle* self = (Tlinestyle*)dt;
    gui_pixmap_drawPM(vdev,x,y,self->dt_pxms[self->dt_index],0,0,dx,dy);
}

/*======================================================================*/
