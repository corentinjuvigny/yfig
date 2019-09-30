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
# $Software: YFIG
#       $Id: Makefile.am 370 2018-11-23 00:35:34Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

InclDir    = ../include
EXTRA_DIST = 

noinst_LIBRARIES = libyfig.a 

libyfig_a_CPPFLAGS = -I $(YFIG_TOP_SRCDIR)/yfig/include -I $(YFIG_TOP_SRCDIR)/include
libyfig_a_SOURCES  = 

EXTRA_DIST        += $(InclDir)/yfig-config.h
EXTRA_DIST        += $(InclDir)/yfig-types.h
EXTRA_DIST        += $(InclDir)/yfig-drawparam.h
EXTRA_DIST        += yfig-main-argv-parser.c
libyfig_a_SOURCES += yfig-main.c 
EXTRA_DIST        += $(InclDir)/yfig-mw.h
libyfig_a_SOURCES += yfig-mw.c 
libyfig_a_SOURCES += yfig-scene.c 
EXTRA_DIST        += $(InclDir)/yfig-export-option.h
EXTRA_DIST        += $(InclDir)/yfig-dsds.h
libyfig_a_SOURCES += yfig-dsds.c 
EXTRA_DIST        += $(InclDir)/yfig-device.h
libyfig_a_SOURCES += yfig-device.c 
EXTRA_DIST        += $(InclDir)/yfig-pixmap.h
libyfig_a_SOURCES += yfig-pixmap.c 
libyfig_a_SOURCES += yfig-layers.c 

EXTRA_DIST        += topbar/topbar.h
libyfig_a_SOURCES += topbar.c 
libyfig_a_SOURCES += topbar/tb-file.c 
EXTRA_DIST        += $(InclDir)/ydw/tb-file-OMSF.h 
libyfig_a_SOURCES += topbar/tb-file-dw-OMF.c 
libyfig_a_SOURCES += topbar/tb-edit.c 
libyfig_a_SOURCES += topbar/tb-option.c 
libyfig_a_SOURCES += topbar/tb-debug.c 

EXTRA_DIST        += dppanel/dppanel.h
libyfig_a_SOURCES += dppanel.c 
libyfig_a_SOURCES += dppanel/l1l2base.c 
libyfig_a_SOURCES += dppanel/l1l2v3.c 
libyfig_a_SOURCES += dppanel/l1l2color.c 
libyfig_a_SOURCES += dppanel/top.c 
libyfig_a_SOURCES += dppanel/grid.c 
libyfig_a_SOURCES += dppanel/align.c 
libyfig_a_SOURCES += dppanel/line.c 
libyfig_a_SOURCES += dppanel/line-style.c 
libyfig_a_SOURCES += dppanel/line-arrow.c 
libyfig_a_SOURCES += dppanel/fill.c 
libyfig_a_SOURCES += dppanel/text.c 
libyfig_a_SOURCES += dppanel/text-font.c 
libyfig_a_SOURCES += dppanel/text-dir.c 

EXTRA_DIST        += $(InclDir)/yfig-efsmSelObj.h
libyfig_a_SOURCES += yfig-efsmSelObj.c 
EXTRA_DIST        += $(InclDir)/yfig-efsmSelObj+move.h
libyfig_a_SOURCES += yfig-efsmSelObj+move.c 
EXTRA_DIST        += $(InclDir)/yfig-efsmSelNObj.h
libyfig_a_SOURCES += yfig-efsmSelNObj.c 
EXTRA_DIST        += modepanel/modepanel.h
libyfig_a_SOURCES += modepanel.c 
libyfig_a_SOURCES += modepanel/arc.c 
libyfig_a_SOURCES += modepanel/ellipse.c 
libyfig_a_SOURCES += modepanel/line.c 
libyfig_a_SOURCES += modepanel/spline.c 
libyfig_a_SOURCES += modepanel/text.c 
libyfig_a_SOURCES += modepanel/rect-hv.c 
EXTRA_DIST        += modepanel/compound-do.c 
EXTRA_DIST        += modepanel/compound-open.c 
libyfig_a_SOURCES += modepanel/compound.c 
libyfig_a_SOURCES += modepanel/zmcd.c 
libyfig_a_SOURCES += modepanel/align.c 
libyfig_a_SOURCES += modepanel/marker.c 
libyfig_a_SOURCES += modepanel/symrot.c 
libyfig_a_SOURCES += modepanel/edit.c 
libyfig_a_SOURCES += modepanel/update.c 
libyfig_a_SOURCES += modepanel/arrowheads.c
libyfig_a_SOURCES += modepanel/measurelength.c

EXTRA_DIST        += $(InclDir)/ydw/ydw-arrow.h
libyfig_a_SOURCES += ydw/ydw-arrow.c 
EXTRA_DIST        += $(InclDir)/ydw/ydw-update.h
libyfig_a_SOURCES += ydw/ydw-update.c 
EXTRA_DIST        += $(InclDir)/ydw/ydw.h
libyfig_a_SOURCES += ydw/ydw-colors.c 
libyfig_a_SOURCES += ydw/ydw-colorPFT.c 
libyfig_a_SOURCES += ydw/ydw-textpen.c 
libyfig_a_SOURCES += ydw/ydw-pen.c 
libyfig_a_SOURCES += ydw/ydw-fill.c
libyfig_a_SOURCES += ydw/ydw-export-option.c
libyfig_a_SOURCES += ydw/ydw-export.c
EXTRA_DIST        += $(InclDir)/ydw/ydw-objedit.h
libyfig_a_SOURCES += ydw/ydw-objedit.c 
libyfig_a_SOURCES += ydw/ydw-objedit-header.c 
libyfig_a_SOURCES += ydw/ydw-objedit-du.c 
libyfig_a_SOURCES += ydw/ydw-objedit-arc.c 
libyfig_a_SOURCES += ydw/ydw-objedit-boxhv.c 
libyfig_a_SOURCES += ydw/ydw-objedit-ellipse.c 
libyfig_a_SOURCES += ydw/ydw-objedit-line.c 
libyfig_a_SOURCES += ydw/ydw-objedit-text.c 

#=========================================================================
