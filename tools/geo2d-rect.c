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
 *      $Id: geo2d-rect.c 35 2017-03-11 07:01:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/geo2d-rect.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= 2 dimension geometric utilities                                    =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/geo2d.h"

/*======================================================================*/

extern void tool_rectToNWSE_XYI (int* x1, int* y1, int* x2, int* y2)
{
    int nwx,nwy,sex,sey;

    if ( *x1<*x2 ) {
        nwx = *x1;
        sex = *x2;
    } else {
        nwx = *x2;
        sex = *x1;
    }
    if ( *y1<*y2 ) {
        nwy = *y1;
        sey = *y2;
    } else {
        nwy = *y2;
        sey = *y1;
    }
    *x1=nwx; *y1=nwy; *x2=sex; *y2=sey;
}

extern void tool_rectToNWSE_XYD (
        double* x1, double* y1, double* x2, double* y2)
{
    double nwx,nwy,sex,sey;

    if ( *x1<*x2 ) {
        nwx = *x1;
        sex = *x2;
    } else {
        nwx = *x2;
        sex = *x1;
    }
    if ( *y1<*y2 ) {
        nwy = *y1;
        sey = *y2;
    } else {
        nwy = *y2;
        sey = *y1;
    }
    *x1=nwx; *y1=nwy; *x2=sex; *y2=sey;
}

extern void tool_rectToNWSE_BXD (TboxDble* b)
{
    if ( b->box_size.x<0 ) {
        b->box_northWest.x +=  b->box_size.x;
        b->box_size.x       = -b->box_size.x;
    }
    if ( b->box_size.y<0 ) {
        b->box_northWest.y +=  b->box_size.y;
        b->box_size.y       = -b->box_size.y;
    }
}

/*======================================================================*/

extern int tool_rectOverlap_XYI(
        int xmin1, int ymin1, int xmax1, int ymax1,
        int xmin2, int ymin2, int xmax2, int ymax2)
{
    if (xmin1 < xmin2)
        if (ymin1 < ymin2)
            return (xmax1 >= xmin2 && ymax1 >= ymin2);
        else
            return (xmax1 >= xmin2 && ymin1 <= ymax2);
    else if (ymin1 < ymin2)
        return (xmin1 <= xmax2 && ymax1 >= ymin2);
    else
        return (xmin1 <= xmax2 && ymin1 <= ymax2);
}

extern int tool_rectOverlap_XYD(
        double xmin1, double ymin1, double xmax1, double ymax1,
        double xmin2, double ymin2, double xmax2, double ymax2)
{
    if (xmin1 < xmin2)
        if (ymin1 < ymin2)
            return (xmax1 >= xmin2 && ymax1 >= ymin2);
        else
            return (xmax1 >= xmin2 && ymin1 <= ymax2);
    else if (ymin1 < ymin2)
        return (xmin1 <= xmax2 && ymax1 >= ymin2);
    else
        return (xmin1 <= xmax2 && ymin1 <= ymax2);
}

/*======================================================================*/

