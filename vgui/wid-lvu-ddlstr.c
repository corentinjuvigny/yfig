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
 *      $Id: wid-lvu-ddlstr.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-ddlstr.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item as a DDL of strings.              =*/
/*=   - in an input LVU the change callback is called when the user    =*/
/*=     selects an item.                                               =*/ 
/*=   - in an output LVU, only the DDL callback is called.             =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*= See the vgui/wid-menu.h header for documentation about DDL.        =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"
#include "vgui/wid-menu.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_valDdlStr {
    Tlvu_valBase0 base;

    char**       eles;   // the string table
    int          eleNb;  // the table size
    Twid_cb_Uap  cb;
    void*        ud;


    /* initial value */
    int initValue;    // the index in DDL of the string that is shown
                      // at start time.

    /* current value */
    int currValue;    // the index in DDL of the string that is
                      // currently shown.

    /* implementation */
    Twidm_ddl*   ddl;
};


/*======================================================================*/

static void lvuvt_dllstr_cb(Twidm_any*m, Twidm_entry* e);

static void lvuvt_dllstr_dbf(Twid_lvuBase* lvu)
{
    Tlvu_valDdlStr* self = lvu->lvu_valDdlStr;
    Tvgui*          gui  =  vgui_dw_getVGui(lvu->lvu_vdw);
    Twidm_cb cb = self->cb==0 ? 0 : lvuvt_dllstr_cb;
    void*    ud = self->cb==0 ? 0 : lvu;
    self->ddl = widm_ddl_newStr(gui,
            NULL,NULL, self->eles, // vgui-data, help, string table
            cb, ud);
    widmD_ddl_dbf(self->ddl,vgui_dw_getWidEGS(lvu->lvu_vdw));
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static void lvuvt_dllstr_free(Twid_lvuBase* lvu)
{
    Tlvu_valDdlStr* self = lvu->lvu_valDdlStr;
    widm_ddl_delete(self->ddl);
    free( self );
}

static Tegs_widget lvuvt_dllstr_widEGS  (Twid_lvuBase* lvu) { return lvu->lvu_valDdlStr->ddl->m_btnEGS; }
static Twidm_ddl*  lvuvt_dllstr_valDdl  (Twid_lvuBase* lvu) { return lvu->lvu_valDdlStr->ddl; }
static double      lvuvt_dllstr_getNum10(Twid_lvuBase* lvu) { return  lvu->lvu_valDdlStr->currValue; }
static void        lvuvt_dllstr_setNum10(Twid_lvuBase* lvu, double v)
{
    Tlvu_valDdlStr* self = lvu->lvu_valDdlStr;
    int iv = v;
    TEMP_ase_printf( iv<0 || self->eleNb<=iv, 
            "v=%d out of range [0:%d[",iv,self->eleNb);
    iv = iv<0 ? 0 : self->eleNb<=iv ? self->eleNb-1 : iv;
    self->currValue = iv;
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static void lvuvt_dllstr_reset(Twid_lvuBase* lvu)
{
    Tlvu_valDdlStr* self = lvu->lvu_valDdlStr;
    self->currValue = self->initValue;
    widm_ddl_setCurrentEntryIndex(self->ddl,self->currValue);
}

static Tlvu_valVtable lvuvt_dllstr_VT = {
    .dbf      = lvuvt_dllstr_dbf,
    .free     = lvuvt_dllstr_free,
    .widEGS   = lvuvt_dllstr_widEGS,
    .valDdl   = lvuvt_dllstr_valDdl,
    .getNum10 = lvuvt_dllstr_getNum10,
    .setNum10 = lvuvt_dllstr_setNum10,
    .reset    = lvuvt_dllstr_reset,
};

extern Tlvu_valBase* lvu_newOValDdlStr(Twid_lvuBase0* _lvu,
                     int  iv,char**strs,
                     Twid_cb_Uap cb, void*ud)
{
    Tlvu_valDdlStr* self  = tool_malloc_0( sizeof(*self) );
    self->base.vtable = &lvuvt_dllstr_VT;
    self->eles      = strs;
  //self->eleNb     = 0;
    self->initValue = iv;
    self->currValue = iv;
    self->cb        = cb;
    self->ud        = ud;
    return (Tlvu_valBase*)self;
}

extern Tlvu_valBase* lvu_newIValDdlStr(Twid_ilvu* lvu,
                     int  iv,char**strs)
{
    return lvu_newOValDdlStr(&lvu->base, iv, strs, 
            lvu->lvu_cbChg, lvu->lvu_ud);
}

/*======================================================================*/

static void lvuvt_dllstr_cb(Twidm_any*m, Twidm_entry* e)
{
    Twidm_ddl*      ddl  = (Twidm_ddl*) m;
    Twid_lvuBase*   lvu  = ddl->m_appCBud; 
    Tlvu_valDdlStr* self = lvu->lvu_valDdlStr;
    self->currValue = ddl->m_selectedEntry;
    if ( self->cb ) {
        double valDble = self->currValue;
        self->cb(lvu->lvu_vdw,lvu,valDble);
    }
}

/*======================================================================*/
