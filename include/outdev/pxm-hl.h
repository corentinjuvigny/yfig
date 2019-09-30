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
 *      $Id: pxm-hl.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/pxm-hl.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling pixmap.                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_PXM_HL_H
#define FILE_OUTDEV_PXM_HL_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/bitmap-types.h"

/*======================================================================*/
/*= Pixmap.                                                            =*/

/**
 * The od_pxm_new function creates a pixmap of width times heigth pixels.
 *
 * The od_pxm_createFromBtm function creates a pixmap from the btm bitmap,
 * coloring the 1 pixels with the fgc color and the 0 pixels with the
 * bgc color.
 *
 * The od_pxm_delete function deletes the pxm pixmap by freeing all
 * the memory that it allocated.
 *
 * Return:
 *   The od_pxm_new and od_pxm_createFromBtm functions returns the
 *   created pixmap.
**/

TpixmapEGS od_bpi_pxm_new        (Toutdev*od, int width, int height);
TpixmapEGS od_bpi_pxm_newFromBtm (Toutdev*od, Cbitmap*btm,Ccolor*fgc,Ccolor*bgc);
TpixmapEGS od_bpi_pxm_newFromImg (Toutdev*od, int width, int height, TimageEGS*img);
void       od_bpi_pxm_delete     (Toutdev*od, TpixmapEGS pxm);
void       od_bpi_pxm_draw       (Toutdev*od, Tod_GCdef gcd, double x, double y,
                                  TpixmapEGS pxm, int xp, int yp, int dxp, int dyp);


#define od_pxm_new            od_bpi_pxm_new
#define od_pxm_delete         od_bpi_pxm_delete
#define od_pxm_createFromData od_bpi_pxm_newFromData

/*======================================================================*/
/*= Image.                                                             =*/

extern void      od_bpi_img_free0    (Toutdev*od, TimageEGS*img );
extern void      od_bpi_img_free     (Toutdev*od, Timage*   img );
extern CstrBrief od_bpi_img_path     (Toutdev*od, int idx);

/**
 * The od_bpi_draw_imgXYHW function draws the img image in the od output
 * device.
 *  - it is drawn into the rectangle a diagonal of which is the
 *    line segment from (x,y) to (x+dx,y+dy).
 *  - if the od rectangle is smaller or larger at pixel level than the
 *    image, the image is scaled to fit the od rectangle.
 *  - if img->img_op defines operations (flip and/or rotations), they
 *    are done before printing.
 *  - if problem occurs that stops the image drawing (no EGS driver,
 *    the image file does not exist, ...) a simple box is drawing that
 *    contatins the image file name when it is possible.
 *
 * Parameters:
 *  x,y  : The position (x,y) of image in the outdev device in source unit.
 *  dx,dy: The size (dx x dy) of image in the outdev device in source unit.
 *  img  : The image to drawn.
**/
extern void od_bpi_img_drawXYHW(Toutdev*od, Tod_GCdef gcd, Timage* img,
                                double x, double y, double dx, double dy);

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/


/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_PXM_HL_H
/*======================================================================*/
