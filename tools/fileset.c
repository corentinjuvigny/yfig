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
 *      $Id: fileset.c 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/fileset.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Interface to the glob feature.                                     =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/file.h"

// Note that the configuration (./configure and ...) excludes this
// file from TOOL library if:
//   glob_t is not defined
//   glob and globfree do not exist

#if defined(HAVE_GLOB_H) && HAVE_GLOB_H
#include <glob.h>
#endif
// add the declarations of glob and globfree if needed 
#if !defined(HAVE_DECL_GLOB) || !HAVE_DECL_GLOB
extern int glob(const char *pattern, int flags,
                int (*errfunc) (const char *epath, int eerrno),
                glob_t *pglob);
#endif
#if !defined(HAVE_DECL_GLOBFREE) || !HAVE_DECL_GLOBFREE
extern void globfree(glob_t *pglob);
#endif

/*======================================================================*/

extern TastrSet* tool_fileEleOfDirD(Tmsgdrv*md, CstrBrief dp, int wh, int wpp)
{
    int i;
    glob_t globbuf;
    int    flag;
    TstrEaten cwd_save = tool_getcwd_eated();
    TastrSet* ret = 0;
    
    if ( tool_chdir0(dp)<0 ) {
        tool_errerr_printf(md,"can not change to \"%s\" directory: %s\n",dp,strerror(errno));
        goto end_free;
    }

    globbuf.gl_offs = 0;
    flag=0;
    if ( wh ) flag |= GLOB_PERIOD;
    glob("*",flag, NULL, &globbuf);

    ret = astrset_new(10);
    astrset_add(ret,"..");
    for (i=0 ; i<globbuf.gl_pathc ; i+=1) {
        char* fn=tool_aprintf("%s/%s",dp,globbuf.gl_pathv[i]);
        struct stat info;
        if ( stat(fn,&info)==0 && S_ISDIR(info.st_mode) )
            astrset_add(ret,globbuf.gl_pathv[i]);
        free(fn);
    }
    globfree(&globbuf);

#if 0
    for (i=0 ; i<ret->eleNb ; i+=1) {
         printf("%2d/%2d: %s\n",i,ret->eleNb,ret->eles[i]);
    }
#endif

    tool_chdir0(cwd_save);
end_free:
    free(cwd_save);
    return ret;
}

extern TastrSet* tool_fileEleOfDirF(Tmsgdrv*md, CstrBrief dp, CstrBrief re, int wh)
{
    int i;
    glob_t    globbuf;
    int       flag;
    TstrEaten cwd_save = tool_getcwd_eated();
    TastrSet* ret      = 0;
    
    if ( tool_chdir0(dp)<0 ) {
        tool_errerr_printf(md,"can not change to \"%s\" directory: %s\n",dp,strerror(errno));
        goto end_free;
    }

    globbuf.gl_offs = 0;
//    globbuf.gl_readdir = fnset_new_readdir_skipDir;
    flag = 0;
    if ( wh ) flag |= GLOB_PERIOD;
//    flag |= GLOB_ALTDIRFUNC;
    if ( TOOL_strIsEmpty(re) ) re = "*";
    glob(re,flag, NULL, &globbuf);

    ret = astrset_new(10);
    for (i=0 ; i<globbuf.gl_pathc ; i+=1) {
        char* fn=tool_aprintf("%s/%s",dp,globbuf.gl_pathv[i]);
        struct stat info;
        if ( stat(fn,&info)==0 && S_ISREG(info.st_mode) )
            astrset_add(ret,globbuf.gl_pathv[i]);
        free(fn);
    }
    globfree(&globbuf);

#if 0
    for (i=0 ; i<ret->eleNb ; i+=1) {
         printf("%2d/%2d: %s\n",i,ret->eleNb,ret->eles[i]);
    }
#endif

    tool_chdir0(cwd_save);
end_free:
    free(cwd_save);
    return ret;
}

/*======================================================================*/
