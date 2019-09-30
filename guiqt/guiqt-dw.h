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
 *      $Id: guiqt-dw.h 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-dw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_DW_H
#define FILE_GUIQT_DW_H
/*======================================================================*/

#include "guiqt.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"


#ifdef QT_VERS_5
#include <QDialog>
#endif

/*======================================================================*/

class TGUI_dialog : public QDialog
{
    //Q_OBJECT
public:
    TGUI_dialog(Tvgui_dw*_d, int bd, int dx, int dy, int resizable);
    // virtual ~DialogFile();
public:
   virtual bool close();
   //virtual void done(int r);
   virtual void accept();
   virtual void reject();
//public slots :
   void activateWindow();
   void doBreakloop(Twid_dialStatus s) { done(s); }
public:
    Tvgui_dw*    d;
    QGridLayout* grid;
};

/*======================================================================*/

class TGUI_subdialog : public QWidget
{
    //Q_OBJECT
public:
    TGUI_subdialog(Tvgui_dw*_d, int bd, int dx, int dy, int resizable);
    // virtual ~DialogFile();
public:
    Tvgui_dw*    d;
    QGridLayout* grid;
};

/*======================================================================*/
#endif //FILE_GUIQT_DW_H
/*======================================================================*/
