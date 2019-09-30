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
 *      $Id: wid-simple.h 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-simple.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * Base widgets.
 *
 * - Twid_dialStatus: the code returned by dialog after completion.
 * - Twid_cb_xxx: miscellaneous callbacks
 * - utilities for creating base dialog widgets:
 *     * label
 *     * text output widgets (VGUI writes data into this widget, the
 *                            end user reads them)
 *     * push button
 *     * text input widgets  (the end user writes data into this widget,
 *                            VGUI reads them)
 *     * spinner
 *     * a drop drown list for length unit
 *     * input & output widgets (base element to present set of values)
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_SIMPLE_H
#define FILE_WID_SIMPLE_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "tools/range.h"

/*======================================================================*/
/*= dialog status                                                      =*/

/**
 * The Twid_dialStatus type defines the status returned by the popup
 * dialogs when they exit (popdown).
 * Its values can be also ored to define the functions a dialog supports.
 * The 8 less significant bits (LSB) are not used.
**/
typedef enum _Twid_dialStatus {
    WID_DIALSTATUS_None         = 0,      // No status
    WID_DIALSTATUS_Cancel       = 1<< 8,  // the Cancel button was clicked
    WID_DIALSTATUS_CancelX      = 1<< 9,  // the X of window was clicked
                                          // the widget must be deleted.
    WID_DIALSTATUS_Abort        = 1<<10,  // the Abort button was clicked
    WID_DIALSTATUS_Ignore       = 1<<11,  // the Ignore button was clicked
    WID_DIALSTATUS_No           = 1<<12,  // the No button was clicked
    WID_DIALSTATUS_Yes          = 1<<13,  // the Yes button was clicked
    WID_DIALSTATUS_Apply        = 1<<14,  // the Apply button was clicked
    WID_DIALSTATUS_NewDiscard   = 1<<15,  // the New and discard change
                                          // button was clicked
    WID_DIALSTATUS_New          = 1<<16,  // the New  button was clicked
    WID_DIALSTATUS_OpenDiscard  = 1<<17,  // the Open and Discard change
                                          // button was clicked
    WID_DIALSTATUS_Cont         = 1<<18,  // the Continue button was clicked
    WID_DIALSTATUS_Next         = 1<<19,  // the Next. button was clicked
    WID_DIALSTATUS_Open         = 1<<20,  // the Open button was clicked
    WID_DIALSTATUS_Save         = 1<<21,  // the Save button was clicked
    WID_DIALSTATUS_Do           = 1<<22,  // the Do button was clicked
    WID_DIALSTATUS_Ok           = 1<<23,  // the OK button was clicked
    WID_DIALSTATUS_Last         = 1<<24,  // the Save button was clicked
} Twid_dialStatus;
#define WID_DIALSTATUS_First WID_DIALSTATUS_Cancel
typedef Twid_dialStatus Tvgui_dwStatus;

extern CstrStatic vgui_dw_status2buttLab(Tvgui_dwStatus b);

/*======================================================================*/
/*= miscellaneous types and callbacks                                  =*/

typedef struct _Twid_spinner  Twid_spinner;
typedef const   Twid_spinner  Cwid_spinner;

typedef struct _Twid_slin  Twid_slin;
typedef const   Twid_slin  Cwid_slin;

typedef struct _Twid_slider  Twid_slider;
typedef const   Twid_slider  Cwid_slider;

typedef struct _Twid_lenunit  Twid_lenunit;
typedef const   Twid_lenunit  Cwid_lenunit;

//typedef Cwid_ilvu Cwid_input; // obsolete

typedef void (*Twid_cb_Ud)  (void* ud,       double step);
typedef void (*Twid_cb_Uap) (void* ud,       ...);
typedef void (*Twid_cb_D)   (Tvgui_dw*vdw);
typedef void (*Twid_cb_DU)  (Tvgui_dw*vdw,  void* ud);

typedef void (*Twid_cb_WU)  (Tegs_widget   w,  void*       ud);
typedef void (*Twid_cb_TU)  (Twid_slin*    iw, void*       ud);
typedef void (*Twid_cb_LiU) (Twid_lenunit* lu, int         u,  void*ud);
typedef void (*Twid_cb_IU)  (Twid_ilvu*   iw, void*       ud);
typedef void (*Twid_cb_DSU) (Tvgui_dw*     dw, Twid_dialStatus s, void*ud);
typedef void (*Twid_cb_DI)  (Tvgui_dw*     dw, Twid_ilvu* iw);
typedef void (*Twid_cb_DIs) (Tvgui_dw*     dw, Twid_ilvu* iw, CstrBrief v);
typedef void (*Twid_cb_DIi) (Tvgui_dw*     dw, Twid_ilvu* iw, int       v);
typedef void (*Twid_cb_DId) (Tvgui_dw*     dw, Twid_ilvu* iw, double    v);
typedef void (*Twid_cb_SUd) (Twid_spinner* s,  void*       ud, double    v);
typedef void (*Twid_cb_LUd) (Twid_slider*  s,  void*       ud, double    v);
typedef void (*Twid_cb_Us)  (void*         ud, CstrBrief   v);

struct _Twid_slin {
    Tvgui*      gui;
    Tegs_widget widEGS;
    Twid_cb_TU  chg;
    Twid_cb_TU  val;
    void*       ud;
};

struct _Twid_spinner {
    /* parameter of spinner */
    char        enabled;
    double      round;    // real-val = range-val + round
                          // range-val is the displayed value and it
                          // is rounded: val=5.23 fmt=99.9 --> range-val=5.2
                          // round=0.03
    Trange      range;
    Twid_cb_SUd cbVal;    // cbVal(this,val,ud) (value validated)
    Twid_cb_SUd cbChg;    // cbChg(this,val,ud) (value changed)
    void*       cbUd;

    /* implémentation of spinner, VGUI requires only self */
    Tegs_widget  self;  // the top widget
    Twid_slin*   wVal;
    Tegs_widget  wSpi;
};

/*======================================================================*/
/*= Label widget                                                       =*/ 

/**
 * The widM_label_nbf function creates a label widget showing
 * the text string as a child of the pw EGS widget.
 *
 * If the width string is not null, it is used to compute the widget width.
 * If the width string is null, the text string is used for computing
 * the widget width.
 * The text parameter can be the null pointer (same as "").
 *
 * The widD_label_nbf function creates a label widget showing
 * the text string as a child of the d dialog.
 *
 * The wid_label_setText allows to change the string printed into
 * the self label widget.
 *
 * Return:
 *   The widM_label_nbf and widD_label_nbf functions return  the created
 *   EGS widget.
**/
extern Tegs_widget widM_label_nbf(Tvgui*     gui,  Tegs_widget pw,
                                  CstrBrief text, CstrBrief width);
extern Tegs_widget widD_label_nbf(Tvgui_dw*vdw,
                                  CstrBrief text, CstrBrief width);

extern Tegs_widget widM_label_nbfBox(Tvgui* gui, Tegs_widget pw,
                                     int    bdw, int dx, int dy);
extern Tegs_widget widD_label_nbfBox(Tvgui_dw*vdw,
                                     int bdw, int dx, int dy);

extern void wid_label_setText    (Tegs_widget self, CstrBrief   text);
extern void wid_label_setTextSS  (Tegs_widget self, CstrBrief   text);
extern void wid_label_setTextAS  (Tegs_widget self, CstrBrief   text);
extern void wid_label_setBgColor (Tegs_widget self, Ccolor*     c);
extern void wid_label_setFgColor (Tegs_widget self, Ccolor*     c);
extern void wid_label_setBgPxm   (Tegs_widget self, TpixmapEGS pxm);
extern void wid_label_setPixmap  (Tegs_widget self, TpixmapEGS pxm);
extern void wid_label_setSize    (Tegs_widget self, int dx, int dy);

// Interface to EGS.
extern Tegs_widget gui_label_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text);

extern void gui_label_setTextAS(Tegs_widget self, CstrBrief  text);
extern void gui_label_setTextSS(Tegs_widget self, CstrBrief  text);
extern void gui_label_setPixmap(Tegs_widget self, TpixmapEGS pm);

/*======================================================================*/
/*= Text inpput widgets (Single Line)                                  =*/

#define WID_SLIN_MaxLen 127 // maximum char of input string (strlen)

extern Twid_slin* widM_slin_nbf   (Tvgui*     gui,  Tegs_widget pw,
                                   CstrBrief  text, CstrBrief   width,
                                   Twid_cb_TU chg,  Twid_cb_TU val, void*ud);
extern Twid_slin* widM_slin_nbf_DX(Tvgui*gui, Tegs_widget pw,
                                   CstrBrief text, int width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud);
extern Twid_slin* widM_slin_nbf_CN(Tvgui*gui, Tegs_widget pw,
                                   CstrBrief text, int cn,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud);

extern Twid_slin* widD_slin_nbf   (Tvgui_dw*vdw,
                                   CstrBrief text, CstrBrief width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud);
extern Twid_slin* widD_slin_nbf_DX(Tvgui_dw*vdw,
                                   CstrBrief text, int width,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud);
extern Twid_slin* widD_slin_nbf_CN(Tvgui_dw*vdw,
                                   CstrBrief text, int cn,
                                   Twid_cb_TU chg, Twid_cb_TU val, void*ud);
extern void       wid_slin_free   (Twid_slin* self);

extern TstrEaten  wid_slin_getText(Twid_slin* self);
extern void       wid_slin_setText(Twid_slin* self,  CstrBrief str);
extern void       wid_slin_disable(Twid_slin* self);
extern void       wid_slin_enable (Twid_slin* self);

// Interface to EGS.
extern Tegs_widget gui_slin_dbf(Twid_slin*self,
                    Tegs_widget pw, CstrBrief wn,
                    int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
                    CstrBrief text);
extern TstrEaten   gui_slin_getText   (Twid_slin* self);
extern void        gui_slin_setText   (Twid_slin* self,  CstrBrief str);
extern void        gui_slin_disable(Twid_slin* self);
extern void        gui_slin_enable (Twid_slin* self);

#define DBG0_MSlinE(fmt,...) DBG0_printf("widslin","Enter : " fmt,__VA_ARGS__)
#define DBG0_MSlinM(fmt,...) DBG0_printf("widslin","Middle: " fmt,__VA_ARGS__)
#define DBG0_MSlinL(fmt,...) DBG0_printf("widslin","Leave : " fmt,__VA_ARGS__)
#define DBG1_MSlinE(fmt,...) DBG1_printf("widslin","Enter : " fmt,__VA_ARGS__)
#define DBG1_MSlinM(fmt,...) DBG1_printf("widslin","Middle: " fmt,__VA_ARGS__)
#define DBG1_MSlinL(fmt,...) DBG1_printf("widslin","Leave : " fmt,__VA_ARGS__)
#define DBG2_MSlinE(fmt,...) DBG2_printf("widslin","Enter : " fmt,__VA_ARGS__)
#define DBG2_MSlinM(fmt,...) DBG2_printf("widslin","Middle: " fmt,__VA_ARGS__)
#define DBG2_MSlinL(fmt,...) DBG2_printf("widslin","Leave : " fmt,__VA_ARGS__)

/*======================================================================*/
/*= Text output widgets (Single Line)                                  =*/

extern Tegs_widget widM_slout_nbf   (Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, CstrBrief width);
extern Tegs_widget widM_slout_nbf_DX(Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, int width);
extern Tegs_widget widM_slout_nbf_CN(Tvgui*gui, Tegs_widget pw,
                                     CstrBrief text, int cn);

extern Tegs_widget widD_slout_nbf   (Tvgui_dw*vdw,
                                     CstrBrief text, CstrBrief width);
extern Tegs_widget widD_slout_nbf_DX(Tvgui_dw*vdw,
                                     CstrBrief text, int width);
extern Tegs_widget widD_slout_nbf_CN(Tvgui_dw*vdw,
                                     CstrBrief text, int cn);

extern TstrEaten   wid_slout_getText   (Tegs_widget self);
extern void        wid_slout_setText   (Tegs_widget self,  CstrBrief str);
extern void        wid_slout_setTextWrn(Tegs_widget self,  CstrBrief str);
extern void        wid_slout_setTextErr(Tegs_widget self,  CstrBrief str);

// Interface to EGS.
extern Tegs_widget gui_slout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text);
extern TstrEaten
            gui_slout_getText   (Tegs_widget self);
extern void gui_slout_setText   (Tegs_widget self, CstrBrief str);
extern void gui_slout_setTextWrn(Tegs_widget self, CstrBrief str);
extern void gui_slout_setTextErr(Tegs_widget self, CstrBrief str);

#define DBG0_SloutE(fmt,...) DBG0_printf("widslout","Enter : "fmt,__VA_ARGS__)
#define DBG0_SloutM(fmt,...) DBG0_printf("widslout","Middle: "fmt,__VA_ARGS__)
#define DBG0_SloutL(fmt,...) DBG0_printf("widslout","Leave : "fmt,__VA_ARGS__)
#define DBG1_SloutE(fmt,...) DBG1_printf("widslout","Enter : "fmt,__VA_ARGS__)
#define DBG1_SloutM(fmt,...) DBG1_printf("widslout","Middle: "fmt,__VA_ARGS__)
#define DBG1_SloutL(fmt,...) DBG1_printf("widslout","Leave : "fmt,__VA_ARGS__)
#define DBG2_SloutE(fmt,...) DBG2_printf("widslout","Enter : "fmt,__VA_ARGS__)
#define DBG2_SloutM(fmt,...) DBG2_printf("widslout","Middle: "fmt,__VA_ARGS__)
#define DBG2_SloutL(fmt,...) DBG2_printf("widslout","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Text output widgets (Multiple Line with scroll bar)                =*/

typedef struct _Twid_mlout Twid_mlout;
typedef const   Twid_mlout Cwid_mlout;

struct _Twid_mlout {
    Tvgui*      gui;
    Tegs_widget widEGS;
};

extern Twid_mlout* widM_mlout_nbf   (Tvgui*gui, Tegs_widget pw,
                                     int cn, int ln, int sb);
extern Twid_mlout* widD_mlout_nbf   (Tvgui_dw*vdw,
                                     int cn, int ln, int sb);
extern void        wid_mlout_free   (Twid_mlout* self);

extern void wid_mlout_setText  (Twid_mlout* self, CstrBrief str); 

extern void wid_mlout_printf   (Twid_mlout* self, CstrBrief fmt, ...); 
extern void wid_mlout_printfWrn(Twid_mlout* self, CstrBrief fmt, ...); 
extern void wid_mlout_printfErr(Twid_mlout* self, CstrBrief fmt, ...); 

extern void wid_mlout_vprintf   (Twid_mlout* self,
                                 CstrBrief fmt, va_list ap); 
extern void wid_mlout_vprintfWrn(Twid_mlout* self,
                                 CstrBrief fmt, va_list ap); 
extern void wid_mlout_vprintfErr(Twid_mlout* self,
                                 CstrBrief fmt, va_list ap); 

extern Tegs_widget gui_mlout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        int sb);
extern void gui_mlout_setText   (Twid_mlout* self, CstrBrief str);
extern void gui_mlout_setTextWrn(Twid_mlout* self, CstrBrief str);
extern void gui_mlout_setTextErr(Twid_mlout* self, CstrBrief str);
extern void gui_mlout_appText   (Twid_mlout* self, CstrBrief str);
extern void gui_mlout_appTextWrn(Twid_mlout* self, CstrBrief str);
extern void gui_mlout_appTextErr(Twid_mlout* self, CstrBrief str);

#define DBG0_MloutE(fmt,...) DBG0_printf("widmlout","Enter : "fmt,__VA_ARGS__)
#define DBG0_MloutM(fmt,...) DBG0_printf("widmlout","Middle: "fmt,__VA_ARGS__)
#define DBG0_MloutL(fmt,...) DBG0_printf("widmlout","Leave : "fmt,__VA_ARGS__)
#define DBG1_MloutE(fmt,...) DBG1_printf("widmlout","Enter : "fmt,__VA_ARGS__)
#define DBG1_MloutM(fmt,...) DBG1_printf("widmlout","Middle: "fmt,__VA_ARGS__)
#define DBG1_MloutL(fmt,...) DBG1_printf("widmlout","Leave : "fmt,__VA_ARGS__)
#define DBG2_MloutE(fmt,...) DBG2_printf("widmlout","Enter : "fmt,__VA_ARGS__)
#define DBG2_MloutM(fmt,...) DBG2_printf("widmlout","Middle: "fmt,__VA_ARGS__)
#define DBG2_MloutL(fmt,...) DBG2_printf("widmlout","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Spinner widget.                                                    =*/

/**
 * The gui_spinner_create function creates a spinner widget as a
 * child of the p widget of the gui EGS.
 * cb(ud,1)  is trigerred when the user activates the up button.
 * cb(ud,-1) is trigerred when the user activates the down button.
 *
 * The spin spinner widget can be disabled (it does not trigger callback
 * anymore) and enabled (it triggers callback) with the functions
 * gui_spinner_/disable/enable functions.
 *
 * The gui_spinner_free function frees the memory allocated by the spin widget.
 *
 * Return: The gui_spinner_create function returns the created spinner.
**/

// iof:  >0:value, 0:auto, <0:error
extern Twid_spinner* wid_spinner_newInc(
                     Tvgui*      gui,  CstrBrief  fmt,
                     double      min,  double      max, double iof,
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud);
extern Twid_spinner* wid_spinner_newMul(
                     Tvgui*      gui,  CstrBrief  fmt,
                     double      min,  double      max, double iof,
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud);
extern Twid_spinner* wid_spinner_newRange(
                     Tvgui*      gui,  Crange*     r, 
                     Twid_cb_SUd val,  Twid_cb_SUd chg, void*  ud);
extern void          wid_spinner_update(Twid_spinner* spin,
                     CstrBrief  fmt,  double      val,
                     double      min,  double      max, double iof);
extern void          wid_spinner_update_minVal(Twid_spinner* spin, double v);
extern void          wid_spinner_update_maxVal(Twid_spinner* spin, double v);

extern void          wid_spinner_free   (Twid_spinner* spin);
extern void          wid_spinner_disable(Twid_spinner* spin);
extern void          wid_spinner_enable (Twid_spinner* spin);

extern double        wid_spinner_getNum (Twid_spinner* spin);
extern void          wid_spinner_setNum (Twid_spinner* spin, double v);

// Interface to EGS.
extern void          gui_spinner_dbf   (Twid_spinner* spin,
                                        Tvgui* gui,
                                        Tegs_widget pw); 
extern void          gui_spinner_free   (Twid_spinner* spin);
extern void          gui_spinner_update (Twid_spinner* spin);
extern TstrEaten     gui_spinner_setStr (Twid_spinner* spin);
extern void          gui_spinner_setNum (Twid_spinner* spin, double v);
extern void          gui_spinner_clear  (Twid_spinner* spin);
extern void          gui_spinner_enable (Twid_spinner* spin);
extern void          gui_spinner_disable(Twid_spinner* spin);

#define DBG0_SPINE(fmt,...) DBG0_printf("widspin","Enter : "fmt,__VA_ARGS__)
#define DBG0_SPINM(fmt,...) DBG0_printf("widspin","Middle: "fmt,__VA_ARGS__)
#define DBG0_SPINL(fmt,...) DBG0_printf("widspin","Leave : "fmt,__VA_ARGS__)
#define DBG1_SPINE(fmt,...) DBG1_printf("widspin","Enter : "fmt,__VA_ARGS__)
#define DBG1_SPINM(fmt,...) DBG1_printf("widspin","Middle: "fmt,__VA_ARGS__)
#define DBG1_SPINL(fmt,...) DBG1_printf("widspin","Leave : "fmt,__VA_ARGS__)
#define DBG2_SPINE(fmt,...) DBG2_printf("widspin","Enter : "fmt,__VA_ARGS__)
#define DBG2_SPINM(fmt,...) DBG2_printf("widspin","Middle: "fmt,__VA_ARGS__)
#define DBG2_SPINL(fmt,...) DBG2_printf("widspin","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= Slider widget.                                                     =*/

struct _Twid_slider {
    Tvgui*      gui;
    double      val;  // current value (in [0:1] range)
    double      inc;  // 1/N with N the number of steps
    Twid_cb_LUd chg;
    void*       ud;
    Tegs_widget widEGS;
};

extern Twid_slider* widM_slider_nbf( Tvgui* gui, Tegs_widget pw,
                                     double iv, double inc,
                                     Twid_cb_LUd  chg, void*  ud);
extern Twid_slider* widD_slider_nbf( Tvgui_dw* vdw,
                                     double iv, double inc,
                                     Twid_cb_LUd  chg, void*  ud);
extern void         wid_slider_free(Twid_slider*self);

extern double       wid_slider_setSize(Twid_slider*self,int dx,int dy);
extern double       wid_slider_get (Twid_slider*self);
extern void         wid_slider_set (Twid_slider*self, double v);
#define             wid_slider_enable(s)  gui_widegs_enable (s->widEGS)
#define             wid_slider_disable(s) gui_widegs_disable(s->widEGS)
extern Tegs_widget  gui_slider_create(Twid_slider*self,
                                      Tegs_widget pw, CstrBrief wn,
                                      int bdt, int dx, int dy);
extern double       gui_slider_get (Twid_slider*self);
extern void         gui_slider_set (Twid_slider*self, double v);

#define DBG0_SliderE(fmt,...) DBG0_printf("widslider","Enter : "fmt,__VA_ARGS__)
#define DBG0_SliderM(fmt,...) DBG0_printf("widslider","Middle: "fmt,__VA_ARGS__)
#define DBG0_SliderL(fmt,...) DBG0_printf("widslider","Leave : "fmt,__VA_ARGS__)
#define DBG1_SliderE(fmt,...) DBG1_printf("widslider","Enter : "fmt,__VA_ARGS__)
#define DBG1_SliderM(fmt,...) DBG1_printf("widslider","Middle: "fmt,__VA_ARGS__)
#define DBG1_SliderL(fmt,...) DBG1_printf("widslider","Leave : "fmt,__VA_ARGS__)
#define DBG2_SliderE(fmt,...) DBG2_printf("widslider","Enter : "fmt,__VA_ARGS__)
#define DBG2_SliderM(fmt,...) DBG2_printf("widslider","Middle: "fmt,__VA_ARGS__)
#define DBG2_SliderL(fmt,...) DBG2_printf("widslider","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= length unit widget                                                 =*/
/*= (purely VGUI)                                                      =*/

/**
 * The wid_lenunit_nbf function creates a unit DDL (Drop Down list) of
 * standard length units as a child of the d dialog widget.
 * The cb(self, unit, ud) is trigerred when the user changes the unit,
 * self beeing the widget itself and unit being a LUNIT_ID_... macro.
 * The initial unit that is shown is the LUNIT_ID_Inch.
 *
 * The wid_lenunit_nbf2 function is similar to the former but sets the
 * initial unit to iu. The value of iu must be a LUNIT_ID_... macro.
 *
 * The wid_lenunit_free function frees all the memory allocated by the
 * lu widget.
 *
 * The wid_lenunit_convToFU function converts the value v in the current
 * unit of lu widget to YFIG unit (FU: Fig Unit).
 * The wid_lenunit_convToWU does the reverse conversion (WU: Widget Wnit).
 *
 * The wid_lenunit_setUnit function changes to u (a LUNIT_ID_... macro)
 * the unit of the lu widget and then calls the callback "cb(self,u,ud).
 *
 * Return:
 *  - The wid_lenunit_nbf and wid_lenunit_nbf2 functions return the
 *    created widget.
 *  - The wid_lenunit_convToFU and wid_lenunit_convToWU functions
 *    return the converted value.
 *  - The wid_lenunit_getUnit function returns a LUNIT_ID_... macro.
**/

extern Twid_lenunit* wid_lenunit_nbf( Tvgui_dw*vdw,
                                      Twid_cb_LiU cb, void*ud);
extern Twid_lenunit* wid_lenunit_nbf2(Tvgui_dw*vdw,
                                      Twid_cb_LiU cb, void*ud, int u);
extern void          wid_lenunit_free(Twid_lenunit*lu);

extern double        wid_lenunit_convToFU(Cwid_lenunit*lu, double v);
extern double        wid_lenunit_convToWU(Cwid_lenunit*lu, double v);
extern int           wid_lenunit_getUnit( Cwid_lenunit*lu);
extern int           wid_lenunit_setUnit( Cwid_lenunit*lu, int u);
extern Tegs_widget   wid_lenunit_widget(  Cwid_lenunit*lu);

/*======================================================================*/
/*= Output widget                                                      =*/
/*= (purely VGUI)                                                      =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Base constructors and destructor                                   =*/

/**
 * The full layout of an output widget is:
 *   title + val + unit-DDL
 * Only the val widget is mandatory.
**/

extern Twid_olvu* wid_olvu_newT(CstrBrief iv, CstrBrief width);
// width and dx are exclusive
extern Twid_olvu* wid_olvu_newC(CstrBrief iv, CstrBrief width,
                                Ccolor*   ic, int dx, int dy);

/**
 * For numeric OLVU, the width parameter must be formated as for
 * Trange: 999.99
**/
extern Twid_olvu* wid_olvu_newN10(double v, CstrBrief width);
extern Twid_olvu* wid_olvu_newN11(double v, CstrBrief width, double v2);
extern void       wid_olvu_free(Twid_olvu*ow);
//extern void widM_olvu_dbf(Twid_olvu*ow, Tvgui*gui, Tegs_widget pw);
extern void widD_olvu_dbf(Twid_olvu*ow, Tvgui_dw*vdw);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= configuration                                                      =*/

// set the printf format (only for output numeric LVU)
// note that the values (v and v2) are double so for instance,
// to print 2 integers on 4 digits, you must use the format
// "%4.0f x %4.0f" 
extern void wid_olvu_setFmt    (Twid_olvu* ow, CstrBrief fmt);

extern void wid_olvu_addLabel  (Twid_olvu* ow, CstrBrief text,
                                               CstrBrief width);
extern void wid_olvu_addUnitStd(Twid_olvu* ow);
extern void wid_olvu_chgUnitStd(Twid_olvu* ow, int unit);
extern void wid_olvu_addUnitLab(Twid_olvu* ow, CstrBrief text,
                                               CstrBrief width);
// b must not be build
extern void wid_olvu_addButton (Twid_olvu* ow, Twid_button*b);
extern void wid_olvu_addPixDll (Twid_olvu* ow, TpixmapEGS*t, int nb);

extern void*wid_olvu_getUniWid (Twid_olvu* ow);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= get and change values and control                                  =*/

extern CstrBrief
              wid_olvu_getStr  (Cwid_olvu* self);
extern void   wid_olvu_setStr  (Twid_olvu* self, CstrBrief v);

extern double wid_olvu_getNum10(Cwid_olvu* self);
extern double wid_olvu_getNum01(Cwid_olvu* self);
extern void   wid_olvu_setNum10(Twid_olvu* self, double v);
extern void   wid_olvu_setNum01(Twid_olvu* self, double v2);
extern void   wid_olvu_setNum11(Twid_olvu* self, double v, double v2);

extern void   wid_olvu_reset   (Twid_olvu* self);
extern void   wid_olvu_clear   (Twid_olvu* self);
extern void   wid_olvu_unshow  (Twid_olvu* self);
extern void   wid_olvu_show    (Twid_olvu* self);
extern void   wid_olvu_disable (Twid_olvu* self);
extern void   wid_olvu_enable  (Twid_olvu* self);

extern void wid_olvu_setBgClr(Twid_olvu* self, Ccolor* c);
extern void wid_olvu_setBgPxm(Twid_olvu* self, TpixmapEGS pxm);
extern void wid_olvu_setBgBtm(Twid_olvu* self, Cvgui_icon* ico);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Quick constructors                                                 =*/

extern Twid_olvu* wid_olvu_newTT   (CstrBrief v, CstrBrief vw,
                                    CstrBrief t, CstrBrief tw);
extern Twid_olvu* wid_olvu_newN10T (double v,    CstrBrief fmt,
                                    CstrBrief t, CstrBrief tw);
extern Twid_olvu* wid_olvu_newN11T (double v,    CstrBrief fmt, double v2,
                                    CstrBrief t, CstrBrief tw);
extern Twid_olvu* wid_olvu_newN10TU(double v,    CstrBrief fmt,
                                    CstrBrief t, CstrBrief tw);
extern Twid_olvu* wid_olvu_newN11TU(double v, CstrBrief fmt, double v2,
                                        CstrBrief t, CstrBrief tw);
extern Twid_olvu* widD_olvu_nbfTT   (Tvgui_dw*vdw,
                                     CstrBrief v, CstrBrief vw,
                                     CstrBrief t, CstrBrief tw);
extern Twid_olvu* widD_olvu_nbfN10T (Tvgui_dw*vdw,
                                     double v, CstrBrief fmt,
                                     CstrBrief t, CstrBrief tw);
extern Twid_olvu* widD_olvu_nbfN11T (Tvgui_dw*vdw,
                                     double v, CstrBrief fmt, double v2,
                                     CstrBrief t, CstrBrief tw);
extern Twid_olvu* widD_olvu_nbfN10TU(Tvgui_dw*vdw,
                                     double v, CstrBrief fmt,
                                     CstrBrief t, CstrBrief tw);
extern Twid_olvu* widD_olvu_nbfN11TU(Tvgui_dw*vdw,
                                     double v, CstrBrief fmt, double v2,
                                     CstrBrief t, CstrBrief tw);

#define DBG0_OUTE(fmt,...) DBG0_printf("widoutput","Enter : "fmt,__VA_ARGS__)
#define DBG0_OUTM(fmt,...) DBG0_printf("widoutput","Middle: "fmt,__VA_ARGS__)
#define DBG0_OUTL(fmt,...) DBG0_printf("widoutput","Leave : "fmt,__VA_ARGS__)
#define DBG1_OUTE(fmt,...) DBG1_printf("widoutput","Enter : "fmt,__VA_ARGS__)
#define DBG1_OUTM(fmt,...) DBG1_printf("widoutput","Middle: "fmt,__VA_ARGS__)
#define DBG1_OUTL(fmt,...) DBG1_printf("widoutput","Leave : "fmt,__VA_ARGS__)
#define DBG2_OUTE(fmt,...) DBG2_printf("widoutput","Enter : "fmt,__VA_ARGS__)
#define DBG2_OUTM(fmt,...) DBG2_printf("widoutput","Middle: "fmt,__VA_ARGS__)
#define DBG2_OUTL(fmt,...) DBG2_printf("widoutput","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= input widget                                                       =*/
/*= (purely VGUI)                                                      =*/

/**
 * The full layout of an input widget is:
 *   label + tin + spinner + unit-DDL
 * Only the tin widget is mandatory.
**/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Base constructors and destructor                                   =*/

/**
 * The wid_ilvu_newT function creates the data structure for a widget
 * dedicated to get string typed by the user. The iv argument is the
 * initial value that the widget shows.
 *
 * The wid_ilvu_newI (D) function creates the data structure for a
 * widget dedicated to get integer (double) typed by the user.
 * The iv argument is the initial value that the widget shows.
 * If min is less than max the widget will accept only numbers in this
 * range included.
 *
 * The wid_ilvu_newDdlStr (Pxm) function creates the data structure for a
 * widget dedicated to select an entry among the strs strings (pxms pixmap).
 * The iv argument is the index in the strs (pxms) arrays of the initial
 * string (pixmap) that the widget shows.
 *
 * The widD_ilvu_dbf (DoBuildFinish) creates the widgets of the iw
 * iw input widgets as sub-widgets of the vdw dialog. 
 *
 * When the user validates the input, the widget triggers:
 *    val(d, p, v)
 * When the user changes the input, the widget triggers:
 *    chg(d, p, v)
 * Where d is the dialog set by widD_ilvu_dbf,
 *       p the pointer to the widget itself,
 *       v the value of the widget
 *           * const char* for wid_ilvu_newT.
 *           * int (double) for  wid_ilvu_newI (D).
 *           * int for wid_ilvu_newDll..., in this case v is the index
 *             of the DDL entry that the user selected.
 *
 * The wid_ilvu_free function frees all the memory allocated by the
 * data structure pointed to by the iw input widget.
 *   - For the widgets created by wid_ilvu_newDdlStr, the strs[i]
 *     strings and the strs table are not freed.
 *   - For the widgets created by wid_ilvu_newDdlPxm, the
 *     wid_ilvu_free function frees all the pixmaps (pxms[i] and the
 *     the pxms pixmap table itself.
 *     Use the  wid_ilvu_dontFreePxm function for disabling this feature.
**/
extern Twid_ilvu* wid_ilvu_newT(CstrBrief  iv,   CstrBrief   width,
                                Twid_cb_DIs vld, Twid_cb_DIs chg, void*ud);
extern Twid_ilvu* wid_ilvu_newI(double  iv,      CstrBrief   width,
                                Twid_cb_DIi vld, Twid_cb_DIi chg, void*ud,
                                int         min, int         max);
extern Twid_ilvu* wid_ilvu_newD(double      iv,  CstrBrief   width,
                                Twid_cb_DId vld, Twid_cb_DId chg, void*ud,
                                double      min, double      max);
extern Twid_ilvu* wid_ilvu_newA(double      iv,  CstrBrief   width,
                                Twid_cb_DId vld, Twid_cb_DId chg, void*ud,
                                double      min, double      max,
                                CstrBrief   labWidth);
//extern Twid_ilvu* wid_ilvu_newC(Ccolor*     iv,  CstrBrief   width);
extern Twid_ilvu* wid_ilvu_newDdlStr(int iv, char**strs,
                                     Twid_cb_Uap cb, void*ud);
extern Twid_ilvu* wid_ilvu_newDdlPxm(int iv, TpixmapEGS*ps,  int pn,
                                int dx, int dy, Twid_cb_Uap cb, void*ud);

extern void       widD_ilvu_dbf       (Twid_ilvu*iw, Tvgui_dw*vdw);
extern void       wid_ilvu_free       (Twid_ilvu* iw);
extern void       wid_ilvu_dontFreePxm(Twid_ilvu* iw);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= configuration                                                      =*/
/**
 * The next functions allows to add option to data structure of
 * the input widget.
 *
 * CAUTION:
 *   The functions must be used before the invocation of the widD_ilvu_dbf
 *   function.
**/

extern void     wid_ilvu_addSpinInc(Twid_ilvu* iw, double inc);
extern void     wid_ilvu_addSpinMul(Twid_ilvu* iw, double fac);
extern void     wid_ilvu_addLabel  (Twid_ilvu* iw, CstrBrief text,
                                                   CstrBrief width);
extern void     wid_ilvu_addUnitStd(Twid_ilvu* iw);
extern void     wid_ilvu_chgUnitStd(Twid_ilvu* iw, int unit);
extern void     wid_ilvu_addUnitLab(Twid_ilvu* iw, CstrBrief text,
                                                   CstrBrief width);
// b must not be build
extern void     wid_ilvu_addButton (Twid_ilvu* iw, Twid_button*b);
extern void     wid_ilvu_addPixDll (Twid_ilvu* iw, TpixmapEGS*t, int nb);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Using the input widget                                             =*/

/**
 * The wid_ilvu_getValDdl function returns the ddl widget used as value
 * in the iw widget or the NULL pointer if the value of the iw widget is
 * not implemented with a ddl widget.
 *
 * The wid_ilvu_getVallDdl_CurrIdAny function returns the identifier of
 * the current entry selected by the value ddl of the iw widget.
 * Caution: If the value of the iw widget is not implemented with a ddl
 * widget (wid_ilvu_getValDdl(iw) is NULL), it returns a tool_any_null.
**/ 
extern Twidm_ddl* wid_ilvu_getValDdl(Twid_ilvu* iw);
extern Tany       wid_ilvu_getValDdl_CurrIdAny(Twid_ilvu* iw);
extern void       wid_ilvu_setValDdl_CurrIdAny(Twid_ilvu* iw, Tany id);
Inline void       wid_ilvu_setValDdl_CurrIdInt(Twid_ilvu* iw, int id) { wid_ilvu_setValDdl_CurrIdAny(iw, any_initBySInt(id)); }

extern void*      wid_ilvu_getUniWid (Twid_ilvu* iw);

extern void       wid_ilvu_chgValMin(Twid_ilvu* iw, double v);
extern void       wid_ilvu_chgValMax(Twid_ilvu* iw, double v);

/**
 * The wid_ilvu_setStr functions sets the current string of the
 * inw widget to v and shows it.
 * The wid_ilvu_setNum functions sets the current value of the
 * inw widget to v and shows it.
 * The wid_ilvu_getStr functions returns the current string of the
 * inw widget.
 * The wid_ilvu_getNum functions returns the current value of the
 * inw widget. In case of an integer input widget, the returned value
 * is already an integer (coded into a double).
 * Ensure:
 *   - wid_set/get/Str assume that iw is a string input widget.
 *   - wid_set/get/Num assume that iw is an integer or double input
 *     widget.
**/
extern CstrBrief
              wid_ilvu_getStr  (Cwid_ilvu* self);
extern void   wid_ilvu_setStr  (Twid_ilvu* self, CstrBrief v);

extern double wid_ilvu_getNum10(Cwid_ilvu* self);
extern void   wid_ilvu_setNum10(Twid_ilvu* self, double v);

extern void   wid_ilvu_reset   (Twid_ilvu* self);
extern void   wid_ilvu_clear   (Twid_ilvu* self);
extern void   wid_ilvu_unshow  (Twid_ilvu* self);
extern void   wid_ilvu_show    (Twid_ilvu* self);
extern void   wid_ilvu_disable (Twid_ilvu* self);
extern void   wid_ilvu_enable  (Twid_ilvu* self);

#define DBG0_INPE(fmt,...) DBG0_printf("widinput","Enter : "fmt,__VA_ARGS__)
#define DBG0_INPM(fmt,...) DBG0_printf("widinput","Middle: "fmt,__VA_ARGS__)
#define DBG0_INPL(fmt,...) DBG0_printf("widinput","Leave : "fmt,__VA_ARGS__)
#define DBG1_INPE(fmt,...) DBG1_printf("widinput","Enter : "fmt,__VA_ARGS__)
#define DBG1_INPM(fmt,...) DBG1_printf("widinput","Middle: "fmt,__VA_ARGS__)
#define DBG1_INPL(fmt,...) DBG1_printf("widinput","Leave : "fmt,__VA_ARGS__)
#define DBG2_INPE(fmt,...) DBG2_printf("widinput","Enter : "fmt,__VA_ARGS__)
#define DBG2_INPM(fmt,...) DBG2_printf("widinput","Middle: "fmt,__VA_ARGS__)
#define DBG2_INPL(fmt,...) DBG2_printf("widinput","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
/*= PUM: PopUp Message widget                                          =*/

/**
 * The wid_pum_critical function builds a message using the fmt format
 * and the subsequent arguments as printf does it. Then it popups into
 * the gui GUI a dialog window containing the message and the bs
 * buttons.
 * It blocks the application until one clicks a button.
 * 
 * The wid_pum_criticalWBN function works like wid_pum_critical function
 * but allows to set the buttons labels. The list arguments starts by a zero
 * terminated set of couple (Twid_dialStatus, TstrBrief) and ends with
 * standard printf argument as shown below.
 *      Twid_dialStatus status = wid_pum_criticalWN(gui,
 *          WID_DIALSTATUS_Open, "Continue\ndiscarding change",
 *          WID_DIALSTATUS_Save, "Save current\nfigure and continue",
 *          WID_DIALSTATUS_Abort, NULL,
 *          WID_DIALSTATUS_None,
 *          "a %s %s with %d buttons", "popup", "dialog" 3);
 * The buttons are printed from left hand to right one. with the former example
 * we got:
 *  +--------------------+  +-------------------+  +-------+
 *  |      continue      |  |   save current    |  | Abort |
 *  | discarding changes |  | figure & continue |  |       |
 *  +--------------------+  +--------------------+  +------+
 * A NULL button label means the default button label.
 *
 * Return: It returns the clicked button.
**/
extern Twid_dialStatus wid_pum_critical(Tvgui_mw*vmw,
                            Twid_dialStatus butts, CstrBrief fmt, ...);

extern Twid_dialStatus wid_pum_criticalWBL(Tvgui_mw*vmw, ...);

/*======================================================================*/
/*= GNV: Get Numeric Value dialog.                                     =*/

extern Twid_dialStatus vdw_gnv_start(Tvgui_mw*vmw, CstrBrief title,
                                     Trange* range);

/*======================================================================*/
/*= GSLS: dialog to Get Single Line String.                            =*/

/**
 * The vdwM_gsls_new_CancelOk function creates a dialog asking for
 * a string with CANCEL and OK buttons.
 *
 * The vdwM_gsls_new_CancelContOk function creates a dialog asking
 * for a string with a CANCEL, CONT and OK buttons.
 * 
 * The vdw_gsls_bf function builds the d GSLS dialog and makes the
 * d GLSLS dialog ready to start.
 * 
 * The vdw_gsls_run function starts the d GLSL dialog in
 * a new top window.
 * The keyboard focus is set to the d dialog and the other widgets
 * of application are disable until the d dialog is closed.
 *
 * The vdw_gsls_run2 function runs the d GLSL dialog as the
 * vdwM_gsls_run2 function does but it places the GLSL dialog inside
 * the pw widget at (x,y) position.
 *
 * The vdwM_gsls_start_CancelOk function is a shortcut that chains
 * vdwM_gsls_new_CancelOk/bf/run functions.
 *
 * The vdwM_gsls_start_CancelContOk function is a shortcut that chains
 * vdwM_gsls_new_CancelContOk/bf/run functions.
 *
 * When the dialog is running:
 *  - The enter key leaves the dialog with the WID_DIALSTATUS_Ok status.
 *  - The enter key plus a meta key leaves the dialog with the
 *    WID_DIALSTATUS_Cont status. This is only available for GSLS with
 *    a CONT button.
 *  - The escape key leaves the dialog with the WID_DIALSTATUS_Cancel
 *    status.
 *
 * Return:
 *  The vdwM_gsls_new_CancelOk/CancelContOk functions returns the new
 *  created dialog.
 *
 * Parameter:
 *  horiz: 0:layout of dialog is done vertically 
 *         1:layout of dialog is done horizontally.
 *  pw:    The parent widget where dialog is done.
 *         If it  set to NULL, the parent dialog is created as a top
 *         level widget.
 *  x,y:   Position of the GSLS dialog inside the pw widget.
**/
extern Tvgui_dw* vdwM_gsls_new_CancelOk(
                               Tvgui_mw*vmw,   CstrBrief title, int hlayout,
                               Twid_cb_Us chg, Twid_cb_Us val, void*ud);
extern Tvgui_dw* vdwM_gsls_new_CancelContOk(
                               Tvgui_mw*vmw,   CstrBrief title, int hlayout,
                               Twid_cb_Us chg, Twid_cb_Us val, void*ud);
extern void      vdw_gsls_bf(  Tvgui_dw*self,
                               CstrBrief text, CstrBrief width);
extern void      vdw_gsls_free(Tvgui_dw*self);

extern Twid_dialStatus vdw_gsls_run (Tvgui_dw*self);
extern Twid_dialStatus vdw_gsls_run2(Tvgui_dw*self,
                                     Tegs_widget pw, int x, int y);

extern void            vdw_gsls_clear (Tvgui_dw*self);

extern Twid_dialStatus vdwM_gsls_start_CancelOk(
                                   Tvgui_mw*vmw, CstrBrief title,
                                   CstrBrief text, CstrBrief width,
                                   Twid_cb_Us chg, Twid_cb_Us val, void*ud,
                                   Tegs_widget pw, int horiz);

extern Twid_dialStatus vdwM_gsls_start_CancelContOk(
                                   Tvgui_mw*vmw,   CstrBrief title,
                                   CstrBrief text, CstrBrief width,
                                   Twid_cb_Us chg, Twid_cb_Us val, void*ud,
                                   Tegs_widget pw, int horiz);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_SIMPLE_H
/*======================================================================*/

