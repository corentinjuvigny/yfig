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
 *      $Id: outdev-vt.h 107 2018-01-07 16:50:33Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/outdev-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Abstract type for translating something into something.            =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_OUTDEV_VT_H
#define FILE_OUTDEV_OUTDEV_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "top-vt.h"
#include "color-vt.h"
#include "pxm-vt.h"
#include "ldaf-vt.h"
#include "string-vt.h"

/*======================================================================*/ 

struct _Toutdev_vt {
    Toutdev_top_vt    topVT;
    Toutdev_color_vt  colorVT;
    Toutdev_pxm_vt    pxmVT;
    Toutdev_ldaf_vt   ldafVT; 
    Toutdev_string_vt strVT; 
};

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_OUTDEV_VT_H
/*======================================================================*/
