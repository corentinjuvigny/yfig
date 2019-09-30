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
# $Software: YFIG (YFIG driver to Qt)
#       $Id: yfigqt.pro.in 148 2018-06-04 06:59:33Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/qt/yfigqt.pro.in $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================


TEMPLATE = app
TARGET   = yfig-qt
DESTDIR  = /home/kodra/Documents/SVN/yfig/yfig/yfig/qt

LIBS += ../common/libyfig.a
LIBS += ../../vgui/libvgui.a
LIBS += ../../guiqt/libguiqt.a
LIBS += /home/kodra/Documents/SVN/yfig/yfig/yfig/qt/libvgui2.a
LIBS += ../../fig/libfig.a 
LIBS += ../../outdev/libodev.a
LIBS += ../../tools/libtools.a
LIBS += -ljpeg  -lpng16 -lz 
LIBS += -lm
QT += widgets

CONFIG  += debug
CONFIG  += warn_off

#DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../include
INCLUDEPATH += ../../include
INCLUDEPATH += ../../include/guiqt
INCLUDEPATH += /home/kodra/Documents/SVN/yfig/yfig/yfig/include 
INCLUDEPATH += /home/kodra/Documents/SVN/yfig/yfig/include 

# Input
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/yfig/qt/yfigqt-main.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/yfig/qt/yfigqt-mw.cc
SOURCES += /home/kodra/Documents/SVN/yfig/yfig/yfig/qt/tbqt-file-OMSF.cc
