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
 *$Software: YFIG-TMB (Top Menu Bar)
 *      $Id: tb-debug.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/tb-debug.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the debug menu.                                        =*/
/*======================================================================*/

#include "topbar.h"
#include "ydw/tb-file-OMSF.h"
#include "yfig-mw.h"
#include "yfig-dsds.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "fig/object-priv.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void action_dumpcurrfig       (Twidm_any*m, Twidm_entry* e);
static void action_showModified      (Twidm_any*m, Twidm_entry* e);
static void action_chgFigUnitToCm    (Twidm_any*m, Twidm_entry* e);
static void action_toogleFBEmbedded  (Twidm_any*m, Twidm_entry* e);
static void action_submenu           (Twidm_any*m, Twidm_entry* e);
static void action_pathAdd           (Twidm_any*m, Twidm_entry* e);
static void action_pathSup           (Twidm_any*m, Twidm_entry* e);

static char* TA[]={ "A aaa","A bbb","A ccc ccc",NULL};
static char* TB[]={ "B aaa","B bbb","B ccc ccc",NULL};

static Twidm_item table[] = {
  WIDM_ITEM_STR   ("Dump current figure"                           , 0,0 , action_dumpcurrfig      ),
  WIDM_ITEM_STR   ("Show IsModified  flags"                        , 0,0 , action_showModified     ),
  WIDM_ITEM_STR   ("Chg fig unit to cm"                            , 0,0 , action_chgFigUnitToCm   ),
  WIDM_ITEM_STR   ("Toggle file browser embedded flag"             , 0,0 , action_toogleFBEmbedded ),
  WIDM_ITEM_STR_SM("A A A"                                         , 0,0 , action_submenu           , TA ),
  WIDM_ITEM_STR_SM("B B B"                                         , 0,0 , action_submenu           , TB ),
  WIDM_ITEM_STR   ("/home/ia/xfig/trunk-0.1.0/all.fig"             , 0,0 , action_pathAdd          ),
  WIDM_ITEM_STR   ("/home/ia/xfig/trunk-0.1.0/all-inch.fig"        , 0,0 , action_pathAdd          ),
  WIDM_ITEM_STR   ("/home/ia/xfig/trunk-0.1.0/all-cm.fig"          , 0,0 , action_pathAdd          ),
  WIDM_ITEM_STR   ("/home/ia/xfig/figfiles/flux.fig"               , 0,0 , action_pathAdd          ),
  WIDM_ITEM_STR   ("/home/ia/xfig/trunk-0.1.0/../figfiles/str.fig" , 0,0 , action_pathAdd          ),
  WIDM_ITEM_STR   ("sup path"                                      , 0,0 , action_pathSup          ),
};

extern Twidm_item*  tb_debug_t() { return table; }
extern int          tb_debug_n() { return TOOL_TABLE_NbEle(table); }

/*======================================================================*/
/*= Dump current figure                                                =*/

static void action_dumpcurrfig    (Twidm_any*m, Twidm_entry* e)
{
    mb_action_debug(m,e);
    Tvgui_mw* vmw  = m->ma_appCBud;
    Tyfig_mw* ymw  = m->ma_appCBud;
    Tfigure*  fig  = yfig_cursce_fig(ymw);
    obj_dump( obj_ObjAll(fig->objects), 0 );
}

static void action_showModified    (Twidm_any*m, Twidm_entry* e)
{
    mb_action_debug(m,e);
    Tyfig_mw* ymw  = m->ma_appCBud;
    printf(
      "%s: ymw_currHistSave=%2d ymw_currHistCurr=%2d isModified(ymw)=%d\n",
      __func__,ymw->ymw_currHistSave,ymw->ymw_currHistCurr,
      yfig_cursce_isModified(ymw)
    );
}

/*======================================================================*/

#include "yfig-device.h"
#include "vgui/wid-canvas.h"

static void action_chgFigUnitToCm    (Twidm_any*m, Twidm_entry* e)
{
void yfig_cursce_newUpdate(Tyfig_mw*ymw); // set to static when suppressed
    mb_action_debug(m,e);
    Tyfig_mw*      ymw  = m->ma_appCBud;
    Tyfig_device*  fsod = ymw_fsod( ymw );
    Tfigure*       fig  = fig_new_clone( fsod_getFigure(fsod) );
    fig_chgUnit(fig,1.*2540);
    Tfig_scene* scene = fig_scene_newByFig(fig);
    fsod_detachAttach( fsod, scene, 2 ); // 1: real size
    yfig_cursce_newUpdate( ymw );
    yfig_cursce_redraw( ymw );
}

/*======================================================================*/

static void action_toogleFBEmbedded  (Twidm_any*m, Twidm_entry* e)
{
    Tvgui* gui = m->ma_gui;
    gui->g_unsprtEmbeddedFb = gui->g_unsprtEmbeddedFb!=0 ? 0 : 1;
    printf("%s: gui->g_unsprtEmbeddedFb=%d\n",__func__,gui->g_unsprtEmbeddedFb);
}

/*======================================================================*/

static void action_submenu  (Twidm_any*m, Twidm_entry* me)
{
    mb_action_debug(m,me);
    printf("%s: %s:%s ident=%d %d\n",__func__,me->me_menu->ma_pentry->me_label,me->me_label,me->me_ident.sint,me->me_iud);
}

/*======================================================================*/

#include "tools/file.h"
static void action_pathAdd  (Twidm_any*m, Twidm_entry* me)
{
    Tyfig_mw*      ymw  = m->ma_appCBud;
    Tyfig_dsds*    dsds = ymw->ymw_dsds;
    printf("%s: %s\n",__func__,me->me_label);
    tool_rf_addClone(dsds->ds_recentFiles, me->me_label);
    yfig_topbar_update(ymw,YFIG_TB_FILE);
}

static void action_pathSup  (Twidm_any*m, Twidm_entry* me)
{
    Tyfig_mw*      ymw  = m->ma_appCBud;
    Tyfig_dsds*    dsds = ymw->ymw_dsds;
    printf("%s: %s\n",__func__,me->me_label);
    tool_rf_chgSize( dsds->ds_recentFiles, dsds->ds_recentFiles->rf_cacheSize-1);
    yfig_topbar_update(ymw,YFIG_TB_FILE);
}

/*======================================================================*/
