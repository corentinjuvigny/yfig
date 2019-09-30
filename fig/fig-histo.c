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
 *      $Id: fig-histo.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-histo.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= History of changes of an object hierarchy.                         =*/
/*=                                                                    =*/
/*= See the fig/fig-histo.h header for documentation.                  =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/fig-histo.h"
#include "fig/object-priv.h"
#include "fig/obj-compound.h"

/*======================================================================*/
/*= Interface of Tfig_hele                                             =*/

extern void* fig_histo_ele_new(int size, Tfigure* figure,
                               Tfig_hfunc unredo, Tfig_hfree free)
{
    Tfig_hele* self = tool_malloc_0(size);
    self->he_unredo = unredo;
    self->he_free   = free;
    self->he_figure = figure;
    return self;
}

extern void  fig_histo_ele_free(Tfig_hele* e)
{
    if ( e->he_free==0 )
        free( e );
    else
        e->he_free( e );
}

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tfig_histo {
    int     eleMemNb;  // element number of the memory block eles
    int     eleNb;     // element number of set (eleNb<=eleMemNb)
    Tfig_hele** eles;  // the set elements

  /* data for circular fifo */
    // newer and older are valid when the eles table is not empty,
    // otherwise (eleNB==0) they are set to -1.
    int     newer;     // eles[newer] is the newer element
    int     older;     // eles[older] is the older element.

  /* data for undo/redo operation */
    Tfigure* figure;
    int      undoNb;   // number of undo performed
    int      undo;     // -1 when not used, it must be initialized to
                       // newer when table is not full

  /* data for undo/redo operation */
    // modifNb is the number of modifications that have been pushed
    // into the history since its creation and that have not been undone.
    int      modifNb;
};

#define IsFull(h)    (h)->eleNb==(h)->eleMemNb
#define IsEmpty(h)   (h)->eleNb==0
#define NextIdx(h,i) ( (i)==((h)->eleMemNb-1) ? 0 : i+1 )
#define PrevIdx(h,i) ( (i)==0 ? (h)->eleMemNb-1 : i-1 )

/*======================================================================*/
/*= Base operations                                                    =*/

extern Tfig_histo* fig_histo_new(Tfigure* f, int nb)
{
    Tfig_histo* ret = malloc(sizeof(*ret));
    ret->eleMemNb  = nb ; 
    ret->eleNb     = 0;
    ret->eles      = tool_malloc_0(sizeof(*ret->eles)*nb);
    ret->newer     = -1;
    ret->older     = -1;
    ret->figure    = f;
    ret->undo      = -1;
    ret->undoNb    = -1;
    ret->modifNb   = 0;
    return ret;
}

extern void        fig_histo_delete(Tfig_histo* h)
{
    int i,k;
    for ( i=0,k=h->older ; i<h->eleNb ; i++) {
        Tfig_hele* e = h->eles[k];
        if ( e->he_free )
            e->he_free( e );
        else
            tool_free(e);
        k = NextIdx(h,k);
    }
    free( h->eles );
    free( h );
}

extern void        fig_histo_push(Tfig_histo* h, Tfig_hele* he)
{
    if ( IsEmpty(h) ) {
        h->eles[0] = he;
        h->eleNb = 1;
        h->newer = 0;
        h->older = 0;
    } else if ( IsFull(h) ) {
        // free the older element before replacing it by he
        Tfig_hele* e = h->eles[h->older];
        fig_histo_ele_free(e);
        h->eles[h->older] = he;
        // update newer and older
        h->newer = h->older ;
        h->older = NextIdx(h,h->older);
    } else {
        h->newer = NextIdx(h,h->newer);
        h->eles[h->newer] = he;
        h->eleNb +=1;
    }
    h->modifNb += 1;
}

extern void        fig_histo_pop(Tfig_histo* h)
{
    TEMP_asf_printf(IsEmpty(h),"empty history",0);
    Tfig_hele* e = h->eles[h->newer];
    fig_histo_ele_free(e);
    h->newer  = PrevIdx(h,h->newer);
    h->eleNb -= 1;
    if ( h->eleNb==0 ) {
       h->newer  = -1; 
       h->older  = -1; 
       h->undo   = -1; 
       h->undoNb = -1; 
    }
}

extern int         fig_histo_modifNb(Tfig_histo* h)
{ return h->modifNb; }

/*======================================================================*/
/*= Moving in history                                                  =*/

extern int         fig_histo_undoNb(Tfig_histo* h)
{
    int nb = h->eleNb;
    if ( h->undoNb>=0 )
        nb -= h->undoNb;
    return nb; 
}

extern int         fig_histo_redoNb(Tfig_histo* h)
{
    return  h->undoNb<=0 ? 0 :  h->undoNb;
}

extern void        fig_histo_undo  (Tfig_histo* h)
{
    Tfig_hele* e;
    if ( h->undo==-1 ) {
        if ( h->eleNb==0 )
            return;
        h->undo   = h->newer;
        h->undoNb = 0;
    }
    if ( h->undoNb==h->eleNb )
        return;
    e = h->eles[h->undo];
    e->he_unredo(e,1);
    h->undo    = PrevIdx(h,h->undo);
    h->undoNb += 1;
    h->modifNb -= 1;
}

extern void        fig_histo_redo  (Tfig_histo* h)
{
    Tfig_hele* e;
    if ( h->undoNb<=0 ) return; 
    h->undo  = NextIdx(h,h->undo);
    h->undoNb -= 1;
    e = h->eles[h->undo];
    e->he_unredo(e,0);
    h->modifNb += 1;
}

extern void        fig_histo_commit(Tfig_histo* h)
{
    int i;
    for ( i=0 ; i<h->undoNb ; i++)
        fig_histo_pop(h);
    h->undo   = -1;
    h->undoNb = -1;
}

/*======================================================================*/
/*= Adding figure modifications                                        =*/

static Tfig_hele* fig_hele_newAddObj (Tfigure* f, Tobject o, TobjDir d);
static Tfig_hele* fig_hele_newSupObj (Tfigure* f, Tobject o, TobjDir d);
static Tfig_hele* fig_hele_newSupObjSet(Tfigure* f, TobjAll*objs,
                                        int objnb, TobjDir d);
static Tfig_hele* fig_hele_newCpdDo  (Tfigure* f, TobjDir o, TobjDir p);
static Tfig_hele* fig_hele_newCpdFlat(Tfigure* f, TobjDir o, TobjDir p);
static Tfig_hele* fig_hele_newChgObj (Tfigure* f, Tobject old,
                                      Tobject latest, TobjDir d);
static Tfig_hele* fig_hele_newMovObj (Tfigure* f, Tobject obj, Tdble2 tv);

extern void        fig_histo_pushAddObj (Tfig_histo* h, Tobject obj)
{
    TobjAny*   any = (TobjAny*)obj;
    TobjDir    dir = any->OA_parent;
    Tfig_hele* he  = fig_hele_newAddObj(h->figure, obj, dir);
    fig_histo_push(h,he);
}
extern void        fig_histo_pushDelObj (Tfig_histo* h, Tobject obj)
{
    TobjDir    dir = obj->OA_parent;
    Tfig_hele* he  = fig_hele_newSupObj(h->figure, obj, dir);
    fig_histo_push(h,he);
}
extern void        fig_histo_pushDelObjsS(Tfig_histo* h, CobjAllSet* set,
                                            TobjDir dir)
{
    Tfig_hele* he  = fig_hele_newSupObjSet(h->figure, set->eles,set->eleNb, dir);
    fig_histo_push(h,he);
}
extern void        fig_histo_pushDelObjsT(Tfig_histo* h, TobjAll*objs,
                                            int objnb, TobjDir dir)
{
    Tfig_hele* he  = fig_hele_newSupObjSet(h->figure, objs,objnb, dir);
    fig_histo_push(h,he);
}

extern void        fig_histo_pushCpdDo  (Tfig_histo* h, TobjDir obj)
{
    TobjDir    dir = obj->O_parent;
    Tfig_hele* he  = fig_hele_newCpdDo(h->figure, obj, dir);
    fig_histo_push(h,he);
}

extern void        fig_histo_pushCpdFlat(Tfig_histo* h, TobjDir obj)
{
    TobjDir    dir = obj->O_parent;
    Tfig_hele* he  = fig_hele_newCpdFlat(h->figure, obj, dir);
    fig_histo_push(h,he);
}

extern void fig_histo_pushChgObj (Tfig_histo* h, Tobject old,
                                                 Tobject latest)
{
    TobjDir dir = old->OA_parent;
    if ( dir==0 )
        dir = latest->OA_parent;
    Tfig_hele* he  = fig_hele_newChgObj(h->figure, old, latest, dir);
    fig_histo_push(h,he);
}

extern void fig_histo_pushMovObj  (Tfig_histo* h, Tobject obj, Tdble2 tv)
{
    Tfig_hele* he  = fig_hele_newMovObj(h->figure, obj, tv);
    fig_histo_push(h,he);
}

/*======================================================================*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Actions that add or suppress an object of the object tree          =*/

typedef struct _Tfig_hele_addSupObj {
    Tfig_hele0 base;
    TobjAll    he_obj;
    TobjAll    he_dir;
} Tfig_hele_addSupObj;

static void addObj_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_addSupObj* self = (Tfig_hele_addSupObj*)he;
    if ( undo ) {
        obj_dir_delObjQ( self->he_obj );
    } else {
        obj_dir_addObj( self->he_dir, self->he_obj );
    }
}

static void supObj_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_addSupObj* self = (Tfig_hele_addSupObj*)he;
    if ( undo ) {
        obj_dir_addObj( self->he_dir, obj_newLazy(self->he_obj) );
    } else {
        obj_dir_delObjQ( self->he_obj );
    }
}

static void addSupObj_free(Tfig_hele* he)
{
    Tfig_hele_addSupObj* self = (Tfig_hele_addSupObj*)he;
    if ( OBJ_IsDefined(self->he_obj) ) obj_delete(self->he_obj);
    if ( OBJ_IsDefined(self->he_dir) ) obj_delete(self->he_dir);
    free( self );
}

static Tfig_hele* fig_hele_newAddObj(Tfigure* f, Tobject o, TobjDir d)
{
    Tfig_hele_addSupObj* self = fig_histo_ele_new(sizeof(*self),
            f,addObj_unredo, addSupObj_free);
    self->he_obj    = obj_newLazy( obj_ObjAll(o) );
    self->he_dir    = obj_newLazy( obj_ObjAll(d) );
    return (Tfig_hele*)self;
}

static Tfig_hele* fig_hele_newSupObj(Tfigure* f, Tobject o, TobjDir d)
{
    Tfig_hele_addSupObj* self = fig_histo_ele_new(sizeof(*self),
            f, supObj_unredo, addSupObj_free);
    self->he_obj    = obj_newLazy( obj_ObjAll(o) );
    self->he_dir    = obj_newLazy( obj_ObjAll(d) );
    return (Tfig_hele*)self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

typedef struct _Tfig_hele_supObjSet {
    Tfig_hele0  base;
    TobjAllSet* he_objs;
    TobjDir     he_dir;
} Tfig_hele_supObjSet;

static void supObjSet_unredo(Cfig_hele* he, int undo)
{
    int i;
    Tfig_hele_supObjSet* self = (Tfig_hele_supObjSet*)he;
    TobjAllSet*          set  = self->he_objs;
    if ( undo ) {
        for ( i=0 ; i<set->eleNb ; i+=1 )
            obj_dir_addObj( obj_ObjAll(self->he_dir), obj_newLazy(set->eles[i]) );
    } else {
        for ( i=0 ; i<set->eleNb ; i+=1 )
            obj_dir_delObjQ( set->eles[i] );
    }
}

static void supObjSet_free(Tfig_hele* he)
{
    int i;
    Tfig_hele_supObjSet* self = (Tfig_hele_supObjSet*)he;
    TobjAllSet*          set  = self->he_objs;
    for ( i=0 ; i<set->eleNb ; i+=1 )
        obj_delete(set->eles[i]);
    if ( self->he_dir ) obj_delete( obj_ObjAll(self->he_dir) );
    objAllSet_delete( self->he_objs );
    free( self );
}

static Tfig_hele* fig_hele_newSupObjSet(Tfigure* f, TobjAll*objs,
                                        int objnb, TobjDir d)
{
    int i;
    Tfig_hele_supObjSet* self = fig_histo_ele_new(sizeof(*self),
            f,supObjSet_unredo, supObjSet_free);
    self->he_objs   = objAllSet_new( objnb );
    self->he_dir    = obj_newLazy( obj_ObjAll(d) ).dir;
    for ( i=0 ; i<objnb ; i+=1 ) {
        TobjAll o = obj_newLazy( objs[i] );
        objAllSet_add( self->he_objs, o);
    }
    return (Tfig_hele*)self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Actions that create or suppress an hierarchical node in            =*/
/*= the object tree                                                    =*/

typedef struct _Tfig_hele_addSupDir {
    Tfig_hele0  base;
    TobjAll     he_obj;      // the node to add or suppress
    TobjAll     he_dir;      // the parent of he_obj
    TobjAllSet* he_children; // children of he_obj
} Tfig_hele_addSupDir;

static void addDir_do(Tfig_hele_addSupDir* self)
{
    int i;
    obj_dir_addObj(self->he_dir, obj_newLazy(self->he_obj) );
    for ( i=0 ; i<self->he_children->eleNb ; i+=1 ) {
        TobjAll o = self->he_children->eles[i];
        obj_dir_addObj(self->he_obj,obj_newLazy(o) );
    }
}

static void addDir_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_addSupDir* self = (Tfig_hele_addSupDir*)he;
    if ( undo ) {
        obj_dir_flat( self->he_obj );
    } else {
        addDir_do( self );
    }
}

static void supDir_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_addSupDir* self = (Tfig_hele_addSupDir*)he;
    if ( undo ) {
        int i;
        for ( i=0 ; i<self->he_children->eleNb ; i+=1 ) {
            TobjAll o = self->he_children->eles[i];
            obj_delete( o );
        }
        addDir_do( self );
    } else {
        obj_dir_flat( self->he_obj );
    }
}

static void addSupDir_free(Tfig_hele* he)
{
    Tfig_hele_addSupDir* self = (Tfig_hele_addSupDir*)he;
    int i;
    for ( i=0 ; i<self->he_children->eleNb ; i+=1 ) {
        TobjAll o = self->he_children->eles[i];
        obj_delete(o);
    }
    objAllSet_delete( self->he_children );
    if ( OBJ_IsDefined(self->he_obj) ) obj_delete(self->he_obj);
    if ( OBJ_IsDefined(self->he_dir) ) obj_delete(self->he_dir);
    free( self );
}

static Tfig_hele* fig_hele_newCpdDo(Tfigure* f, TobjDir o, TobjDir d)
{
    int i;
    Tfig_hele_addSupDir* self = fig_histo_ele_new(sizeof(*self),
            f,addDir_unredo, addSupDir_free);
    self->he_obj      = obj_newLazy( obj_ObjAll(o) );
    self->he_dir      = obj_newLazy( obj_ObjAll(d) );
    self->he_children = objAllSet_newClone( obj_getChildren(obj_ObjAll(o)) );
    for ( i=0 ; self->he_children && i<self->he_children->eleNb ; i+=1 ) {
        TobjAll o = obj_newLazy( self->he_children->eles[i] );
    }
    return (Tfig_hele*)self;
}

static Tfig_hele* fig_hele_newCpdFlat(Tfigure* f, TobjDir o, TobjDir d)
{
    int i;
    Tfig_hele_addSupDir* self = fig_histo_ele_new(sizeof(*self),
            f,supDir_unredo, addSupDir_free);
    self->he_obj      = obj_newLazy( obj_ObjAll(o) );
    self->he_dir      = obj_newLazy( obj_ObjAll(d) );
    self->he_children = objAllSet_newClone( obj_getChildren(self->he_obj) );
    for ( i=0 ; self->he_children!=0 && i<self->he_children->eleNb ; i+=1 ) {
        TobjAll o = obj_newLazy( self->he_children->eles[i] );
    }
    return (Tfig_hele*)self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Action that modififies an object                                   =*/

typedef struct _Tfig_hele_chgObj {
    Tfig_hele0 base;
    TobjAll    he_old;      // he_old was replaced by
    TobjAll    he_latest;   // he_latest
    TobjDir    he_dir;      // where he_old and he_latest reside
} Tfig_hele_chgObj;

static void chgObj_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_chgObj* self = (Tfig_hele_chgObj*)he;
    if ( undo ) {
        obj_dir_delObjQ( self->he_latest );
        obj_dir_addObj ( obj_ObjAll(self->he_dir), obj_newLazy(self->he_old) );
    } else {
        obj_dir_delObjQ( self->he_old );
        obj_dir_addObj ( obj_ObjAll(self->he_dir), obj_newLazy(self->he_latest) );
    }
}

static void chgObj_free(Tfig_hele* he)
{
    Tfig_hele_chgObj* self = (Tfig_hele_chgObj*)he;
    if ( OBJ_IsDefined(self->he_old)    ) obj_delete(self->he_old);
    if ( OBJ_IsDefined(self->he_latest) ) obj_delete(self->he_latest);
    if ( self->he_dir                   ) obj_delete( obj_ObjAll(self->he_dir) );
    free( self );
}

static Tfig_hele* fig_hele_newChgObj(Tfigure* f, Tobject old,
                                     Tobject latest, TobjDir d)
{
    Tfig_hele_chgObj* self = fig_histo_ele_new(sizeof(*self),
            f,chgObj_unredo, chgObj_free);
    self->he_old    = obj_newLazy( obj_ObjAll(old) );
    self->he_latest = obj_newLazy( obj_ObjAll(latest) );
    self->he_dir    = obj_newLazy( obj_ObjAll(d) ).dir;
    return (Tfig_hele*)self;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Action that moves an object                                        =*/

typedef struct _Tfig_hele_movObj {
    Tfig_hele0 base;
    TobjAll    he_obj;   // he_old was replaced by
    Tdble2     he_tv;    // the translation vector
} Tfig_hele_movObj;

static void movObj_unredo(Cfig_hele* he, int undo)
{
    Tfig_hele_movObj* self = (Tfig_hele_movObj*)he;
    if ( undo ) {
        obj_translation( self->he_obj, -self->he_tv.x, -self->he_tv.y );
    } else {
        obj_translation( self->he_obj, +self->he_tv.x, +self->he_tv.y );
    }
}

static void movObj_free(Tfig_hele* he)
{
    Tfig_hele_movObj* self = (Tfig_hele_movObj*)he;
    if ( OBJ_IsDefined(self->he_obj)    ) obj_delete(self->he_obj);
    free( self );
}

static Tfig_hele* fig_hele_newMovObj (Tfigure* f, Tobject obj, Tdble2 tv)
{
    Tfig_hele_movObj* self = fig_histo_ele_new(sizeof(*self),f,
                                 movObj_unredo, movObj_free);
    self->he_obj = obj_newLazy( obj_ObjAll(obj) );
    self->he_tv  = tv;
    return (Tfig_hele*)self;
}

/*======================================================================*/
