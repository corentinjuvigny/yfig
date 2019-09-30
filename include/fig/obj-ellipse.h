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
 *      $Id: obj-ellipse.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-ellipse.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for full circle and ellipse.                                =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_ELLIPSE_H
#define FILE_OBJ_ELLIPSE_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"

/*======================================================================*/

struct _TobjEllipse {
    TobjAny     o_any;       // the common attributes to all objects

    int         oel_type;
    /* drawing attributes */
    Tsty_line   oel_curve;
    Tsty_fill   oel_fill;
    /* ellipse parameters */
    float       oel_angle;      /* in radians */
    int         oel_direction;
    Tdble2      oel_center;
    Tdble2      oel_radiuses;
    Tdble2      oel_start;
    Tdble2      oel_end;
};

#define OEL_depth      O_depth
#define OEL_type       oel_type

#define OEL_curve      oel_curve
#define OEL_thickness  oel_curve.dpl_thickness
#define OEL_style      oel_curve.dpl_style
#define OEL_style_val  oel_curve.dpl_dotdash
#define OEL_pen_color  oel_curve.dpl_color
#define OEL_fill       oel_fill
#define OEL_fill_style oel_fill.dpf_style
#define OEL_fill_color oel_fill.dpf_color

#define OEL_angle      oel_angle
#define OEL_direction  oel_direction
#define OEL_center     oel_center
#define OEL_radiuses   oel_radiuses
#define OEL_start      oel_start
#define OEL_end        oel_end

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_ELLIPSE_H
/*======================================================================*/
