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
# $Software: YFIG-TOOL (utilities shared by the different modules)
#       $Id: Makefile.am 374 2018-12-01 13:56:19Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/Makefile.am $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

IncDir     = ../include/tools
EXTRA_DIST = 

noinst_LIBRARIES     = libtools.a 
libtools_a_CPPFLAGS  = -I $(YFIG_TOP_SRCDIR)/include
libtools_a_SOURCES   = 

EXTRA_DIST += tools-config.h
EXTRA_DIST += $(IncDir)/CPLUSPLUS.h
EXTRA_DIST += $(IncDir)/set-header-template.h
EXTRA_DIST += $(IncDir)/set-body-template.h
EXTRA_DIST += $(IncDir)/math-macros.h

EXTRA_DIST         += $(IncDir)/base-types.h
libtools_a_SOURCES += base-types.c
libtools_a_SOURCES += base-types-debug.c
EXTRA_DIST         += $(IncDir)/lenunit.h
libtools_a_SOURCES += lenunit.c
EXTRA_DIST         += $(IncDir)/globalvars.h
libtools_a_SOURCES += globalvars.c
EXTRA_DIST         += $(IncDir)/messagedriver.h
libtools_a_SOURCES += messagedriver.c
EXTRA_DIST         += $(IncDir)/strings.h
libtools_a_SOURCES += strings-convert.c
libtools_a_SOURCES += strings-filename.c
EXTRA_DIST         += $(IncDir)/string-builder.h
libtools_a_SOURCES += string-builder.c
EXTRA_DIST         += $(IncDir)/charset.h
EXTRA_DIST         += charset-table.h
libtools_a_SOURCES += charset.c
libtools_a_SOURCES += charset-table-unicode.c
libtools_a_SOURCES += charset-others.c
EXTRA_DIST         += $(IncDir)/boxes.h
libtools_a_SOURCES += boxes.c
EXTRA_DIST         += $(IncDir)/range.h
libtools_a_SOURCES += range.c
EXTRA_DIST         += $(IncDir)/file.h
libtools_a_SOURCES += file.c
if GLOB_AVAILABLE
libtools_a_SOURCES += fileset.c
endif
EXTRA_DIST         += $(IncDir)/geo2d.h
libtools_a_SOURCES += geo2d.c
libtools_a_SOURCES += geo2d-ellipseToMLine.c
libtools_a_SOURCES += geo2d-splineToMLine.c
libtools_a_SOURCES += geo2d-rect.c
libtools_a_SOURCES += geo2d-rect.c
EXTRA_DIST         += $(IncDir)/papersize.h
libtools_a_SOURCES += papersize.c
EXTRA_DIST         += $(IncDir)/color-types.h ../include/tools/color.h
libtools_a_SOURCES += color.c
libtools_a_SOURCES += color-rgb2hlv.c
libtools_a_SOURCES += color-dbg.c
EXTRA_DIST         += $(IncDir)/lnc.h
libtools_a_SOURCES += lnc.c
EXTRA_DIST         += $(IncDir)/grid.h
libtools_a_SOURCES += grid.c
EXTRA_DIST         += $(IncDir)/bitmap-types.h
EXTRA_DIST         += $(IncDir)/bitmap.h
libtools_a_SOURCES += bitmap.c
libtools_a_SOURCES += bitmap-pattBtms.c
libtools_a_SOURCES += bitmap-lighShadBtms.c
libtools_a_SOURCES += bpi-image.c
EXTRA_DIST += $(IncDir)/font-types.h
EXTRA_DIST += $(IncDir)/font.h
libtools_a_SOURCES += font.c
libtools_a_SOURCES += font-default.c
EXTRA_DIST += $(IncDir)/imgfmt.h
libtools_a_SOURCES += imgfmt.c
EXTRA_DIST += $(IncDir)/cmdline.h
libtools_a_SOURCES += cmdline.c
libtools_a_SOURCES += missing.c

# #########################################################################
