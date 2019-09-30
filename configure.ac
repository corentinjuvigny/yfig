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
#       $Id: configure.ac 374 2018-12-01 13:56:19Z ia $
#  $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/configure.ac $
#   $Author: Ivan Auge (Email: auge@ensiie.fr)
#=========================================================================

dnl # ####################################################################
dnl # SET THE VERSION NUMBER AND TOP PARAMETERS
dnl # CAUTION: Only M4 code here (shell code is ignored by autoconf
dnl #          until the AC_INIT below).
define([M4_version_major],0)
define([M4_version_minor],1)
define([M4_version_branch],0)
define([M4_version_mm],M4_version_major.M4_version_minor)
define([M4_version],M4_version_mm.M4_version_branch)

#svn_branch=trunk
#svn_url="http://lunix120.ensiie.fr/ia/yfig"
#download_url="http://lunix120.ensiie.fr/download"

dnl # ####################################################################
dnl # autoconf setup
dnl AC_INIT(yfig,[1.0],,,http://lunix120.ensiie.fr/ia/yfig)
AC_INIT(yfig,[M4_version])
AC_CONFIG_SRCDIR([tools/base-types.c])
AC_PREFIX_DEFAULT([/usr/local/])

version="M4_version"

echo "Configuring yfig version $version with arguments:"
echo "  $*"
AM_INIT_AUTOMAKE(subdir-objects)
AM_SILENT_RULES([yes])

dnl # ####################################################################
dnl # TOP @xxxxxx@ TOKENS THAT ARE RECOGNIZED
dnl #
dnl #    @YFIG_VERSION@       : release number (major.minor.branch)
dnl #    @YFIG_VERSION_MM@    : release number (major.minor)
dnl #    @YFIG_VERSION_MAJOR@    : release number (major)
dnl #    @YFIG_VERSION_MINOR@    : release number (minor)
dnl #    @YFIG_VERSION_BRANCH@   : release number (branch)
dnl #
dnl #    @YFIG_TOP_SRCDIR@    : absolute path of yfig sources directory.
dnl #    @YFIG_TOP_OBJDIR@    : absolute path of yfig object directory.
dnl #
dnl # ####################################################################
AC_SUBST(YFIG_VERSION,       [AC_PACKAGE_VERSION])
AC_SUBST(YFIG_VERSION_MM,    [M4_version_mm])
AC_SUBST(YFIG_VERSION_MAJOR, [M4_version_major])
AC_SUBST(YFIG_VERSION_MINOR, [M4_version_minor])
AC_SUBST(YFIG_VERSION_BRANCH,[M4_version_branch])

YFIG_abs_srcdir="$(cd $srcdir ; pwd)"
YFIG_abs_objdir="$(pwd)"
AC_SUBST(YFIG_TOP_SRCDIR,"$YFIG_abs_srcdir")
AC_SUBST(YFIG_TOP_OBJDIR,"$YFIG_abs_objdir")

dnl # ####################################################################
dnl # ## user options for target yfigs
dnl # ##
dnl # ## Generate shell variables:
dnl # ##    gen_yfig_xt            : yes/no/auto (OS Linux only)
dnl # ##    gen_yfig_qt    : yes/no/auto 
dnl # ##    gen_yfig_gtk   : yes/no/auto 
dnl # ##    gen_yfig_mingw : yes/no/auto (OS MinGw only)
dnl # ##
dnl # ## Notes:
dnl # ##  - auto is the default
dnl # ##  - disbaled is set when the initial value is auto and packages
dnl # ##    are missing.
dnl # ##
dnl # ## options for yfig_xt (OS Linux only):
dnl # ##    gen_yfig_xt_with_xaw3d : yes/no (default yes)
dnl # ##    gen_yfig_xt_with_pango : yes/no (default yes)
dnl OLD YFIG_with_xaw3d="$enableval"],

AC_ARG_ENABLE(xt,
    [ AS_HELP_STRING([--enable-xt],[Indicates if yfig-xt (version based on
        the XT/XAW/XAW3D tool kit) must be generated or not.
        The default is autodetect.])],
    [ case "$enableval" in
        no|yes|auto) ;;
        *) AC_MSG_ERROR([Invalid value for --enable-xt switch (valid values are no, yes or auto.)]) ;;
      esac
      gen_yfig_xt="$enableval"],
    [ gen_yfig_xt=auto ]
)

AC_ARG_ENABLE(qt,
    [ AS_HELP_STRING([--enable-qt],[Indicates if yfig-qt (version based on
        QT tool kit) must be generated or not. The default is autodetect.])],
    [ case "$enableval" in
        no|yes|auto) ;;
        *) AC_MSG_ERROR([Invalid value for --enable-qt switch (valid values are no, yes or auto).])
      esac
      gen_yfig_qt="$enableval"],
    [ gen_yfig_qt=auto ]
)

AC_ARG_ENABLE(gtk,
    [ AS_HELP_STRING([--enable-gtk],[Indicates if yfig-gtk (version based on
        GTK+3 tool kit) must be generated or not. The default is autodetect.])],
    [ case "$enableval" in
        no|yes|auto) ;;
        *) AC_MSG_ERROR([Invalid value for --enable-gtk switch (valid values are no, yes or auto).])
      esac
      gen_yfig_gtk="$enableval"],
    [ gen_yfig_gtk=auto ]
)

AC_ARG_ENABLE(mingw,
    [ AS_HELP_STRING([--enable-mingw],[Indicates if yfig-mingw (version based on
        win32 tool kit) must be generated or not. The default is autodetect.])],
    [ case "$enableval" in
        no|yes|auto) ;;
        *) AC_MSG_ERROR([Invalid value for --enable-mingw (valid values are no, yes or auto).])
      esac
      gen_yfig_mingw="$enableval"],
    [ gen_yfig_mingw=auto ]
)

dnl # ## options for yfig_xt (OS Linux only):
AC_ARG_ENABLE(
    xaw3d,
    AS_HELP_STRING([--disable-xaw3d],[Disable support for xaw3d
        in yfig-xt and so modern fonts (default is enabled).])],
    [ case "$enableval" in
        no|yes) gen_yfig_xt_with_xaw3d=$enableval;;
        *) AC_MSG_ERROR(m4_normalize([
               "$enableval" is an invalid value for --enable-xaw3d switch
               (valid values are "no" and "yes".)]))
      esac],
    [gen_yfig_xt_with_xaw3d=yes]
)

AC_ARG_ENABLE([pangoxft],
   [AS_HELP_STRING([--disable-pangoxft],[Disable support for pangoxft
        in yfig-xt and so modern fonts (default is enabled).])],
   [ case "$enableval" in
        no|yes) gen_yfig_xt_with_pango=$enableval ;; 
        *) AC_MSG_ERROR(m4_normalize([
               "$enableval" is an invalid value for --enable-pangoxft switch
               (valid values are "no" and "yes".)]))
      esac
   ],
   [gen_yfig_xt_with_pango=yes]
)

dnl # ## options for yfig_qt
YFIG_qmake="@undefined"
AC_ARG_WITH(
    qmake,
    AS_HELP_STRING([--with-qmake=cmd],[path of qmake (Default: search in PATH).]),
    [YFIG_qmake="$withval"],
)

AC_ARG_WITH(
    qt-version,
    AS_HELP_STRING([--with-qt-version=N],[4 for QT4, 5 for QT5
        (Default: auto detected using "qmake --version")].),
    [ case "$withval" in
        4|5) ;;
        *)
            AC_MSG_ERROR([Invalid value for --with-qt-version (valid values are 4 and 5.)])
      esac
      YFIG_with_qt_version="$withval"],
    [YFIG_with_qt_version=""]
)

dnl # ## options for yfig_qt
# set YFIG_memlimit to N (unit:Mb, 0:disabled)
AC_ARG_WITH(
    mem-limit,
    AS_HELP_STRING([--with-mem-limit=#],[limit the memory amount of YFIG to # Mb
        (Default: 1024). Set it to zero to disable this feature.]),
    [ case "$withval" in
        [[0-9]] ) ;;
        [[0-9]][[0-9]] ) ;;
        [[0-9]][[0-9]][[0-9]] ) ;;
        [[0-9]][[0-9]][[0-9]][[0-9]] ) ;;
        *)
            AC_MSG_ERROR([[value "$withval" is invalid for --with-mem-limit=# (must be in [0:1024])]])
     esac
     test "$withval" -gt 1024 &&  AC_MSG_ERROR([[value "$withval" is invalid for --with-mem-limit=# (must be in [0:1024])]])
     YFIG_memlimit="$withval"],
    [YFIG_memlimit="1024"]
)

AC_ARG_ENABLE(
    dbg-temp,
    AS_HELP_STRING([--enable-dbg-temp],[Activates temporary watchdog]),
    [AC_DEFINE(TOOL_DBG_SpOn,1,[Activate the TEMP_..._printf macros])],
    enable_dbg_temp=no)

AC_ARG_ENABLE(
    dbg-trace,
    AS_HELP_STRING([--enable-dbg-trace],[enable code for tracing]),
    [AC_DEFINE(TOOL_DBG_Trace,1,[Activate the DBG._printf macros])],
    enable_dbg_trace=no)

AC_ARG_ENABLE(
    dbg-mem,
    AS_HELP_STRING([--enable-dbg-mem],[enable code that checks the
        memory allocation and free of colors, fonts and figure objects.]),
    [AC_DEFINE(TOOL_DBG_MemClr,1,[Check memory allocation and freeing of colors.])
     AC_DEFINE(TOOL_DBG_MemFnt,1,[Check memory allocation and freeing of fonts.])
     AC_DEFINE(TOOL_DBG_MemObj,1,[Check memory allocation and freeing of figure objects.])
    ],
    enable_dbg_mem=no)

AC_ARG_ENABLE(
    dbg-dev,
    AS_HELP_STRING([--enable-dbg-dev],[enable code dedicated to development such
        as the debug menu in the top menu bar.]),
    [AC_DEFINE(TOOL_DBG_Dev,1,[Activate features that are dedicated to development.])],
    enable_dbg_dev=no)

AC_ARG_ENABLE(
    dbg,
    AS_HELP_STRING([--enable-dbg],[Short cut to --enable-dbg-temp/trace/mem/dev]),
    [AC_DEFINE(TOOL_DBG_SpOn,1,[Activate the TEMP_..._printf macros])
     AC_DEFINE(TOOL_DBG_Trace,1,[Activate the DBG._printf macros])
     AC_DEFINE(TOOL_DBG_MemClr,1,[Check memory allocation and freeing of colors.])
     AC_DEFINE(TOOL_DBG_MemFnt,1,[Check memory allocation and freeing of fonts.])
     AC_DEFINE(TOOL_DBG_MemObj,1,[Check memory allocation and freeing of figure objects.])
     AC_DEFINE(TOOL_DBG_Dev,1,[Activate features that are dedicated to development.])
     enable_dbg_temp=yes
     enable_dbg_trace=yes
     enable_dbg_mem=yes
     enable_dbg_dev=yes
    ])

dnl # ####################################################################
dnl # Target Operating System
dnl #
dnl # get build: the system on which the package is built
dnl # get host:  the system where built programs and libraries will run.
dnl #
dnl # On my unbuntu pc (x86_64-pc-linux-gnu) when I generate YFIG for windows
dnl # using mingw, I must run:
dnl #   $ ./configure [--build X86_64-pc-linux-gnu] --host i686-mingw32
# The --build is not mandatory autoconf can get it (./config.guess)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
dnl # Generate shell variable:
dnl #    host  host_cpu  host_vendor  host_os
dnl #   build build_cpu build_vendor build_os
dnl #   tos_linux   : 1/0 Target Operating System is/is not Linux
dnl #   tos_mingw   : 1/0 Target Operating System is/is not MinGw
dnl # Generate CPP macros in config.h:
dnl #   TOS_LINUX   : either set to 1 (is) or 0 (is not)
dnl #   TOS_MINGW   : either set to 1 (is) or 0 (is not)
dnl # Generate substitution tokens:
dnl #   none
dnl # Generate conditional 
dnl #   TosIsLinux  :
dnl #   TosIsMinGw  :

tos_linux=0; tos_mingw=0
case "$host_os" in 
    *Linux*) tos_linux=1 ;;
    *linux*) tos_linux=1 ;;
    *mingw*) tos_mingw=1 ;;
esac
AC_DEFINE_UNQUOTED(TOS_LINUX,[$tos_linux],[set to 1/0 if Target Operating System is/is not Linus])
AC_DEFINE_UNQUOTED(TOS_MINGW,[$tos_mingw],[set to 1/0 if Target Operating System is/is not MinGw])
AM_CONDITIONAL([TosIsLinux],[test $tos_linux = 1])
AM_CONDITIONAL([TosIsMinGw],[test $tos_mingw = 1])

dnl # ####################################################################
dnl # check for miscellaneous tools
PKG_PROG_PKG_CONFIG()

dnl # ####################################################################
dnl # ## check for C & C++ compiler
dnl # ## check for C headers included in config.h

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_YACC

AC_CHECK_HEADERS(
  math.h,
  [ac_includes_default="\
$ac_includes_default
#include <math.h>"],
   AC_MSG_ERROR([math.h must be available])
)
AC_CHECK_HEADERS(
  stdarg.h,
  [ac_includes_default="\
$ac_includes_default
#include <stdarg.h>"],
   AC_MSG_ERROR([stdarg.h must be available])
)

AC_CHECK_DECLS([strdup,bzero,index])
AC_CHECK_FUNC([strdup],,AC_MSG_ERROR([strdup must be available]))
AC_CHECK_FUNCS([bzero index])

AC_CHECK_HEADERS(
  fcntl.h,
  [ac_includes_default="\
$ac_includes_default
#include <fcntl.h>"],
)

AC_CHECK_DECLS([creat])
AC_CHECK_FUNC([creat],,AC_MSG_ERROR([creat must be available]))

# checking for libgen.h and basename function
AC_CHECK_HEADER(libgen.h,
  [ac_includes_default="\
$ac_includes_default
#include <libgen.h>"],
  [AC_INCLUDES_DEFAULT])
AC_CHECK_DECLS([basename],,[
    yfig_extra_funcdecls="$yfig_extra_funcdecls
#undef basename
char *basename(char *);"])
AC_CHECK_FUNC([basename],,AC_MSG_ERROR([basename must be available]))

# check for errno.h and error code like ERANGE macro
AC_CHECK_HEADER(errno.h,
  [errno_h_present=1
   ac_includes_default="\
$ac_includes_default
#include <errno.h>"]
  errno_h_present=0,
  [AC_INCLUDES_DEFAULT])
AC_CHECK_DECL(ERANGE,,
    AC_MSG_ERROR([errno.h and/or error codes like ERANGE must be available]),AC_INCLUDES_DEFAULT)

# check for PATH_MAX
AC_CHECK_DECL(PATH_MAX,,,AC_INCLUDES_DEFAULT)
if test $ac_cv_have_decl_PATH_MAX = no ; then 
  ac_includes_default_save="$ac_includes_default"
  AC_CHECK_HEADER(limits.h,
      [limits_h_present=1
       ac_includes_default="\
$ac_includes_default
#include <limits.h>"],
      [limits_h_present=0],
      [AC_INCLUDES_DEFAULT])
      if test $limits_h_present = 1 ; then
        unset ac_cv_have_decl_PATH_MAX
        AC_CHECK_DECL(PATH_MAX,,,AC_INCLUDES_DEFAULT)
      fi
      if test $ac_cv_have_decl_PATH_MAX = no ; then 
        ac_includes_default="$ac_includes_default_save"
        yfig_extra_defines="$yfig_extra_defines
#ifndef PATH_MAX
#define PATH_MAX 1025
#endif"
        AC_MSG_NOTICE([define PATH_MAX to 1025])
      fi
fi

# check for ctype.h isdigit isalpha, ...
AC_CHECK_HEADER(ctype.h,
  [ctype_h_present=1
   ac_includes_default="\
$ac_includes_default
#include <ctype.h>"],
  [ctype_h_present=0],
  [AC_INCLUDES_DEFAULT])
AC_CHECK_DECLS([isdigit],,[
                yfig_extra_funcdecls="$yfig_extra_funcdecls
#undef isdigit
int isdigit(int);"],[AC_INCLUDES_DEFAULT])
AC_CHECK_FUNC([isdigit],,AC_MSG_ERROR(["int isdigit(int c)" must be available]))

# check for time.h, time_t and time()
AC_CHECK_HEADER(time.h,
[ac_includes_default="$ac_includes_default
#include <time.h>"])
AC_CHECK_DECLS([time],,
[yfig_extra_funcdecls="$yfig_extra_funcdecls
#undef time
time_t time(time_t*);"])
AC_CHECK_TYPE([time_t],,AC_MSG_ERROR(["time_t type" must be available]))
AC_CHECK_FUNC([time],,AC_MSG_ERROR(["time_t time(time_t)" must be available]))

dnl # ## Checks glob
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   GLOB: 0/1 glob is unavailable/available
dnl # ## Generate CPP macros in config.h:
dnl # ##   GLOB_AVAILABLE          : either set to 1 or 0
dnl # ##   HAVE_GLOB_H             : either set to 1 or 0
dnl # ##   HAVE_DECL_GLOB/GLOBFREE : either set to 1 or 0
dnl # ##   HAVE_GLOB/GLOBFREE      : either set to 1 or undefined
dnl # ## Generate substitution tokens:
dnl # ##   @glob_available@ : 0/1: unavailable/available
dnl # ## Generate conditional 
dnl # ##   GLOB_AVAILABLE
GLOB=1
AC_CHECK_HEADERS([glob.h],
  [   AC_CHECK_TYPE([glob_t],
       [ AC_CHECK_DECLS([glob,globfree],[],[],[[#include <glob.h>]])
         AC_CHECK_FUNCS([glob globfree],,[GLOB=0])],
       [GLOB=0],[[#include <glob.h>]])
  ],[ AC_CHECK_TYPE([glob_t],
       [ AC_CHECK_DECLS([glob,globfree],[],[])
         AC_CHECK_FUNCS([glob globfree],,[GLOB=0])],
       [GLOB=0])
  ])
if test $GLOB = 0 -a "x$gen_yfig_xt" != "xno" ; then
    if test "x$gen_yfig_xt" = "xyes" ; then
        AC_MSG_ERROR([glob utility must be available for generating yfig-xt,
   - you must fix the problem using CFLAGS and/or LDFLAGS,
   - or you can disable yfig-xt with --disable-xt.])
    else 
        AC_MSG_NOTICE([generation of yfig-xt is disabled.])
        gen_yfig_xt="disabled"
    fi
fi
if test $GLOB = 0 ; then
    AC_DEFINE(GLOB_AVAILABLE,0,[indicate if glob feature is either available or not])
    AC_SUBST([glob_available],"0")
else
    AC_DEFINE(GLOB_AVAILABLE,1,[indicate if glob feature is either available or not])
    AC_SUBST([glob_available],"1")
fi
AM_CONDITIONAL([GLOB_AVAILABLE],[test $GLOB = 1])

dnl # ## Checks getrlimit and setrlimit
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   GSRLIMIT: 1/0 g/setrlimit is available/unavailable
dnl # ## Generate CPP macros in config.h:
dnl # ##   GSRLIMIT_AVAILABLE      : either set to 1 or 0
dnl # ##   HAVE_SYS_TYPES_H        : either set to 1 or 0
dnl # ##   HAVE_SYS_RESOURCE_H     : either set to 1 or 0
dnl # ##   HAVE_DECL_GET/SETRLIMIT : either set to 1 or 0
dnl # ##   HAVE_GET/SETRLIMIT      : either set to 1 or undefined
GSRLIMIT=1
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/resource.h],
  [   includes="
#if defined(HAVE_SYS_TYPES_H) && HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/resource.h>"
      AC_CHECK_DECLS([getrlimit,setrlimit],[],[GSRLIMIT=0],[$includes])
      AC_CHECK_FUNCS([getrlimit setrlimit],[],[GSRLIMIT=0],[$includes])
  ],[
      GSRLIMIT=0
  ],[
#if defined(HAVE_SYS_TYPES_H) && HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
  ])
test $GSRLIMIT = 0 && AC_DEFINE(GSRLIMIT_AVAILABLE,0,[0/1 the get/setrlimit functions are available/unavailable])
test $GSRLIMIT = 1 && AC_DEFINE(GSRLIMIT_AVAILABLE,1,[0/1 the get/setrlimit functions are available/unavailable])
if test $GSRLIMIT = 0 -a $YFIG_memlimit -ne 0 ; then
    AC_MSG_WARN([disable the option that limits the memory amount (--with-mem-limit=$YFIG_memlimit).]) 
    YFIG_memlimit=0
fi
test $YFIG_memlimit -ne 0 && AC_DEFINE_UNQUOTED(YFIG_MemLimitMb,[$YFIG_memlimit],[maximal memory amount in Mb that YFIG can use.])

# check for locale.h and setlocale
AC_CHECK_HEADER(locale.h,AC_DEFINE(HAVE_LOCALE_H,1,
            [indicate if locale.h is either available or not]))
AC_CHECK_DECLS([setlocale],,,[[#include <locale.h>]])
AC_CHECK_FUNC([setlocale],AC_DEFINE(HAVE_SETLOCALE,1,
            [indicate if setlocale function exists and is usable]))

# check for iconv
AM_ICONV
if test "$am_cv_func_iconv" != "yes" ; then 
    AC_MSG_ERROR([the "iconv" features must be available])
fi

dnl # ####################################################################
dnl # ## Checks libjpeg and libpng
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   LIBJPEG: 1 has libjpeg  suppoort otherwise 0
dnl # ##   LIBPNG : 1 has libpng12 suppoort otherwise 0
dnl # ## Generate CPP macros in config.h:
dnl # ##   HAVE_JPEGLIB_H: either set to 1 or undefined
dnl # ##   HAVE_LIBPNG_H : either set to 1 or undefined
dnl # ##   LIBJPEG_AVAILABLE: either set to 1 or undefined
dnl # ##   LIBPNG_AVAILABLE : either set to 1 or undefined
dnl # ## Generate substitution tokens:
dnl # ##   @JPEG_CFLAGS@ : CPP flags required for libjpeg 
dnl # ##   @JPEG_LIBS@   : LD  flags and library required for libjpeg 
dnl # ##   @PNG_CFLAGS@  : CPP flags required for libjpeg 
dnl # ##   @PNG_LIBS@    : LD  flags and library required for libjpeg 

LIBJPEG=1
PKG_CHECK_MODULES(JPEG,[libjpeg],
        [AC_DEFINE([HAVE_JPEGLIB_H],    [1], [Define to 1 if you have the <jpeglib.h> header file.])],
        [LIBJPEG=0]
        )
# search if libjpeg is really usable or really not present
AC_MSG_CHECKING([for JPEG in usual directories])
AC_MSG_RESULT([])
if test "$JPEG_LIBS" = "" ; then JPEG_LIBS="-ljpeg" ; fi
CPPFLAGS_save="$CPPFLAGS"
LIBS_save="$LIBS"
  CPPFLAGS="$JPEG_CFLAGS $CPPFLAGS"
  LIBS="$JPEG_LIBS $LIBS"
  cont=1
  AC_CHECK_DECLS([JCS_BG_RGB,JCS_BG_YCC],,[cont=0],[$ac_includes_default
#include <jpeglib.h>
    ])
  if test $cont = 1 ; then
    AC_CHECK_FUNCS([jpeg_std_error jpeg_destroy_decompress jpeg_read_header],,[cont=0])
  fi
CPPFLAGS="$CPPFLAGS_save"
LIBS="$LIBS_save"
AC_MSG_CHECKING([for JPEG in usual directories])
if test $cont = 0 ; then
    AC_MSG_RESULT([no])
    LIBJPEG=0
else
    AC_MSG_RESULT([yes])
    LIBJPEG=1
    AC_DEFINE([LIBJPEG_AVAILABLE], [1], [Define to 1 if libjpeg is available])
fi
echo cont=$cont LIBJPEG=$LIBJPEG JPEG_CFLAGS=$JPEG_CFLAGS JPEG_LIBS=$JPEG_LIBS

LIBPNG=1
PKG_CHECK_MODULES(PNG,[libpng >= 1.2],
        [AC_DEFINE([HAVE_LIBPNG_H],    [1], [Define to 1 if you have the <libpng.h> header file.])
         AC_DEFINE([LIBPNG_AVAILABLE], [1], [Define to 1 if libpng is available])],
        [LIBPNG=0]
        )

dnl # ####################################################################
dnl # ## #define HAVE_X11 0/1 --> in config.h
dnl # ## #define USE_X11  0/1 --> in config.h
dnl # ##
dnl # ## HAVE_X11: if X11 is available.
dnl # ## USE_X11 : if the external graphic framework uses X11.
dnl # ##

# HAVE_X11
X11_MinVers=1.6

x11=0
if test "x$X11_CFAGS" = "x" -a "x$X11_LIBS" = "x" ; then
  PKG_CHECK_MODULES(X11,[x11 >= $X11_MinVers],
      [x11=1],
      [AC_MSG_NOTICE([$X11_PKG_ERRORS])])
  if test x11 = 1 ; then 
    PKG_CHECK_MODULES(X11,[x11 >= $X11_MinVers])
  fi
else
  x11=1
fi

if test $x11 = 1 ; then
  AC_CHECK_HEADER(X11/X.h,
     [],
     [x11=0],
     [AC_INCLUDES_DEFAULT])
fi

if test $x11 = 1 ; then
  CPPFLAGS_save="$CPPFLAGS"
  LIBS_save="$LIBS"

  CPPFLAGS="$CPPFLAGS $X11_CFLAGS"
  LIBS="$LIBS $X11_LIBS"
  x11_includes="\
$ac_includes_default
#include <X11/X.h>"
  AC_CHECK_FUNC([XOpenDisplay],,x11=0,[$x11_includes])

  CPPFLAGS="$CPPFLAGS_save"
  LIBS="$LIBS_save"
fi
if test $x11 = 1 ; then
  AC_DEFINE(HAVE_X11,1,[indicate if X11 is either available or not])
else
  AC_DEFINE(HAVE_X11,0,[indicate if X11 is either available or not])
fi

# USE_X11
if test $x11 = 0 ; then
  use_x11=0
else
  case "$host_os" in 
    *Linux*) use_x11=1 ;;
    *linux*) use_x11=1 ;;
    *)       use_x11=0 ;;
  esac
fi
AC_DEFINE_UNQUOTED(USE_X11,[$use_x11],[sets to 1 if the external graphic framework uses X11])

dnl # ####################################################################
dnl # ## Checks for XAW XAW3D 
dnl # ## required for src/xt/config.h
dnl # ##
dnl # ## Choose between xaw or xaw3d toolkiks.
dnl # ## one of these toolkits is required for yfig-xt.
dnl # ##
dnl # ## gen_yfig_xt=yes      --> yfig-xt must be generated
dnl # ## gen_yfig_xt=no       --> yfig-xt must not be generated
dnl # ## gen_yfig_xt=disabled --> generation of yfig-xt was in auto
dnl # ##                          detection mode and a problem occurs.
dnl # ##
dnl # ## @YFIG_xt_gen@            --> either "" or "xt" 
dnl # ## @YFIG_xt_gui@            --> either "" or "guixt" 
dnl # ## @YFIG_xt_xaw@            --> either 0  or 1 
dnl # ## @YFIG_xt_xaw3d@          --> either 0  or 1 
dnl # ## @YFIG_xt_cppflags@       --> either "" or "CPPFLAGS for yfig-xt"
dnl # ## @YFIG_xt_libs@           --> either "" or "-L ... -l... for yfig-xt"
dnl # ## @YFIG_xt_x11_includes@   --> either "" or "X11 includes" 
dnl # ## @YFIG_xt_xaw_includes@   --> either "" or "xaw7 includes" 
dnl # ## @YFIG_xt_xaw3d_includes@ --> either "" or "xaw3D includes" 

if test "x${gen_yfig_xt}" != "xno" ; then
XAW_MinVers="1.0.10"
XAW3D_MinVers="1.6.0"

# first check for xaw3d (except if user disabled it)
xaw3d=0
if test "x$YFIG_with_xaw3d" != "xno" ; then
  if test "x$XAW3D_CFLAGS" != "x" ||
     test "x$XAW3D_LIBS"   != "x" ; then
    # given by user
    xaw3d=1
  else
    PKG_CHECK_MODULES(XAW3D,[xaw3d >= $XAW3D_MinVers],
      [xaw3d=1],
      [AC_MSG_NOTICE([$XAW3D_PKG_ERRORS])])
  fi
fi
# second if no xaw3d available try xaw
xaw=0;
if test $xaw3d = 0 ; then
  if test "x$XAW_CFLAGS" != "x" ||
     test "x$XAW_LIBS"   != "x" ; then
    # given by user
    xaw=1
  else
    PKG_CHECK_MODULES(XAW,[xaw7 >= $XAW_MinVers],
      [xaw=1],
      [xaw=0
       AC_MSG_NOTICE([$XAW_PKG_ERRORS])
       AC_MSG_NOTICE([
 Disable yfig-xt with --disable-xt,
 or make xaw7 or xaw3d available by pkg-config,
 or use the --with-xaw-cppflags/libs switches to force cppflags/libs of xaw xt and x11,
 or use the --with-xaw3d-cppflags/libs switches to force cppflags/libs of xaw3d xt and x11,])
       ])
  fi
fi

if test $xaw3d = 0 -a $xaw = 0 ; then
    if test "x${gen_yfig_xt}" = "xyes" ; then
       AC_MSG_ERROR([
 Either xaw7 or xaw3d are mandatory for generating yfig-xt. So you can:
   - disable yfig-xt with --disable-xt,
   - make xaw7 or xaw3d available by pkg-config,
   - use the --with-xaw-cppflags/libs switches to force cppflags/libs of xaw xt and x11,
   - use the --with-xaw3d-cppflags/libs switches to force cppflags/libs of xaw3d xt and x11,])
    else # auto is asked so we disable yfig-xt
      gen_yfig_xt="disabled"
    fi
else # at this point xaw3d and xaw are either (0,1) or (1,0)
    if test $xaw = 1 ; then 
        if  test "x$XAW_CFLAGS" = "x" -a "x$XAW_LIBS" = "x" ; then
            PKG_CHECK_MODULES(XAW,[x11 >= 1.0 xaw >= $XAW_MinVers])
        fi
        XT_CFLAGS="$XAW_CFLAGS"
        XT_LIBS="$XAW_LIBS"
    fi
    if test $xaw3d = 1 ; then
        if test "x$XAW3D_CFLAGS" = "x" -a "x$XAW3D_LIBS" = "x" ; then
            PKG_CHECK_MODULES(XAW,[xaw3d >= $XAW3D_MinVers])
        fi
        XT_CFLAGS="$XAW3D_CFLAGS"
        XT_LIBS="$XAW3D_LIBS"
    fi

    CPPFLAGS_save="$CPPFLAGS"
    LIBS_save="$LIBS"

    if test xaw = 1 ; then
      CPPFLAGS="$CPPFLAGS_save $XAW_CFLAGS"
      LIBS="$LIBS_save $XAW_LIBS"
    else
      CPPFLAGS="$CPPFLAGS_save $XAW3D_CFLAGS"
      LIBS="$LIBS_save $XAW3D_LIBS"
    fi

    # check usability of X11 includes
    x11_files="X11/X.h X11/cursorfont.h"
    x11_files="$x11_files X11/StringDefs.h X11/Shell.h"
    x11_files="$x11_files X11/Xatom.h X11/XKBlib.h"
    AC_CHECK_HEADERS($files,,AC_MSG_ERROR(This header must be available))
    include=""
    for f in $x11_files ; do
        includes="$includes
#include <$f>"
    done
    AC_SUBST([YFIG_xt_x11_includes],"$includes")
    x11_includes="\
$ac_includes_default
$includes"

    # check usability of XAW includes if needed
    includes=""
    if test $xaw = 1 ; then
        files=""
        files="$files X11/Xaw/Command.h"
        files="$files X11/Xaw/Label.h"
        files="$files X11/Xaw/Dialog.h"
        files="$files X11/Xaw/Box.h"
        files="$files X11/Xaw/Form.h"
        files="$files X11/Xaw/Cardinals.h"
        files="$files X11/Xaw/Text.h"
        files="$files X11/Xaw/AsciiText.h"
        files="$files X11/Xaw/MenuButton.h"
        files="$files X11/Xaw/Scrollbar.h"
        files="$files X11/Xaw/Simple.h"
        files="$files X11/Xaw/SimpleMenu.h"
        files="$files X11/Xaw/Sme.h"
        files="$files X11/Xaw/SmeLine.h"
        files="$files X11/Xaw/Toggle.h"
        files="$files X11/Xaw/Paned.h"
        files="$files X11/Xaw/Viewport.h"
        files="$files X11/Xaw/List.h"
        files="$files X11/Xaw/SmeBSB.h"
        AC_CHECK_HEADERS($files,
          [],
          [if test "x$gen_yfig_xt" = "xyes" ; then
              AC_MSG_ERROR([This header must be available for generating yfig-xt, you can:
   - fix the problem,
   - use the --with-xaw-cppflags/libs switches to force cppflags/libs of xaw xt and x11,
   - disable yfig-xt with --disable-xt.])
           else
              gen_yfig_xt="disabled"
              AC_MSG_NOTICE([Disabling the generation of yfig-xt])
          fi],
          [$x11_includes])
        if test "x$gen_yfig_xt" != "disabled" ; then 
          for f in $files ; do
            includes="$includes
#include <$f>"
          done
        fi
    fi
    AC_SUBST([YFIG_xt_xaw_includes],"$includes")

    # check usability of XAW3D includes if needed
    includes=""
    if test $xaw3d = 1 ; then
        files=""
        files="$files X11/Xaw3d/Command.h"
        files="$files X11/Xaw3d/Label.h"
        files="$files X11/Xaw3d/Dialog.h"
        files="$files X11/Xaw3d/Box.h"
        files="$files X11/Xaw3d/Form.h"
        files="$files X11/Xaw3d/Cardinals.h"
        files="$files X11/Xaw3d/Text.h"
        files="$files X11/Xaw3d/AsciiText.h"
        files="$files X11/Xaw3d/MenuButton.h"
        files="$files X11/Xaw3d/Scrollbar.h"
        files="$files X11/Xaw3d/Simple.h"
        files="$files X11/Xaw3d/SimpleMenu.h"
        files="$files X11/Xaw3d/Sme.h"
        files="$files X11/Xaw3d/SmeLine.h"
        files="$files X11/Xaw3d/Tip.h"
        files="$files X11/Xaw3d/Toggle.h"
        files="$files X11/Xaw3d/Paned.h"
        files="$files X11/Xaw3d/Viewport.h"
        files="$files X11/Xaw3d/List.h"
        files="$files X11/Xaw3d/SmeBSB.h"
        AC_CHECK_HEADERS($files,
          [],
          [if test "x$gen_yfig_xt" = "xyes" ; then
              AC_MSG_ERROR([This header must be available for generating yfig-xt, you can:
   - fix the problem,
   - use the --with-xaw3d-cppflags/libs switches to force cppflags/libs of xaw3d xt and x11,
   - try to use xaw7 instead of xaw3d with --disable-xaw3d,
   - disable yfig-xt with --disable-xt.])
           else
              gen_yfig_xt="disabled"
              AC_MSG_NOTICE([Disabling the generation of yfig-xt])
          fi],
          [$x11_includes])
        if test "x$gen_yfig_xt" != "disabled" ; then 
          for f in $files ; do
            includes="$includes
#include <$f>"
          done
        fi
    fi
    AC_SUBST([YFIG_xt_xaw3d_includes],"$includes")

    CPPFLAGS="$CPPFLAGS_save"
    LIBS="$LIBS_save"

    # change gen_yfig_xt variable from auto to yes
    if test "$gen_yfig_xt" = "auto" ; then
        gen_yfig_xt=yes
    fi
fi 
fi

if test "$gen_yfig_xt" = "yes" ; then
  AC_SUBST([YFIG_xt_gen],"xt")
  AC_SUBST([YFIG_xt_gui],"guixt")
  AC_SUBST([YFIG_xt_cppflags],"$XT_CFLAGS")
  AC_SUBST([YFIG_xt_libs],"$XT_LIBS")
  AC_SUBST([YFIG_xt_xaw],"$xaw")
  AC_SUBST([YFIG_xt_xaw3d],"$xaw3d")
else
  AC_SUBST([YFIG_xt_gen],"")
  AC_SUBST([YFIG_xt_gui],"")
  AC_SUBST([YFIG_xt_cppflags],"")
  AC_SUBST([YFIG_xt_libs],"")
  AC_SUBST([YFIG_xt_x11_includes],"")
  AC_SUBST([YFIG_xt_xaw_includes],"")
  AC_SUBST([YFIG_xt_xaw3d_includes],"")
fi

dnl # ## Checks PANGOXFT
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   PANGOXFT      : 0/1 PangoXft is unavailable/available
dnl # ##   XFT_VERS      : XFT version
dnl # ##   PANGOXFT_VERS : PANGOXFT version
dnl # ##   PANGOXFT_INCLUDES   :  either "" or  "pango-xft #includes"
dnl # ##   PANGOXFT_CFLAGS     : CPP flags 
dnl # ##   PANGOXFT_LIBS       : LD  flags and libraries
dnl # ## Generate CPP macros in config.h:
dnl # ##   PANGOXFT_AVAILABLE  : either set to 1 or 0
dnl # ## Generate substitution tokens:
dnl # ##   @pangoxft_includes@  : either "" or  "pango-xft #includes" 
dnl # ##   @PANGOXFT_CFLAGS@  : CPP flags 
dnl # ##   @PANGOXFT_LIBS@    : LD  flags and libraries
dnl # ## Generate conditional 
dnl # ##   PANGOXFT           : true/false

if test $gen_yfig_xt != no -a $gen_yfig_xt != disabled \
     -a $gen_yfig_xt_with_pango = yes ; then
    m4_include(config.ac/pangoxft-check.ac)
else
    PANGOXFT=0
fi

if test $PANGOXFT = 1 ; then
      XFT_VERS="$($PKG_CONFIG --modversion xft)"
    PANGO_VERS="$($PKG_CONFIG --modversion pangoxft)"
   #PANGOXFT_INCLUDES=
   #PANGOXFT_CFLAGS=
   #PANGOXFT_LIBS=
else
      XFT_VERS=""
    PANGO_VERS=""
    PANGOXFT_INCLUDES=""
    PANGOXFT_CFLAGS=""
    PANGOXFT_LIBS=""
fi
AC_DEFINE_UNQUOTED([PANGOXFT_AVAILABLE],[$PANGOXFT],[indicates if PANGOXFT is available])
AC_SUBST([pangoxft_includes],[$PANGOXFT_INCLUDES])
AC_SUBST([PANGOXFT_CFLAGS],[$PANGOXFT_CFLAGS])
AC_SUBST([PANGOXFT_LIBS],[$PANGOXFT_LIBS])
AM_CONDITIONAL([PANGOXFT],[test $PANGOXFT = 1])
    
dnl # ####################################################################
dnl # ## Checks for QT
dnl # ## required for include/guiqt/config.h
dnl # ##
dnl # ## @YFIG_qt_gen@            --> either "" or "qt" 
dnl # ## @YFIG_qt_gui@            --> either "" or "guiqt" 
dnl # ## @YFIG_qmake@             --> either "" or "path to qmake" 
dnl # ## @YFIG_qt_major@          --> either "" or "QT_VERS_4/5" 
dnl # ## @YFIG_qt_debug@          --> either "" or "CONFIG  += debug"

# search for absolute PATH of qmake
if test "x$gen_yfig_qt" != "xno" ; then
    if test "x$YFIG_qmake" = "x@undefined" ; then
        ac_prog_tmp=""
        AC_PATH_PROG(tmp,qmake,no,[$PATH])
        YFIG_qmake="$tmp"
        if test "x$YFIG_qmake" = "xno" ; then
          if test "x$gen_yfig_qt" = "xauto" ; then
              gen_yfig_qt="disabled"
              AC_MSG_NOTICE([Generation of yfig-qt is disabled])
          else
              AC_MSG_ERROR([qmake is mandatory to generate yfig-qt. You can:
  - Make qmake command accessible through the PATH.
  - Fix the path of the qmake command with the --with-qmake=path_to_qmake switch.
  - Disable yfig-qt with --disable-qt.
              ])
          fi
        fi
    else
        case "$YFIG_qmake" in
           /* )
              ;;
           *  )
              ac_prog_tmp=""
              AC_PATH_PROG(tmp,$YFIG_qmake,no,[$PATH])
              YFIG_qmake="$tmp"
              if test "x$YFIG_qmake" = "xno" ; then
                AC_MSG_ERROR([qmake is mandatory to generate yfig-qt. You can:
  - Make qmake command accessible through the PATH.
  - Fix the path of the qmake command with the --with-qmake=path_to_qmake switch.
  - Disable yfig-qt with --disable-qt.
                ])
              fi
              ;;
        esac
    fi
fi # "xYFIG_with_qt" != "xno"

# check if "qmake --version" can be run

# search for QT version that qmake handles
if test "x$YFIG_with_qt_version" = "x" -a \
        "x$gen_yfig_qt" != "xno"       -a \
        "x$gen_yfig_qt" != "xdisabled" ; then
    AC_MSG_CHECKING([for running "$YFIG_qmake --version"])
    YFIG_qt_version=$( $YFIG_qmake --version | 
       sed -n -e ['s/.*Qt.*\([0-9]\+\.[0-9]\+\.[0-9]\+\).*/\1/p'] )
    if test "x$YFIG_qt_version" != "x" ; then
      AC_MSG_RESULT([$YFIG_qt_version])
    else
      AC_MSG_RESULT(failed "$YFIG_qt_version")
      if test "x$gen_yfig_qt" = "xauto" ; then
          gen_yfig_qt="disabled"
          AC_MSG_NOTICE([Generation of yfig-qt is disabled])
      else
          AC_MSG_ERROR(
[running "qmake --version" is mandatory to generate yfig-qt. You can:
  - Force the QT version using the switch: --with-qt-version=4 or 5.
  - Make qmake command accessible through the PATH.
  - Fix the path of the qmake command with the --with-qmake=path_to_qmake switch.
  - Disable yfig-qt with --disable-qt.
          ])
      fi
    fi
fi

# Check QT version
if test "x$YFIG_with_qt_version" = "x" -a \
        "x$gen_yfig_qt" != "xno"       -a \
        "x$gen_yfig_qt" != "xdisabled" ; then
    case "$YFIG_qt_version" in 
        [5.[0-4].*)]
            YFIG_qt_version_error=">=5.5.0"
            YFIG_qt_version_major=""
            ;;
        [5.[5-9].*)]
            YFIG_qt_version_error=""
            YFIG_qt_version_major=5
            ;;
        [5.1[0-9].*)]
            YFIG_qt_version_error=""
            YFIG_qt_version_major=5
            ;;
        4.8.*)
            YFIG_qt_version_error=""
            YFIG_qt_version_major=4
            ;;
        4.*.*)
            YFIG_qt_version_error=">=4.8.0"
            YFIG_qt_version_major=""
            ;;
        *.*.*)
            YFIG_qt_version_error=">=4.8.0 for QT4 and >=5.5.0 for QT5"
            YFIG_qt_version_major=""
            ;;
    esac
    AC_MSG_CHECKING([Checking QT version $YFIG_qt_version"])
    if test "x$YFIG_qt_version_error" = "x" ; then
        AC_MSG_RESULT([ok])
    else
        AC_MSG_RESULT([failed expected $YFIG_qt_version_error])
        if test "x$gen_yfig_qt" = "xauto" ; then
            gen_yfig_qt="disabled"
            AC_MSG_NOTICE([Generation of yfig-qt is disabled])
        else
            AC_MSG_ERROR(
  [QT4 >= 4.8.0 or QT5 >= 5.5.0 are mandatory to generate yfig-qt. You can:
    - Given a valid qmake command through the PATH.
    - Given a valid qmake command with --with-qmake=path_to_qmake switch.
    - Force the QT version using the switch: --with-qt-version=4 or 5.
    - Disable yfig-qt with --disable-qt.])
       fi
    fi
fi

# use the QT version gviven by the user
if test "x$YFIG_with_qt_version" != "x" -a \
        "x$gen_yfig_qt" != "xno"       -a \
        "x$gen_yfig_qt" != "xdisabled" ; then
        YFIG_qt_version_major=$YFIG_with_qt_version
fi

if test "x$gen_yfig_qt" = "xauto" ; then
    gen_yfig_qt="yes"
fi

if test "x$gen_yfig_qt" = "xyes" ; then
  AC_SUBST([YFIG_qt_gen],"qt")
  AC_SUBST([YFIG_qt_gui],"guiqt")
  AC_SUBST([YFIG_qt_qmake],"$YFIG_qmake")
  AC_SUBST([YFIG_qt_major],"QT_VERS_$YFIG_qt_version_major")
  if test $enable_dbg_trace = yes -o $enable_dbg_mem = yes -o \
          $enable_dbg_dev = yes ; then
      case "$host_os" in
        *mingw* ) AC_SUBST([YFIG_qt_debug],"") ;;
              * ) AC_SUBST([YFIG_qt_debug],"CONFIG  += debug") ;;
      esac
  else
        AC_SUBST([YFIG_qt_debug],"")
  fi
else
  AC_SUBST([YFIG_qt_gen],"")
  AC_SUBST([YFIG_qt_gui],"")
  AC_SUBST([YFIG_qt_qmake],"")
  AC_SUBST([YFIG_qt_major],"")
  AC_SUBST([YFIG_qt_debug],"")
fi

dnl # ####################################################################
dnl # ## Handle the yfig-gtk generation
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   gen_yfig_gtk      : yes or no (no more auto)
dnl # ##   GTK3_AVAILABLE    : 1/0 package is/is not available
dnl # ##   GTK3_VERS         : version if the pkg is available
dnl # ## Generate CPP macros in config.h:
dnl # ## Generate substitution tokens:
dnl # ##   @YFIG_gtk_gen@ --> either "" or "gtk" 
dnl # ##   @YFIG_gtk_gui@ --> either "" or "guigtk" 

if test $gen_yfig_gtk = no ; then
    GTK3_AVAILABLE=0
else
    m4_include(config.ac/gtk3-check.ac)
fi

if test $GTK3_AVAILABLE = 1 ; then
    GTK3_VERS="$($PKG_CONFIG --modversion gtk+-3.0)"
fi

if test $gen_yfig_gtk = yes -a $GTK3_AVAILABLE = 0 ; then
    AC_MSG_ERROR([
  yfig-gtk requires the GTK+-3.0 package. You can
    - help me to find the GTK+-3.0 package by setting the pkg-config search path.
    - help me to find the GTK+-3.0 package by setting the GTK3_/CFLAGS/LIBS variables.
    - disable yfig-gtk (--disable-gtk switch).])
fi
test $gen_yfig_gtk = auto -a $GTK3_AVAILABLE = 0 && gen_yfig_gtk=no
test $gen_yfig_gtk = auto -a $GTK3_AVAILABLE = 1 && gen_yfig_gtk=yes
if test $gen_yfig_gtk = yes ; then
    AC_SUBST([YFIG_gtk_gen],"gtk")
    AC_SUBST([YFIG_gtk_gui],"guigtk")
else
    AC_SUBST([YFIG_gtk_gen],"")
    AC_SUBST([YFIG_gtk_gui],"")
fi

dnl # ####################################################################
dnl # ## Handle the yfig2dev generation
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   gen_yfig2dev      : yes or no (no more auto)
dnl # ## Generate CPP macros in config.h:
dnl # ## Generate substitution tokens:
dnl # ##   @YFIG2DEV@ --> either "" or "yfig2dev" 

AC_ARG_ENABLE(yfig2dev,
    [ AS_HELP_STRING([--enable-yfig2dev],[Indicates if yfig2dev must be
        generated or not. The default is autodetect.])],
    [ case "$enableval" in
        no|yes|auto) ;;
        *) AC_MSG_ERROR([Invalid value for --enable-yfig2dev switch (valid values are no, yes or auto).])
      esac
      gen_yfig2dev="$enableval"],
    [ gen_yfig2dev=auto ]
)

if test $gen_yfig2dev != no ; then
    # require at least libjpeg or libpng
    if  test $LIBJPEG = 1 -o $LIBPNG = 1 ; then
        gen_yfig2dev=yes
    else
        if test $gen_yfig2dev = auto ; then
            gen_yfig2dev=no
        else
            AC_MSG_ERROR([
  yfig2dev requires either libjpeg or libpng that are missing. You can:
   - disable yfig2dev with --disable-yfig2dev,
   - make libjpeg or libpng available (the 2 is better),
   - rerun me setting the jpeg and/or png compilation environment variable
     (run ./configure --help to get their names).])
        fi
    fi
fi

if test $gen_yfig2dev = yes ; then
    AC_SUBST([YFIG2DEV],"yfig2dev")
else
    AC_SUBST([YFIG2DEV],"")
fi

dnl # ####################################################################
dnl # ## Handle the yfig-mingw generation
dnl # ##
dnl # ## Generate shell variable:
dnl # ##   gen_yfig_mingw  : yes or no (no more auto)
dnl # ##   MINGW_AVAILABLE : 1/0 package is/is not available
dnl # ##   MINGW_VERS      : version if pkg is available
dnl # ##   MINGW_AVAILABLE : 1/0 package is/is not available
dnl # ##   WIN32_INCLUDES  :  either "" or  "package #includes"
dnl # ##   WIN32_CFLAGS    : either "" or  "package compilation flags"
dnl # ##   WIN32_LIBS      : either "" or  "package LD flags and libraries"
dnl # ## Generate substitution tokens:
dnl # ##   @YFIG_mingw_gen@ --> either "" or "mingw" 
dnl # ##   @YFIG_mingw_gui@ --> either "" or "guimingw" 
dnl # ##   @WIN32_INCLUDES@ --> either "" or  "package #includes"
dnl # ##   @WIN32_CFLAGS@   --> either "" or  "package compilation flags"
dnl # ##   @WIN32_LIBS@     --> either "" or  "package LD flags and libraries"

if test $gen_yfig_mingw = no ; then
    MINGW_AVAILABLE=0
else
    m4_include(config.ac/mingw-check.ac)
fi

if test $MINGW_AVAILABLE = 1 ; then
   #MINGW_VERS=???
    AC_SUBST([YFIG_mingw_gen],"mingw")
    AC_SUBST([YFIG_mingw_gui],"guimingw")
    AC_SUBST([WIN32_INCLUDES],["$WIN32_INCLUDES"])
    AC_SUBST([WIN32_CFLAGS],["$WIN32_CFLAGS"])
    AC_SUBST([WIN32_LIBS],["$WIN32_LIBS"])
else
    AC_SUBST([YFIG_mingw_gen],"")
    AC_SUBST([YFIG_mingw_gui],"")
    AC_SUBST([WIN32_INCLUDES],"")
    AC_SUBST([WIN32_CFLAGS],[""])
    AC_SUBST([WIN32_LIBS],[""])
fi

if test $gen_yfig_mingw = yes -a $MINGW_AVAILABLE = 0 ; then
    AC_MSG_ERROR([yfig-mingw requires the MINGW environment])
fi
test $gen_yfig_mingw = auto -a $MINGW_AVAILABLE = 0 && gen_yfig_mingw=no
test $gen_yfig_mingw = auto -a $MINGW_AVAILABLE = 1 && gen_yfig_mingw=yes

dnl # ########################################################################
dnl # ## generation of the outputs

touch config.h.toappend
cat > config.h.toappend << EOF

/***************************************************************/
/* include the system headers                                  */
$ac_includes_default

/***************************************************************/
/* define macros that was not found in the system headers      */
$yfig_extra_defines

/***************************************************************/
/* declare functions that was not found in the system headers  */
$yfig_extra_funcdecls
EOF

dnl AH_BOTTOM()
AC_CONFIG_HEADERS([config.h:config.h.in:config.h.toappend])

AC_CONFIG_FILES(
  Makefile
  include/yfig-version.h
  tools/Makefile
  outdev/Makefile
  fig/Makefile
  vgui/Makefile
  yfig/Makefile
  yfig/common/Makefile
)
if test $gen_yfig2dev = yes ; then
  AC_CONFIG_FILES(
    yfig2dev/Makefile
  )
fi
if test $gen_yfig_xt = yes ; then
  AC_CONFIG_FILES(
    guixt/Makefile
    include/guixt/guixt-config.h
    guixt/pangoxft-config.h
    yfig/xt/Makefile
  )
fi
if test $gen_yfig_qt = yes ; then
  AC_CONFIG_FILES(
    guiqt/Makefile
    include/guiqt/guiqt-config.h
    guiqt/guiqt.pro
    yfig/qt/Makefile
    yfig/qt/yfigqt.pro
  )
fi
if test $gen_yfig_gtk = yes ; then
  AC_CONFIG_FILES(
    guigtk/Makefile
    yfig/gtk/Makefile
  )
fi
if test $gen_yfig_mingw = yes ; then
  AC_CONFIG_FILES(
    guimingw/Makefile
    include/guimingw/guimingw-config.h
    yfig/mingw/Makefile
  )
fi

AC_OUTPUT

dnl # ########################################################################
dnl # ## STATUS

src_HAVE_X11=$(if test $x11    = 1 ; then echo yes ; else echo no; fi)
src_USE_X11=$(if test $use_x11 = 1 ; then echo yes ; else echo no; fi)
AC_MSG_NOTICE([======================================================])
AC_MSG_NOTICE([==== Flags Configuration])
AC_MSG_NOTICE([====   CPPFLAGS  = ${CPPFLAGS}])
AC_MSG_NOTICE([====   CFLAGS    = ${CFLAGS}])
AC_MSG_NOTICE([====   CXXFLAGS  = ${CXXFLAGS}])
AC_MSG_NOTICE([====   LDFLAGS   = ${LDFLAGS}])
AC_MSG_NOTICE([====   LIBS      = ${LIBS}])
AC_MSG_NOTICE([====   HAVE_X11  = ${src_HAVE_X11}])
AC_MSG_NOTICE([====   USE_X11   = ${src_USE_X11}])
AC_MSG_NOTICE([====])

if test $x11 = 1 ; then
    str_HAVE_X11=yes
    str_X11_CPPFLAGS="not used"
    str_X11_LIBS="not used"
else
    str_HAVE_X11=no
    str_X11_CPPFLAGS="$X11_CFLAGS"
    str_X11_LIBS="$X11_LIBS"
fi
AC_MSG_NOTICE([==== X11 Configuration])
AC_MSG_NOTICE([====   HAVE_X11     = $str_HAVE_X11])
AC_MSG_NOTICE([====   X11_CPPFLAGS = $str_X11_CPPFLAGS])
AC_MSG_NOTICE([====   X11_LIBS     = $str_X11_LIBS])
AC_MSG_NOTICE([====])

str_YFIG_with_xt="$gen_yfig_xt"
str_XAW_TOOLKIT="not used"
str_XAW3D_TOOLKIT="not used"
str_XT_CPPFLAGS="not used"
str_XT_LIBS="not used"
str_PANGOXFT_PKG="not used"
str_PANGOXFT_CPPFLAGS="not used"
str_PANGOXFT_LIBS="not used"
if test "$gen_yfig_xt" = "yes" ; then
  if test "$xaw" = 1 ; then
    str_XAW_TOOLKIT="used"
  elif test "$xaw3d" = 1 ; then
    str_XAW3D_TOOLKIT="used"
  else
      AC_MSG_NOTICE([Internal Bug: conflict between variables: gen_yfig_xt=$gen_yfig_xt xaw=$xaw and xaw3d=$xaw3d.])
  fi
  str_XT_CPPFLAGS="$XT_CFLAGS"
  str_XT_LIBS="$XT_LIBS"
  test $PANGOXFT = 1 && str_PANGOXFT_PKG="used" 
  test $PANGOXFT = 1 && str_PANGOXFT_CPPFLAGS="$PANGOXFT_CFLAGS"
  test $PANGOXFT = 1 && str_PANGOXFT_LIBS="$PANGOXFT_LIBS"
fi

libjpg_str=disable; test $LIBJPEG = 1 && libjpg_str=enable
libpng_str=disable; test $LIBPNG  = 1 && libpng_str=enable
AC_MSG_NOTICE([==== yfig2dev Configuration])
AC_MSG_NOTICE([====   yfig2dev          = ${gen_yfig2dev}])
if test $gen_yfig2dev = yes ; then
AC_MSG_NOTICE([====   libpng            = $libpng_str]) 
AC_MSG_NOTICE([====   libjpeg           = ${libjpg_str}])
fi
AC_MSG_NOTICE([====])

AC_MSG_NOTICE([==== yfig-xt Configuration])
AC_MSG_NOTICE([====   yfig-xt           = ${str_YFIG_with_xt}])
AC_MSG_NOTICE([====   XAW7  toolkit     = ${str_XAW_TOOLKIT}])
AC_MSG_NOTICE([====   XAW3D toolkit     = ${str_XAW3D_TOOLKIT}])
AC_MSG_NOTICE([====   PANGOXFT package  = ${str_PANGOXFT_PKG}])
AC_MSG_NOTICE([====   XT_CPPFLAGS       = ${str_XT_CPPFLAGS}])
AC_MSG_NOTICE([====   XT_LIBS           = ${str_XT_LIBS}])
AC_MSG_NOTICE([====   PANGOXFT_CPPFLAGS = ${str_PANGOXFT_CPPFLAGS}])
AC_MSG_NOTICE([====   PANGOXFT_LIBS     = ${str_PANGOXFT_LIBS}])
AC_MSG_NOTICE([====])
AC_MSG_NOTICE([==== yfig-qt Configuration])
AC_MSG_NOTICE([====   yfig-qt           = ${gen_yfig_qt}])
AC_MSG_NOTICE([====   QT version        = ${YFIG_qt_version_major}])
AC_MSG_NOTICE([====])
AC_MSG_NOTICE([==== yfig-gtk Configuration])
AC_MSG_NOTICE([====   yfig-gtk          = ${gen_yfig_gtk}])
if test $gen_yfig_gtk = yes ; then
AC_MSG_NOTICE([====   GTK version       = ${GTK3_VERS}])
fi
AC_MSG_NOTICE([====])
AC_MSG_NOTICE([==== yfig-mingw Configuration])
AC_MSG_NOTICE([====   yfig-mingw          = ${gen_yfig_mingw}])
#if test $gen_yfig_mingw = yes ; then
#AC_MSG_NOTICE([====   MinGw version       = ${MINGW_VERS}])
#fi
AC_MSG_NOTICE([====])

AC_MSG_NOTICE([==== Options shared by all YFIG flavors])
str="temp:$enable_dbg_temp"
str="$str trace:$enable_dbg_trace"
str="$str mem:$enable_dbg_mem"
str="$str dev:$enable_dbg_dev"
AC_MSG_NOTICE([==== - debugging: $str])
test $YFIG_memlimit -eq 0 && str="disabled"
test $YFIG_memlimit -ne 0 && str="enabled to $YFIG_memlimit Mb"
AC_MSG_NOTICE([==== - limitation of the memory amount: $str])
if  test $LIBPNG = 1 -a $LIBJPEG = 1 ; then
    str_png="YES (direct support using libpng library)  $PNG_LIBS"
    str_jpg="YES (direct support using libjpeg library) $JPEG_LIBS"
    str_oth="yes (converted into jpeg using 'convert')"
elif test $LIBPNG = 1 -a $LIBJPEG = 0 ; then
    str_png="YES (direct support using libpng library) $PNG_LIBS"
    str_jpg="yes (converted into png using 'convert')"
    str_oth="yes (converted into png using 'convert')"
elif test $LIBPNG = 0 -a $LIBJPEG = 1 ; then
    str_png="yes (converted into jpg using 'convert')"
    str_jpg="YES (direct support using libjpeg library) $JPEG_LIBS"
    str_oth="yes (converted into png using 'convert')"
else
    str_png="NO" 
    str_jpg="NO" 
    str_oth="NO" 
fi
AC_MSG_NOTICE([==== - Image format of PDF output driver])
AC_MSG_NOTICE([====      PNG        = ${str_png}])
AC_MSG_NOTICE([====      JPEG       = ${str_jpg}])
AC_MSG_NOTICE([====      others     = ${str_oth}])

dnl # ########################################################################
dnl # ## set/clean default compilation flags.
dnl CPPFLAGS="-I../include $CPPFLAGS"
dnl CPPFLAGS="$(echo $CPPFLAGS | sed -e 's/-g//' -e 's/-O2//')"
dnl CFLAGS="$(echo $CFLAGS | sed -e 's/-g//' -e 's/-O2//')"
dnl CXXFLAGS="$(echo $CXXFLAGS | sed -e 's/-g//' -e 's/-O2//')"
dnl # ########################################################################

