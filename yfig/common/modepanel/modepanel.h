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
 *$Software: YFIG-MP (Mode Panel)
 *      $Id: modepanel.h 315 2018-07-19 15:28:59Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/modepanel.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the mode panel.                                        =*/
/*=                                                                    =*/
/*= The user chooses a mode, then he can do operations corresponding   =*/
/*= on the canvas. The operations (mode) are for instance: object      =*/
/*= creation, object move, object remove.                              =*/
/*=                                                                    =*/
/*= The mode panel is a panel of On/off buttons (Tvgui_buttonPanel     =*/
/*= type), each button corresponding to a mode.                        =*/
/*=                                                                    =*/
/*= The main points of software organization are:                      =*/
/*=  * "modepanel/modelpanel.h" file                                   =*/
/*=     - it is included in all the mode panel sources.                =*/ 
/*=     - it is not included in any other sources.                     =*/ 
/*=     - it does the common includes.                                 =*/ 
/*=     - for every XXX mode it contains the declarations:             =*/
/*=          extern void mp_XXX_cb(Tvgui_mw* vmv, void* ud);           =*/
/*=          extern Cstr mp_XXX_small[];                               =*/
/*=          extern Cstr mp_XXX_big[];                                 =*/
/*=       They correspond to the callback function, the bits of the    =*/
/*=       small and big bitmap.                                        =*/
/*=  * "modepanel/modeX.c" file                                        =*/
/*=     - it includes "modelpanel.h".                                  =*/
/*=     - it contains the definitions of the mp_XXX_cb function and    =*/
/*=       of the mp_XXX_small/big arrays.                              =*/
/*=     - Theses 3 symbols are the only entries to the file.           =*/
/*=       Indeed all the other functions must be set static.           =*/
/*=    Note: a such file can group several modes that are similar.     =*/
/*=    For instance "modepanel/ellipse.c" groups the mode "ellipse by  =*/
/*=    radius" and "ellipse by bounding box".                          =*/
/*=  * "modepanel-icons/xxx.h" files                                   =*/
/*=    It contains the bitmaps of the buttons under the X11 bitmap     =*/
/*=    format. They are not used by the software.                      =*/ 
/*= * "modepanel.c" file                                               =*/
/*=    Its only entry is the yfig_modepanl_nbf functions that creates  =*/
/*=    the mode panels using the definitions of the "modepanel.h"      =*/
/*=    file.                                                           =*/
/*= * "mp_XXX_cb" functions                                            =*/
/*=    - the true prototype is:                                        =*/
/*=          extern void mp_XXX_cb(Tyfig_mw* ymv, Tygui_efs** ret);    =*/
/*=      Its rule is to reate an EFSM and to returns it in the *ret    =*/
/*=      parameter.                                                    =*/
/*=    - the standard algorithm is:                                    =*/
/*=          extern void mp_XXX_cb(Tvgui_mw* vmv, void* ud)            =*/
/*=          {                                                         =*/
/*=            Tyfig_mw*   ywm = (Tyfig_mw*)wm);                       =*/
/*=            Twid_efsm** ret = (Twid_efsm**)ud;                      =*/
/*=                                                                    =*/
/*=            // create and initialize an EFSM                        =*/
/*=            T..efsm..*  fsm = ..efsm.._new(...);                    =*/
/*=            ...                                                     =*/
/*=                                                                    =*/
/*=            *ret = fsm;                                             =*/
/*=          }                                                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_MODEPANEL_MODEPANEL_H
#define FILE_MODEPANEL_MODEPANEL_H
/*======================================================================*/


#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-device.h"

#include "vgui/vgui-efsm.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-button.h"
#include "vgui/wid-canvas.h"
#include "fig/object.h"

/*======================================================================*/

#define CUstr const unsigned char

extern void yfig_modePanel_chgMode(
         Twid_buttonPanel*self, Twid_button*next);

/*======================================================================*/
/*= The bitmap sizes.                                                  =*/

#define MP_BM_SMALL_DX 22
#define MP_BM_SMALL_DY 22

#define MP_BM_BIG_DX   36
#define MP_BM_BIG_DY   32

/*======================================================================*/
/*= The modes.                                                         =*/

extern CUstr mp_ellrad_small[];
extern CUstr mp_ellrad_big[];
extern void  mp_ellrad_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_elldia_small[];
extern CUstr mp_elldia_big[];
extern void  mp_elldia_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_lineclosed_small[];
extern CUstr mp_lineclosed_big[];
extern void  mp_lineclosed_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_lineopen_small[];
extern CUstr mp_lineopen_big[];
extern void  mp_lineopen_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_splineclosed_small[];
extern CUstr mp_splineclosed_big[];
extern void  mp_splineclosed_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_splineopen_small[];
extern CUstr mp_splineopen_big[];
extern void  mp_splineopen_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_splineclosedInt_small[];
extern CUstr mp_splineclosedInt_big[];
extern void  mp_splineclosedInt_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_splineopenInt_small[];
extern CUstr mp_splineopenInt_big[];
extern void  mp_splineopenInt_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_sqrrect_small[];
extern CUstr mp_sqrrect_big[];
extern void  mp_sqrrect_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_arcrect_small[];
extern CUstr mp_arcrect_big[];
extern void  mp_arcrect_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_arcopen_small[];
extern CUstr mp_arcopen_big[];
extern void  mp_arcopen_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_arcpop_small[];
extern CUstr mp_arcpop_big[];
extern void  mp_arcpop_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_text_small[];
extern CUstr mp_text_big[];
extern void  mp_text_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_image_small[];
extern CUstr mp_image_big[];
extern void  mp_image_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_cpdDo_small[];
extern CUstr mp_cpdDo_big[];
extern void  mp_cpdDo_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_cpdBreak_small[];
extern CUstr mp_cpdBreak_big[];
extern void  mp_cpdBreak_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_cpdOpen_small[];
extern CUstr mp_cpdOpen_big[];
extern void  mp_cpdOpen_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_zoom_small[];
extern CUstr mp_zoom_big[];
extern void  mp_zoom_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_align_small[];
extern CUstr mp_align_big[];
extern void  mp_align_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_move_small[];
extern CUstr mp_move_big[];
extern void  mp_move_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_copy_small[];
extern CUstr mp_copy_big[];
extern void  mp_copy_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_delete_small[];
extern CUstr mp_delete_big[];
extern void  mp_delete_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_mkrmov_small[];
extern CUstr mp_mkrmov_big[];
extern void  mp_mkrmov_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_mkradd_small[];
extern CUstr mp_mkradd_big[];
extern void  mp_mkradd_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_mkrdel_small[];
extern CUstr mp_mkrdel_big[];
extern void  mp_mkrdel_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_fliph_small[];
extern CUstr mp_fliph_big[];
extern void  mp_fliph_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_flipv_small[];
extern CUstr mp_flipv_big[];
extern void  mp_flipv_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_rotCW_small[];
extern CUstr mp_rotCW_big[];
extern void  mp_rotCW_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_rotCCW_small[];
extern CUstr mp_rotCCW_big[];
extern void  mp_rotCCW_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_rota_small[];
extern CUstr mp_rota_big[];
extern void  mp_rota_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_update_small[];
extern CUstr mp_update_big[];
extern void  mp_update_cb(Tvgui_mw* vmv, int uid, void* ud);

extern CUstr mp_edit_small[];
extern CUstr mp_edit_big[];
extern void  mp_edit_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_arrowheads_small[];
extern CUstr mp_arrowheads_big[];
extern void  mp_arrowheads_cb(Tvgui_mw* vmv, int uid, void* ud);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern CUstr mp_measurelength_small[];
extern CUstr mp_measurelength_big[];
extern void  mp_measurelength_cb(Tvgui_mw* vmv, int uid, void* ud);

/*======================================================================*/
#endif // FILE_MODEPANEL_MODEPANEL_H
/*======================================================================*/
