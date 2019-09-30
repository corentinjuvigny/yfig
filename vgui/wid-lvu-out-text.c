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
 *      $Id: wid-lvu-out-text.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-out-text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item for                               =*/
/*    - text output: It is based on a label widget.                    =*/
/*    - color label: It is based on a label widget.                    =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/wid-egs.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_valText {
    Tlvu_valBase0 base;

    int       dx,dy;  // prefered size, 0 means adjust to width
    TstrEaten width;  // size label to contain width

    /* initial value */
    TstrEaten initValue;

    /* current value */
    TstrEaten currValue;

    /* implementation */
    union {
        Tegs_widget label;
        Tegs_widget slout;
        Twid_slin   slin;
    } widget;

    /* extra fields for color label */
    //Tcolor*   vguiBg;  // default bg color of the widget
    Ccolor*   initBg;  // bg color set by the constructor
    Ccolor*   currBg;  // current bg color of label.
};

/*======================================================================*/
/*= Shared utilities.                                                  =*/

static CstrBrief lvuvt_getText(Twid_lvuBase* lvu)
{ Tlvu_valText* self = lvu->lvu_valText; return self->currValue; }

static void lvuvt_setText0(Twid_lvuBase* lvu, CstrBrief v)
{
    Tlvu_valText* self = lvu->lvu_valText;
    tool_free( self->currValue );
    self->currValue = tool_strdup( v );
}

#define STD_INIT(self,vt,iv,width) \
    self->base.vtable = &vt; \
    self->initValue = tool_strdup( iv ); \
    self->width = tool_strdup( width ); \
    self->currValue = tool_strdup( iv )

/*======================================================================*/
/*= Implementation of text output with a label widget                  =*/

static void lvuvt_labelDbf(Twid_lvuBase* lvu)
{
    Tlvu_valText* self = lvu->lvu_valText;
    self->widget.label = widD_label_nbf(lvu->lvu_vdw,
                self->currValue, self->width);
}

static void lvuvt_labelFree(Twid_lvuBase* lvu)
{
    Tlvu_valText* self = lvu->lvu_valText;
    tool_free( self->width );
    tool_free( self->initValue );
    tool_free( self->currValue );
    free( self );
}

static Tegs_widget lvuvt_labelWidEGS(Twid_lvuBase* lvu)
    { return lvu->lvu_valText->widget.label; }

static void lvuvt_labelSetText(Twid_lvuBase* lvu, CstrBrief v)
{
    Tlvu_valText* self = lvu->lvu_valText;
    // update local storage
    lvuvt_setText0(lvu,v);
    // update widget
    wid_label_setTextSS(self->widget.label,self->currValue);
}

static void lvuvt_labelSetNum10(Twid_lvuBase* lvu, double v)
{
    char buf[100];
    sprintf(buf,"%f",v);
    lvuvt_labelSetText(lvu,buf);
}

static Tlvu_valVtable lvuvt_labelVT = {
    .dbf      = lvuvt_labelDbf,
    .free     = lvuvt_labelFree,
    .widEGS   = lvuvt_labelWidEGS,
    .getText  = lvuvt_getText,
    .setText  = lvuvt_labelSetText,
    .setNum10 = lvuvt_labelSetNum10,
};

extern Tlvu_valBase* lvu_newOValTxtLab(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width)
{
    Tlvu_valText* self  = tool_malloc_0( sizeof(*self) );

    STD_INIT(self,lvuvt_labelVT,iv,width);
    return (Tlvu_valBase*)self;
}

/*======================================================================*/
/*= Implementation of output color label                               =*/

static void lvuvt_clrlabSetBgClr (Twid_lvuBase* lvu, Ccolor* c);

static void lvuvt_clrlabDbf(Twid_lvuBase* lvu)
{
    Tlvu_valText* self = lvu->lvu_valText;
    Tegs_widget label;
    if (  self->width==0 ) {
        label = widD_label_nbfBox(lvu->lvu_vdw, 0, self->dx, self->dy);
        if ( self->currValue!=0 )
            wid_label_setTextSS(label,self->currValue);
    } else {
        label = widD_label_nbf(lvu->lvu_vdw, 
                               self->currValue, self->width);
        self->dx = vgui_widegs_getDX(label);
        if ( self->dy==0 ) 
            self->dy = vgui_widegs_getDY(label);
        else
            wid_label_setSize(label,self->dx,self->dy);
    }
    self->widget.label = label;
    if ( self->initBg==0 ) {
        Tvgui* gui   = vgui_dw_getVGui(lvu->lvu_vdw);
        self->initBg = vgui_wn_getBgColor(gui,wn_dialLabel);
    }
    if ( self->currBg )
        wid_label_setBgColor(self->widget.label, self->initBg);
}

static Tegs_widget lvuvt_clrlabWidEGS(Twid_lvuBase* lvu)
    { return lvu->lvu_valText->widget.label; }

static void lvuvt_clrlabSetText(Twid_lvuBase* lvu, CstrBrief v)
{
    Tlvu_valText* self = lvu->lvu_valText;
    // update local storage
    lvuvt_setText0(lvu,v);
    // update widget
    wid_label_setTextSS(self->widget.label,self->currValue);
}

static void lvuvt_clrlabSetNum10(Twid_lvuBase* lvu, double v)
{
    char buf[100];
    sprintf(buf,"%f",v);
    lvuvt_clrlabSetText(lvu,buf);
}

static void lvuvt_clrlabReset(Twid_lvuBase* lvu)
{
    Tlvu_valText* self = lvu->lvu_valText;
    lvuvt_clrlabSetBgClr(lvu,self->initBg);
    lvuvt_clrlabSetText(lvu,self->initValue);
}

static void lvuvt_clrlabSetBgClr (Twid_lvuBase* lvu, Ccolor* c)
{
    Tlvu_valText* self = lvu->lvu_valText;
    self->currBg = c;
    wid_label_setBgColor(self->widget.label, self->currBg );
}

static void lvuvt_clrlabSetBgPxm (Twid_lvuBase* lvu, TpixmapEGS tile)
{
    Tlvu_valText* self = lvu->lvu_valText;
    wid_label_setBgPxm(self->widget.label, tile);
}

static void lvuvt_clrlabSetBgBtm (Twid_lvuBase* lvu, Cvgui_icon* icon)
{
    Tvgui*        gui  = vgui_dw_getVGui(lvu->lvu_vdw);
    Tlvu_valText* self = lvu->lvu_valText;
    Ccolor*       fg   = gui->vg_black;
    Ccolor*       bg   = self->currBg ? self->currBg : self->initBg;
    TpixmapEGS    pxm  = vgui_pixmap_create(gui,fg,bg,icon);
    wid_label_setBgPxm(self->widget.label,pxm);
    gui_pixmap_free(gui,pxm);
}

static Tlvu_valVtable lvuvt_clrlabVT = {
    .dbf      = lvuvt_clrlabDbf,
    .free     = lvuvt_labelFree,
    .widEGS   = lvuvt_clrlabWidEGS,
    .getText  = lvuvt_getText,
    .setText  = lvuvt_clrlabSetText,
    .setNum10 = lvuvt_clrlabSetNum10,
    .reset    = lvuvt_clrlabReset,
    .setBgClr = lvuvt_clrlabSetBgClr,
    .setBgPxm = lvuvt_clrlabSetBgPxm,
    .setBgBtm = lvuvt_clrlabSetBgBtm,
};

extern Tlvu_valBase* lvu_newOValClrLab(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width,
                        Ccolor*   ic, int dx, int dy)
{
    Tlvu_valText* self  = tool_malloc_0( sizeof(*self) );

    STD_INIT(self,lvuvt_clrlabVT,iv,width);
    self->initBg = ic;
    self->currBg = ic;
    self->dx = dx;
    self->dy = dy;
    return (Tlvu_valBase*)self;
}

/*======================================================================*/
