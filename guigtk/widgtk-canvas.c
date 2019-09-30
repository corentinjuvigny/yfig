/*======================================================================
 *  Copyright 2018 Timothee Denoux
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
 *$Software: YFIG-GTK (Interface to Gtk-3.22)
 *      $Id: widgtk-canvas.c 111 2018-01-07 18:20:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/widgtk-canvas.c $
 *  $Author: Timothee Denoux (Email: timothee.denoux@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales (EGS scale).            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h and widgtk-canvas.h headers for the     =*/
/*= type description, the function description and for documentation.  =*/
/*======================================================================*/

#include "guigtk.h"
#include "widgtk-canvas.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/vgui-outdev.h"
#include "vgui/vgui-efsm.h"
#include "vgui/vgui-names.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/

//#define DEBUG_BORDER
#define DEBUG_BORDER_MBD 10

/*======================================================================*/

extern Tegs_widget gui_canvas_buildTop(Twid_canvas* self, Tegs_widget parent)
{
    Tvgui *gui = self->vgcm_gui;
    int borderWidth = 1;
    GtkWidget *widget;
    widget = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(widget),15);
    gtk_grid_set_column_spacing(GTK_GRID(widget),15);
    widget_setVguiData(widget,self);
#ifdef DEBUG_BORDER
    GdkRGBA red = {1,0,0,1};
    borderWidth = DEBUG_BORDER_MBD;
    widget_setBorderThClr(widget,borderWidth,&red);
#else
    widget_setBorderThClr(widget,borderWidth,NULL);
#endif    
    widget_setColors(gui,widget,wn_mainTop,gui->vg_background->egs,EGSCOLOR_none);
    if(self->vgcm_mode >= 0 && self->vgcm_mode <3) {
        gtk_widget_set_hexpand(widget,TRUE);
        gtk_widget_set_vexpand(widget,TRUE);
    } else {
        PERM_spe_printf("%d is a not supported mode",self->vgcm_mode);
    }
    return widget;
}

extern void  gui_canvas_finishTop(Twid_canvas* canvas)
{
    GtkWidget* hscroll = (GtkWidget*)canvas->vgcm_horizontalScale.widEGS;
    GtkWidget* vscroll = (GtkWidget*)canvas->vgcm_verticalScale.widEGS;
    GtkWidget* area    = (GtkWidget*)canvas->vgcm_drawingArea.widEGS;
    GtkWidget* top = canvas->vgcm_widEGS;
    if (hscroll) gtk_grid_attach(GTK_GRID(top),hscroll,0,0,1,1);
    if (vscroll) gtk_grid_attach(GTK_GRID(top),vscroll,1,1,1,1);
    if (area)    gtk_grid_attach(GTK_GRID(top),area,0,1,1,1);
    //gtk_widget_show(top);
    //gtk_widget_show(area);
    Box_x (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_y (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_dx(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = gtk_widget_get_allocated_width (area);
    Box_dy(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = gtk_widget_get_allocated_height(area);
}

/*======================================================================*/
/*= the rulers                                                         =*/

static int gui_canvas_key(Twid_canvas*self,GdkEventKey *event,Tegs_widget qw)
{
    int w,kmod;
    kmod = conv_KeyMod_Gtk2Vgui(event->state);
    w = qw==self->cvs_hruler.widEGS ? WIDCVS_WH_HR 
      : qw==self->cvs_vruler.widEGS ? WIDCVS_WH_VR
      : WIDCVS_WH_DA;
    switch ( event->keyval ) {
        case GDK_KEY_Down:      wid_canvas_action(self,WIDCVS_KeyDown   ,w,kmod,0,0,0); return 1;
        case GDK_KEY_Up:        wid_canvas_action(self,WIDCVS_KeyUp     ,w,kmod,0,0,0); return 1;
        case GDK_KEY_Left:      wid_canvas_action(self,WIDCVS_KeyLeft   ,w,kmod,0,0,0); return 1;
        case GDK_KEY_Right:     wid_canvas_action(self,WIDCVS_KeyRight  ,w,kmod,0,0,0); return 1;
        case GDK_KEY_Page_Down: wid_canvas_action(self,WIDCVS_KeyPgDown ,w,kmod,0,0,0); return 1;
        case GDK_KEY_Page_Up:   wid_canvas_action(self,WIDCVS_KeyPgUp   ,w,kmod,0,0,0); return 1;
#if 0
        case GDK_KEY_F1/*GDK_KEY_ZoomIn*/:   wid_canvas_action(self,WIDCVS_KeyZoomP  ,w,kmod,0,0,0); return 1;
        case GDK_KEY_F2/*GDK_KEY_ZoomOut*/:  wid_canvas_action(self,WIDCVS_KeyZoomM  ,w,kmod,0,0,0); return 1;
#endif
        default :
               TEMP_spw_printf("Key %d not recognized",event->keyval);
    }
    return 0;
}

#if 0
static gboolean scale_draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GdkPixbuf* pxm = widget_getatt_bgPXM(widget);
    gdk_cairo_set_source_pixbuf(cr,pxm,0,0);
    cairo_paint(cr);
    return TRUE;
}
#endif

static gboolean scale_enterEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{ gtk_widget_grab_focus(widget); return TRUE; }

static gboolean scale_leaveEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{ gtk_window_set_focus(GTK_WINDOW(gtk_widget_get_toplevel(widget)),NULL); return TRUE; }

static gboolean scale_mousePressEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    Twid_canvas *canvas = widget_getVguiData(widget);
    GdkEventButton *e = (GdkEventButton*) event;
    int btn;
    if ( (btn = conv_BtnNum_Gtk2Vgui(e))==0 ) {
        return FALSE;
    }
    int *data = (int *) user_data;
    int cmd = *data ? WIDCVS_HR_BtnDown : WIDCVS_VR_BtnDown; 
    TaccelDef kmod = conv_KeyMod_Gtk2Vgui(e->state);
    wid_canvas_action(canvas,cmd,e->x,e->y,btn,kmod,0);            
    return TRUE;
}

static gboolean scale_scrollEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    GdkEventScroll *e = (GdkEventScroll*) event;
    Twid_canvas *canvas = widget_getVguiData(widget);
    int kmod,btn,times = 0;
    int *data = (int *) user_data;
    btn = conv_wheel_Gtk2Vgui(event,&times,*data);
    kmod = conv_KeyMod_Gtk2Vgui(e->state);
    int cmd = *data ? WIDCVS_HR_BtnDown : WIDCVS_VR_BtnDown; 
    wid_canvas_action(canvas,cmd,e->x,e->y,btn,kmod,times);
    return TRUE;
}

static gboolean scale_keyPressEvent (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GdkEventKey *e = (GdkEventKey *) event;
    Twid_canvas *canvas = widget_getVguiData(widget);
    gui_canvas_key(canvas,e,widget);
    return TRUE;
}

static gboolean scale_resizeEvent(GtkWidget* widget, GdkEvent* event,
                                  gpointer user_data)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

extern Tegs_widget gui_canvas_buildScale(Twid_canvas_scale* scale,
                                         int horizontal)
{
    GtkWidget *widget;
    widget = gtk_drawing_area_new();
    
    GdkRGBA red = { 1,0,0,1 };
    widget_setBorderThClr(widget,3,&red);
    widget_setVguiData(widget,scale->canvas);
    if ( horizontal ) {
        gtk_widget_set_hexpand(widget,TRUE);
        gtk_widget_set_vexpand(widget,FALSE);
    }
    else {
        gtk_widget_set_hexpand(widget,FALSE);
        gtk_widget_set_vexpand(widget,TRUE);
    }
    //signals
    gtk_widget_set_events (widget, gtk_widget_get_events (widget)
                                    | GDK_BUTTON_PRESS_MASK
                                    | GDK_FOCUS_CHANGE_MASK
                                    | GDK_SCROLL_MASK
                                    | GDK_KEY_PRESS_MASK
                                    | GDK_ENTER_NOTIFY_MASK
                                    | GDK_LEAVE_NOTIFY_MASK);
    gtk_widget_set_can_focus(widget,TRUE);
    int *data = malloc(sizeof(data));
    *data = horizontal;
    g_signal_connect(widget,"draw",G_CALLBACK(widget_fctatt_bgPXM),NULL);
    g_signal_connect(widget,"focus-in-event",G_CALLBACK(scale_enterEvent),NULL);
    g_signal_connect(widget,"focus-out-event",G_CALLBACK(scale_leaveEvent),NULL);
    g_signal_connect(widget,"button-press-event",G_CALLBACK(scale_mousePressEvent),data);
    g_signal_connect(widget,"scroll-event",G_CALLBACK(scale_scrollEvent),data);
    g_signal_connect(widget,"key-press-event",G_CALLBACK(scale_keyPressEvent),NULL);
    g_signal_connect(widget,"configure-event",G_CALLBACK(scale_resizeEvent), data);

    return widget;
}

/*======================================================================*/
/*= the drawing area                                                   =*/

static gboolean da_resizeEvent(GtkWidget *widget, GdkEvent *event,
                               gpointer user_data)
{ 
    Twid_canvas *canvas = widget_getVguiData(widget);
    GtkWidget* hscroll = (GtkWidget*)canvas->vgcm_horizontalScale.widEGS;
    GtkWidget* vscroll = (GtkWidget*)canvas->vgcm_verticalScale.widEGS;
    GdkEventConfigure *e = (GdkEventConfigure*) event;
    int dx = e->width;
    int dy = e->height;

    odcvs_chgOdData(widget);
    Box_x (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_y (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_dx(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = dx;
    Box_dy(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = dy;
    wid_canvas_boxPixUpdated(canvas);

    return TRUE;
}

static gboolean da_enterEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    Twid_canvas *canvas = widget_getVguiData(widget);
    if ( canvas->vgcm_mode==0 ) {
        gtk_widget_grab_focus(widget);
        wid_canvas_action(canvas,WIDCVS_DA_Enter,0,0,0,0,0);
    }
    return TRUE;
}

static gboolean da_leaveEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    Twid_canvas *canvas = widget_getVguiData(widget);
    if ( canvas->vgcm_mode==0 ) {
        gtk_window_set_focus(GTK_WINDOW(gtk_widget_get_toplevel(widget)),NULL);
        wid_canvas_action(canvas,WIDCVS_DA_Leave,0,0,0,0,0);
    }
    return TRUE;
}

static gboolean da_mousePressEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    Twid_canvas *canvas = widget_getVguiData(widget);
    GdkEventButton *e = (GdkEventButton*) event;
    int btn;
    if ( (btn = conv_BtnNum_Gtk2Vgui(e))==0 ) {
        return FALSE;
    }
    TaccelDef kmod = conv_KeyMod_Gtk2Vgui(e->state);
    wid_canvas_action(canvas,WIDCVS_DA_BtnDown,e->x,e->y,btn,kmod,1);            
    return TRUE;
}

static gboolean da_scrollEvent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    GdkEventScroll *e = (GdkEventScroll*) event;
    Twid_canvas *canvas = widget_getVguiData(widget);
    int kmod,btn,times = 1;
    int x = e->x, y = e->y;
    btn = conv_wheel_Gtk2Vgui(event,&times,0);
    kmod = conv_KeyMod_Gtk2Vgui(e->state);
    wid_canvas_action(canvas,WIDCVS_DA_BtnDown,x,y,btn,kmod,times);
    return TRUE;
}

static gboolean da_keyPressEvent (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GdkEventKey *e = (GdkEventKey *) event;
    Twid_canvas *canvas = widget_getVguiData(widget);
    gui_canvas_key(canvas,e,widget);
    return TRUE;
}

static gboolean da_mouseMoveEvent (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    GdkEventMotion *e = (GdkEventMotion *) event;
    Twid_canvas *canvas = widget_getVguiData(widget);
    int x = e->x;
    int y = e->y;
    wid_canvas_action(canvas,WIDCVS_DA_Motion,x,y,0,0,0);
    return TRUE;
}

static gboolean da_draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    Tgui_daData *dt = odcvs_getOdData(widget);
    TEMP_asf_printf(dt->in_ms==1 ,"%s","invalid call in std section");
    TEMP_asf_printf(dt->in_xds==1,"%s","invalid call in xorDrawSection");
    TEMP_asf_printf(dt->in_xes==1,"%s","invalid call in xorEraseSection");
    cairo_surface_t *surface = cairo_get_target(dt->main_gc);
    cairo_set_source_surface(cr,surface,0,0);
    cairo_paint(cr);
    return TRUE;
}

extern Tegs_widget  gui_canvas_buildArea    (Twid_canvas_area*  area)
{
    Twid_canvas *canvas = area->canvas;
    GtkWidget *drawing_area = gtk_drawing_area_new();  
    widget_setVguiData(drawing_area,canvas);
    //signals
    gtk_widget_set_can_focus(drawing_area,TRUE);
    if ( !canvas->vgcm_mode ) {
      g_signal_connect(drawing_area,"draw",               G_CALLBACK(da_draw_callback),  NULL);
      g_signal_connect(drawing_area,"configure-event",    G_CALLBACK(da_resizeEvent),    NULL);
      g_signal_connect(drawing_area,"enter-notify-event", G_CALLBACK(da_enterEvent),     NULL);
      g_signal_connect(drawing_area,"leave-notify-event", G_CALLBACK(da_leaveEvent),     NULL);
      g_signal_connect(drawing_area,"button-press-event", G_CALLBACK(da_mousePressEvent),NULL);
      g_signal_connect(drawing_area,"scroll-event",       G_CALLBACK(da_scrollEvent),    NULL);
      g_signal_connect(drawing_area,"key-press-event",    G_CALLBACK(da_keyPressEvent),  NULL);
      g_signal_connect(drawing_area,"motion-notify-event",G_CALLBACK(da_mouseMoveEvent), NULL);
    } else {
      g_signal_connect(drawing_area,"draw",               G_CALLBACK(da_draw_callback),  NULL);
      g_signal_connect(drawing_area,"configure-event",    G_CALLBACK(da_resizeEvent),    NULL);
    }

    int borderWidth=0, minWidth,minHeight;
    if ( canvas->vgcm_mode==0 ) {
        minWidth  = 600;
        minHeight = 300;
        gtk_widget_set_size_request(drawing_area,minWidth,minHeight);
        gtk_widget_set_hexpand(drawing_area,TRUE);
        gtk_widget_set_vexpand(drawing_area,TRUE);
        GdkRGBA red = {1,0,0,1};
        widget_setBorderThClr(drawing_area,5,&red);
    } else {
        borderWidth = canvas->vgcm_daBW;
        if ( borderWidth>0 ) {
            widget_setBorderThClr(drawing_area,borderWidth,NULL);
        } 
        minWidth  = canvas->vgcm_daWidth;
        minHeight = canvas->vgcm_daHeight;
        gtk_widget_set_size_request(drawing_area,minWidth,minHeight);
        gtk_widget_set_hexpand(drawing_area,TRUE);
        gtk_widget_set_vexpand(drawing_area,TRUE);
    }
    //Tvgui* gui =canvas->cvs_gui;
    //widget_setColors(gui,drawing_area,wn_mainTop,gui->vg_grayMedian->egs,EGSCOLOR_none);
#ifdef DEBUG_BORDER
        GdkRGBA magenta = {0,1,0,1};
        widget_setBorderThClr(drawing_area,10,&magenta);
#endif
    gtk_widget_set_events (drawing_area, gtk_widget_get_events (drawing_area)
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_FOCUS_CHANGE_MASK
                                     | GDK_SCROLL_MASK
                                     | GDK_KEY_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK
                                     | GDK_ENTER_NOTIFY_MASK
                                     | GDK_LEAVE_NOTIFY_MASK);

    return drawing_area;
}

extern void gui_canvas_clear(Twid_canvas_area*  area)
{
    GtkWidget*   widget = (GtkWidget*)area->widEGS;  
    Tgui_daData* dt     = odcvs_getOdData(widget);
    PERM_error_NYI();
    //cairo_t *cr         = cairo_create(cairo_get_target(widget_getGCbyWid(widget)));
    //cairo_paint(cr);
}

/*======================================================================*/
/*= Interface to Tgui_daData                                           =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= constructor & destructor.                                          =*/

#define ODDATA "oddata"
extern void odcvs_chgOdData(GtkWidget* w)
{
    int i;
    Tgui_daData *dt = g_object_get_data(G_OBJECT(w),ODDATA);
    if (dt == 0) return;
    if (dt->main_gc) cairo_destroy(dt->main_gc);
    for (i = 0; i < OD_DS_NBXOR; i++)
        if (dt->main_gc_cpy[i]) cairo_surface_destroy(dt->main_gc_cpy[i]);
    if (dt->xor_gc) cairo_destroy(dt->xor_gc);
    tool_free(dt);
    g_object_set_data(G_OBJECT(w),ODDATA,NULL);
}

static Tgui_daData *odcvs_getOdDataByObjDxDy(GObject *obj, int dx, int dy)
{
    int i;
    Tgui_daData *dt = g_object_get_data(obj,ODDATA);
    if (dt != 0) {
        return dt;
    }
    dt = tool_malloc(sizeof(*dt));
    dt->main_gc = cairo_create(
                    gdk_window_create_similar_surface(
                        gdk_get_default_root_window(),
                        CAIRO_CONTENT_COLOR, dx,dy));
    for (i = 0; i < OD_DS_NBXOR; i++) {
        dt->main_gc_cpy[i] = cairo_surface_create_similar_image(cairo_get_target(dt->main_gc), CAIRO_FORMAT_ARGB32,dx,dy);
    }
    dt->xor_gc =cairo_create(cairo_surface_create_similar_image(cairo_get_target(dt->main_gc), CAIRO_FORMAT_ARGB32,dx,dy));
    dt->in_ms = 0;
    dt->in_xds = 0;
    dt->in_xes = 0;
    g_object_set_data(obj,ODDATA,dt);
    return dt;
}

extern Tgui_daData *odcvs_getOdData(GtkWidget *widget)
{ return odcvs_getOdDataByObjDxDy(G_OBJECT(widget),gtk_widget_get_allocated_width(widget),gtk_widget_get_allocated_height(widget));}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= enter and leave sections.                                          =*/

extern void odvt_ds_stdStartCvs      (const void*dev)
{
    Cvgui_outdev* vod    = dev;
    GtkWidget*    widget = (GtkWidget *)vod->v_cvsWidEGS;
    Tgui_daData*  dt     = odcvs_getOdData(widget);
    dt->in_ms = 1;
}
extern void odvt_ds_stdCommitCvs     (const void*dev)
{
    Cvgui_outdev* vod    = dev;
    GtkWidget*    widget = (GtkWidget *)vod->v_cvsWidEGS;
    Tgui_daData*  dt     = odcvs_getOdData(widget);
    dt->in_ms = 0;
    gtk_widget_queue_draw(widget);
}

extern void odvt_ds_xorDrawStartCvs(const void*dev, int sn)
{
    Tvgui_odcanvas *cod = (void *)dev;
    GtkWidget *widget   = cod->vv_cvsWidEGS;
    Tgui_daData *dt     = odcvs_getOdData(widget);

    // initialze all the pixel of the xor surface to the full transparent black
    cairo_set_operator(dt->xor_gc,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(dt->xor_gc,0,0,0,0);
    cairo_paint(dt->xor_gc);

    // save the image drawn on the canvas into the main_gc_cpy[sn] image
    cairo_surface_t *main_s = cairo_get_target(dt->main_gc);
    cairo_t*        cr      = cairo_create(dt->main_gc_cpy[sn]);
    cairo_set_source_surface(cr, main_s, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);

    // xor drawing section is started
    dt->in_xds = 1;
}

extern void odvt_ds_xorDrawCommitCvs(const void*dev, int sn)
{
    Tvgui_odcanvas *cod = (void *)dev;
    GtkWidget *widget = cod->vv_cvsWidEGS;
    Tgui_daData *dt = odcvs_getOdData(widget);

    // we leave the xor drawing section
    dt->in_xds = 0;

    // write the xor_gc image into the canvas surface
    cairo_surface_t *surface = cairo_get_target(dt->xor_gc);
    cairo_t *cr = dt->main_gc;
    cairo_set_operator(cr,CAIRO_OPERATOR_ATOP);
    cairo_set_source_surface(cr,surface,0,0);
    cairo_paint(cr);

    // write the canvas surface on the screen
    gtk_widget_queue_draw_area(widget,0,0, Box_dx(cod->vv_boxDes),Box_dy(cod->vv_boxDes));
}

extern void odvt_ds_xorEraseStartCvs(const void*dev, int sn)
{   
    Tvgui_odcanvas *cod = (void *)dev;
    GtkWidget *widget = cod->vv_cvsWidEGS;
    Tgui_daData *dt = odcvs_getOdData(widget);

    // write the main_gc_cpy[sn] image into the canvas surface,
    // so all the drawings that have been done in the sn drawing section
    // are erased.
    cairo_t *cr = dt->main_gc;
    cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(cr,dt->main_gc_cpy[sn],0,0);
    cairo_paint(cr);

    // write the canvas surface on the screen
    //gtk_widget_queue_draw_area(widget,0,0, Box_dx(cod->vv_boxDes),Box_dy(cod->vv_boxDes));
    gtk_widget_queue_draw(widget);

    // xor erase section is started
    dt->in_xes = 1;
}

extern void odvt_ds_xorEraseCommitCvs(const void*dev, int sn)
{
    Tvgui_odcanvas *cod = (void*)dev;
    GtkWidget *widget = cod->vv_cvsWidEGS;
    Tgui_daData *dt = odcvs_getOdData(widget);

    // we leave the xor erase section
    dt->in_xes = 0;
    gtk_widget_queue_draw(widget);
}

/*======================================================================*/    
