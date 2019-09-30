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
 *      $Id: globalvars.h 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/globalvars.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some global variables about the running process.                   =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_GLOBALVARS_H
#define FILE_TOOLS_GLOBALVARS_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/

extern Cstr  gl_absname;    // the absolute path of the running program.
extern Cstr  gl_progname;   // the basename of executable
extern Cstr  gl_homeDir;    // the home directory of user.
extern Cstr  gl_startDir;   // the current working directory, when
                            // the program started (absolute path).
extern Cstr  gl_currDir;    // the current working directory, it is
                            // updated by tool_chdir (absolute path).
extern Cstr  gl_tempDir;    // the directory where tempory files are created.
extern Cstr  gl_appRcDir;   // the RC directory of application.

extern Tmsgdrv gl_msgdrv_default; // print user message on stdout,
                                  //      error messages on stderr.
extern Tmsgdrv gl_msgdrv_silence; // no messages except fatal ones on stderr

/*======================================================================*/

extern void tool_glvars_init(const CstrBrief*argv, CstrBrief appRcBn);
extern void tool_glvars_free();
extern void tool_glvars_dump();
extern void tool_glvars_dumpItem(CstrBrief label, CstrBrief fmt, ...);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_GLOBALVARS_H
/*======================================================================*/
