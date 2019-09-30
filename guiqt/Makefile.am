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
#       $Id: Makefile.am 113 2018-01-09 18:35:22Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

InclDir    = ../include/guiqt
EXTRA_DIST = 

#noinst_LIBRARIES = libguiqt.a

#libguixt_a_CPPFLAGS  = -I ../include
#libguixt_a_CPPFLAGS += @YFIG_xt_cppflags@ @pangoxft_cflags@
#libguixt_a_SOURCES   = 

EXTRA_DIST += guiqt.pro
EXTRA_DIST += guiqt.h
EXTRA_DIST += $(InclDir)/guiqt-config.h
EXTRA_DIST += $(InclDir)/guiqt-topform.h
EXTRA_DIST += guiqt-topform.cc
EXTRA_DIST += $(InclDir)/guiqt-gc.h
EXTRA_DIST += $(InclDir)/guiqt-top.h
EXTRA_DIST += guiqt-top.cc
EXTRA_DIST += guiqt-init.cc
EXTRA_DIST += guiqt-color.cc
EXTRA_DIST += guiqt-resources.cc
EXTRA_DIST += guiqt-create.cc $(InclDir)/guiqt-create.h
EXTRA_DIST += guiqt-outdev-pixmap.cc
EXTRA_DIST += guiqt-outdev-ldaf-gc.cc
EXTRA_DIST += guiqt-outdev-ldaf.cc
EXTRA_DIST += guiqt-outdev-font.cc
EXTRA_DIST += guiqt-outdev-vt.cc
EXTRA_DIST += guiqt-util.cc $(InclDir)/guiqt-util.h
EXTRA_DIST += guiqt-dw.cc              guiqt-dw.h
EXTRA_DIST += guiqt-dw-layout.cc
EXTRA_DIST += guiqt-outdev-vt.cc

EXTRA_DIST += widqt-egs.cc
EXTRA_DIST += widqt-simple.h
EXTRA_DIST += widqt-simple.cc
EXTRA_DIST += widqt-simple-spinner.cc widqt-simple-spinner.h
EXTRA_DIST += widqt-simple-slin.cc    widqt-simple-slin.h

EXTRA_DIST += widqt-button.cc      $(InclDir)/widqt-button.h 
EXTRA_DIST += widqt-buttonPanel.cc $(InclDir)/widqt-buttonPanel.h 
EXTRA_DIST += widqt-canvas.cc      $(InclDir)/widqt-canvas.h
EXTRA_DIST += widqt-menu.cc        widqt-menu.h
EXTRA_DIST += widqt-filebrowser.cc widqt-filebrowser.h

all: Makefile.qt always
	make -f Makefile.qt

Makefile.qt: guiqt.pro
	@YFIG_qt_qmake@ -o Makefile.qt guiqt.pro

always: 
