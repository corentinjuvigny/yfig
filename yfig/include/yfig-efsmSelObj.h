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
 *      $Id: yfig-efsmSelObj.h 280 2018-07-05 23:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-efsmSelObj.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A EFSM specialization that selects an object within a marker point.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_EFSMSELOBJ_H
#define FILE_YFIG_EFSMSELOBJ_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-efsm.h"

/*======================================================================*/

typedef struct _TefsmSel1Obj  TefsmSel1Obj;
typedef const   TefsmSel1Obj  CefsmSel1Obj;

/**
 * This function is called when the user selected the obj object.
 * The click parameter gives the button the user clicked for selecting
 * the object (1:left button, 2:middle button, 3:right button).
 * The m parameter is the marker point of the selected object.
 *
 * Furthermore notice
 *   - that click, obj and m are also stored in the fsm structure in
 *     the fsm_selClick, selObj and selMark fields of the fsm.
 *   - that the exact position of the pointer is stored in the
 *     fsm_selShpt field of the fsm.
 *   - that the m marker index (identifier of the maker in the object)
 *     is stored in the fsm_selMarkIdx field of the fsm.
**/ 
typedef void (*TefsmSel1Obj_selFunc) (TefsmSel1Obj*a, int click,
                                      TobjAll obj, Tdble2 marker);
/**
 * This function is called when the user click a button in a position
 * that does not select any object.
 * The click parameter gives the button the user clicked (1:left button,
 * 2:middle button, 3:right button).
 * The p parameter gives the pointer position where the click occurs.
**/ 
typedef void (*TefsmSel1Obj_posFunc) (TefsmSel1Obj*a, int click, Tdble2 p);

/*======================================================================*/
/*= the FSM structure                                                  =*/

struct _TefsmSel1Obj {
    Tvgui_efsm    any;
    TefsmSel1Obj_selFunc fsm_selfunc;
    TefsmSel1Obj_posFunc fsm_posfunc;

    int        fsm_objMask;    // the objects that can be selected
    int        fsm_noCtrMkr;   // ignore markers that are also object center
    int        fsm_mkrOpeMask; // ignore object that do not support these
                               // marker operations (see FIGO_MkrOpe_...)
               
    int        fsm_selClick;   // the button that selected the object
    TobjAll    fsm_selObj;     // the selected object
    Tdble2     fsm_selShpt;    // the selected point on object shape
    Tdble2     fsm_selMark;    // the coordinates of the marker the nearest to
                               // the selected point (fsm_selShpt).
    int        fsm_selMarkIdx; // the index in the object marker table of
                               // the selected marker.


  /**
   * Features for drawing axis
  **/
    Tdble2  fsm_AxisPos;    // position of axis
    char    fsm_AxisHLine;  // 0/1: disabled/draw line y=fsm_AxisPos.y
    char    fsm_AxisVLine;  // 0/1: disabled/draw line x=fsm_AxisPos.x
    char    fsm_AxisDiag;   // 0/1: disabled/draw diagonal lines in fsm_AxisPos
};

/**
 * The ygui_efsmSel1Obj_new function allocates an FSM and attaches it
 * to the dev canvas.
 * Its behavior is:
 *   - It selects the objects drawn on the canvas using the mask parameter.
 *     The mask parameter must be an or of YSC_MM_xx flags.
 *   - Then it draws the markers of the selected objects.
 *   - Every times one clicks a marker, it calls the func(self,click,obj,m)
 *     function (see above).
 *
 * Return:
 *   The ygui_efsmSel1Obj_new function returns a pointer to the created
 *   FSM.
**/
extern TefsmSel1Obj* yfig_efsmSel1Obj_new(Twid_canvas* cvs,
              int mask, TefsmSel1Obj_selFunc func, void* ud);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_YFIG_EFSMSELOBJ_H
/*======================================================================*/
