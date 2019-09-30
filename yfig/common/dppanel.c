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
 *$Software: YFIG-DDP (Draw Parameter Panel)
 *      $Id: dppanel.c 353 2018-11-11 22:56:57Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-drawparam.h"
#include "yfig-mw.h"
#include "yfig-device.h"
#include "yfig-dsds.h"
#include "ydw/ydw-update.h"
#include "dppanel/dppanel.h"

#include "vgui/wid-egs.h"
#include "vgui/wid-canvas.h"

//#include "fig/fig-drawparam.h"

#define DrawParam ymw->ymw_currYDP

/*======================================================================*/
/*= Interface to select the parameter buttons that are shown           =*/
/*= in the panel.                                                      =*/

extern void yfig_dpp_show     (Cyfig_mw* ymw, int mask)
{
    Cyfig_drawparam* dpw = &DrawParam;

    // general parameters
    if ( (DPP_SHOW_Update&mask)!=0 ) {
        gui_widegs_show(vgui_dw_getWidEGS(dpw->dp_update));
        ydw_update_enable(ymw);
    }
    if ( (DPP_SHOW_Zoom&mask)!=0 )
        gui_widegs_show(dpw->dp_zoom->but_widEGS);
    if ( (DPP_SHOW_Depth&mask)!=0 )
        gui_widegs_show(dpw->dp_depth->but_widEGS);
    if ( (DPP_SHOW_Angle&mask)!=0 )
        gui_widegs_show(dpw->dp_angle->but_widEGS);

    // align parameters
    if ( (DPP_SHOW_Align&mask)!=0 )
        gui_widegs_show(dpw->dp_alignpanel->bp_widEGSint);

    // line parameters
    if ( (DPP_SHOW_Line&mask)!=0 ) {
        if ( (DPP_SHOW_Arrow&mask)!=0 )
            gui_widegs_show(dpw->dp_linearrow->but_widEGS);
        else
            gui_widegs_unshow(dpw->dp_linearrow->but_widEGS);
        gui_widegs_show(dpw->dp_linepanel->bp_widEGSint);
    }

    // fill parameters
    if ( (DPP_SHOW_Fill&mask)!=0 )
        gui_widegs_show(dpw->dp_fillpanel->bp_widEGSint);

    // text parameters
    if ( (DPP_SHOW_Text&mask)!=0 )
        gui_widegs_show(dpw->dp_textpanel->bp_widEGSint);
}

extern void yfig_dpp_unshow   (Cyfig_mw* ymw, int mask)
{
    Cyfig_drawparam* dpw = &DrawParam;

    // general parameters
    if ( (DPP_SHOW_Update&mask)!=0 ) {
        gui_widegs_unshow(vgui_dw_getWidEGS(dpw->dp_update));
        ydw_update_disable(ymw);
    }
    if ( (DPP_SHOW_Zoom&mask)!=0 )
        gui_widegs_unshow(dpw->dp_zoom->but_widEGS);
    if ( (DPP_SHOW_Depth&mask)!=0 )
        gui_widegs_unshow(dpw->dp_depth->but_widEGS);
    if ( (DPP_SHOW_Angle&mask)!=0 )
        gui_widegs_unshow(dpw->dp_angle->but_widEGS);

    // align parameters
    if ( (DPP_SHOW_Align&mask)!=0 )
        gui_widegs_unshow(dpw->dp_alignpanel->bp_widEGSint);

    // line parameters
    if ( (DPP_SHOW_Line&mask)!=0 )
        gui_widegs_unshow(dpw->dp_linepanel->bp_widEGSint);

    // fill parameters
    if ( (DPP_SHOW_Fill&mask)!=0 )
        gui_widegs_unshow(dpw->dp_fillpanel->bp_widEGSint);

    // text parameters
    if ( (DPP_SHOW_Text&mask)!=0 )
        gui_widegs_unshow(dpw->dp_textpanel->bp_widEGSint);
}

extern void yfig_dpp_showonly (Cyfig_mw* ymw, int mask)
{
    Cyfig_drawparam* dpw = &DrawParam;

    // general parameters
    if ( (DPP_SHOW_Update&mask)!=0 ) {
        gui_widegs_show(vgui_dw_getWidEGS(dpw->dp_update));
        ydw_update_enable(ymw);
    } else {
        gui_widegs_unshow(vgui_dw_getWidEGS(dpw->dp_update));
        ydw_update_disable(ymw);
    }
    if ( (DPP_SHOW_Zoom&mask)!=0 )
        gui_widegs_show  (dpw->dp_zoom->but_widEGS);
    else
        gui_widegs_unshow(dpw->dp_zoom->but_widEGS);
    if ( (DPP_SHOW_Depth&mask)!=0 )
        gui_widegs_show  (dpw->dp_depth->but_widEGS);
    else
        gui_widegs_unshow(dpw->dp_depth->but_widEGS);
    if ( (DPP_SHOW_Angle&mask)!=0 )
        gui_widegs_show  (dpw->dp_angle->but_widEGS);
    else
        gui_widegs_unshow(dpw->dp_angle->but_widEGS);

    // align parameters
    if ( (DPP_SHOW_Align&mask)!=0 )
        gui_widegs_show  (dpw->dp_alignpanel->bp_widEGSint);
    else
        gui_widegs_unshow(dpw->dp_alignpanel->bp_widEGSint);

    // line parameters
    if ( (DPP_SHOW_Line&mask)!=0 ) {
        if ( (DPP_SHOW_Arrow&mask)!=0 )
            gui_widegs_show  (dpw->dp_linearrow->but_widEGS);
        else
            gui_widegs_unshow(dpw->dp_linearrow->but_widEGS);
        gui_widegs_show  (dpw->dp_linepanel->bp_widEGSint);
    } else
        gui_widegs_unshow(dpw->dp_linepanel->bp_widEGSint);

    // fill parameters
    if ( (DPP_SHOW_Fill&mask)!=0 )
        gui_widegs_show  (dpw->dp_fillpanel->bp_widEGSint);
    else
        gui_widegs_unshow(dpw->dp_fillpanel->bp_widEGSint);

    // text parameters
    if ( (DPP_SHOW_Text&mask)!=0 )
        gui_widegs_show  (dpw->dp_textpanel->bp_widEGSint);
    else
        gui_widegs_unshow(dpw->dp_textpanel->bp_widEGSint);
}

/*======================================================================*/
/*= Interface to get/change parameters                                 =*/

extern double yfig_mw_getZoom    (Cyfig_mw* ymw)             { return dpp_zoom_getVal(DrawParam.dp_zoom); }
extern void   yfig_mw_setZoom    (Cyfig_mw* ymw, double val) { dpp_zoom_setVal(DrawParam.dp_zoom,val); }
extern double yfig_mw_getGridShow(Cyfig_mw* ymw)             { return dpp_gridshow_getVal(DrawParam.dp_gridP); }
extern double yfig_mw_getGridMag (Cyfig_mw* ymw)             { return dpp_gridmag_getVal(DrawParam.dp_gridM); }
extern int    yfig_mw_getAlignOpe(Cyfig_mw* ymw)             { return dpp_align_getVal(DrawParam.dp_alignH)
                                                                    | dpp_align_getVal(DrawParam.dp_alignV); }
extern int    yfig_mw_getDepth   (Cyfig_mw* ymw)             { return ymw->ymw_currDP.fdp_depth; }
extern void   yfig_mw_setDepth   (Cyfig_mw* ymw, int val)    { dpp_l1l2v3_setVal(DrawParam.dp_depth,val); }
extern double yfig_mw_getAngleDeg(Cyfig_mw* ymw)             { return dpp_l1l2v3_getVal(DrawParam.dp_angle); }
extern double yfig_mw_getAngleRad(Cyfig_mw* ymw)             { return DegToRad( dpp_l1l2v3_getVal(DrawParam.dp_angle) ); }
extern void   yfig_mw_setAngle   (Cyfig_mw* ymw, double val) { dpp_l1l2v3_setVal(DrawParam.dp_angle,val); }
extern double yfig_mw_getLSF     (Cyfig_mw* ymw)             { return dpp_l1l2v3_getVal(DrawParam.dp_textLSF); }

/*======================================================================*/

extern Twid_buttonPanel* yfig_dppanel_nbf(Tyfig_mw* ymw)
{
    int i;
    Tvgui*                   gui   =  ymw->any.gui;
    Tyfig_dsds*              dsds  =  yfig_mw_getDsds(ymw);
    Tfdp*                    fdp   = &ymw->ymw_currDP;
    Tyfig_drawparam*         param = &ymw->ymw_currYDP;
    Twid_buttonPanel*        panel;

    Twid_buttonPanel*        subpanel;
    Twid_button*             butt;

    panel = widM_buttonPanel_newR1Cn(&ymw->any,0,0);
    panel->bp_scroll = 3; // permanent scrollbar
    panel->bp_buttonHVsep = 5;
    
    int dx,dy;
    int dxb,dyb;

  /* general parameters */
    param->dp_update = ymw->ymw_update;
  //param->dp_zoom  = dpp_zoom_new (panel, &ymw->ymw_canvas->vgcm_device->vv_vdev, 0);
    param->dp_zoom  = dpp_zoom_new (panel, YMW_vdev(ymw), 0);
    param->dp_gridP = dpp_gridshow_new(panel);
    param->dp_gridM = dpp_gridmag_new (panel);
    param->dp_depth = dpp_depth_new(panel, &fdp->fdp_depth);
    param->dp_angle = dpp_angle_new(panel, NULL);
    
  /* align parameters */
    param->dp_alignpanel = subpanel = widM_buttonPanel_newR1Cn(&ymw->any,0,0);
    subpanel->bp_buttonHVsep = 2;
      param->dp_alignV = dpp_align_new(subpanel, 0);
      param->dp_alignH = dpp_align_new(subpanel, 1);
    butt = widM_button_subpanel_new(subpanel);
    wid_buttonPanel_addButton(panel,butt);

  /* Drawing parameters for line */
    Tsty_line* dpl = &fdp->fdp_line;
    param->dp_linepanel = subpanel = widM_buttonPanel_newR1Cn(&ymw->any,0,0);
    subpanel->bp_buttonHVsep = 2;
      param->dp_linewidth = dpp_linewidth_new(subpanel, &dpl->dpl_thickness);
      param->dp_linecolor = dpp_linecolor_new(subpanel, &dpl->dpl_color);
      param->dp_linestyle = dpp_linestyle_new(subpanel, &dpl->dpl_style);
      param->dp_linearrow = dpp_linearrow_new(subpanel, dsds->ds_baseColors);
    butt = widM_button_subpanel_new(subpanel);
    wid_buttonPanel_addButton(panel,butt);

  /* Drawing parameters for filling area */
    Tsty_fill* dpf = &fdp->fdp_fill;
    param->dp_fillpanel = subpanel = widM_buttonPanel_newR1Cn(&ymw->any,0,0);
    subpanel->bp_buttonHVsep = 2;
      param->dp_fillcolor = dpp_fillcolor_new(subpanel, dpf);
      param->dp_filltile  = dpp_filltile_new (subpanel, dpf);
    butt = widM_button_subpanel_new(subpanel);
    wid_buttonPanel_addButton(panel,butt);

  /* Drawing parameters for text */
    Tsty_text* dpt = &fdp->fdp_text;
    param->dp_textpanel = subpanel = widM_buttonPanel_newR1Cn(&ymw->any,0,0);
    subpanel->bp_buttonHVsep = 2;
      //dpt->dpt_color = ((Tufig_dsds*)gui->g_dsds)->ds_baseColors->eles[0];
      param->dp_textdir   = dpp_textdir_new  (subpanel, &fdp->fdp_dir);
      param->dp_textcolor = dpp_textcolor_new(subpanel, &dpt->dpt_color);
      param->dp_textsize  = dpp_textsize_new (subpanel, &dpt->dpt_pts);
      param->dp_textfont  = dpp_textfont_new (subpanel, &dpt->dpt_font);
      param->dp_textangle = dpp_textangle_new(subpanel, &dpt->dpt_angle);
      param->dp_textLSF   = dpp_textLSF_new  (subpanel, NULL);
    butt = widM_button_subpanel_new(subpanel);
    wid_buttonPanel_addButton(panel,butt);

    wid_buttonPanel_build(panel,NULL);
    wid_buttonPanel_finish(panel);
    yfig_dpp_showonly(ymw,DPP_SHOW_Zoom);
    return panel;
}

extern void yfig_dppanel_update(Tyfig_mw* ymw)
{
    Tyfig_drawparam*         param = &ymw->ymw_currYDP;
    double zoom = od_getUsrZoom( fsod_getOD(ymw_fsod(ymw)) );

    dpp_zoom_setVal ( param->dp_zoom, zoom );
    dpp_depth_update( param->dp_depth, ymw );

    // line 
    dpp_linewidth_update( param->dp_linewidth, ymw );
    dpp_linecolor_update( param->dp_linecolor, ymw );
    dpp_linestyle_update( param->dp_linestyle, ymw );
    dpp_linearrow_update( param->dp_linearrow, ymw );

    // fill 
    dpp_l1l2color_update( param->dp_fillcolor, ymw );
    dpp_filltile_update ( param->dp_filltile,  ymw );

    // text 
    dpp_textdir_update  ( param->dp_textdir  , ymw );
    dpp_textcolor_update( param->dp_textcolor, ymw );
    dpp_textsize_update ( param->dp_textsize , ymw );
    dpp_textfont_update ( param->dp_textfont , ymw );
    dpp_textangle_update( param->dp_textangle, ymw );
    dpp_textLSF_update  ( param->dp_textLSF  , ymw );
}

/*======================================================================*/
