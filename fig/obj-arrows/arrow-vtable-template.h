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
 *      $Id: arrow-vtable-template.h 266 2018-07-04 00:11:04Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arrows/arrow-vtable-template.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Template for vtable of arrow object.                               =*/
/*=                                                                    =*/
/*= Usage:                                                             =*/
/*=     #define ARROW_ID xxx                                           =*/
/*=     #include "arrow-vtable-template.h"                             =*/
/*=                                                                    =*/
/*= Creates the arrowTxxx_vtable with                                  =*/
/*=      vtable field        set to macro      default of macro        =*/
/*=    .vt_draw_sc          ARROW_draw_sc     arrowxxx_draw_sc         =*/
/*=    .vt_arrowDoBegLine   ARROW_doBegLine   arrowxxx_doBegLine       =*/
/*=    .vt_arrowDoEndLine   ARROW_doEndLine   arrowxxx_doEndLine       =*/
/*=    .vt_arrowDoBegSpline ARROW_doBegSpline arrowxxx_doBegSpline     =*/
/*=    .vt_arrowDoEndSpline ARROW_doEndSpline arrowxxx_doEndSpline     =*/
/*=    .vt_arrowDoBegArc    ARROW_doBegArc    arrowxxx_doBegArc        =*/
/*=    .vt_arrowDoEndArc    ARROW_doEndArc    arrowxxx_doEndArc        =*/
/*======================================================================*/

#undef  TOSTR
#define TOSTR(s) #s

#undef  CAT2
#undef  CAT2b
#define CAT2(x,y) CAT2b(x,y)
#define CAT2b(x,y) x##y

#undef  CAT3
#undef  CAT3b
#define CAT3(x,y,z) CAT3b(x,y,z)
#define CAT3b(x,y,z) x##y##z

#ifndef ARROW_draw_sc 
#define ARROW_draw_sc CAT3(arrow,ARROW_ID,_draw_sc)
#endif

#ifndef ARROW_doBegLine 
#define ARROW_doBegLine CAT3(arrow,ARROW_ID,_doBegLine)
#endif

#ifndef ARROW_doEndLine
#define ARROW_doEndLine CAT3(arrow,ARROW_ID,_doEndLine)
#endif

#ifndef ARROW_doBegSpline 
#define ARROW_doBegSpline CAT3(arrow,ARROW_ID,_doBegSpline)
#endif

#ifndef ARROW_doEndSpline
#define ARROW_doEndSpline CAT3(arrow,ARROW_ID,_doEndSpline)
#endif

#ifndef ARROW_doBegArc 
#define ARROW_doBegArc CAT3(arrow,ARROW_ID,_doBegArc)
#endif

#ifndef ARROW_doEndArc
#define ARROW_doEndArc CAT3(arrow,ARROW_ID,_doEndArc)
#endif

/*======================================================================*/

TobjVtable CAT3(arrowT,ARROW_ID,_vtable) = {
    .vt_kind             = CAT2(FIG_KD_ArrowT,ARROW_ID),
    .vt_bytesz           = sizeof( TobjArrow ),
    .vt_fdl              = offsetof(TobjArrow,OAW_line),
    .vt_fdf              = offsetof(TobjArrow,OAW_fill),
    .vt_name             = "arrow-" TOSTR(ARROW_ID),
    .vt_dump             = arrow_dump,

    .vt_free             = arrow_free,
    .vt_clone            = arrow_clone,
  //.vt_dupNDP           = arrow_dupNDP,
    .vt_boundBox         = arrow_boundBox,

    .vt_translation      = arrow_translation,
  //.vt_homothety00      = arrow_homothety00,
    .vt_homothetyCC      = arrow_homothetyCC,
  //.vt_scale00          = arrow_scale00,
    .vt_scaleCC          = arrow_scaleCC,
    .vt_rotate           = arrow_rotate,
  //.vt_flip             = arrow_flip,
  //.vt_doDrawData       = arrow_doDrawData,
    .vt_draw_sc          = ARROW_draw_sc,

    .vt_arrowDoBegLine   = ARROW_doBegLine,
    .vt_arrowDoEndLine   = ARROW_doEndLine,
    .vt_arrowDoBegSpline = ARROW_doBegSpline,
    .vt_arrowDoEndSpline = ARROW_doEndSpline,
    .vt_arrowDoBegArc    = ARROW_doBegArc,
    .vt_arrowDoEndArc    = ARROW_doEndArc,
};

/*======================================================================*/

#undef  TOSTR
#undef  CAT2
#undef  CAT2b
#undef  CAT3
#undef  CAT3b

#undef ARROW_ID
#undef ARROW_draw_sc 
#undef ARROW_doBegLine 
#undef ARROW_doEndLine
#undef ARROW_doBegSpline 
#undef ARROW_doEndSpline
#undef ARROW_doBegArc 
#undef ARROW_doEndArc

/*======================================================================*/
