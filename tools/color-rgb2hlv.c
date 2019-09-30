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
 *      $Id: color-rgb2hlv.c 141 2018-05-27 13:32:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/color-rgb2hlv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Color package.                                                     =*/
/*=                                                                    =*/
/*= Implementation of the converters from (to) RGB to (from) HSV       =*/
/*= color formats.                                                     =*/
/*=                                                                    =*/
/*= See the color-types.h and color.h headers for documentation.       =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/color.h"

/*======================================================================*/

static void rgb8_init(TcolorRGB* rgb, double r, double g, double b);

/*======================================================================*/

extern void color_rgb8ToHsv(double* hue, double* sat, double* val,
                            CcolorRGB* rgb)
    {
    double M = 0.0, m = 0.0, c = 0.0;
    double r = rgb->red;   r /= 255.;
    double g = rgb->green; g /= 255.;
    double b = rgb->blue;  b /= 255.;
    M = max3(r, g, b);
    m = min3(r, g, b);
    c = M - m;
    *hue=0;
    *sat= M==0 ? 0.0 : c/M;
    *val=M;
    if ( c < 0.0001 )
        return;
    if (M == r) {
        *hue = (g - b) / c ; //fmod(((g - b) / c), 6.0);
    } else if (M == g) {
        *hue = (b - r) / c + 2.0;
    } else /*if(M==b)*/ {
        *hue = (r - g) / c + 4.0;
    }
    *hue *= 60.0;
    if ( *hue<0.0 ) hue += 360;
}

extern void color_hsvToRgb8(TcolorRGB* rgb,
                            double     hue, double  sat, double  val)
{
    double c = 0.0, m = 0.0, x = 0.0;
    c = val * sat;
    x = c * (1.0 - fabs(fmod(hue / 60.0, 2) - 1.0));
    m = val - c;
    if (hue >= 0.0 && hue < 60.0) {
        rgb8_init(rgb,c + m, x + m, m);
    } else if (hue >= 60.0 && hue < 120.0) {
        rgb8_init(rgb,x + m, c + m, m);
    } else if (hue >= 120.0 && hue < 180.0) {
        rgb8_init(rgb,m, c + m, x + m);
    } else if (hue >= 180.0 && hue < 240.0) {
        rgb8_init(rgb,m, x + m, c + m);
    } else if (hue >= 240.0 && hue < 300.0) {
        rgb8_init(rgb,x + m, m, c + m);
    } else if (hue >= 300.0 && hue < 360.0) {
        rgb8_init(rgb,c + m, m, x + m);
    } else {
        rgb8_init(rgb,m, m, m);
    }
}

/*======================================================================*/

static void rgb8_init(TcolorRGB* rgb, double r, double g, double b)
{
    rgb->red   = round(r*255.);
    rgb->green = round(g*255.);
    rgb->blue  = round(b*255.);
}

/*======================================================================*/
