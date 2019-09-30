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
 *      $Id: vgui-efsm.h 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-efsm.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A canvas Event FSM definition:
 *   - The inputs are the events that a drawing area can generate such
 *     as button click and cursor move.
 *   - The outputs are fully abstract.
 *
 * The next types are defined:
 *   - Tvgui_efsm:    The EFSM top class
 *   - Tvgui_efsm_in: The enumeration of the event kinds.
 *   - Tvgui_efsm_freeFunc: the abstract function for freeing the
 *     memory allocated by the specialized Tvgui_efsm classes.
 *   - Tvgui_efsm_treatFunc: This abstract function associates a
 *     treatment to the EFSM.
 *   - Tvgui_efsm_transFunc: This abstract function is the FSM
 *     transition function of the EFSM.
 *
 * The next functions provide the main EFSM interface:
 *   - vgui_efsm_new: Constructor.
 *   - vgui_efsm_free: Destructor.
 *   - vgui_efsm_transition: It performs a step of the EFSM transition
 *     function.
 *   - vgui_efsm_runTreatment: It performs the treatment that is
 *     associated to the EFSM. It locks the EFSM until the treatment
 *     finishes.
 *
**======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_EFSM_H
#define FILE_VGUI_EFSM_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "outdev/types.h"

/*======================================================================*/
/*= Input kind of EFSM.                                                =*/

typedef enum _Tvgui_efsm_in {
    // This input kind asks to the EFSM to enter the EFSMST_END state.
    // If the transition function enters the end state then:
    //   - the FSM is disabled (vgui_efsm_transition will not 
    //     invoke the fsm transition function anymore).
    //   - the system is authorized to delete the EFSM.
    // If the transition function does not enter the end state then
    // the system continues to transmit input events and does not
    // delete the EFSM.
    // x and y arguments are set to 0.
    EFSMIN_Quit,
    // send only once to the EFSM before any other request (x=0,y=0)
    EFSMIN_Init,
    // pointer moved to (x,y)
    EFSMIN_MouseMotion,
    // button N pressed and the pointer position is (x,y)
    EFSMIN_But1Pressed,  EFSMIN_But2Pressed,  EFSMIN_But3Pressed, 
    // button N released and the pointer position is (x,y)
    EFSMIN_But1Released, EFSMIN_But2released, EFSMIN_But3released, 
    // canvas was cleared, redraw is needed (x=0,y=0).
    EFSMIN_Redraw,
    // Pointer enters or leaves the canvas window (x=0,y=0).
    EFSMIN_EnterCanvas, EFSMIN_LeaveCanvas,
} Tvgui_efsm_in;

/*======================================================================*/
/*= states the EFSM share.                                             =*/

#define EFSMST_BEGIN  0  // the start state
#define EFSMST_END    1  // the end state (EFSM is disabled)
#define EFSMST_NFS    2  // Number of First State of specialized EFSM

// shortcut
#define EState             fsm->fsm_state

/*======================================================================*/
/*= Types.                                                             =*/

typedef void (*Tvgui_efsm_freeFunc) (Tvgui_efsm* fsm);
typedef void (*Tvgui_efsm_treatFunc)(Tvgui_efsm* fsm);
typedef void (*Tvgui_efsm_transFunc)(Tvgui_efsm* fsm,
               Tvgui_efsm_in kind, int x, int y);

struct _Tvgui_efsm {
    Twid_canvas*         indevice;  // device that generates the inputs
    Toutdev*             outdevice; // device for drawing
    Tvgui_efsm_freeFunc  freeFunc;
    Tvgui_efsm_treatFunc treatmentFunc;
    Tvgui_efsm_transFunc transitionFunc;
    int                  state;

    // 1: Indicates that the FSM is locked, the transition
    // function will discard most of the iinput events.
    int                  locked;

    // 1: Indicates to transmit to the transitionFunc function,
    //    magnetic coordinates (eg: rounded to the nearest cross
    //    of the  magnetic grid of the canvas).
    //    If the associated canvas has not the magnetic grid enabled,
    //    this option has no effect.
    int                  magnet;

    // support for wid-mousefun widget
    // if mfun is not null "wid_mfun_showDt(mfun,EState)"
    // is performed evrery time the fsm state changes, and the
    // pointer enters or leaves the canvas window.
    Twid_mousefunDt*     mfun;

    // not handle by the EFSM module.
    void*                userdata;
};

// shortcut for specialized EFSM
#define fsm_indevice       any.indevice
#define fsm_outdevice      any.outdevice
#define fsm_freeFunc       any.freeFunc
#define fsm_treatmentFunc  any.treatmentFunc
#define fsm_transitionFunc any.transitionFunc
#define fsm_userdata       any.userdata
#define fsm_state          any.state
#define fsm_locked         any.locked
#define fsm_mousefun       any.mousefun
#define fsm_mousefunMess   any.mousefunMess

/*======================================================================*/
/*= Interface.                                                         =*/

/**
 * The vgui_efsm_new function allocates a Tvgui_efsm structure of
 * bs byte size, initializes all the  fields to zero except of
 *   - the function pointers (set to the corresponding parameter), 
 *   - state (set to EFSMST_BEGIN),
 *   - userdata (set to ud),
 *
 * The vgui_efsm_delete function frees all the memory the fsm EFSM
 * allocated. if fsm is the NULL pointer, it returns immediately. 
 *
 * Return:
 *  The vgui_efsm_new returns a pointer to the EFSM that it created.
**/
extern Tvgui_efsm* vgui_efsm_new(int bs, Twid_canvas* dev,
                                 Tvgui_efsm_freeFunc  free,
                                 Tvgui_efsm_treatFunc treatment,
                                 Tvgui_efsm_transFunc trans,
                                 void*                ud);

extern void    vgui_efsm_delete (Tvgui_efsm* fsm);

/**
 * The vgui_efsm_transition executes one step of the fsm FSM for
 * the kind, x, y inputs.
 *
 * The treatments for the different input kinds are summarized on the
 * following table. The "END" column shows what does the function when
 * the state of the FSM is EFSMST_END, the "unlocked" and "locked"
 * columns show what does the function when the state of the FSM is not
 * EFSMST_END and the fsm FSM is marked (fsm->fsm_locked flag) unlocked
 * and locked respectively.
 *     input kind         unlocked  locked  END
 *   EFSMIN_Quit             T        D      D
 *   EFSMIN_Init             T        D      D
 *   EFSMIN_Redraw           T        D      D
 *   EFSMIN_EnterCanvas      IT       IT     D
 *   EFSMIN_LeaveCanvas      IT       IT     D
 *   others                  T        D      D
 *
 * Parameters:
 *   - fsm: the FSM. If fsm is null, the function returns 1 immediately.
 *   - kind: the event that occurs
 *   - x,y:  the position of pointer when the event occurred or (0,0) if
 *           the event does not matter with pointer (ex: REDRAW).
**/
extern void vgui_efsm_transition(Tvgui_efsm* fsm,
                                 Tvgui_efsm_in kind, int x, int y);

/**
 * The vgui_efsm_runTreatment function performs:
 *    1) It locks the fsm EFSM.
 *    2) It runs the fsm->treatmentFunc function
 *    3) It unlocks the fsm EFSM.
 * So when the treatment function is running, the transition function
 * (vgui_efsm_transition) ignores most of the input event.
 *
 * Note that if the fsm->treatmentFunc is the NULL pointer, the
 * function returns immediately.
**/
extern void vgui_efsm_runTreatment(Tvgui_efsm* fsm);

/**
 * The vgui_efsm_mousefun_show function writes the texts that are
 * associated to the fsm state into the wid-mousefun that is
 * associated to the fsm FSM.
 * The associated wid-mousefun widget is given by fsm->mousefun.
 * The associated texts are the fsm->mousefunMess[state].str[i] strings
 * for i in {0, 1, 2}. If such a string is the NULL pointeur, write it
 * means erasing the corresponding area.
 *
 * The vgui_efsm_mousefun_clear function erases the texts written in
 * the wid-mousefun that is associated to the fsm FSM.
 *
 * The both functions do nothing if the fsm->mousefun widget is the
 * NULL pointer.
 * The vgui_efsm_mousefun_show function  does nothing if the 
 * fsm->mousefunMess text table is the NULL pointer
 **/
extern void vgui_efsm_mousefun_show(Tvgui_efsm* fsm);
extern void vgui_efsm_mousefun_clear(Tvgui_efsm* fsm);

extern void vgui_efsm_mousefun_enable(Tvgui_efsm* fsm,
                                      Twid_mousefun*  w,
                                      Cwid_mousefun3S m[]);
extern void vgui_efsm_mousefun_disable(Tvgui_efsm* fsm);

/*======================================================================*/
/*= debug                                                              =*/

#define DBG0_Efsm(fmt,...)   DBG0_printf("efsm",fmt,__VA_ARGS__)
#define DBG1_Efsm(fmt,...)   DBG1_printf("efsm",fmt,__VA_ARGS__)
#define DBG2_Efsm(fmt,...)   DBG2_printf("efsm",fmt,__VA_ARGS__)

#define DBG0_EfsmE(fmt,...) DBG0_printf("efsm","Enter : "fmt,__VA_ARGS__)
#define DBG0_EfsmM(fmt,...) DBG0_printf("efsm","Middle: "fmt,__VA_ARGS__)
#define DBG0_EfsmL(fmt,...) DBG0_printf("efsm","Leave : "fmt,__VA_ARGS__)
#define DBG1_EfsmE(fmt,...) DBG1_printf("efsm","Enter : "fmt,__VA_ARGS__)
#define DBG1_EfsmM(fmt,...) DBG1_printf("efsm","Middle: "fmt,__VA_ARGS__)
#define DBG1_EfsmL(fmt,...) DBG1_printf("efsm","Leave : "fmt,__VA_ARGS__)
#define DBG2_EfsmE(fmt,...) DBG2_printf("efsm","Enter : "fmt,__VA_ARGS__)
#define DBG2_EfsmM(fmt,...) DBG2_printf("efsm","Middle: "fmt,__VA_ARGS__)
#define DBG2_EfsmL(fmt,...) DBG2_printf("efsm","Leave : "fmt,__VA_ARGS__)

#if defined(TOOL_DBG_Trace) && TOOL_DBG_Trace
#define DBG0_EfsmEkd(fmt,...) while (0)
#define DBG0_EfsmMkd(fmt,...) while (0)
#define DBG0_EfsmLkd(fmt,...) while (0)
#define DBG1_EfsmEkd(fmt,...) while (0)
#define DBG1_EfsmMkd(fmt,...) while (0)
#define DBG1_EfsmLkd(fmt,...) while (0)
#define DBG2_EfsmEkd(fmt,...) while (0)
#define DBG2_EfsmMkd(fmt,...) while (0)
#define DBG2_EfsmLkd(fmt,...) while (0)
#else
#define EFSM_FMTkd "state=%d, event=%d x=%d, y=%d"
#define EFSM_ARGkd fsm->fsm_state,kind,x,y
#define DBG0_EfsmEkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG0_EfsmE(fmt,__VA_ARGS__); } while (0)
#define DBG0_EfsmMkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG0_EfsmM(fmt,__VA_ARGS__); } while (0)
#define DBG0_EfsmLkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG0_EfsmL(fmt,__VA_ARGS__); } while (0)
#define DBG1_EfsmEkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG1_EfsmE(fmt,__VA_ARGS__); } while (0)
#define DBG1_EfsmMkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG1_EfsmM(fmt,__VA_ARGS__); } while (0)
#define DBG1_EfsmLkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG1_EfsmL(fmt,__VA_ARGS__); } while (0)
#define DBG2_EfsmEkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG2_EfsmE(fmt,__VA_ARGS__); } while (0)
#define DBG2_EfsmMkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG2_EfsmM(fmt,__VA_ARGS__); } while (0)
#define DBG2_EfsmLkd(fmt,...) do { if (kind!=EFSMIN_MouseMotion) DBG2_EfsmL(fmt,__VA_ARGS__); } while (0)
#endif

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_EFSM_H
/*======================================================================*/
