/*======================================================================
 *  Copyright 2017 Timothee Denoux
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
 *      $Id: guigtk-init.c 163 2018-06-06 07:27:50Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-init.c $
 *  $Author: Timothee Denoux (Email: timothee.denoux@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The color EGS driver.                                              =*/
/*=                                                                    =*/
/*= See the "tools/color.h" "vgui/vgui.h" headers for the type         =*/
/*= description, the function description and for the documentation.   =*/
/*======================================================================*/

#include "guigtk.h"

/*======================================================================*/

extern void gui_color_delete(Tvgui*gui, TcolorEGS c)
{
    gdk_rgba_free(GtkCOLOR(c));
}

extern TcolorEGS gui_color_dup(Tvgui*gui, TcolorEGS c)
{
    GdkRGBA *gtkcolor = gdk_rgba_copy(GtkCOLOR(c));
    TcolorEGS ret;
    ret.ptr = gtkcolor;
    return ret;
}

extern TcolorEGS gui_color_newByName(Tvgui* gui, const char* name)
{
    GdkRGBA gtkcolor;
    if(gdk_rgba_parse(&gtkcolor,name)){
        TcolorEGS ret; ret.ptr = tool_malloc_0(sizeof(GdkRGBA));
        memcpy(ret.ptr,&gtkcolor,sizeof(GdkRGBA));
        return ret;
    } else{
        return EGSCOLOR_none;
    }
}

extern TcolorEGS gui_color_newByRGB08(Tvgui* gui, int r, int g, int b)
{
    GdkRGBA gtkcolor;
    char colors[100];
    sprintf(colors,"rgb(%d,%d,%d)",r,g,b);
    if(gdk_rgba_parse(&gtkcolor,colors)){
        TcolorEGS ret; ret.ptr = tool_malloc_0(sizeof(GdkRGBA));
        memcpy(ret.ptr,&gtkcolor,sizeof(GdkRGBA));
        return ret;
    } else {
        return EGSCOLOR_none;
    }
}

extern TcolorEGS gui_color_newByRGB16(Tvgui* gui, int r, int g, int b)
{
    return gui_color_newByRGB08(gui,r>>8,g>>8,b>>8);
}

extern void       gui_color_getRGB08(Tvgui* gui, TcolorEGS c,
                                   int*r, int*g, int*b)
{
    if ( COLOREGS_IsDefined(c) ) {
        *r = (int)(255*GtkCOLOR(c)->red);
        *g = (int)(255*GtkCOLOR(c)->green);
        *b = (int)(255*GtkCOLOR(c)->blue);
    } else {
        *r=-1; *g=-1; *b=-1;
    }

}
extern void       gui_color_getRGB16(Tvgui* gui, TcolorEGS c,
                                   int*r, int*g, int*b)
{
    if ( COLOREGS_IsDefined(c)) {
        gui_color_getRGB08(gui,c,r,g,b);
        *r <<= 8; *g <<= 8; *b <<= 8; 
    } else {
        *r=-1; *g=-1; *b=-1;
    }

}
/*======================================================================*/
