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
 *      $Id: outdevSvg-priv.h 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/svg/outdevSvg-priv.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= TEX output device structure.                                       =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev-drv.h"
#include "outdev/outdevPdf.h"
#include "tools/color.h"
#include "tools/bitmap.h"
#include "tools/string-builder.h"

/*======================================================================*/

// 1 PT in Cm
#define PT1 LUNIT_PtToCm(1.)

#define SVG_MESS(mess) "SVG handler: " mess

extern Toutdev_vt svg_vtable;

/*======================================================================*/
/*= SVG output device type                                             =*/

struct _TodSvg {
    ODDRV_FIELDS();

    double   picDx,picDy;    // source picture size (cm)
    double    ppDx, ppDy;    // dest. paper size (cm)
    double    daDx, daDy;    // drawing area size inside a dest. paper (cm)
    double   border;         // border width on paper aroud the source picture (cm)
                             //   ppDx = daDx + border*2
                             //   ppDy = daDy + border*2
    double   scale;

    TfontInfo strFontInfo;
    Tcolor*   strColor;
};

/*======================================================================*/
/*= Miscellaneous utilities (outdefSvg-priv.c)                         =*/

extern char*     odsvg_getColor(char*buffer, TodSvg*od, Ccolor*clr);

extern TstrEaten odsvg_getStyle0(TodSvg*od,Tod_GCdef gcd);
extern TstrEaten odsvg_getStyle1(TodSvg*od,Tod_GCdef gcd);

/*======================================================================*/
