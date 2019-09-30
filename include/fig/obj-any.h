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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-any.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-any.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The top class of any object (The data structure that all the       =*/
/*= share).                                                            =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_ANY_H
#define FILE_OBJ_ANY_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "fig-types.h"

#include "tools/boxes.h"
#include "outdev/style.h"

#define VARS_Self(o) TobjAny* self=o.any
#define VARS_SeVt(o) VARS_Self(o); CobjVtable* vtable =  self->oa_vtable

/*======================================================================*/

#define FIG_TAG_HISTORY             0x00002  // in history
#define FIG_TAG_CanHaveArrow        0x00004  // you can add arrows to this object
#define FIG_TAG_BBoxUpToDate        0x00010  // oa_boundBox is up to date
#define FIG_TAG_DrawingDisabled     0x00020  // do not invoke drawing abstract function 
#define FIG_TAG_DrawingDataReady    0x00040  // drawing data are ready. This flags
                                             // is cleared when an object is modified.
                                             // obj_doDrawData sets it again.
// for text object
#define FIG_TAG_FontGeomUpToDate 0x01000

/*======================================================================*/

struct _TobjAny {
    CobjVtable*
                oa_vtable;
    int         oa_tags;        // see FIG_TAG_... tags.
    int         oa_cnt;         // reference number in both the history
                                // and the main object tree.
                                // So, 0 means the object is not referenced
                                // anymore in figure.
                                // Notice an object can be only referenced
                                // once in the object tree.
    double      oa_ppi;         // object unit.
    Tfigure*    oa_figure;      // belong to the object tree of this figure
    TobjDir     oa_parent;      // compound hierarchy
    TobjAll     oa_partof;      // sub object of
    TobjAny*    oa_next;
    TstrEaten   oa_comment;
    TboxDble    oa_boundBox;
    int         oa_depthmin;    // minimal depth of object hierarchy
    int         oa_depthmax;    // minimal depth of object hierarchy
                                // 0,0:    nothing to draw
                                // d,d+1:  a drawing at d depth
                                // d,d+n:  a drawing at d depth +
                                //         a drawing at d+n-1 depth +
                                //         0 or more drawing at l depth
                                //         with l: d+1<=l<d+n-1
#define OA_setDepth(self,depth) do { self->oa_depthmin=depth; \
                self->OA_depthmax=(depth)+1; } while (0)
};

// macro for TobjAny
#define OA_TO(o)    ((Tobject)&(o->oa_vtable))
#define OA_vtable   oa_vtable
#define OA_tags     oa_tags
#define OA_cnt      oa_cnt
#define OA_ppi      oa_ppi
#define OA_figure   oa_figure
#define OA_parent   oa_parent
#define OA_partof   oa_partof
#define OA_next     oa_next
#define OA_comment  oa_comment
#define OA_boundBox oa_boundBox
#define OA_depthmin oa_depthmin
#define OA_depthmax oa_depthmax
#define OA_depth    oa_depthmin

// macro for other TobjXxx
#define O_2A(o)         (&(o)->o_any)
#define O_2O(o)         ((Tobject)O_2A(o))
#define O_setDepth(o,d) OA_setDepth(O_2A(o),d)
#define O_vtable        o_any.OA_vtable
#define O_tags          o_any.OA_tags
#define O_cnt           o_any.OA_cnt
#define O_ppi           o_any.oa_ppi
#define O_figure        o_any.oa_figure
#define O_parent        o_any.OA_parent
#define O_partof        o_any.OA_partof
#define O_next          o_any.OA_next
#define O_comment       o_any.OA_comment
#define O_boundBox      o_any.OA_boundBox
#define O_depthmin      o_any.OA_depthmin
#define O_depthmax      o_any.OA_depthmax
#define O_depth         o_any.OA_depthmin

extern int  objlist_size    (CobjAny* list);
extern void objlist_addAhead(TobjAny** list, Tobject obj);
extern void objlist_remove  (TobjAny** list, Tobject obj);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_ANY_H
/*======================================================================*/
