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
# $Software: YFIG  (YFIG driver to Xt and Xawd or Xaw3d)
#       $Id: Makefile.am 213 2018-06-13 09:12:50Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/xt/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

EXTRA_DIST      =

bin_PROGRAMS    = yfig-xt 

yfig_xt_SOURCES  = 
yfig_xt_SOURCES += yfigxt-main.c
yfig_xt_SOURCES += yfigxt-mw.c
yfig_xt_SOURCES += tbxt-file-OMSF.c

yfig_xt_CPPFLAGS   = 
yfig_xt_CPPFLAGS  += -I ../../include/guixt
yfig_xt_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/xt
yfig_xt_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/yfig/include -I $(YFIG_TOP_SRCDIR)/include
yfig_xt_CPPFLAGS  += @YFIG_xt_cppflags@

yfig_xt_LDADD      = 
yfig_xt_LDADD     += ../common/libyfig.a
yfig_xt_LDADD     += ../../vgui/libvgui.a
yfig_xt_LDADD     += ../../guixt/libguixt.a
yfig_xt_LDADD     += ../../vgui/libvgui.a
yfig_xt_LDADD     += ../../fig/libfig.a
yfig_xt_LDADD     += ../../outdev/libodev.a
yfig_xt_LDADD     += ../../tools/libtools.a
yfig_xt_LDADD     += @YFIG_xt_libs@ @PANGOXFT_LIBS@
yfig_xt_LDADD     += -lfontconfig -lfreetype
yfig_xt_LDADD     += @JPEG_LIBS@ @PNG_LIBS@
yfig_xt_LDADD     += -lm

#=========================================================================
