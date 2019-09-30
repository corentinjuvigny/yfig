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
 *      $Id: topbar.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/topbar.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the top menu bar.                                      =*/
/*=                                                                    =*/
/*= This utility amounts only to the yfig_topbar_nbf function that is  =*/
/*= builds the top menu bar and returns it.                            =*/
/*=                                                                    =*/
/*= This header is included and ONLY by the "topbar.c" file all the    =*/
/*= topbar/*.c sources.                                                =*/
/*=                                                                    =*/
/*= It does the common includes and then it defines for each XXX menu  =*/
/*= the 2 following functions:                                         =*/
/*=   extern Twidm_entry* tb_XXX_t();                                  =*/
/*=   extern int          tb_XXX_n();                                  =*/
/*=   extern void         tb_XXX_update(Cyfig_mw*ymw, Twidm_any* m);   =*/
/* that return respectively the entry table of the menu and the        =*/
/* number of elements of the table.                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOPBAR_TOPBAR_H
#define FILE_TOPBAR_TOPBAR_H
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"

#include "vgui/vgui.h"
#include "vgui/wid-simple.h" // for WID_DIALSTATUS_xxx
#include "vgui/wid-menu.h"

// shortcut to declare variables in callback
#define ACTMM_V_YMW(m,e)  Tyfig_mw* ymw = m->ma_appCBud
#define ACTMM_V_FSOD(m,e) ACTMM_V_YMW(m,e); Tyfig_device* fsod = YMW_ydev(ymw); Toutdev* od = fsod_getOD(fsod)
#define ACTMM_V_COBJ(m,e) ACTMM_V_FSOD(m,e); TobjAll obj = fsod_getCurrObj(fsod); CboxDble*boxObj = obj_BBox(obj)
#define ACTMM_V_TOBJ(m,e) ACTMM_V_FSOD(m,e); TobjAll obj = obj_ObjAll(fsod_getTopObj(fsod)); CboxDble*boxObj = obj_BBox(obj)

#define ACTSM_V_YMW(m,e)  Tyfig_mw* ymw = m->ma_pentry->me_menu->ma_appCBud
#define ACTSM_V_FSOD(m,e) ACTSM_V_YMW(m,e); Tyfig_device* fsod = YMW_ydev(ymw); Toutdev* od = fsod_getOD(fsod)
#define ACTSM_V_COBJ(m,e) ACTSM_V_FSOD(m,e); TobjAll obj = fsod_getCurrObj(fsod); CboxDble*boxObj = obj_BBox(obj)
#define ACTSM_V_TOBJ(m,e) ACTSM_V_FSOD(m,e); TobjAll obj = obj_ObjAll(fsod_getTopObj(fsod)); CboxDble*boxObj = obj_BBox(obj)

/*======================================================================*/
/*= the menus                                                          =*/

extern Twidm_item*  tb_file_t();
extern int          tb_file_n();
extern void         tb_file_update(Cyfig_mw*ymw, Twidm_any* m);

extern Twidm_item*  tb_edit_t();
extern int          tb_edit_n();
extern void         tb_edit_update(Cyfig_mw*ymw, Twidm_any* m);

extern Twidm_item*  tb_option_t();
extern int          tb_option_n();

extern Twidm_item*  tb_debug_t();
extern int          tb_debug_n();

/*======================================================================*/
/*= miscellaneous menu utilities                                       =*/ 

#define DBG_MenuFmt "Top Bar entry selected: top->%s->%s"
#define DBG_MenuPar        DBG_MenuParME(menu,e)
#define DBG_MenuParE(e)    DBG_MenuParME(menu,e)
#define DBG_MenuParME(m,e) (m)->m_label,e->me_label
#define DBG_MenuParMAE(m,e) DBG_MenuParME((Twidm_popup*)(m),e)

#define DBG_MenuSelected(e)     DBG0_Menu(  DBG_MenuFmt,DBG_MenuParE(e))
#define DBG0_MenuSelectedE(e)   DBG0_MenuE( DBG_MenuFmt,DBG_MenuParE(e))
#define DBG1_MenuSelectedL(e)   DBG1_MenuL( DBG_MenuFmt,DBG_MenuParE(e))
#define DBG0_MenuSelectedMAE(m,e) DBG0_MenuE( DBG_MenuFmt,DBG_MenuParMAE(m,e))
#define DBG1_MenuSelectedMAL(m,e) DBG1_MenuL( DBG_MenuFmt,DBG_MenuParMAE(m,e))

/**
 * The mb_action_debug function performs DBG_MenuSelected(e) and
 * then returns
**/
extern void mb_action_debug(Twidm_any*m, Twidm_entry*e);

#define NoPxm .me_pixmap={.ulong=PXMEGS_NONE}

/*======================================================================*/
/*= debug                                                              =*/ 

#define DBG0_Menu(fmt,...) DBG0_printf("menu",fmt,__VA_ARGS__)
#define DBG1_Menu(fmt,...) DBG1_printf("menu",fmt,__VA_ARGS__)
#define DBG2_Menu(fmt,...) DBG2_printf("menu",fmt,__VA_ARGS__)

#define DBG0_MenuE(fmt,...) DBG0_printf("menu","Enter : "fmt,__VA_ARGS__)
#define DBG0_MenuM(fmt,...) DBG0_printf("menu","Middle: "fmt,__VA_ARGS__)
#define DBG0_MenuL(fmt,...) DBG0_printf("menu","Leave : "fmt,__VA_ARGS__)
#define DBG1_MenuE(fmt,...) DBG1_printf("menu","Enter : "fmt,__VA_ARGS__)
#define DBG1_MenuM(fmt,...) DBG1_printf("menu","Middle: "fmt,__VA_ARGS__)
#define DBG1_MenuL(fmt,...) DBG1_printf("menu","Leave : "fmt,__VA_ARGS__)
#define DBG2_MenuE(fmt,...) DBG2_printf("menu","Enter : "fmt,__VA_ARGS__)
#define DBG2_MenuM(fmt,...) DBG2_printf("menu","Middle: "fmt,__VA_ARGS__)
#define DBG2_MenuL(fmt,...) DBG2_printf("menu","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
#endif // FILE_TOPBAR_TOPBAR_H
/*======================================================================*/
