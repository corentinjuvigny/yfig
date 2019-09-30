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
 *      $Id: widqt-buttonPanel.h 75 2017-05-15 16:47:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/widqt-buttonPanel.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Lines, rows and matrix of buttons (EGS driver).                    =*/
/*=                                                                    =*/
/*= Definition of C++ classes making the link between the VGUI button  =*/
/*= panels and the EGS button panels.                                  =*/
/*=                                                                    =*/
/*= See the vgui/wid-buttonPanel.h header for the type and function    =*/
/*= documentation.                                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_BUTTONPANEL_H
#define FILE_GUIQT_BUTTONPANEL_H
/*======================================================================*/

#include "widqt-button.h"

#include "vgui/wid-buttonPanel.h"

#ifdef QT_VERS_5
//#include <QToolButton>
#include <QScrollArea>
#endif

/*======================================================================*/
/*= The panels                                                         =*/

//class TGUI_ButtonPanel : public QFrame {
class TGUI_ButtonPanel : public QScrollArea {
public:
    TGUI_ButtonPanel(Twid_buttonPanel* p);
    virtual ~TGUI_ButtonPanel();
    void enableScrollBar();
    QWidget* container;
    virtual void finish() = 0;
    virtual void update() ;
public:
    Twid_buttonPanel*panel;
    QLabel*          padding;
};

class TGUI_ButtonPanelRaCa : public TGUI_ButtonPanel {
public:
    TGUI_ButtonPanelRaCa(Twid_buttonPanel* p);
    virtual void finish() ;
    virtual void update() ;
public: // local function
    void finishAlc();
    void doMappingAlc(int colNb);
    int  maxColAlc()       { return 7; }
    void finishFlc();
    void resizeEvent(QResizeEvent * event);
public:
    QGridLayout* grid;
    int rowNb,colNb;
};

class TGUI_ButtonPanelR1Cn : public TGUI_ButtonPanel {
public:
    TGUI_ButtonPanelR1Cn(Twid_buttonPanel* p);
    void finish();
public:
    QHBoxLayout* grid;
    virtual void showEvent(QShowEvent *event); // FIXME
};

class TGUI_ButtonPanelRnC1 : public TGUI_ButtonPanel {
public:
    TGUI_ButtonPanelRnC1(Twid_buttonPanel* p);
    void finish();
public:
    QVBoxLayout* grid;
};

/*======================================================================*/
#endif // FILE_GUIQT_BUTTONPANEL_H
/*======================================================================*/
