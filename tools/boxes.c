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
 *      $Id: boxes.c 293 2018-07-11 10:18:05Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/boxes.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= rectangular boxes.                                                 =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/boxes.h"
#include "tools/geo2d.h"

/*======================================================================*/

CboxDble tool_boxDble_undef = { { 0,0}, {0,0} };

/*======================================================================*/
/*= Initialization functions                                           =*/

extern void tool_boxDble_initXY(
        TboxDble* b, double x1, double y1, double x2, double y2)
{
    tool_rectToNWSE_XYD(&x1,&y1,&x2,&y2);
    b->box_northWest.x = x1;
    b->box_northWest.y = y1;
    b->box_size.x      = x2-x1;
    b->box_size.y      = y2-y1;
}

extern TboxDble tool_boxdble_initXY(double x1, double y1, double x2, double y2)
{ TboxDble box; tool_boxDble_initXY(&box,x1,y1,x2,y2); return box; }

extern TboxDble tool_boxDble_initByDble2T   (Cdble2*s, int nb)
{ TboxDble box; return *tool_boxDble_initByDble2Tptr(s,nb,&box); }

extern TboxDble*tool_boxDble_initByDble2Tptr(Cdble2*s, int nb, TboxDble*bb)
{
    Tdble2 min,max;
    dble2_minmaxXY(s,nb,&min,&max);
    Box_initBy_minXY_maxXY(*bb,min.x,min.y,max.x,max.y);
    return bb;
}

extern TboxDble tool_boxDble_initByDble2S   (Cdble2Set*s)
{
    TboxDble bb;
    if (s==0)
        bb=tool_boxDble_undef;
    else
        bb=tool_boxDble_initByDble2T(s->eles,s->eleNb);
    return bb; 
}

extern TboxDble*tool_boxDble_initByDble2Sptr(Cdble2Set*s,TboxDble*bb)
{
    if (s==0) {
        *bb=tool_boxDble_undef;
        return bb; 
    } else
        return tool_boxDble_initByDble2Tptr(s->eles,s->eleNb,bb);
}

/*======================================================================*/
/*= Other functions                                                    =*/

extern void tool_boxDble_union(TboxDble* dest, const TboxDble* src)
{
    if ( tool_boxDble_isUndef(*src) )
        return;
    if ( tool_boxDble_isUndef(*dest) ) {
        // dest is not a rectangle.
        *dest=*src;
        return;
    }
    double minx = Box_xmin(*dest)<Box_xmin(*src) ? Box_xmin(*dest) : Box_xmin(*src);
    double miny = Box_ymin(*dest)<Box_ymin(*src) ? Box_ymin(*dest) : Box_ymin(*src);
    double maxx = Box_xmax(*dest)>Box_xmax(*src) ? Box_xmax(*dest) : Box_xmax(*src);
    double maxy = Box_ymax(*dest)>Box_ymax(*src) ? Box_ymax(*dest) : Box_ymax(*src);
    tool_boxDble_initXY(dest,minx,miny,maxx,maxy);
}

extern int tool_boxDble_overlap(CboxDble* b1, CboxDble* b2)
{
    if ( Box_xmin(*b1)<Box_xmin(*b2) )
       if ( Box_ymin(*b1)<Box_ymin(*b2) )
            return Box_xmax(*b1) >= Box_xmin(*b2) && Box_ymax(*b1) >= Box_ymin(*b2);
       else
            return Box_xmax(*b1) >= Box_xmin(*b2) && Box_ymin(*b1) <= Box_ymax(*b2);
    else if (Box_ymin(*b1) < Box_ymin(*b2))
        return Box_xmin(*b1) <= Box_xmax(*b2) && Box_ymax(*b1) >= Box_ymin(*b2);
    else
        return Box_xmin(*b1) <= Box_xmax(*b2) && Box_ymin(*b1) <= Box_ymax(*b2);
}

extern int tool_boxDble_isInside(CboxDble* bin, CboxDble* bco)
{
    Tdble2 inpt1 = tool_boxdble_pt1(bin);
    Tdble2 inpt2 = tool_boxdble_pt3(bin);
    Tdble2 copt1 = tool_boxdble_pt1(bco);
    Tdble2 copt2 = tool_boxdble_pt3(bco);

    if ( inpt1.x < copt1.x ) return 0;
    if ( inpt1.y < copt1.y ) return 0;
    if ( inpt2.x > copt2.x ) return 0;
    if ( inpt2.y > copt2.y ) return 0;

    return 1;
}

/*======================================================================*/
