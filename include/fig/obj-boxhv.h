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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-boxhv.h 292 2018-07-11 07:32:52Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-boxhv.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object(s) container the shape of which is a rectangular boxes      =*/
/*= without any rotation.                                              =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_BOXHV_H
#define FILE_OBJ_BOXHV_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"
#include "tools/bitmap-types.h"

/*======================================================================*/

struct _TobjBoxhv {
    TobjAny     o_any;   // the common attributes to all objects

    int         obhv_type; // see FIGOBHV_T_... in obj-object.h

    /* box parameters */
    Tdble2      obhv_points[4]; // list of vertices

    /* box content (depends on obhv_type) */
    union {
        Timage* img;
    } u;
};

/*======================================================================*/

#define OBHV_depth        O_depth
#define OBHV_type         obhv_type

#define OBHV_points       obhv_points
#define OBHV_x0(o)        (o)->OBHV_points[0].x
#define OBHV_y0(o)        (o)->OBHV_points[0].y
#define OBHV_x2(o)        (o)->OBHV_points[0].x
#define OBHV_y2(o)        (o)->OBHV_points[0].y
#define OBHV_diagonal(o)  OBHV_x0(o),OBHV_y0(o),OBHV_x2(o),OBHV_y2(o)
#define OBHV_xywh(o)      BoxArg_XYWH(o->O_boundBox)
#define OBHV_img          u.img

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_BOXHV_H
/*======================================================================*/
