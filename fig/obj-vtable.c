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
 *      $Id: obj-vtable.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-vtable.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Handle the call of virtual object functions.                       =*/
/*= See the fig/obj-object.h header to get the high level object       =*/
/*= services.                                                          =*/
/*= See the fig/obj-vtable.h header to get the basic operation that    =*/
/*= an object must support to be handled by the figure module.         =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/object-priv.h"
#include "fig/obj-vtable.h"
#include "fig/obj-any.h"
#include "fig/obj-compound.h"
#include "fig/figure.h"

#include "outdev/style.h"
#include "tools/math-macros.h"

/*======================================================================*/
#define DEF_STD_VARS(o) \
    TobjAny*    self   = (TobjAny*)o; \
    CobjVtable* vtable =  self->oa_vtable; \
    TEMP_asf_printf( vtable==0, "self.vtable is null",0)

#define DEF_STD_VARS_OA(o) \
    TobjAny*    self   = o.any; \
    CobjVtable* vtable =  self->oa_vtable; \
    TEMP_asf_printf( vtable==0, "self.vtable is null",0)

/*======================================================================*/
/*= object creation                                                    =*/

extern TobjVtable arc_vtable;
extern TobjVtable ellipse_vtable;
extern TobjVtable line_vtable;
extern TobjVtable spline_vtable;
extern TobjVtable text_vtable;
extern TobjVtable compound_vtable;
extern TobjVtable boxhv_vtable;
extern TobjVtable arrowT00_vtable;
extern TobjVtable arrowT01_vtable;
extern TobjVtable arrowT02_vtable;
//extern TobjVtable arrowT03_vtable, arrowT04_vtable;

void* obj_vtable[] = {
    0,
    &ellipse_vtable,  // FIG_KD_Ellipse=1
    &line_vtable,     // FIG_KD_Line=2
    &spline_vtable,   // FIG_KD_Spline=3
    &text_vtable,     // FIG_KD_Text=4
    // 5
    &arc_vtable,      // FIG_KD_Arc=5
    &compound_vtable, // FIG_KD_Compound=6
    &boxhv_vtable,    // FIG_KD_Boxhv=7
    &arrowT00_vtable,
    &arrowT01_vtable,
    &arrowT02_vtable,
    //&arrowT03_vtable, &arrowT04_vtable,
    0
};

extern TobjAll obj_newDepth(void*vt, int depth)
{
    TobjAll self = obj_new(vt);
    if ( depth>=0 ) {
        self.any->OA_depthmin  = depth;
        self.any->OA_depthmax  = depth+1;
    }
    return self;
}

extern TobjAll obj_newByOid(int oid, TobjVTT vtt)
{
    if (vtt==0) vtt = (TobjVTT)obj_vtable;
    void* vt = (void*)vtt[oid];
    return obj_new(vt);
}

extern TobjAll   obj_newByOidDepth(int oid, TobjVTT vtt, int depth)
{
    if (vtt==0) vtt = (TobjVTT)obj_vtable;
    void* vt = vtt[oid];
    return obj_newDepth(vt,depth);
}

extern TobjAll  obj_newByODS(int oid, TobjVTT vtt, int depth, 
                             Csty_line*stl, Csty_fill*stf, Csty_text*stt)
{
    TobjAll self; Tsty_line*sstl; Tsty_fill*sstf; Tsty_text*sstt;

    self =  obj_newByOidDepth(oid,vtt,depth);
    if ( (sstl=obj_StyLine(self)) != 0 ) *sstl = odsty_copyLine(stl);
    if ( (sstf=obj_StyFill(self)) != 0 ) *sstf = odsty_copyFill(stf);
    if ( (sstt=obj_StyText(self)) != 0 ) *sstt = odsty_copyText(stt);

    return self;
}

extern TobjAll  obj_newByFdp(int oid, Cfdp* fdp)
{ return  obj_newByODS(oid,fdp->fdp_vtt,fdp->fdp_depth,
            &fdp->fdp_line, &fdp->fdp_fill, &fdp->fdp_text); }

/*======================================================================*/
/*= Object getters                                                     =*/

#define PTROA(oa,offset) ((void*)(offset==0 ? ((char*)0) : ((char*)(oa).any)+(offset)))
#define INTOA(oa,offset) (        offset==0 ? ((int  )0) :*((int*)(((char*)(oa).any)+(offset))))

extern int         obj_kind       (Cobject obj) { DEF_STD_VARS   (obj); return vtable->vt_kind; }
extern int         obj_Kind       (CobjAll obj) { VARS_SeVt(obj); return vtable->vt_kind; }
extern int         obj_SubKind    (CobjAll obj) { VARS_SeVt(obj); return INTOA(obj,vtable->vt_subKind); }
extern CstrBrief   obj_Name       (CobjAll obj) { VARS_SeVt(obj); return vtable->vt_nameF ? vtable->vt_nameF(obj) : vtable->vt_name; }
extern int         obj_Mask       (CobjAll obj) { VARS_SeVt(obj); return vtable->vt_mask | (
                                                  obj_arrow_IsSupported(obj) ? FIG_KDM_AOW : 0); }
extern double      obj_Ppi        (CobjAll obj) { VARS_Self(obj); if (self->OA_ppi!=0) return self->OA_ppi;
                                                  Tfigure* fig=obj_Figure(obj); return fig ? fig->fig_ppi : LUNIT_FuDefault; }
extern Tfigure*    obj_Figure     (CobjAll obj) { VARS_Self(obj); return self!=0 ? self->OA_figure : NULL; }
extern Tfig_colors*obj_ClrFig     (CobjAll obj) { Tfigure* fig=obj_Figure(obj); return fig ? &fig->colors : (Tfig_colors*)0; } 
extern Ccolor*     obj_FigClrBlack(CobjAll obj) { Tfig_colors* clrs=obj_ClrFig(obj); return clrs ? clrs->predefinedColors->eles[0] : (Ccolor*)0; }
extern Ccolor*     obj_FigClrWhite(CobjAll obj) { Tfig_colors* clrs=obj_ClrFig(obj); return clrs ? clrs->predefinedColors->eles[7] : (Ccolor*)0; }
extern Tsty_text*  obj_StyText    (CobjAll obj) { VARS_SeVt(obj); return PTROA(obj,vtable->vt_fdt); }
extern Tsty_line*  obj_StyCurve   (CobjAll obj) { VARS_SeVt(obj); return PTROA(obj,vtable->vt_fdc); }
extern Tsty_line*  obj_StyLine    (CobjAll obj) { VARS_SeVt(obj); Tsty_line* ret = PTROA(obj,vtable->vt_fdl);
                                                  if (ret==0) ret=obj_StyCurve(obj); return ret; }
extern Tsty_fill*  obj_StyFill    (CobjAll obj) { VARS_SeVt(obj); return PTROA(obj,vtable->vt_fdf); }
//extern int         obj_isAttached       (CobjAll obj) { VARS_Self(obj); return (self->OA_tags&FIG_TAG_ATTACHED)!=0 ; }
extern int         obj_isDrawDataReady  (CobjAll obj) { VARS_Self(obj); return (self->OA_tags&FIG_TAG_DrawingDataReady)!=0 ; }
extern int         obj_isDrawingDisabled(CobjAll obj) { VARS_Self(obj); return (self->OA_tags&FIG_TAG_DrawingDisabled)!=0 ; }

extern int         obj_arrow_IsSupported(CobjAll obj) { return OBJ_IsNone(obj) ? 0 : (obj.any->OA_tags&FIG_TAG_CanHaveArrow)!=0 ; }
extern TobjArrow*  obj_arrow_Beg        (CobjAll obj) { return OBJ_IsNone(obj) ? 0 : obj_arrow_BegPtr(obj)==0 ? 0 : obj_arrow_BegPtr(obj)->arrow; }
extern TobjArrow*  obj_arrow_End        (CobjAll obj) { return OBJ_IsNone(obj) ? 0 : obj_arrow_EndPtr(obj)==0 ? 0 : obj_arrow_EndPtr(obj)->arrow; }
extern TobjAll*    obj_arrow_BegPtr     (CobjAll obj) { VARS_SeVt(obj); return PTROA(obj,vtable->vt_begArrow); }
extern TobjAll*    obj_arrow_EndPtr     (CobjAll obj) { VARS_SeVt(obj); return PTROA(obj,vtable->vt_endArrow); }

/*======================================================================*/
/*= kind, dump and figure                                              =*/

extern void     obj_dump(TobjAll obj, int sh)
{
    VARS_SeVt(obj);
    if ( vtable->vt_dump==0 ) {
        obj_dump_any(obj,sh,__func__);
        printf("\n");
    } else
        vtable->vt_dump(obj,sh);
}

extern TobjAll obj_newChangingDP(CobjAll obj, Cfdp*att, Toutdev*od)
{
    TobjAll clone;
    VARS_SeVt(obj);
    TEMP_asf_printf( vtable->vt_dupNDP==0,
            "no dupNDP virtual funnction in %d kind object (%s)",
            vtable->vt_kind, vtable->vt_name);
    clone = vtable->vt_dupNDP(obj,att,od);

    if ( obj_Kind(obj)==FIG_KD_Text ) 
        return clone;

    clone.any->OA_tags &= ~(FIG_TAG_DrawingDataReady|FIG_TAG_BBoxUpToDate);

    // update arrows if needed
    TobjAll* arrow;
    CobjVtable*avtable;
    int abslen; double len,angle,thickness; Csty_fill*fill;
    if ( (arrow=obj_arrow_BegPtr(obj))!=0 && OBJ_IsDefined(*arrow) ) {
        obj_arrow_getParams(*arrow,&avtable,&abslen,&fill,&len,&angle,&thickness);
        if ( abslen==0 )
            obj_arrow_chgRel(clone,1,(void*)avtable,fill,len,angle);
        else
            obj_arrow_chgAbs(clone,1,(void*)avtable,fill,len,angle,thickness);
    }
    if ( (arrow=obj_arrow_EndPtr(obj))!=0 && OBJ_IsDefined(*arrow) ) {
        obj_arrow_getParams(*arrow,&avtable,&abslen,&fill,&len,&angle,&thickness);
        if ( abslen==0 )
            obj_arrow_chgRel(clone,0,(void*)avtable,fill,len,angle);
        else
            obj_arrow_chgAbs(clone,0,(void*)avtable,fill,len,angle,thickness);
    }

    return clone;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Depth                                                              =*/

extern int       obj_usedDepths(CobjDir dir, int*t)
{
    int it,id;
    Tscdepth depths[FIG_MAX_DEPTH];
    int      depthn = obj_allDepths(dir,depths);
    for (id=0,it=0; id<depthn ; id++) {
        if ( depths[id]!=0 ) t[it++]=id;
    }
    return it;
}

extern void      obj_allDepthsAdd(CobjDir dir, Tscdepth*t)
{
    int i,j;
    CobjAllSet* set = obj_getChildren( obj_ObjAll(dir) );
    for ( i=0 ; set!=0 && i<set->eleNb ; i+=1) {
        TobjAll obj = set->eles[i];
        if ( obj_Kind(obj)==FIG_KD_Compound )
            obj_allDepthsAdd(obj.dir,t);
        else {
            for (j=obj.any->OA_depthmin ; j<obj.any->OA_depthmax ; j+=1 )
                t[j] +=1;
        }
    }
}

extern int       obj_allDepths (CobjDir dir, Tscdepth*t)
{
    int nb = FIG_MAX_DEPTH;
    memset(t,0,sizeof(t[0])*nb);
    obj_allDepthsAdd(dir,t);

    return nb;
}

/*======================================================================*/
/*= change the virtual table of object hierarchy.                      =*/

extern void  obj_vtable_chg(Tobject obj, void*vt[FIG_KD_LowerFreeId])
{
    if ( obj==0 ) return;

    DEF_STD_VARS(obj);
    if (  vtable->vt_vtable_chg!=0 ) {
         vtable->vt_vtable_chg(obj,vt);
    }
    if ( vt[vtable->vt_kind]!=0 )
        self->OA_vtable = vt[vtable->vt_kind];
}

/*======================================================================*/
/*= Bounding box                                                       =*/

extern CboxDble* obj_BBox(CobjAll obj)
{
    CboxDble *babox,  *eabox; // BB of arrows
    TobjAll  *barrow, *earrow;
    VARS_SeVt(obj);

    if ( (self->OA_tags&FIG_TAG_BBoxUpToDate)==0 ) {
        TEMP_asf_printf( vtable->vt_boundBox==0 ,"%s","vt_boundBox is null");
        TobjAny* wself = (TobjAny*)self;
        wself->OA_boundBox = vtable->vt_boundBox(obj);
        barrow=obj_arrow_BegPtr( obj );
        earrow=obj_arrow_EndPtr( obj );
        if ( (barrow!=0 && OBJ_IsDefined(*barrow)) ||
             (earrow!=0 && OBJ_IsDefined(*earrow)) ) {
             if ( (self->OA_tags&FIG_TAG_DrawingDataReady)==0 )
                obj_doDrawData( obj );
        }
#if 0
        babox = barrow==0 || OBJ_IsNone(*barrow) ? 0 : obj_BBox( *barrow );
        eabox = earrow==0 || OBJ_IsNone(*earrow) ? 0 : obj_BBox( *earrow );
#else
       if ( barrow==0 || OBJ_IsNone(*barrow) )
            babox = 0;
       else {
            obj_doDrawData( *barrow );
            babox = obj_BBox( *barrow );
       }
       if ( earrow==0 || OBJ_IsNone(*earrow) )
            eabox = 0;
       else {
            obj_doDrawData( *earrow );
            eabox = obj_BBox( *earrow );
       }
#endif
        if ( babox!=0 ) tool_boxDble_union(&wself->OA_boundBox,babox);
        if ( eabox!=0 ) tool_boxDble_union(&wself->OA_boundBox,eabox);
        wself->OA_tags  |= FIG_TAG_BBoxUpToDate;
    }
    return &self->OA_boundBox;
}

extern void obj_doDrawData(CobjAll obj)
{
    DEF_STD_VARS_OA(obj);
    if ( (self->OA_tags&FIG_TAG_DrawingDataReady)!=0 )
        return;
    if ( vtable->vt_doDrawData!=0)
        vtable->vt_doDrawData(obj);
    self->OA_tags |= FIG_TAG_DrawingDataReady;
}

extern void obj_clrEGScache(CobjAll obj, void* od)
{
    DEF_STD_VARS_OA(obj);
    if ( vtable->vt_clrEGScache!=0 ) {
        vtable->vt_clrEGScache(obj,od);
    }
}

/*======================================================================*/
/*= Geometric transformation                                           =*/

extern void obj_translation(TobjAll obj, double tx, double ty)
{
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_translation==0,"%s","vtable->vt_translation is null");
    vtable->vt_translation(obj,tx,ty);
    TRANSLATE_Box(self->OA_boundBox ,tx,ty);
    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_homothety00(TobjAll obj, int wtcf, double f)
{
    Tsty_text* stt;
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_homothety00==0 && vtable->vt_homothetyCC==0 &&
                     vtable->vt_scale00==0     && vtable->vt_scaleCC==0,
               "%s","vtable->vt_homothety00/CC & vt_scale/scale00/CC are both null");
    SCALE00_Box(self->OA_boundBox ,f,f);
    if      ( vtable->vt_homothety00 ) vtable->vt_homothety00(obj,wtcf,f);
    else if ( vtable->vt_homothetyCC ) vtable->vt_homothetyCC(obj,wtcf,f,0,0);
    else if ( vtable->vt_scale00     ) vtable->vt_scale00    (obj,wtcf,f,f);
    else                               vtable->vt_scaleCC    (obj,wtcf,f,f,0,0);
    if ( (wtcf&FIGO_SA_Thickness)!=0 ) {
        Tsty_line* stl = obj_StyLine(obj);
        if ( stl ) stl->stl_thickness *= abs(f);
    }
    if ( (wtcf&FIGO_SA_Arrow)!=0 ) {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_homothety00(obj_ObjAll(arrow),wtcf,f);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_homothety00(obj_ObjAll(arrow),wtcf,f);
    }

    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_homothetyCC(TobjAll obj, int wtcf, double f, double cx, double cy)
{
    Tsty_text* stt;
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_homothetyCC==0 && vtable->vt_scaleCC==0,
               "%s","vtable->vt_homothety/vt_scale are both null");
    SCALECC_Box(self->OA_boundBox ,f,f,cx,cy);
    if      ( vtable->vt_homothetyCC ) vtable->vt_homothetyCC(obj,wtcf,f,cx,cy);
    else                               vtable->vt_scaleCC    (obj,wtcf,f,f,cy,cy);
    if ( (wtcf&FIGO_SA_Thickness)!=0 ) {
        Tsty_line* stl = obj_StyLine(obj);
        if ( stl ) stl->stl_thickness *= abs(f);
    }
    if ( (wtcf&FIGO_SA_Arrow)!=0 ) {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_homothetyCC(obj_ObjAll(arrow),wtcf,f,cx,cy);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_homothetyCC(obj_ObjAll(arrow),wtcf,f,cx,cy);
    }
    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_scale00  (TobjAll obj, int wtcf, double fx, double fy)
{
    Tsty_text* stt;
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_scale00==0 && vtable->vt_scaleCC==0,
               "%s","vtable->vt_scale00/scaleCC are both null");
    SCALE00_Box(self->OA_boundBox ,fx,fy);
    if      ( vtable->vt_scale00   ) vtable->vt_scale00  (obj,wtcf,fx,fy);
    else                             vtable->vt_scaleCC  (obj,wtcf,fx,fy,0,0);
    if ( (wtcf&FIGO_SA_Thickness)!=0 && dble_close(fx,fy) ) {
        Tsty_line* stl = obj_StyLine(obj);
        if ( stl ) stl->stl_thickness *= min2(abs(fx),abs(fy));
    }
    if ( (wtcf&FIGO_SA_Arrow)!=0 ) {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_scale00(obj_ObjAll(arrow),wtcf,fx,fy);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_scale00(obj_ObjAll(arrow),wtcf,fx,fy);
    }
    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_scaleCC(TobjAll obj, int wtcf, double fx, double fy, double cx, double cy)
{
    Tsty_text* stt;
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_scaleCC==0,"%s","vtable->vt_scale is null");
    SCALECC_Box(self->OA_boundBox ,fx,fy,cx,cy);
    vtable->vt_scaleCC(obj,wtcf,fx,fy,cx,cy);
    if ( (wtcf&FIGO_SA_Thickness)!=0 && dble_close(fx,fy) ) {
        Tsty_line* stl = obj_StyLine(obj);
        if ( stl ) stl->stl_thickness *= min2(abs(fx),abs(fy));
    }
    if ( (wtcf&FIGO_SA_Arrow)!=0 ) {
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_scaleCC(obj_ObjAll(arrow),wtcf,fx,fy,cx,cy);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_scaleCC(obj_ObjAll(arrow),wtcf,fx,fy,cx,cy);
    }
    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_rotate     (TobjAll obj, double a, double cx, double cy)
{
    DEF_STD_VARS_OA(obj);
    if ( vtable->vt_rotate ) {
        self->OA_tags &= ~(FIG_TAG_BBoxUpToDate);
        self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
        vtable->vt_rotate(obj,a,cx,cy);
    } else {
        Tdble2 oldcenter = obj_center(obj);
        Tdble2 newcenter = oldcenter;
        ROTCC_PAXY(newcenter,a,cx,cy);
        obj_translation(obj,newcenter.x-oldcenter.x,newcenter.y-oldcenter.y);
    }
}

extern void obj_flip       (TobjAll obj, int h, int v, int r,
                            double cx, double cy)
{
    DEF_STD_VARS_OA(obj);
    TEMP_asf_printf( vtable->vt_flip==0,"%s","vtable->vt_flip is null");
    if ( (self->OA_tags&FIG_TAG_BBoxUpToDate)!=0 ) {
        Tdble2 pt1 = tool_boxdble_pt1(&self->OA_boundBox);
        Tdble2 pt3 = tool_boxdble_pt3(&self->OA_boundBox);
        FLIP_PHVRXY(pt1,h,v,r,cx,cy);
        FLIP_PHVRXY(pt3,h,v,r,cx,cy);
        tool_boxDble_initPT(&self->OA_boundBox,pt1,pt3);
    }
    vtable->vt_flip(obj,h,v,r,cx,cy);
    self->OA_tags &= ~(FIG_TAG_DrawingDataReady);
}

extern void obj_setPpi     (CobjAll obj, double ppi)
{
    VARS_SeVt(obj);
    int i,objKind = vtable->vt_kind;
    self->OA_ppi = ppi;
    if ( objKind==FIG_KD_Compound ) {
        TobjAllSet* children = obj.dir->OCO_children;
        for (i=0 ; children && i<children->eleNb ; i+=1)
            obj_setPpi(children->eles[i],ppi);
        // no arrow, no drawing styles in directories
    } else {
        // change arrow unit if any
        TobjArrow* arrow;
        if ( (arrow=obj_arrow_Beg(obj))!=0 ) obj_setPpi(obj_ObjAll(arrow),ppi);
        if ( (arrow=obj_arrow_End(obj))!=0 ) obj_setPpi(obj_ObjAll(arrow),ppi);
    }
}

extern void obj_align0( CobjAllSet* s, TalignOpe f, CboxDble b)
{
    int i;
    Tdble2 bctr = dble2_init( Box_x(b) + Box_dx(b)/2, Box_y(b) + Box_dy(b)/2);
    double tx,ty;
    for (i=0 ; i<s->eleNb ; i++) {
        TobjAll o = s->eles[i];
        tx=ty=0;
        Tdble2 octr; int octr_valid=0; // o center
        CboxDble* obox = obj_BBox(o);
        // vertical alignment
        if ( (f&ALIGN_XSS)!=0 )
            tx = Box_x(b) - Box_x(*obox);
        else if ( (f&ALIGN_XCC)!=0 ) {
            octr = obj_center(o); octr_valid=1;
            tx = bctr.x - octr.x;
        } else if ( (f&ALIGN_XGG)!=0 )
            tx = Box_x(b)+Box_dx(b) - (Box_x(*obox)+Box_dx(*obox));

        // horizontal alignment
        if ( (f&ALIGN_YSS)!=0 )
            ty = Box_y(b) - Box_y(*obox);
        else if ( (f&ALIGN_YCC)!=0 ) {
            if ( octr_valid==0 ) octr = obj_center(o);
            ty = bctr.y - octr.y;
        } else if ( (f&ALIGN_YGG)!=0 )
            ty = Box_y(b)+Box_dy(b) - (Box_y(*obox)+Box_dy(*obox));

        // translate if needed
        if ( tx!=0 || ty!=0 )
            obj_translation(o,tx,ty);
    }
}

extern void obj_alignD( TobjDir  self, TalignOpe f)
{ TobjAll oa = obj_ObjAll(self); obj_align0(obj_getChildren(oa),f,*obj_BBox(oa) ); }

/*======================================================================*/
// vim: set textwidth=120:
/*======================================================================*/
