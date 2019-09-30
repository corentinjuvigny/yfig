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
 *      $Id: wid-lvu-in.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-in.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvuI.h headers for documentation.  =*/
/*= See the vgui/wid-simple.h header for the LVU interface.            =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"

/*======================================================================*/
/*= Base constructors & destructor                                     =*/

extern Twid_ilvu* wid_ilvu_newT(CstrBrief  iv,   CstrBrief  width,
                                Twid_cb_DIs vld, Twid_cb_DIs chg, void*ud)
{
    Twid_ilvu* self   = tool_malloc_0( sizeof(*self) );
    self->lvu_cbVld   = (Twid_cb_Uap)vld;
    self->lvu_cbChg   = (Twid_cb_Uap)chg;
    self->lvu_ud      = ud;
    self->lvu_valBase = lvu_newIValTxt((Twid_olvu*)self,iv,width);
    return self;
}

extern Twid_ilvu* wid_ilvu_newI(double  iv,      CstrBrief   fmt,
                                Twid_cb_DIi vld, Twid_cb_DIi chg, void*ud,
                                int     min,     int         max)
{
    Twid_ilvu* self   = tool_malloc_0( sizeof(*self) );
    self->lvu_cbVld   = (Twid_cb_Uap)vld;
    self->lvu_cbChg   = (Twid_cb_Uap)chg;
    self->lvu_ud      = ud;
    self->lvu_valBase = lvu_newIValInt((Twid_olvu*)self,
                                         iv,min,max,fmt,0,0);
    return self;
}

extern Twid_ilvu* wid_ilvu_newD(double      iv,  CstrBrief   fmt,
                                Twid_cb_DId vld, Twid_cb_DId chg, void*ud,
                                double      min, double      max)
{
    Twid_ilvu* self   = tool_malloc_0( sizeof(*self) );
    self->lvu_cbVld   = (Twid_cb_Uap)vld;
    self->lvu_cbChg   = (Twid_cb_Uap)chg;
    self->lvu_ud      = ud;
    self->lvu_valBase = lvu_newIValDle((Twid_olvu*)self,
                                         iv,min,max,fmt,0,0);
    return self;
}

extern Twid_ilvu* wid_ilvu_newA(double      iv,  CstrBrief   fmt,
                                Twid_cb_DId vld, Twid_cb_DId chg, void*ud,
                                double      min, double      max,
                                CstrBrief   labwidth)
{
    Twid_ilvu* self   = wid_ilvu_newD(iv,fmt,vld,chg,ud,min,max);
    wid_ilvu_addUnitLab(self,"degree", labwidth);
    return self;
}

extern Twid_ilvu* wid_ilvu_newDdlStr(int iv, char**strs,
                                     Twid_cb_Uap cb, void*ud)
{
    Twid_ilvu* self   = tool_malloc_0( sizeof(*self) );
    self->lvu_cbChg   = (Twid_cb_Uap)cb;
    self->lvu_ud      = ud;
    self->lvu_valBase = lvu_newIValDdlStr(self,iv,strs);
    return self;
}

extern Twid_ilvu* wid_ilvu_newDdlPxm(int iv, TpixmapEGS*ps, int pn,
                                int dx, int dy,
                                Twid_cb_Uap cb, void*ud)
{
    Twid_ilvu* self   = tool_malloc_0( sizeof(*self) );
    self->lvu_cbChg   = (Twid_cb_Uap)cb;
    self->lvu_ud      = ud;
    self->lvu_valBase = lvu_newIValDdlPxm(self,iv,ps,pn,dx,dy);
    return self;
}

extern void        wid_ilvu_free(Twid_ilvu* iw)
{  lvu_delete(&iw->base); }

/*======================================================================*/
/*= quick constructors                                                 =*/

/*======================================================================*/
/*= adding options                                                     =*/

extern void     wid_ilvu_addLabel  (Twid_ilvu* iw, CstrBrief text,
                                                   CstrBrief width)
{ lvu_labelAdd(&iw->base,text,width); }

extern void     wid_ilvu_addUnitStd(Twid_ilvu* iw)        { lvu_unitStdAdd(&iw->base); }
extern void     wid_ilvu_chgUnitStd(Twid_ilvu* iw, int u) { lvu_unitStdChg(&iw->base,u); }
extern void     wid_ilvu_chgValMin(Twid_ilvu* iw, double v)
{
    Twid_lvuBase* self = (Twid_lvuBase*)iw;
    Tlvu_valVtable* vtable = self->lvu_valVtable;
    TEMP_ase_printf( vtable->chgValMin==0, "%s","Invalid call (ignored)");
    if ( vtable->chgValMin!=0 )
            vtable->chgValMin(self,v);
}
extern void     wid_ilvu_chgValMax(Twid_ilvu* iw, double v)
{
    Twid_lvuBase* self = (Twid_lvuBase*)iw;
    Tlvu_valVtable* vtable = self->lvu_valVtable;
    TEMP_ase_printf( vtable->chgValMax==0, "%s","Invalid call (ignored)");
    if ( vtable->chgValMax!=0 )
            vtable->chgValMax(self,v);
}

extern void     wid_ilvu_addUnitLab(Twid_ilvu* iw, CstrBrief text,
                                                   CstrBrief width) { lvu_unitLabAdd(&iw->base, text, width); }
extern void     wid_ilvu_addButton (Twid_ilvu* iw, Twid_button*b)   { lvu_unitBtnAdd(&iw->base,b); }

extern void     wid_ilvu_addSpinInc(Twid_ilvu* iw, double inc)
{
    Tlvu_valVtable* vtable = iw->lvu_valVtable;
    if ( vtable->getRange!=0 ) {
        Trange* range = vtable->getRange((Twid_lvuBase*)iw);
        range_setModeInc(range,inc);
    } else
        TEMP_spe_printf("invalid invocation (return)",0);
}

extern void     wid_ilvu_addSpinMul(Twid_ilvu* iw, double fac)
{
    Tlvu_valVtable* vtable = iw->lvu_valVtable;
    if ( vtable->getRange!=0 ) {
        Trange* range = vtable->getRange((Twid_lvuBase*)iw);
        range_setModeMul(range,fac);
    } else
        TEMP_spe_printf("invalid invocation (return)",0);
}

extern Twidm_ddl* wid_ilvu_getValDdl(Twid_ilvu* iw)
{
    Tlvu_valVtable* vtable = iw->lvu_valVtable;
    if ( vtable->valDdl!=0 )
        return vtable->valDdl((Twid_lvuBase*)iw);
    else
        return NULL;
}

#include "vgui/wid-menu.h"

extern Tany       wid_ilvu_getValDdl_CurrIdAny(Twid_ilvu* iw)
{
    Twidm_ddl* ddl = wid_ilvu_getValDdl(iw);
    TEMP_ase_printf(ddl==0,"%s","invalid call (value of widget is not a ddl)");
    return ddl==0 ? tool_any_null : widm_ddl_getCurrentEntryIdent(ddl);
}
extern void       wid_ilvu_setValDdl_CurrIdAny(Twid_ilvu* iw, Tany id)
{
    Twidm_ddl* ddl = wid_ilvu_getValDdl(iw);
    TEMP_ase_printf(ddl==0,"%s","invalid call (value of widget is not a ddl)");
    if ( ddl==0 ) return;
    widm_ddl_setCurrentEntryIdent(ddl,id);
}

extern void*    wid_ilvu_getUniWid (Twid_ilvu* iw)
{ return iw->lvu_uniMisWidget; }

extern void     wid_ilvu_dontFreePxm (Twid_ilvu* iw)
{ iw->lvu_options &= ~LVUOPT_FreePxm; }

/*======================================================================*/
/*= creation of widgets                                                =*/

extern void widD_ilvu_dbf(Twid_ilvu*iw, Tvgui_dw*vdw)
{
    iw->lvu_vdw = vdw;

    /* A create the label if needed */
    lvu_labelDBF(&iw->base);

    /* B create the value widget */
    Tlvu_valVtable* vtable = iw->lvu_valVtable;
    TEMP_asf_printf( vtable==0,"vtable=%p",NULL);
    vtable->dbf((Twid_lvuBase*)iw);
    iw->lvu_valEGS = vtable->widEGS((Twid_lvuBase*)iw);

    /* C create the unit widget  */
    if ( (iw->lvu_options&LVUOPT_UnitDDL)==LVUOPT_UnitDDL ) {
        lvu_unitStdDBF(&iw->base,lvu_cb_UnitChg);
        lvu_cb_UnitChg(iw->lvu_uniDdlWidget,
                wid_lenunit_getUnit(iw->lvu_uniDdlWidget),iw);
    } else if ( (iw->lvu_options&LVUOPT_UnitBtn)==LVUOPT_UnitBtn ) {
        lvu_unitBtnDBF(&iw->base);
    } else if ( (iw->lvu_options&LVUOPT_UnitLab)==LVUOPT_UnitLab ) {
        lvu_unitLabDBF(&iw->base);
    }
}

/*======================================================================*/
/*= get and change values and control                                  =*/

extern CstrBrief wid_ilvu_getStr(Cwid_ilvu* lvu)
{ return wid_lvu_getStr((Cwid_lvuBase*)lvu); }

extern void   wid_ilvu_setStr  (Twid_ilvu* self, CstrBrief v)
{ return wid_lvu_setStr((Twid_lvuBase*)self,v); }

extern double wid_ilvu_getNum10(Cwid_ilvu* self)
{ return wid_lvu_getNum10((Cwid_lvuBase*)self); }
extern void   wid_ilvu_setNum10(Twid_ilvu* self, double v)
{ wid_lvu_setNum10((Twid_lvuBase*)self,v); }

extern void   wid_ilvu_reset   (Twid_ilvu* self) { wid_lvu_reset  ((Twid_lvuBase*)self); }
extern void   wid_ilvu_clear   (Twid_ilvu* self) { wid_lvu_clear  ((Twid_lvuBase*)self); }
extern void   wid_ilvu_unshow  (Twid_ilvu* self) { wid_lvu_unshow ((Twid_lvuBase*)self); }
extern void   wid_ilvu_show    (Twid_ilvu* self) { wid_lvu_show   ((Twid_lvuBase*)self); }
extern void   wid_ilvu_disable (Twid_ilvu* self) { wid_lvu_disable((Twid_lvuBase*)self); }
extern void   wid_ilvu_enable  (Twid_ilvu* self) { wid_lvu_enable ((Twid_lvuBase*)self); }

/*======================================================================*/
