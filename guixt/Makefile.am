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
# $Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
#       $Id: Makefile.am 213 2018-06-13 09:12:50Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

SrcDir     = .
IncDir     = ../include/guixt
EXTRA_DIST = 

noinst_LIBRARIES = libguixt.a

libguixt_a_CPPFLAGS  = -I ../include/guixt -I ../include/
libguixt_a_CPPFLAGS += -I $(YFIG_TOP_SRCDIR)/include -I $(top_builddir)/include
libguixt_a_CPPFLAGS += @YFIG_xt_cppflags@ @PANGOXFT_CFLAGS@
libguixt_a_SOURCES   = 

EXTRA_DIST         += guixt.h
EXTRA_DIST         += $(IncDir)/guixt-config.h

EXTRA_DIST         += $(IncDir)/guixt-top.h
libguixt_a_SOURCES += guixt-init.c
libguixt_a_SOURCES += guixt-color.c
EXTRA_DIST         += $(IncDir)/guixt-event.h
libguixt_a_SOURCES += guixt-event.c
EXTRA_DIST         += guixt-outdev-vt-pxm.c
EXTRA_DIST         += guixt-outdev-vt-ldaf-gc.c
EXTRA_DIST         += guixt-outdev-vt-ldaf.c
libguixt_a_SOURCES += guixt-outdev-vt.c
libguixt_a_SOURCES += guixt-pixmap.c
EXTRA_DIST         += pangoxft-config.h.in
if PANGOXFT
libguixt_a_SOURCES += guixt-font-pango.c
else
libguixt_a_SOURCES += guixt-font-x11.c
endif
libguixt_a_SOURCES += guixt-resources.c
EXTRA_DIST         += $(IncDir)/guixt-util.h
libguixt_a_SOURCES += guixt-util.c
EXTRA_DIST         += $(IncDir)/guixt-create.h
libguixt_a_SOURCES += guixt-create.c
EXTRA_DIST         += guixt-dw.h
libguixt_a_SOURCES += guixt-dw.c
libguixt_a_SOURCES += guixt-dw-layout.c

libguixt_a_SOURCES += widxt-egs.c
libguixt_a_SOURCES += widxt-simple.c
libguixt_a_SOURCES += widxt-simple-slin.c
libguixt_a_SOURCES += widxt-simple-spinner.c
libguixt_a_SOURCES += widxt-button.c
libguixt_a_SOURCES += widxt-buttonPanel.c
libguixt_a_SOURCES += widxt-menu.c
libguixt_a_SOURCES += widxt-canvas.c
libguixt_a_SOURCES += widxt-filebrowser.c

#libguixt_a_SOURCES += ../../libXaw3d-1.6.2/src/Label.c
#libguixt_a_SOURCES += TMstate.c
#libguixt_a_SOURCES += TMparse.c


#=========================================================================
