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
# $Software: YFIG2DEV
#       $Id: Makefile.am 377 2018-12-09 22:39:34Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

YFIG_OBJDIR = ..
InclDir     = .
EXTRA_DIST  = 

bin_PROGRAMS    = yfig2dev

yfig2dev_SOURCES  = 
EXTRA_DIST       += ${InclDir}/yfig2dev-config.h
yfig2dev_SOURCES += main.c
EXTRA_DIST       += ${InclDir}/user-options.h
yfig2dev_SOURCES += user-options-depths.y
yfig2dev_SOURCES += user-options.c
EXTRA_DIST       += ${InclDir}/ds.h
yfig2dev_SOURCES += ds.c
yfig2dev_SOURCES += gen-pdf.c
yfig2dev_SOURCES += gen-tex.c
yfig2dev_SOURCES += gen-svg.c

yfig2dev_CPPFLAGS   = 
yfig2dev_CPPFLAGS  += -I $(YFIG_TOP_SRCDIR)/include

yfig2dev_LDADD      = 
yfig2dev_LDADD     += $(YFIG_OBJDIR)/fig/libfig.a
yfig2dev_LDADD     += $(YFIG_OBJDIR)/outdev/libodev.a
yfig2dev_LDADD     += $(YFIG_OBJDIR)/tools/libtools.a
yfig2dev_LDADD     += @PNG_LIBS@ @JPEG_LIBS@ -lm
