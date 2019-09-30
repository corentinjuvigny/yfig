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
 *      $Id: obj-any.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-any.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The top class of any object (The data structure that all the       =*/
/*= share).                                                            =*/
/*= See the fig/obj-any.h header for types and documentation.          =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/obj-any.h"
#include "fig-config.h"
#include "fig/obj-any.h"

/*======================================================================*/

extern int  objlist_size(CobjAny* list)
{
    int size=0;
    for ( ; list!=0 ; list=list->oa_next ) size+=1;
    return size;
}

extern void objlist_addAhead(TobjAny** list, Tobject obj)
{
    TobjAny* any = (TobjAny*)obj;
    any->oa_next = *list;
    *list = any;
}

extern void objlist_remove  (TobjAny** list, Tobject obj)
{
    TobjAny* any = (TobjAny*)obj;

    if ( *list==0 ) return;
    if ( (*list)==any ) {
        (*list)=any->oa_next;
        any->oa_next=0;
        return;
    }
    objlist_remove(&(*list)->oa_next,obj);
}

/*======================================================================*/
