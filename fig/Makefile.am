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
# $Software: YFIG-FDS (Figure Data Structure)
#       $Id: Makefile.am 377 2018-12-09 22:39:34Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

InclDir    = ../include/fig
EXTRA_DIST = 

noinst_LIBRARIES = libfig.a 

libfig_a_CPPFLAGS  = -I ../include/ -I $(YFIG_TOP_SRCDIR)/include
EXTRA_DIST += fig-config.h

EXTRA_DIST += $(InclDir)/fig-types.h
EXTRA_DIST += $(InclDir)/figure.h
EXTRA_DIST += $(InclDir)/fig-histo.h
EXTRA_DIST += $(InclDir)/object.h
EXTRA_DIST += $(InclDir)/object-priv.h
EXTRA_DIST += $(InclDir)/obj-vtable.h
EXTRA_DIST += $(InclDir)/obj-any.h
EXTRA_DIST += $(InclDir)/obj-arc.h
EXTRA_DIST += $(InclDir)/obj-ellipse.h
EXTRA_DIST += $(InclDir)/obj-line.h
EXTRA_DIST += $(InclDir)/obj-spline.h
EXTRA_DIST += $(InclDir)/obj-text.h
EXTRA_DIST += $(InclDir)/obj-boxhv.h
EXTRA_DIST += $(InclDir)/obj-arrow.h
EXTRA_DIST += $(InclDir)/obj-compound.h

libfig_a_SOURCES  = 
libfig_a_SOURCES += figure.c
libfig_a_SOURCES += fig-histo.c
EXTRA_DIST += $(InclDir)/fig-scene.h
libfig_a_SOURCES += fig-scene.c
libfig_a_SOURCES += fig-scene-pdf.c
libfig_a_SOURCES += fig-scene-tex.c
libfig_a_SOURCES += fig-scene-svg.c

libfig_a_SOURCES += obj-vtable.c
libfig_a_SOURCES += obj-util.c
libfig_a_SOURCES += obj-hierarchy.c
libfig_a_SOURCES += obj-any.c
libfig_a_SOURCES += obj-arc.c
libfig_a_SOURCES += obj-ellipse.c
libfig_a_SOURCES += obj-line.c
libfig_a_SOURCES += obj-spline.c
libfig_a_SOURCES += obj-text.c
libfig_a_SOURCES += obj-boxhv.c
EXTRA_DIST       += obj-arrows/arrow-vtable-template.h
EXTRA_DIST       += obj-arrows/00.c
EXTRA_DIST       += obj-arrows/01.c
EXTRA_DIST       += obj-arrows/02to04.c
libfig_a_SOURCES += obj-arrow.c
libfig_a_SOURCES += obj-compound.c
EXTRA_DIST       += fig-io.h
libfig_a_SOURCES += fig-save.c
libfig_a_SOURCES += fig-load.c

EXTRA_DIST       += io/xfig32.h
libfig_a_SOURCES += io/xfig32-save.c
libfig_a_SOURCES += io/xfig32-load.c
libfig_a_SOURCES += io/xfig32-converter.c

