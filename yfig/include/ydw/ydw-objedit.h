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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-objedit.h 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/ydw/ydw-objedit.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialogs that edit figure objects.                                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YDW_YDW_OBJEDIT_H
#define FILE_YDW_YDW_OBJEDIT_H
/*======================================================================*/

#include "yfig-config.h"
#include "ydw.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-grp.h"
#include "fig/object.h"

/*======================================================================*/
/*= Data structure of sub types                                        =*/

// handle the object preview and applying
typedef struct _Tyoed_state {
    TobjAll  oorg;   // original object
    TobjAll  onew;   // current modified object
    TobjAll  odrw;   // last drawn object (when applying)
    TobjAll  opvw;   // preview object (currently drawn in preview mod)

    int      preview; // 1:preview onew in main canvas when it changes
                      // 0:do not preview oonewobject in main canvas
} Tyoed_state;

// virtual table of object edit dialog
typedef struct _Tyoed_vtable Tyoed_vtable;
typedef const   Tyoed_vtable Cyoed_vtable;
struct _Tyoed_vtable {
    int      op_size;              // size of objparams sub-dialog
    int      op_center;            // add default center in objparams sub-dialog
    void    (*typeChg)(void*oed);  // user changed sub-kind (seld->dw_type)
    void    (*objChg) (void*oed);  // user changed the current object
    void    (*unitChg)(void*oed);  // user changed unit to self->dw_uu (LUNIT_ID_...)
    TobjAll (*newObj) (void*oed);  // create a new object usig oed parameters
};

#if 0 // for debug to chech if object reference counters are ok.
#include "fig/obj-any.h"
#define DBG_YOES(state,n) PERM_spe_printf("%d: org=%p:%d:%p pre=%p:%d drw=%p:%d new=%p:%d:%p",n,\
    (state)->oorg.any,(state)->oorg.any==0?  -1:(state)->oorg.any->oa_cnt, \
                      (state)->oorg.any==0?NULL:obj_Figure((state)->oorg), \
    (state)->opvw.any,(state)->opvw.any==0?  -1:(state)->opvw.any->oa_cnt, \
    (state)->odrw.any,(state)->odrw.any==0?  -1:(state)->odrw.any->oa_cnt, \
    (state)->onew.any,(state)->onew.any==0?  -1:(state)->onew.any->oa_cnt, \
                      (state)->onew.any==0?NULL:obj_Figure((state)->onew))
#else
#define DBG_YOES(state,n) while (0)
#endif

/*======================================================================*/
/*= Data structure of the base class of the dialogs that edit objects  =*/

#define YOED_FIELDS() \
    Tvgui_dw      any;                              \
  /* widgets */                                     \
    Tvgui_dw*     header;                           \
    Tvgui_dw*     depthUnit;                        \
    Tydw_tpen*    textpen;                          \
    Tydw_pen*     pen;                              \
    Tydw_fill*    fill;                             \
    Twid_filebrowser* filebrowser;                  \
    Twid_grp*     center;                           \
    Tvgui_dw*     objparams;                        \
  /* data */                                        \
    Tyfig_mw*   dw_ymw;  /* FIXME:REMOVE */         \
    CstrStatic  dw_xyfmt; /* ~ 9999.99 */           \
    int         dw_uu;    /* user unit */           \
    Tyoed_state dw_state;                           \
    int         dw_kind;  /* object kind */         \
    int         dw_type;  /* & sub-type */          \
    TstyleMask  dw_styleMask; /* used object parameters */ \
    Tfdp        dw_dp;        /* object parameters */      \
    int         dw_inUpdate   /* set to 1 when in update function,
                               * avoid recursion */

typedef struct _Tyoed_any { YOED_FIELDS(); } Tyoed_any;
typedef const   Tyoed_any                    Cyoed_any;
#define oed_vtable(self) ((Tyoed_vtable*)self->any.wa_vtable->usrvtable)

extern Twid_anyVtable yoed_any_vtable;

// shortcut
#define oed_depth dw_dp.fdp_depth
#define oed_dir   dw_dp.fdp_dir
#define oed_dpt   dw_dp.fdp_text
#define oed_dpl   dw_dp.fdp_line
#define oed_dpf   dw_dp.fdp_fill

/*======================================================================*/
/*= Interface to base class of object edit dialogs.                    =*/

// constructor/destructors
extern Tyoed_any* yoed_any_new    (Twid_anyVtable* vt, Tyfig_mw* ymw, TobjAll obj);
extern void       yoed_any_dbf    (Tyoed_any* oed);

// services
// yoed_any_setNewObj creates a new obj using the current object
//   parameters, sets it as current object and then draws it if needed.
// yoed_any_setNewObjObjChg does the same actions as yoed_any_setNewObj
//   but it also performs the vtable->objChg abstract function.
extern void       yoed_any_addCenter        (Tyoed_any* oed);
extern void       yoed_any_update           (Tyoed_any* oed);
extern int        yoed_any_restoreInitialFig(Tyoed_any* oed);
extern void       yoed_any_setNewObj        (Tyoed_any* oed);
extern void       yoed_any_setNewObjObjChg  (Tyoed_any* oed); 
Inline TobjAll    yoed_any_currObj          (Tyoed_any* oed)                     { return OBJ_IsDefined(oed->dw_state.onew) ? oed->dw_state.onew : oed->dw_state.oorg; }

// default callback
extern void yoed_any_cb_ilvuD(Tvgui_dw*vdw, Twid_ilvu*iw, double v);
extern void yoed_any_cb_ilvuT(Tvgui_dw*vdw, Twid_ilvu*iw, CstrBrief s);
extern void yoed_any_cb_grp (Twid_grp*grp, void* ud);

/*======================================================================*/
/*= Interface to object edit dialogs.                                  =*/

extern Tyoed_any* yoed_arc_nbf    (Tyfig_mw* ymw, TobjAll obj);
extern Tyoed_any* yoed_boxhv_nbf  (Tyfig_mw* ymw, TobjAll obj);
extern Tyoed_any* yoed_ellipse_nbf(Tyfig_mw* ymw, TobjAll obj);
extern Tyoed_any* yoed_line_nbf   (Tyfig_mw* ymw, TobjAll obj); // line & spline
extern Tyoed_any* yoed_text_nbf   (Tyfig_mw* ymw, TobjAll obj);

/*======================================================================*/
/*= Sub-dialog of base class of object edit dialogs.                   =*/

extern Tvgui_dw* yoed_any_header_nbf   (Tyoed_any* oed);
extern void      yoed_any_header_delete(Tvgui_dw*  self);
extern void      yoed_any_header_update(Tvgui_dw*  self);

// depth, unit, ...
#if 0 // new not yet changed
extern Tvgui_dw* yoed_any_du_nbf    (Tyoed_any* oed);
extern void      yoed_any_du_delete (Tvgui_dw*  self);
extern void      yoed_any_du_update (Tvgui_dw*  self);
extern int       yoed_any_du_depth1 (Tvgui_dw*  self);
extern int       yoed_any_du_usrunit(Tvgui_dw*  self);
#else
extern Tvgui_dw* ydw_objedit_du_nbfSub(Tvgui_dw* vdw, int lu, int opt,
                                       Twid_cb_DU cbd, Twid_cb_DU cbu,
                                       void* ud);
extern void      ydw_objedit_du_free  (Tvgui_dw* vdw);
extern int       ydw_objedit_du_depth1(Tvgui_dw* vdw);
extern int       ydw_objedit_du_unit  (Tvgui_dw* vdw);
extern void      ydw_objedit_du_update(Tvgui_dw* vdw, int u, int d1, int d2);
#endif


/*======================================================================*/
#endif  // FILE_YDW_YDW_OBJEDIT_H
/*======================================================================*/
