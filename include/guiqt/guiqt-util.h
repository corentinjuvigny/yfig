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
 *      $Id: guiqt-util.h 148 2018-06-04 06:59:33Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-util.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_UTIL_H
#define FILE_GUIQT_UTIL_H
/*======================================================================*/

/*======================================================================*/
/*= miscellaneous converters                                           =*/

// return 0:unknown button; 1/2/3:left/middle/right button
extern int       conv_BtnNum_Qt2Vgui(QMouseEvent *event);
extern TaccelDef conv_KeyMod_Qt2Vgui(QInputEvent *event);

// return the vgui button (4 or 5)
// *times contains the number of times the corresponding action must
// be do (proportional to wheel rotation speed).
// *times is always >= 1.
// times can be the NULL pointer
extern int       conv_wheel_Qt2Vgui(QWheelEvent*event, int*times);

// return a default value when ls, js or cs are bad or unsuported.
extern Qt::PenStyle     conv_lineStyle_od2qt(TlineStyle ls);
extern Qt::PenJoinStyle conv_joinStyle_od2qt(TjoinStyle fig);
extern Qt::PenCapStyle  conv_capStyle_od2qt (TcapStyle fig);


/*======================================================================*/
/*= miscellaneous utilities                                            =*/

void widget_getColors(Tvgui*gui, CstrBrief wn,
                      TcolorEGS*bg, TcolorEGS*fg);
void widget_setColor (Tvgui* gui, QWidget* self, CstrBrief wn,
                      TcolorEGS bg=EGSCOLOR_default,
                      TcolorEGS fg=EGSCOLOR_default);

//extern CstrBrief widget_getLabel (Tegs_widget self);
//extern void      widget_setLabel (Tegs_widget self, CstrBrief text);
//extern CstrBrief widget_getString(Tegs_widget self);
//extern void      widget_setString(Tegs_widget self, CstrBrief text);

/*======================================================================*/
/*= widget geometry                                                    =*/

/*======================================================================*/
/*= place widget in container                                          =*/

extern void widget_place(QGridLayout*g, void* ow,
                     int li, int ci, int ls=1, int cs=1,
                     QSizePolicy::Policy horizontal=QSizePolicy::Fixed,
                     QSizePolicy::Policy vertical  =QSizePolicy::Fixed);
extern void widget_place_FF(QGridLayout*g, QWidget*w,
                            int li, int ci, int ls=1, int cs=1);
extern void widget_place_EE(QGridLayout*g, QWidget*w,
                            int li, int ci, int ls=1, int cs=1);
extern void widget_place_EF(QGridLayout*g, QWidget*w,
                            int li, int ci, int ls=1, int cs=1);
extern void widget_place_FE(QGridLayout*g, QWidget*w,
                            int li, int ci, int ls=1, int cs=1);

/*======================================================================*/
/*= here some shared functions avoid very small headers.               =*/

// extern void widget_dialog_breakLoop(Twid_dialog* d, Twid_dialStatus b);

/*======================================================================*/
#endif // FILE_GUIQT_UTIL_H
/*======================================================================*/
