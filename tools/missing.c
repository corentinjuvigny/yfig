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
 *      $Id: missing.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/missing.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Replacement for missing features.                                  =*/
/*======================================================================*/

#include "tools-config.h"

#if !defined(HAVE_BZERO) || HAVE_BZERO == 0
extern void bzero(void *s, size_t n) { memset(s,0,n); }
#endif

#if !defined(HAVE_INDEX) || HAVE_INDEX == 0
extern char *index(const char *s, int c)
{
    for ( ; *s!=0 ; s+=1) if ( *s==c) return (char*)s;
    return NULL;
}
#endif

#if TOS_MINGW == 1
extern int WIFEXITED(int s)
{
    printf("%s:ZZZZZZZZZZZZZZZZZZ:s=%d\n",__func__,s);
    return 1;
}

extern int WEXITSTATUS(int s) {
    printf("%s:ZZZZZZZZZZZZZZZZZZ:s=%d\n",__func__,s);
    return s;
}
#endif

/*======================================================================*/
