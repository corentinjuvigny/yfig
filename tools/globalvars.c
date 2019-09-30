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
 *      $Id: globalvars.c 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/globalvars.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some global variables about the running process.                   =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/globalvars.h"
#include "tools/messagedriver.h"

/*======================================================================*/

const char* gl_absname;    // the absolute path of the running program.
const char* gl_progname;   // the basename of executable
const char* gl_homeDir;    // the home directory of user.
const char* gl_startDir;   // the current working directory, when
                           // the program started (absolute path).
const char* gl_currDir;    // the current working directory, updated
                           // by tool_chdir
const char* gl_tempDir;    // the directory where tempory files are created.
const char* gl_appRcDir;   // the RC directory of application.

Tmsgdrv gl_msgdrv_default = {
    // print user message on stdout,
    //       error messages on stderr.
    .usridt = 0,
    .usrmsg = 1,
    .addapp = 1,
    .addlvl = 1,
    .errlvl = 3,
    .usrmsg_print = tool_usrmsg_default,
    .errmsg_print = tool_errmsg_default,
    .exit   = 0,
};

Tmsgdrv gl_msgdrv_silence = {
    // no messages except fatal ones on stderr
    .usridt = 0,
    .usrmsg = 0,
    .addapp = 1,
    .addlvl = 1,
    .errlvl = 0,
    .usrmsg_print = 0,
    .errmsg_print = tool_errmsg_default,
    .exit   = 0,
};

/*======================================================================*/

extern void tool_glvars_init(const CstrBrief*argv, CstrBrief appRcBn)
{
    Cstr rawprgname = argv[0];
    char* str;

    // gl_progname
    // first for enabling the tool message handler
    str= strdup(rawprgname);
    gl_progname = strdup(basename(str));
    free(str);

    // gl_homeDir
    str =  getenv("HOME");
    if ( str!=0 ) 
        str = tool_strdup( str );
    if ( str==0 ) {
        str = "/tmp";
        tool_errerr_printf(NULL,"No HOME directory, It is set to \"%s\" directory",str);
    }
    gl_homeDir = str;

    // gl_startDir & gl_currDir
    char  cwd[PATH_MAX];
    if ( getcwd(cwd,sizeof(cwd))==0 ) {
        tool_errerr_printf(NULL,"too long current working directory,\n");
        tool_ftlerr_printf(NULL,"must not exceed %d characters.\n",PATH_MAX-1);
        exit(1);
    }
    gl_startDir = tool_strdup(cwd);
    gl_currDir  = tool_strdup(cwd);

    // gl_absname
    if ( rawprgname[0]=='/' )
        gl_absname = tool_strdup(rawprgname);
    else {
        Cstr fmt = gl_startDir[strlen(gl_startDir)-1]=='/' ? "%s%s" : "%s/%s";
        gl_absname = tool_aprintf(fmt,gl_startDir,rawprgname);
    }

    // gl_tempDir
    gl_tempDir = tool_strdup("/tmp");

    // gl_appRcDir
    if ( appRcBn==0 ) {
        gl_appRcDir = tool_aprintf("%s/.%s",gl_homeDir,gl_progname);
    } else {
        if ( appRcBn[0]=='/' )
            gl_appRcDir = tool_strdup(appRcBn);
        else
            gl_appRcDir = tool_aprintf("%s/%s",gl_homeDir,appRcBn);
    }
}

extern void tool_glvars_free()
{
    tool_free( (char*)gl_absname  );
    tool_free( (char*)gl_progname );
    tool_free( (char*)gl_homeDir  );
    tool_free( (char*)gl_startDir );
    tool_free( (char*)gl_currDir  );
    tool_free( (char*)gl_tempDir  );
    tool_free( (char*)gl_appRcDir  );
}

/*======================================================================*/

extern void tool_glvars_dumpItem(CstrBrief label, CstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    TstrEaten str = tool_vaprintf(fmt,ap);
    if ( label!=0 )
        tool_inferr_printf(NULL,"%-25s: %s\n",label,str);
    else
        tool_inferr_printf(NULL,"%-25s  %s\n",""   ,str);
    va_end(ap);
}

extern void tool_glvars_dump()
{
    int i;
    tool_glvars_dumpItem("program path","%s",gl_absname);
    tool_glvars_dumpItem("home dir"    ,"%s",gl_homeDir);
    tool_glvars_dumpItem("current WD"  ,"%s",gl_startDir);
    tool_glvars_dumpItem("tempory dir" ,"%s",gl_tempDir);
    tool_glvars_dumpItem("RC dir"      ,"%s",gl_appRcDir);
    // verbosity and debug level
    i = gl_msgdrv_default.errlvl; tool_glvars_dumpItem("mess. level", "%s", i==0 ? "fatal" : i==1 ? "error" : i==2 ? "warning" : "information");
#if defined(TOOL_DBG_Trace) && TOOL_DBG_Trace!=0
    i = tool_dbg_level;           tool_glvars_dumpItem("debug level", "%d (%s)",i,i==0 ? "fully quiet" : i==1 ? "quiet" : i==2 ? "verbose" : "very verbose");
#else
                                  tool_glvars_dumpItem("debug level", "%s", "disabled");
#endif
}

/*======================================================================*/
