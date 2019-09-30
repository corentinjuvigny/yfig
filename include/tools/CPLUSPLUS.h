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
 *      $Id: CPLUSPLUS.h 97 2017-11-11 17:20:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/CPLUSPLUS.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= For compiling in C++ environment.                                  =*/
/*=                                                                    =*/
/*= // to insert at top of header                                      =*/
/*= // it opens an "ectern C" section if needed: ectern "C" {          =*/
/*= #include "tools/CPLUSCPLUS.h"                                       =*/
/*= ...                                                                =*/
/*= ...                                                                =*/
/*= // to insert at bottom of header                                   =*/
/*= // it closes the "ectern C" section if needed: }                   =*/
/*= TOOLS_CPLUSPLUS_CLOSE                                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_CPLUSPLUS_H
#define FILE_TOOLS_CPLUSPLUS_H
/*======================================================================*/
#ifdef __cplusplus
#   define TOOLS_CPLUSPLUS_OPEN  extern "C" {
#   define TOOLS_CPLUSPLUS_CLOSE }
#else
#   define TOOLS_CPLUSPLUS_OPEN
#   define TOOLS_CPLUSPLUS_CLOSE
#endif

/*======================================================================*/
#endif //FILE_VGUI_CPLUSPLUS_H
TOOLS_CPLUSPLUS_OPEN
/*======================================================================*/
