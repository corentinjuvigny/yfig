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
 *$Software: YFIG-XT (VGUI driver to Xt and Xawd or Xaw3d)
 *      $Id: guixt-outdev-vt-pxm.c 111 2018-01-07 18:20:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-outdev-vt-pxm.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= output device implementation (pixmap utilities)                    =*/
/*=                                                                    =*/
/*= This file is included by guixt-outdevice-vt.c.                     =*/
/*======================================================================*/

#include "tools/bitmap.h"

/*======================================================================*/

static TpixmapEGS odvt_pxm_new(void*  dev, int width, int height)
{
    STD_VARS;
    Pixmap pixmap;
    pixmap = XCreatePixmap(E_Display, 
                DefaultRootWindow(E_Display), width, height, E_Depth);
    return X112PXMEGS(pixmap);
}

static TpixmapEGS odvt_pxm_createFromBtm(void*  dev, Cbitmap*btm,
                                           Ccolor*fgc, Ccolor* bgc)
{
    STD_VARS;
    TcolorEGS fg = fgc->egs;
    TcolorEGS bg = bgc->egs;
    Pixmap pixmap;
    pixmap = XCreatePixmapFromBitmapData(
                E_Display, DefaultRootWindow(E_Display),
                (char*)btm->bits->data, btm->dx,btm->dy,
                fg.ulong, bg.ulong, E_Depth);
    return X112PXMEGS(pixmap);
}

static void odvt_pxm_delete(void*dev, TpixmapEGS pxm)
{
    STD_VARS;
    Pixmap pixmap = pxm.ulong;
    if ( pixmap==0 ) return;
    XFreePixmap(E_Display,PXMEGS2X11(pxm));
}

/*======================================================================*/
