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
 *      $Id: ydw-objedit-du.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-du.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that handles the depth and the unit of edit top-dialog. =*/
/*= All the dialogues that edit figure objects must place it just      =*/
/*= under the header sub-dialog.                                       =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-lvu.h"

#include "tools/lenunit.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Toed_du {
    Tvgui_dw      any;

    // widgets
    Twid_ilvu*    dw_depth1;
    Twid_olvu*    dw_depth2;   // NULL when no max depth
    Tegs_widget   dw_unitTit;  // NULL: the option no-unit set (label)
    Twid_lenunit* dw_unitDdl;  // NULL: the option no-unit set

    // proper data
   Twid_cb_DU dw_cbDepth;
   Twid_cb_DU dw_cbUnit;
   void*      dw_cbUD;

   int        dw_d1,dw_d2; // current value of the two depths
   int        dw_u;        // current unit
} ;

typedef struct _Toed_du Toed_du;
typedef const   Toed_du Coed_du;

/*======================================================================*/
/*= Interface                                                          =*/

static void oed_du_cb_depth(Tvgui_dw* vdw, Twid_ilvu*iw, int v);
static void oed_du_cb_unit (Twid_lenunit* lu, int u, void*ud);

extern Tvgui_dw* ydw_objedit_du_nbfSub(Tvgui_dw* vdw, int lu, int opt,
                                       Twid_cb_DU cbd, Twid_cb_DU cbu,
                                       void* ud)
{
    //Tvgui*      gui   = vgui_dw_getVGui(vdw);
    const char*   depthtitle = (opt&1)!=0 ? " Depths " : " Depth ";
    const char*   depthwidth = " 999 ";
    const char*   unittitle  = " unit ";

    // sub-dialog creation
    Toed_du* self = (Toed_du*) vgui_dw_newSub(sizeof(*self),vdw,NULL);
    self->dw_cbDepth = cbd;
    self->dw_cbUnit  = cbu;
    self->dw_cbUD    = ud;
    self->dw_u       = lu;

    // widget creation
    self->dw_depth1 = wid_ilvu_newI(5, depthwidth,
                        oed_du_cb_depth,oed_du_cb_depth,self,
                        FIG_MIN_DEPTH, FIG_MAX_DEPTH);
    wid_ilvu_addLabel(self->dw_depth1, depthtitle, NULL);
    widD_ilvu_dbf(self->dw_depth1,SelfA);
    if ( opt&1 ) {
        self->dw_depth2 = wid_olvu_newN10(0,depthwidth);
        widD_olvu_dbf(self->dw_depth2,SelfA);
    }
    if ( (opt&2)==0 ) {// 2 option means no DDL unit
        self->dw_unitTit = widD_label_nbf(SelfA,unittitle,NULL);
        self->dw_unitDdl = wid_lenunit_nbf(SelfA,oed_du_cb_unit,self);
        wid_lenunit_setUnit(self->dw_unitDdl,lu);
    }

    // vidget layout
#   define OD1_Lab self->dw_depth1->lvu_labEGS
#   define OD1_Val self->dw_depth1->lvu_valEGS
#   define OD2_Val self->dw_depth2 ? self->dw_depth2->lvu_valEGS : NULL
#   define OU_Lab  self->dw_unitTit
#   define OU_Ddl  self->dw_unitDdl ? \
                       wid_lenunit_widget(self->dw_unitDdl) : NULL
    vgui_dw_finishIL(SelfA
     ,0          ,OD1_Lab
     ,0          ,OD1_Val
     ,0          ,OD2_Val
     ,VDWLF_SEP3 ,OU_Lab
     ,0          ,OU_Ddl
   ,-1);

    return SelfA;
}

extern void      ydw_objedit_du_free  (Tvgui_dw* vdw)
{
    Toed_du* self = (Toed_du*)vdw;

    wid_ilvu_free ( self->dw_depth1 );
    if ( self->dw_depth2 )
        wid_olvu_free ( self->dw_depth2 );
    if ( self->dw_unitDdl )
        wid_lenunit_free( self->dw_unitDdl );
    vgui_dw_free( vdw );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern int       ydw_objedit_du_depth1(Tvgui_dw* vdw) { return ((Toed_du*)vdw)->dw_d1; }
extern int       ydw_objedit_du_depth2(Tvgui_dw* vdw) { return ((Toed_du*)vdw)->dw_d2; }
extern int       ydw_objedit_du_unit  (Tvgui_dw* vdw) { return ((Toed_du*)vdw)->dw_u; }
extern void      ydw_objedit_du_update(Tvgui_dw* vdw, int u, int d1, int d2)
{
    Toed_du* self = (Toed_du*)vdw;
    if ( u!=LUNIT_ID_None && self->dw_unitDdl!=0 ) {
        TEMP_asf_printf( ! LUNIT_ID_IsValid(u), "invalid LUNIT_ID %d.",u);
        wid_lenunit_setUnit(self->dw_unitDdl,u);
    }
    if ( d1>=0 && self->dw_depth1!=0 ) {
        TEMP_asf_printf( d1>=1000, "invalid depth %d.",d1);
        wid_ilvu_setNum10(self->dw_depth1,d1);
    }
    if ( d2>=0 && self->dw_depth2!=0 ) {
        TEMP_asf_printf( d2>=1000, "invalid depth %d.",d1);
        wid_ilvu_setNum10(self->dw_depth1,d1);
    }
}

/*======================================================================*/
/*= Private utilities                                                  =*/

/*======================================================================*/
/*= Callbacks                                                          =*/

static void oed_du_cb_depth(Tvgui_dw* vdw, Twid_ilvu*iw, int v)
{
    Toed_du* self = (Toed_du*)vdw;
    if (v==self->dw_d1) return;
    int inc = v-self->dw_d1;
    if ( self->dw_depth2!=0 ) {
        if ( (self->dw_d2+inc)>=FIG_MAX_DEPTH ) {
            inc = FIG_MAX_DEPTH-self->dw_d2;
        }
        self->dw_d2 += inc;
        wid_olvu_setNum10( self->dw_depth2, self->dw_d2 );
    }
    self->dw_d1 += inc;
    wid_ilvu_setNum10( self->dw_depth1,  self->dw_d1 );
    if ( self->dw_cbDepth )
        self->dw_cbDepth(SelfA, self->dw_cbUD);
}

static void oed_du_cb_unit (Twid_lenunit* lu, int u, void*ud)
{
    Toed_du* self = ud;
    self->dw_u = wid_lenunit_getUnit(self->dw_unitDdl);
    if ( self->dw_cbUnit )
        self->dw_cbUnit(SelfA, self->dw_cbUD);
}

/*======================================================================*/
