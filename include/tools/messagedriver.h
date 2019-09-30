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
 *      $Id: messagedriver.h 127 2018-04-14 10:30:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/messagedriver.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for printing messages dedicated to final user.           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_MESSAGEDRIVER_H
#define FILE_TOOLS_MESSAGEDRIVER_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
/*= Data structures                                                    =*/

struct _Tmsgdrv {
    char usridt; // 0: do not print user interactive data
    char usrmsg; // 0: do not print user message

    char addapp; // 0: do not add application name to error message
    char addlvl; // 0: do not add level name to error message
    int  errlvl; // 0: print only fatal messages
                 // 1: print fatal and error messages
                 // 2: print fatal, error and warning messages
                 // 3: print all the messages

    // The function that prints user message & interactive data.
    // The tool_usridt_[v]printf functions invoke it if usridt is not zero.
    // The tool_usrmsg_[v]printf functions invoke it if usrmsg is not zero.
    // When it is invoked by the tool_usrmsg_[v]printf functions, the
    // last character of the msg string is never '\n'.
    void (*usrmsg_print) (Cmsgdrv*drv,CstrBrief msg);

    // The function that prints error message
    // tool_[inf|wrn|err|ftl]msg_[v]printf invokes it if
    //   - errlvl >= lvl
    //   - and it is not NULL.
    // When it is invoked the msg string may already contains
    // the application name and the level name depending of
    // the addapp and addlvl fields.
    // Furthermore the last character of the msg string is never '\n'.
    void (*errmsg_print) (Cmsgdrv*drv,CstrBrief msg, int lvl);

    // function that is called after the printing of a fatal error.
    // if it is set to the null pointer then the exit(int) function
    // is called.
    void (*exit)         (Tmsgdrv*drv,int n);

    Tany     userdata;  // not handled by the driver
    Tmsgdrv* next;      // next driver if the messages must be printed
                        // into various place (eg: terminal, log, window,...)
};

/*======================================================================*/
/*= default printing functions                                         =*/

/**
 * The tool_usrmsg_default prints the msg string into the stdout stream.
 *
 * The tool_errmsg_default prints the msg string into the stderr stream.
**/
extern void tool_usrmsg_default(Cmsgdrv*drv, CstrBrief msg);
extern void tool_errmsg_default(Cmsgdrv*drv, CstrBrief msg, int lvl);

/*======================================================================*/
/*= Interface for printing messages                                    =*/
/*= see base-types.h                                                   =*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_MESSAGEDRIVER_H
/*======================================================================*/
