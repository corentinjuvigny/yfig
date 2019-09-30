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
 *      $Id: obj-compound.c 266 2018-07-04 00:11:04Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-compound.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= An object container.                                               =*/
/*= Implement:                                                         =*/
/*=   - the virtual table of the TobjCompound type.                    =*/
/*=   - the constructors of the TobjCompound type.                     =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"     // used just for dumping it
#include "fig/object-priv.h"
#include "fig/obj-compound.h"

#include "fig/fig-scene.h"   // used for drawing it
#include "outdev/ldaf-hl.h"  // used for drawing it

/*======================================================================*/

#define MVARS_Self(obj) TobjCompound *self = obj.dir; TobjAny *o
#define MVARS_SeVt(obj) MVARS_Self(obj); CobjVtable *vtable = self->O_vtable

#define DBG0_E(fmt,...) DBG0_printf("compound","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("compound","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("compound","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("compound","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("compound","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("compound","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("compound","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("compound","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("compound","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Top information.                                                   =*/

static void     compound_dump(TobjAll obj, int sh)
{
    TobjAll* arrow;
    MVARS_Self(obj);
    char tmp[100]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh);

    if ( self->O_parent==0 && self->O_figure!=0 ) {
        Tfigure* fig = self->O_figure;
            printf("%20s:%s:<<<--figure=%p--compound=%p----------\n",
                    __func__,tmp,fig,self);

            printf("%20s:%s:     path        =%s\n",__func__,tmp,fig->path ? fig->path : "");
            printf("%20s:%s:     loaded_fmt  =%d\n",__func__,tmp,fig->loaded_format);
            printf("%20s:%s:     save_fmt    =%d\n",__func__,tmp,fig->save_format);
            printf("%20s:%s:     io_ppi      =%d\n",__func__,tmp,fig->io_ppi);
            printf("%20s:%s:     num_ellipses=%d\n",__func__,tmp,fig->objCounters.num_ellipses);
            printf("%20s:%s:     num_arcs    =%d\n",__func__,tmp,fig->objCounters.num_arcs);
            printf("%20s:%s:     num_texts   =%d\n",__func__,tmp,fig->objCounters.num_texts);
            printf("%20s:%s:     num_lines   =%d\n",__func__,tmp,fig->objCounters.num_lines);
            printf("%20s:%s:     num_splines =%d\n",__func__,tmp,fig->objCounters.num_splines);
            printf("%20s:%s:     num_others  =%d\n",__func__,tmp,fig->objCounters.num_others);
            printf("%20s:%s:     fig_ppi     =%f\n",__func__,tmp,fig->fig_ppi);
            printf("%20s:%s:-----figure=%p--compound=%p------->>>\n",
                    __func__,tmp,fig,fig->objects);
    } else {
            printf("%20s:%s:<<<--figure=%p--compound=%p------->>>\n",
                    __func__,tmp,NULL,NULL);
    }
    obj_dump_any(obj,sh,__func__);
    printf(" child=%p(%d)\n",self->OCO_children, self->OCO_children ?  self->OCO_children->eleNb : -1);
    STD_LOOPS(o,
        obj_dump( obj_ObjAll(o), sh+3 );
        if ( (arrow=obj_arrow_BegPtr(obj_ObjAll(o)))!=0 && OBJ_IsDefined(*arrow)) obj_dump(*arrow,sh+6);
        if ( (arrow=obj_arrow_EndPtr(obj_ObjAll(o)))!=0 && OBJ_IsDefined(*arrow)) obj_dump(*arrow,sh+6);
    );
}

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

static void compound_free(TobjAll obj)
{
    MVARS_Self(obj);
    objAllSet_delete( self->OCO_children );
}

static TobjAll compound_dup(TobjAll obj)
{
    MVARS_Self(obj);
    DBG1_E("obj=%p",self);
    TobjAll       dup  = obj_new((void*)self->O_vtable);
    TobjCompound *adup = dup.dir;
    TobjAll       on;

    STD_LOOPS_NoComp(o,
        on = obj_newClone( obj_ObjAll(o) );
        obj_dir_addObj(dup,on);
    )

    STD_LOOP(o,OCO_compounds , 
        on = compound_dup(  obj_ObjAll(o) );
        obj_dir_addObj(dup,on);
    )

    DBG1_L("obj=%p",self);
    return dup;
}

//static void    vt_clone(Tobject obj);

static TboxDble compound_boundBox(CobjAll obj)
{
    MVARS_Self(obj);

    TboxDble bb = tool_boxDble_undef;

    STD_LOOPS(o, tool_boxDble_union(&bb, obj_BBox( obj_ObjAll(o) )); )
    
    return bb;
}

/*======================================================================*/
/*= geometric transformation                                           =*/

static void compound_translation(TobjAll obj, double tx, double ty)
{ MVARS_Self(obj); STD_LOOPS(o, obj_translation(obj_ObjAll(o),tx,ty); ) }

static void compound_homothety00(TobjAll obj, int wtcf, double f)
{ MVARS_Self(obj); STD_LOOPS(o, obj_homothety00(obj_ObjAll(o),wtcf,f); ) }

static void compound_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{ MVARS_Self(obj); STD_LOOPS(o, obj_homothetyCC(obj_ObjAll(o),wtcf,f,cx,cy); ) }

static void compound_scale00(TobjAll obj, int wtcf, double fx, double fy)
{ MVARS_Self(obj); STD_LOOPS(o, obj_scale00(obj_ObjAll(o),wtcf,fx,fy); ) }

static void compound_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{ MVARS_Self(obj); STD_LOOPS(o, obj_scaleCC(obj_ObjAll(o),wtcf,fx,fy,cx,cy); ) }

static void compound_rotate(TobjAll obj, double a, double cx, double cy)
{ MVARS_Self(obj); STD_LOOPS(o, obj_rotate(obj_ObjAll(o),a,cx,cy); ) }

static void compound_flip(TobjAll obj, int h, int v, int r, double cx, double cy)
{ MVARS_Self(obj); STD_LOOPS(o, obj_flip(obj_ObjAll(o),h,v,r,cx,cy); ) }

/*======================================================================*/
/*= Drawing and filling.                                               =*/

static void compound_drawI(CobjCompound* self,
        Tfig_scene* sc, Tod_GCdef gcd, int depth)
{
    TobjAll o;

    for ( o.any=self->OCO_arcs  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_ellipses  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_lines  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_splines  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_texts  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_others  ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
    for ( o.any=self->OCO_compounds ; o.any!=0 ; o.any=o.any->oa_next ) {
        if ( ! (o.any->OA_depthmin<=depth && depth<o.any->OA_depthmax) ) continue;
        fig_scene_draw_obj(sc,o,OD_GC_UsrDraw);
    }
}

static void compound_draw_sc(CobjAll   obj, Tfig_scene* sc,
                             Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);
    Toutdev* dev  = sc->sc_device;

    if ( gcd==OD_GC_UsrDraw )
        compound_drawI(self,sc,gcd,sc->sc_rtp->currDepth);
    else
        od_draw_rectBB(dev,gcd,self->O_boundBox);
}

static void compound_clrEGScache(CobjAll   obj, Toutdev* od)
{
    MVARS_Self(obj);
    STD_LOOPS(o, obj_clrEGScache( obj_ObjAll(o),od); )
}

/*======================================================================*/
/*= The table                                                          =*/
#include "fig/obj-vtable.h"

TobjVtable compound_vtable = {
    .vt_kind          = FIG_KD_Compound,
    .vt_bytesz        = sizeof( TobjCompound ),
    .vt_mask          = FIG_KDM_DIR,
    .vt_name          = "directory",
    .vt_dump          = compound_dump,

    .vt_free          = compound_free,
    .vt_dup           = compound_dup,
    .vt_boundBox      = compound_boundBox,

    .vt_translation   = compound_translation,
    .vt_homothety00   = compound_homothety00,
    .vt_homothetyCC   = compound_homothetyCC,
    .vt_scale00       = compound_scale00,
    .vt_scaleCC       = compound_scaleCC,
    .vt_rotate        = compound_rotate,
    .vt_flip          = compound_flip,

    .vt_draw_sc       = compound_draw_sc,
    .vt_clrEGScache   = compound_clrEGScache,
    .vt_markerBB      = 1
};

/*======================================================================*/
/*= Constructors                                                       =*/

static void obj_compound_init(TobjAll obj)
{
}

extern TobjAll obj_dir_newF(TobjVTT vtt)
{
    TobjAll obj;
    obj=obj_newByOid(FIG_KD_Compound,vtt);
    obj_compound_init(obj);
    return obj;
}

extern TobjAll obj_compound_newA(Cfdp* fdp)
{
    TobjAll obj;
    obj=obj_newByOid(FIG_KD_Compound,fdp->fdp_vtt);
    obj_compound_init(obj);
    return obj;
}

extern TobjAll obj_dir_mkdir     (TobjAll dir)
{
    TobjAll obj;
    obj=obj_new((void*)dir.any->OA_vtable);
    obj_compound_init(obj);
    obj_dir_addObj(dir,obj);
    return obj;
}

/*======================================================================*/
