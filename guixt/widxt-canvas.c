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
 *      $Id: widxt-canvas.c 252 2018-06-25 14:14:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-canvas.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales (EGS scale).            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for the type description,        =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-efsm.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/

static void gui_canvas_da_event(Widget w, XEvent *event, String *params,
                             Cardinal *num_params);
static void gui_canvas_ruler_event(Widget w, XEvent *event, String *params,
                             Cardinal *num_params);

static XtActionsRec	canvas_actions[] = {
    {"EventCanv",  gui_canvas_da_event},
    {"EventRuler", gui_canvas_ruler_event},
};

//#define DEBUG_BORDER
#define DEBUG_BORDER_MBD 10

#define DBG0(fmt,...)   DBG0_Canv(fmt,__VA_ARGS__)
#define DBG1(fmt,...)   DBG1_Canv(fmt,__VA_ARGS__)
#define DBG2(fmt,...)   DBG2_Canv(fmt,__VA_ARGS__)

/*======================================================================*/

// return 1 if the key is recognized otherwise it returns 0
static int gui_canvas_key(Twid_canvas*self,XKeyEvent *event,Tegs_widget xw)
{
    Tvgui* gui = self->cvs_gui;
    int w,kmod;
    kmod = conv_KeyMod_Xt2Vgui(event->state);
    w = xw==self->cvs_hruler.widEGS ? WIDCVS_WH_HR 
      : xw==self->cvs_vruler.widEGS ? WIDCVS_WH_VR
      : WIDCVS_WH_DA;
    KeySym keysym  = XkbKeycodeToKeysym(E_Display,event->keycode,0,(event->state&ShiftMask)!=0);
    switch ( keysym ) {
        case XK_Down:     wid_canvas_action(self,WIDCVS_KeyDown   ,w,kmod,0,0,0); return 1;
        case XK_Up:       wid_canvas_action(self,WIDCVS_KeyUp     ,w,kmod,0,0,0); return 1;
        case XK_Left:     wid_canvas_action(self,WIDCVS_KeyLeft   ,w,kmod,0,0,0); return 1;
        case XK_Right:    wid_canvas_action(self,WIDCVS_KeyRight  ,w,kmod,0,0,0); return 1;
        case XK_Page_Down:wid_canvas_action(self,WIDCVS_KeyPgDown ,w,kmod,0,0,0); return 1;
        case XK_Page_Up:  wid_canvas_action(self,WIDCVS_KeyPgUp   ,w,kmod,0,0,0); return 1;
      //case XK_ZoomIn:   wid_canvas_action(self,WIDCVS_KeyZoomP  ,w,kmod,0,0,0); return 1;
      //case XK_ZoomOut:  wid_canvas_action(self,WIDCVS_KeyZoomM  ,w,kmod,0,0,0); return 1;
    }
    return 0;
}

#define VG_XtResizeWidget(w,dx,dy,bd) XtResizeWidget(w,dx-2*bd,dy-2*bd,bd)

extern void gui_canvas_resize (Tvgui* gui, Twid_canvas* canvas,
        int x, int y, int dx, int dy)
{
    int mbd=1;
#ifdef  DEBUG_BORDER
    mbd=DEBUG_BORDER_MBD;
#endif
    DBG2_CanvE("canvas=%p: x,y,dx,dy=%d %d %d %d",canvas,x,y,dx,dy);
    XtMoveWidget  (canvas->vgcm_widEGS,x,y);
    VG_XtResizeWidget(canvas->vgcm_widEGS,dx,dy,mbd);

    Tegs_widget da = canvas->cvs_drawingArea.widEGS;
    int abd1=canvas->vgcm_daBW;
    int abd2= abd1*2;
    int hvs1= canvas->vgcm_daHVSep;

    int ax = hvs1;
    int ay = ax;
    int adx= dx- 2*mbd -2*ax-abd2 ; if (adx<=0) adx=1;
    int ady= dy- 2*mbd -2*ay-abd2 ; if (ady<=0) ady=1;
    if ( canvas->vgcm_scale==0 ) {
        XtMoveWidget  (canvas->vgcm_drawingArea.widEGS,ax,ay);
        XtResizeWidget(canvas->vgcm_drawingArea.widEGS,adx,ady,abd1);
        Box_x (canvas->vgcm_outdev->vv_boxDes) = 0;
        Box_y (canvas->vgcm_outdev->vv_boxDes) = 0;
        Box_dx(canvas->vgcm_outdev->vv_boxDes) = adx;
        Box_dy(canvas->vgcm_outdev->vv_boxDes) = ady;
        wid_canvas_boxPixUpdated(canvas);
    } else {
        Dimension hrdx,hrdy,vrdx,vrdy; 
        int       hrx,hry,hrbd1=0,vrx,vry,vrbd1=0;
        Tegs_widget hr = canvas->cvs_hruler.widEGS;
        Tegs_widget vr = canvas->cvs_vruler.widEGS;
        hrdy = canvas->cvs_hruler.fixdim; //XtVaGetValues(hr, XtNheight,&hrdy, NULL);
        vrdx = canvas->cvs_vruler.fixdim; //XtVaGetValues(vr, XtNwidth,&vrdx,  NULL);
        ay += hrdy;  adx -= vrdx; ady -= hrdy;
        if (adx<=0) adx=1;
        if (ady<=0) ady=1;
        hrx=ax-abd1; hry=0;       hrdx=adx;
        vrx=ax+adx;  vry=ay-abd1; vrdy=ady;
        XtMoveWidget  (da,ax,ay);
        XtResizeWidget(da,adx,ady,abd1);
        XtMoveWidget  (hr,hrx,hry);
        XtResizeWidget(hr,hrdx,hrdy,hrbd1);
        XtMoveWidget  (vr,vrx,vry);
        XtResizeWidget(vr,vrdx,vrdy,vrbd1);

        Box_x (canvas->vgcm_outdev->vv_boxDes) = 0;
        Box_y (canvas->vgcm_outdev->vv_boxDes) = 0;
        Box_dx(canvas->vgcm_outdev->vv_boxDes) = adx;
        Box_dy(canvas->vgcm_outdev->vv_boxDes) = ady;
        wid_canvas_boxPixUpdated(canvas);
    }
    DBG2_CanvL("canvas=%p: x,y,dx,dy=%d %d %d %d",canvas,x,y,dx,dy);
}

extern Tegs_widget  gui_canvas_buildTop  (Twid_canvas* self, Tegs_widget pw)
{
    Tvgui*      gui  = self->vgcm_gui;
    int mbd=1;
#ifdef  DEBUG_BORDER
    mbd=DEBUG_BORDER_MBD;
    self->vgcm_daBW    = 5;
    self->vgcm_daHVSep = 5;
#endif
    int lr = 2*mbd+2*self->vgcm_daHVSep+2*self->vgcm_daBW;
    int dx = self->vgcm_daWidth + lr;
    int dy = self->vgcm_daHVSep + lr;
    Widget widget;

    widget = create_form(
            gui, pw, self->vgcm_wn,
            mbd,self->vgcm_daBW, dx,dy, EGSCOLOR_default, EGSCOLOR_default);
    return widget;
}

extern void  gui_canvas_finishTop(Twid_canvas* self)
{
    Tvgui*      gui  = self->vgcm_gui;

    Widget    area   = self->vgcm_drawingArea.widEGS;
    TboxDble* box;
    box = &self->vgcm_outdev->vv_boxDes;
    Dimension dx=0,dy=0;
    XtVaGetValues(area, XtNwidth, &dx, XtNheight,&dy,NULL);
    Box_dx(*box) = dx;
    Box_dy(*box) = dy;
}

/*======================================================================*/
/*= the scales                                                         =*/

static void gui_canvas_ruler_event(Widget w, XEvent *event, String *params,
                             Cardinal *num_params)
{
    Twid_canvas* canvas=0;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p",&canvas)!=1,
            "Can not find the data pointer params=%p *param=%s",
            params, params==0 ? "" : *params);
    sscanf(*params,"%p",&canvas);

    int cmd  = canvas->cvs_hruler.widEGS==w ? WIDCVS_HR_BtnDown
                                            : WIDCVS_VR_BtnDown;
    switch ( event->type ) {
      case ButtonPress: {
          XButtonPressedEvent* e = (XButtonPressedEvent*)event;
          int btn  = conv_BtnNum_Xt2Vgui(e->button);
          if ( btn==0 ) return;
          TaccelDef kmod = conv_KeyMod_Xt2Vgui(e->state);
          DBG2_CanvE("canvas=%p : Button Press: bnum=%d : x=%03d y=%03d",
                canvas,btn, e->x,e->y);
          wid_canvas_action(canvas,cmd,e->x,e->y,btn,kmod,1);
          DBG2_CanvL("canvas=%p : Button press",canvas);
          return;
        }
      case KeyPress:
           gui_canvas_key(canvas,(XKeyEvent*)event,w);
           return;
      default:
          PERM_spw_printf("unexpected event (type=%d) is ignored.",
            event->type);
          DBG2_CanvL("canvas=%p : Expose event.",canvas);
          return;
    }
}
extern Tegs_widget  gui_canvas_buildScale(Twid_canvas_scale* scale,
                                          int horizontal)
{
    Tegs_widget widget;
    Twid_canvas* canvas = scale->canvas;
    Tvgui*       gui    = canvas->cvs_gui;
    Tegs_widget* parent = canvas->cvs_widEGS;
    widget = create_label(gui,parent,NULL,0,0,0,EGSCOLOR_none, EGSCOLOR_none,NULL);

    char tmp[1000]; tmp[0]=0;
    sprintf(tmp+strlen(tmp),"Any<BtnDown>:EventRuler(%p)\n",canvas);
    sprintf(tmp+strlen(tmp),"<KeyDown>:EventRuler(%p)\n",canvas);
    XtAugmentTranslations(widget, XtParseTranslationTable(tmp) );

    return widget;
}

/*======================================================================*/
/*= the drawing area                                                   =*/

static void gui_canvas_da_event(Widget w, XEvent *event, String *params,
                             Cardinal *num_params)
{
    Twid_canvas* canvas=0;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p",&canvas)!=1,
            "Can not find the data pointer params=%p *param=%s",
            params, params==0 ? "" : *params);
    sscanf(*params,"%p",&canvas);

    //Tvgui_device* vdev = &canvas->vgcm_device->vv_vdev;
    Tvgui_efsm*   fsm  = canvas->vgcm_automata;
    double        x,y;

    switch ( event->type ) {
      case Expose: {
          DBG2_CanvE("canvas=%p : Expose event.",canvas);
          wid_canvas_draw(canvas,NULL);
          DBG2_CanvL("canvas=%p : Expose event.",canvas);
          return;
        }
      case MotionNotify: {
          XMotionEvent* e = (XMotionEvent*)event;                   
          //DBG2_CanvE("canvas=%p : Mouse Motion : x=%03d y=%03d",canvas,e->x,e->y);
          wid_canvas_action(canvas,WIDCVS_DA_Motion,e->x,e->y,0,0,0);
          return;
        }
      case ButtonPress: {
          XButtonPressedEvent* e = (XButtonPressedEvent*)event;
          int btn  = conv_BtnNum_Xt2Vgui(e->button);
          if ( btn==0 ) return;
          TaccelDef kmod = conv_KeyMod_Xt2Vgui(e->state);
          DBG2_CanvE("canvas=%p : Button Press: bnum=%d : x=%03d y=%03d",
                canvas,btn, e->x,e->y);
          wid_canvas_action(canvas,WIDCVS_DA_BtnDown,e->x,e->y,btn,kmod,1);
          DBG2_CanvL("canvas=%p : Button press",canvas);
          return;
        }
      case EnterNotify:
          DBG2_CanvE("canvas=%p : Enter window.",canvas);
          wid_canvas_action(canvas,WIDCVS_DA_Enter,0,0,0,0,0);
          DBG2_CanvL("canvas=%p : Enter window.",canvas);
          return;
      case LeaveNotify:
          DBG2_CanvE("canvas=%p : Leave window.",canvas);
          wid_canvas_action(canvas,WIDCVS_DA_Leave,0,0,0,0,0);
          DBG2_CanvL("canvas=%p : Leave window.",canvas);
          return;
      case KeyPress:
           gui_canvas_key(canvas,(XKeyEvent*)event,w);
           return;
      default:
          PERM_spw_printf("unexpected event (type=%d) is ignored.",
            event->type);
          DBG2_CanvL("canvas=%p : Expose event.",canvas);
          return;
    }
}

extern Tegs_widget  gui_canvas_buildArea    (Twid_canvas_area*  area)
{
    Twid_canvas* canvas = area->canvas;
    Tvgui*       gui  = canvas->vgcm_gui;
    Widget       widget;
    XT_DeclareArgs(20);

    ArgCount=0;
    XT_NextArg(XtNlabel, "A A A");
    XT_NextArg(XtNbackground, CLRvgui2xt(gui->vg_daBackground));
    XT_NextArg(XtNforeground, CLRvgui2xt(gui->vg_daDrawColor));
    XT_NextArg(XtNborderWidth, 0);
    //XT_FirstArg(XtNhSpace, 0);
    //XT_FirstArg(XtNvSpace, 0);
    if ( canvas->vgcm_mode==0 ) {
        // canvas with input event
        XT_NextArg(XtNspace,0);
        XT_NextArg(XtNresizable,True);
        XT_NextArg(XtNwidth,  300);
        XT_NextArg(XtNheight, 300);
    } else {
        // preview
        XT_NextArg(XtNborderWidth, canvas->vgcm_daBW);
        //XT_NextArg(XtNspace,       canvas->vgcm_daHVSep);
        XT_NextArg(XtNresizable,   True);
        XT_NextArg(XtNwidth,       canvas->vgcm_daWidth);
        XT_NextArg(XtNheight,      canvas->vgcm_daHeight);
    }
    widget = XtCreateManagedWidget("drawingarea", labelWidgetClass,
            canvas->vgcm_widEGS, Args, ArgCount);
    if ( canvas->vgcm_mode==0 ) {
        widget_place_GrowToSE(widget,0,0,0,0);
        // canvas with input event
        XtAppAddActions(E_AppContext, canvas_actions, XtNumber(canvas_actions));
        char tmp[1000]; tmp[0]=0;
        sprintf(tmp+strlen(tmp),"<Expose>:EventCanv(%p)\n",canvas);
        sprintf(tmp+strlen(tmp),"<Motion>:EventCanv(%p)\n",canvas);
        sprintf(tmp+strlen(tmp),"Any<BtnDown>:EventCanv(%p)\n",canvas);
        sprintf(tmp+strlen(tmp),"<EnterWindow>:EventCanv(%p)\n",canvas);
        sprintf(tmp+strlen(tmp),"<LeaveWindow>:EventCanv(%p)\n",canvas);
        sprintf(tmp+strlen(tmp),"<KeyDown>:EventCanv(%p)\n",canvas);
        XtAugmentTranslations(widget, XtParseTranslationTable(tmp) );
    } else {
        int hvs = canvas->vgcm_daHVSep + canvas->vgcm_daBW;
        widget_place_GrowToSE(widget,hvs,0,hvs,0);
        // preview
        XtAppAddActions(E_AppContext, canvas_actions, XtNumber(canvas_actions));
        char tmp[1000];
        sprintf(tmp            ,"<Expose>:EventCanv(%p)\n",canvas);
        XtAugmentTranslations(widget, XtParseTranslationTable(tmp) );
    }
    return widget;
}

extern void         gui_canvas_clear        (Twid_canvas_area*  area)
{ 
    Twid_canvas* canvas = area->canvas;
    Tvgui*       gui    = canvas->vgcm_gui;
    XClearWindow(E_Display, _XtWindow(area->widEGS));
}

/*======================================================================*/
