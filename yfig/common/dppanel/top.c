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
 *      $Id: top.c 141 2018-05-27 13:32:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/top.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*=                                                                    =*/
/*= The parameters that are handled in this file are:                  =*/
/*=   - depth  used for all objects                                    =*/
/*=   - angle  used for some objects like ellipse and for rotating     =*/
/*=            objects.                                                =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

// for zoom
#include "yfig-mw.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-canvas-geomcb.h"
#include "tools/math-macros.h"

/*======================================================================*/

static Tdpp_l1l2v3 zoom   = {
    "Zoom"       , 0                     , "99.99",
    .initval=1   , .minval=CVSGEO_Zmin   , .maxval=CVSGEO_Zmax,
    .fact=CVSGEO_Zstep
};

static Tdpp_l1l2v3 depth   = {
    "Depth"      , 0                     , "999",
    .initval=50  , .minval=FIG_MIN_DEPTH , .maxval=FIG_MAX_DEPTH
};

static Tdpp_l1l2v3 angle   = {
    "Angle"      , 0             , "999.9",
    .initval=0   , .minval=-180  , .maxval=180
};

/*======================================================================*/

extern double dpp_zoom_getVal(Cwid_button* btn)
{ return dpp_l1l2v3_getVal( btn ); }

extern void   dpp_zoom_setVal(Twid_button* btn, double val)
{ return dpp_l1l2v3_setVal(btn,val); }

static void dpp_zoom_action(Tyfig_mw*ymw, Tany av)
{
    Tvgui_outdev* vod = &ymw->ymw_vdev->vv_vod;
    if ( vod==0 ) return;
    double oz = od_getUsrZoom( &vod->v_od );
    double nz = av.dreal;
    if ( dble_closeAtP(oz,nz,0.02) ) return;
    if ( wid_canvas_setZoomCE(ymw->ymw_canvas,nz)!=0 )
        yfig_cursce_redraw (ymw);
}

extern Twid_button* dpp_zoom_new(
        Twid_buttonPanel* panel, Tvgui_outdev*dev, double*syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newDble(panel,&zoom, syn);
    dpp_l1l2l3_setAction(button,dpp_zoom_action);
    return button;
}

/*======================================================================*/

extern Twid_button* dpp_depth_new (
        Twid_buttonPanel* panel, int* syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newInt(panel,&depth, syn);
    return button;
}
extern void   dpp_depth_update(Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2v3_update(self,ymw); }

/*======================================================================*/

extern Twid_button* dpp_angle_new (
        Twid_buttonPanel* panel, double* syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newDble(panel,&angle, syn);
    return button;
}

/*======================================================================*/
