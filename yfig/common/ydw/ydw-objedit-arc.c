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
 *      $Id: ydw-objedit-arc.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-arc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top dialog for editing arc objects.                                =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"

#include "fig/obj-arc.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Toed_arc Toed_arc;
typedef const   Toed_arc Coed_arc;

struct _Toed_arc { YOED_FIELDS(); Twid_grp* points[3]; };

Inline Tyoed_any* ANY(Toed_arc* self) { return (Tyoed_any*)self; }

static Twid_anyVtable widany_vt_arc;

/*======================================================================*/
/*= Interface                                                          =*/

extern Tyoed_any* yoed_arc_nbf(Tyfig_mw* ymw, TobjAll obj)
{
    Toed_arc*  self;
    int        kind = obj_Kind(obj);

    TEMP_asf_printf( kind!=FIG_KD_Arc, "%d is an unexpected object kind",kind);

    self = (Toed_arc*)yoed_any_new(&widany_vt_arc,ymw,obj);

    self->points[0] = wid_iopt_nbf(self->objparams,"point 1 x/y", NULL,yoed_any_cb_grp,self, obj.arc->OAR_points[0], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->points[1] = wid_iopt_nbf(self->objparams,"point 2 x/y", NULL,yoed_any_cb_grp,self, obj.arc->OAR_points[1], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->points[2] = wid_iopt_nbf(self->objparams,"point 3 x/y", NULL,yoed_any_cb_grp,self, obj.arc->OAR_points[2], obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);

    vgui_dw_finishMTX(self->objparams
        ,VDWLF_ROW(0) ,MTX_IOPT_VXYx(0,self->center)
        ,VDWLF_ROW(1) ,MTX_IOPT_VXYx(0,self->points[0])
        ,VDWLF_ROW(2) ,MTX_IOPT_VXYx(0,self->points[1])
        ,VDWLF_ROW(3) ,MTX_IOPT_VXYx(0,self->points[2])
        ,-1);

    yoed_any_dbf((Tyoed_any*)self);
    return (Tyoed_any*)self;
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void oedarc_vt_objChg(void*oed)
{
    int i; Tdble2 pt;
    Toed_arc* self = oed;
    for ( i=0; i<3 ; i+=1) {
        pt = yoed_any_currObj(oed).arc->OAR_points[i];
        wid_grp_setDblePt(self->points[i],pt);
    }
}

static void oedarc_vt_unitChg(void*oed)
{
    Toed_arc* self = oed;
    wid_grp_chgUsrUnit( self->points[0], self->dw_uu );
    wid_grp_chgUsrUnit( self->points[1], self->dw_uu );
    wid_grp_chgUsrUnit( self->points[2], self->dw_uu );
}

static TobjAll oedarc_vt_newObj(void*oed)
{
    Toed_arc* self = oed;
    Tdble2    pt1  = wid_grp_getDblePt( self->points[0] );
    Tdble2    pt2  = wid_grp_getDblePt( self->points[1] );
    Tdble2    pt3  = wid_grp_getDblePt( self->points[2] );
    return obj_arc_newA3(&self->dw_dp, self->dw_type, pt1,pt2,pt3);
}

static Tyoed_vtable yoed_vt_arc = {
    .op_size=sizeof(Twid_box), .op_center=1,
    .objChg  = oedarc_vt_objChg,
    .unitChg = oedarc_vt_unitChg,
    .newObj  = oedarc_vt_newObj,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oedarc_vt_free(Twid_any*oed)
{
    Toed_arc* self = (void*)oed;
    wid_grp_delete ( self->points[0] );
    wid_grp_delete ( self->points[1] );
    wid_grp_delete ( self->points[2] );
}
static Twid_anyVtable widany_vt_arc = { WID_ANY_SCVT_TBX(sizeof(Toed_arc),&yoed_any_vtable,oedarc_vt_free, &yoed_vt_arc) };

/*======================================================================*/
