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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: fig-scene.c 337 2018-11-05 22:44:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-scene.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the fig/fig-scene.h header for documentation.                  =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"
#include "fig/object-priv.h"
#include "fig/obj-vtable.h"
#include "fig/obj-any.h"
#include "fig/fig-scene.h"

#include "outdev/style.h"
#include "outdev/outdev.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"

/*======================================================================*/
/* Interface for creating/deleting scene                               =*/

static Tfig_scene* fig_scene_new()
{
    Tfig_scene* scene = tool_malloc_0( sizeof(*scene) );
    return scene;
}

extern Tfig_scene* fig_scene_newByFig (Tfigure*fig)
{
    Tfig_scene* scene = fig_scene_new();
    fig_scene_enableAllDepths( scene );
    scene->sc_depths.unactiveMode=0;   // ignore unactive depth
    scene->sc_selectableObjectMask = 0;
    scene->sc_dessin = fig; // fig_new_lazy( fig );
    scene->sc_currObj = obj_newLazy( obj_ObjAll(fig->objects) );
    return scene;
}

//extern Tfig_scene* fig_scene_newByPath(CstrBrief path);
extern void fig_scene_delete(Tfig_scene*sc)
{
    obj_delete( sc->sc_currObj );
    if (sc->sc_dessin)  fig_delete( sc->sc_dessin );
    free( sc );
}

#include "outdev/src-vt.h"

static void outdev_draw(Coutdev*od)
{
    fig_scene_draw(od->parSrc);
}

static Toutdev_src_vt scene_src_vt = {
    .parent = &outdev_src_vt,
    .draw   = outdev_draw,
};

extern void        fig_scene_attachDev(Tfig_scene*sc, Toutdev*od,
                                       int realSize)
{
    Tfigure* fig  = sc->sc_dessin;
    sc->sc_device = od;
    od_attach(od,&scene_src_vt,fig->fig_ppi,sc);
    if ( realSize==0 ) {
        // figure is scalled to enter the device
        CboxDble* box = obj_BBox( obj_ObjAll(fig->objects) );
        od_updateSrcBB_toBox( od, box );
    }
#if 0
printf("%s: SRC=%f/%f DES=%f/%f : homo=%f zoom=%f\n",__func__,
 Box_dx(od->boxSrc),Box_dy(od->boxSrc),
 Box_dx(od->boxDes),Box_dy(od->boxDes),
 od->homothetySrc2Des, od_getUsrZoom(od)
);
#endif
}

extern void        fig_scene_attachDevXY(Tfig_scene*sc, Toutdev*od,
                                       double x, double y)
{
    Tfigure* fig  = sc->sc_dessin;
    sc->sc_device = od;
    od_attach(od,&scene_src_vt,fig->fig_ppi,sc);
    CboxDble* box = obj_BBox( obj_ObjAll(fig->objects) );
    od_updateSrcBB_toBoxXY( od, box, x, y );
}

/*======================================================================*/
/* Interface for configuring scene                                     =*/

extern void fig_scene_enableAllDepths (Tfig_scene*sc)
{
    int i;
    Tfig_sceneDepth* depths = &sc->sc_depths;
    for (i=0;i<FIG_MAX_DEPTH;i+=1) depths->table[i]=1;
}

extern void fig_scene_disableAllDepths(Tfig_scene*sc)
{
    int i;
    Tfig_sceneDepth* depths = &sc->sc_depths;
    for (i=0;i<FIG_MAX_DEPTH;i+=1) depths->table[i]=0;
}

#define ASF_ValidDepth(i) TEMP_asf_printf(i<0||FIG_MAX_DEPTH<=i,"i=%d is an invalid depth (expected in [0:%d[)",i,FIG_MAX_DEPTH)
extern void fig_scene_enableDepth     (Tfig_scene*sc, int d) { ASF_ValidDepth(d); sc->sc_depths.table[d]=1; }
extern void fig_scene_disableDepth    (Tfig_scene*sc, int d) { ASF_ValidDepth(d); sc->sc_depths.table[d]=0; }

extern TobjDir fig_scene_getTopObj (Cfig_scene*sc) { return sc->sc_dessin->objects; }
extern TobjAll fig_scene_getCurrObj(Cfig_scene*sc) { return sc->sc_currObj; }
extern void    fig_scene_setCurrObj(Cfig_scene*sc, TobjAll o)
{ 
    TobjAll old = sc->sc_currObj;
    ((Tfig_scene*)sc)->sc_currObj = obj_newLazy(o);
    obj_delete( old );
}

/*======================================================================*/
/*= Interface for getting bounding boxes.                              =*/

extern CboxDble* fig_scene_BBox     (Tfig_scene*sc)
{
    CboxDble* box = obj_BBox( obj_ObjAll(sc->sc_dessin->objects) );
    return box;
}

extern TboxDble  fig_scene_BBoxTopAll(Tfig_scene*sc) { return *fig_scene_BBox(sc); }
extern TboxDble  fig_scene_BBoxTopSel(Tfig_scene*sc)
{
    int i;
    TboxDble    box  = tool_boxDble_undef;
    TobjAll     root = obj_ObjAll(fig_scene_getTopObj(sc));
    TobjAllSet* set  = obj_getChildrenD(root,sc->sc_depths.table);
    for ( i=0 ; i<set->eleNb ; i+=1) 
        tool_boxDble_union(&box,obj_BBox(set->eles[i]));
    objAllSet_delete(set);
    return box;
}

extern TboxDble  fig_scene_BBoxCurAll(Tfig_scene*sc) { return *obj_BBox( sc->sc_currObj ); }
extern TboxDble  fig_scene_BBoxCurSel(Tfig_scene*sc)
{
    int i;
    TboxDble    box  = tool_boxDble_undef;
    TobjAll     root = fig_scene_getCurrObj(sc);
    TobjAllSet* set  = obj_getChildrenD(root,sc->sc_depths.table);
    for ( i=0 ; i<set->eleNb ; i+=1) 
        tool_boxDble_union(&box,obj_BBox(set->eles[i]));
    objAllSet_delete(set);
    return box;
}

static TboxDble  fig_scene_convBox(Cfig_scene* sc,TboxDble box,int uu)
{
    tool_boxDble_time(&box,LUNIT_FuPerInch/sc->sc_dessin->fig_ppi*lu_convFromFu(1,uu));
    return box;
}

extern TboxDble  fig_scene_BBoxTopAllU(Tfig_scene*sc, int uu) { TboxDble box = fig_scene_BBoxTopAll(sc); return fig_scene_convBox(sc,box,uu); }
extern TboxDble  fig_scene_BBoxTopSelU(Tfig_scene*sc, int uu) { TboxDble box = fig_scene_BBoxTopSel(sc); return fig_scene_convBox(sc,box,uu); }
extern TboxDble  fig_scene_BBoxCurAllU(Tfig_scene*sc, int uu) { TboxDble box = fig_scene_BBoxCurAll(sc); return fig_scene_convBox(sc,box,uu); }
extern TboxDble  fig_scene_BBoxCurSelU(Tfig_scene*sc, int uu) { TboxDble box = fig_scene_BBoxCurSel(sc); return fig_scene_convBox(sc,box,uu); }

/*======================================================================*/
/* Interface for printing scene                                        =*/

extern void      fig_scene_zoom     (Tfig_scene*sc, double zoom)           { obj_zoom       ( obj_ObjAll(fig_scene_getTopObj(sc)), zoom); }
extern void      fig_scene_translate(Tfig_scene*sc, double tx, double ty)  { obj_translation( obj_ObjAll(fig_scene_getTopObj(sc)), tx,ty); }

extern int       fig_scene_draw     (Tfig_scene*sc)
{
    DBG0_DrawE("sc=%p dir=%p",sc, sc ? sc->sc_dessin->objects : NULL);
    if ( sc==0 ) return 0;
    Tfig_sceneRTP rtp;
    sc->sc_rtp = &rtp;

    //TobjAll obj;  obj.dir = sc->sc_dessin->objects;
    TobjAll obj = sc->sc_currObj;
    TobjAny*any = obj.any;
od_ds_stdStart(sc->sc_device);
    for ( rtp.currDepth=FIG_MAX_DEPTH-1 ; rtp.currDepth>=0 ; rtp.currDepth-=1 ) {
        if ( ! sc_isActive(sc,rtp.currDepth) ) continue; 
        if ( ! (any->OA_depthmin<=rtp.currDepth &&
                rtp.currDepth<any->OA_depthmax) ) continue;
        DBG0_DrawM("sc=%p dir=%p doing depth %d",sc,any,rtp.currDepth);
        fig_scene_draw_obj(sc,obj,OD_GC_UsrDraw);
    }
od_ds_stdCommit(sc->sc_device);
    DBG0_DrawL("sc=%p dir=%p",sc, sc ? sc->sc_dessin->objects : NULL);

    sc->sc_rtp = 0;
    return 0;
}


extern void      fig_scene_draw_obj(Tfig_scene*sc, CobjAll obj, Tod_GCdef gcd)
{
    CobjAny* self = obj.any;
    if ( obj_isDrawingDisabled(obj) ) return;
#warning rtp must be a parameter
//    Tfig_sceneRTP rtp;
//    if ( self->OA_depthmin<=rtp.currDepth && rtp.currDepth<self->OA_depthmax) return;

    obj_draw(sc,obj,gcd);
}

extern void      obj_draw (Tfig_scene*sc, CobjAll obj, Tod_GCdef gcd)
{
    TobjAll*     arrow;
    CobjAny*     self   = obj.any;
    CobjVtable*  vtable = self->OA_vtable;
    PERM_asf_printf(vtable==0,"found object without virtual table",0);
    DBG0_DrawE("obj=%p",self);
    if ( obj_isDrawingDisabled(obj) ) return;
    obj_BBox(obj);
    obj_doDrawData(obj);
    if ( gcd!=OD_GC_UsrDraw ) {
        if ( vtable->vt_draw!=0 )
            vtable->vt_draw(obj,sc->sc_device,gcd);
        if ( vtable->vt_draw_sc!=0 )
            vtable->vt_draw_sc(obj,sc,gcd,OD_GC_None);
        goto draw_arrow;
    }

    // gcd==VGUI_GC_UsrDraw --> means a real drawing,
    //                          not a preview, not a rubber, ...
#define OffsetToVoid(base,offset) ((void*)(((char*)base)+offset))

    Tod_GCdef gcf = OD_GC_UsrFill;

    // check for text
    if ( vtable->vt_fdt!=0 ) {
        Tsty_text* dpt = OffsetToVoid(obj.any,vtable->vt_fdt);
        DBG0_DrawM("obj=%p : text",self);
        od_str_setGC_byStyle(sc->sc_device,gcd,dpt);
        if ( vtable->vt_draw!=0 )
            vtable->vt_draw(obj,sc->sc_device,gcd);
        goto leave;
    }

    if ( vtable->vt_fdl!=0 && vtable->vt_fdf!=0 ) {
        DBG0_DrawM("obj=%p : line + fill",self);
        Tsty_line* dpl = OffsetToVoid(obj.any,vtable->vt_fdl);
        Tsty_fill* dpf = OffsetToVoid(obj.any,vtable->vt_fdf);
        if ( dpl->dpl_style!=DA_LS_None && dpf->dpf_style==DA_FS_NoFill ) {
            // draw only
            goto draw_with_no_fill;
        } else if ( dpl->dpl_style==DA_LS_None && dpf->dpf_style!=DA_FS_NoFill ) {
            // fill only
            od_ldaf_setGCll_byStyle(sc->sc_device,gcf,1.0,dpf);
            if ( vtable->vt_draw_sc!=0 )
                vtable->vt_draw_sc(obj,sc,OD_GC_None,gcf);
        } else if ( dpl->dpl_style!=DA_LS_None && dpf->dpf_style!=DA_FS_NoFill ) {
            // draw & fill 
            od_ldaf_setGCddZ_byStyle(sc->sc_device,gcd,dpl);
            od_ldaf_setGCll_byStyle(sc->sc_device,gcf,1.0,dpf);
            if ( vtable->vt_draw_sc!=0 )
                vtable->vt_draw_sc(obj,sc,gcd,gcf);
        }
    } else if ( vtable->vt_fdl!=0 ) {
draw_with_no_fill:
        DBG0_DrawM("obj=%p : line + no-fill",self);
        Tsty_line* dpl = OffsetToVoid(obj.any,vtable->vt_fdl);
        if ( dpl->dpl_style!=DA_LS_None ) {
            od_ldaf_setGCddZ_byStyle(sc->sc_device,gcd,dpl);
            //ydev_line_setGC(ydev,gcd,dpl);
            if ( vtable->vt_draw!=0 )
                vtable->vt_draw(obj,sc->sc_device,gcd);
            if ( vtable->vt_draw_sc!=0 )
                vtable->vt_draw_sc(obj,sc,gcd,OD_GC_None);
        }
    } else {
        DBG0_DrawM("obj=%p : directory or obj without any dp",self);
        if ( vtable->vt_draw!=0 )
            vtable->vt_draw(obj,sc->sc_device,gcd);
        if ( vtable->vt_draw_sc!=0 )
            vtable->vt_draw_sc(obj,sc,gcd,OD_GC_None);
        goto leave;
    }

draw_arrow:
    if ( (arrow=obj_arrow_BegPtr(obj))!=0 && ! OBJ_IsNone(*arrow) )
        obj_draw(sc,*arrow,gcd);
    if ( (arrow=obj_arrow_EndPtr(obj))!=0 && ! OBJ_IsNone(*arrow) )
        obj_draw(sc,*arrow,gcd);

leave:
    DBG0_DrawL("obj=%p",self);
}

/*======================================================================*/
/*= Miscellaneous scene services.                                      =*/

extern void        fig_scene_clrEGScache(Tfig_scene* sc)
{ fig_clrEGScache(sc->sc_dessin,sc->sc_device); }

/*======================================================================*/
