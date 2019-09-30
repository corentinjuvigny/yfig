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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: obj-arc.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-arc.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Object for elliptical arcs.                                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_ARC_H
#define FILE_OBJ_ARC_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"

#include "tools/color-types.h"

/*======================================================================*/

struct _TobjArc {
    TobjAny     o_any;    // the common attributes to all objects

    int         oar_type;  // see the FIGOAR_T_... macros

    /* drawing attributes */
    Tsty_line   oar_line;      // join and cap fixed by system
    Tsty_fill   oar_fill;

    /* arc parameters */
    Tdble2 oar_points[4];  // the 3 points of the arc + the center 
#define    oar_center      oar_points[3]
    int    oar_direction;  // 0:points[0:2] are clockwise,
                           // 1:points[0:2] are counter clockwise 

    /* arrows only for opened arcs, set to NULL if none */
    TobjAll oar_begArrow;   // the arrow that starts the arc.
    TobjAll oar_endArrow;   // the arrow that ends the arc.

    /* pts to draw. Same points than oar_points but shorted when arrows */
    Tdble2 oar_drawpts[4];
};

/*======================================================================*/

#define OAR_depth        O_depth
#define OAR_type         oar_type

#define OAR_line         oar_line
#define OAR_thickness    oar_line.dpl_thickness
#define OAR_style        oar_line.dpl_style
#define OAR_style_val    oar_line.dpl_dotdash
#define OAR_pen_color    oar_line.dpl_color
#define OAR_cap          oar_line.dpl_cap
#define OAR_fill         oar_fill
#define OAR_fill_style   oar_fill.dpf_style
#define OAR_fill_color   oar_fill.dpf_color

#define OAR_points       oar_points
#define OAR_center       oar_center
#define OAR_direction    oar_direction

#define OAR_begArrow     oar_begArrow
#define OAR_endArrow     oar_endArrow

#define OAR_drawpts      oar_drawpts

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_ARC_H
/*======================================================================*/
