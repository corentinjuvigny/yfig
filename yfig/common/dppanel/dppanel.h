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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: dppanel.h 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/dppanel.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_DPPANEL_H
#define FILE_YFIG_DPPANEL_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-button.h"
#include "tools/strings.h"

#include "outdev/style.h"

/*======================================================================*/
/*= Some definitions and utilities                                     =*/

//      DPP_WID_DX is undefined, the ddp widget width is free
#define DPP_WID_DY 32   // height of ddp widget (in pixel)

// many dpp widgets have a NONE/UNDEFINED state. 
// please use thdpp_none string for representing this state.
extern const char* dpp_none;

// Make *dx and/or *dy large enough to contain the dpp_none string.
// If *dx (resp: *dy) is large enough, it is left unchanged.
// dx & dy are in pixel
extern void dpp_none_adjust(Tvgui*gui, int fi, double pts, int*dx, int*dy);
// x,y,dx,dy are in source unit.
extern void dpp_none_draw  (Tvgui_outdev*vod, double x, double y, double dx, double dy);

// dx & dy are in pixel
extern void l1l2_none_adjust (Tvgui*gui, int*dx, int*dy);

/*======================================================================*/
/*= The Tdpp_l1l2base widget is the base of all the DPP widget.        =*/
/*=                                                                    =*/
/*= It is a "very specialized" button, it handles 3 fields:            =*/
/*=   - l1: a first string                                             =*/
/*=   - l2: a second string                                            =*/
/*=   - v3: an abstract value                                          =*/
/*= l1 & l2 allow the user to identify the drawing parameter.          =*/
/*= v3 shows to the user the current value of the drawing parameter.   =*/
/*=                                                                    =*/
/*= The standard layout is:     LHSep             MHSep      RHSep     =*/
/*=   l1<-->NW-title           |     |           |     |    |     |    =*/
/*=   l1<-->NW-title           |     |NW-title   |     |data|     |    =*/
/*=                            |     |SW-titles..|     |data|     |    =*/
/*=   v3<-->data               |     |           |     |    |     |    =*/
#define DPP_V3STD_DX 20  // the preferred width  for data in std layout
#define DPP_V3STD_DY 15  // the preferred height for data in std layout

#define L1L2_FontId     gui->g_devFid      // for NW/SW-title
#define L1L2_FontSize   gui->g_devPtsSmall // for NW/SW-title
#define L1L2_FontSizeV3 gui->g_devPtsStd   // for data area
#define L1L2_VertShif -3  // the amount, the y positions of NW/SW-titles
                          // are shifted 
#define LeftHSep   3      // x of labels
#define MidHSep    5      // horizontal space between labels and data
#define RightHSep  3      // horizontal space at right hand of data

typedef struct _Tdpp_l1l2base Tdpp_l1l2base;
typedef const   Tdpp_l1l2base Cdpp_l1l2base;

struct _Tdpp_l1l2base {
    // layout 
    CstrStatic    ll_label1;   // left top label (text)
    CstrStatic    ll_label2;   // left bottom label (font)
    Tvgui_odpixmap*            // the device used for drawing the
                  ll_dev;      // pixmap.
#define ll_pixmap ll_dev->vv_pixmap
    int           ll_dx,ll_dy; // size of ll_pixmap
    int           ll_xl,ll_yl1,ll_yl2; // positition of the label NW corner.
    int           ll_xdt,ll_ydt;   // positition of the value NW corner.
    int           ll_dxdt,ll_dydt; // size of the value.

    Twid_button*  ll_btn;    // the button of widget

    // base actions of widget.
    // they are called by the default
    //   - callback function (dpp_l1l2base_cb)
    //   - drawing function  (dpp_l1l2base_draw) // in source unit
    //   - destructor        (dpp_l1l2base_delete)
    void (*ll_free)  (Tdpp_l1l2base*dt);
    void (*ll_cb1)   (Tdpp_l1l2base*dt, Tyfig_mw* ymw);
    void (*ll_inc)   (Tdpp_l1l2base*dt);
    void (*ll_dec)   (Tdpp_l1l2base*dt);
    void (*ll_redraw)(Tdpp_l1l2base*dt,Tvgui_outdev*vdev, double x, double y, double dx, double dy);
};

#define DT_ymw(dt) (Tyfig_mw*)wid_button_getMw(base.ll_btn))
#define dt_label1 base.ll_label1
#define dt_label2 base.ll_label2
#define dt_pod    base.ll_dev
#define dt_dev    base.ll_dev // obsolete
#define dt_pod    base.ll_dev
#define dt_od     base.ll_dev->vv_od
#define dt_pixmap base.ll_pixmap
#define dt_dx     base.ll_dx
#define dt_dy     base.ll_dy
#define dt_xl     base.ll_xl
#define dt_yl1    base.ll_yl1
#define dt_yl2    base.ll_yl2
#define dt_xdt    base.ll_xdt
#define dt_ydt    base.ll_ydt
#define dt_dxdt   base.ll_dxdt
#define dt_dydt   base.ll_dydt
#define dt_btn    base.ll_btn 
#define dt_free   base.ll_free
#define dt_cb1    base.ll_cb1
#define dt_inc    base.ll_inc
#define dt_dec    base.ll_dec
#define dt_redraw base.ll_redraw

/**
 * The dpp_l1l2base_init function initializes all the self data using
 * the l1 and l2 labels and the minimal size of the data area given
 * by the dx and dy parameters.
 *
 * The dpp_l1l2base_free function frees the memory the dpp_l1l2base_init
 * allocated.
 * 
 * The dpp_l1l2base_redraw function clears the pixmap and then redraws the
 * ll_label1 and ll_label2 labels at the (ll_xl,llyl1) and (ll_xl,ll__yl2)
 * positiions.
**/
// v3sep<=0 means default v3sep
extern int  dpp_l1l2base_getMaxV3Dy(Cvgui* gui, int v3sep);

extern Twid_button*
            dpp_l1l2base_initBtnStdPXM3_V3VSep(Tdpp_l1l2base*self,
                              Twid_buttonPanel* panel,
                              CstrStatic l1, CstrStatic l2,
                              int dx, int dy, int v3_vsep);
extern Twid_button*
            dpp_l1l2base_initBtnStdPXM3(Tdpp_l1l2base*self,
                              Twid_buttonPanel* panel,
                              CstrStatic l1, CstrStatic l2,
                              int dx, int dy);

extern void dpp_l1l2base_free  (Tdpp_l1l2base*self);
extern void dpp_l1l2base_delete(Tdpp_l1l2base*self);
extern void dpp_l1l2base_redrawBase(Tdpp_l1l2base*self);
extern void dpp_l1l2base_redrawFull(Tdpp_l1l2base*self);

/*======================================================================*/
/*= The Tdpp_l1l2v3 type defines the data of a button that contains    =*/
/*= 2 fixed labels and a number (integer or real).                     =*/ 
/*=  - a button1 click popups a dialog to change the value.            =*/ 
/*=  - a button2 click increments  the value (+ 1 unit).               =*/ 
/*=  - a button3 click deccrements the value (- 1 unit).               =*/ 

struct _Tdpp_l1l2v3 {
    CstrStatic label1,label2;
    CstrStatic fmt;
    double initval,minval,maxval; // initial, minimal, maximal value
    double incr;                  // increment 0=default
    double fact;                  // factor    0=no factor
};

typedef struct _Tdpp_l1l2v3 Tdpp_l1l2v3;
typedef const   Tdpp_l1l2v3 Cdpp_l1l2v3;

extern Twid_button*
            dpp_l1l2v3_newDble( Twid_buttonPanel* p, Cdpp_l1l2v3* llv,
                                                     double* piv);
extern Twid_button*
            dpp_l1l2v3_newInt ( Twid_buttonPanel* p, Cdpp_l1l2v3* llv,
                                                     int* piv);
extern void dpp_l1l2l3_setAction(Twid_button* btn,
                                 void(*cb)(Tyfig_mw*ymw, Tany av));

extern double dpp_l1l2v3_getVal( Cwid_button* btn);
extern void   dpp_l1l2v3_setVal( Cwid_button* btn, double v);
extern void   dpp_l1l2v3_update( Twid_button* btn, Tyfig_mw* ymw);

/*======================================================================*/
/*= The color selector button                                          =*/

extern Twid_button* dpp_l1l2color_newCLR( Twid_buttonPanel* panel,
                                      CstrStatic l1, CstrStatic l2,
                                      Tcolor**sync);
extern Twid_button* dpp_l1l2color_newDPF( Twid_buttonPanel* panel,
                                      CstrStatic l1, CstrStatic l2,
                                      Tsty_fill*sync);
extern void     dpp_l1l2color_update( Twid_button* btn, Tyfig_mw* ymw);

/*======================================================================*/
/*= top buttons                                                        =*/

extern Twid_button*
              dpp_zoom_new   (Twid_buttonPanel* panel,
                              Tvgui_outdev*dev, double*syn);
extern double dpp_zoom_getVal(Cwid_button* self);
extern void   dpp_zoom_setVal(Twid_button* self, double val);

extern Twid_button*
              dpp_gridshow_new(Twid_buttonPanel* panel);
extern double dpp_gridshow_getVal(Twid_button* self);
extern double dpp_gridshow_setVal(Twid_button* self,double);

extern Twid_button*
              dpp_gridmag_new(Twid_buttonPanel* panel);
extern double dpp_gridmag_getVal(Twid_button* self);
extern double dpp_gridmag_setVal(Twid_button* self,double);

extern Twid_button*
              dpp_align_new(Twid_buttonPanel* panel, int horiz);
extern int    dpp_align_getVal(Twid_button* self);

extern Twid_button*
              dpp_depth_new   (Twid_buttonPanel* panel, int*syn);
extern void   dpp_depth_update(Twid_button* self, Tyfig_mw* ymw);
extern int    dpp_depth_getVal(Cwid_button* self);
extern void   dpp_depth_setVal(Twid_button* self, int val);

extern Twid_button*
              dpp_angle_new   (Twid_buttonPanel* panel, double*syn);
extern double dpp_angle_getVal(Cwid_button* self);
extern void   dpp_angle_setVal(Twid_button* self, double val);

/*======================================================================*/
/*= buttons for line drawing                                           =*/

extern Twid_button* dpp_linewidth_new  (Twid_buttonPanel* panel, int*syn);
extern Twid_button* dpp_linecolor_new  (Twid_buttonPanel* panel,
                                        Tcolor** syn);
extern Twid_button* dpp_linestyle_new  (Twid_buttonPanel* panel,
                                        TlineStyle*syn);
extern Twid_button* dpp_linearrow_new  (Twid_buttonPanel* panel,
                                        CcolorSet* clrs);

extern void dpp_linewidth_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_linecolor_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_linestyle_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_linearrow_update(Twid_button* self, Tyfig_mw* ymw);

extern void dpp_line_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw);

/*======================================================================*/
/*= buttons for shape filling                                          =*/

extern Twid_button* dpp_fillcolor_new  (Twid_buttonPanel* panel,
                                        Tsty_fill*  syn);
extern Twid_button* dpp_filltile_new   (Twid_buttonPanel* panel,
                                        Tsty_fill* syn);

extern void dpp_linecolor_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_filltile_update (Twid_button* self, Tyfig_mw* ymw);

/*======================================================================*/
/*= buttons for text drawing                                           =*/

extern Twid_button* dpp_textdir_new  (Twid_buttonPanel* panel, int*syn);
extern Twid_button* dpp_textcolor_new(Twid_buttonPanel* panel,
                                      Tcolor** syn);
extern Twid_button* dpp_textsize_new (Twid_buttonPanel* panel, double*syn);
extern Twid_button* dpp_textfont_new (Twid_buttonPanel* panel, int*syn);
extern Twid_button* dpp_textangle_new(Twid_buttonPanel* panel, double*syn);
extern Twid_button* dpp_textLSF_new  (Twid_buttonPanel* panel, double*syn);

extern void dpp_textdir_update  (Twid_button* self, Tyfig_mw* ymw);
extern void dpp_textcolor_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_textsize_update (Twid_button* self, Tyfig_mw* ymw);
extern void dpp_textfont_update (Twid_button* self, Tyfig_mw* ymw);
extern void dpp_textangle_update(Twid_button* self, Tyfig_mw* ymw);
extern void dpp_textLSF_update  (Twid_button* self, Tyfig_mw* ymw);

/*======================================================================*/

#define DOPT_FMT     "option: %s %s : " 
#define DOPT_PSE     self->dt_label1,self->dt_label2
#define DOPT_PDT(dt) dt->ll_label1,dt->ll_label2

#define DOPT_FMTC        DOPT_FMT"%s : " 
#define DOPT_PSEC(c)     DOPT_PSE,DBG_Color2str(c)
#define DOPT_PDTC(dt,c)  DOPT_PDT(dt),DBG_Color2str(c)

#define DBG0_Dp(fmt,...)  DBG0_printf("drawparam",fmt,__VA_ARGS__)
#define DBG1_Dp(fmt,...)  DBG1_printf("drawparam",fmt,__VA_ARGS__)
#define DBG2_Dp(fmt,...)  DBG2_printf("drawparam",fmt,__VA_ARGS__)

#define DBG0_DpE(fmt,...) DBG0_printf("drawparam","Enter : "fmt,__VA_ARGS__)
#define DBG0_DpM(fmt,...) DBG0_printf("drawparam","Middle: "fmt,__VA_ARGS__)
#define DBG0_DpL(fmt,...) DBG0_printf("drawparam","Leave : "fmt,__VA_ARGS__)
#define DBG1_DpE(fmt,...) DBG1_printf("drawparam","Enter : "fmt,__VA_ARGS__)
#define DBG1_DpM(fmt,...) DBG1_printf("drawparam","Middle: "fmt,__VA_ARGS__)
#define DBG1_DpL(fmt,...) DBG1_printf("drawparam","Leave : "fmt,__VA_ARGS__)
#define DBG2_DpE(fmt,...) DBG2_printf("drawparam","Enter : "fmt,__VA_ARGS__)
#define DBG2_DpM(fmt,...) DBG2_printf("drawparam","Middle: "fmt,__VA_ARGS__)
#define DBG2_DpL(fmt,...) DBG2_printf("drawparam","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YFIG_DPPANEL_H
/*======================================================================*/
