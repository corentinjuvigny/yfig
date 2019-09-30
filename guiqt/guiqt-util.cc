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
 *$Software: YFIG-QT (Interface to Qt)
 *      $Id: guiqt-util.cc 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-util.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

#include "guiqt.h"
#include "vgui/vgui-accelkey.h"

/*======================================================================*/
/*= miscellaneous converters                                           =*/

extern int       conv_BtnNum_Qt2Vgui(QMouseEvent *event)
{
    switch ( event->button() ) {
        case Qt::LeftButton:   return 1;
        case Qt::MiddleButton: return 2;
        case Qt::RightButton:  return 3;
        default:
            TEMP_spw_printf("unexpected QT button identifier %04x",
                event->button());
            return 0;
    }
}

extern TaccelDef conv_KeyMod_Qt2Vgui(QInputEvent *event)
{
    TaccelDef kmod=0;
    if ( event->modifiers()&Qt::ShiftModifier )   kmod |= GL_ACCEL_Shift;
    if ( event->modifiers()&Qt::ControlModifier ) kmod |= GL_ACCEL_Control;
    if ( event->modifiers()&Qt::MetaModifier )    kmod |= GL_ACCEL_Meta;
    if ( event->modifiers()&Qt::AltModifier )     kmod |= GL_ACCEL_Meta;
    return kmod;
}

extern int       conv_wheel_Qt2Vgui(QWheelEvent*event, int*times)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    if ( numSteps>0 ) {
        if (times)
            *times = numSteps<1 ? 1 : numSteps;
        return 4;
    } else {
        if (times)
            *times = numSteps>-1 ? 1 : -numSteps;
        return 5;
    }
}

extern Qt::PenStyle     conv_lineStyle_od2qt(TlineStyle ls)
{
    switch (ls) {
      default: 
        PERM_spe_printf("%d is an invalid line style (set to line-solid).",ls);
      case DA_LS_Solid    : return Qt::SolidLine;
      case DA_LS_Dash     : return Qt::DashLine;
      case DA_LS_Dot      : return Qt::DotLine;
      case DA_LS_Dash1Dot : return Qt::DashDotLine;
      case DA_LS_Dash2Dot : return Qt::DashDotDotLine;
      case DA_LS_Dash3Dot : return Qt::DashDotDotLine;
      case DA_LS_None:  ;
        PERM_spe_printf("None (%d) line style is unexpected  (set to line-solid).",ls);
        return Qt::SolidLine;
    }
}

extern Qt::PenJoinStyle conv_joinStyle_od2qt(TjoinStyle js)
{
    switch (js) {
      default:
        PERM_spe_printf("%d is an invalid join style (set to join-miter).",js);
      case DA_JS_Miter: return Qt::MiterJoin;
      case DA_JS_Round: return Qt::RoundJoin;
      case DA_JS_Bevel: return Qt::BevelJoin;
      case DA_JS_None:
        PERM_spe_printf("None (%d) join style is unexpected  (set to join-miter).",js);
        return Qt::MiterJoin;
    }
}

extern Qt::PenCapStyle  conv_capStyle_od2qt (TcapStyle cs)
{
    switch (cs) {
      default:
        PERM_spe_printf("%d is an invalid cap style (set to cap-butt).",cs);
      case DA_CS_Butt:  return Qt::FlatCap;   // FIXME
      case DA_CS_Round: return Qt::RoundCap;
      case DA_CS_Proj:  return Qt::SquareCap;  // FIXME
      case DA_CS_None:  ;
        PERM_spe_printf("None (%d) cap style is unexpected  (set to cap-butt).",cs);
        return Qt::FlatCap;
    }
}

/*======================================================================*/
/*= miscellaneous utilities                                            =*/

void widget_getColors(Tvgui*gui, CstrBrief wn,
                      TcolorEGS*bg, TcolorEGS*fg)
{
    if ( fg->ulong==EGSCOLOR_DEFAULT || bg->ulong==EGSCOLOR_DEFAULT ) {
        Tegs_color fgd, bgd;
        vgui_wn_getColorEGS(gui,wn, &fgd, &bgd);
        if ( fg->ulong==EGSCOLOR_DEFAULT ) *fg=fgd;
        if ( bg->ulong==EGSCOLOR_DEFAULT ) *bg=bgd;
    }
}

void widget_setColor (Tvgui* gui, QWidget* self, CstrBrief wn,
                      TcolorEGS bg, TcolorEGS fg)
{
    widget_getColors(gui,wn,&bg,&fg);

    if (fg.ulong!=EGSCOLOR_NONE || bg.ulong!=EGSCOLOR_NONE) {
        QPalette pal = self->palette();
        if ( fg.ulong!=EGSCOLOR_NONE ) {
            pal.setColor(QPalette::WindowText, QtCOLOR(fg));
            pal.setColor(QPalette::Text,QtCOLOR(fg) );
            pal.setColor(QPalette::ButtonText,QtCOLOR(fg) );
        }
        if ( bg.ulong!=EGSCOLOR_NONE ) {
            pal.setColor(QPalette::Window, QtCOLOR(bg));
            pal.setColor(QPalette::Base,QtCOLOR(bg) );
            pal.setColor(QPalette::Button,QtCOLOR(bg) );
        }
        self->setPalette(pal);
        self->setAutoFillBackground(true);
    }
}

//extern CstrBrief widget_getLabel (Tegs_widget self);
//extern void      widget_setLabel (Tegs_widget self, CstrBrief text)
//{
//    ((QLabel*)self)->setText(text);
//}
//extern CstrBrief widget_getString(Tegs_widget self);
//extern void      widget_setString(Tegs_widget self, CstrBrief text)
//{ ((QLabel*)self)->setText(text); }

/*======================================================================*/
/*= place widget in container                                          =*/

void widget_place   (QGridLayout*g, void*ow,int li, int ci, int ls, int cs,
                     QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical)
{
    QObject* o=(QObject*)ow;
    if ( o->isWidgetType() ) {
        QWidget* wid = (QWidget*)ow;
        g->addWidget( wid, li,ci, ls,cs);
        wid->setSizePolicy(horizontal,vertical);
    } else {
        QLayout* lay = (QLayout*)ow;
        g->addLayout( lay, li,ci, ls,cs);
        //lay->setSizePolicy(horizontal,vertical);
    }
}

void widget_place_FF(QGridLayout*g, QWidget*w, int li, int ci, int ls, int cs)
{ widget_place(g,w,li,ci,ls,cs,QSizePolicy::Fixed,QSizePolicy::Fixed); }
void widget_place_FF(QGridLayout*g, void*  ow, int li, int ci, int ls, int cs)
{ widget_place(g,ow,li,ci,ls,cs,QSizePolicy::Fixed,QSizePolicy::Fixed); }


void widget_place_EE(QGridLayout*g, QWidget*w, int li, int ci, int ls, int cs)
{ widget_place(g,w,li,ci,ls,cs,QSizePolicy::Expanding,QSizePolicy::Expanding); }
void widget_place_EE(QGridLayout*g, void*  ow, int li, int ci, int ls, int cs)
{ widget_place(g,ow,li,ci,ls,cs,QSizePolicy::Expanding,QSizePolicy::Expanding); }

void widget_place_EF(QGridLayout*g, QWidget*w, int li, int ci, int ls, int cs)
{ widget_place(g,w,li,ci,ls,cs,QSizePolicy::Expanding,QSizePolicy::Fixed); }
void widget_place_EF(QGridLayout*g, void*  ow, int li, int ci, int ls, int cs)
{ widget_place(g,ow,li,ci,ls,cs,QSizePolicy::Expanding,QSizePolicy::Fixed); }

void widget_place_FE(QGridLayout*g, QWidget*w, int li, int ci, int ls, int cs)
{ widget_place(g,w,li,ci,ls,cs,QSizePolicy::Fixed,QSizePolicy::Expanding); }
void widget_place_FE(QGridLayout*g, void*  ow, int li, int ci, int ls, int cs)
{ widget_place(g,ow,li,ci,ls,cs,QSizePolicy::Fixed,QSizePolicy::Expanding); }

//void widget_place_GG(QGridLayout*g, QWidget*w, int li, int ci, int ls, int cs)
//{ widget_place(g,w,li,ci,ls,cs,QSizePolicy::Ignored,QSizePolicy::Ignored); }
//void widget_place_GG(QGridLayout*g, void*  ow, int li, int ci, int ls, int cs)
//{ widget_place(g,ow,li,ci,ls,cs,QSizePolicy::Ignored,QSizePolicy::Ignored); }

/*======================================================================*/
