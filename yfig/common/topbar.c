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
 *      $Id: topbar.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the top menu bar.                                      =*/
/*======================================================================*/

#include "topbar/topbar.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= creation of top menu bar                                           =*/

extern Twidm_bar* yfig_topbar_nbf(Tyfig_mw* ymw)
{
  Twidm_bar* bar = widmM_tmb_nbx(&ymw->any);

  widm_tmb_addMenuByItemT(bar,
        "Open, Save, Export file, Quit, ...",
        tb_file_t(),tb_file_n(), "File", 0, ymw);
  widm_tmb_addMenuByItemT(bar,
        "Undo, Redo, Copy, Paste, Settings, ...",
        tb_edit_t(),tb_edit_n(), "Edit", 0, ymw);
  widm_tmb_addMenuByItemT(bar,
        "Configure the YFIG tool.",
        tb_option_t(),tb_option_n(), "Options", 0, ymw);
   ymw->ymw_do_useFigUnit = 1;
   ymw->ymw_do_scaleText  = tb_option_t()[2].me_checked;
   ymw->ymw_do_scaleStyle = tb_option_t()[3].me_checked;
   ymw->ymw_do_symrotCtr  = tb_option_t()[4].me_checked;

#if defined(TOOL_DBG_Dev) && TOOL_DBG_Dev
  widm_tmb_addMenuByItemT(bar,
        "Miscellaneous debug",
        tb_debug_t(),tb_debug_n(), "Debug", 0, ymw);
#endif

  widm_tmb_finish(bar);

  return bar;
}

extern  void yfig_topbar_delete(Tyfig_mw* ymw, Twidm_bar* tb) { widm_tmb_delete( tb ); }

/*======================================================================*/
/*= update the top menu bar                                            =*/

extern void yfig_topbar_update(Cyfig_mw* ymw, int mask)
{
    Twidm_bar* bar = ymw->ymw_menuBar;
    if (mask&YFIG_TB_FILE)
        tb_file_update(ymw, &bar->mb_menus[0]->any);
    if (mask&YFIG_TB_EDIT)
        tb_edit_update(ymw, &bar->mb_menus[1]->any);
}

/*======================================================================*/
/*= miscellaneous menu utilities                                       =*/ 

extern void mb_action_debug(Twidm_any*m, Twidm_entry*e)
{
    Twidm_popup* menu = e->me_menuPP;
    DBG0_Menu("Top Bar entry selected: top->%s->%s\n",
            menu->m_label,e->me_label);
}

/*======================================================================*/
