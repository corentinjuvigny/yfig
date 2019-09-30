#=========================================================================
# Copyright 2017 Ivan Augé
#
# This file is part of the YFIG software.
# 
# YFIG is free  software;  you  can redistribute it and/or modify it
# under the terms of the GNU  General Public License as published by
# the  Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# YFIG software is distributed  in the  hope that it will be useful,
# but WITHOUT ANY WARRANTY;  without even the  implied  warranty  of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  the GNU
# General Public License for more details.
# 
# You should have received  a copy of the GNU General Public License
# along with YFIG. If not, see <http://www.gnu.org/licenses/>.
#=========================================================================
# $Software: YFIG-QT (Interface to Qt)
#       $Id: guiqt.pro.in 148 2018-06-04 06:59:33Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt.pro.in $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

TEMPLATE = lib
TARGET   = guiqt
DESTDIR  = /home/kodra/Documents/SVN/yfig/yfig/guiqt


QT += widgets

CONFIG  += staticlib
CONFIG  += debug
CONFIG  += warn_off

#DEPENDPATH += .
INCLUDEPATH += ../include/guiqt /home/kodra/Documents/SVN/yfig/yfig/guiqt /home/kodra/Documents/SVN/yfig/yfig/include 

# Input
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-topform.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-top.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-init.cc 
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-color.cc 
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-resources.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-util.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-create.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-dw.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-dw-layout.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/guiqt-outdev-vt.cc

SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-egs.cc
HEADERS += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple.cc
HEADERS += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple-spinner.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple-spinner.cc
HEADERS += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple-slin.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-simple-slin.cc

HEADERS += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-menu.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-menu.cc

HEADERS += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-filebrowser.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-filebrowser.cc

HEADERS += /home/kodra/Documents/SVN/yfig/yfig/include/guiqt/widqt-button.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-button.cc
HEADERS += /home/kodra/Documents/SVN/yfig/yfig/include/guiqt/widqt-buttonPanel.h
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-buttonPanel.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/guiqt/widqt-canvas.cc

