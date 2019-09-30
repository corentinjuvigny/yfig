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
 *      $Id: charset-table.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset-table.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Table types for charset conversions.                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_CHARSET_TABLE_H
#define FILE_TOOLS_CHARSET_TABLE_H
//#include "CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"

/*======================================================================*/

#ifndef TOOL_TuintNN
#define TOOL_TuintNN
typedef unsigned char Tuint8;
typedef const Tuint8  Cuint8;
typedef unsigned int  Tuint32;
typedef const Tuint32 Cuint32;
#endif

#define CS_UNICODE(c)     ((c)&0xFFFFF)
#define CS_FLAG(c)        (((c)>>20)&0xFFF)
#define CS_UCODE_SET(c,f) (((c)&0xFFFFF)|(((f)<<20)&0xFFF00000))
#define CS_UCODE_IS(c,f)  ((c)&(f<<20))

#define CS_FL_ZapfDingbats 1
#define CS_FL_ZapfDingbatsSet(uc)  CS_UCODE_SET(uc,CS_FL_ZapfDingbats)
#define CS_FL_ZapfDingbatsClr(uc)  CS_UCODE_CLR(uc,CS_FL_ZapfDingbats)
#define CS_FL_ZapfDingbatsIs(uc)   CS_UCODE_IS( uc,CS_FL_ZapfDingbats)

typedef struct _TcodeEle   TcodeEle;
typedef const   TcodeEle   CcodeEle;
typedef struct _TcodeTable TcodeTable;
typedef const   TcodeTable CcodeTable;

/*======================================================================*/

struct _TcodeEle {
    Tuint32   ucode;    // CS_UNICODE(ucode) is the unicode
                        // CS_FLAG(ucode) is an associated flag
    Tuint8    zcode;    // 0 or code in ZapfDingbats encoding
    char* utf8;
    char* htmlname;
    char* pdfglyph;
    int   npsglyph;
    char* psglyphs; // if npsglyph>1 must be cast in char** 
    char* comment;
};

struct _TcodeTable {
    int mincode;  // codes start at mincode (included)
    int maxcode;  // codes end   at maxcode (excluded)
    TcodeEle** eles; // maxcode-mincode elements
                     // code of eles[i] is i+mincode
};

/*======================================================================*/

extern TcodeTable* gl_unicodes;
extern TcodeTable* gl_zapfDingBats;

/*======================================================================*/
/*======================================================================*/
//TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_CHARSET_TABLE_H
/*======================================================================*/

