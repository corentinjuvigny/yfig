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
 *      $Id: lnc.h 63 2017-04-22 08:03:26Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/lnc.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= printf utilities that work with LC_NUMERIC set to "C".             =*/
/*=                                                                    =*/
/*= All these functions work as following:                             =*/
/*=   1) turn LC_NUMERIC locale to "C".                                =*/
/*=   2) do the print.                                                 =*/
/*=   3) restore the original value to LC_NUMERIC.                     =*/
/*=                                                                    =*/
/*= If locale feature is not available the functions just perform      =*/
/*= the printf.                                                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_LNC_H
#define FILE_TOOLS_LNC_H
/*======================================================================*/

/*======================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*======================================================================*/

extern int tool_LNC_printf(FILE*stream, const char*fmt, ...);
extern int tool_LNC_sprintf(char*buf,   const char*fmt, ...);

/*======================================================================*/
#ifdef __cplusplus
}
#endif
/*======================================================================*/
#endif // FILE_TOOLS_LNC_H
/*======================================================================*/
