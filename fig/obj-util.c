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
 *      $Id: obj-util.c 367 2018-11-18 23:50:25Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-util.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous object utilities.                                    =*/
/*=                                                                    =*/
/*= See the obj-object.h and obj-util.h header for documentation.      =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/object-priv.h"
#include "fig/obj-any.h"
#include "fig/obj-compound.h"
#include "fig/obj-vtable.h"

#include "tools/lenunit.h"
#include "tools/geo2d.h"

/*======================================================================*/

TobjAll OBJ_none = { .any=0 };

/*======================================================================*/
/*= Object set                                                         =*/

#define TPLSET objAll
#define TPLSET_Func(f) objAllSet_##f
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

extern int  objAllSet_existObj (TobjAllSet* s, TobjAll obj)
{
    int i;
    if (s==0) return 0;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( s->eles[i].any==obj.any ) return 1;
    }
    return 0;
}

extern int  objAllSet_indexObj   (TobjAllSet* s, TobjAll obj)
{
    int i;
    if (s==0) return -1;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( s->eles[i].any==obj.any ) return i;
    }
    return -1;
}

extern void objAllSet_supObjQuick(TobjAllSet* s, TobjAll obj)
{ int idx; if ( (idx=objAllSet_indexObj(s,obj))>=0 ) objAllSet_supQuick(s,idx); }

extern void objAllSet_supObjShift(TobjAllSet* s, TobjAll obj)
{ int idx; if ( (idx=objAllSet_indexObj(s,obj))>=0 ) objAllSet_supShift(s,idx); }

/*======================================================================*/
/*= Utilities for implementing the vt_dump service.                    =*/

extern TstrBrief obj_dump_anyStr(TstrBrief buf,TobjAll obj)
{
    VARS_SeVt(obj);
    int len=0;
    len += sprintf(buf+len,"vt=%c%d--tag=%02x--cnt=%02d--parent=%p--next=%10p--fig=%p",
            self->OA_vtable==obj_vtable[FIG_KD_Compound] ? 'F' : 'X',
            vtable->vt_kind,self->OA_tags,self->OA_cnt,
            self->OA_parent, self->OA_next, obj_Figure(obj) );
    len += sprintf(buf+len,"--depth=%3d-%3d",
            self->OA_depthmin,self->OA_depthmax);
    len += sprintf(buf+len,"--BBpos=%f %f BBsz=%f %f",
            Box_x (self->OA_boundBox), Box_y (self->OA_boundBox),
            Box_dx(self->OA_boundBox), Box_dy(self->OA_boundBox)  );
    return buf;
}

extern void     obj_dump_any(TobjAll obj, int sh, CstrBrief fn)
{
    VARS_SeVt(obj);
    char atmp[500];
    char tmp[100]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh);
    printf("%20s:%s%8s=%p--%s",fn,tmp,
            vtable->vt_name,
            obj.any,obj_dump_anyStr(atmp,obj));
}

extern void obj_dump_dble2(Cdble2* s, int nb, int sh, CstrBrief fn)
{
    int i;
    char tmp[100]; memset(tmp,0,sizeof(tmp)); memset(tmp,' ',sh);

    for (i=0 ; i<nb ; i+=1) {
        if ( i!=0 &&  (i%5)==0 ) printf("\n");
        if ( (i%5)==0 ) printf("%20s:%s %2d/%2d ",fn, tmp,i,nb);
        printf("(%5.0f %5.0f) ", s[i].x, s[i].y);
    }
    printf("\n");
}

extern void obj_dump_dble2set(Cdble2Set* s, int sh, CstrBrief fn)
{ if (s) obj_dump_dble2(s->eles, s->eleNb, sh, fn); }

/*======================================================================*/
/*= Object selection.                                                  =*/

CobjAllSet* obj_getChildren (TobjAll dir)
{
    if ( obj_Kind(dir)!=FIG_KD_Compound ) return 0;
    return dir.dir->OCO_children;
}

extern TobjAllSet* obj_getChildrenB  (TobjAll dir, CboxDble* bb)
{
    int i;
    CobjAllSet* children = obj_getChildren(dir);
    TobjAllSet* set      = objAllSet_new(children ? children->eleNb : 0);
    if ( children!=0 ) {
        for (i=0 ; i<children->eleNb ; i+=1) {
            TobjAll o = children->eles[i];
            if ( tool_boxDble_overlap(bb,obj_BBox(o)) )
                objAllSet_add(set,o);
        }
    }
    return set;
}

#define SC_DepthIsActive(depths,d)  ((depths)[d]>0)

static void obj_getChildrenD_do (TobjAllSet*set, TobjAll dir, Cscdepth* depths)
{
    int i,skip,d;
    CobjAllSet* children = obj_getChildren(dir);
    if ( children==0 ) return;
    for (i=0 ; i<children->eleNb ; i+=1) {
        TobjAll o = children->eles[i];
        for ( skip=1,d=o.any->OA_depthmin ; d<o.any->OA_depthmax ; d+=1 )
            if ( SC_DepthIsActive(depths,d) ) { skip=0; break; }
        if ( skip ) continue;
        if ( obj_Kind(o)!=FIG_KD_Compound ) 
            objAllSet_add(set,o);
        else
            obj_getChildrenD_do(set,o,depths);
    }
}
extern TobjAllSet* obj_getChildrenD  (TobjAll dir, Cscdepth* depths)
{
    int i,skip;
    TobjAllSet* set = objAllSet_new(100);
    obj_getChildrenD_do(set,dir,depths);
    return set;
}

extern TobjAllSet* obj_getChildrenM  (TobjAll dir, int msk);
extern TobjAllSet* obj_getChildrenBD (TobjAll dir, CboxDble* bb, Cscdepth* depths)
{
    int i,d,skip;
    CobjAllSet* children = obj_getChildren(dir);
    TobjAllSet* set      = objAllSet_new(children ? children->eleNb : 0);
    if ( children!=0 ) {
        for (i=0 ; i<children->eleNb ; i+=1) {
            TobjAll o = children->eles[i];
            for ( skip=1,d=o.any->OA_depthmin ; d<o.any->OA_depthmax ; d+=1 )
                if ( SC_DepthIsActive(depths,d) ) { skip=0; break; }
            if ( skip ) continue;
            if ( ! tool_boxDble_overlap(bb,obj_BBox(o)) ) continue;
            objAllSet_add(set,o);
        }
    }
    return set;
}

extern TobjAllSet* obj_getChildrenBM (TobjAll dir, CboxDble* bb, int msk);
extern TobjAllSet* obj_getChildrenBDM(TobjAll dir, CboxDble* bb,
                                      Cscdepth* depths, int msk)
{
    int i,d,skip;
    TobjAllSet* set;
    CobjAllSet* children = obj_getChildren(dir);
    if ( children==0 )
        set = objAllSet_new(0);
    else {
        set = objAllSet_new(children->eleNb);
        for (i=0 ; i<children->eleNb ; i+=1) {
            TobjAll o = children->eles[i];
            if ( (obj_Mask(o)&msk)==0 ) continue;
            for ( skip=1,d=o.any->OA_depthmin ; d<o.any->OA_depthmax ; d+=1 )
                if ( SC_DepthIsActive(depths,d) ) { skip=0; break; }
            if ( skip ) continue;
            if ( ! tool_boxDble_overlap(bb,obj_BBox(o)) ) continue;
            objAllSet_add(set,o);
        }
    }
    return set;
}

static TobjCloseToPt obj_getObjCloseToPtFirst_PRIV(
                        CobjAllSet* objs, double d, Tdble2 pt, int nc)
{
    TobjCloseToPt ret;
    Tdble2Set* markers;
    int i,m,ctridx;
    Tdble2 center;
    for (i=0 ; i<objs->eleNb ; i+=1) {
        TobjAll o = objs->eles[i];
        // check if close to a marker
        markers = obj_markersAndCtr(o,&ctridx);
        for (m=0 ; markers && m<markers->eleNb ; m+=1) {
            if ( tool_closeToPointPTDbl(d,&pt,markers->eles+m)!=0 &&
                    ( nc==0 || ctridx!=m) ) {
                ret.obj  = o;
                ret.mkr  = markers->eles[m];
                ret.shpt = markers->eles[m];
                ret.idx  = m;
                dble2set_delete(markers);
                return ret;
            }
        }
        // if o has a vt_closeto service, we use it.
        CobjVtable* vtable = o.any->OA_vtable;
        if ( vtable->vt_closeto ) {
            ret.idx = vtable->vt_closeto(o,d,pt,&ret.shpt);
            if ( ret.idx>=0 && (nc==0 || ctridx!=ret.idx) ) {
                ret.obj  = o;
                ret.mkr  = markers->eles[ret.idx];
                return ret;
            }
        }
        dble2set_delete(markers);
    }
    ret.obj  = OBJ_none;
    ret.mkr  = dble2_init(0,0);
    ret.shpt = dble2_init(0,0);
    ret.idx  = -1;
    ret.ctr  = dble2_init(0,0);
    return ret;
}

extern TobjCloseToPt obj_getObjCloseToPtFirst  (CobjAllSet* objs, double d, Tdble2 pt) { return obj_getObjCloseToPtFirst_PRIV(objs,d,pt,0); }
extern TobjCloseToPt obj_getObjCloseToPtFirstNC(CobjAllSet* objs, double d, Tdble2 pt) { return obj_getObjCloseToPtFirst_PRIV(objs,d,pt,1); }

/*======================================================================*/
/*= Object center and markers.                                         =*/

extern Tdble2        obj_center(TobjAll obj)
{
    VARS_SeVt(obj);
    Tdble2 center;
    if ( vtable->vt_markerBB!=0 ) {
        CboxDble* bb = obj_BBox(obj);
        center = dble2_init( Box_x(*bb) + Box_dx(*bb)/2,
                             Box_y(*bb) + Box_dy(*bb)/2);
    } else if ( vtable->vt_centerMkr!=0 && vtable->vt_markerIF!=0 ) {
        center = vtable->vt_markerIF(obj,vtable->vt_centerMkrIdx);
    } else if ( vtable->vt_centerMkr!=0 ) {
        Tdble2Set* markers = obj_markers(obj);
        center = markers->eles[vtable->vt_centerMkrIdx];
        dble2set_delete( markers );
    } else if ( vtable->vt_centerF!=0 ) {
        center = vtable->vt_centerF(obj);
    } else {
        Tdble2Set* markers = obj_markers(obj);
        center = dble2set_barycentre(markers);
        dble2set_delete( markers );
    }
    return center;
}

extern int obj_markerNb(TobjAll obj)
{
    VARS_SeVt(obj);
    if ( vtable->vt_markerBB ) return 4;
    return vtable->vt_markerNb!=0 ? vtable->vt_markerNb : vtable->vt_markerNbF(obj);
}

extern Tdble2Set*    obj_markers(TobjAll obj)
{
#define PTROA(oa,offset) ((void*)(offset==0 ? ((char*)0) : ((char*)(oa).any)+(offset)))
    VARS_SeVt(obj);
    Tdble2Set* markers;
    if ( vtable->vt_markerBB ) {
        CboxDble* bb = obj_BBox(obj);
//printf("mkr:1 BB=%.2f/%.2f %.2f/%.2f\n",BoxArg_XYWH(*bb));
        markers = dble2set_new(4);
        dble2set_addXY(markers,Box_x(*bb)            ,Box_y(*bb));
        dble2set_addXY(markers,Box_x(*bb)+Box_dx(*bb),Box_y(*bb));
        dble2set_addXY(markers,Box_x(*bb)+Box_dx(*bb),Box_y(*bb)+Box_dy(*bb));
        dble2set_addXY(markers,Box_x(*bb)            ,Box_y(*bb)+Box_dy(*bb));
        return markers;
    }
    int nb = obj_markerNb(obj);
    if (  vtable->vt_markerT ) {
        return dble2set_newTable(PTROA(obj,vtable->vt_markerT),nb);
    } else if (  vtable->vt_markerS ) {
        markers = *(Tdble2Set**)PTROA(obj,vtable->vt_markerS);
        return dble2set_newTable(markers->eles,nb);
    } else if ( vtable->vt_markerTF )  {
        return dble2set_newTable(vtable->vt_markerTF(obj),nb);
    } else if (  vtable->vt_markerSF ) {
        Cdble2Set* set = vtable->vt_markerSF(obj);
        return dble2set_newTable(set->eles,nb);
    } else if ( vtable->vt_markerIF ) {
        int i;
        markers = dble2set_new(nb);
        for (i=0 ; i<nb ; i+=1) 
            dble2set_add(markers,vtable->vt_markerIF(obj,i));
        return markers;
    } else if ( vtable->vt_markerFF ) {
        markers = dble2set_new(nb);
        markers->eleNb = nb;
        vtable->vt_markerFF(obj,markers->eles);
        return markers;
    } else {
        PERM_spf_printf("missing markers feature in kd=%d=%s",
                vtable->vt_kind,vtable->vt_name); 
        return 0;
    }
}

extern Tdble2Set* obj_markersUU    (TobjAll obj, int uu)
{
    int i;
    Tdble2Set* markers = obj_markers(obj);
    double     ppi     = obj_Ppi(obj);

    for ( i=0 ; i<markers->eleNb ; i++) {
        markers->eles[i].x = lu_convToFuPpi( markers->eles[i].x, ppi);
        markers->eles[i].x = lu_convFromFu ( markers->eles[i].x, uu);
    }

    return markers;
}

extern Tdble2Set*    obj_markersAndCtr(TobjAll obj, int* ctrdx)
{
    VARS_SeVt(obj);
    Tdble2Set* markers = obj_markers(obj);
    *ctrdx = vtable->vt_centerMkr ?  vtable->vt_centerMkrIdx : -1;
    return markers;
}

extern int        obj_markerOpe    (TobjAll obj)
{
    VARS_SeVt(obj);
    if ( vtable->vt_mkrOpeEnabled!=0 )
        return vtable->vt_mkrOpeEnabled(obj);
    int ret = 0;
    if ( vtable->vt_mkrOpeInsert!=0 ) ret |= FIGO_MkrOpe_Ins;
    if ( vtable->vt_mkrOpeChange!=0 ) ret |= FIGO_MkrOpe_Chg;
    if ( vtable->vt_mkrOpeDelete!=0 ) ret |= FIGO_MkrOpe_Del;
    return ret;
}

extern Tdble2      obj_markerIns    (TobjAll obj, int idx, Cdble2*pos)
{
    VARS_SeVt(obj);
    TEMP_asf_printf( (obj_markerOpe(obj)&FIGO_MkrOpe_Ins)==0,
            "operation not allowed for %d object kind",obj_Kind(obj));
    TEMP_asf_printf( idx<0 || obj_markerNb(obj)<idx,
            "%d idx not in range [0:%d]",idx,obj_markerNb(obj));
    return vtable->vt_mkrOpeInsert(obj, idx, pos);
}

extern void       obj_markerChg    (TobjAll obj, int idx, Tdble2 pt)
{
    VARS_SeVt(obj);
    TEMP_asf_printf( (obj_markerOpe(obj)&FIGO_MkrOpe_Chg)==0,
            "operation not allowed for %d object kind",obj_Kind(obj));
    TEMP_asf_printf( idx<0 || obj_markerNb(obj)<=idx,
            "%d idx not in range [0:%d[",idx,obj_markerNb(obj));
    vtable->vt_mkrOpeChange(obj, idx,pt);
}

extern void       obj_markerChgUU  (TobjAll obj, int idx, Tdble2 pt, int uu);

extern void       obj_markerDel    (TobjAll obj, int idx)
{
    VARS_SeVt(obj);
    TEMP_asf_printf( (obj_markerOpe(obj)&FIGO_MkrOpe_Del)==0,
            "operation not allowed for %d object kind",obj_Kind(obj));
    TEMP_asf_printf( idx<0 || obj_markerNb(obj)<=idx,
            "%d idx not in range [0:%d[",idx,obj_markerNb(obj));
    vtable->vt_mkrOpeDelete(obj, idx);
}

/*======================================================================*/
/*= Drawing parameters.                                                =*/

// Only required by figdp_getFdpRaw to get the text direction
// FIXME: must be changed
#include "fig/obj-text.h"

extern TstyleMask figdp_getFdpRaw(Tfdp*fdp, CobjAll obj)
{
    Csty_line* stl; Csty_fill* stf; Csty_text* stt;
    VARS_Self(obj);
    TstyleMask m=0;

    m |= FDP_G_depth;
    fdp->fdp_depth = self->OA_depthmin;
    if ( obj_Kind(obj)==FIG_KD_Text) {
        // direction (text only)
        m |= FDP_G_dir;
        fdp->fdp_dir = obj.text->OTE_type;
    }

    // text
    if ( (stt=obj_StyText(obj))!=0 ) {
        m |= FDP_T_all;
        fdp->fdp_text = *stt;
    }

    // line or curve
    if ( (stl=obj_StyCurve(obj))!=0 ) {
        m |= FDP_C_all;
        fdp->fdp_line = *stl;
    } else if ( (stl=obj_StyLine(obj))!=0 ) {
        m |= FDP_L_all;
        fdp->fdp_line = *stl;
    }

    // fill
    if ( (stf=obj_StyFill(obj))!=0 ) {
        m |= FDP_F_all;
        fdp->fdp_fill = *stf;
    }
    return m;
}

extern void       figdp_free   (Tfdp* fdp)
{ odsty_free( &fdp->fdp_style ); }

extern TstyleMask figdp_cmpMask(TstyleMask m, Cfdp*l,  Cfdp*r)
{
    if ( (m&FDP_G_depth)!=0 && l->fdp_depth==r->fdp_depth ) m&=~FDP_G_depth;
    if ( (m&FDP_G_dir  )!=0 && l->fdp_dir  ==r->fdp_dir   ) m&=~FDP_G_dir;
    return odsty_cmpMask(m,&l->fdp_style,&r->fdp_style);
}

extern Tfdp       figdp_merge1(TstyleMask mall, Cfdp*all,
                               TstyleMask mset, Cfdp*set )
{
#define FromSet(a,s,f) (f&a&s)!=0
    Tfdp res;

    res.fdp_vtt = all->fdp_vtt;
    res.fdp_depth = FromSet(mall,mset,FDP_G_depth) ? set->fdp_depth : all->fdp_depth;
    res.fdp_dir   = FromSet(mall,mset,FDP_G_dir  ) ? set->fdp_dir   : all->fdp_dir  ;
    res.fdp_style = odsty_merge1(mall,&all->fdp_style,mset,&set->fdp_style);
    return res;
}

extern Tfdp       figdp_merge2(TstyleMask mall, Cfdp*all,
                               TstyleMask mset, Cfdp*set )
{ return figdp_merge1(~mall,all,mset,set); }

/*======================================================================*/
