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
 *      $Id: vdw-title.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-title.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog defining the title of a dialog.                         =*/
/*=                                                                    =*/
/*= See the vgui/vdw-title.h header for documentation.                 =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-names.h"
#include "vgui/wid-button.h"
#include "vgui/vdw-title.h"

/*======================================================================*/
/*= Constructor and destructor                                         =*/

extern Tvgui_dw* vdw_title_nbx(Tvgui_dw* vdw, CstrStatic wcn, TstrEaten str)
{
    Tvgui*      gui  = vgui_dw_getVGui(vdw);
    Tvdw_title* self = tool_malloc_0( sizeof(*self) );
    Cstr        wn   = wcn!=0        ? wcn 
                     : vdw->D_wcn!=0 ? vdw->D_wcn : wn_dialSubTitle;

    self->dw_vmw     = vdw->D_vmw;
    self->title      = str;
    self->dw_pDialog = vdw;
    self->dw_widEGS  = gui_dw_create(&self->any,wn,
            0, -1, -1, // bd, dx,dy
            1          // resizable
    );
    self->label = gui_label_nbf(gui,self->wd_widEGS, wn_dialSub,
            0,-1,-1,  EGSCOLOR_default,EGSCOLOR_default,
            self->title);

    return &self->any;
}

#define WIDNULL WID_VDW_WIDNULL
extern void      vdw_title_finish(Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw;
    vgui_dw_finishIL(SelfA
            ,0              ,self->leftFUF ? self->leftFUF->but_widEGS : NULL
            ,VDWLF_SEP1     ,self->label
            ,VDWLF_SEP2     ,self->extra
            ,VDWLF_SEP2     ,self->rightFUF ? self->rightFUF->but_widEGS : NULL
    ,-1);
}

extern void vdw_title_delete(Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw;
    tool_free( self->title );
    wid_button_delete( self->leftFUF );
    wid_button_delete( self->rightFUF );
    vgui_dw_free( vdw );
}

/*======================================================================*/
/*= Configuring title dialog (caution vdw is the parent widget)        =*/

// fold/unfold button
extern void vgui_dw_titleAddFufLabL(Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    self->leftFUF = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    widD_button_fuf_addWidToIgn(self->leftFUF,
            vdw->D_widEGS,self->dw_widEGS,NULL);
}

extern void vgui_dw_titleAddFufLabR(Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    self->rightFUF = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    widD_button_fuf_addWidToIgn(self->rightFUF,
            vdw->D_widEGS,self->dw_widEGS,NULL);
}

extern void vgui_dw_titleAddFufPxmL(Tvgui_dw* vdw);
extern void vgui_dw_titleAddFufPxmR(Tvgui_dw* vdw);
extern void vgui_dw_titleFold      (Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    Twid_button* fuf = self->leftFUF ? self->leftFUF : self->rightFUF;
    TEMP_asf_printf(fuf==NULL,"Dialog has no fuf button",0);
    widD_button_fuf_setFolded(fuf);
}

extern void vgui_dw_titleUnFold    (Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    Twid_button* fuf = self->leftFUF ? self->leftFUF : self->rightFUF;
    TEMP_asf_printf(fuf==NULL,"Dialog has no fuf button",0);
    widD_button_fuf_setUnFolded(fuf);
}

// extra label at the left hand of the title
extern void vgui_dw_titleAddLabTxt (Tvgui_dw* vdw,
                                    CstrBrief text, CstrBrief width)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    self->extra = widD_label_nbf(SelfA, text, width);
}

extern void vgui_dw_titleAddLabBox (Tvgui_dw* vdw,
                                    int bdw, int dx, int dy);
extern void vgui_dw_titleLabSetText(Tvgui_dw* vdw, CstrBrief text)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    TEMP_asf_printf(self->extra==NULL,"title dialog has no extra label",0);
    wid_label_setText(self->extra, text);
}
extern void vgui_dw_titleLabSetBgClr(Tvgui_dw* vdw, Ccolor* c)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    TEMP_asf_printf(self->extra==NULL,"title dialog has no extra label",0);
    wid_label_setBgColor(self->extra, c);
}
extern Tegs_widget vgui_dw_titleLab(Tvgui_dw* vdw)
{
    Tvdw_title* self = (Tvdw_title*)vdw->D_title;
    TEMP_asf_printf(self==NULL,"Dialog has no title",0);
    TEMP_asf_printf(self->extra==NULL,"title dialog has no extra label",0);
    return self->extra;
}

/*======================================================================*/
