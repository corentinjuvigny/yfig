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
 *      $Id: tb-option.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/tb-option.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the configure menu.                                    =*/
/*======================================================================*/

#include "topbar.h"
#include "ydw/ydw.h"
#include "yfig-dsds.h"
#include "ydw/ydw.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "fig/object.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void action_ExportOption (Twidm_any*m, Twidm_entry* e);
static void action_PrintOption  (Twidm_any*m, Twidm_entry* e);
static void action_ScaleText    (Twidm_any*m, Twidm_entry* e);
static void action_ScaleStyle   (Twidm_any*m, Twidm_entry* e);
static void action_SymrotCenter (Twidm_any*m, Twidm_entry* e);

static Twidm_item table[] = {
//                me_label                        checkmark  me_action
  WIDM_ITEM_STR( "Export Parameters"               , 0,0   , action_ExportOption ),
  WIDM_ITEM_STR( "Print Parameters"                , 0,0   , action_PrintOption  ),
  WIDM_ITEM_STR( "Scale text fonts"                , 1,1   , action_ScaleText    ),
  WIDM_ITEM_STR( "Scale line thicknesses"          , 1,0   , action_ScaleStyle   ),
  WIDM_ITEM_STR( "Flip/rotate from object center"  , 1,1   , action_SymrotCenter ),
};

extern Twidm_item*  tb_option_t() { return table; }
extern int          tb_option_n() { return TOOL_TABLE_NbEle(table); }
//extern void         tb_option_update(Cyfig_mw*ymw, Twidm_any* m) { }

/*======================================================================*/

static void action_ExpPrtOption (Twidm_any*m, Twidm_entry* e, int flag)
{
    Tvgui*        gui  = m->ma_gui;
    Twidm_popup*  menu = e->me_menuPP;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    Tyfig_mw*     ymw   = menu->m_appCBud;
    Tyfig_dsds*   dsds  = ymw->ymw_dsds;
    TexportOption*eo    = flag==0 ? &dsds->ds_expOpt :  &dsds->ds_prtOpt;
    Tydw_expopt*    dialog = ydw_expopt_nbf(ymw,eo,flag,0,0);
    Twid_dialStatus status = ydw_expopt_run(dialog);
    if ( status==WID_DIALSTATUS_Ok )
        yfig_expopt_cpy(eo,ydw_expopt_get(dialog));
    ydw_expopt_delete(dialog);
    DBG1_MenuL("top->%s->%s status=%s\n",menu->m_label,e->me_label,vgui_dw_status2buttLab(status));
}
static void action_ExportOption (Twidm_any*m, Twidm_entry* e) { action_ExpPrtOption(m,e,0); }
static void action_PrintOption  (Twidm_any*m, Twidm_entry* e) { action_ExpPrtOption(m,e,1); }

/*======================================================================*/

static void action_ScaleText (Twidm_any*m, Twidm_entry* e)
{
    Tvgui*        gui  = m->ma_gui;
    Twidm_popup*  menu = e->me_menuPP;
    Tyfig_mw*     ymw   = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
    ymw->ymw_do_scaleText = e->me_checked;
    DBG1_MenuL("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
}

static void action_ScaleStyle (Twidm_any*m, Twidm_entry* e)
{
    Tvgui*        gui  = m->ma_gui;
    Twidm_popup*  menu = e->me_menuPP;
    Tyfig_mw*     ymw   = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
    ymw->ymw_do_scaleStyle = e->me_checked;
    DBG1_MenuL("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
}

static void action_SymrotCenter (Twidm_any*m, Twidm_entry* e)
{
    Tvgui*        gui  = m->ma_gui;
    Twidm_popup*  menu = e->me_menuPP;
    Tyfig_mw*     ymw   = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
    ymw->ymw_do_symrotCtr = e->me_checked;
    DBG1_MenuL("top->%s->%s : checked=%d\n",menu->m_label,e->me_label,e->me_checked);
}

/*======================================================================*/
