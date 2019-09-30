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
 *      $Id: ydw-colors.c 296 2018-07-12 09:46:36Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-colors.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub_dialog widget dedicated to color management.                   =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "ydw/ydw.h"
#include "vgui/vgui.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-egs.h"
#include "vgui/vdw-color.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "tools/color.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Tydw_colors Tydw_colors;
typedef const   Tydw_colors Cydw_colors;

struct _Tydw_colors {
    Tvgui_dw          any;
    Tfig_colors*      dw_figcolors;

    Tvdw_colorSel*    dw_predefined; // panel of predefined colors
    Tvdw_colorSel*    dw_private;    // panel of private colors

    Tvdw_colorSel_cb  dw_cb;
    void*             dw_ud;
};

/*======================================================================*/

// In the ud color dialog, the c color of the cs color panel
// has been selected.
// c==0 means the none button.
static void ydw_colors_cb(Tvdw_colorSel*cs, Ccolor*c, void*ud);

/*======================================================================*/
/*= Interface                                                          =*/

static Tvgui_dw* ydw_colors_nbfPriv(Tvgui_dw* vdw, Tyfig_mw* ymw,
                           Ccolor*ic, Tvdw_colorSel_cb cb, void*ud,
                           int withNone, CstrBrief none)
{
    Tydw_colors* self = (Tydw_colors*) vgui_dw_newSub(sizeof(*self), vdw,NULL);

    self->dw_figcolors = &yfig_cursce_fig(ymw)->colors;
    self->dw_cb        = cb;
    self->dw_ud        = ud;

    /*  predefined colors */
    CcolorSet* cset;
    cset = self->dw_figcolors->predefinedColors;
#if 1
    if ( withNone )
        self->dw_predefined = vdwD_colorsel_newWN(SelfA
                            ,tool_strdup("Predefined colors")
                            ,ydw_colors_cb,self
                            ,NULL
                            ,cset->eles,cset->eleNb
                            ,none);
    else
        self->dw_predefined = vdwD_colorsel_new(SelfA
                            ,tool_strdup("Predefined colors")
                            ,ydw_colors_cb,self
                            ,cset->eles,cset->eleNb);
    vdw_colorsel_set(self->dw_predefined,
                     YDW_CLR_NbCol,(cset->eleNb+YDW_CLR_NbCol-1)/YDW_CLR_NbCol,
                     YDW_CLR_BtnDx,YDW_CLR_BtnDy);
    vdw_colorsel_dbf(self->dw_predefined,NULL);
#else
    if ( withNone )
        self->dw_predefined = vdwD_colorsel_nbfWN(SelfA
                            ,tool_strdup("Predefined colors")
                            ,cset->eles,cset->eleNb
                            ,ydw_colors_cb,self
                            ,YDW_CLR_NbCol,0
                            ,2,none);
    else
        self->dw_predefined = vdwD_colorsel_nbf(SelfA
                            ,tool_strdup("Predefined colors")
                            ,cset->eles,cset->eleNb
                            ,ydw_colors_cb,self
                            ,YDW_CLR_NbCol,0);
#endif

    /*  figure colors */
    cset = self->dw_figcolors->privateColors;
    self->dw_private    = vdwD_colorsel_nbf(SelfA
                            ,tool_strdup("Figure colors")
                            ,cset ? cset->eles  : NULL
                            ,cset ? cset->eleNb : 0
                            ,ydw_colors_cb,self
                            ,YDW_CLR_NbCol,2);

    /* layout */
    vgui_dw_finishIC(&self->any,
#if 0
        0,             self->dw_predefined->dw_widEGS,
        0|VDWLF_SEP1,  self->dw_private->dw_widEGS,
#else
        VDWLF_EH,             self->dw_predefined->dw_widEGS,
        VDWLF_EH|VDWLF_SEP1,  self->dw_private->dw_widEGS,
#endif
    -1);

    return SelfA;
}

extern Tvgui_dw* ydw_colors_nbf(Tvgui_dw* vdw, Tyfig_mw* ymw,
                           Ccolor*ic, Tvdw_colorSel_cb cb, void*ud)
{ return ydw_colors_nbfPriv(vdw,ymw,ic,cb,ud,0,NULL); }

extern Tvgui_dw* ydw_colors_nbfWN(Tvgui_dw* vdw, Tyfig_mw* ymw,
             Ccolor*ic, Tvdw_colorSel_cb cb, void*ud, CstrBrief none)
{ return ydw_colors_nbfPriv(vdw,ymw,ic,cb,ud,1, none?none:"none"); }

extern void    ydw_colors_free (Tvgui_dw* vdw)
{
    Tydw_colors* self  = (Tydw_colors*)vdw;
    vdw_colorsel_free(  self->dw_predefined );
    vdw_colorsel_free(  self->dw_private );
    vgui_dw_free( vdw );
}

extern Ccolor* ydw_colors_color(Tvgui_dw* vdw);

extern void     ydw_colors_addClr(Tvgui_dw* vdw, Ccolor*c)
{
    Tydw_colors* self  = (Tydw_colors*)vdw;
    vdw_colorsel_addC(self->dw_private,c,1);
}
extern void     ydw_colors_delClr(Tvgui_dw* vdw, Ccolor*c);

/*======================================================================*/
/*= Callback                                                           =*/

static void ydw_colors_cb(Tvdw_colorSel*cs, Ccolor*c, void*ud)
{
    Tydw_colors*      self  = ud;

    DBG0_CdwE("c=%s", DBG_Color2str(c));
    //ydw_color_cancelAction(self);
    //ydw_color_showAct(self,c);
    if ( self->dw_cb ) 
        self->dw_cb(cs, c, self->dw_ud);
    DBG1_CdwL("c=%s", DBG_Color2str(c));
}

/*======================================================================*/
