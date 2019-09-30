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
 *      $Id: widqt-menu.h 222 2018-06-15 14:18:01Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-menu.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets (EGS driver).                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-menu.h header for documentation.                  =*/
/*======================================================================*/

#include "guiqt.h"
#include "vgui/wid-menu.h"

/*======================================================================*/

// for key accelerators
class TGUI_menuAction : public QAction {
    Q_OBJECT
public:
    TGUI_menuAction(const char *text, QObject* parent, Cwidm_entry* me);
    Cwidm_entry* data;
    //void triggered(bool checked = false) {  fprintf(stderr,"%p:%s\n",this,__func__); }
public slots:
    void selected();
};

// for DDL widget
class TGUI_menuDD : public QComboBox {
    Q_OBJECT
public:
     TGUI_menuDD( Twidm_ddl*m, QWidget*parent);
public slots:
    void selected(int);
public:
    Twidm_ddl* menu;
};

/*======================================================================*/
