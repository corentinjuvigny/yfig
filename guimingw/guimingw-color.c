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
 *$Software: YFIG-WIN32 (Interface to WIN32 EGS under MinGw)
 *      $Id: guimingw-color.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guimingw/guimingw-color.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The color EGS driver.                                              =*/
/*=                                                                    =*/
/*= See the "tools/color.h" "vgui/vgui.h" headers for the type         =*/
/*= description, the function description and for the documentation.   =*/
/*======================================================================*/

#include "guimingw.h"

/*======================================================================*/

#define T16to8(x) ( ((x)+128)/256 )
#define T8to16(x) ( (x)*256+(x) )

extern void      gui_color_delete    (Tvgui*gui, TcolorEGS c) { }
extern TcolorEGS gui_color_dup       (Tvgui*gui, TcolorEGS c) { return c; }
extern TcolorEGS gui_color_newByName (Tvgui* gui, const char* name)    { return EGSCOLOR_none; }
extern TcolorEGS gui_color_newByRGB08(Tvgui* gui, int r, int g, int b) { return CLRmingw2egs( RGB(r,g,b) ); }
extern TcolorEGS gui_color_newByRGB16(Tvgui* gui, int r, int g, int b) { return CLRmingw2egs( RGB(T16to8(r),T16to8(g),T16to8(b)) ); }
extern void      gui_color_getRGB08(Tvgui* gui, TcolorEGS _c, int*r, int*g, int*b)
{
    COLORREF c = CLRegs2mingw(_c);
    *r = GetRValue(c);
    *g = GetGValue(c);
    *b = GetBValue(c);
}
extern void      gui_color_getRGB16(Tvgui* gui, TcolorEGS _c, int*r, int*g, int*b)
{
    COLORREF c = CLRegs2mingw(_c);
    *r = T8to16( GetRValue(c) );
    *g = T8to16( GetGValue(c) );
    *b = T8to16( GetBValue(c) );
}

/*======================================================================*/
