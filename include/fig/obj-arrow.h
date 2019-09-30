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
 *      $Id: obj-arrow.h 266 2018-07-04 00:11:04Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-arrow.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for arrows                                                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_ARROW_H
#define FILE_OBJ_ARROW_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"
#include "tools/color-types.h"

/*======================================================================*/

struct _TobjArrow {
    TobjAny     o_any;   // the common attributes to all objects

    /* drawing attributes */
    Tsty_line   oaw_line;
    Tsty_fill   oaw_fill;

    /* arrow base parameters */
    TstyleMask             // indicate the style fields that must
           oaw_stymask;    // be updated from parent styles.
    int    oaw_baseabs;    // 1: oaw_baselength is an absolute value
                           // 0: it is relative to the parent line thickness
    float  oaw_baselength; // in FU unit
    double oaw_baseangle;  // in radian (<pi/2)

    /* arrow current parameters */
    double oaw_currlength;  // in OA_ppi unit

    /**
     * points of arrow
     * Their meaning depends on the arrow kind
    **/
    Tdble2Set* oaw_curr1pts;    // points of first part (usually the middle line)
    Tdble2Set* oaw_curr2pts;    // points of second part (usually the arrow lines)
    Tdble2Set* oaw_curr3pts;    // points of third part (extra
};

/*======================================================================*/

#define OAW_depth        O_depth
#define OAW_type         oli_type

#define OAW_line         oaw_line
#define OAW_thickness    OAW_line.dpl_thickness
#define OAW_style        OAW_line.dpl_style
#define OAW_style_val    OAW_line.dpl_dotdash
#define OAW_pen_color    OAW_line.dpl_color
#define OAW_cap          OAW_line.dpl_cap
#define OAW_join         OAW_line.dpl_join
#define OAW_fill         oaw_fill
#define OAW_fill_style   oaw_fill.dpf_style
#define OAW_fill_color   oaw_fill.dpf_color
#define OAW_pen_style    oaw_pen_style

#define OAW_stymask      oaw_stymask
#define OAW_baseabs      oaw_baseabs
#define OAW_baselength   oaw_baselength
#define OAW_baseangle    oaw_baseangle

#define OAW_currlength   oaw_currlength
#define OAW_currangle    oaw_currangle

#define OAW_pts1         oaw_curr1pts
#define OAW_pts2         oaw_curr2pts
#define OAW_pts3         oaw_curr3pts

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_ARROW_H
/*======================================================================*/

