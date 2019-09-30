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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: widqt-canvas.cc 341 2018-11-06 18:38:47Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-canvas.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional rulers (EGS scale).            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for the type description,        =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guiqt.h"
#include "guiqt/widqt-canvas.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/vgui-efsm.h"

/*======================================================================*/

//#define DEBUG_BORDER
#define DEBUG_BORDER_MBD 10

#define DBG0(fmt,...)   DBG0_Canv(fmt,__VA_ARGS__)
#define DBG1(fmt,...)   DBG1_Canv(fmt,__VA_ARGS__)
#define DBG2(fmt,...)   DBG2_Canv(fmt,__VA_ARGS__)

/*======================================================================*/

TGUI_canvas::TGUI_canvas(Twid_canvas *g, QWidget*parent)
: QFrame(parent)
{
    int borderWidth = 1;
    gui    = g->vgcm_gui;
    canvas = g;
    grid   = 0;
#ifdef DEBUG_BORDER 
    QPalette pal = palette();
    pal.setColor(QPalette::Window,     Qt::green);
    pal.setColor(QPalette::WindowText, Qt::red);
    setAutoFillBackground(true);
    setPalette(pal);
    borderWidth = DEBUG_BORDER_MBD;
#else
    QPalette pal = palette();
    pal.setColor(QPalette::Window, COLOR(gui->vg_background)); 
    //pal.setColor(QPalette::Button, COLOR(gui->vg_buttonOffBackground));
    setAutoFillBackground(true);
    setPalette(pal);
#endif
    setLineWidth(borderWidth);
    setFrameStyle(QFrame::Box|QFrame::Plain);
    if ( canvas->vgcm_mode==0 ) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else if ( canvas->vgcm_mode==1 ) {
        setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    } else if ( canvas->vgcm_mode==2 ) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        PERM_spe_printf("%d is a not supported mode",canvas->vgcm_mode);
    }
}

TGUI_canvas::~TGUI_canvas()
{
    //setLayout(0);
    //delete grid;
}

void TGUI_canvas::creation_finish()
{
    int dx,dy;
    QWidget* hscroll = (QWidget*)canvas->vgcm_horizontalScale.widEGS;
    QWidget* vscroll = (QWidget*)canvas->vgcm_verticalScale.widEGS;
    QFrame* area     = (QFrame*)canvas->vgcm_drawingArea.widEGS;
    grid = new QGridLayout;
    grid->setSpacing(10);
    if (hscroll) grid->addWidget(hscroll,0,0);
    if (vscroll) grid->addWidget(vscroll,1,1);
    if (area)    grid->addWidget(area,   1,0);
    setLayout(grid);
}

void TGUI_canvas::resizeEvent(QResizeEvent *event)
{
    DBG2("enter: empty func",0);
}

extern Tegs_widget  gui_canvas_buildTop  (Twid_canvas* canvas, Tegs_widget parent)
{
    TGUI_canvas* self = new TGUI_canvas(canvas, 0); //(QWidget*)parent);
    return self;
}

extern void gui_canvas_finishTop  (Twid_canvas* canvas)
{
    TGUI_canvas* self = (TGUI_canvas*)canvas->vgcm_widEGS;
    self->creation_finish();
}

/*======================================================================*/
/*= the rulers                                                         =*/

TGUI_ruler::TGUI_ruler(Twid_canvas*c, Twidcvs_ruler* r)
: QFrame(), canvas(c), ruler(r)
{
    //qw->setLineWidth(5);
    //qw->setFrameStyle(QFrame::Box|QFrame::Plain);
    setContentsMargins(5,5,5,5);
    if ( isHruler() ) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

void TGUI_ruler::enterEvent(QEvent *event)
{ setFocus(); }
void TGUI_ruler::leaveEvent(QEvent *event)
{ clearFocus(); }

void TGUI_ruler::mousePressEvent(QMouseEvent *event)
{
    int btn,kmod,cmd;
    if ( (btn=conv_BtnNum_Qt2Vgui(event))==0 ) return;
    kmod = conv_KeyMod_Qt2Vgui(event);
    cmd = isHruler() ? WIDCVS_HR_BtnDown : WIDCVS_VR_BtnDown;
    wid_canvas_action(canvas,cmd,event->x(),event->y(),btn,kmod,0);
}

void TGUI_ruler::wheelEvent(QWheelEvent *event)
{
    int btn,times,kmod,cmd;
    btn  = conv_wheel_Qt2Vgui(event,&times);
    kmod = conv_KeyMod_Qt2Vgui(event);
    cmd  = isHruler() ? WIDCVS_HR_BtnDown : WIDCVS_VR_BtnDown;
    wid_canvas_action(canvas,cmd,event->x(),event->y(),btn,kmod,times);
}

// return 1 if the key is recognized otherwise it returns 0
static int gui_canvas_key(Twid_canvas*self,QKeyEvent *event,Tegs_widget qw)
{
    int w,kmod;
    kmod = conv_KeyMod_Qt2Vgui(event);
    w = qw==self->cvs_hruler.widEGS ? WIDCVS_WH_HR 
      : qw==self->cvs_vruler.widEGS ? WIDCVS_WH_VR
      : WIDCVS_WH_DA;
    switch ( event->key() ) {
        case Qt::Key_Down:     wid_canvas_action(self,WIDCVS_KeyDown   ,w,kmod,0,0,0); return 1;
        case Qt::Key_Up:       wid_canvas_action(self,WIDCVS_KeyUp     ,w,kmod,0,0,0); return 1;
        case Qt::Key_Left:     wid_canvas_action(self,WIDCVS_KeyLeft   ,w,kmod,0,0,0); return 1;
        case Qt::Key_Right:    wid_canvas_action(self,WIDCVS_KeyRight  ,w,kmod,0,0,0); return 1;
        case Qt::Key_PageDown: wid_canvas_action(self,WIDCVS_KeyPgDown ,w,kmod,0,0,0); return 1;
        case Qt::Key_PageUp:   wid_canvas_action(self,WIDCVS_KeyPgUp   ,w,kmod,0,0,0); return 1;
        case Qt::Key_ZoomIn:   wid_canvas_action(self,WIDCVS_KeyZoomP  ,w,kmod,0,0,0); return 1;
        case Qt::Key_ZoomOut:  wid_canvas_action(self,WIDCVS_KeyZoomM  ,w,kmod,0,0,0); return 1;
    }
    return 0;
}

void TGUI_ruler::keyPressEvent(QKeyEvent *event)
{ gui_canvas_key(canvas,event,this); }

extern Tegs_widget  gui_canvas_buildScale(Twid_canvas_scale* scale, int horizontal)
{
    TGUI_ruler* ruler = new TGUI_ruler(scale->canvas,scale);
    return ruler;
}

/*======================================================================*/
/*= the drawing area                                                   =*/

TGUI_DrawingArea::TGUI_DrawingArea(Twid_canvas* c)
: QFrame(), canvas(c), drawnPicture(0),
  screenPicture(0), painter(new QPainter)
{
    int borderWidth=0, minWidth,minHeight;
    if ( canvas->vgcm_mode==0 ) {
        minWidth  = 150; // FIXME: very bad patch: 150 was got assuming min dx of YFIG main window is 550
        minHeight = 300; // FIXME: very bad patch: 150 was got assuming min dy of YFIG main window is 500
        setMinimumSize(minWidth,minHeight);
        setMouseTracking(1);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        borderWidth = canvas->vgcm_daBW;
#ifdef DEBUG_BORDER
        QPalette pal = palette();
        pal.setColor(QPalette::Window,     Qt::magenta);
        pal.setColor(QPalette::WindowText, Qt::blue);
        setAutoFillBackground(true);
        setPalette(pal);
        borderWidth = 1;
#endif
        if ( borderWidth>0 ) {
            setLineWidth(borderWidth);
            setFrameStyle(QFrame::Box|QFrame::Plain);
        } else 
            setFrameStyle(QFrame::NoFrame);
        minWidth  = canvas->vgcm_daWidth;
        minHeight = canvas->vgcm_daHeight;
        setMinimumSize(minWidth,minHeight);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    DBG2("mode=%d minSize=%d %d", canvas->vgcm_mode,minWidth,minHeight);
}

TGUI_DrawingArea::~TGUI_DrawingArea()
{
    freePictures(screenPicture,drawnPicture);
    delete painter;
}

void TGUI_DrawingArea::paintEvent(QPaintEvent *event)
{
    DBG2("Expose event.",0);
    painter->begin(this);
    painter->drawPixmap(0,0,*screenPicture);
    painter->end();
    QFrame::paintEvent(event);
}

void TGUI_DrawingArea::resizeEvent(QResizeEvent *event) {
    DBG2("Resize event.",0);
    Tvgui* gui = canvas->vgcm_gui;
    int dx = event->size().width();
    int dy = event->size().height();
    drawnPicture= new QPixmap(dx,dy);
    drawnPicture->fill( COLOR(gui->vg_daBackground) );
    //QPainter painter(pixmap);
    //painter.drawPixmap(0,0,*pixmap,0,0,dx,dy);
    //painter.end();
    Box_x (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_y (canvas->vgcm_drawingArea.outdev->vv_boxPIX) = 0;
    Box_dx(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = dx;
    Box_dy(canvas->vgcm_drawingArea.outdev->vv_boxPIX) = dy;
    wid_canvas_boxPixUpdated(canvas);
    delete screenPicture; 
    screenPicture = drawnPicture;
}

void TGUI_DrawingArea::enterEvent(QEvent *event)
{
    if ( canvas->vgcm_mode==0 ) {
        setFocus();
        wid_canvas_action(canvas,WIDCVS_DA_Enter,0,0,0,0,0);
    }
}

void TGUI_DrawingArea::leaveEvent(QEvent *event)
{
    if ( canvas->vgcm_mode==0 ) {
        clearFocus();
        wid_canvas_action(canvas,WIDCVS_DA_Leave,0,0,0,0,0);
    }
}

void TGUI_DrawingArea::mousePressEvent(QMouseEvent *event) 
{
    int btn;
    if ( (btn=conv_BtnNum_Qt2Vgui(event))==0 )
        return;
    TaccelDef kmod = conv_KeyMod_Qt2Vgui(event);

    wid_canvas_action(canvas,WIDCVS_DA_BtnDown,event->x(),event->y(),btn,kmod,1);
}

void TGUI_DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    DBG2("Button Release event: but=%d : x=%03d y=%03d",
            event->button()==Qt::LeftButton   ? 1
          : event->button()==Qt::MiddleButton ? 2
          : event->button()==Qt::RightButton  ? 3 : 0,
          event->x(), event->y());
}

void TGUI_DrawingArea::wheelEvent(QWheelEvent *event)
{
    int kmod,btn,times;
    int x = event->x(),y=event->y();
    btn  = conv_wheel_Qt2Vgui(event,&times);
    kmod = conv_KeyMod_Qt2Vgui(event);
    wid_canvas_action(canvas,WIDCVS_DA_BtnDown,x,y,btn,kmod,times);
}

void TGUI_DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    wid_canvas_action(canvas,WIDCVS_DA_Motion,x,y,0,0,0);
}

void TGUI_DrawingArea::keyPressEvent(QKeyEvent *event)
{ gui_canvas_key(canvas,event,this); }

void TGUI_DrawingArea::clear()
{
    Tvgui* gui = canvas->vgcm_gui;
    drawnPicture->fill( COLOR(gui->vg_daBackground) );
    if ( screenPicture && screenPicture!=drawnPicture )
      screenPicture->fill( COLOR(gui->vg_daDrawColor) );
    update();
}

extern Tegs_widget  gui_canvas_buildArea (Twid_canvas_area* area)
{
    Twid_canvas* canvas = area->canvas;
    TGUI_DrawingArea* self = new TGUI_DrawingArea(canvas);
    return self;
}

extern void         gui_canvas_clear        (Twid_canvas_area*  area)
{ 
    TGUI_DrawingArea* self = (TGUI_DrawingArea*)area->widEGS;
    self->clear();
}

/*======================================================================*/
