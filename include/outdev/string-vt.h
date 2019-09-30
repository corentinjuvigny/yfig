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
 *      $Id: string-vt.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/string-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing strings.                                     =*/
/*=                                                                    =*/
/*= This API allows:                                                   =*/
/*=   - to build new string GC                                         =*/
/*=   - to change string GC parameters                                 =*/
/*=   - to draw strings.                                               =*/
/*=   - area filling.                                                  =*/
/*=                                                                    =*/
/*= Notes:                                                             =*/
/*=  - GC means Graphic Context.                                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_STRING_VT_H
#define FILE_OUTDEV_STRING_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/boxes.h"
#include "tools/color-types.h"
#include "tools/font.h"
#include "outdev/types.h"

/*======================================================================*/
/*= Interface                                                          =*/
struct _Toutdev_string_vt {
/*======================================================================*/

/*======================================================================*/
/*= initialization features                                            =*/

/**
 * The initGC function initializes the gcd th GC for drawing text with
 * the f font, the fg foreground color and the bg background color.
 * It must be performed once at init time for the GC that may be used
 * for drawing texts.
**/
void (*initGC)(void*dev, Tod_GCdef gcd, TfontEGS f, Ccolor* fg, Ccolor* bg);

/**
 * The fontLoadPts function loads at the EGS level, the fi th font
 * sized to pts points.
 * The fontLoadPix function loads at the EGS level, the fi th font
 * sized to pts pixel of the dev output device.
 *
 * The fontUnload function unloads the f font at the EGS level.
 *
 * Return:
 *  - On success, the fontLoadPts/Pix functions returns the EGS identifier
 *    of the asked font, otherwise it returns  EGSFONT_NONE;
 *    The main failure reasons are:  the font does not exist, the font
 *    is not found, a problem occurs when loading it.
**/

TfontEGS (*fontLoadPts) (void*dev, int fi, double pts, double*ratio);
TfontEGS (*fontLoadPix) (void*dev, int fi, int    pts, double*ratio);
void     (*fontUnload)  (void*dev, TfontEGS f);

/**
 * The setGC function sets the font and the drawing color of the gcd
 * graphic context.
 * The background color for string drawings is not modified.
**/
void (*setGC)(void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg);

/*======================================================================*/
/*= Drawing strings (extents)                                          =*/

/**
 * The extentsPts function does not draw anything.
 * It just returns the geometric parameters of the text string
 * drawn using the f font.
 *  - ascent is the distance between the top border and the base line.
 *  - descent is the distance between the bottom border and the base line.
 *  - dx is the width of the text.
 *  - dy is the height of the text.
 * All the returned distance are given in point (1/72 Inch).
 *
 * You can set the ascent, descent, dx and dy pointers to the null
 * pointer when you do not need the corresponding values.
 *
 * The extentsPix function is similar to the 
 * extentsPix functions except that it returns the geometric
 * parameters in the output device unit (pixel).
**/
void (*extentsPts)(void* dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent,
                   double* dx,     double* dy);
void (*extentsPix)(void* dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent,
                   double* dx,     double* dy);

/**
 * The lineInfoPix function returns for the font defined
 * in the gcd th graphic context:
 *   - into *idy the max inked height of the characters (where pixels
 *     can be inked),
 *   - into *ldy the max logic height of the characters,
 *   - into *ils the inter-line space between lines.
 * The relation between these dimension is: ldy=idy+ils.
 *
 * The returned dimension are given in pixel.
 *
 * Parameters:
 *  - idy: where the inked height is returned or NULL.
 *  - ldy: where the logic height is returned or NULL.
 *  - ils: where the inter line space is returned or NULL.
**/
void (*lineInfoPix)(void* dev, Tod_GCdef gcd, double*idy, double*ldy, double*lskip);

/*======================================================================*/
/*= Drawing strings                                                    =*/

/**
 * The drawWL_Pix function draws the text string into the dev output device
 * using the font and the color specified by the gcd th graphic context.
 * The text is drawn from left to right starting at position (x,y) where
 * y is the base line of the text.
 * WL means "West base Line".  
 * 
 * The drawNW_Pix function is similar to the drawWL function except that
 * the (x,y) position gives the North West position of the text bounding
 * box.
 *
 * The drawWLr_Pix function draws the text string into the dev output
 * device using the font and the color specified by the gcd th graphic
 * context.
 * The text is drawn from left to right starting at position (x,y) where
 * y is the base line of the text. Then the text is rotate from a radian,
 * the center of rotation being (x,y).
 *
 * The output device API provides a default function that draws n X
 * where n is the number of drawable characters of the text string.
 *
 * The next table inicates how the output device API chooses the
 * low level drawing function. 
 *    +--------+--------+---------++---------+---------+
 *    | drawWL | drawNW | drawWLr ||  a==0   |  a!=0   |
 *    +--------+--------+---------++---------+---------+
 *    |   1    |   *    |   0     || drawWL  | default |
 *    |   1    |   *    |   1     || drawWL  | drawWLr |
 *    |   0    |   1    |   0     || drawNW  | default |
 *    |   0    |   1    |   1     || drawNW  | drawWLr |
 *    |   0    |   0    |   0     || default | default |
 *    +--------+--------+---------++---------+---------+
 *
 * Parameters:
 *  - dev : the output device (Toudev* or sub classes).
 *  - x,y : the position where string is drawn in the output device unit.
 *  - text: the text to draw. It must be neither the  NULL pointer nor
 *          the empty string ("").
 *  - a   : rotation angle in radian.
 *            +-----------------------------> x
 *            |         / a=-45 degree
 *            |        /  
 *            | (x,y) X--------------- a=0 degree
 *            V        \
 *            y         \ a=+45 degree
**/
void (*drawWL_Pix) (void* dev, Tod_GCdef gcd, double x, double y, CstrBrief text);
void (*drawNW_Pix) (void* dev, Tod_GCdef gcd, double x, double y, CstrBrief text);
void (*drawNWr_Pix)(void* dev, Tod_GCdef gcd, double x, double y,
                    CstrBrief text, double a);
void (*drawWLr_Pix)(void* dev, Tod_GCdef gcd, double x, double y,
                    CstrBrief text, double a);

/*======================================================================*/
};
/*======================================================================*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_STRING_VT_H
/*======================================================================*/
