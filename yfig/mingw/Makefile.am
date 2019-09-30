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
# $Software: YFIG  (YFIG driver to  WIN32 EGS under MinGw)
#       $Id: Makefile.am 213 2018-06-13 09:12:50Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/mingw/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

EXTRA_DIST      =

bin_PROGRAMS    = yfig-mingw 

yfig_mingw_SOURCES  = 
yfig_mingw_SOURCES += yfigmingw-main.c
yfig_mingw_SOURCES += yfigmingw-mw.c
yfig_mingw_SOURCES += tbmingw-file-OMSF.c

yfig_mingw_CPPFLAGS   = 
yfig_mingw_CPPFLAGS  += -I ../../include/guimingw
yfig_mingw_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/mingw
yfig_mingw_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/include -I $(YFIG_TOP_SRCDIR)/include
yfig_mingw_CPPFLAGS  += @YFIG_mingw_cppflags@

yfig_mingw_LDADD      = 
yfig_mingw_LDADD     += ../common/libyfig.a
yfig_mingw_LDADD     += ../../vgui/libvgui.a
yfig_mingw_LDADD     += ../../guimingw/libguimingw.a
yfig_mingw_LDADD     += ../../vgui/libvgui.a
yfig_mingw_LDADD     += ../../fig/libfig.a
yfig_mingw_LDADD     += ../../outdev/libodev.a
yfig_mingw_LDADD     += ../../tools/libtools.a
yfig_mingw_LDADD     += @YFIG_mingw_libs@
yfig_mingw_LDADD     += @JPEG_LIBS@ @PNG_LIBS@
yfig_mingw_LDADD     += -lm

#=========================================================================
