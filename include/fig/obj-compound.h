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
 *      $Id: obj-compound.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-compound.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= An object container.                                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_COMPOUND_H
#define FILE_OBJ_COMPOUND_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "obj-any.h"

/*======================================================================*/

struct _TobjCompound {
    TobjAny  o_any;    // the common attributs to all objects

    TobjAllSet* o_children;

    TobjAny* o_lines;
    TobjAny* o_ellipses;
    TobjAny* o_splines;
    TobjAny* o_texts;
    TobjAny* o_arcs;
    TobjAny* o_compounds;
    TobjAny* o_others;
};

#define OCO_children    o_children
#define OCO_nwcorner    o_nwcorner
#define OCO_secorner    o_secorner
#define OCO_lines       o_lines
#define OCO_ellipses    o_ellipses
#define OCO_splines     o_splines
#define OCO_texts       o_texts
#define OCO_arcs        o_arcs
#define OCO_compounds   o_compounds
#define OCO_others      o_others
                       
/*======================================================================*/

#define STD_LOOP(o,type,...)  \
    for ( o=self->type  ; o!=0 ; o=o->oa_next ) { __VA_ARGS__}

#define STD_LOOPS_NoComp(o,...)  \
    STD_LOOP(o,OCO_arcs     ,__VA_ARGS__) \
    STD_LOOP(o,OCO_ellipses ,__VA_ARGS__) \
    STD_LOOP(o,OCO_lines    ,__VA_ARGS__) \
    STD_LOOP(o,OCO_splines  ,__VA_ARGS__) \
    STD_LOOP(o,OCO_texts    ,__VA_ARGS__) \
    STD_LOOP(o,OCO_others   ,__VA_ARGS__)

#define STD_LOOPS(o,...)  \
    STD_LOOPS_NoComp(o,__VA_ARGS__) \
    STD_LOOP(o,OCO_compounds,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_COMPOUND_H
/*======================================================================*/
