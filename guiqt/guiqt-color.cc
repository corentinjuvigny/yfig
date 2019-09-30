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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: guiqt-color.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-color.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The color EGS driver.                                              =*/
/*=                                                                    =*/
/*= See the "tools/color.h" "vgui/vgui.h" headers for the type         =*/
/*= description, the function description and for the documentation.   =*/
/*======================================================================*/

#include "guiqt.h"

/*======================================================================*/

extern void gui_color_delete(Tvgui*gui, TcolorEGS c)
{
    delete &QtCOLOR(c);
}

extern TcolorEGS gui_color_dup(Tvgui*gui, TcolorEGS c)
{ QColor* qcolor = new QColor( QtCOLOR(c) ); 
  TcolorEGS ret; ret.ptr = qcolor; return ret; }

extern TcolorEGS gui_color_newByName(Tvgui* gui, const char* name)
{
    //Tgui_main* main = gui->g_main;
    QColor* qcolor = new QColor( name );
    if ( qcolor->isValid() ) {
        TcolorEGS ret; ret.ptr = qcolor;
        return ret;
    } else {
        delete qcolor;
        return EGSCOLOR_none;
    }
}

extern TcolorEGS gui_color_newByRGB08(Tvgui* gui, int r, int g, int b)
{
    QColor* qcolor = new QColor( r,g,b );
    if ( qcolor->isValid() ) {
        TcolorEGS ret; ret.ptr = qcolor;
        return ret;
    } else {
//        delete qcolor;
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
    if ( c.ulong!=EGSCOLOR_NONE ) {
        QtCOLOR(c).getRgb(r,g,b);
    } else {
        *r=-1; *g=-1; *b=-1;
    }
}

extern void       gui_color_getRGB16(Tvgui* gui, TcolorEGS c,
                                   int*r, int*g, int*b)
{
    if ( c.ulong!=EGSCOLOR_NONE ) {
        QtCOLOR(c).getRgb(r,g,b);
        *r <<= 8; *g <<= 8; *b <<= 8; 
    } else {
        *r=-1; *g=-1; *b=-1;
    }
}

/*======================================================================*/
