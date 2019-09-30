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
 *      $Id: vgui-dw.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-dw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets.                       =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "vgui-config.h"

#include "vgui/vgui-dw.h"
#include "vgui/vgui-mw.h"
#include "vgui/vgui-names.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/vdw-title.h"

#include "tools/strings.h"

/*======================================================================*/
/*= abstract accessors                                                 =*/

extern Tvgui*      vgui_dw_getVGui  (Cvgui_dw*vdw) { return vdw->D_gui; }
extern Tvgui_mw*   vgui_dw_getMw    (Cvgui_dw*vdw) { return vdw->D_vmw; }
extern Tegs_widget vgui_dw_getWidEGS(Cvgui_dw*vdw) { return vdw->D_widEGS; }
extern Tmsgdrv*    vgui_dw_getMsgDrv(Cvgui_dw*vdw) {
    Tvgui_dwTop* top = vgui_dw_getTopData(vdw);
    if ( top!=0 && top->dt_msgdrv!=0 ) return top->dt_msgdrv;
    return vgui_mw_getMsgDrv( vdw->D_vmw );
}

/*======================================================================*/
/*= Constructors and destructor                                        =*/

static Tvgui_dw* vgui_dw_new0(int bytesz)
{
    Tvgui_dw* d = tool_malloc_0( bytesz );
    d->D_hspace  = -1;
    d->D_vspace  = -1;
    d->D_hmargin = -1;
    d->D_vmargin = -1;
    return d;
}

extern Tvgui_dw* vgui_dw_newTop(int bytesz, Tvgui_mw*vmw,
                                TstrEaten title, Twid_dialStatus bs)
{
    Tvgui_dw* d = vgui_dw_new0(bytesz);
    d->D_topDialog = 1;

    d->D_vmw     = vmw;
    d->D_top     = tool_malloc_0( sizeof(*d->top) );
    d->D_wcn     = wn_dialTop;
    d->D_widEGS  = gui_dw_create(d, d->D_wcn,
            0, -1, -1,  // bd, x,y
            1           // resizable (not used in gui_dw_create)
    );
    vgui_dw_titleUpdate(d,title);

    d->top->dt_ctlButtons = widD_buttonPanel_newR1Cn(d,0,0);
    d->top->dt_ctlButtons->bp_buttonHVsep = 12;
    d->top->dt_ctlButtons->bp_adjustR1C1  =  1;
    if ( bs==WID_DIALSTATUS_CancelX ) {
        d->top->dt_wmX = 1;
        d->top->dt_buttsMaskOrg |= WID_DIALSTATUS_Cancel;
    } else
        vgui_dw_addCtlButtsDN(d,bs);

    return d;
}

extern Tvgui_dw* vgui_dw_newSub(int bytesz, Tvgui_dw*vdm,
                                TstrEaten title)
{
    Tvgui_dw* self = vgui_dw_new0(bytesz);

    self->D_vmw     = vdm->D_vmw;
    self->D_pDialog = vdm;
    self->D_wcn     = wn_dialSub;
    self->D_widEGS  = gui_dw_create(self, self->D_wcn,
            2, -1, -1, // bd, x,y
            0          // resizable (not used in gui_dw_create)
    );
    if ( ! TOOL_strIsEmpty(title) )
        self->D_title = vdw_title_nbx(self,NULL,title);

    return self;
}

extern Tvgui_dw* vguiM_dw_newTop(int bytesz, Tvgui_mw*vmw,
                                   Tegs_widget pw)
{
    Tvgui_dw* d = vgui_dw_new0(bytesz);
    if ( pw==0 )
        pw = vmw->MW_widEGS;

    d->D_vmw     = vmw;
    d->D_wcn     = wn_mainTop;
    d->D_top     = tool_malloc_0( sizeof(*d->top) );
    d->D_widEGS  = gui_dw_create2(d, pw, d->D_wcn,
            0, -1, -1,  // bd, x,y
            //1, 150, 150,  // bd, x,y
            1           // resizable (not used in gui_dw_create)
    );
    return d;
}

extern void vgui_dw_free(Tvgui_dw* self)
{
    gui_dw_free(self);
    if ( self->D_title ) vdw_title_delete( self->D_title );
    if ( self->D_top!=0 ) {
        Tvgui_dwTop* top = self->D_top;
        tool_free( top->dt_title );
        if ( top->dt_ctlButtons ) wid_buttonPanel_free( top->dt_ctlButtons );
        free( top );
    }
    free(self);
}

/*======================================================================*/
/*= Layout constructors                                                =*/

// done in vgui-dw-layout.c

/*======================================================================*/
/*= Configuring Title dialog                                           =*/

extern void vgui_dw_titleSet(Tvgui_dw* vdw, CstrBrief wcn, CstrBrief str)
{
    if ( TOOL_strIsEmpty(str) ) return;
    if ( vdw->D_title==0 ) {
        vdw->D_title = vdw_title_nbx(vdw,wcn,tool_strdup(str));
    } else {
        Tvdw_title* self = (Tvdw_title*)vdw->D_title;
        tool_strchgClone(&self->title,str);
    }
}

/*======================================================================*/
/*= Running dialog                                                     =*/

extern Twid_dialStatus  vgui_dw_run (Tvgui_dw*vdw)
{ 
    int status;
    Tvgui_dw* vdwTop = vgui_dw_getTopDialog(vdw);
    vdwTop->top->dt_buttsMaskCur = vdwTop->top->dt_buttsMaskOrg;
    vdwTop->top->dt_buttSelected = 0;
    if ( vdwTop->top->vt_modal_run ) {
        status  = vdwTop->top->vt_modal_run(vdwTop);
    } else {
        status = vdwTop->top->vt_modal_run2(vdwTop,NULL,0,0);
    }
    return status;
}

extern Twid_dialStatus  vgui_dw_run2(Tvgui_dw*vdw,
                                     Tegs_widget inw, int x, int y)
{ 
    Tvgui_dw* vdwTop = vgui_dw_getTopDialog(vdw);
    vdwTop->top->dt_buttsMaskCur = vdwTop->top->dt_buttsMaskOrg;
    vdwTop->top->dt_buttSelected = 0;
    return vdwTop->top->vt_modal_run2(vdwTop,inw,x,y);
}

extern void             vgui_dw_breakloop(Tvgui_dw*vdw, Twid_dialStatus s)
{
    Tvgui_dw* vdwTop = vgui_dw_getTopDialog(vdw);
    if ( (vdwTop->top->dt_buttsMaskCur&s)!=0 ) {
        vdwTop->top->dt_buttSelected = s;
        if ( vdwTop->top->vt_modal_breakloop ) {
            vdwTop->top->vt_modal_breakloop(vdwTop,s);
        }
    }
}

extern void vgui_dw_start(Tvgui_dw*vdw)
{
    Tvgui_dw* vdwTop = vgui_dw_getTopDialog(vdw);
    if ( vdwTop->top->vt_modeless_start ) {
        vdwTop->top->dt_buttsMaskCur = vdwTop->top->dt_buttsMaskOrg;
        vdwTop->top->vt_modeless_start(vdwTop);
    }
}

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/

extern void      vgui_dw_titleUpdate(Tvgui_dw* d, TstrEaten t)
{
    Tvgui_dwTop* top = d->top;
    TEMP_asf_printf(top==0,"not a top dialog",0);

    char pn[100]; strcpy(pn,PACKAGE); tool_strtoupper(pn);
    TstrEaten mess;
    if ( t!=0 ) {
        tool_free(top->dt_title); top->dt_title = t;
        mess = tool_aprintf("%s-dialog: %s",pn,t);
    } else if ( top->dt_title!=0 ) {
        mess = tool_aprintf("%s-dialog: %s",pn,top->dt_title);
    } else
        mess = tool_aprintf("%s-dialog",pn);
    gui_dw_setTitle(d,mess);
    free( mess );
}

/*======================================================================*/
/*= Top dialog features                                                =*/

extern Tvgui_dw*    vgui_dw_getTopDialog(Tvgui_dw*d)
{
    Tvgui_dw* top;
    for ( top=d ; top->pDialog!=0 ; top=top->pDialog );
    return top;
}

extern Tvgui_dwTop* vgui_dw_getTopData  (Cvgui_dw*d)
{
    Cvgui_dw* top;
    for ( top=d ; top->pDialog!=0 ; top=top->pDialog );
    return top->top;
}

extern void vgui_dw_ctlbutton_cb(Tvgui_dw*vdw, int uid, void*ud)
{
    Tvgui_dwStatus s   = uid;
    Tvgui_dwTop*   top = vdw->top;
    TEMP_asf_printf(top==0,"no top data, probably not a top dialog",0);
    if ( top->dt_cb ) {
        // modeless dialog 
        top->dt_cb(vdw,s, top->dt_cb_ud);
    } else {
        // modal dialog button
        vgui_dw_breakloop(vdw,s);
    }
}

extern void vgui_dw_ctlbutton_cb_sub(Tvgui_dw*vdw, int uid, void*ud)
{ vgui_dw_ctlbutton_cb( vgui_dw_getTopDialog(vdw), uid, ud); }

static Twid_button* vgui_dw_ctlbutton_new( Tvgui_dw*vdw,
        CstrBrief label, CstrBrief width, Tvgui_dwStatus b)
{
    Twid_button* button = widD_button_std_newLab3(vdw,
            NULL, // help text
            vgui_dw_ctlbutton_cb, NULL,
            NULL, // free function
            label,width);
    button->but_uid = b;
    return button;
}

extern void vgui_dw_addBtnCtl(Tvgui_dw*vdw, Twid_dialStatus b,
                                                CstrBrief l)
{
    Tvgui_dw*    topdw = vgui_dw_getTopDialog(vdw);
    Tvgui_dwTop* top   = vgui_dw_getTopData(vdw);

    // special case of Cancel and CancelX
    if ( b==WID_DIALSTATUS_CancelX ) {
        top->dt_wmX = 1;
        if ( (top->dt_buttsMaskOrg&WID_DIALSTATUS_Cancel)==0 ) {
            b = WID_DIALSTATUS_Cancel;
        } else // Cancel button is alredy set
            return;
    }

    if ( l==0 ) l = vgui_dw_status2buttLab(b);

    Twid_button* button = vgui_dw_ctlbutton_new(topdw,l,0,b);
    wid_buttonPanel_addButton(top->dt_ctlButtons, button);
    top->dt_buttsMaskOrg |= b;
}

extern void vgui_dw_addCtlButtsDN(Tvgui_dw*d, Twid_dialStatus bs)
{
    int i; Twid_dialStatus b;

    for (i=8,b=WID_DIALSTATUS_First ; b<WID_DIALSTATUS_Last ; i+=1,b<<=1 ) {
        if ( (b&bs)!=0 )
            vgui_dw_addBtnCtl (d, b, 0);
    }
}

extern void vgui_dw_addCtlButts  (Tvgui_dw*d, ...)
{
    va_list ap;
    va_start(ap,d);
    vgui_dw_addCtlButtsVa(d,ap);
    va_end(ap);
}

extern void vgui_dw_addCtlButtsVa(Tvgui_dw*d, va_list ap)
{
    Twid_dialStatus b;
    for ( b=va_arg(ap,Twid_dialStatus) ; b!=WID_DIALSTATUS_None ;
          b=va_arg(ap,Twid_dialStatus) ) {
        CstrStatic l = va_arg(ap,CstrStatic);
        vgui_dw_addBtnCtl(d,b,l);
    }
}

extern Twid_button* vgui_dw_addBtnStdLab (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        CstrBrief text, CstrBrief width)
{
    Tvgui_dwTop* top = vgui_dw_getTopData(vdw);
    Twid_button* btn = widD_button_std_newLab(vdw,
            helpText,cb,NULL,NULL,text,width);
    wid_buttonPanel_addButton(top->dt_ctlButtons, btn);
    return btn;
}

extern Twid_button* vgui_dw_addBtnStdPxm (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        int dx, int dy, TpixmapEGS pm);

extern Twid_button* vgui_dw_addBtnOnOffLab (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        CstrBrief off, CstrBrief on)
{
    Tvgui_dwTop* top = vgui_dw_getTopData(vdw);
    Twid_button* btn = widD_button_onoff_newLab(vdw,
            helpText,cb,NULL,NULL,off,on);
    wid_buttonPanel_addButton(top->dt_ctlButtons, btn);
    return btn;
}

extern Twid_button* vgui_dw_addBtnOnOffPxm (Tvgui_dw*vdw, 
                        CstrBrief helpText,
                        Twid_button_cbDw cb,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on);

#if 0
/*======================================================================*/
/*= A Tvgui_dw set                                                     =*/

extern Tvgui_dwSet*  vgui_dwset_new  (int n)
{
    Tvgui_dwSet* ret = malloc(sizeof(*ret));
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? malloc(sizeof(*ret->eles)*n) : 0;
    return ret;
}

extern void          vgui_dwset_free (Tvgui_dwSet* s)
{ if ( s->eles ) free(s->eles); free(s); }

extern void          vgui_dwset_add  (Tvgui_dwSet* s, Tvgui_dw* p)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    s->eles[s->eleNb] = p;
    s->eleNb += 1;
}

extern Tvgui_dw*     vgui_dwset_first(Cvgui_dwSet* s)
{ return s->eleNb!=0 ? s->eles[0]: 0; }

extern Tvgui_dw*     vgui_dwset_last (Cvgui_dwSet* s)
{ return s->eleNb!=0 ? s->eles[s->eleNb-1] : 0; }
#endif
/*======================================================================*/
