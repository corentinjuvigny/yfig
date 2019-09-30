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
 *      $Id: yfig-efsmSelNObj.h 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-efsmSelNObj.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A EFSM specialization that selects either one object or all the
 * objects inside a rectangular area.
 *
 *  - button 1: a single object
 *  - button 2: first corner of an area
 *        * button 1/2: second corner of area --> all the object in area
 *        * button 3:   cancel.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_EFSMSELNOBJ_H
#define FILE_YFIG_EFSMSELNOBJ_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-efsm.h"

/*======================================================================*/

typedef struct _TefsmSelNObj  TefsmSelNObj;
typedef const   TefsmSelNObj  CefsmSelNObj;

/**
 * This function is called once for every object that the user selected.
**/ 
typedef void (*TefsmSelNObj_sel1Func) (TefsmSelNObj*fsm, TobjAll obj);

/**
 * This function is called once for every area that the user selected.
**/ 
typedef void (*TefsmSelNObj_selNFunc) (TefsmSelNObj*fsm, CobjAllSet* obj);

/*======================================================================*/
/*= the FSM structure                                                  =*/

struct _TefsmSelNObj {
    Tvgui_efsm    any;
    TefsmSelNObj_sel1Func fsm_sel1func;
    TefsmSelNObj_selNFunc fsm_selNfunc;

    int       fsm_objMask;      // the object that can be selected

    Tdble2    fsm_regPt1;
    Tdble2    fsm_regPt2;
};

/**
 * The ygui_efsmSelNObj_new function allocates an FSM and attaches it
 * to the dev canvas.
 *
 * Return:
 *   The ygui_efsmSelNObj_new function returns a pointer to the created
 *   FSM.
**/
extern TefsmSelNObj* yfig_efsmSelNObj_new(Twid_canvas* dev,
              int mask, void* ud,
              TefsmSelNObj_sel1Func f1, TefsmSelNObj_selNFunc fn);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_YFIG_EFSMSELNOBJ_H
/*======================================================================*/
