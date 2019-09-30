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
#       $Id: Makefile.am 148 2018-06-04 06:59:33Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/qt/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

EXTRA_DIST  = yfigqt.pro.in
EXTRA_DIST += yfigqt.h
EXTRA_DIST += yfigqt-main.cc
EXTRA_DIST += yfigqt-mw.cc
EXTRA_DIST += tbqt-file-OMSF.cc

bin_PROGRAMS    = yfig-qt 

yfig_qt_SOURCES = 

yfig-qt$(EXEEXT): Makefile.qt .libs_changed always
	make -f Makefile.qt

Makefile.qt: yfigqt.pro
	@YFIG_qt_qmake@ -o Makefile.qt yfigqt.pro

.libs_changed: ../common/libyfig.a .\
               ../../vgui/libvgui.a ../../guiqt/libguiqt.a \
               ../../fig/libfig.a ../../outdev/libodev.a \
               ../../tools/libtools.a
	test -e libvgui2.a || ln -s ../../vgui/libvgui.a libvgui2.a
	rm -f yfig-qt
	touch .libs_changed

always: 

#=========================================================================
