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
 *$Software: YFIG (YFIG driver to Qt)
 *      $Id: yfigqt-main.cc 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/qt/yfigqt-main.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= main: it just calls YFIG_main.                                     =*/
/*======================================================================*/

#include "yfigqt.h"

extern "C" {

#if !defined(HAVE_BZERO) || HAVE_BZERO == 0
void bzero(void *s, size_t n) { memset(s,0,n); }
#endif

#if !defined(HAVE_INDEX) || HAVE_INDEX == 0
char *index(const char *s, int c)
{
    for ( ; *s!=0 ; s+=1) if ( *s==c) return (char*)s;
    return NULL;
}
#endif

#if TOS_MINGW == 1
int WIFEXITED(int s)
{
    printf("%s:ZZZZZZZZZZZZZZZZZZ:s=%d\n",__func__,s);
    return 1;
}

int WEXITSTATUS(int s) {
    printf("%s:ZZZZZZZZZZZZZZZZZZ:s=%d\n",__func__,s);
    return s;
}
#endif

}

/*======================================================================*/

extern "C" {

extern int YFIG_main(int argc, char**argv);

}

/*======================================================================*/

int main(int argc, char**argv)
{
    return YFIG_main(argc,argv);
}

/*======================================================================*/
