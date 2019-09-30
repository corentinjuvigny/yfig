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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-efsm-points.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-efsm-points.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * FSM definition for getting a point set.
 *
 * The Tvgui_efsmPTS class specializes the Tvgui_efsm class.
 *
 * The specialization consists of:
 *   1) building a set of points by clicking in the drawing area.
 *   2) calling the treatment function when the set is validated.
 *   3) starting again at step 1)
 *
 * The interface allows to create 2 EFSM kinds:
 *   - vgui_efsmPTS_new_Nexact: A EFSM that calls the treatment function
 *     when the points set has exactly N poins.
 *   - vgui_efsmPTS_new_NorMore: A EFSM that calls the treatment function
 *     when the points set has at least N poins (N>2).
 * The created EFSM can be deleted after use with the vgui_efsm_free
 * function.
 *
 * One can customizes the EFSM for:
 *  - drawing a skeleton in the canvas (a multi-line joining the selected
 *    points. See the fsm->skeletonMode field.
 *  - drawing a preview of target object by defining the fsm->drawSetFuncs
 *    function. This is called every times a point is added to the set.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_EFSM_POINTS_H
#define FILE_VGUI_EFSM_POINTS_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-efsm.h"

/*======================================================================*/
/*= Types.                                                             =*/

typedef struct _Tvgui_efsmPTS  Tvgui_efsmPTS;
typedef const   Tvgui_efsmPTS  Cvgui_efsmPTS;

typedef void (*Tvgui_efsmPTS_drawSetFunc) (Tvgui_efsmPTS*fsm,
                                 int click1, Cdble2*pts, int ptNb);

struct _Tvgui_efsmPTS {
    Tvgui_efsm  any;

  /* these parameter allows to enable a draft preview */
    // preview or clear default parameter.
    int         skeletonMode; // 0:None 
                              // 1:multi-line
                              // 2:closed multi-line
    Tod_GCdef   skeletonGcd;  // the GC to use 

  /* these parameter allows to enable a custom preview */

    // preview or clear (use a xored GC) the drawing for all
    // the selected points (ptNb>=2)
    Tvgui_efsmPTS_drawSetFunc drawSetFunc;

  /* these parameter stores the FSM states */
    int            firstClick;   // 1=button1 2=button2 3=button3
    int            nb;           // for exactly N points FSM
                                 //  <1: forbidden
                                 //   1:means exactly 1 points,
                                 //   2:means exactly 2 points,
                                 //    and so one.
                                 // for at least N points FSM
                                 //  <2: forbidden
                                 //   2:means at least 2 points,
                                 //   3:means at least 3 points,
                                 //    and so one.
    Tdble2Set* points;           // where points are stored

    // User data: not used by the EFSM-POINTS module, 
    // just set to 0 by the constructor.
    int iflag1;
};

/*======================================================================*/
/*= Interface.                                                         =*/

/**
 * nbPoint=1,2,... : exactly nbPoint points
 *             button-1/2 click gives a new point.
 *             treatment is called when we got N points
 *             button-3 click restarts the FSM.
 *
 * preview(dev,buttonOfFirstClick,pts,ptNb) is called every time the
 * pointer moves and the point sequence has nbPoint elements.
 *
 * treatment(sd,a) is called when the user selected the N points. They
 *    are in fsm->points set. Then the FSM is restarted.
 *
 * Use the vgui_efsm_free function to free the memory allocated by the
 * returned fsm.
**/
extern Tvgui_efsmPTS* vgui_efsmPTS_new_Nexact (
        int nbPoint, Twid_canvas* dev,
        Tvgui_efsm_treatFunc      treatment,
        Tvgui_efsmPTS_drawSetFunc preview,
        void*                     ud);

/**
 * nbPoint=2,3,... : at least nbPoint points
 *         button-1 click gives a new point.
 *         button-2 click gives the last point so treatment is called
 *         button-3 click restarts.
 *
 * preview(dev,buttonOfFirstClick,pts,ptNb)  is called every time the
 * pointer moves and they are at least 2 points in pts (ptNb>=2).
 * The last point of pts is the pointer position.
 *
 * treatment(sd,a) is called when the user finishs the point selection.
 *    The points are in a->points set and there are at least nbPoint. 
 *    Then the FSM is restarted.
 *
 * Use the vgui_efsm_free function to free the memory allocated by the
 * returned fsm.
**/
extern Tvgui_efsmPTS* vgui_efsmPTS_new_NorMore (
        int nbPoint, Twid_canvas* dev,
        Tvgui_efsm_treatFunc      treatment,
        Tvgui_efsmPTS_drawSetFunc preview,
        void*                     ud);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_EFSM_POINTS_H
/*======================================================================*/
