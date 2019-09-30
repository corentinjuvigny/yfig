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
# $Software: YFIG-GTK (Interface to GTK)
#       $Id: Makefile.am 148 2018-06-04 06:59:33Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

SrcDir     = .
IncDir     = ../include/guigtk
EXTRA_DIST = 

noinst_LIBRARIES = libguigtk.a

libguigtk_a_CPPFLAGS  = -I ../include/guigtk -I ../include/
libguigtk_a_CPPFLAGS += -I $(YFIG_TOP_SRCDIR)/include -I $(top_builddir)/include
libguigtk_a_CPPFLAGS += @GTK3_CFLAGS@
libguigtk_a_SOURCES   = 

EXTRA_DIST         += guigtk.h
EXTRA_DIST         += $(IncDir)/guigtk-config.h

EXTRA_DIST         += $(IncDir)/guigtk-top.h
libguigtk_a_SOURCES += guigtk-init.c
libguigtk_a_SOURCES += widgtk-egs.c
libguigtk_a_SOURCES += guigtk-resources.c
libguigtk_a_SOURCES += guigtk-color.c
libguigtk_a_SOURCES += guigtk-dw.c
libguigtk_a_SOURCES += guigtk-dw-layout.c
EXTRA_DIST         += $(IncDir)/guigtk-util.h
libguigtk_a_SOURCES += guigtk-util.c
EXTRA_DIST         +=  guigtk-outdev-pixmap.c
EXTRA_DIST         +=  guigtk-outdev-vt-ldaf-gc.c
EXTRA_DIST         +=  guigtk-outdev-vt-ldaf.c
libguigtk_a_SOURCES += guigtk-outdev-vt.c
EXTRA_DIST          += widgtk-canvas.h
libguigtk_a_SOURCES += widgtk-canvas.c
libguigtk_a_SOURCES += widgtk-menu.c
libguigtk_a_SOURCES += widgtk-button.c
libguigtk_a_SOURCES += widgtk-buttonPanel.c
libguigtk_a_SOURCES += widgtk-simple.c
libguigtk_a_SOURCES += widgtk-simple-spinner.c
libguigtk_a_SOURCES += widgtk-filebrowser.c

#=========================================================================
