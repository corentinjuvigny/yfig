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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: wid-lvu-ddlpxm.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-ddlpxm.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item as a DDL of pixmaps.              =*/
/*=   - in an input LVU the change callback is called when the user    =*/
/*=     selects an item.                                               =*/ 
/*=   - in an output LVU, only the DDL callback is called.             =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*= See the vgui/wid-menu.h header for documentation about DDL.        =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/wid-menu.h"

#include "wid-lvuI.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_valDdlPxm {
    Tlvu_valBase0 base;

    int         dx,dy;  // size of DDL pixmap.
    TpixmapEGS* eles;   // the pixmap table
    int         eleNb;  // the table size
    Twid_cb_Uap cb;
    void*       ud;


    /* initial value */
    int initValue;    // the index in DDL of the pixmap that is shown
                      // at start time.

    /* current value */
    int currValue;    // the index in DDL of the pixmap that is
                      // currently shown.

    /* implementation */
    Twidm_ddl*   ddl;
};


/*======================================================================*/

static void lvuvt_dllpxm_cb(Twidm_any*m, Twidm_entry* e);

static void lvuvt_dllpxm_dbf(Twid_lvuBase* lvu)
{
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    Tvgui*          gui  =  vgui_dw_getVGui(lvu->lvu_vdw);
    Twidm_cb cb = self->cb==0 ? 0 : lvuvt_dllpxm_cb;
    void*    ud = self->cb==0 ? 0 : lvu;
    self->ddl = widm_ddl_newPxm(gui,
            NULL,NULL,               // vgui-data and help
            self->eles, self->eleNb, // pxm table
            self->dx,   self->dy,    // pxm size
            cb, ud);
    widmD_ddl_dbf(self->ddl,vgui_dw_getWidEGS(lvu->lvu_vdw));
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static void lvuvt_dllpxm_free(Twid_lvuBase* lvu)
{
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    widm_ddl_delete(self->ddl);
    if ( (lvu->lvu_options&LVUOPT_FreePxm)!=0 ) {
        Tvgui*          gui  = vgui_dw_getVGui(lvu->lvu_vdw);
        int i;
        for ( i=0 ; i<self->eleNb ; i+=1 ) {
            gui_pixmap_free(gui,self->eles[i] );
        }
        free( self->eles );
    }
    free( self );
}

static Tegs_widget lvuvt_dllpxm_widEGS(Twid_lvuBase* lvu)
{ return lvu->lvu_valDdlPxm->ddl->m_btnEGS; }

static Twidm_ddl* lvuvt_dllpxm_valDdl(Twid_lvuBase* lvu)
{
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    return  self->ddl;
}

static double lvuvt_dllpxm_getNum10(Twid_lvuBase* lvu)
{ return  lvu->lvu_valDdlPxm->currValue; }

static void lvuvt_dllpxm_setNum10(Twid_lvuBase* lvu, double v)
{
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    int iv = v;
    TEMP_ase_printf( iv<0 || self->eleNb<=iv, 
            "v=%d out of range [0:%d[",iv,self->eleNb);
    iv = iv<0 ? 0 : self->eleNb<=iv ? self->eleNb-1 : iv;
    self->currValue = iv;
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static void lvuvt_dllpxm_reset(Twid_lvuBase* lvu)
{
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    self->currValue = self->initValue;
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static Tlvu_valVtable lvuvt_dllpxm_VT = {
    .dbf      = lvuvt_dllpxm_dbf,
    .free     = lvuvt_dllpxm_free,
    .widEGS   = lvuvt_dllpxm_widEGS,
    .valDdl   = lvuvt_dllpxm_valDdl,
    .getNum10 = lvuvt_dllpxm_getNum10,
    .setNum10 = lvuvt_dllpxm_setNum10,
    .reset    = lvuvt_dllpxm_reset,
};

extern Tlvu_valBase* lvu_newOValDdlPxm(Twid_lvuBase0* lvu,
                     int  iv,TpixmapEGS*ps, int pn,
                     int dx, int dy, Twid_cb_Uap cb, void*ud)
{
    Tlvu_valDdlPxm* self  = tool_malloc_0( sizeof(*self) );
    self->base.vtable = &lvuvt_dllpxm_VT;
    self->dx        = dx;
    self->dy        = dy;
    self->eles      = ps;
    self->eleNb     = pn;
    self->initValue = iv;
    self->currValue = iv;
    self->cb        = cb;
    self->ud        = ud;
    lvu->options |= LVUOPT_FreePxm;
    return (Tlvu_valBase*)self;
}

extern Tlvu_valBase* lvu_newIValDdlPxm(Twid_ilvu* lvu,
                     int  iv,TpixmapEGS*ps, int pn,
                     int dx, int dy)
{
    return lvu_newOValDdlPxm(&lvu->base, iv, ps,pn, dx,dy,
            lvu->lvu_cbChg, lvu->lvu_ud);
}

/*======================================================================*/

static void lvuvt_dllpxm_cb(Twidm_any*m, Twidm_entry* e)
{
    Twidm_ddl*      ddl  = (Twidm_ddl*) m;
    Twid_lvuBase*   lvu  = ddl->m_appCBud; 
    Tlvu_valDdlPxm* self = lvu->lvu_valDdlPxm;
    self->currValue = ddl->m_selectedEntry;
    if ( self->cb ) {
        double valDble = self->currValue;
        self->cb(lvu->lvu_vdw,lvu,valDble);
    }
}

/*======================================================================*/
