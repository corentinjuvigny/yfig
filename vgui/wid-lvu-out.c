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
 *      $Id: wid-lvu-out.c 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-out.c $
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

extern Twid_olvu* wid_olvu_newT(CstrBrief  iv,  CstrBrief  width)
{
    Twid_olvu* ow  = tool_malloc_0( sizeof(*ow) );
    ow->lvu_valBase = lvu_newOValTxtLab(ow,iv,width);
    return ow;
}

extern Twid_olvu* wid_olvu_newC(CstrBrief iv, CstrBrief width,
                                Ccolor*   ic, int dx, int dy)
{
    Twid_olvu* ow  = tool_malloc_0( sizeof(*ow) );
    ow->lvu_valBase = lvu_newOValClrLab(ow,iv,width,ic,dx,dy);
    return ow;
}


extern Twid_olvu* wid_olvu_newN10(double  iv, CstrBrief  fmt)
{
    Twid_olvu* ow  = tool_malloc_0( sizeof(*ow) );
    ow->lvu_valBase = lvu_newOValNumLab10(ow,iv,fmt,0,0);
    return ow;
}

extern Twid_olvu* wid_olvu_newN11(double iv, CstrBrief fmt, double iv2)
{
    Twid_olvu* ow   = tool_malloc_0( sizeof(*ow) ); // FIXME wid_olvu_newN10(iv,fmt);
    ow->lvu_valBase = lvu_newOValNumLab11(ow,iv,iv2,fmt,0,0);
    return ow;
}

extern void        wid_olvu_free(Twid_olvu* ow)
{  lvu_delete(&ow->base); }

/*======================================================================*/
/*= quick constructors                                                 =*/

extern Twid_olvu* wid_olvu_newTT   (CstrBrief v, CstrBrief vw,
                                    CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newT(v,vw); wid_olvu_addLabel(o,t,tw); return o; }

extern Twid_olvu* wid_olvu_newN10T (double v,    CstrBrief fmt,
                                    CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN10(v,fmt); wid_olvu_addLabel(o,t,tw); return o; }

extern Twid_olvu* wid_olvu_newN11T (double v, CstrBrief fmt, double v2,
                                    CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN11(v,fmt,v2); wid_olvu_addLabel(o,t,tw); return o; }

extern Twid_olvu* wid_olvu_newN10TU(double v, CstrBrief fmt,
                                    CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN10(v,fmt); wid_olvu_addLabel(o,t,tw);
  wid_olvu_addUnitStd(o); return o; }

extern Twid_olvu* wid_olvu_newN11TU(double v, CstrBrief fmt, double v2,
                                    CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN11(v,fmt,v2); wid_olvu_addLabel(o,t,tw);
  wid_olvu_addUnitStd(o); return o; }

extern Twid_olvu* widD_olvu_nbfTT   (Tvgui_dw*d,
       CstrBrief v, CstrBrief vw, CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newTT(v,vw,t,tw); widD_olvu_dbf(o,d); return o; }

extern Twid_olvu* widD_olvu_nbfN10T (Tvgui_dw*d,
        double v, CstrBrief fmt, CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN10T(v,fmt,t,tw); widD_olvu_dbf(o,d);
  return o; }

extern Twid_olvu* widD_olvu_nbfN11T (Tvgui_dw*d,
       double v, CstrBrief fmt, double v2, CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN11T(v,fmt,v2,t,tw); widD_olvu_dbf(o,d);
  return o; }

extern Twid_olvu* widD_olvu_nbfN10TU(Tvgui_dw*d,
       double v, CstrBrief fmt, CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN10TU(v,fmt,t,tw); widD_olvu_dbf(o,d);
  return o; }

extern Twid_olvu* widD_olvu_nbfN11TU(Tvgui_dw*d,
       double v, CstrBrief fmt, double v2, CstrBrief t, CstrBrief tw)
{ Twid_olvu*o=wid_olvu_newN11TU(v,fmt,v2,t,tw); widD_olvu_dbf(o,d);
  return o; }

/*======================================================================*/
/*= adding options                                                     =*/

extern void     wid_olvu_addLabel  (Twid_olvu* ow, CstrBrief text,
                                                   CstrBrief width)
{ lvu_labelAdd(&ow->base,text,width); }

extern void     wid_olvu_addUnitStd   (Twid_olvu* ow)
{ lvu_unitStdAdd(&ow->base); }
extern void     wid_olvu_chgUnitStd(Twid_olvu* ow, int u)
{ lvu_unitStdChg(&ow->base,u); }

extern void     wid_olvu_addUnitLab(Twid_olvu* ow, CstrBrief text,
                                                   CstrBrief width)
{ lvu_unitLabAdd(&ow->base, text, width); }

extern void     wid_olvu_addButton (Twid_olvu* ow, Twid_button*b)
{ lvu_unitBtnAdd(&ow->base, b); }

extern void*    wid_olvu_getUniWid (Twid_olvu* ow)
{ return ow->lvu_uniMisWidget; }

/*======================================================================*/
/*= creation of widgets                                                =*/

static void olvu_cb_UnitChg (Twid_lenunit*lu, int unit, void*_ow);

extern void widD_olvu_dbf(Twid_olvu*ow, Tvgui_dw*vdw)
{
    ow->lvu_vdw = vdw;

    /* A create the label if needed */
    lvu_labelDBF(&ow->base);

    /* B create the value widget  */
    Tlvu_valVtable* vtable = ow->lvu_valVtable;
    TEMP_asf_printf( vtable==0,"vtable=%p",NULL);
    vtable->dbf((Twid_lvuBase*)ow);
    ow->lvu_valEGS = vtable->widEGS((Twid_lvuBase*)ow);
    
    /* C create the unit widget  */
    if ( (ow->lvu_options&LVUOPT_UnitDDL)==LVUOPT_UnitDDL ) {
        lvu_unitStdDBF(&ow->base,lvu_cb_UnitChg);
        //lvu_cb_UnitChg(ow->lvu_uniDdlWidget,
        //        wid_lenunit_getUnit(ow->lvu_uniDdlWidget),ow);
    } else if ( (ow->lvu_options&LVUOPT_UnitBtn)==LVUOPT_UnitBtn ) {
        lvu_unitBtnDBF(&ow->base);
    } else if ( (ow->lvu_options&LVUOPT_UnitLab)==LVUOPT_UnitLab ) {
        lvu_unitLabDBF(&ow->base);
    }
}

/*======================================================================*/
/*= get and change values and control                                  =*/

extern CstrBrief wid_olvu_getStr(Cwid_olvu* self)
{ return wid_lvu_getStr((Cwid_lvuBase*)self); }
extern void      wid_olvu_setStr  (Twid_olvu* self, CstrBrief v)
{ return wid_lvu_setStr((Twid_lvuBase*)self,v); }

extern double wid_olvu_getNum10(Cwid_olvu* self)
{ return wid_lvu_getNum10((Cwid_lvuBase*)self); }
extern double wid_olvu_getNum01(Cwid_olvu* self)
{ return wid_lvu_getNum01((Cwid_lvuBase*)self); }
extern void   wid_olvu_setNum10(Twid_olvu* self, double v)
{ wid_lvu_setNum10((Twid_lvuBase*)self,v); }
extern void   wid_olvu_setNum01(Twid_olvu* self, double v2)
{ wid_lvu_setNum01((Twid_lvuBase*)self,v2); }
extern void   wid_olvu_setNum11(Twid_olvu* self, double v, double v2)
{ wid_lvu_setNum11((Twid_lvuBase*)self,v,v2); }

extern void   wid_olvu_reset   (Twid_olvu* self) { wid_lvu_reset((Twid_lvuBase*)self); }
extern void   wid_olvu_clear   (Twid_olvu* self) { wid_olvu_setStr(self,""); }
extern void   wid_olvu_unshow  (Twid_olvu* self) { wid_lvu_unshow ((Twid_lvuBase*)self); }
extern void   wid_olvu_show    (Twid_olvu* self) { wid_lvu_show   ((Twid_lvuBase*)self); }
extern void   wid_olvu_disable (Twid_olvu* self) { wid_lvu_disable((Twid_lvuBase*)self); }
extern void   wid_olvu_enable  (Twid_olvu* self) { wid_lvu_enable((Twid_lvuBase*)self); }

extern void wid_olvu_setBgClr(Twid_olvu* ow, Ccolor* c)
{
    DBG0_OUTE("olvu=%p c=%s\n",ow,DBG_Color2str(c));
    TEMP_asf_printf( ow->lvu_valBase==0,"ow->lvu_valBase=%p",NULL);
    Tlvu_valVtable* vtable = ow->lvu_valVtable;
    if ( vtable->setBgClr!=0 )
         vtable->setBgClr((Twid_lvuBase*)ow,c);
    else {
        TEMP_spe_printf("invalid call (color not set: %s)",
                DBG_Color2str(c));
    }
    DBG1_OUTL("olvu=%p c=%s\n",ow,DBG_Color2str(c));
}

extern void wid_olvu_setBgPxm(Twid_olvu* ow, TpixmapEGS pxm)
{
    DBG0_OUTE("olvu=%p pxm=%p\n",ow,pxm);
    TEMP_asf_printf( ow->lvu_valBase==0,"ow->lvu_valBase=%p",NULL);
    Tlvu_valVtable* vtable = ow->lvu_valVtable;
    if ( vtable->setBgPxm!=0 )
         vtable->setBgPxm((Twid_lvuBase*)ow,pxm);
    else {
        TEMP_spe_printf("invalid call (tile not set: %p)",pxm);
    }
    DBG1_OUTL("olvu=%p pxm=%p\n",ow,pxm);
}
extern void wid_olvu_setBgBtm(Twid_olvu* ow, Cvgui_icon* icon)
{
    DBG0_OUTE("olvu=%p ico=%p\n",ow,icon);
    TEMP_asf_printf( ow->lvu_valBase==0,"ow->lvu_valBase=%p",NULL);
    Tlvu_valVtable* vtable = ow->lvu_valVtable;
    if ( vtable->setBgBtm!=0 )
         vtable->setBgBtm((Twid_lvuBase*)ow,icon);
    else {
        TEMP_spe_printf("invalid call (tile not set: %p)",icon);
    }
    DBG1_OUTL("olvu=%p ico=%p\n",ow,icon);
}

/*======================================================================*/
