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
 *      $Id: widqt-simple-slin.h 74 2017-05-11 19:17:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple-slin.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*= Implementation of single line input widget (SLIN).                 =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDQT_SIMPLE_SLIN_H
#define FILE_WIDQT_SIMPLE_SLIN_H
/*======================================================================*/

#include "guiqt.h"
#include "vgui/wid-simple.h"

#ifdef QT_VERS_5
#include <QLineEdit>
#endif

/*======================================================================*/

class TGUI_LineEdit : public QLineEdit {
    Q_OBJECT
public:
    TGUI_LineEdit(Twid_slin* self, CstrBrief text);
public slots:
    void textChanged_cb(const QString &text);
    void editingFinished_cb();
    void returnPressed_cb();
public:
    Twid_slin* data;
};

/*======================================================================*/
#endif // FILE_WIDQT_SIMPLE_SLIN_H
/*======================================================================*/
