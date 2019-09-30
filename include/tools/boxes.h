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
 *      $Id: boxes.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/boxes.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= rectangular boxes.                                                 =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_BOXES_H
#define FILE_TOOLS_BOXES_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/

#define Box_x(b)      ((b).box_northWest.x)
#define Box_y(b)      ((b).box_northWest.y)
#define Box_dx(b)     ((b).box_size.x)
#define Box_dy(b)     ((b).box_size.y)
#define Box_width(b)  ((b).box_size.x)
#define Box_height(b) ((b).box_size.y)
#define Box_xmin(b)   ((b).box_northWest.x)
#define Box_ymin(b)   ((b).box_northWest.y)
#define Box_xmax(b)   ((b).box_northWest.x+Box_dx(b))
#define Box_ymax(b)   ((b).box_northWest.y+Box_dy(b))

#define Box_nwx(b)   ((b).box_northWest.x)
#define Box_nwy(b)   ((b).box_northWest.y)
#define Box_nex(b)   ((b).box_northWest.x+Box_dx(b))
#define Box_ney(b)   ((b).box_northWest.y)
#define Box_sex(b)   ((b).box_northWest.x+Box_dx(b))
#define Box_sey(b)   ((b).box_northWest.y+Box_dy(b))
#define Box_swx(b)   ((b).box_northWest.x)
#define Box_swy(b)   ((b).box_northWest.y+Box_dy(b))

#define BoxArg_XYWH(b) Box_x(b),Box_y(b),Box_dx(b),Box_dy(b)

#define Box_initBy_minXY_maxXY(b,xmin,ymin,xmax,ymax) do {\
            Box_x(b)  = xmin;          Box_y(b)  = ymin; \
            Box_dx(b) = (xmax)-(xmin); Box_dy(b) = (ymax)-(ymin); \
          } while (0)

/*======================================================================*/

struct _TboxDble {
    Tdble2 box_northWest;  // north west points
    Tdble2 box_size;       // size of box
};

extern CboxDble tool_boxDble_undef;
#define         tool_boxDble_isUndef(b)   (((b).box_size.x==0 && (b).box_size.y==0) ? 1 : 0)
#define         tool_boxDble_isDefined(b) (((b).box_size.x==0 && (b).box_size.y==0) ? 0 : 1)

Inline Tdble2   tool_boxdble_pt1(CboxDble* b) { Tdble2 p=dble2_init(Box_x(*b)              ,Box_y(*b)); return p; }
Inline Tdble2   tool_boxdble_pt2(CboxDble* b) { Tdble2 p=dble2_init(Box_x(*b)+Box_dx(*b)   ,Box_y(*b)); return p; }
Inline Tdble2   tool_boxdble_pt3(CboxDble* b) { Tdble2 p=dble2_init(Box_x(*b)+Box_dx(*b)   ,Box_y(*b)+Box_dy(*b)); return p; }
Inline Tdble2   tool_boxdble_pt4(CboxDble* b) { Tdble2 p=dble2_init(Box_x(*b)              ,Box_y(*b)+Box_dy(*b)); return p; }
Inline Tdble2   tool_boxdble_ctr(CboxDble* b) { Tdble2 p=dble2_init(Box_x(*b)+Box_dx(*b)/2.,Box_y(*b)+Box_dy(*b)/2.); return p; }

/*======================================================================*/
/*= Initialization functions                                           =*/

/**
 * The tool_boxDble_initXY function initializes the box setting:
 *   - the dx and dy of rectangle positive
 *   - the corner of the box to the (xmin,ymin) corner of the rectangle.
 *      --> North-West corner for x axis from left to right hand
 *          and y axis from top to bottom.
 *      --> Sud-West corner for x axis from left to right hand
 *          and y axis from bottom to top.
**/
extern void     tool_boxDble_initXY(TboxDble* b, double x1, double y1, double x2, double y2);
#define         tool_boxDble_initPT(b,pt1,pt2) tool_boxDble_initXY(b,pt1.x,pt1.y, pt2.x,pt2.y)
extern TboxDble tool_boxdble_initXY(double x1, double y1, double x2, double y2);
#define         tool_boxdble_initPT(pt1,pt2) tool_boxdble_initXY(pt1.x,pt1.y, pt2.x,pt2.y)

Inline void     tool_boxDble_initXYWH(TboxDble* b, double x, double y, double dx, double dy) { Box_x(*b)=x; Box_y(*b)=y; Box_dx(*b)=dx; Box_dy(*b)=dy; }
#define         tool_boxDble_initPTWH(b,pt,dx,dy) tool_boxDble_initXY(b,pt.x,pt.y, dx,dy)
Inline TboxDble tool_boxdble_initXYWH(double x, double y, double dx, double dy)              { TboxDble box; tool_boxDble_initXYWH(&box,x,y,dx,dy); return box; }
#define         tool_boxdble_initPTWH(pt,dx,dy) tool_boxdble_initXY(pt.x,pt.y, dx,dy)

extern TboxDble tool_boxDble_initByDble2T   (Cdble2*s, int nb);
extern TboxDble*tool_boxDble_initByDble2Tptr(Cdble2*s, int nb, TboxDble*bb);
extern TboxDble tool_boxDble_initByDble2S   (Cdble2Set*s);
extern TboxDble*tool_boxDble_initByDble2Sptr(Cdble2Set*s,TboxDble*bb);

/*======================================================================*/
/*= Other functions                                                    =*/

Inline void tool_boxDble_time   (TboxDble* b, double f) { b->box_northWest.x *= f; b->box_northWest.y *= f; 
                                                          b->box_size.x      *= f; b->box_size.y *= f; }
extern void tool_boxDble_union  (TboxDble* dest, CboxDble* src);

/**
 * Return
 *   1: if the boxes overlap (intersection of the rectangles is not empty).
 *   0: if the boxes do'nt overlap
**/
extern int tool_boxDble_overlap(CboxDble* b1, CboxDble* b2);

/**
 * Return
 *   1: if the bin box is within the bco box.
 *   0: oftherwise.
**/
extern int tool_boxDble_isInside(CboxDble* bin, CboxDble* bco);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif  // FILE_TOOLS_BOXES_H
/*======================================================================*/

