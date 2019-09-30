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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-util.h 330 2018-10-30 21:26:32Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guixt/guixt-util.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_UTIL_H
#define FILE_GUIXT_UTIL_H
/*======================================================================*/

/*======================================================================*/
/*= miscellaneous converters                                           =*/

// return 0:unknown button; 1/2/3/4/5:left/middle/right/wheel button
extern int       conv_BtnNum_Xt2Vgui(int  xnb);
extern TaccelDef conv_KeyMod_Xt2Vgui(int  xkmod);

// return a default value when ls, js or cs are bad or unsuported.
extern int conv_lineStyle_Od2X11(TlineStyle ls);
extern int conv_joinStyle_Od2X11(TjoinStyle js);
extern int conv_capStyle_Od2X11(TcapStyle cs);

/*======================================================================*/
/*= miscellaneous utilities                                            =*/
 
extern void widget_getColors(Tvgui*gui, CstrBrief wn,
                             TcolorEGS*bg, TcolorEGS*fg);

extern CstrBrief widget_getLabel (Widget w);
extern CstrBrief widget_getString(Widget w);
extern void      widget_setLabel (Widget w, CstrBrief text);
extern void      widget_setString(Widget w, CstrBrief text);
 
/*======================================================================*/
/*= widget geometry                                                    =*/

// _x, _y, _dx, _dy can be set to NULL pointer
extern int  widget_bdw     (Widget w);
extern void widget_size    (Widget w, int*_dx, int*_dy);
extern void widget_geometry(Widget w, int*_x, int*_y, int*_dx, int*_dy);

extern void widget_setSameDx(Cvgui*gui,...);
extern void widget_setSameDy(Cvgui*gui,...);

/**
 * widget_resize_toEast resizes the w widget to fill the space
 * at its right hand until the west border of the ew widget.
 * if lw is null, the parent east border is used. 
 *
 * widget_resize_toSouth resizes the w widget to fill the space
 * below it until the north border of the sw widget.
 * if sw is null, the parent south border is used. 
**/
extern void widget_resize_toEast (Widget w, Widget ew);
extern void widget_resize_toSouth(Widget w, Widget sw);
extern void widget_resize_toSE   (Widget w, Widget sw);

/*======================================================================*/
/*= place wideget in container                                         =*/

#define WPW_FixedSize    XtChainTop,XtChainTop,XtChainLeft,XtChainLeft
#define WPW_FixedSizeSE  XtChainBottom,XtChainBottom,XtChainLeft,XtChainLeft
#define WPW_GrowRight    XtChainTop,XtChainTop,XtChainLeft,XtChainRight
#define WPW_GrowRightSW  XtChainBottom,XtChainBottom,XtChainLeft,XtChainRight
#define WPW_GrowBot      XtChainTop,XtChainBottom,XtChainLeft,XtChainLeft
#define WPW_GrowRB       XtChainTop,XtChainBottom,XtChainLeft,XtChainRight
extern void widget_place(Tegs_widget w,
        int top, int   bot, int   left, int   right,
        int hspace, Tegs_widget hwidget,
        int vspace, Tegs_widget vwidget);
#define widget_place_widget widget_place
#define widget_place_FixedSize(w,ld,lw,td,tw)    widget_place(w,WPW_FixedSize,   ld,lw,td,tw)
#define widget_place_FixedSizeSE(w,ld,lw,td,tw)  widget_place(w,WPW_FixedSizeSE, ld,lw,td,tw)
#define widget_place_GrowToEast(w,ld,lw,td,tw)   widget_place(w,WPW_GrowRight,   ld,lw,td,tw)
#define widget_place_GrowToEastSW(w,ld,lw,td,tw) widget_place(w,WPW_GrowRightSW, ld,lw,td,tw)
#define widget_place_GrowToSouth(w,ld,lw,td,tw)  widget_place(w,WPW_GrowBot,     ld,lw,td,tw)
#define widget_place_GrowToSE(w,ld,lw,td,tw)     widget_place(w,WPW_GrowRB,      ld,lw,td,tw)

/*======================================================================*/
/*= here some shared functions avoid very small headers.               =*/
  
// between widxt_button.c and widxt_buttonPanel.c
extern void gui_button_onoff_cb(Widget widget, XtPointer _button,
        XEvent *event, Boolean *continue_to_dispatch);

// usefull if the size of a main widget changes 
extern void gui_canvas_resize (Tvgui* gui, Twid_canvas* canvas,
        int x, int y, int dx, int dy);

/**
 * usefull when the size of a dialog widget change and standard
 * XT resizing feature fails to render a pretty layout.  
 *
 * Add the cb callback to the d dialog Widget. The cb function is
 * called every times the size of the d widget changes.
**/
typedef int (*Txt_dw_cb_resize)(Tvgui_dw*vdw);
extern void xt_dw_addCBresize(Tvgui_dw*d, Txt_dw_cb_resize cb);

// interface to filebrowser widget
//extern void      xt_filebrowser_dbf      (Twid_filebrowser* fbd);
extern void      xt_filebrowser_resize   (Twid_filebrowser* fbd);
//extern Widget    xt_filebrowser_getWidget(Twid_filebrowser* fbd);

// initialization and free functions used once by guixt_init.c
extern void gui_pixmaps_init(Tvgui* gui);
extern void gui_pixmaps_free(Tvgui* gui);
extern void gui_gc_create_font(Tvgui* gui, Tod_GCdef gcd,
                int fi, double pts, Ccolor* fg, Ccolor* bg);
extern void gui_gc_free_font  (Tvgui* gui, Tod_GCdef gcd);

/*======================================================================*/
#endif // FILE_GUIXT_UTIL_H
/*======================================================================*/
