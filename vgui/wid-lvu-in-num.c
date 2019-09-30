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
 *      $Id: wid-lvu-in-num.c 364 2018-11-18 23:10:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvu-in-num.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= "Label Value Unit" composite widget.                               =*/
/*=                                                                    =*/
/*= Implementation of the value item for numeric input                 =*/
/*= It is based on Twid_spinner widget.                                =*/
/*=                                                                    =*/
/*= See the wid-lvuI.h and vgui/wid-lvu.h headers for documentation.   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-lvuI.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tlvu_ivalNum {
    Tlvu_valBase0 base;

    int       integer; // integer spinner
    int       dx,dy;   // prefered size, 0 means adjust to width
                       // no yet used
    TstrEaten width;   // format of a number as for Trange.

    /* initial value */
    Trange*   initRange;   // in Fu unit when std lenunit

    /* current value */
    double currValFu;  // in Fu unit
    Trange*currRange;  // in Wu unit when std lenunit

    /* implementation */
    union {
        Twid_spinner* spin;
        Twid_slin*    slin; // without spinner not yet implemented
    } widget;
};

/*======================================================================*/
/*= Implementation of numeric value in a spinner                       =*/

static void lvuvt_ivnspi_cb_vld (Twid_spinner*spin, void*ud, double v);
static void lvuvt_ivnspi_cb_chg (Twid_spinner*spin, void*ud, double v);

static void lvuvt_ivnspi_dbf(Twid_lvuBase* lvu)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;

        Tvgui_dw*   vdw    = lvu->lvu_vdw;
        Tvgui*      gui    = vgui_dw_getVGui(vdw);
        Tegs_widget parent = vgui_dw_getWidEGS(vdw);
        // spinner callback must always be called, for
        // updating the current value.
        Twid_cb_SUd vld = lvuvt_ivnspi_cb_vld ;
        Twid_cb_SUd chg = lvuvt_ivnspi_cb_chg ;
        void*       ud  = lvu ;
        self->widget.spin = wid_spinner_newRange(
                 gui, self->currRange, vld,chg,ud);
        gui_spinner_dbf(self->widget.spin,gui,parent);
        wid_spinner_setNum(self->widget.spin,self->initRange->val);
}

static void lvuvt_ivnspi_free(Twid_lvuBase* lvu)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    tool_free  ( self->width );
    range_free ( self->initRange );
    range_free ( self->currRange );
    if (  self->widget.spin )
        wid_spinner_free( self->widget.spin );
    free( self );
}

static Tegs_widget lvuvt_ivnspi_widEGS(Twid_lvuBase* lvu)
    { return lvu->lvu_ivalNum->widget.spin->self; }

//static void lvuvt_ivnspi_print(Twid_lvuBase* lvu)
//{
//    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
//}

static double lvuvt_ivnspi_getNum10(Twid_lvuBase* lvu)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    if ( self->integer )
        return round( self->currValFu );
    else
        return self->currValFu;
}

static void lvuvt_ivnspi_setNum10(Twid_lvuBase* lvu, double v)
{
#define val self->currValFu
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    DBG0_INPE("lvu=%p cur=%f v=%f conv(v)=%f",lvu,val,v,Fu2Wu(lvu,v));
    self->currValFu = v;
    gui_spinner_setNum(self->widget.spin, Fu2Wu(lvu,v) );
    DBG1_INPL("ilvu=%p cur=%f v=%f conv(v)=%f",lvu,val,v,Fu2Wu(lvu,v));
#undef val
}

static void lvuvt_ivnspi_reset    (Twid_lvuBase* lvu)
{ Tlvu_ivalNum* self = lvu->lvu_ivalNum;
  lvuvt_ivnspi_setNum10( lvu, self->initRange->val); }

static void lvuvt_ivnspi_clear    (Twid_lvuBase* lvu)
{ Tlvu_ivalNum* self = lvu->lvu_ivalNum;
  gui_spinner_clear(self->widget.spin); }

static void lvuvt_ivnspi_disable  (Twid_lvuBase* lvu)
{ Tlvu_ivalNum* self = lvu->lvu_ivalNum;
  wid_spinner_disable(self->widget.spin); }

static void lvuvt_ivnspi_enable   (Twid_lvuBase* lvu)
{ Tlvu_ivalNum* self = lvu->lvu_ivalNum;
  wid_spinner_enable(self->widget.spin); }


static Trange* lvuvt_ivnspi_getRange (Twid_lvuBase* lvu)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    return self->currRange;
}

static void   lvuvt_ivnspi_unitChged(Twid_lvuBase* lvu, int newunit)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    Twid_spinner* spin = self->widget.spin;
    double iof=1;
    switch (newunit) {
      case LUNIT_ID_FU:   iof=self->initRange->iof; break;
      case LUNIT_ID_Pt:   iof=1;    break;
      case LUNIT_ID_Inch: iof=0.1;  break;
      case LUNIT_ID_Cm:   iof=0.1;  break;
      default:
        PERM_spe_printf("%d is an unexpected unit code",newunit);
    }
    double valFu = self->currValFu;
    wid_spinner_update(spin, self->width,
            Fu2Wu(lvu,valFu),
            Fu2Wu(lvu,self->initRange->min),
            Fu2Wu(lvu,self->initRange->max), iof);
}

static void lvuvt_ivnspi_chgValMin(Twid_lvuBase* lvu, double v)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    Twid_spinner* spin = self->widget.spin;
    wid_spinner_update_minVal(spin, Fu2Wu(lvu,v));
    self->currValFu = Wu2Fu(lvu,wid_spinner_getNum(spin));
}

static void lvuvt_ivnspi_chgValMax(Twid_lvuBase* lvu, double v)
{
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;
    Twid_spinner* spin = self->widget.spin;
    wid_spinner_update_maxVal(spin, Fu2Wu(lvu,v));
    self->currValFu = Wu2Fu(lvu,wid_spinner_getNum(spin));
}

static Tlvu_valVtable lvuvt_ivnspi_VT = {
    .dbf       = lvuvt_ivnspi_dbf,
    .free      = lvuvt_ivnspi_free,
    .widEGS    = lvuvt_ivnspi_widEGS,
  //.print     = lvuvt_ivnspi_print,
  //.getText   = lvuvt_getText,
  //.setText   = lvuvt_ivnspi_setText,
    .getNum10  = lvuvt_ivnspi_getNum10,
    .setNum10  = lvuvt_ivnspi_setNum10,
  //.setNum01  = lvuvt_ivnspi_setNum01,
  //.setNum11  = lvuvt_ivnspi_setNum11,
    .reset     = lvuvt_ivnspi_reset,
    .clear     = lvuvt_ivnspi_clear,
    .disable   = lvuvt_ivnspi_disable,
    .enable    = lvuvt_ivnspi_enable,
    .getRange  = lvuvt_ivnspi_getRange,
    .unitChged = lvuvt_ivnspi_unitChged,
    .chgValMax = lvuvt_ivnspi_chgValMax,
    .chgValMin = lvuvt_ivnspi_chgValMin,
};

extern Tlvu_valBase* lvu_newIValInt(Twid_olvu* ow,
                        double  iv, int min, int max,
                        CstrBrief  width, int dx, int dy)
{
    Tlvu_ivalNum* self  = tool_malloc_0( sizeof(*self) );
    Trange*       r     = range_newIMM(iv,width,min,max);

    TEMP_asf_printf(width==0,
            "width parameter (eg: 999.999) is mandatory",0);

    self->base.vtable    = &lvuvt_ivnspi_VT;
    self->integer        = 1;
    self->dx             = dx;
    self->dy             = dy;
    self->width          = tool_strdup(width);
    self->initRange      = r;
    self->currValFu      = iv;
    self->currRange      = range_dup(r);
    return (Tlvu_valBase*)self;
}

extern Tlvu_valBase* lvu_newIValDle(Twid_olvu* ow,
                        double  iv, double min, double max,
                        CstrBrief  width, int dx, int dy)
{
    Tlvu_ivalNum* self  = tool_malloc_0( sizeof(*self) );
    Trange*       r     = range_newIMM(iv,width,min,max);

    TEMP_asf_printf(width==0,
            "width parameter (eg: 999.999) is mandatory",0);

    self->base.vtable    = &lvuvt_ivnspi_VT;
    self->integer        = 0;
    self->dx             = dx;
    self->dy             = dy;
    self->width          = tool_strdup(width);
    self->initRange      = r;
    self->currValFu      = iv;
    self->currRange      = range_dup(r);
    return (Tlvu_valBase*)self;
}

/*======================================================================*/
/*= spinner widget callback                                            =*/

static void lvuvt_ivnspi_cb_vld (Twid_spinner*spin, void*ud, double v)
{
    DBG0_INPE("lvu=%p spin=%p v=%f\n",ud,spin,v);
    Twid_ilvu*    lvu  = ud;
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;

    // update widget
    double valDble = Wu2Fu(lvu,v);
    self->currValFu = valDble;

    // trigger the callback if any.
    if ( lvu->lvu_cbVld ) {
        if ( self->integer ) {
            int valInt = round( valDble );
            lvu->lvu_cbVld(lvu->lvu_vdw,lvu,valInt);
        } else
            lvu->lvu_cbVld(lvu->lvu_vdw,lvu,valDble);
    }
    DBG1_INPL("lvu=%p spin=%p v=%f\n",ud,spin,v);
}

static void lvuvt_ivnspi_cb_chg (Twid_spinner*spin, void*ud, double v)
{
    DBG0_INPE("lvu=%p spin=%p v=%f\n",ud,spin,v);
    Twid_ilvu*    lvu  = ud;
    Tlvu_ivalNum* self = lvu->lvu_ivalNum;

    // update widget
    double valDble = Wu2Fu(lvu,v);
    self->currValFu = valDble;

    // trigger the callback if any.
    if ( lvu->lvu_cbChg ) {
        if ( self->integer ) {
            int valInt = round( valDble );
            lvu->lvu_cbChg(lvu->lvu_vdw,lvu,valInt);
        } else
            lvu->lvu_cbChg(lvu->lvu_vdw,lvu,valDble);
    }
    DBG1_INPL("lvu=%p spin=%p v=%f\n",ud,spin,v);
}

/*======================================================================*/
