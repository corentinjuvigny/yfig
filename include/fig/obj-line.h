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
 *      $Id: obj-line.h 280 2018-07-05 23:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-line.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= object for opened and closed multi-lines, rectangular boxes        =*/
/*= and pictures.                                                      =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_LINE_H
#define FILE_OBJ_LINE_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"
#include "tools/color-types.h"

/*======================================================================*/

struct _TobjLine {
    TobjAny     o_any;   // the common attributes to all objects

    int         oli_type; // see FIGOLI_T_... in obj-object.h

    /* drawing attributes */
    Tsty_line   oli_line;
    Tsty_fill   oli_fill;

    /* line parameters */
    Tdble2Set*      oli_points; // list of vertices
                                // first and last points are the same
                                // when it's a closed multi-line (kind:
                                // T_Box, FIGOLI_T_Polygon, ...);
    int             oli_radius; // corner radius in ?? unit for round box
                                // (type=T_RoundBox), otherwise 0.
                                // Caution: not used in YFIG except for
                                // loading/saving to xfig format.

    /* arrows only for opened lines, set to NULL if none */
    TobjAll oli_begArrow;   // the arrow that starts the line.
    TobjAll oli_endArrow;   // the arrow that ends the line.

    /* pts to draw. Same points than oli_points but shorted when arrows */
    Tdble2Set* oli_drawpts;
};

/*======================================================================*/

#define OLI_depth        O_depth
#define OLI_type         oli_type

#define OLI_line         oli_line
#define OLI_thickness    oli_line.dpl_thickness
#define OLI_style        oli_line.dpl_style
#define OLI_style_val    oli_line.dpl_dotdash
#define OLI_pen_color    oli_line.dpl_color
#define OLI_cap          oli_line.dpl_cap
#define OLI_join         oli_line.dpl_join
#define OLI_fill         oli_fill
#define OLI_fill_style   oli_fill.dpf_style
#define OLI_fill_color   oli_fill.dpf_color

#define OLI_points       oli_points
#define OLI_radius       oli_radius

#define OLI_begArrow     oli_begArrow
#define OLI_endArrow     oli_endArrow

#define OLI_drawpts      oli_drawpts

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_LINE_H
/*======================================================================*/
