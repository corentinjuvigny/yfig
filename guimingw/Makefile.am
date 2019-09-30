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
# $Software: YFIG-WIN32 (Interface to WIN32 EGS under MinGw)
#       $Id: Makefile.am 333 2018-10-30 21:40:51Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guimingw/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

SrcDir     = .
IncDir     = ../include/guimingw
EXTRA_DIST = 

noinst_LIBRARIES = libguimingw.a

libguimingw_a_CPPFLAGS  = -I ../include/guimingw -I ../include/
libguimingw_a_CPPFLAGS += -I $(YFIG_TOP_SRCDIR)/include -I $(top_builddir)/include
libguimingw_a_CPPFLAGS += @WIN32_CFLAGS@
libguimingw_a_SOURCES   = 

EXTRA_DIST         += guimingw.h
EXTRA_DIST         += $(IncDir)/guimingw-config.h

EXTRA_DIST            += $(IncDir)/guimingw-top.h
libguimingw_a_SOURCES += guimingw-init.c
libguimingw_a_SOURCES += guimingw-color.c
# EXTRA_DIST            += $(IncDir)/guimingw-event.h
# libguimingw_a_SOURCES += guimingw-event.c
# EXTRA_DIST            += guimingw-outdev-vt-pxm.c
# EXTRA_DIST            += guimingw-outdev-vt-ldaf-gc.c
# EXTRA_DIST            += guimingw-outdev-vt-ldaf.c
# libguimingw_a_SOURCES += guimingw-outdev-vt.c
# libguimingw_a_SOURCES += guimingw-pixmap.c
# libguimingw_a_SOURCES += guimingw-font.c
# libguimingw_a_SOURCES += guimingw-resources.c
# EXTRA_DIST            += $(IncDir)/guimingw-util.h
# libguimingw_a_SOURCES += guimingw-util.c
# EXTRA_DIST            += $(IncDir)/guimingw-create.h
# libguimingw_a_SOURCES += guimingw-create.c
# EXTRA_DIST            += guimingw-dw.h
# libguimingw_a_SOURCES += guimingw-dw.c
# libguimingw_a_SOURCES += guimingw-dw-layout.c
# 
# libguimingw_a_SOURCES += widmingw-egs.c
# libguimingw_a_SOURCES += widmingw-simple.c
# libguimingw_a_SOURCES += widmingw-simple-slin.c
libguimingw_a_SOURCES += widmingw-spinner.c
# libguimingw_a_SOURCES += widmingw-button.c
# libguimingw_a_SOURCES += widmingw-buttonPanel.c
# libguimingw_a_SOURCES += widmingw-menu.c
# libguimingw_a_SOURCES += widmingw-canvas.c
# libguimingw_a_SOURCES += widmingw-filebrowser.c

#libguimingw_a_SOURCES += ../../libXaw3d-1.6.2/src/Label.c
#libguimingw_a_SOURCES += TMstate.c
#libguimingw_a_SOURCES += TMparse.c


#=========================================================================
