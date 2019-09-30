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
 *      $Id: string-builder.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/string-builder.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for making big string by successive appends.             =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_STRING_BUILDER_H
#define FILE_TOOLS_STRING_BUILDER_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "strings.h"

/*======================================================================*/

struct _Tstrfmt {
    TstrEaten  str;          // the string that is in progress
                             // it always end by the footer
    int        bytesize;     // current byte size of str without the footer
                             // so next write starts at str+bytesize
    int        shift;        // lines start with shift space
    int        nbItemByLine; // number of item by line
    int        nbItem;       // number of items in the current line.
    char       foot[100];    // the end sequence
    int        footlen;      // the foot bytesize
};

/*======================================================================*/
/*= Initialization and freeing.                                        =*/

extern void     strfmt_iniLSILF(Tstrfmt*str,
                                int initlen, int shift, int nbItemByLine);
extern Tstrfmt* strfmt_newLSILF(int initlen, int shift, int nbItemByLine);
extern void     strfmt_free  (Tstrfmt*str);
extern void     strfmt_delete(Tstrfmt*str);

extern void     strfmt_restart(Tstrfmt*str);

/*======================================================================*/
/*= building the string.                                               =*/

extern void strfmt_write    (Tstrfmt*str, const void* data, int datalen);
extern void strfmt_writeLF  (Tstrfmt*str);
extern void strfmt_writeItem(Tstrfmt*str, const void* data, int datalen);
extern void strfmt_writeStr    (Tstrfmt*str, CstrBrief data);
extern void strfmt_writeStrItem(Tstrfmt*str, CstrBrief data);
extern void strfmt_printf     (Tstrfmt*str, CstrBrief fmt, ...);
extern void strfmt_printfItem (Tstrfmt*str, CstrBrief fmt, ...);
extern void strfmt_vprintf    (Tstrfmt*str, CstrBrief fmt, va_list ap);
extern void strfmt_vprintfItem(Tstrfmt*str, CstrBrief fmt, va_list ap);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_STRING_BUILDER_H
/*======================================================================*/
