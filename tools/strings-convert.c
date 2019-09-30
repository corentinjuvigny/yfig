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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: strings-convert.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/strings-convert.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some string features (conversion)                                  =*/
/*======================================================================*/

#include "tools-config.h"
#include "../include/tools/strings.h"

#if defined(HAVE_LOCALE_H) && HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(HAVE_DECL_SETLOCALE) && !HAVE_DECL_SETLOCALE
extern char *setlocale(int category, const char *locale);
#endif

/*======================================================================*/

extern int tool_strtosint32(Tsint32* res, CstrBrief str)
{
    char* endptr;
    long    val;
    Tsint32 ival;

    errno = 0;
    val = strtol(str, &endptr, 10);
    
    if (str     == endptr) return -1;
    if (errno   != 0     ) return -1;
    if (*endptr != '\0')   return -1;

    ival=val;
    if (ival!=val) return -1;
    
    *res = ival;
    return 0;
}

extern int tool_strtodouble(double *res, CstrBrief str)
{
#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    char * currentlocale = setlocale(LC_NUMERIC, "C");
    currentlocale=tool_strdup(currentlocale);
#endif
    int   ret=0;
    char* endptr;

    errno = 0;
    *res = strtod(str, &endptr);

    if (str     == endptr) { ret = -1; goto end; }
    if (errno   == ERANGE) { ret = -1; goto end; }
    if (*endptr != '\0')   { ret = -1; goto end; }
end:
#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        tool_free(currentlocale);
    }
#endif
    return ret;
}

/*======================================================================*/

static int tool_strtofont_getField(char*tmp, char** str)
{
    Cstr p=*str;
    int len=0;

    while ( *p==' ' || *p=='\t' ) p+=1;
    while ( len<99 && *p!=':' && *p!=0 ) {
        tmp[len++]=*p++;
    }
    if ( len>=99 ) { *str=(char*)p; return -1; }
    tmp[len]=0;
    while ( (len-1)>=0 && (tmp[len-1]==' ' || tmp[len-1]=='\t') ) {
        len -= 1; tmp[len] = 0; 
    }

    *str = (char*)p;
    return len;
}

extern int  tool_strtofont  (char*f, double*s, int*d,     CstrBrief str)
{
    Cstr p=str;
    char tmp[100];
    int len,ret=0;

    // initialization to not found
    f[0]=0; *s=-1; *d=-1;

    // family
    len = tool_strtofont_getField(tmp,(char**)&p);
    if ( len<0  ) return -1;
    if ( len!=0 ) {
        strcpy(f,tmp);
        ret += 1;
    }
    if ( *p==':' ) p+=1;
    if ( *p==0   ) return ret;

    // font size
    len = tool_strtofont_getField(tmp,(char**)&p);
    if ( len<0  ) return -1;
    if ( len!=0 ) {
        if ( tool_strtodouble(s,tmp)<0 ) { *s=-1; return -1; }
        ret += 1;
    }
    if ( *p==':' ) p+=1;
    if ( *p==0   ) return ret;

    // font density
    len = tool_strtofont_getField(tmp,(char**)&p);
    if ( len<0  ) return -1;
    if ( len!=0 ) {
        if ( tool_strtosint32(d,tmp)<0 ) { *d=-1; return -1; }
        ret += 1;
    }

    if ( *p!=0   ) return -1;
    return ret;
}

extern int  tool_strtoX11geo(int*x, int*y, int*dx,int*dy, CstrBrief str);

/*======================================================================*/

extern void      tool_strtoupper(char* str)
{
    for ( ; *str!=0 ; str+=1 )
        *str = toupper((unsigned)*str);
}

extern void      tool_strtolower(char* str)
{
    for ( ; *str!=0 ; str+=1 )
        *str = tolower((unsigned)*str);
}

/*======================================================================*/
