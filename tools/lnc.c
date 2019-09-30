/*======================================================================
 *  Copyright 2017 Ivan Augé
 *
 *  This file is part of the YFIG software.
 * 
 *  YFIG is free  software;  you  can redistribute it and/or modify it
 *  under the terms of the GNU  General Public License as published by
 *  the  Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  YFIG software is distributed  in the  hope that it will be useful,
 *  but WITHOUT ANY WARRANTY;  without even the  implied  warranty  of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  the GNU
 *  General Public License for more details.
 * 
 *  You should have received  a copy of the GNU General Public License
 *  along with YFIG. If not, see <http://www.gnu.org/licenses/>.
**======================================================================
 * $Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: lnc.c 57 2017-03-22 19:32:14Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/lnc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= printf utilities that work with LC_NUMERIC set to "C".             =*/
/*=                                                                    =*/
/*= See the lnc.h header for the function description and the          =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/lnc.h"

#if defined(HAVE_LOCALE_H) && HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(HAVE_DECL_SETLOCALE) && !HAVE_DECL_SETLOCALE
extern char *setlocale(int category, const char *locale);
#endif

/*======================================================================*/

extern int tool_LNC_printf(FILE*stream, const char*fmt, ...)
{
    va_list ap;
    int     ret;

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    char * currentlocale = setlocale(LC_NUMERIC, "C");
    if ( currentlocale!=0 ) currentlocale=strdup(currentlocale);
#endif

    va_start(ap,fmt);
    ret = vfprintf(stream,fmt,ap);
    va_end(ap);

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        free(currentlocale);
    }
#endif
    return ret;
}

extern int tool_LNC_sprintf(char*buf,   const char*fmt, ...)
{
    va_list ap;
    int     ret;

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    char * currentlocale = setlocale(LC_NUMERIC, "C");
    if ( currentlocale!=0 ) currentlocale=strdup(currentlocale);
#endif

    va_start(ap,fmt);
    ret = vsprintf(buf,fmt,ap);
    va_end(ap);

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        free(currentlocale);
    }
#endif
    return ret;
}

/*======================================================================*/
