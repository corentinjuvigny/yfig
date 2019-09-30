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
 *      $Id: guiqt-topform.h 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-topform.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The WtopForm implement a Tvgui_mw (a main widos).                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_TOPFORM_H
#define FILE_GUIQT_TOPFORM_H
/*======================================================================*/

class WtopForm : public QWidget {
public:
    WtopForm(int x, int y,int dx, int dy);
    WtopForm(int dx, int dy);
    virtual QSize 	sizeHint() const;
    int preferedWidth,preferedHeight;
    virtual void closeEvent (QCloseEvent *event);
};

/*======================================================================*/
#endif // FILE_GUIQT_TOPFORM_H
/*======================================================================*/
