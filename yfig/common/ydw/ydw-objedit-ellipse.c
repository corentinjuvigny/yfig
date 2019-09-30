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
 *      $Id: ydw-objedit-ellipse.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-ellipse.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top dialog for editing ellipse objects.                            =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"

#include "fig/obj-ellipse.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Toed_ell Toed_ell;
typedef const   Toed_ell Coed_ell;

struct _Toed_ell { YOED_FIELDS(); Twid_grp* axis; Twid_grp* angle; } ;

Inline Tyoed_any* ANY(Toed_ell* self) { return (Tyoed_any*)self; }

static Twid_anyVtable widany_vt_ell;

/*======================================================================*/
/*= Interface                                                          =*/

static void oedell_cb_axis (Twid_grp*grp, void*ud)
{
    Toed_ell*  self = ud; if ( self->dw_inUpdate ) return;

    if ( self->dw_type==FIGOELL_T_CirByRad || self->dw_type==FIGOELL_T_CirByDia ) {
        Tdble2 newradius = wid_grp_getDblePt(grp);
        Tdble2 oldradius = yoed_any_currObj(ANY(self)).ell->OEL_radiuses;
        if ( ! dble_close(newradius.x,oldradius.x) ) {
            wid_grp_setDble01(grp, newradius.x); // set y to x
            yoed_any_cb_grp(grp,ANY(self));
        } else if ( ! dble_close(newradius.y,oldradius.y) ) {
            wid_grp_setDble10(grp, newradius.y); // set x to y
            yoed_any_cb_grp(grp,ANY(self));
        }
    } else
        yoed_any_cb_grp(grp,ANY(self));
}

extern Tyoed_any* yoed_ellipse_nbf(Tyfig_mw* ymw, TobjAll obj)
{
    Toed_ell*  self;
    int        kind = obj_Kind(obj);

    TEMP_asf_printf( kind!=FIG_KD_Ellipse, "%d is an unexpected object kind",kind);

    self = (Toed_ell*)yoed_any_new(&widany_vt_ell,ymw,obj);

    self->axis = wid_iopt_nbf   (self->objparams,"radius x/y", NULL,oedell_cb_axis ,self, obj.ell->OEL_radiuses, obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->angle = wid_ioa180_nbf(self->objparams,"angle",      NULL,yoed_any_cb_grp,self, obj.ell->OEL_angle);
    wid_grp_updMin(self->axis,0); // radiuses >= 0

    vgui_dw_finishMTX(self->objparams
        ,VDWLF_ROW(0) ,MTX_IOPT_VXYx(0,self->center)
        ,VDWLF_ROW(1) ,MTX_IOPT_VXYx(0,self->axis)
        ,VDWLF_ROW(2) ,MTX_IOAD_VAU (0,self->angle)
        ,-1);

    yoed_any_dbf((Tyoed_any*)self);
    return (Tyoed_any*)self;
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void oedell_vt_unitChg(void*oed);

static void oedell_vt_typeChg(void*oed)
{
    Toed_ell*    self = oed;
    TobjEllipse* oe   = yoed_any_currObj(oed).ell;
    switch ( self->dw_type ) {
        case FIGOELL_T_CirByDia:
        case FIGOELL_T_CirByRad:
            wid_grp_clear  (self->angle);
            wid_grp_disable(self->angle);
            break;
        default:
            wid_grp_setDble10(self->angle,  oe->OEL_angle);
            wid_grp_enable   (self->angle);
    }
}

static void oedell_vt_objChg(void*oed)
{
    Toed_ell* self   = oed;
    Tdble2    radius = yoed_any_currObj(oed).ell->OEL_radiuses;
    wid_grp_setDblePt(self->axis,radius);
}

static void oedell_vt_unitChg(void*oed)
{
    Toed_ell*    self = oed;
    TobjEllipse* oe   = yoed_any_currObj(oed).ell;
    wid_grp_chgUsrUnit( self->center, self->dw_uu );
    wid_grp_chgUsrUnit( self->axis,   self->dw_uu );
}

static TobjAll oedell_vt_newObj(void*oed)
{
    Toed_ell* self   = oed;
    Tdble2    center = wid_grp_getDblePt(self->center);
    Tdble2    radius = wid_grp_getDblePt(self->axis);
    double    alpha  = wid_grp_getDble10(self->angle);
    return obj_ellipse_newAcra(&self->dw_dp, self->dw_type, center, radius, alpha);
}

static Tyoed_vtable yoed_vt_ell = {
    .op_size=sizeof(Twid_box), .op_center=1,
    .typeChg = oedell_vt_typeChg,
    .objChg  = oedell_vt_objChg,
    .unitChg = oedell_vt_unitChg,
    .newObj  = oedell_vt_newObj,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oedell_vt_free(Twid_any*oed)
{
    Toed_ell* self = (void*)oed;
    wid_grp_delete ( self->axis  );
    wid_grp_delete ( self->angle );
}
static Twid_anyVtable widany_vt_ell = { WID_ANY_SCVT_TBX(sizeof(Toed_ell),&yoed_any_vtable,oedell_vt_free, &yoed_vt_ell) };

/*======================================================================*/
