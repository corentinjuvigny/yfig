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
 *      $Id: pxm-vt.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/pxm-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling pixmap.                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_PXM_VT_H
#define FILE_OUTDEV_PXM_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/bitmap-types.h"
#include "tools/color-types.h"

/*======================================================================*/
/*= Interface                                                          =*/
struct _Toutdev_pxm_vt {
/*======================================================================*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Pixmap utilities.                                                  =*/

/**
 * The pxm_createFromBtm function creates a pixmap from the ptm bitmap,
 * coloring the 1 pixels with the fgc color and the 0 pixels with the
 * bgc color.
 *
 * The pxm_delete function deletes the pxm pixmap by freeing all the memory
 * that it uses.
**/
TpixmapEGS (*pxm_new)        (void*dev, int width, int height);
TpixmapEGS (*pxm_newFromBtm) (void*dev, Cbitmap*btm,Ccolor*fgc,Ccolor*bgc);
TpixmapEGS (*pxm_newFromImg) (void*dev, int width, int height, TimageEGS*img);
void       (*pxm_delete)     (void*dev, TpixmapEGS pxm);
void       (*pxm_draw)       (void*dev, Tod_GCdef gcd, double x, double y,
                              TpixmapEGS pxm, int xp, int yp, int dxp, int dyp);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Image utilities.                                                   =*/

/**
 * The img_load function loads an image from the path file.
 * The img_free function frees the memory that the img image allocated.
 *
 * The img_transform function transforms the img image as following
 *  - h true: it flips the image horizontally (x axis usually image width).
 *  - v true: it flips the image vertically (y axis usually image height).
 *  - r  ==1: it rotates the image by +90 degrees ("sens trigonométrique").
 *  - r  ==3: it rotates the image by -90 degrees ("sens trigonométrique").
 * When a rotation is done, the function updates img->dx and img->dy by
 * swapping them.
 *
 * The img_draw function draws the img image at (x,y) position in the
 * srcBox. The image is transformed using the h, v, and r parameters
 * as the img_transform function does (see above).
 *
 * Return:
 *  The img_load function returns a TimageEGS structure fully initialized
 *  (dx,dy,egs) on success, otherwise it returns IMGEGS_none.
 *
 * Notes:
 *  - If the img_draw function exists the outdev API does not use the
 *    img_transform function.
 *  - If the img_transform function exists and the img_draw does not
 *    exists, the outdev API uses the does not use the pxm_newFromImg
 *    function.
**/
int          img_prefFormat; // prefered format 
                             //  -1:none : IMGFMT_xxx:xxx format
TimageEGS  (*img_load)     (void*dev, CstrBrief path, CstrBrief opath);
void       (*img_free)     (void*dev, TimageEGS  img);
void       (*img_transform)(void*dev, TimageEGS* img, int h, int v, int r);
void       (*img_draw)     (void*dev, Tod_GCdef gcd, CboxDble*srcbox,
                            TimageEGS img, int h, int v, int r);

/*======================================================================*/
};
/*======================================================================*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_PXM_VT_H
/*======================================================================*/
