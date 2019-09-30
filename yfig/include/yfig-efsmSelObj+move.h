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
 *      $Id: yfig-efsmSelObj+move.h 280 2018-07-05 23:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-efsmSelObj+move.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A EFSM specialization that selects an object within a marker point
 * and then gives the new position of the marker.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_EFSMSELOBJ_MOVE_H
#define FILE_YFIG_EFSMSELOBJ_MOVE_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-efsm.h"

/*======================================================================*/

typedef struct _TefsmSel1ObjMv  TefsmSel1ObjMv;
typedef const   TefsmSel1ObjMv  CefsmSel1ObjMv;

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
typedef void (*TefsmSel1ObjMv_selFunc) (TefsmSel1ObjMv*fsm, int click,
                                        TobjAll obj, Tdble2 m);

/**
 * After the user selected an object, this function is called when
 * the user moves the mouse pointer.
 * The p point gives the new position of the mouse pointer.
**/ 
typedef void (*TefsmSel1ObjMv_posFunc) (TefsmSel1ObjMv*fsm, Tdble2 p);

/**
 * After the user selected an object, this function is called when
 * the user clicks a mouse button.
 * The p point gives the new position of the mouse pointer and the
 * click parameter gives the button the user clicked (1:left button,
 * 2:middle button, 3:right button).
**/ 
typedef void (*TefsmSel1ObjMv_finFunc) (TefsmSel1ObjMv*fsm, int click,
                                        Tdble2 p);

/*======================================================================*/
/*= the FSM structure                                                  =*/

struct _TefsmSel1ObjMv {
    Tvgui_efsm    any;

    // the callbacks
    TefsmSel1ObjMv_selFunc fsm_selfunc;
    int                    fsm_selAbort; // fsm_selfunc must set it to 1
                                         // for refusing the selection.
    TefsmSel1ObjMv_posFunc fsm_posfunc;
    TefsmSel1ObjMv_finFunc fsm_finfunc;

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

    // user data
    Tdble2        fsm_prevTv;
    TobjAll       fsm_prevObj;
};

/**
 * The ygui_efsmSel1ObjMv_new function allocates an FSM and attaches it
 * to the dev canvas.
 * Its behavior is:
 *   - It selects the objects drawn on the canvas using the mask parameter.
 *     The mask parameter must be an or of YSC_MM_xx flags.
 *   - Then it draws the markers of the selected objects.
 *   - When one clicks a marker, it calls the sel(self,click,obj,m)
 *     function, click indicating the clicked mouse button, m being the
 *     position of the selected marker and obj being the object the m
 *     marker belongs to.
 *   - Then the function mov(self,m) is called every time the
 *     position of the mouse pointer changes.
 *   - When one clicks a mouse button, it calls the finish(self,click,m)
 *     and restart the FSM.
 *
 * Return:
 *   The ygui_efsmSel1ObjMv_new function returns a pointer to the created
 *   FSM.
**/
extern TefsmSel1ObjMv* yfig_efsmSel1ObjMv_new(Twid_canvas* dev,
              int mask, void* ud,
              TefsmSel1ObjMv_selFunc sel,
              TefsmSel1ObjMv_posFunc move,
              TefsmSel1ObjMv_finFunc finish);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_YFIG_EFSMSELOBJ_MOVE_H
/*======================================================================*/
