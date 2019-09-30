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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: color-vt.h 107 2018-01-07 16:50:33Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/color-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling colors.                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_COLOR_VT_H
#define FILE_OUTDEV_COLOR_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/color-types.h"

/*======================================================================*/
/*= Interface                                                          =*/
struct _Toutdev_color_vt {
/*======================================================================*/

/**
 * The black function returns the black color.
 * The white function returns the white color.
 *
 * The caller must delete the returned color after use.
 *
 * CAUTION: these functions are mandatory.
**/
Tcolor* (*black)(const void*dev);
Tcolor* (*white)(const void*dev);

/*======================================================================*/
};
/*======================================================================*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_COLOR_VT_H
/*======================================================================*/
