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
# $Software: YFIG-ODEV (a virtual Output DEVice)
#       $Id: Makefile.am 381 2018-12-24 04:44:15Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

IncDir     = ../include/outdev
EXTRA_DIST = 

noinst_LIBRARIES    = libodev.a 
libodev_a_CPPFLAGS  = @JPEG_CFLAGS@ @PNG_CFLAGS@
libodev_a_CPPFLAGS += -I $(YFIG_TOP_SRCDIR)/outdev
libodev_a_CPPFLAGS += -I $(YFIG_TOP_SRCDIR)/include
libodev_a_SOURCES   = 

EXTRA_DIST        += outdev-config.h
EXTRA_DIST        += $(IncDir)/types.h
EXTRA_DIST        += $(IncDir)/style.h
libodev_a_SOURCES += style.c
libodev_a_SOURCES += style-merge.c
libodev_a_SOURCES += style-dbg.c

EXTRA_DIST        += $(IncDir)/outdev.h
EXTRA_DIST        += $(IncDir)/outdev-vt.h
EXTRA_DIST        += $(IncDir)/top-vt.h
EXTRA_DIST        += $(IncDir)/color-vt.h
EXTRA_DIST        += $(IncDir)/pxm-vt.h
EXTRA_DIST        += $(IncDir)/ldaf-vt.h
EXTRA_DIST        += $(IncDir)/string-vt.h
EXTRA_DIST        += $(IncDir)/src-vt.h
EXTRA_DIST        += outdev-vt.c
libodev_a_SOURCES += outdev.c

EXTRA_DIST        += $(IncDir)/pxm-hl.h
libodev_a_SOURCES += pxm-hl.c
EXTRA_DIST        += $(IncDir)/ldaf-hl.h
libodev_a_SOURCES += ldaf-hl.c
libodev_a_SOURCES += ldaf-hl-gc.c
EXTRA_DIST        += $(IncDir)/string-hl.h
libodev_a_SOURCES += string-hl.c
EXTRA_DIST        += $(IncDir)/resources.h
libodev_a_SOURCES += resources.c

EXTRA_DIST        += outdev-drv.h
libodev_a_SOURCES += outdev-drv.c

EXTRA_DIST        += $(IncDir)/outdevPdf.h
EXTRA_DIST        += outdevPdf-priv.h
EXTRA_DIST        += outdevPdf-image.c
EXTRA_DIST        += outdevPdf-vt.c
libodev_a_SOURCES += outdevPdf-priv.c
libodev_a_SOURCES += outdevPdf-page.c
libodev_a_SOURCES += outdevPdf.c

EXTRA_DIST        += latex/outdevTex-priv.h
libodev_a_SOURCES += latex/outdevTex.c
libodev_a_SOURCES += latex/outdevTex-priv.c
libodev_a_SOURCES += latex/outdevTex-vt.c
libodev_a_SOURCES += latex/outdevTex-conv.c

EXTRA_DIST        += svg/outdevSvg-priv.h
libodev_a_SOURCES += svg/outdevSvg.c
libodev_a_SOURCES += svg/outdevSvg-priv.c
libodev_a_SOURCES += svg/outdevSvg-vt.c

# #########################################################################
