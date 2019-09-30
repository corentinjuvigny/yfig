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
 *      $Id: ydw-objedit-text.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top dialog for editing text objects.                               =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"

#include "vgui/wid-lvu.h"
#include "fig/obj-text.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Toed_text Toed_text;
typedef const   Toed_text Coed_text;

struct _Toed_text { YOED_FIELDS(); Twid_grp*refpoint; Twid_ilvu* intext; };

Inline Tyoed_any* ANY(Toed_text* self) { return (Tyoed_any*)self; }

static Twid_anyVtable widany_vt_text;

/*======================================================================*/
/*= Interface                                                          =*/

extern Tyoed_any* yoed_text_nbf(Tyfig_mw* ymw, TobjAll obj)
{
    int i;
    Toed_text* self;
    int        kind = obj_Kind(obj);
    char       width[100]; for (i=0;i<40;i+=1) width[i]='W'; width[i]=0;

    TEMP_asf_printf( kind!=FIG_KD_Text, "%d is an unexpected object kind",kind);

    self = (Toed_text*)yoed_any_new(&widany_vt_text,ymw,obj);

    self->refpoint = wid_iopt_nbf(self->objparams,"reference point x/y", NULL,yoed_any_cb_grp,self, obj.text->OTE_base, obj_Ppi(obj), self->dw_uu, self->dw_xyfmt);
    self->intext   = wid_ilvu_newT(obj.text->OTE_str,width, NULL,yoed_any_cb_ilvuT,self);
    wid_ilvu_addLabel(self->intext,"Text",NULL);
    widD_ilvu_dbf(self->intext,self->objparams);

    vgui_dw_finishMTX(self->objparams
        ,VDWLF_ROW(0) ,MTX_IOPT_VXYx(0,self->center)
        ,VDWLF_ROW(1) ,MTX_IOPT_VXYx(0,self->refpoint)
        ,VDWLF_ROW(2) ,0,self->intext->lvu_labEGS ,1|VDWLF_EH,self->intext->lvu_valEGS ,2,self->intext->lvu_valEGS
        ,-1);

    yoed_any_dbf((Tyoed_any*)self);
    return (Tyoed_any*)self;
}

/*======================================================================*/
/* Abstract services of class                                          =*/

static void oedtext_vt_objChg(void*oed)
{
    int i;
    Toed_text* self = oed;
    Tdble2 pt = yoed_any_currObj(oed).text->OTE_base;
    wid_grp_setDblePt(self->refpoint,pt);
}

static void oedtext_vt_unitChg(void*oed)
{
    Toed_text* self = oed;
    wid_grp_chgUsrUnit( self->refpoint, self->dw_uu );
}

static TobjAll oedtext_vt_newObj(void*oed)
{
    Toed_text* self = oed;
    double    width  = 0;
    double    height = 0;
    int       flag   = 0; // not used.
    CstrBrief text   = wid_ilvu_getStr(self->intext);
    Tdble2    pt     = wid_grp_getDblePt( self->refpoint );
    return obj_text_newA(&self->dw_dp, self->dw_type, pt,width,height, text, flag);
}

static Tyoed_vtable yoed_vt_text = {
    .op_size=sizeof(Twid_box), .op_center=1,
    .objChg  = oedtext_vt_objChg,
    .unitChg = oedtext_vt_unitChg,
    .newObj  = oedtext_vt_newObj,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void oedtext_vt_free(Twid_any*oed)
{ Toed_text* self = (void*)oed; wid_ilvu_free ( self->intext ); }

static Twid_anyVtable widany_vt_text = { WID_ANY_SCVT_TBX(sizeof(Toed_text),&yoed_any_vtable,oedtext_vt_free, &yoed_vt_text) };

/*======================================================================*/
