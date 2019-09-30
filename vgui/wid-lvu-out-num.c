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
 *      $Id: wid-lvu-out-num.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-out-num.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item for numeric output                =*/
/*= It is based on a label widget.                                     =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_valNum {
    Tlvu_valBase0 base;

    int       nbVal;  // 1:a single value; 2: two values
    int       dx,dy;  // prefered size, 0 means adjust to width
    TstrEaten width;  // format of a number as for Trange.
    TstrEaten fmt;    // printf format

    /* initial value */
    double initValue;  // initial value
    double initValue2; // second initial value

    /* current value */
    double currValue;  // current value
    double currValue2; // second current value

    /* implementation */
    union {
        Tegs_widget label;
        Tegs_widget slout;  // not yet implemented
    } widget;
};

/*======================================================================*/
/*= Shared utilities.                                                  =*/

// initialize all but the printf format.
#define STD_INIT(self,vt,iv,iv2,width,dx,dy) \
    self->base.vtable = &vt; \
    self->dx        = dx; \
    self->dy        = dy; \
    self->width     = tool_strdup( width ); \
    self->initValue = iv; \
    self->initValue2= iv2; \
    self->currValue = iv; \
    self->currValue2= iv2;

/*======================================================================*/
/*= Implementation of numeric output value with a label widget.        =*/

static void lvuvt_ovnlab_setNum11(Twid_lvuBase* lvu, double v, double v2);
static void lvuvt_ovnlab_print(Twid_lvuBase* lvu);

static void lvuvt_ovnlab1_dbf(Twid_lvuBase* lvu)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    if ( self->fmt==0 ) {
        char fmt[100];
        range_Rfmt2Pfmt(fmt,self->width);
        if ( self->nbVal==1 ) {
            self->fmt = tool_strdup(fmt);
        } else {
            char fmt2[200];
            sprintf(fmt2,"%s x %s",fmt,fmt);
            self->fmt = tool_strdup(fmt2);
        }
    }
    self->widget.label = widD_label_nbf(lvu->lvu_vdw,NULL,self->width);
    lvuvt_ovnlab_print(lvu);
}

static void lvuvt_ovnlab1_free(Twid_lvuBase* lvu)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    tool_free( self->width );
    tool_free( self->fmt );
    free( self );
}

static Tegs_widget lvuvt_ovnlab_widEGS(Twid_lvuBase* lvu)
    { return lvu->lvu_valNum->widget.label; }

static void lvuvt_ovnlab_print(Twid_lvuBase* lvu)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    double v  = Fu2Wu(lvu,self->currValue);
    double v2 = Fu2Wu(lvu,self->currValue2);
    char str[100];
    sprintf(str,self->fmt,v,v2);
    wid_label_setTextSS(self->widget.label,str);
}

static void lvuvt_ovnlab_setText(Twid_lvuBase* lvu, CstrBrief v)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    wid_label_setTextSS(self->widget.label,v);
}

static void lvuvt_ovnlab_setNum10(Twid_lvuBase* lvu, double v)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    lvuvt_ovnlab_setNum11(lvu,v,self->currValue2);
}

static void lvuvt_ovnlab_setNum01(Twid_lvuBase* lvu, double v2)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    lvuvt_ovnlab_setNum11(lvu,self->currValue,v2);
}

static void lvuvt_ovnlab_setNum11(Twid_lvuBase* lvu, double v, double v2)
{
    Tlvu_valNum* self = lvu->lvu_valNum;
    if ( v==self->currValue && v2==self->currValue2 )
        return;
    self->currValue  = v;
    self->currValue2 = v2;
    lvuvt_ovnlab_print(lvu);
}

static Tlvu_valVtable lvuvt_ovnlab_VT = {
    .dbf      = lvuvt_ovnlab1_dbf,
    .free     = lvuvt_ovnlab1_free,
    .widEGS   = lvuvt_ovnlab_widEGS,
    .print    = lvuvt_ovnlab_print,
    //.getText  = lvuvt_getText,
    .setText  = lvuvt_ovnlab_setText,
    .setNum10 = lvuvt_ovnlab_setNum10,
    .setNum01 = lvuvt_ovnlab_setNum01,
    .setNum11 = lvuvt_ovnlab_setNum11,
};

extern Tlvu_valBase* lvu_newOValNumLab10(Twid_olvu* ow,
                        double  iv,
                        CstrBrief  width, int dx, int dy)
{
    Tlvu_valNum* self  = tool_malloc_0( sizeof(*self) );

    TEMP_asf_printf(width==0,
            "width parameter (eg: 999.999) is mandatory",0);

    self->nbVal = 1;
    STD_INIT(self,lvuvt_ovnlab_VT,iv,0,width,dx,dy);
    return (Tlvu_valBase*)self;
}
extern Tlvu_valBase* lvu_newOValNumLab11(Twid_olvu* ow,
                        double  iv, double iv2,
                        CstrBrief  width, int dx, int dy)
{
    Tlvu_valNum* self  = tool_malloc_0( sizeof(*self) );

    TEMP_asf_printf(width==0,
            "width parameter (eg: 999.999) is mandatory",0);

    self->nbVal = 2;
    STD_INIT(self,lvuvt_ovnlab_VT,iv,iv2,width,dx,dy);
    return (Tlvu_valBase*)self;
}

/*======================================================================*/
