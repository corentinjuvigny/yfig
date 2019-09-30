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
 *      $Id: widqt-button.h 93 2017-06-01 06:51:03Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/widqt-button.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons (EGS driver).                                =*/
/*=                                                                    =*/
/*= Definition of C++ classes making the link between the VGUI buttons =*/
/*= and the EGS buttons.                                               =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for the type and function         =*/
/*= documentation.                                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_BUTTON_H
#define FILE_GUIQT_BUTTON_H
/*======================================================================*/

#include "guiqt.h"
#include "vgui/wid-button.h"

#ifdef QT_VERS_5
#include <QToolButton>
#include <QCheckBox>
#endif

/*======================================================================*/
/*= The panel buttons                                                  =*/

class TGUI_Button { //: public QToolButton {
    // Q_OBJECT
public:
    TGUI_Button(Twid_button* b, Tegs_widget pw);
//public slots:
public:
    virtual void selectedByShortcut();
public:
    void init(QAbstractButton* qbtn);
    Twid_button*      button;
public:
    virtual void redraw();  // default print an error message
    virtual void setOn();   // default print an error message
    virtual void setOff();  // default print an error message
    virtual void showEvent(QShowEvent *event);
};

class TGUI_ButtonStd : public QToolButton, public TGUI_Button {
    Q_OBJECT
public:
    TGUI_ButtonStd(Twid_button* b, Tegs_widget pw);
public:
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
public:
    virtual void redraw();
};

class TGUI_ButtonOnOff : public QToolButton, public TGUI_Button {
    Q_OBJECT
public:
    TGUI_ButtonOnOff(Twid_button* b, Tegs_widget pw);
public slots:
    void selected(bool checked);
    void selectedByShortcut();
public:
    virtual void setOn();
    virtual void setOff();
    virtual void showEvent(QShowEvent* event);
};

class TGUI_ButtonCB : public QCheckBox, public TGUI_Button {
    Q_OBJECT
public:
    TGUI_ButtonCB(Twid_button* b, Tegs_widget pw);
public slots:
    void stateChanged(int state);
public:
    virtual void redraw();
    virtual void setOn();
    virtual void setOff();
public:
    int lock;
};

/*======================================================================*/
#endif // FILE_GUIQT_BUTTON_H
/*======================================================================*/
