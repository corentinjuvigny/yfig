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
# $Software: YFIG-VGUI (Virtual Graphic User Interface)
#       $Id: Makefile.am 365 2018-11-18 23:45:10Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

InclDir    = ../include/vgui
EXTRA_DIST = 

noinst_LIBRARIES = libvgui.a 

libvgui_a_CPPFLAGS = -I $(YFIG_TOP_SRCDIR)/include
libvgui_a_SOURCES  = 
EXTRA_DIST        += vgui-config.h
EXTRA_DIST        += $(InclDir)/vgui-CPLUSPLUS.h

EXTRA_DIST        += $(InclDir)/vgui-names.h
libvgui_a_SOURCES += vgui-names.c 

EXTRA_DIST        += $(InclDir)/vgui-resources.h
EXTRA_DIST        += $(InclDir)/vgui-resources-macro.h
EXTRA_DIST        += $(InclDir)/vgui-resources-table.h
libvgui_a_SOURCES += vgui-resources.c 

EXTRA_DIST        += $(InclDir)/vgui-accelkey.h
libvgui_a_SOURCES += vgui-accelkey.c 

EXTRA_DIST        += $(InclDir)/vgui-types.h
EXTRA_DIST        += $(InclDir)/vgui.h
EXTRA_DIST        += $(InclDir)/vgui-colors.h
libvgui_a_SOURCES += vgui-main.c 
libvgui_a_SOURCES += vgui-print.c 

EXTRA_DIST        += $(InclDir)/vgui-pixmap.h
libvgui_a_SOURCES += vgui-pixmap.c 

EXTRA_DIST        += $(InclDir)/vgui-outdev.h
libvgui_a_SOURCES += vgui-outdev.c 

EXTRA_DIST        += $(InclDir)/vgui-efsm.h
libvgui_a_SOURCES += vgui-efsm.c 
EXTRA_DIST        += $(InclDir)/vgui-efsm-points.h
libvgui_a_SOURCES += vgui-efsm-points.c 
EXTRA_DIST        += $(InclDir)/vgui-mw.h
libvgui_a_SOURCES += vgui-mw.c 

EXTRA_DIST        += $(InclDir)/wid-egs.h
libvgui_a_SOURCES += wid-egs.c 
EXTRA_DIST        += $(InclDir)/wid-simple.h
libvgui_a_SOURCES += wid-simple.c 
EXTRA_DIST        += $(InclDir)/wid-button.h
libvgui_a_SOURCES += wid-button.c 
libvgui_a_SOURCES += wid-button-fuf.c 
EXTRA_DIST        += wid-lvuI.h $(InclDir)/wid-lvu.h 
libvgui_a_SOURCES += wid-lvu.c 
libvgui_a_SOURCES += wid-lvu-ddlstr.c 
libvgui_a_SOURCES += wid-lvu-ddlpxm.c 
libvgui_a_SOURCES += wid-lvu-out-text.c 
libvgui_a_SOURCES += wid-lvu-out-num.c 
libvgui_a_SOURCES += wid-lvu-out.c 
libvgui_a_SOURCES += wid-lvu-in-text.c 
libvgui_a_SOURCES += wid-lvu-in-num.c 
libvgui_a_SOURCES += wid-lvu-in.c 

EXTRA_DIST        += $(InclDir)/wid-buttonPanel.h
libvgui_a_SOURCES += wid-buttonPanel.c 

EXTRA_DIST        += $(InclDir)/wid-canvas.h
EXTRA_DIST        += $(InclDir)/wid-canvas-geomcb.h
EXTRA_DIST        += wid-canvasI.h
libvgui_a_SOURCES += wid-canvas.c 
libvgui_a_SOURCES += wid-canvas-unit.c 
libvgui_a_SOURCES += wid-canvas-action.c 
EXTRA_DIST        += $(InclDir)/wid-mousefun.h
libvgui_a_SOURCES += wid-mousefun.c 
EXTRA_DIST        += $(InclDir)/wid-menu.h
libvgui_a_SOURCES += wid-menu.c 
EXTRA_DIST        += $(InclDir)/wid-filebrowser.h
libvgui_a_SOURCES += wid-filebrowser.c 

EXTRA_DIST        += $(InclDir)/vgui-dw.h
libvgui_a_SOURCES += vgui-dw.c 
libvgui_a_SOURCES += vgui-dw-layout.c 
EXTRA_DIST        += $(InclDir)/vdw-title.h 
libvgui_a_SOURCES += vdw-title.c 
libvgui_a_SOURCES += vdw-gnv.c 
libvgui_a_SOURCES += vdw-pum.c 
libvgui_a_SOURCES += vdw-gsls.c 
EXTRA_DIST        += $(InclDir)/vdw-console.h
libvgui_a_SOURCES += vdw-console.c 
EXTRA_DIST        += $(InclDir)/vdw-color.h
libvgui_a_SOURCES += vdw-color.c 
EXTRA_DIST        += $(InclDir)/vdw-viewer.h
libvgui_a_SOURCES += vdw-viewer-pts.c 
EXTRA_DIST        += $(InclDir)/wid.h
libvgui_a_SOURCES += wid-grp.c 
EXTRA_DIST        += $(InclDir)/wid-grp.h
libvgui_a_SOURCES += wid-grp-pt.c 
libvgui_a_SOURCES += wid-grp-angle.c 

#=========================================================================
