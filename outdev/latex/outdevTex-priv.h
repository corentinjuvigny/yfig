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
 *      $Id: outdevTex-priv.h 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/latex/outdevTex-priv.h $
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

#define TEX_MESS(mess) "TEX handler: " mess

extern Toutdev_vt tex_vtable;

/*======================================================================*/
/*= TEX output device type                                             =*/

struct _TodTex {
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
/*= Miscellaneous utilities (outdefTex-priv.c)                         =*/

extern char*     odtex_gc_getColor(char*buffer, TodTex*od, Ccolor*clr);
extern TstrEaten odtex_gc_get(char*clr,TodTex*od,Tod_GCdef gcd);

extern void odtex_str_print_std (Tmsgdrv*md, FILE* stream, CstrBrief text);
extern void odtex_str_print_ding(Tmsgdrv*md, FILE* stream, CstrBrief text);

/*======================================================================*/
