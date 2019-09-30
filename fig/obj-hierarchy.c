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
 *      $Id: obj-hierarchy.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-hierarchy.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Object hierarchy handler.                                          =*/
/*=                                                                    =*/
/*= See the fig/obj-object.h header for documentation.                 =*/ //TODO obsolete doc
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"
#include "fig/object-priv.h"
#include "fig/obj-any.h"
#include "fig/obj-compound.h"
#include "fig/obj-vtable.h"

/*======================================================================*/
/*= private feature to update object depth.                            =*/
/*=                                                                    =*/
/*= The obj_depth_reinit function updates the depths of the dir object =*/
/*= computing it fully again.                                          =*/
/*=                                                                    =*/
/*= The obj_depth_childSup function updates the depths of the dir      =*/
/*= object assuming that a child was suppressed from the dir object    =*/
/*= and that its depth was [min,max[.                                  =*/
/*=                                                                    =*/
/*= The obj_depth_childAdd function updates the depths of the dir      =*/
/*= object assuming that a child was added to the dir object and that  =*/
/*= its depth is [min,max[.                                            =*/
/*=                                                                    =*/
/*= The obj_depth_childChg function updates the depths of the dir      =*/
/*= object assuming that the depths of a dir child changed from        =*/
/*= [olmin,olmax[ to [newmin,newmax[.                                  =*/
/*=                                                                    =*/
/*= All these functons propagate the new depth to the parents of the   =*/
/*= dir object.                                                        =*/
/*=                                                                    =*/
/*=   dir: A directory object or NULL.                                 =*/

static void obj_depth_childChg(TobjDir dir, int oldmin, int oldmax, int newmin, int newmax);

static void obj_depth_reinit(TobjDir dir)
{
    int i;
    if ( dir==0 ) return;

    for (i=0 ; i<dir->OCO_children->eleNb ; i+=1) {
        int childmin =  dir->OCO_children->eles[i].any->OA_depthmin;
        int childmax =  dir->OCO_children->eles[i].any->OA_depthmax;
        if ( ! (childmin==0 && childmax==0) ) break;
    }
    if (  i<dir->OCO_children->eleNb ) {
        dir->O_depthmin = dir->OCO_children->eles[i].any->OA_depthmin;
        dir->O_depthmax = dir->OCO_children->eles[i].any->OA_depthmax;
        for (i=i+1 ; i<dir->OCO_children->eleNb ; i+=1) {
            int childmin =  dir->OCO_children->eles[i].any->OA_depthmin;
            int childmax =  dir->OCO_children->eles[i].any->OA_depthmax;
            if ( childmin==0 && childmax==0 ) continue;
            if ( childmin < dir->O_depthmin ) dir->O_depthmin = childmin;
            if ( childmax > dir->O_depthmax ) dir->O_depthmax = childmax;
        }
    } else {
        dir->O_depthmin = 0;
        dir->O_depthmax = 0;
    }
}

static void obj_depth_childSup(TobjDir dir, int min, int max)
{
    if ( dir==0 ) return;
    if ( min==0 && max==0 ) return;
    if ( dir->OCO_children->eleNb==0 ) {
        dir->O_depthmin = 0;
        dir->O_depthmax = 0;
        return;
    }
    int dirmin = dir->O_depthmin;
    int dirmax = dir->O_depthmax;
    if ( dirmin==min || dirmax==max ) {
        obj_depth_reinit(dir);
        obj_depth_childChg(dir->O_parent, dirmin,dirmax, dir->O_depthmin,dir->O_depthmax);
    }
}

static void obj_depth_childAdd(TobjDir dir, int min, int max)
{
    int i;
    if ( dir==0 ) return;
    if ( min==0 && max==0 ) return;

    int dirmin = dir->O_depthmin;
    int dirmax = dir->O_depthmax;
    if ( dirmin==0 && dirmax==0 ) {
        dir->O_depthmin = min;
        dir->O_depthmax = max;
    } else {
        if ( min<dirmin ) dir->O_depthmin = min;
        if ( max>dirmax ) dir->O_depthmax = max;
    }
    obj_depth_childChg(dir->O_parent, dirmin,dirmax, dir->O_depthmin,dir->O_depthmax);
}

static void obj_depth_childChg(TobjDir dir, int oldmin, int oldmax, int newmin, int newmax)
{
    if ( dir==0 ) return;
    if ( oldmin==0 && oldmax==0 ) { obj_depth_childAdd(dir,newmin,newmax); return; }
    if ( newmin==0 && newmax==0 ) { obj_depth_childSup(dir,oldmin,oldmax); return; }
    if ( oldmin==newmin && oldmax==newmax ) return;

    int dirmin = dir->O_depthmin;
    int dirmax = dir->O_depthmax;
    if ( newmin<=dirmin && dirmax<=newmax ) {
        dir->O_depthmin = newmin;
        dir->O_depthmax = newmax;
    } else if ( oldmin==dirmin || oldmax==dirmax ) {
        obj_depth_reinit(dir);
    }
    obj_depth_childChg(dir->O_parent, dirmin,dirmax, dir->O_depthmin,dir->O_depthmax);
}

/*======================================================================*/
/*= Attach to and detach object from a figure                          =*/

static void update_fig_objcnt(Tfigure* fig, int kind, int incr)
{
    if ( fig==0 ) return;
    Tfig_objCounters* counters = &fig->objCounters;
    switch ( kind ) {
        case FIG_KD_Ellipse:  counters->num_ellipses += incr; break;
        case FIG_KD_Arc:      counters->num_arcs     += incr; break;
        case FIG_KD_Line:     counters->num_lines    += incr; break;
        case FIG_KD_Text:     counters->num_texts    += incr; break;
        case FIG_KD_Spline:   counters->num_splines  += incr; break;
        case FIG_KD_Compound: break;
        case FIG_KD_Boxhv:    counters->num_others   += incr; break;
        default:
              if ( ! (FIG_KD_ArrowT00<=kind && kind<=FIG_KD_ArrowTend) )
                    PERM_spe_printf("%d is an uregonized object kind", kind);
              return;
    }
}

extern void     obj_fig_attach (TobjAll obj,Tfigure*fig)
{
    VARS_Self(obj);
    int i,objKind = self->OA_vtable->vt_kind;
    if ( self->OA_figure ) {
        if ( self->OA_figure==fig ) return;
        obj_fig_detach(obj);
    }
    obj_chgPpi(obj,fig->fig_ppi);

    if ( objKind==FIG_KD_Compound ) {
        TobjAllSet* children = obj.dir->OCO_children;
        for (i=0 ; children && i<children->eleNb ; i+=1)
            obj_fig_attach(children->eles[i],fig);
        // no arrow, no drawing styles in directories
    } else {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_fig_attach(obj_ObjAll(arrow),fig);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_fig_attach(obj_ObjAll(arrow),fig);
        // updates drawing styles
        Tsty_line*stl; Tsty_fill*stf; Tsty_text*stt;
        if ( (stl=obj_StyLine(obj))!=0 )
            color_chgLazy(&stl->stl_color,fig_colors_getByClr(fig,stl->stl_color));
        if ( (stf=obj_StyFill(obj))!=0 )
            color_chgLazy(&stf->stf_color,fig_colors_getByClr(fig,stf->stf_color));
        if ( (stt=obj_StyText(obj))!=0 )
            color_chgLazy(&stt->stt_color,fig_colors_getByClr(fig,stt->stt_color));
        // update the figure object counters 
        update_fig_objcnt( self->OA_figure,  objKind, 1);
    }
    self->OA_figure = fig;
}

extern void     obj_fig_detach(TobjAll obj)
{
#define CHG_CLR(clr) do { Tcolor* __priv = color_newPriv(clr); \
                 color_chgLazy(&clr,__priv); color_delete(__priv); } while (0)
    VARS_Self(obj);
    int i,objKind = self->OA_vtable->vt_kind;
    if ( self->OA_figure==0 ) return;

    if ( objKind==FIG_KD_Compound ) {
        TobjAllSet* children = obj.dir->OCO_children;
        for (i=0 ; children && i<children->eleNb ; i+=1)
            obj_fig_detach(children->eles[i]);
        // no arrow, no drawing styles in directories
    } else {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_fig_detach(obj_ObjAll(arrow));
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_fig_detach(obj_ObjAll(arrow));
        // updates drawing styles
        Tsty_line*stl; Tsty_fill*stf; Tsty_text*stt;
        if ( (stl=obj_StyLine(obj))!=0 ) CHG_CLR(stl->stl_color);
        if ( (stf=obj_StyFill(obj))!=0 ) CHG_CLR(stf->stf_color);
        if ( (stt=obj_StyText(obj))!=0 ) CHG_CLR(stt->stt_color);
        // update the figure object counters 
        update_fig_objcnt( self->OA_figure,  objKind, -1);
    }
    self->OA_figure = 0;
}

/*======================================================================*/
/*= parent/unparent an object to/from of a directory object.           =*/

static void obj_unparent(TobjAll dir, TobjAll obj)
{
    int i;
    TobjAny*      any  = obj.any;
    TobjDir       self = dir.dir;
    if ( self==0 ) return;

    switch ( obj_Kind(obj) ) {
        case FIG_KD_Arc:      objlist_remove(&self->OCO_arcs,     any); break;
        case FIG_KD_Ellipse:  objlist_remove(&self->OCO_ellipses, any); break;
        case FIG_KD_Line:     objlist_remove(&self->OCO_lines,    any); break;
        case FIG_KD_Spline:   objlist_remove(&self->OCO_splines,  any); break;
        case FIG_KD_Text:     objlist_remove(&self->OCO_texts,    any); break;
        case FIG_KD_Boxhv:    objlist_remove(&self->OCO_others,   any); break;
        case FIG_KD_Compound: objlist_remove(&self->OCO_compounds,any); break;  
        default:
          PERM_spf_printf("%d is an unexpected object kind.", obj_Kind(obj));
    }
    for (i=0 ; self->OCO_children!=0 && i<self->OCO_children->eleNb ; i+=1) {
        if ( self->OCO_children->eles[i].any == any ) {
            objAllSet_supQuick( self->OCO_children, i );
            break;
        }
    }
    any->OA_parent = 0;
    obj_depth_childSup(self, any->OA_depthmin, any->OA_depthmax);
    self->O_tags &= ~FIG_TAG_BBoxUpToDate;
}

/*======================================================================*/
/*= object constructors & destructor                                   =*/

extern TobjAll obj_new(void*vt)
{
    TobjAll obj;
    TobjVtable* vtable = vt;
    obj.any=tool_malloc_0( vtable->vt_bytesz);
    obj.any->OA_vtable = vtable;
    obj.any->OA_cnt = 1;
    obj.any->OA_ppi = LUNIT_FuDefault;
#if defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj
    dbg_obj_new( obj );
#endif
    return obj;
}

extern void     obj_delete(TobjAll obj)
{
    if ( OBJ_IsNone(obj) ) return;
    VARS_SeVt(obj);
#if defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj
    dbg_obj_del( obj );
#endif
    self->OA_cnt -= 1;
    if ( self->OA_cnt>0 ) {
        return;
    }

    int i,objKind = vtable->vt_kind;
    if ( self->OA_figure!=0 ) obj_fig_detach(obj);

    if ( objKind==FIG_KD_Compound ) {
        TobjAllSet* children = obj.dir->OCO_children;
        for (i=0 ; children && i<children->eleNb ; i+=1)
            obj_delete(children->eles[i]);
        // no arrow, no drawing styles in directories
    } else {
        // delete arrows if any
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_delete(obj_ObjAll(arrow));
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_delete(obj_ObjAll(arrow));
        // free styles
        Tsty_line*stl; Tsty_fill*stf; Tsty_text*stt;
        if ( (stl=obj_StyLine(obj))!=0 ) odsty_freeLine(stl);
        if ( (stf=obj_StyFill(obj))!=0 ) odsty_freeFill(stf);
        if ( (stt=obj_StyText(obj))!=0 ) odsty_freeText(stt);
    }

    // free proper data
    if ( vtable->vt_free )
        vtable->vt_free( obj );

    free( self );
}

extern void     obj_deleteS0(TobjAll* obj)
{ obj_delete(*obj); *obj=OBJ_none; }

/*======================================================================*/

extern void obj_dir_flat   (TobjAll dir)
{
    int i;
    TobjDir self   = dir.dir;
    TobjAll parent = obj_ObjAll(self->O_parent);

    if ( OBJ_IsNone(parent) ) return;

    TobjAllSet* chidren = objAllSet_newClone( self->OCO_children );
    for (i=0 ; i<chidren->eleNb ; i +=1) {
        obj_dir_addObj(parent,chidren->eles[i]);
    }
    objAllSet_delete(chidren);

    obj_dir_delObjQ(dir);
}

extern void obj_dir_addObj(TobjAll dir, TobjAll obj)
{
    TobjAny* any    = obj.any;
    TobjDir  newdir = dir.dir;
    TobjDir  olddir = any->OA_parent;
    Tfigure* dirfig = newdir->O_figure; // newdir figure
    Tfigure* objfig = any->OA_figure;   // obj figure

    if ( dirfig==objfig ) {
        // none none or fig fig
        obj_unparent( obj_ObjAll(olddir),obj);
    } else if (dirfig==NULL && objfig!=NULL) {
        TEMP_asf_printf( any->oa_parent!=0, "%s","found an attached fig unparented" );
        // unparent + detach obj of objfig
        obj = obj_dir_remObj( obj_ObjAll(olddir), obj);
    } else if (dirfig!=NULL && objfig==NULL) {
        obj_unparent( obj_ObjAll(olddir), obj);
        obj_fig_attach(obj,dirfig);
    }
    obj_chgPpi(obj,obj_Ppi(dir));

    // reparent obj
    any->oa_parent = newdir;
    switch ( obj_Kind(obj) ) {
      case FIG_KD_Arc:      objlist_addAhead( &newdir->OCO_arcs     ,any); break;
      case FIG_KD_Ellipse:  objlist_addAhead( &newdir->OCO_ellipses ,any); break;
      case FIG_KD_Line:     objlist_addAhead( &newdir->OCO_lines    ,any); break;
      case FIG_KD_Spline:   objlist_addAhead( &newdir->OCO_splines  ,any); break;
      case FIG_KD_Text:     objlist_addAhead( &newdir->OCO_texts    ,any); break;
      case FIG_KD_Compound: objlist_addAhead( &newdir->OCO_compounds,any); break;
      case FIG_KD_Boxhv:    objlist_addAhead( &newdir->OCO_others   ,any); break;
      default:
        PERM_spe_printf("%d is an unexpected object kind.", obj_Kind(obj));
        return ;
    }
    if ( newdir->OCO_children==0 ) newdir->OCO_children = objAllSet_new(10);
    objAllSet_add( newdir->OCO_children, obj);

    // update depth of newdir/dir and its parents
#if 1
    obj_depth_childAdd(newdir,any->OA_depthmin,any->OA_depthmax);
#else
    TobjDir  p; TobjAny* child;
    for ( p=newdir,child=any ; p!=0 ;
            child=O_2A(p),p=(TobjCompound*)p->O_parent ) {
        if ( p->O_depthmin==0  && p->O_depthmax==0 ) {
            // first element
            p->O_depthmin = child->OA_depthmin;
            p->O_depthmax = child->OA_depthmax;
        } else {
            if ( child->OA_depthmin<p->O_depthmin )
                p->O_depthmin = child->OA_depthmin;
            if ( child->OA_depthmax>p->O_depthmax )
                p->O_depthmax = child->OA_depthmax;
        }
    }
#endif

    // update the bounding box of newdir/dir object and of its parents 
    if ( (any->OA_tags&FIG_TAG_BBoxUpToDate)==0 ) {
        TobjDir  p; TobjAny* child;
        for ( p=newdir,child=any ; p!=0 && (p->O_tags&FIG_TAG_BBoxUpToDate)!=0 ;
                child=O_2A(p),p=(TobjCompound*)p->O_parent )
            p->O_tags &= ~FIG_TAG_BBoxUpToDate;
    } else {
        TobjDir  p; TobjAny* child;
        for ( p=newdir,child=any ; p!=0 && (p->O_tags&FIG_TAG_BBoxUpToDate)!=0 ;
                child=O_2A(p),p=(TobjCompound*)p->O_parent ) {
            tool_boxDble_union(&p->O_boundBox,obj_BBox(obj_ObjAll(child)));
        }
    }
}

extern TobjAll obj_dir_remObj(TobjAll dir, TobjAll obj)
{
    VARS_Self(obj);
    TEMP_asf_printf( self==0,   "%s", "attemp to remove an undefined object");
    TEMP_asf_printf( dir.dir==0,"%s", "dir is an undefined object");
    TEMP_asf_printf( self->OA_parent!=dir.dir,"%s", "dir is not the obj parent");
    obj_unparent(dir,obj);
    obj_fig_detach(obj);

    return obj;
}

extern void    obj_dir_delObj (TobjAll pdir, TobjAll obj)
{ obj = obj_dir_remObj( pdir, obj ); obj_delete( obj ); }

extern void    obj_dir_movObj (TobjAll pdir, TobjAll obj, TobjAll to);

extern TobjAll obj_dir_remObjQ(TobjAll obj)
{
    VARS_Self(obj);
    if ( self==0 ) return obj;
    if ( self->OA_parent!=0 )
        obj = obj_dir_remObj( obj_ObjAll(self->OA_parent), obj );
    return obj;
}

extern void    obj_dir_delObjQ(TobjAll obj)
{ obj=obj_dir_remObjQ(obj); obj_delete(obj); }

extern void    obj_dir_movObjQ(TobjAll obj, TobjAll to);

/*======================================================================*/

extern TobjAll obj_newClone(CobjAll obj)
{
    int i;
    TobjAll* arrow_obj;
    TobjAll* arrow_clone;
    TobjAll  clone;
    TobjAny* aclone;

    if ( OBJ_IsNone(obj) ) return OBJ_none;

    VARS_SeVt(obj);

    if ( vtable->vt_dup!=0 ) {
        clone = vtable->vt_dup( obj );
        aclone=clone.any;
        aclone->OA_parent = 0;
        aclone->OA_figure = 0;
        if ( (arrow_clone=obj_arrow_BegPtr(clone))!=0 &&
             (arrow_obj  =obj_arrow_BegPtr(obj))!=0 && OBJ_IsDefined(*arrow_obj) ) {
            *arrow_clone = obj_newClone( *arrow_obj);
            arrow_clone->any->OA_partof = clone;
        }
        if ( (arrow_clone=obj_arrow_EndPtr(clone))!=0 &&
             (arrow_obj  =obj_arrow_EndPtr(obj))!=0 && OBJ_IsDefined(*arrow_obj) ) {
            *arrow_clone = obj_newClone( *arrow_obj);
            arrow_clone->any->OA_partof = clone;
        }
        return clone;
    }
    aclone=clone.any= tool_malloc( vtable->vt_bytesz );
    memcpy(aclone,obj.any,vtable->vt_bytesz);

    // unparent and detach
    Tsty_line*stl; Tsty_fill*stf; Tsty_text*stt;
    aclone->OA_tags  &= ~(FIG_TAG_HISTORY);
    aclone->OA_cnt    = 1;
    aclone->OA_figure = 0;
    aclone->OA_parent = 0; 
    aclone->OA_partof = OBJ_none; 
    aclone->OA_next   = 0; 
    aclone->OA_comment= 0; 
    if ( (stl=obj_StyLine(clone))!=0 ) stl->stl_color=color_newPriv(stl->stl_color);
    if ( (stf=obj_StyFill(clone))!=0 ) stf->stf_color=color_newPriv(stf->stf_color);
    if ( (stt=obj_StyText(clone))!=0 ) stt->stt_color=color_newPriv(stt->stt_color);

    // clone proper data
    if ( vtable->vt_clone!=0 ) vtable->vt_clone(clone);

    // clone arrow if any
    if ( (arrow_clone=obj_arrow_BegPtr(clone))!=0 && OBJ_IsDefined(*arrow_clone)) {
        *arrow_clone = obj_newClone( *arrow_clone);
        arrow_clone->any->OA_partof = clone;
    }
    if ( (arrow_clone=obj_arrow_EndPtr(clone))!=0 && OBJ_IsDefined(*arrow_clone) ) {
        *arrow_clone = obj_newClone( *arrow_clone);
        arrow_clone->any->OA_partof = clone;
    }

    return clone;
}

extern TobjAll   obj_newLazy(CobjAll obj)
{
    VARS_Self(obj);
    self->OA_cnt += 1;
    return obj;
}

/*======================================================================*/
extern double obj_getLength(TobjAll obj)
{
    VARS_SeVt(obj);
    switch ( self->OA_vtable->vt_kind ) {
        case FIG_KD_Compound :  break;
        case FIG_KD_Boxhv :     break;
        case FIG_KD_Text :      break;
        default :               
            if ( self->OA_vtable->vt_length ) return self->OA_vtable->vt_length(obj);
    }
    return 0.0;
}

/*======================================================================*/
#if defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj

#define DBG_object_unfreedMAX 1000

TobjAll dbg_object_unfreedT[DBG_object_unfreedMAX];
int     dbg_object_unfreedN;

extern void    dbg_obj_new(TobjAll obj)
{
    if ( OBJ_IsNone(obj) ) return;
    int i;
    for (i=0 ; i<dbg_object_unfreedN ; i++) {
        if ( dbg_object_unfreedT[i].any==obj.any ) {
            return ;
        }
    }
    // c not found in dbg_object_unfreedT, we add it
    if (dbg_object_unfreedN==DBG_object_unfreedMAX) return;
    dbg_object_unfreedT[dbg_object_unfreedN++] = obj;
}
extern void    dbg_obj_del(TobjAll obj)
{
    if ( OBJ_IsNone(obj) ) return;
    PERM_asf_printf(obj.any->OA_cnt<1, "unexpected ref counts %d for %p obj",
        obj.any->OA_cnt, obj.any);
    int i;
    for (i=0 ; i<dbg_object_unfreedN ; i++) {
        if ( dbg_object_unfreedT[i].any==obj.any ) {
            if ( obj.any->OA_cnt==1 ) {
                dbg_object_unfreedN -= 1;
                dbg_object_unfreedT[i] = dbg_object_unfreedT[dbg_object_unfreedN];
            }
            return ;
        }
    }
    // c not found in dbg_object_unfreedT, we add it if refcount is not 1
    if ( obj.any->OA_cnt==1 ) return;
    if (dbg_object_unfreedN==DBG_object_unfreedMAX) return;
    dbg_object_unfreedT[dbg_object_unfreedN++] = obj;
}

extern void    dbg_obj_printUnfreed()
{
    int i;
    for (i=0 ; i<dbg_object_unfreedN ; i++) {
        TobjAll obj = dbg_object_unfreedT[i];
        obj_dump(obj,0);
    }
}

#endif // TOOL_DBG_MemObj

/*======================================================================*/
