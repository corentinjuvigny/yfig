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
 *      $Id: widqt-simple.h 74 2017-05-11 19:17:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDQT_SIMPLE_H
#define FILE_WIDQT_SIMPLE_H
/*======================================================================*/

#include "guiqt.h"
#include "vgui/wid-simple.h"

/*======================================================================*/

class TGUI_slider : public QSlider {
    Q_OBJECT
public:
    TGUI_slider(Twid_slider* dt,QWidget*pw);
public slots:
    void valueChanged_cb(int nv);
public:
    //       L or bot ------------> R or top
    // vgui:   0      ------------>  1
    // egs:    N      ------------>  0
    int    convVgui2Egs(double vgui) {
        double res = 1./data->inc - (vgui/data->inc);
        return round(res); }
    double convEgs2Vgui(int egs) {
        double res = egs;
        res = 1. - res*data->inc;
        return res; }
public:
    Twid_slider* data;
    int          lock;  // avoid recursion in gui_slider_set and
                        // valueChanged_cb
};

/*======================================================================*/
#endif // FILE_WIDQT_SIMPLE_H
/*======================================================================*/
