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
 *      $Id: strings-filename.c 336 2018-11-04 16:24:44Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/strings-filename.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some string features (about filename)                              =*/
/*======================================================================*/

#include "tools-config.h"
#include "../include/tools/strings.h"

/*======================================================================*/
extern int tool_renamefile (CstrBrief file)
{
    char bak_name[PATH_MAX];

    strcpy(bak_name,file);
    strcat(bak_name,".bak");
    if (rename(file,bak_name) < 0)
      return (-1);
    return 0;
}

extern TstrEaten tool_basename   (CstrBrief p)
{
    TstrEaten dup = strdup(p?p:"");
    TstrEaten ret = strdup( basename(dup) );
    free(dup);
    return ret;
}

extern TstrEaten tool_basenameNoExt(CstrBrief p)
{
    if (p==NULL) return NULL;
    TstrEaten bn = tool_basename( p );
    // special cases
    if ( strcmp(bn,".")==0 )  return bn;
    if ( strcmp(bn,"..")==0 ) return bn;
    // do the job
    char* dot=strrchr(bn,'.');
    if ( dot==0 ) return bn;
    if ( dot[1]==0 ) return bn;
    *dot = 0;
    TstrEaten ret = tool_strdup( bn );
    tool_free( bn );
    return ret;
}

extern TstrEaten tool_basenameExt  (CstrBrief p)
{
    if (p==NULL) return NULL;
    TstrEaten bn = tool_basename( p );
    // special cases
    if ( strcmp(bn,".")==0 )  { tool_free(bn); return NULL; }
    if ( strcmp(bn,"..")==0 ) { tool_free(bn); return NULL; }
    // do the job
    char* dot=strrchr(bn,'.');
    if ( dot==0 )    { tool_free(bn); return NULL; }
    if ( dot[1]==0 ) { tool_free(bn); return NULL; }
    TstrEaten ret = tool_strdup( dot+1 );
    tool_free( bn );
    return ret;
}

extern TstrEaten tool_dirname    (CstrBrief p)
{
    TstrEaten dup = strdup(p?p:"");
    TstrEaten ret = strdup( dirname(dup) );
    free(dup);
    return ret;
}

extern void tool_dirbasename(CstrBrief p, TstrEaten* dn, TstrEaten* bn)
{
    *dn = tool_dirname(p);
    *bn = tool_basename(p);
}

extern TstrEaten tool_chgPathExt(CstrBrief p, CstrBrief ext)
{
    char *ret,*dirname,*basname;
    dirname = tool_dirname( p );
    basname = tool_basenameNoExt( p );
    ret     = tool_aprintf("%s/%s.%s",dirname,basname,ext);
    tool_free(dirname); tool_free(basname);
    return ret;
}

/*======================================================================*/
