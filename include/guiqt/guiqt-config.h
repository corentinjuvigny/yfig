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
 *      $Id: guiqt-config.h.in 142 2018-05-28 16:35:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-config.h.in $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= QT configuration.                                                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_CONFIG_H
#define FILE_GUIQT_CONFIG_H
/*======================================================================*/

#define QT_VERS_5

#if !defined(QT_VERS_4) && !(defined QT_VERS_5)
#error  neither QT_VERS_4 nor QT_VERS_5 is defined
#error  either QT_VERS_4 or QT_VERS_5 must be defined
#endif

#if defined(QT_VERS_4) && (defined QT_VERS_5)
#error  both QT_VERS_4 and QT_VERS_5 are defined
#error  either QT_VERS_4 or QT_VERS_5 must be defined
#endif

#ifdef QT_VERS_4
#include <QtGui>
#endif

#ifdef QT_VERS_5
#include <QApplication>
#include <QTimer>
#include <QScreen>
#include <QString>
#include <QWidget>
#include <QAction>
#include <QMenuBar>
#include <QBitmap>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPicture>
#include <QResizeEvent>
#include <QComboBox>
#include <QShortcut>
#include <QTextEdit>
#include <QDialog>
#include <QScrollBar>
#endif

/*======================================================================*/
#endif // FILE_GUIQT_CONFIG_H
/*======================================================================*/
