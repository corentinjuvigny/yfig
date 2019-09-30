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
 *$Software: YFIG-XT (VGUI driver to Xt and Xawd or Xaw3d)
 *      $Id: guixt-color.c 113 2018-01-09 18:35:22Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-color.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The color EGS driver.                                              =*/
/*=                                                                    =*/
/*= See the "tools/color.h" "vgui/vgui.h" headers for the type         =*/
/*= description, the function description and for the documentation.   =*/
/*======================================================================*/

#include "guixt.h"

/*======================================================================*/

extern void gui_color_delete(Tvgui*gui, TcolorEGS c)
{
    Tgui_main* main = gui->g_main;
    if (c.ulong==EGSCOLOR_NONE) return;
    XFreeColors(main->m_display,main->m_colorMap,&c.ulong,1,0);
}

extern TcolorEGS gui_color_dup(Tvgui*gui, TcolorEGS c)
{ 
    int r,g,b;
    gui_color_getRGB16(gui,c,&r,&g,&b);
    Tegs_color nc = gui_color_newByRGB16(gui,r,g,b); 
    return nc;
}

extern TcolorEGS gui_color_newByName(Tvgui* gui, const char* name)
{
    Tgui_main* main = gui->g_main;
    XColor color;
    if ( XAllocNamedColor(main->m_display, main->m_colorMap, name,
                          &color, &color) ) {
        TcolorEGS ret;
        ret.ulong = color.pixel;
        return ret;
    } else {
        return EGSCOLOR_none;
    }
}

extern TcolorEGS gui_color_newByRGB08(Tvgui* gui, int r, int g, int b)
{
    return gui_color_newByRGB16(gui, (r<<8)+r, (g<<8)+g, (b<<8)+b );
}

extern TcolorEGS gui_color_newByRGB16(Tvgui* gui, int r, int g, int b)
{
    Tgui_main* main = gui->g_main;
    XColor color;
    color.red  =r;
    color.green=g;
    color.blue =b;
    if ( XAllocColor(main->m_display, main->m_colorMap, &color) ) {
        TcolorEGS ret;
        ret.ulong = color.pixel;
        return ret;
    } else {
        return EGSCOLOR_none;
    }
}

extern void       gui_color_getRGB08(Tvgui* gui, TcolorEGS c,
                                   int*r, int*g, int*b)
{
    if ( c.ulong!=EGSCOLOR_NONE ) {
        gui_color_getRGB16(gui,c,r,g,b);
        *r >>= 8; *g >>= 8; *b >>= 8; 
    } else {
        *r=-1; *g=-1; *b=-1;
    }
}
extern void       gui_color_getRGB16(Tvgui* gui, TcolorEGS c,
                                   int*r, int*g, int*b)
{
    if ( c.ulong!=EGSCOLOR_NONE ) {
        Tgui_main* main = gui->g_main;
        XColor color;
        color.pixel = c.ulong;
        if ( XQueryColor(main->m_display, main->m_colorMap, &color) ) {
            *r=color.red;
            *g= color.green;
            *b= color.blue;
            return;
        }
    }
    *r=-1; *g=-1; *b=-1;
}

/*======================================================================*/
