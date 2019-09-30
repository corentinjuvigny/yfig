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
 *      $Id: wid-grp.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-grp.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Composite widget for input/output of 2d points.                    =*/
/*=                                                                    =*/
/*= See the wid.h headers for documentation.                           =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-lvu.h"

#include "tools/math-macros.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= Interface to Twid_any (not the right place)                        =*/

extern void wid_any_delete(Twid_any*any) {
    if ( any!=0 ) {
        Twid_anyVtable* vtable;
        for ( vtable=any->wa_vtable ; vtable!=0 ; vtable=vtable->bcvtable )
            if (vtable->free ) vtable->free( any );
        tool_free( any );
    }
}

/*======================================================================*/
/*= Interface                                                          =*/

extern Twid_grp* wid_grp_newEmpty   (Twid_anyVtable*vt) { Twid_grp*self=tool_malloc_0(vt->bytesize); self->wa_vtable=vt; return self; }
extern Twid_grp* wid_grp_newByBox   (Twid_anyVtable*vt, Twid_box*box) { Twid_grp* self=wid_grp_newEmpty(vt); self->wa_vgui = vgui_dw_getWidEGS(box); return self; } 

#define _grpvtable(grp) grp->wa_vtable->grpvtable
extern TwidEGS   wid_grp_lioEGS(Cwid_grp* grp) { return _grpvtable(grp)->lioEGS!=0 ? _grpvtable(grp)->lioEGS(grp) : WIDEGS_none; }
extern TwidEGS   wid_grp_xioEGS(Cwid_grp* grp) { return _grpvtable(grp)->xioEGS!=0 ? _grpvtable(grp)->xioEGS(grp) : WIDEGS_none; }
extern TwidEGS   wid_grp_yioEGS(Cwid_grp* grp) { return _grpvtable(grp)->yioEGS!=0 ? _grpvtable(grp)->yioEGS(grp) : WIDEGS_none; }
extern TwidEGS   wid_grp_uioEGS(Cwid_grp* grp) { return _grpvtable(grp)->uioEGS!=0 ? _grpvtable(grp)->uioEGS(grp) : WIDEGS_none; }

extern void      wid_grp_chgUsrUnit (Twid_grp*grp, int uu) { if ( _grpvtable(grp)->chgUsrUnit!=0 ) _grpvtable(grp)->chgUsrUnit(grp,uu); } 

extern void      wid_grp_clear    (Twid_grp*grp) { if ( _grpvtable(grp)->clear  !=0 )  _grpvtable(grp)->clear  (grp); }
extern void      wid_grp_enable   (Twid_grp*grp) { if ( _grpvtable(grp)->enable !=0 )  _grpvtable(grp)->enable (grp); }
extern void      wid_grp_disable  (Twid_grp*grp) { if ( _grpvtable(grp)->disable!=0 )  _grpvtable(grp)->disable(grp); }

#define RETV_CALL_VT0(grp,fct)   TEMP_asf_printf( _grpvtable(grp)->fct==0, "Invalid call: vt->%s==0",#fct); return _grpvtable(grp)->fct(grp)
#define RETV_CALL_VT1(grp,fct,p) TEMP_asf_printf( _grpvtable(grp)->fct==0, "Invalid call: vt->%s==0",#fct); return _grpvtable(grp)->fct(grp,p)
#define VOID_CALL_VT0(grp,fct)   TEMP_asf_printf( _grpvtable(grp)->fct==0, "Invalid call: vt->%s==0",#fct);        _grpvtable(grp)->fct(grp)
#define VOID_CALL_VT1(grp,fct,p) TEMP_asf_printf( _grpvtable(grp)->fct==0, "Invalid call: vt->%s==0",#fct);        _grpvtable(grp)->fct(grp,p)

extern void      wid_grp_updMin   (Twid_grp* grp, double v)    { VOID_CALL_VT1(grp,updMin   ,v ); }
extern void      wid_grp_updMax   (Twid_grp* grp, double v)    { VOID_CALL_VT1(grp,updMax   ,v ); }
extern double    wid_grp_getDble10(Twid_grp*grp)               { RETV_CALL_VT0(grp,getDble10   ); }
extern double    wid_grp_getDble01(Twid_grp*grp)               { RETV_CALL_VT0(grp,getDble01   ); }
extern Tdble2    wid_grp_getDblePt(Twid_grp*grp)               { RETV_CALL_VT0(grp,getDblePt   ); }
extern void      wid_grp_setDble10(Twid_grp*grp, double v)     { VOID_CALL_VT1(grp,setDble10,v ); }
extern void      wid_grp_setDble01(Twid_grp*grp, double v)     { VOID_CALL_VT1(grp,setDble01,v ); }
extern void      wid_grp_setDblePt(Twid_grp*grp, Tdble2 pt)    { VOID_CALL_VT1(grp,setDblePt,pt); }

/*======================================================================*/
/*= Interface to Twid_box (not the right place)                        =*/

struct _Twid_boxVtable {
    CstrStatic  title;
};

Twid_boxVtable wid_boxVtable_tbx = {
    .title = NULL,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

#include "vgui/vdw-title.h"
#include "vgui/wid-buttonPanel.h"
static void box_vt_free(Twid_any*any)
{
    Twid_box* self = (void*)any;
    gui_dw_free(self);
    if ( self->D_title ) vdw_title_delete( self->D_title );
    if ( self->D_top!=0 ) {
        Tvgui_dwTop* top = self->D_top;
        tool_free( top->dt_title );
        if ( top->dt_ctlButtons ) wid_buttonPanel_free( top->dt_ctlButtons );
        free( top );
    }
}
static void tbx_vt_free(Twid_any*any) { box_vt_free(any); }

Twid_anyVtable wid_anyVtable_tbx = {
    .bytesize   = sizeof(Twid_box),
    .bcvtable   = NULL,
    .free       = tbx_vt_free,
    .getMainWid = NULL,
    .getBoxWid  = NULL,
    .getDialWid = NULL,
    .widvtable  = NULL,
    .boxvtable  = &wid_boxVtable_tbx,
    .grpvtable  = NULL,
    .usrvtable  = NULL,
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_box* WIDBOX(void*x) { Twid_box*box=x; if ( box==0 ) return box; TEMP_asf_printf(box!=0 && box->wa_vtable->boxvtable==0, "%p is not not a box",x); return box; }
extern TwidEGS   wid_box_widEGS(Twid_box*box)                   { return vgui_dw_getWidEGS(box); }

Inline void      wid_box_init(Twid_box*self, Twid_anyVtable*vt, Tvgui_mw*vmw ) {
    self->wa_vtable = vt;
    self->wa_vgui   = vgui_mw_getVGui(vmw);
    self->wb_vmw    = vmw;
}
extern Twid_box* wid_tbx_newBtnTva(Twid_anyVtable*vt,Tvgui_mw* vmw,
                             Twid_status btn, TstrBrief fmt, va_list ap)
{
    Twid_box* self;
    TstrEaten title = tool_aprintf(fmt,ap);
    self =  vgui_dw_newTop( vt->bytesize, vmw, title, btn);
    wid_box_init(self,vt,vmw);
    return self;
}
extern Twid_box* wid_tbx_new    (Twid_anyVtable* vt,Tvgui_mw* vmw)
{
    Twid_box* self;
    self =  vgui_dw_newTop(vt->bytesize, vmw, NULL, TBXSB_None);
    wid_box_init(self,vt,vmw);
    return self;
}
extern Twid_box* wid_tbx_newBtn (Twid_anyVtable* vt,Tvgui_mw* vmw, Twid_status btn)
{
    Twid_box* self;
    self =  vgui_dw_newTop( vt->bytesize, vmw, NULL, btn);
    wid_box_init(self,vt,vmw);
    return self;
}
extern Twid_box* wid_tbx_newT   (Twid_anyVtable* vt,Tvgui_mw* vmw, TstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    Twid_box* self=wid_tbx_newBtnTva(vt,vmw,TBXSB_None,fmt,ap);
    va_end(ap);
    return self;
}
extern Twid_box* wid_tbx_newBtnT(Twid_anyVtable* vt,Tvgui_mw* vmw,
                                 Twid_status btn, TstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    Twid_box* self=wid_tbx_newBtnTva(vt,vmw,btn,fmt,ap);
    va_end(ap);
    return self;
}

/*======================================================================*/
