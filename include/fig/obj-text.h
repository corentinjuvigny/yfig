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
 *      $Id: obj-text.h 367 2018-11-18 23:50:25Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-text.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Object for text.                                                   =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_TEXT_H
#define FILE_OBJ_TEXT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"

/*======================================================================*/

struct _TobjText {
    TobjAny     o_any;   // the common attributes to all objects

    int         ote_type;  // FIGOTE_T_LEFT/CENTER/RIGHT
                           // currently they are the same as STY_T_... and OD_SORG_...
                           // CAUTION: must correspond to ote_geometry.of

    /* drawing attributes */
    Tsty_text   ote_text;

    /** text parameters:
     * text is writes on a line, ascent is the height over the
     * line and descent is the height under the line.
     *   height = ascent + descent
    **/

    /* ote_base is the rotation center when angle is not null.
     * Wen angle is null:
     *   - ote_base.y gives the line where the text is drawn, ascent
     *     is over the line, descent is under the line.
     *   - ote_base.x is the:
     *      . West border of the text when type is LEFT
     *      . East border of the text when type is RIGHT
     *      . Middle of the text when type is CENTER
     * ote_ascent is the vertical distance from the top of text
     * to the line.*/
    //TpointDble ote_base;
    //double     ote_ascent;

    /* the text length and height. If angle is null, they are
     * the same as the dx and dy of the bounding box but differ
     * otherwise. */
    TstrEaten  ote_str;   /* the text to write */
    int        ote_flag;
#define FIGOTE_FL_DEFAULT -1
#define RIGID_TEXT         1
#define SPECIAL_TEXT       2   // ???
#define PSFONT_TEXT        4   // ???
#define HIDDEN_TEXT        8   // ???
#define psfont_text(t) ( t->OTE_flag!=FIGOTE_FL_DEFAULT && \
                        (t->OTE_flag&PSFONT_TEXT)!=0 )

    /* text geometry */
    Todstrgeo ote_geometry;
};

/*======================================================================*/

#define OTE_depth      O_depth
#define OTE_type       ote_type

#define OTE_text       ote_text
#define OTE_angle      ote_text.dpt_angle
#define OTE_font       ote_text.dpt_font
#define OTE_pts        ote_text.dpt_pts
#define OTE_color      ote_text.dpt_color

#define OTE_flag       ote_flag
#define OTE_str        ote_str
#define OTE_geom       ote_geometry
#define OTE_base       OTE_geom.pos
#define OTE_length     OTE_geom.dx
#define OTE_height     OTE_geom.dy
#define OTE_ascent     OTE_geom.ascent
#define OTE_nl         OTE_geom.nl

/*======================================================================*/

#define DBG0_TextE(fmt,...) DBG0_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG0_TextM(fmt,...) DBG0_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG0_TextL(fmt,...) DBG0_printf("text","Leave : "fmt,__VA_ARGS__)
#define DBG1_TextE(fmt,...) DBG1_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG1_TextM(fmt,...) DBG1_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG1_TextL(fmt,...) DBG1_printf("text","Leave : "fmt,__VA_ARGS__)
#define DBG2_TextE(fmt,...) DBG2_printf("text","Enter : "fmt,__VA_ARGS__)
#define DBG2_TextM(fmt,...) DBG2_printf("text","Middle: "fmt,__VA_ARGS__)
#define DBG2_TextL(fmt,...) DBG2_printf("text","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
#ifdef __cplusplus
}
#endif
/*======================================================================*/
#endif // FILE_OBJ_TEXT_H
/*======================================================================*/
