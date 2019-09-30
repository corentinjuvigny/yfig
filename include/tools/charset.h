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
 *      $Id: charset.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/charset.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous charset conversions.                                 =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_CHARSET_H
#define FILE_TOOLS_CHARSET_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/

/**
 * The cs_conv_La15toUtf8 function converts the str string in the Latin-15
 * (ISO-8859-15) charset into a string in the unicode charset under
 * the UTF-8 format.
 * the cs_conv_Utf8toLa15 function does the reverse job of the
 * cs_conv_chg_La15toUtf8 function.
 *
 * Return:
 *  The cs_conv_La15toUtf8 and cs_conv_Utf8toLa15 returns the converted
 *  string. This later is memory allocated, so the caller must free it
 *  after use.
**/
extern TstrEaten cs_conv_La15toUtf8(CstrBrief str);
extern TstrEaten cs_conv_Utf8toLa15(CstrBrief str);

/**
 * The cs_conv_La15toUtf8_chg function converts the *str string
 * in Latin-15 (ISO-8859-15) charset into unicode charset under UTF-8 format.
 * The original latin-15 *str string is freed and the new one UTF-8 is
 * malloced and returned in the *str variable.
 * The cs_conv_Utf8toLa15_chg function does the reverse job of the
 * cs_conv_chg_La15toUtf8_chg function.
**/
extern void cs_conv_La15toUtf8_chg(char**str);
extern void cs_conv_Utf8toLa15_chg(char**str);

/**
 * The cs_conv_Cp1252toUtf8 and cs_conv_Utf8toCp1252 functions are similar
 * to their equivalent "La15" but do the translation to/from the Windows
 * Code Page 1252 (Windows ANSI) to/from UTF-8.
**/
extern TstrEaten cs_conv_Cp1252toUtf8(CstrBrief str);
extern TstrEaten cs_conv_Utf8toCp1252(CstrBrief str);

/**
 * The cs_conv_Zdb8toUtf8 and cs_conv_Utf8toZdb8 functions are similar
 * to their equivalent "La15" but do the translation to/from the
 * ZapfDingBats charset to/from UTF-8.
**/
extern TstrEaten cs_conv_Zdb8toUtf8(CstrBrief str);
extern TstrEaten cs_conv_Utf8toZdb8(CstrBrief str);

/**
 * The cs_conv_Utf32toUtf8 and cs_conv_Utf8toUtf32 functions perform
 * the conversion from/to the UTF-8 format from/to the UTF-32 format.
 * Notice that they do not change the charset.
 *
 * The returned array must be freed by the caller after use.
 * The returned array are always null terminated.
 *  Utf32toUtf8( {5,6,7}    , 3) --> { 5, 6, 7, 0)
 *  Utf32toUtf8( {5,6,7,0 } , 4) --> { 5, 6, 7, 0)
 * The len returned by Utf8toUtf32 takes in account the tailing null.
 *  Utf8toUtf32( {5,6,7}    , 3) --> { 5, 6, 7, 0) and *len=4
 *  Utf8toUtf32( {5,6,7,0}  , 4) --> { 5, 6, 7, 0) and *len=4
**/
extern TstrEaten cs_conv_Utf32toUtf8(Cuint32*  str, int  len);
extern Tuint32*  cs_conv_Utf8toUtf32(CstrBrief str, int* len);

/*======================================================================*/

/**
 * The cs_char_length function creturns the number of printable
 * chararcters of the text string.
 * 
 * It works as following:
 *  - an ASCII chararcter (c<128) counts 1.
 *  - a  ISO   chararcter (160<=c<256) counts 1.
 *  - a  UTF-8 chararcter (110xxxxx 10xxxxxx or 1110xxxx 10xxxxxx 10xxxxxx
 *    or 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx) counts 1.
 *  - others char are ignored.
 *
 * BUG: the function does not take into account the ligatures (ff) and
 * character composition ('A) that UTF-8 defines.
**/
extern int cs_char_length(CstrBrief text);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_CHARSET_H
/*======================================================================*/


