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
 *      $Id: widqt-simple-spinner.h 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple-spinner.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Spinner widgets (EGS driver).                                      =*/
/*=                                                                    =*/
/*= Class implementing the spinner VGUI widgets                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDQT_SIMPLE_SPINNER_H
#define FILE_WIDQT_SIMPLE_SPINNER_H
/*======================================================================*/

#include "guiqt.h"

#include "vgui/wid-simple.h"

#ifdef QT_VERS_5
#include <QSpinBox>
#include <QDoubleSpinBox>
#endif

/*======================================================================*/

class TGUI_spinBoxDle : public QDoubleSpinBox {
    Q_OBJECT
public:
  TGUI_spinBoxDle(Twid_spinner*s, Tvgui* gui, QWidget* p=0);
  void stepBy(int steps);
public slots:
  void 	valueValidated();
  void 	valueChanged(double);
public:
  Twid_spinner* spin;
};

class TGUI_spinBoxInt : public QSpinBox {
    Q_OBJECT
public:
  TGUI_spinBoxInt(Twid_spinner*s, Tvgui* gui, QWidget* p=0);
  void stepBy(int steps);
public slots:
  void 	valueValidated();
  void 	valueChanged(int);
public:
  Twid_spinner* spin;
};

/*======================================================================*/
#endif // FILE_WIDQT_SIMPLE_SPINNER_H
/*======================================================================*/
