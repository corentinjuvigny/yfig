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
 *      $Id: ydw-objedit-line.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-line.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top dialog for editing line and spline objects.                    =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"
#include "vgui/vdw-viewer.h"

#include "fig/obj-line.h"
#include "fig/obj-spline.h"

#include "tools/lenunit.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Toed_line Toed_line;
typedef const   Toed_line Coed_line;

struct _Toed_line { YOED_FIELDS(); Tvdw_viewpts* pts; };

Inline Tyoed_any* ANY(Toed_line* self) { return (Tyoed_any*)self; }

static Twid_anyVtable widany_vt_line;

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oed_line_cb_pts  (Tvgui_dw*vdw, void* ud, ...);
Inline int  oed_line_isRecPts(Coed_line* self)
{
    if ( self->dw_kind!=FIG_KD_Line      ) return 0;
    if ( self->dw_type==FIGOLI_T_MLine   ) return 0;
    if ( self->dw_type==FIGOLI_T_Polygon ) return 0;
    return 1;
}

/*======================================================================*/
/*= Interface                                                          =*/

extern Tyoed_any* yoed_line_nbf(Tyfig_mw* ymw, TobjAll obj)
{
    Toed_line* self;
    int        kind = obj_Kind(obj);

    TEMP_asf_printf( kind!=FIG_KD_Line && kind!=FIG_KD_Spline,"%d is an unexpected object kind",kind);

    self = (Toed_line*) yoed_any_new(&widany_vt_line,ymw,obj);

    Tdble2Set* points = obj_markersUU(obj,LUNIT_ID_FU);
    self->pts  = vdw_viewpts_nbf(self->objparams,"Points of object", points, 
            oed_line_cb_pts,oed_line_cb_pts,oed_line_cb_pts,self,
            VDWVIEW_OPT_UNIT(self->dw_uu));

#define PtsEGS  vdw_viewpts_widEGS(self->pts)
    vgui_dw_finishMTX(self->objparams
        ,VDWLF_ROW(0) ,MTX_IOPT_VXYx(0,self->center)
        ,VDWLF_ROW(1) ,0,PtsEGS  ,1,PtsEGS  ,2,PtsEGS  ,3,PtsEGS
        ,-1);
    yoed_any_dbf((Tyoed_any*)self);
    return (Tyoed_any*)self;
}

/*======================================================================*/
/* Action                                                              =*/

/*======================================================================*/
/* Callbacks                                                           =*/

static void oed_line_cb_pts_rec(Tdble2Set* pts, Tdble2 pt, int idx_fw, int idx_bw)
{
    if ( pt.x == pts->eles[idx_fw].x ) {
        // order 2 & pt.y changed
        pts->eles[idx_bw].y = pt.y;
    } else if ( pt.y == pts->eles[idx_bw].y ) {
        // order 2 & pt.x changed
        pts->eles[idx_fw].x = pt.x;
    } else if ( pt.y == pts->eles[idx_fw].y ) {
        // order 1 & pt.x changed
        pts->eles[idx_bw].x = pt.x;
    } else if ( pt.x == pts->eles[idx_bw].x ) {
        // order 1 & pt.y changed
        pts->eles[idx_fw].y = pt.y;
    }
}

void oed_line_cb_pts(Tvgui_dw*vdw, void* ud, ...)
{
    Toed_line* self = ud;
    Tdble2Set* pts  = vdw_viewpts_pts   (self->pts);
    int        idx  = vdw_viewpts_chgIdx(self->pts);

    if ( oed_line_isRecPts(self) ) {
        //      order 1  order 2
        // pt0:  x0 y0    x0 y0
        // pt1:  x1 y0    x0 y1
        // pt2:  x1 y1    x1 y1
        // pt3:  x0 y1    x1 y0
        Tdble2 pt  = pts->eles[idx];
        switch (idx) {
            case 0: oed_line_cb_pts_rec(pts,pt,1,3); break;
            case 1: oed_line_cb_pts_rec(pts,pt,2,0); break;
            case 2: oed_line_cb_pts_rec(pts,pt,3,1); break;
            case 3: oed_line_cb_pts_rec(pts,pt,0,2); break;
        }
        yoed_any_setNewObjObjChg( ANY(self) );
    } else {
        yoed_any_setNewObj( ANY(self) );
    }
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void oedline_vt_objChg(void*oed)
{
    int i; Tdble2 pt;
    Toed_line* self   = (void*)oed;
    TobjAll    obj    = yoed_any_currObj( ANY(self) );
    Tdble2Set* points = obj_markersUU(obj,LUNIT_ID_FU);
    TEMP_asf_printf(points->eleNb!=vdw_viewpts_pts(self->pts)->eleNb,
            "number of points in obj=%d and in viewer=%d must be equal",
            points->eleNb,vdw_viewpts_pts(self->pts)->eleNb);
    for ( i=0; i<points->eleNb ; i+=1) {
        vdw_viewpts_setDblePt(self->pts,i,points->eles[i]);
    }
    dble2set_delete( points );
    vdw_viewpts_redraw( self->pts );
}

static void oedline_vt_unitChg(void*oed)
{
    Toed_line* self = (void*)oed;
    vdw_viewpts_setUnit(self->pts, self->dw_uu);
}

static TobjAll oedline_vt_newObj(void*oed)
{
    Toed_line* self = (void*)oed;
    Tdble2Set* pts = dble2set_newClone( vdw_viewpts_pts(self->pts) );
    TobjAll obj;
    if ( self->dw_kind==FIG_KD_Line ) {
        obj = obj_line_newA(&self->dw_dp,self->dw_type,pts);
    } else {
        obj = obj_spline_newA(&self->dw_dp,self->dw_type,pts);
    }
    return obj;
}

static Tyoed_vtable yoed_vt_line = {
    .op_size=sizeof(Twid_box), .op_center=1,
    .objChg  = oedline_vt_objChg,
    .unitChg = oedline_vt_unitChg,
    .newObj  = oedline_vt_newObj,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oedline_vt_free(Twid_any*oed) { vdw_viewpts_free ( ((Toed_line*)oed)->pts ); }

static Twid_anyVtable widany_vt_line = { WID_ANY_SCVT_TBX(sizeof(Toed_line),&yoed_any_vtable,oedline_vt_free, &yoed_vt_line) };

/*======================================================================*/
