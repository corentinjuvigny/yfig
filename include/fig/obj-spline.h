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
 *      $Id: obj-spline.h 234 2018-06-20 08:28:18Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-spline.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for opened and closed spline curves.                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_SPLINES_H
#define FILE_OBJ_SPLINES_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"
#include "tools/color-types.h"

/*======================================================================*/

#define     approx_spline(s)    ( (s->OSP_type==FIGOSP_T_ClosedApprox) ||\
                                  (s->OSP_type==FIGOSP_T_OpenApprox) )
#define     closed_spline(s)    ( (s->OSP_type==FIGOSP_T_ClosedApprox) ||\
                                  (s->OSP_type==FIGOSP_T_ClosedInterp) )
#define S_SPLINE_ANGULAR 0.0
#define S_SPLINE_APPROX  1.0
#define S_SPLINE_INTERP (-1.0)

/*======================================================================*/

struct _TobjSpline {
    TobjAny     o_any;       // the common attributes to all objects

    int         osp_type;    // see  FIGOSP_T_... macros.

    /* drawing attributes */
    Tsty_line   osp_line; // join dpl_join not used
    Tsty_fill   osp_fill;

    /* spline parameters */
    Tdble2Set* osp_points;   // control points (In closed spline, first point
                             // is not repeated at the end) 
    TdbleSet*  osp_nodes;    // spline nodes

    /* arrows (only for opened splines) */
    TobjAll osp_begArrow;   // the arrow that starts the line.
    TobjAll osp_endArrow;   // the arrow that ends the line.

    /* pts to draw. Same as osp_points osp_sfactors but shorted when arrows */
    Tdble2Set* osp_drawpts;
    TdbleSet*  osp_drawnds;
};

/*======================================================================*/

#define OSP_depth        O_depth
#define OSP_type         osp_type
                        
#define OSP_line         osp_line
#define OSP_thickness    osp_line.dpl_thickness
#define OSP_style        osp_line.dpl_style
#define OSP_style_val    osp_line.dpl_dotdash
#define OSP_pen_color    osp_line.dpl_color
#define OSP_cap          osp_line.dpl_cap
#define OSP_join         osp_line.dpl_join
#define OSP_fill         osp_fill
#define OSP_fill_style   osp_fill.dpf_style
#define OSP_fill_color   osp_fill.dpf_color

#define OSP_points       osp_points
#define OSP_nodes        osp_nodes

#define OSP_begArrow     osp_begArrow
#define OSP_endArrow     osp_endArrow

#define OSP_drawctls     osp_drawpts
#define OSP_drawndes     osp_drawnds

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_SPLINES_H
/*======================================================================*/
