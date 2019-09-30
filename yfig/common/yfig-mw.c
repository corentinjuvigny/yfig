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
 *$Software: YFIG
 *      $Id: yfig-mw.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-mw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data structure for top YFIG window.                                =*/
/*=                                                                    =*/
/*= See the yfig-mw.h" header for documentation.                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-device.h"
#include "yfig-dsds.h"
#include "ydw/ydw-update.h"

#include "vgui/wid-simple.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-menu.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/vgui.h"

#include "tools/globalvars.h"
#include "tools/messagedriver.h"
#include "tools/strings.h"

/*======================================================================*/

extern Tyfig_dsds* yfig_mw_getDsds(Cyfig_mw* ymw) { return ymw->ymw_gui->g_ud; }
extern CcolorSet*  yfig_mw_getClrs(Cyfig_mw*ymw)  { return yfig_mw_getDsds(ymw)->ds_baseColors; }
extern Tfig_scene* YMW_scene (Cyfig_mw* ymw) { return fsod_getScene(  ymw_ydev(ymw) ); }
extern Tfigure*    YMW_figure(Cyfig_mw* ymw) { return fsod_getFigure( ymw_ydev(ymw) ); }

/*======================================================================*/

static void yfig_mw_setInputEnable(Tvgui_mw* vmw, int on);

#include "tools/color.h"

static Tyfig_mw* yfig_mw_new (Tvgui* gui)
{
    Tyfig_dsds* dsds = gui->g_ud;
    Tyfig_mw*   ymw  = tool_malloc_0( sizeof(*ymw) );

    ymw->any.gui  = gui;
    ymw->any.setInputEnable = yfig_mw_setInputEnable;

    // init. currDP
    ymw->ymw_currDP.fdp_depth  = 50;
    ymw->ymw_currDP.fdp_text   = odsty_copyText(&dsds->ds_styText);
    ymw->ymw_currDP.fdp_line   = odsty_copyLine(&dsds->ds_styLine);
    ymw->ymw_currDP.fdp_fill   = odsty_copyFill(&dsds->ds_styFill);

    // init. for arrow
    ymw->ymw_arrowbeg = dsds->ds_begArrow;
    ymw->ymw_arrowend = dsds->ds_endArrow;

    // other init. 
    ymw->ymw_userUnit = dsds->ds_userUnit;

    return ymw;
}

static void yfig_mw_create    (Tyfig_mw* ymw)
{
    Tvgui* gui = ymw->any.gui;
    // get verbosity level of message driver widget
    int errlvl = gl_msgdrv_default.errlvl;
    if ( errlvl<2 ) errlvl=2;

    ymw->any.widEGS    =  gui_yfig_create(gui);
    ymw->ymw_menuBar   =  yfig_topbar_nbf(ymw);
    vgui_mw_msgdrv_initWidLvl(&ymw->any,
                          widM_slout_nbf(gui,ymw->any.widEGS,0,0),errlvl);
    ymw->ymw_mouseFun  =  wid_mfun_create(&ymw->any,ymw->any.widEGS);
    ymw->ymw_modePanel =  yfig_modePanel_nbf(ymw);
    ymw->ymw_canvas    =  widM_canvas_nbf0(&ymw->any,ymw->any.widEGS, 0,300,300,0);
    ymw->ymw_update    =  ydw_update_nbfTop(ymw);
    ymw->ymw_dpPanel   = yfig_dppanel_nbf(ymw);
    ymw->ymw_layers    = yfig_layers_nbf(ymw,NULL);
    gui_yfig_createend( ymw );
}

extern Tyfig_mw* yfig_mw_nbf (Tvgui* gui)
{
    Tyfig_mw* ymw = yfig_mw_new( gui );
    yfig_mw_create( ymw );
    //yfig_cursce_new( ymw );
    return ymw;
}

extern int yfig_mw_delete (Tyfig_mw* ymw)
{
    if ( yfig_cursce_delete( ymw )==0 )
        // the user canceled the quit process 
        return 0;

    yfig_layers_delete   ( ymw, ymw->ymw_layers    );
    wid_buttonPanel_free (      ymw->ymw_dpPanel   );
    ydw_update_delete    ( ymw, ymw->ymw_update    );
    wid_canvas_free      (      ymw->ymw_canvas    );
    wid_buttonPanel_free (      ymw->ymw_modePanel );
    wid_mfun_delete      (      ymw->ymw_mouseFun  );
    vgui_mw_msgdrv_free  ( &ymw->any );
    yfig_topbar_delete   ( ymw, ymw->ymw_menuBar );
    gui_yfig_delete      ( ymw );

    figdp_free (&ymw->ymw_currDP  );
    tool_free  ( ymw->ymw_OMSFdir );
    tool_free  ( ymw->ymw_expDir  );
    free ( ymw );
    return 1;
}

static void yfig_mw_setInputEnable (Tvgui_mw* vmw, int on)
{
    Tyfig_mw* ymw = (Tyfig_mw*)vmw;
    if ( on==1 ) {
        widm_tmb_enable( ymw->ymw_menuBar );
        wid_buttonPanel_enable( ymw->ymw_modePanel );
        //wid_buttonPanel_enable( ymw->ymw_dpPanel );
    } else {
        widm_tmb_disable( ymw->ymw_menuBar );
        wid_buttonPanel_disable( ymw->ymw_modePanel );
        //wid_buttonPanel_disable( ymw->ymw_dpPanel );
    }
}

/*======================================================================*/

extern void      yfig_mw_titleUpdate(Cyfig_mw* ymw)
{
   char pn[100]; strcpy(pn,PACKAGE); tool_strtoupper(pn);
   Cstr path = yfig_cursce_saveFilename(ymw);
   if ( TOOL_strIsEmpty(path) ) {
        gui_yfig_setTitle(ymw,pn);
   } else {
        TstrEaten dir,base; tool_dirbasename(path,&dir,&base);
        TstrEaten title = tool_aprintf("%s : %s : %s",pn,base,dir);
        gui_yfig_setTitle(ymw,title);
        free(title); free(base); free(dir);
   }
}

/*======================================================================*/

extern void yfig_canvas_updateUnit          (Tyfig_mw*ymw, CuserUnit* uu)
{
    if ( uu!=0 ) {
        ymw->ymw_userUnit = *uu;
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    wid_canvas_unitSet( ymw->ymw_canvas, &ymw->ymw_userUnit);
}

extern void yfig_canvas_updateMinorGridTick (Tyfig_mw*ymw, double stn);
extern void yfig_canvas_updateMagnetGridTick(Tyfig_mw*ymw, double stn);
extern void yfig_canvas_update              (Tyfig_mw*ymw, double zf);

extern void yfig_canvas_enableAxis          (Tyfig_mw*ymw, int on) { wid_canvas_axisEnable(ymw->ymw_canvas, on); }
extern void yfig_canvas_enableGrid          (Tyfig_mw*ymw, int on);
extern void yfig_canvas_enableMagnet        (Tyfig_mw*ymw, int on);

/*======================================================================*/
