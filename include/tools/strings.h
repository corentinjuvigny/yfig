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
 *      $Id: strings.h 342 2018-11-08 00:14:55Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/strings.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some string features                                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_STRINGS_H
#define FILE_TOOLS_STRINGS_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

typedef struct _Tstrfmt Tstrfmt;
typedef const   Tstrfmt Cstrfmt;

/**
 * The tool_strtosint32 function converts the str string to integer into res.
 * The tool_strtodouble function converts the str string to double into res.
 *
 * Return:
 *   0: success (str was a valid number and nothing else)
 *  -1: failure:
 *        - str does not start with a digit or a signe
 *        - too big or too small (infinity or to near zero)
 *        - str has one or more trailing characters after the number
**/
extern int  tool_strtosint32(Tsint32* res, CstrBrief str);
extern int  tool_strtodouble(double*  res, CstrBrief str);

/**
 * The tool_strtofont function extracts from the str string the font
 * family, the font size and the font density and writes them into
 * the f, s and d parameters. The str format being:
 *   family:size:density
 * The fields are optional, size is in range [1:100] and density is
 * in range [0:100]. The next strings are valid: times, times::,
 *  times:12, times:12:80, ::80.
 * If the density is omitted in the str string f is set to the empty
 * string (f[0] is nulla character).
 * If the size or the density are omitted s and d are respectively
 * set to -1.
 *
 * The tool_strtoX11geo function extracts from the str string in
 * the standard X11 geometry format the position, the width and the
 * height and writes them respectively into the x, y, dx and dy
 * parameters. The X11 geometry format is:
 *   WxH+X+Y
 * If a value is omitted, it is set to -1.
 *
 * Return:
 *   - On succes, the tool_strtofont function returns the number
 *     of items found in the str string (in range [0:3]), otherwise
 *     it returns -1 (str is bad formated).
 *   - On succes, the tool_strtoX11geo function returns:
 *       1: the position is defined.
 *       2: the size is defined.
 *       3: both the position and size are defined.
 *     Otherwise it returns -1 (str is bad formated).
**/
extern int  tool_strtofont  (char*f, double*s, int*d,     CstrBrief str);
extern int  tool_strtoX11geo(int*x, int*y, int*dx,int*dy, CstrBrief str);

extern void tool_strtoupper(char* str);
extern void tool_strtolower(char* str);

extern TstrEaten tool_basename     (CstrBrief p);
extern TstrEaten tool_basenameNoExt(CstrBrief p);
extern TstrEaten tool_basenameExt  (CstrBrief p);
extern TstrEaten tool_dirname      (CstrBrief p);
extern void      tool_dirbasename  (CstrBrief p, TstrEaten* dn, TstrEaten* bn);
extern TstrEaten tool_chgPathExt(CstrBrief p, CstrBrief ext);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_STRINGS_H
/*======================================================================*/
