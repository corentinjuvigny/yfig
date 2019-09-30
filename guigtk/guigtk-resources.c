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
 *$Software: YFIG-GTK (Interface to GTK)
 *      $Id: guixt-resources.c 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-resources.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource API (EGS driver).                                     =*/
/*=                                                                    =*/
/*= See the "vgui/vgui-resources.h" header for the type description,   =*/
/*= the function description and for the documentation.                =*/
/*= FIXME Not yet implemented.                                         =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/vgui-resources.h"
#include "vgui/vgui-resources-table.h"

/*======================================================================*/

extern Tgui_resDB* gui_res_new (Tvgui*gui)
{
    PERM_error_NYI();
    return 0;
}

extern void gui_res_free(Tvgui*gui, Tgui_resDB* rdb)
{
    return;
}

extern char* gui_res_get (Tvgui*gui, Tgui_resDB* rdb, const char* rn)
{
    return NULL;
}

/*======================================================================*/
