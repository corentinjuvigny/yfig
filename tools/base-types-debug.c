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
 *      $Id: base-types-debug.c 363 2018-11-18 22:57:28Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/base-types-debug.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for debuging.                                            =*/
/*= See base-types.h header for documentation.                         =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/base-types.h"
#include "tools/globalvars.h"
#include "tools/strings.h"

/*======================================================================*/
/*= Utilities for printing bug messages.                               =*/

extern void gdb_stop() { }

extern void tool_bug_printf(int stop, CstrBrief level, CstrBrief file,
                            int line, CstrBrief func,  CstrBrief fmt,...)
{
    TstrEaten basename = tool_basename(file);
    TstrEaten header   = tool_aprintf("%-25s:%3d:%25s:%7s",file,line,func,level);
    fprintf(stderr,"%s: ",header);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr,fmt, ap );
    if ( fmt==0 || fmt[strlen(fmt)-1]!='\n' )
        fprintf(stderr,"\n");
    va_end(ap);
    if ( stop==1 ) {
        fprintf(stderr,"%s: can not continue\n",header);
        gdb_stop();
        exit(1);
    }
    tool_free(basename);
    tool_free(header);
}

/*======================================================================*/
/*= Utilities for printing debug messages.                             =*/

int  tool_dbg_level;     // default is 0

extern void tool_dbg_printf(
    CstrBrief tag,   // message is printed if tag==0 or getenv(tag)!=0
    int       level, // message is printed if level < tool_dbg_level
    CstrBrief file,  // the file name
    int       line,  // the line number in file
    CstrBrief func,  // the function name
    CstrBrief fmt,   // the format (same as printf)
    ...)             // the arguments specified by fmt
{
    va_list ap;
    va_start(ap, fmt);
    tool_dbg_vprintf(tag,level,file,line,func,fmt,ap);
    va_end(ap);
}

extern void tool_dbg_vprintf(
    CstrBrief tag,   // message is printed if tag==0 or getenv(tag)!=0
    int       level, // message is printed if level < tool_dbg_level
    CstrBrief file,  // the file name
    int       line,  // the line number in file
    CstrBrief func,  // the function name
    CstrBrief fmt,   // the format (same as printf)
    va_list   ap)    // the arguments specified by fmt
{
    char* stag=0;
    if ( level>=tool_dbg_level )    return;
    if ( tag!=0  && (stag=getenv(tag))==0 ) return;
    if ( stag!=0 && atoi(stag)==0 ) return;
    fprintf( stderr,"%s:%-40s: ",gl_progname,func);
    char tmp[2000];
    int len = vsnprintf(tmp,sizeof(tmp)-2,fmt,ap);
    if ( len>=(sizeof(tmp)-2) ) {
        // truncated
        len=sizeof(tmp)-2;
        tmp[len]=0;
    }
    if ( tmp[len-1]!='\n' ) {
        // no \n, we add it
        tmp[len]='\n'; tmp[len+1]=0;
    }
    fprintf(stderr,"%s",tmp);
}

/*======================================================================*/
