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
# $Software: YFIG  (YFIG driver to GTK)
#       $Id: Makefile.am 148 2018-06-04 06:59:33Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/xt/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

EXTRA_DIST      =

bin_PROGRAMS    = yfig-gtk 

yfig_gtk_SOURCES  = 
yfig_gtk_SOURCES += yfiggtk-main.c
yfig_gtk_SOURCES += yfiggtk-mw.c
yfig_gtk_SOURCES += tbgtk-file-OMSF.c

yfig_gtk_CPPFLAGS   = 
yfig_gtk_CPPFLAGS  += -I ../../include/guigtk
yfig_gtk_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/gtk
yfig_gtk_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/include -I $(YFIG_TOP_SRCDIR)/include
yfig_gtk_CPPFLAGS  += @GTK3_CFLAGS@

yfig_gtk_LDADD      = 
yfig_gtk_LDADD     += ../common/libyfig.a
yfig_gtk_LDADD     += ../../vgui/libvgui.a
yfig_gtk_LDADD     += ../../guigtk/libguigtk.a
yfig_gtk_LDADD     += ../../vgui/libvgui.a
yfig_gtk_LDADD     += ../../fig/libfig.a
yfig_gtk_LDADD     += ../../outdev/libodev.a
yfig_gtk_LDADD     += ../../tools/libtools.a
yfig_gtk_LDADD     += @GTK3_LIBS@
yfig_gtk_LDADD     += @JPEG_LIBS@ @PNG_LIBS@
yfig_gtk_LDADD     += -lm

#=========================================================================
