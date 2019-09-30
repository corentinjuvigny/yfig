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
 *      $Id: wid-lvu-in-text.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-in-text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item for text input                    =*/
/*= It is based on Twid_slin widget.                                   =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_ivalText {
    Tlvu_valBase0 base;

    // int       dx,dy;  // prefered size, 0 means adjust to width
    TstrEaten width;  // size slin to contain the width string

    /* initial value */
    TstrEaten initValue;

    /* current value */
    TstrEaten currValue;

    /* implementation */
    Twid_slin*slin;
};

/*======================================================================*/
/*= widget vtable                                                      =*/

static void lvuvt_ivtsli_cb_val(Twid_slin*slin, void*ud);
static void lvuvt_ivtsli_cb_chg(Twid_slin*slin, void*ud);

static void lvuvt_ivtsli_Dbf(Twid_lvuBase* _lvu)
{
    Twid_ilvu* lvu = (Twid_ilvu*)_lvu;
    Tlvu_ivalText* self = lvu->lvu_ivalText;
    Twid_cb_TU vld = lvu->lvu_cbVld ? lvuvt_ivtsli_cb_val : 0;
    Twid_cb_TU chg = lvu->lvu_cbChg ? lvuvt_ivtsli_cb_chg : 0;
    self->slin = widD_slin_nbf(lvu->lvu_vdw,
                self->currValue, self->width, chg, vld, lvu);
}

static Tegs_widget lvuvt_ivtsli_WidEGS(Twid_lvuBase* lvu)
{ return lvu->lvu_ivalText->slin->widEGS; }

static CstrBrief lvuvt_ivtsli_getText(Twid_lvuBase* lvu)
{ Tlvu_ivalText* self = lvu->lvu_ivalText; return self->currValue; }


static void lvuvt_ivtsli_SetText(Twid_lvuBase* lvu, CstrBrief v)
{
    Tlvu_ivalText* self = lvu->lvu_ivalText;
    // update local storage
    tool_free( self->currValue );
    self->currValue = tool_strdup( v );
    // update widget
    wid_slin_setText(self->slin,self->currValue);
}

static void lvuvt_ivtsli_SetNum10(Twid_lvuBase* lvu, double v)
{
    char buf[100];
    sprintf(buf,"%f",v);
    lvuvt_ivtsli_SetText(lvu,buf);
}

static Tlvu_valVtable lvuvt_ivtsli_VT = {
    .dbf      = lvuvt_ivtsli_Dbf,
    .widEGS   = lvuvt_ivtsli_WidEGS,
    .getText  = lvuvt_ivtsli_getText,
    .setText  = lvuvt_ivtsli_SetText,
    .setNum10 = lvuvt_ivtsli_SetNum10,
};

/*======================================================================*/
/*= Interface                                                          =*/
extern Tlvu_valBase* lvu_newIValTxt(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width)
{
    Tlvu_ivalText* self = tool_malloc_0( sizeof(*self) );

    self->base.vtable = &lvuvt_ivtsli_VT;
    self->initValue = tool_strdup( iv );
    self->width = tool_strdup( width );
    self->currValue = tool_strdup( iv );

    return (Tlvu_valBase*)self;
}

/*======================================================================*/
/*= slin widget callback                                               =*/

static void lvuvt_ivtsli_cb_val(Twid_slin*slin, void*ud)
{
    DBG0_INPE("lvu=%p slin=%p\n",ud,slin);
    Twid_ilvu*     lvu  = ud;
    Tlvu_ivalText* self = lvu->lvu_ivalText;

    TstrEaten str = gui_slin_getText(slin);
    DBG1_INPM("lvu=%p slin=%p ud=%p value CHANGED : %s --> %s\n",
                ud,slin, self->currValue,str);
    tool_free(self->currValue);
    self->currValue = str;  // str is eated
    if ( lvu->lvu_cbVld!=0 ) 
        lvu->lvu_cbVld(lvu->lvu_vdw,lvu,str);
    DBG0_INPL("lvu=%p slin=%p\n",ud,slin);
}

static void lvuvt_ivtsli_cb_chg(Twid_slin*slin, void*ud)
{
    DBG0_INPE("lvu=%p slin=%p\n",ud,slin);
    Twid_ilvu*     lvu  = ud;
    Tlvu_ivalText* self = lvu->lvu_ivalText;

    TstrEaten newval = gui_slin_getText(slin);
    if ( strcmp(self->currValue,newval)==0 ) {
        DBG1_INPL("ilvu=%p slin=%p ud=%p value NOT changed\n",ud,slin);
        tool_free( newval );
        return;
    }
    DBG1_INPM("lvu=%p slin=%p ud=%p value CHANGED : %s --> %s\n",
                ud,slin, self->currValue,newval);
    tool_free(self->currValue);
    self->currValue = newval;  // newval is eated
    if ( lvu->lvu_cbChg!=0 ) 
        lvu->lvu_cbChg(lvu->lvu_vdw,lvu,newval);
    DBG1_INPL("lvu=%p slin=%p\n",ud,slin);
}

/*======================================================================*/
