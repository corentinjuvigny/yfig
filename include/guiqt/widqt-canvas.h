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
 *      $Id: widqt-canvas.h 112 2018-01-09 18:34:30Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/widqt-canvas.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales (EGS scale).            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for the type description,        =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDQT_CANVAS_H
#define FILE_WIDQT_CANVAS_H
/*======================================================================*/

#include "vgui/vgui-outdev.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/

class TGUI_DrawingArea : public QFrame {
//    Q_OBJECT
public:
    TGUI_DrawingArea(Twid_canvas*canvas);
    ~TGUI_DrawingArea();
    void freePictures(QPixmap*p1,QPixmap*p2) {
        if ( p1!=p2 ) {
            if (p1) delete p1;
            if (p2) delete p1;
        } else 
            if (p1) delete p1;
    }
    
public:
    void paintEvent(QPaintEvent *event) ;
    void resizeEvent(QResizeEvent *event) ;
    void enterEvent(QEvent *event) ;
    void leaveEvent(QEvent *event) ;
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *event) ;
    void wheelEvent(QWheelEvent *event) ;
    void mouseMoveEvent(QMouseEvent *event) ;
    void keyPressEvent(QKeyEvent*event);
public:
    void clear();
    void clearRegXY(int x_nw, int y_nw, int dx, int dy);
public:
    Twid_canvas *canvas;
    QPixmap  *drawnPicture;    // the picture were gui_xxx functions write
    QPixmap  *screenPicture;   // the picture that is drawn on screen
    QPainter *painter;         // used by paintEvent for printing screenPicture
};

/*======================================================================*/

class TGUI_ruler : public QFrame {
//    Q_OBJECT
public:
    TGUI_ruler(Twid_canvas*c, Twidcvs_ruler* r);
    //~TGUI_ruler();

public:
    //void resizeEvent(QResizeEvent *event) ;
    void enterEvent(QEvent *event) ;
    void leaveEvent(QEvent *event) ;
    void mousePressEvent(QMouseEvent *event) ;
    //void mouseReleaseEvent(QMouseEvent *event) ;
    void wheelEvent(QWheelEvent *event) ;
    //void mouseMoveEvent(QMouseEvent *event) ;
    void keyPressEvent(QKeyEvent*event);

public:
    Twid_canvas*   canvas;
    Twidcvs_ruler* ruler;
    int isHruler() { return ruler==&canvas->vgcm_horizontalScale; }
};

/*======================================================================*/

class TGUI_canvas : public QFrame {
//    Q_OBJECT
public:
    TGUI_canvas(Twid_canvas*g,QWidget*parent=0);
    ~TGUI_canvas();
    void creation_finish();
public:
    void resizeEvent(QResizeEvent *event) ;
public:
    Tvgui        *gui;
    Twid_canvas *canvas;
    QGridLayout  *grid;
};

/*======================================================================*/
#endif // FILE_WIDQT_CANVAS_H
/*======================================================================*/
