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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-pixmap.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-pixmap.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Pixmap and icon handler                                            =*/
/*=                                                                    =*/
/*= See the vgui/vgui-pixmap.h header for the type description, the    =*/
/*= function description and the documentation.                        =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-pixmap.h"

#include "tools/color.h"

/*======================================================================*/
/*= Icons                                                              =*/

//extern Tvgui_icon* vgui_icon_newLabel(int width, int height, const char* label);

extern Tvgui_icon* vgui_icon_newBitmap(int width, int height, const char* bitmap)
{
    Tvgui_icon* ret = tool_malloc(sizeof(*ret));
    ret->vgicon_kind    = VGUI_ICON_KIND_Bitmap;
    ret->vgicon_width   = width;
    ret->vgicon_height  = height;
    int bytesize        = VGUI_BTM_DataSize(width,height) ;
    ret->vgicon_data    = tool_malloc(bytesize);
    memcpy(ret->vgicon_data,bitmap,bytesize);
    return ret;
}

extern void        vgui_icon_free(Tvgui_icon*icon)
{
    tool_free(icon->vgicon_data);
    tool_free(icon);
}

/*======================================================================*/
/*= Pixmaps                                                            =*/

extern TpixmapEGS vgui_pixmap_create(Tvgui*gui, Ccolor*fg, Ccolor*bg,
                                      const Tvgui_icon* icon)
{
    if ( icon->vgicon_kind==VGUI_ICON_KIND_Bitmap ) {
        return gui_pixmap_create_FromBitmap(gui,
                fg->egs, bg->egs,
                icon->vgicon_width, icon->vgicon_height, icon->vgicon_data);
    } else {
        PERM_spf_printf("pixmap kind %d not implemented\n",
                        icon->vgicon_kind);
    }
    return PXMEGS_none;
}

/*======================================================================*/
