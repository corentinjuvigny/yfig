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
 *      $Id: wid-simple.c 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-simple.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building basic widgets.                              =*/
/*= See header wid-basewidget.h for type and function documentation.   =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-egs.h"

#include "vgui/vgui-names.h"
#include "tools/color.h"

/*======================================================================*/
/*= Label widget                                                       =*/ 

extern Tegs_widget widM_label_nbf(Tvgui*     gui,  Tegs_widget pw,
                                  CstrBrief text, CstrBrief width)
{
    CstrBrief wn = wn_mainLabel;
    int       dx = -1;
    if ( width!=0 ) dx = gui_string_width(gui,wn,width);
    return gui_label_nbf(gui,pw,wn,
            -1, dx, -1, EGSCOLOR_default, EGSCOLOR_default, text);  
}

extern Tegs_widget widD_label_nbf(Tvgui_dw*vdw,
                                  CstrBrief text, CstrBrief width)
{
    Tvgui*      gui = vgui_dw_getVGui(vdw);
    Tegs_widget pw  = vgui_dw_getWidEGS(vdw);
    CstrBrief   wn  = wn_dialLabel;
    int         dx  = -1;
    if ( width!=0 ) dx = gui_string_width(gui,wn,width);
    return gui_label_nbf(gui,pw,wn,
            -1, dx, -1, EGSCOLOR_default, EGSCOLOR_default, text);  
}

extern Tegs_widget widM_label_nbfBox(Tvgui* gui, Tegs_widget pw,
                                     int    bdw, int dx, int dy)
{
    CstrBrief wn = wn_mainLabel;
    return gui_label_nbf(gui,pw,wn,
            bdw, dx, dy, EGSCOLOR_default, EGSCOLOR_default, NULL);  
}

extern Tegs_widget widD_label_nbfBox(Tvgui_dw*vdw,
                                     int bdw, int dx, int dy)
{
    Tvgui*      gui = vgui_dw_getVGui(vdw);
    Tegs_widget pw  = vgui_dw_getWidEGS(vdw);
    CstrBrief   wn  = wn_dialLabel;
    return gui_label_nbf(gui,pw,wn,
            bdw, dx, dy, EGSCOLOR_default, EGSCOLOR_default, NULL);  
}

extern void wid_label_setText    (Tegs_widget self, CstrBrief  text)
{ gui_label_setTextAS(self,text); }
extern void wid_label_setTextAS  (Tegs_widget self, CstrBrief  text)
{ gui_label_setTextAS(self,text); }
extern void wid_label_setTextSS  (Tegs_widget self, CstrBrief  text)
{ gui_label_setTextSS(self,text); }

extern void wid_label_setBgColor (Tegs_widget self, Ccolor* c)
{ gui_widegs_setBgColor(self,c->egs); }

extern void wid_label_setFgColor (Tegs_widget self, Ccolor* c)
{ gui_widegs_setFgColor(self,c->egs); }

extern void wid_label_setBgPxm   (Tegs_widget self, TpixmapEGS pxm)
{ gui_widegs_setBgPxm(self,pxm); }

extern void wid_label_setPixmap  (Tegs_widget self, TpixmapEGS pm)
{ gui_label_setPixmap(self,pm); }

extern void wid_label_setSize    (Tegs_widget self, int dx, int dy)
{ gui_widegs_setSize(self,dx,dy); }

/*======================================================================*/
/*= text input widgets (Single Line)                                   =*/

static Twid_slin* wid_slin_nbf   (
        Tvgui*gui, Tegs_widget pw,
        CstrBrief wn, int dx, CstrBrief text,
        Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Twid_slin *self = tool_malloc( sizeof(*self) );
    self->gui = gui;
    self->chg = chg;
    self->val = val;
    self->ud  = ud;

    self->widEGS = gui_slin_dbf(self,pw,wn,
            -1,dx,-1, EGSCOLOR_default,EGSCOLOR_default,
            text);
    return self;
}


extern Twid_slin* widM_slin_nbf   (Tvgui*gui, Tegs_widget pw,
                                   CstrBrief text, CstrBrief width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Cstr wn = wn_mainSlin;
    int dx = -1;
    if ( width!=0 ) dx = gui_string_width(gui,wn,width);
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern Twid_slin* widM_slin_nbf_DX(Tvgui*gui, Tegs_widget pw,
                                   CstrBrief text, int width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Cstr wn = wn_mainSlin;
    int  dx = width;
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern Twid_slin* widM_slin_nbf_CN(Tvgui*gui, Tegs_widget pw,
                                   CstrBrief text, int cn,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Cstr wn = wn_mainSlin;
    // compute dx for cn letters
    int dx;
    char       str[]  = "WeeeammilM";
    double     lenStr = sizeof(str);
    double     dxStr  = gui_string_width(gui,wn,str);
    dx = (dxStr/lenStr) * (double)cn;
    dx += 1;
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern Twid_slin* widD_slin_nbf   (Tvgui_dw* d,
                                   CstrBrief text, CstrBrief width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    Cstr        wn  = wn_dialSlin;
    int         dx  = -1;
    if ( width!=0 )
        dx = gui_string_width(gui,wn,width);
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern Twid_slin* widD_slin_nbf_DX(Tvgui_dw* d,
                                   CstrBrief text, int width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    Cstr        wn  = wn_dialSlin;
    int         dx  = width;
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern Twid_slin* widD_slin_nbf_CN(Tvgui_dw* d,
                                   CstrBrief text, int cn,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    Cstr        wn  = wn_dialSlin;
    // compute dx for cn letters
    int dx;
    char       str[]  = "WeeeammilM";
    double     lenStr = sizeof(str);
    double     dxStr  = gui_string_width(gui,wn,str);
    dx = (dxStr/lenStr) * (double)cn;
    dx += 1;
    return wid_slin_nbf(gui,pw,wn,dx,text,chg,val,ud);
}

extern void       wid_slin_free   (Twid_slin* self)
{ tool_free( self ); }


extern TstrEaten   wid_slin_getText   (Twid_slin* self)
{ return gui_slin_getText(self); }

extern void        wid_slin_setText   (Twid_slin* self,  CstrBrief str)
{ return gui_slin_setText(self,str); }

extern void       wid_slin_disable(Twid_slin* self)
{ gui_slin_disable(self); }
extern void       wid_slin_enable (Twid_slin* self)
{ gui_slin_enable(self); }

/*======================================================================*/
/*= single line out widgets (program output string into this widget)   =*/

static Tegs_widget wid_slout_nbf   (Tvgui*gui, Tegs_widget pw,
        CstrBrief wn, int dx, CstrBrief text)
{
    //int dx=-1;
    //if ( width ) dx = gui_string_width(gui,wn,width);
    return gui_slout_nbf(gui,pw,wn,
            -1,dx,-1, EGSCOLOR_default,EGSCOLOR_default,
            text);
}

extern Tegs_widget widM_slout_nbf   (Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, CstrBrief width)
{
    Cstr wn = wn_mainSlout;
    int dx = -1;
    if ( width!=0 ) dx = gui_string_width(gui,wn,width);
    return wid_slout_nbf(gui,pw,wn,dx,text);
}

extern Tegs_widget widM_slout_nbf_DX(Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, int width)
{ return wid_slout_nbf(gui,pw,wn_mainSlout,width,text); }

extern Tegs_widget widM_slout_nbf_CN(Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, int cn)
{
    Cstr wn = wn_mainSlout;

    // compute dx for cn letters
    int dx;
    char       str[]  = "WeeeammilM";
    double     lenStr = sizeof(str);
    double     dxStr  = gui_string_width(gui,wn,str);
    dx = (dxStr/lenStr) * (double)cn;
    dx += 1;

    // do the widget
    return wid_slout_nbf(gui,pw,wn,dx,text);
}

extern Tegs_widget widD_slout_nbf   (Tvgui_dw* d,
                                     CstrBrief text, CstrBrief width)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    Cstr    wn  = wn_dialSlout;

    int dx = -1;
    if ( width!=0 ) dx = gui_string_width(gui,wn,width);

    return wid_slout_nbf(gui,pw,wn,dx,text);
}

extern Tegs_widget widD_slout_nbf_DX(Tvgui_dw* d,
                                     CstrBrief text, int width)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    return wid_slout_nbf(gui,pw,wn_dialSlout,width,text);
}


extern Tegs_widget widD_slout_nbf_CN(Tvgui_dw* d,
                                     CstrBrief text, int cn)
{
    Tvgui*      gui = vgui_dw_getVGui(d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    Cstr        wn = wn_dialSlout;

    // compute dx for cn letters
    int dx;
    char       str[]  = "WeeeammilM";
    double     lenStr = sizeof(str);
    double     dxStr  = gui_string_width(gui,wn,str);
    dx = (dxStr/lenStr) * (double)cn;
    dx += 1;

    return wid_slout_nbf(gui,pw,wn,dx,text);
}

extern TstrEaten   wid_slout_getText   (Tegs_widget self)
{ return gui_slout_getText(self); }
extern void        wid_slout_setText   (Tegs_widget self,  CstrBrief text)
{ gui_slout_setText(self,text); }
extern void        wid_slout_setTextWrn(Tegs_widget self,  CstrBrief text)
{ gui_slout_setTextWrn(self,text); }
extern void        wid_slout_setTextErr(Tegs_widget self,  CstrBrief text)
{ gui_slout_setTextErr(self,text); }

/*======================================================================*/
/*= multiple line out widgets (program output string into this widget) =*/

static Twid_mlout* wid_mlout_nbf   (Tvgui*gui, Tegs_widget pw,
        CstrBrief wn, int cn, int ln, int sb)
{
    Twid_mlout* self = tool_malloc( sizeof(*self) );
    self->gui = gui;

    // convert cn in pixel
    int dx;
    char       str[]  = "WeeeammilM";
    double     lenStr = sizeof(str);
    double     dxStr  = gui_string_width(gui,wn,str);
    dx = (dxStr/lenStr) * (double)cn;
    dx += 1;

    // convert ln in pixel
    int dy = ln * 15;

    self->widEGS = gui_mlout_nbf(gui,pw,wn,
            -1,dx,dy, EGSCOLOR_default,EGSCOLOR_default,
            sb);
    return self;
}

extern Twid_mlout* widM_mlout_nbf   (Tvgui*gui, Tegs_widget pw,
                                     int cn, int ln, int sb)
{ return wid_mlout_nbf(gui,pw,wn_mainMlout, cn,ln,sb); }

extern Twid_mlout* widD_mlout_nbf   (Tvgui_dw*d,
                                     int cn, int ln, int sb)
{ 
    Tvgui*      gui = vgui_dw_getVGui  (d);
    Tegs_widget pw  = vgui_dw_getWidEGS(d);
    return wid_mlout_nbf(gui,pw,wn_mainMlout, cn,ln,sb);
}

extern void wid_mlout_free   (Twid_mlout* self) { tool_free(self); }

extern void wid_mlout_setText  (Twid_mlout* self, CstrBrief str)
{ gui_mlout_setText(self,str); }

extern void wid_mlout_printf   (Twid_mlout* self, CstrBrief fmt, ...) { va_list ap; va_start(ap,fmt); wid_mlout_vprintf   (self,fmt,ap); va_end(ap); }
extern void wid_mlout_printfWrn(Twid_mlout* self, CstrBrief fmt, ...) { va_list ap; va_start(ap,fmt); wid_mlout_vprintfWrn(self,fmt,ap); va_end(ap); }
extern void wid_mlout_printfErr(Twid_mlout* self, CstrBrief fmt, ...) { va_list ap; va_start(ap,fmt); wid_mlout_vprintfErr(self,fmt,ap); va_end(ap); }

extern void wid_mlout_vprintf   (Twid_mlout* self,
                                 CstrBrief fmt, va_list ap)
{
    TstrEaten str = tool_vaprintf(fmt,ap);
    gui_mlout_appText(self,str);
    free(str);
}

extern void wid_mlout_vprintfWrn(Twid_mlout* self,
                                 CstrBrief fmt, va_list ap)
{
    TstrEaten str = tool_vaprintf(fmt,ap);
    gui_mlout_appTextWrn(self,str);
    free(str);
}

extern void wid_mlout_vprintfErr(Twid_mlout* self,
                                 CstrBrief fmt, va_list ap)
{
    TstrEaten str = tool_vaprintf(fmt,ap);
    gui_mlout_appTextErr(self,str);
    free(str);
}

/*======================================================================*/
/*= default label of control dialog buttons                            =*/

static Cstr dw_status2buttLab_t[32] = {
    0,0,0,0, 0,0,0,0
   // 8
   ,"Cancel", "Cancel"
   // 10
   ,"Abort"
   ,"Ignore"
   ,"No", "Yes"
   ,"Apply"
   // 15
   ,"New Discarding\nChanges"
   ,"New"
   ,"Open Discarding\nChanges"
   ,"Continue"
   ,"Next"
   // 20
   ,"Open"
   ,"Save"
   ,"Do It"
   ,"Ok"
} ;

//extern CstrStatic wid_ctlBUTlabel(Twid_dialStatus b)
extern CstrStatic vgui_dw_status2buttLab(Tvgui_dwStatus b)
{
    int i; Tvgui_dwStatus but;
    for (i=8,but=WID_DIALSTATUS_First ; but<WID_DIALSTATUS_Last
                                       ; i+=1,but<<=1 ) {
        if ( b==but )
            return dw_status2buttLab_t[i];
    }
    return "???";
}
/*======================================================================*/
/*= spinner widget                                                     =*/

static Twid_spinner* wid_spinner_new(
                     Tvgui*      gui,  CstrBrief   fmt,
                     double      min,  double      max, double iof,
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud)
{
    Twid_spinner* spin = tool_malloc_0( sizeof(*spin) );
    Trange*       range= &spin->range;
    range_initIMM(range, 0, fmt, min, max);
    range->iof = iof;
    spin->cbVal = val;
    spin->cbChg = chg;
    spin->cbUd  = ud;

    return spin;
}

extern Twid_spinner* wid_spinner_newInc(
                     Tvgui*      gui,  CstrBrief   fmt,
                     double      min,  double      max, double iof,
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud)
{
    Twid_spinner* spin = wid_spinner_new(gui, fmt, min,max, iof,
                                         val, chg, ud);
    return spin;
}

extern Twid_spinner* wid_spinner_newMul(
                     Tvgui*      gui,  CstrBrief   fmt,
                     double      min,  double      max, double iof,
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud)
{
    Twid_spinner* spin = wid_spinner_new(gui, fmt, min,max, iof,
                                         val, chg, ud);
    Trange*       range= &spin->range;
    range->flag &= ~RANGE_FL_Inc;
    range->flag |=  RANGE_FL_Mult;
    return spin;
}

extern Twid_spinner* wid_spinner_newRange(
                     Tvgui*      gui,  Crange*     r, 
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud)
{
    Twid_spinner* spin  = tool_malloc_0( sizeof(*spin) );
    Trange*       range = &spin->range;
    range_cpy(range,r);
    spin->cbVal = val;
    spin->cbChg = chg;
    spin->cbUd  = ud;
    return spin;
}

extern void wid_spinner_update(Twid_spinner* spin,
        CstrBrief fmt, double val,
        double min, double max, double iof)
{
    Trange* range= &spin->range;
    range_init0(range, val, fmt);
    range->min   = min;
    range->max   = max;
    range->iof   = iof;
    gui_spinner_update(spin);
    wid_spinner_setNum(spin,val);
}

extern void wid_spinner_update_minVal(Twid_spinner* spin, double v)
{
    Trange* range= &spin->range;
    double  currval = wid_spinner_getNum(spin);
    range->min   = v;
    range->flag |= RANGE_FL_LowBound;
    gui_spinner_update(spin);
    if ( currval<v) 
        wid_spinner_setNum(spin,v);
}

extern void wid_spinner_update_maxVal(Twid_spinner* spin, double v)
{
    Trange* range= &spin->range;
    double  currval = wid_spinner_getNum(spin);
    range->max = v; 
    range->flag |= RANGE_FL_UppBound;
    gui_spinner_update(spin);
    if ( currval>v) 
        wid_spinner_setNum(spin,v);
}

extern void   wid_spinner_free   (Twid_spinner* spin) { gui_spinner_free(spin); tool_free(spin); }
extern void   wid_spinner_disable(Twid_spinner* spin) { spin->enabled=0; gui_spinner_disable(spin); }
extern void   wid_spinner_enable (Twid_spinner* spin) { spin->enabled=1; gui_spinner_enable(spin); }

extern double wid_spinner_getNum (Twid_spinner* spin) { return spin->range.val+spin->round; }
extern void   wid_spinner_setNum (Twid_spinner* spin, double v) {
    char enabled = spin->enabled;
    wid_spinner_disable(spin);
    gui_spinner_setNum( spin,v);
    spin->round = v-spin->range.val;
    if ( enabled)
        wid_spinner_enable(spin);
}

/*======================================================================*/
/*= Slider widget.                                                     =*/

extern Twid_slider* widM_slider_nbf( Tvgui* gui, Tegs_widget pw,
                                     double iv, double inc,
                                     Twid_cb_LUd  chg, void*  ud);
extern Twid_slider* widD_slider_nbf( Tvgui_dw* vdw,
                                     double iv, double inc,
                                     Twid_cb_LUd  chg, void*  ud)
{
    Tegs_widget  pw   = vgui_dw_getWidEGS(vdw);
    Twid_slider* self = tool_malloc_0( sizeof(*self) );
    self->gui = vgui_dw_getVGui(vdw);
    self->val = iv;
    self->inc = inc;
    self->chg = chg;
    self->ud  = ud;
    self->widEGS = gui_slider_create(self,pw,wn_mainBtnOff,-1,10,100);
    gui_slider_set(self,self->val);
    return self;
}

extern void         wid_slider_free(Twid_slider*self)
{
    tool_free( self );
}

extern double       wid_slider_setSize(Twid_slider*self,int dx,int dy)
{
     gui_widegs_setSize(self->widEGS,dx,dy);
}

extern double       wid_slider_get (Twid_slider*self)
{
    TEMP_ase_printf( self->val!=gui_slider_get(self),
            "slider value not up to date: vgui=%f egs=%f\n",
            self->val,gui_slider_get(self));
    return self->val;
}

extern void         wid_slider_set (Twid_slider*self, double v)
{
    if ( v!=self->val ) {
        self->val = v;
        gui_slider_set(self,self->val);
    }
}


extern Tegs_widget  gui_slider_create(Twid_slider*self,
                                      Tegs_widget pw, CstrBrief wn,
                                      int bdt, int dx, int dy);
extern double       wid_slider_get (Twid_slider*self);
extern void         wid_slider_set (Twid_slider*self, double v);


/*======================================================================*/
/*= length unit widget                                                 =*/

//#include "../include/wid-dialog.h"
#include "vgui/wid-menu.h"
#include "tools/lenunit.h"

#define WID_MENU_LU_IndCm 0 
#define WID_MENU_LU_IndIn 1 
#define WID_MENU_LU_IndPt 2 
#define WID_MENU_LU_IndFu 3 

struct _Twid_lenunit {
    Twidm_ddl*    ddl;
    Twid_cb_LiU   cb;
    void*         ud;
} ;

static void lenuni_cb(Twidm_any*m, Twidm_entry* me)
{
    Twidm_ddl*     dll  = me->me_menuDD;
    Twid_lenunit*  lu   = dll->m_appCBud;
    int            unit = wid_lenunit_getUnit(lu);
    if ( lu->cb ) lu->cb(lu,unit,lu->ud);
}

static int lenunit_Unit2EntryIndex(int unit)
{
        return unit==LUNIT_ID_Cm   ? WID_MENU_LU_IndCm
             : unit==LUNIT_ID_Inch ? WID_MENU_LU_IndIn
             : unit==LUNIT_ID_Pt   ? WID_MENU_LU_IndPt
             :                       WID_MENU_LU_IndFu ;
}

static int lenunit_EntryIndex2Unit(int ei)
{
        return ei==WID_MENU_LU_IndCm ? LUNIT_ID_Cm
             : ei==WID_MENU_LU_IndIn ? LUNIT_ID_Inch
             : ei==WID_MENU_LU_IndPt ? LUNIT_ID_Pt
             :                         LUNIT_ID_FU ;
}

extern Twid_lenunit* wid_lenunit_nbf( Tvgui_dw*d,
                                      Twid_cb_LiU cb, void*ud)
{
    static Twidm_item unit_table[] = {
      //               label checkmark action unit
      WIDM_ITEM_STR_IV( "cm" , 0,0    , NULL , LUNIT_ID_Cm   ), 
      WIDM_ITEM_STR_IV( "in" , 0,0    , NULL , LUNIT_ID_Inch ), 
      WIDM_ITEM_STR_IV( "pt" , 0,0    , NULL , LUNIT_ID_Pt   ), 
      WIDM_ITEM_STR_IV( "fu" , 0,0    , NULL , LUNIT_ID_FU   ), 
    };
    Tvgui*      gui = vgui_dw_getVGui  (d);
    Tvgui_mw*   vmw = vgui_dw_getMw    (d);
    Tegs_widget* pw = vgui_dw_getWidEGS(d);
    int DefaultUnit = LUNIT_ID_Inch;
    Twid_lenunit* lu = tool_malloc( sizeof(*lu) );
    lu->ddl = widm_ddl_newByItemT(gui, vmw, "Change length unit",
            TOOL_TABLE_ARG(unit_table),
            lenuni_cb, lu);
    widmD_ddl_dbf(lu->ddl,pw);
    lu->cb  = cb;
    lu->ud  = ud;
    wid_lenunit_setUnit(lu,DefaultUnit);
    return lu;
}

extern Twid_lenunit* wid_lenunit_nbf2(Tvgui_dw*d,
                                      Twid_cb_LiU cb, void*ud, int u);

extern void          wid_lenunit_free(Twid_lenunit*lu)
{
    widm_ddl_delete( lu->ddl );
    free         ( lu );
}

extern double        wid_lenunit_convToFU(Cwid_lenunit*lu, double v)
{
    switch ( lu->ddl->m_selectedEntry ) {
        case WID_MENU_LU_IndCm : return LUNIT_CmToFu(v);
        case WID_MENU_LU_IndIn:  return LUNIT_InToFu(v);
        case WID_MENU_LU_IndPt:  return LUNIT_PtToFu(v);
        case WID_MENU_LU_IndFu:  return v;
        default:                 return v;
    }
}

extern double        wid_lenunit_convToWU(Cwid_lenunit*lu, double v)
{
    switch ( lu->ddl->m_selectedEntry ) {
        case WID_MENU_LU_IndCm : return LUNIT_FuToCm(v);
        case WID_MENU_LU_IndIn:  return LUNIT_FuToIn(v);
        case WID_MENU_LU_IndPt:  return LUNIT_FuToPt(v);
        case WID_MENU_LU_IndFu:  return v;
        default:                 return v;
    }
}

extern int           wid_lenunit_getUnit( Cwid_lenunit*lu)
{ return lenunit_EntryIndex2Unit( lu->ddl->m_selectedEntry); }

extern int           wid_lenunit_setUnit( Cwid_lenunit*lu, int u)
{
    int index = lenunit_Unit2EntryIndex(u);
    widm_ddl_setCurrentEntryIndex(lu->ddl,index);
}

extern Tegs_widget   wid_lenunit_widget(Cwid_lenunit*lu)
{ return lu->ddl->m_btnEGS; }

/*======================================================================*/
