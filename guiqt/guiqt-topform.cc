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
 *      $Id: guiqt-topform.cc 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-topform.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The WtopForm implement a Tvgui_mw (a main widos).                  =*/
/*======================================================================*/

#include "guiqt.h"

WtopForm::WtopForm(int x, int y, int dx, int dy)
  : preferedWidth(dx),preferedHeight(dy) {
    setSizeIncrement(10,10);
    setMinimumSize(550,500);
    resize(dx,dy);
    move(x,y);
}
WtopForm::WtopForm(int dx, int dy)
  : preferedWidth(dx),preferedHeight(dy) {
    //setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setSizeIncrement(10,10);
    //setBaseSize(dx,dy);
    setMinimumSize(550,500);
    //setFixedSize  (dx,dy);
    resize(dx,dy);
}
QSize 	WtopForm::sizeHint() const
{
    return QSize(preferedWidth,preferedHeight);
}
void WtopForm::closeEvent (QCloseEvent *event)
{
    printf("topForm::closeEvent\n");
    QApplication::exit(0);
}

/*======================================================================*/
