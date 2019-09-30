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
 *      $Id: wid-lvu.c 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvuI.h headers for documentation.  =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"

/*======================================================================*/
/*= utilities                                                          =*/

// for numeric widgets only
// Wu=Widget Unit ; Fu=FIG unit (reference unit)
extern double lvu_Wu2Fu(Cwid_lvuBase0* _self, double val)
{
    Cwid_lvuBase* self = (Cwid_lvuBase*)_self;
//    TEMP_asf_printf((self->lvu_valKd&LVUVALKD_Text)!=0,
//            "invalid invocation",0);
    Cwid_lenunit* uw=self->lvu_uniDdlWidget;
    return uw ? wid_lenunit_convToFU(uw,val) : val;
}

extern double lvu_Fu2Wu(Cwid_lvuBase0* _self, double val)
{
    Cwid_lvuBase* self = (Cwid_lvuBase*)_self;
//    TEMP_asf_printf((self->lvu_valKd&LVUVALKD_Text)!=0,
//            "invalid invocation",0);
    Cwid_lenunit* uw=self->lvu_uniDdlWidget;
    return uw ? wid_lenunit_convToWU(uw,val) : val;
}

static void   lvu_unitStdChg0 (Cwid_lvuBase0* _self, int unit)
{
    Twid_lvuBase* lvu = (Twid_lvuBase*)_self;
    DBG0_OUTE("lvu=%p unit=%d\n",lvu,unit);
        Tlvu_valVtable* vtable = lvu->lvu_valVtable;
        if ( vtable->unitChged!=0 )
            vtable->unitChged(lvu,unit);
        else if ( vtable->print!=0 )
             vtable->print(lvu);
        else
            TEMP_spe_printf("invalid call (ignored)",0);
    DBG1_OUTL("lvu=%p unit=%d\n",lvu,unit);
}

extern void   lvu_unitStdChg (Cwid_lvuBase0* _self, int unit)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    TEMP_asf_printf( (self->lvu_options&LVUOPT_UnitDDL)!=LVUOPT_UnitDDL,
        "this lvu does not support unit change (options=%0x\n",self->lvu_options);
    wid_lenunit_setUnit( self->lvu_uniDdlWidget, unit);
    lvu_unitStdChg0(_self,unit);
}

extern void lvu_cb_UnitChg (Twid_lenunit*lu, int unit, void*ud)
{ lvu_unitStdChg0(ud,unit); }

/*======================================================================*/
/*= adding / building / freing options                                 =*/

extern void   lvu_labelAdd (Twid_lvuBase0* _self,
                            CstrBrief text, CstrBrief width)
{ 
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    self->lvu_options |= LVUOPT_Label;
    self->lvu_labText  = tool_strdup(text);
    self->lvu_labWidth = tool_strdup(width);
}
extern void   lvu_labelDBF (Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    if ( (self->lvu_options&LVUOPT_Label)!=0 ) {
        self->lvu_labWidget = widD_label_nbf(self->lvu_vdw,
                self->lvu_labText, self->lvu_labWidth);
        self->lvu_labEGS = self->lvu_labWidget;
    }
}
extern void   lvu_labelFree(Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    tool_free( self->lvu_labText );
    tool_free( self->lvu_labWidth );
}

extern void lvu_unitLabAdd (Twid_lvuBase0* _self, CstrBrief text,
                                                  CstrBrief width)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    self->lvu_options |= LVUOPT_UnitLab;
    self->lvu_uniText  = tool_strdup( text );
    self->lvu_uniWidth = tool_strdup( width );
}
extern void   lvu_unitLabDBF (Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    self->lvu_uniEGS = widD_label_nbf(self->lvu_vdw,
                       self->lvu_uniText, self->lvu_uniWidth);
}
extern void   lvu_unitLabFree(Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    tool_free( self->lvu_uniText  );
    tool_free( self->lvu_uniWidth );
}

extern void   lvu_unitStdAdd(Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    self->lvu_options |= LVUOPT_UnitDDL;
}
extern void   lvu_unitStdDBF(Twid_lvuBase0* _self, Twid_cb_LiU cb)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    if ( self->lvu_options&LVUOPT_UnitDDL ) {
        self->lvu_uniDdlWidget = wid_lenunit_nbf(self->lvu_vdw, cb, self);
        self->lvu_uniEGS       = wid_lenunit_widget(self->lvu_uniDdlWidget);
    }
}
extern void   lvu_unitStdFree(Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    if ( self->lvu_options&LVUOPT_UnitDDL )
        wid_lenunit_free( self->lvu_uniDdlWidget );
}

extern void lvu_unitBtnAdd (Twid_lvuBase0* _self, Twid_button* btn)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    self->lvu_options     |= LVUOPT_UnitBtn;
    self->lvu_uniBtnWidget = btn;
}

extern void lvu_unitBtnDBF (Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    Twid_button*  btn  = self->lvu_uniBtnWidget;
    if ( btn->but_widEGS==0 ) { // not yet build
        btn->but_vdw = self->lvu_vdw;
        wid_button_dbf(btn,NULL);
    }
    self->lvu_uniEGS = btn->but_widEGS;
}

extern void   lvu_unitBtnFree(Twid_lvuBase0* _self)
{
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    if ( self->lvu_options&LVUOPT_UnitBtn ) {
        wid_button_delete( self->lvu_uniBtnWidget );
    }
}

extern void   lvu_delete(Twid_lvuBase0* _self)
{
    if ( _self==0 ) return;
    Twid_lvuBase* self = (Twid_lvuBase*)_self;
    Tlvu_valVtable* vtable = self->lvu_valVtable;
    lvu_labelFree( _self );
    if ( vtable->free!=0 )
        vtable->free((Twid_lvuBase*)self);
    if ( (self->lvu_options&LVUOPT_Unit)!=0 ) {
        if ( (self->lvu_options&LVUOPT_UnitLab)==LVUOPT_UnitLab )
            lvu_unitLabFree(_self);
        else if ( (self->lvu_options&LVUOPT_UnitDDL)==LVUOPT_UnitDDL )
            lvu_unitStdFree( _self );
        else if ( (self->lvu_options&LVUOPT_UnitBtn)==LVUOPT_UnitBtn )
            lvu_unitBtnFree( _self );
        else
            TEMP_spe_printf("unexpected option 0x08x",self->lvu_options);
    }
    free( self );
}

/*======================================================================*/
/*= get and change values and control                                  =*/

extern CstrBrief wid_lvu_getStr(Cwid_lvuBase* self)
{
    CstrBrief str;
    DBG0_OUTE("lvu=%p",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->getText!=0 )
             str = vtable->getText((Twid_lvuBase*)self);
        else {
            str = NULL;
            TEMP_spe_printf("invalid call (return %p)",NULL);
        }
    DBG1_OUTL("lvu=%p v=%s",self,str);
    return str;
}

extern void     wid_lvu_setStr(Twid_lvuBase* self, CstrBrief v)
{
    DBG0_OUTE("lvu=%p v=%f\n",self,v);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setText!=0 )
             vtable->setText((Twid_lvuBase*)self,v);
        else {
            TEMP_spe_printf("invalid call (set %s ignored)",v);
        }
    DBG1_OUTL("lvu=%p v=%f\n",self,v);
}

extern double wid_lvu_getNum10(Cwid_lvuBase* self)
{
    double value=0;
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setNum10!=0 )
             value = vtable->getNum10((Twid_lvuBase*)self);
        else {
            TEMP_spe_printf("invalid call (return %f)",value);
        }
    DBG1_OUTL("lvu=%p\n",self);
    return value;
}
extern double wid_lvu_getNum01(Cwid_lvuBase* self)
{
    double value=0;
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setNum10!=0 )
             value = vtable->getNum01((Twid_lvuBase*)self);
        else {
            TEMP_spe_printf("invalid call (return %f)",value);
        }
    DBG1_OUTL("lvu=%p\n",self);
    return value;
}
extern void   wid_lvu_setNum10(Twid_lvuBase* self, double v)
{
    DBG0_OUTE("lvu=%p v=%f\n",self,v);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setNum10!=0 )
             vtable->setNum10((Twid_lvuBase*)self,v);
        else {
            TEMP_spe_printf("invalid call (set %f ignored)",v);
        }
    DBG1_OUTL("lvu=%p v=%f\n",self,v);
}
extern void   wid_lvu_setNum01(Twid_lvuBase* self, double v2)
{
    DBG0_OUTE("lvu=%p v2=%f",self,v2);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setNum01!=0 )
             vtable->setNum01((Twid_lvuBase*)self,v2);
        else {
            TEMP_spe_printf("invalid call (set %f ignored)",v2);
        }
    DBG1_OUTL("lvu=%p v2=%f",self,v2);
}
extern void   wid_lvu_setNum11(Twid_lvuBase* self, double v, double v2)
{
    DBG0_OUTE("lvu=%p v=%f v2=%f\n",self,v,v2);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->setNum11!=0 )
             vtable->setNum11((Twid_lvuBase*)self,v,v2);
        else {
            TEMP_spe_printf("invalid call (set %f & %f ignored)",v,v2);
        }
    DBG1_OUTL("lvu=%p v=%p v2=%d\n",self,v,v2);
}

extern void   wid_lvu_reset   (Twid_lvuBase* self)
{ 
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->reset!=0 )
             vtable->reset((Twid_lvuBase*)self);
        else {
            TEMP_spe_printf("invalid call (ignored)",0);
        }
    DBG1_OUTL("lvu=%p\n",self);
}
extern void   wid_lvu_clear   (Twid_lvuBase* self)
{
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->clear!=0 )
             vtable->clear((Twid_lvuBase*)self);
        else {
            TEMP_spe_printf("invalid call (ignored)",0);
        }
    DBG1_OUTL("lvu=%p\n",self);
}
extern void   wid_lvu_unshow (Twid_lvuBase* self)
{
    Tegs_widget w;
    DBG0_OUTE("lvu=%p\n",self);
        if ( self->lvu_labEGS ) gui_widegs_unshow(self->lvu_labEGS);
        if ( self->lvu_valEGS ) gui_widegs_unshow(self->lvu_valEGS);
        if ( self->lvu_uniEGS ) gui_widegs_unshow(self->lvu_uniEGS);
    DBG1_OUTL("lvu=%p\n",self);
}
extern void   wid_lvu_show  (Twid_lvuBase* self)
{
    Tegs_widget w;
    DBG0_OUTE("lvu=%p\n",self);
        if ( self->lvu_labEGS ) gui_widegs_show(self->lvu_labEGS);
        if ( self->lvu_valEGS ) gui_widegs_show(self->lvu_valEGS);
        if ( self->lvu_uniEGS ) gui_widegs_show(self->lvu_uniEGS);
    DBG1_OUTL("lvu=%p\n",self);
}
extern void   wid_lvu_disable (Twid_lvuBase* self)
{
    Tegs_widget w;
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->disable!=0 ) {
             vtable->disable((Twid_lvuBase*)self);
        } else if ( vtable->widEGS!=0 ) {
             w = vtable->widEGS((Twid_lvuBase*)self);
             gui_widegs_disable(w);
        } else {
            TEMP_spe_printf("invalid call (ignored)",0);
        }
    DBG1_OUTL("lvu=%p\n",self);
}
extern void   wid_lvu_enable  (Twid_lvuBase* self)
{
    Tegs_widget w;
    DBG0_OUTE("lvu=%p\n",self);
        Tlvu_valVtable* vtable = self->lvu_valVtable;
        if ( vtable->enable!=0 )
             vtable->enable((Twid_lvuBase*)self);
        else if ( vtable->widEGS!=0 ) {
             w = vtable->widEGS((Twid_lvuBase*)self);
             gui_widegs_enable(w);
        } else {
            TEMP_spe_printf("invalid call (ignored)",0);
        }
    DBG1_OUTL("lvu=%p\n",self);
}
 
/*======================================================================*/
