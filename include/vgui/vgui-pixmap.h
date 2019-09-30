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
 *      $Id: vgui-pixmap.h 285 2018-07-07 20:13:37Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-pixmap.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Pixmap and icon handler                                            =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_PIXMAP_H
#define FILE_VGUI_PIXMAP_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================*/
/*= Icons                                                              =*/

#define VGUI_ICON_KIND_Bitmap   1
#define VGUI_ICON_KIND_File     2
#define VGUI_ICON_KIND_Text     3

typedef struct _Tvgui_icon {
    int    vgicon_kind;     // kind of input data see VGUI_ICON_KIND_xxx
    int	   vgicon_width,    // the geometry of expected pixmap and
           vgicon_height;   // of vgicon_data when VGUI_ICON_KIND_Bitmap
    char*  vgicon_data;     // data for creating the pixmap
} Tvgui_icon;

extern Tvgui_icon* vgui_icon_newBitmap(int width, int height,
                                       const char* bits);
extern void        vgui_icon_free     (Tvgui_icon*icon);

#define VGUI_BTM_DataSize(dx,dy) ( (((dx)+7)/8) * (dy) )

extern TpixmapEGS vgui_pixmap_create(Tvgui*gui, Ccolor*fg, Ccolor*bg,
                                      const Tvgui_icon* icon);

#if 0
extern Tvgui_icon* vgui_icon_newLabel (int width, int height,
                                       const char* label);
extern Tvgui_icon* vgui_icon_dup      (Cvgui_icon* icon);
extern Tvgui_icon* vgui_icon_scale    (Tvgui_icon*icon, double f);

#endif

/*======================================================================*/
/*= Interface to EGS                                                   =*/

extern TpixmapEGS    gui_pixmap_create(Tvgui* gui, int wd, int ht);

extern TpixmapEGS    gui_pixmap_create_FromBitmap(Tvgui* gui,
                                  TcolorEGS fg, TcolorEGS bg,
                                  int width, int height, const char* bits);

extern void          gui_pixmap_free(Tvgui* gui, TpixmapEGS pixmap);

extern void          gui_pixmap_draw(Tvgui_outdev*des, int xdes, int ydes,
                                     Tvgui_outdev*src, int xsrc, int ysrc,
                                     int dx, int dy);
                                      
extern void          gui_pixmap_drawPM(
                                     Tvgui_outdev*des, int xdes, int ydes,
                                     TpixmapEGS   src, int xsrc, int ysrc,
                                     int dx, int dy);
extern void          gui_pixmap_drawPMgc(
                                     Tvgui_outdev*des, Tod_GCdef gcd,
                                     int xdes, int ydes, TpixmapEGS src,
                                     int xsrc, int ysrc, int dx, int dy);
extern void          gui_pixmap_cpy( Tvgui*      gui,
                                     TpixmapEGS  des, int xdes, int ydes,
                                     TpixmapEGS  src, int xsrc, int ysrc,
                                     int dx, int dy);

extern TimageEGS    gui_image_init  (Tvgui* gui, int wd, int ht);
extern void         gui_image_free  (Tvgui* gui, TimageEGS img);
extern void         gui_image_cpyPxm(Tvgui* gui, TimageEGS des, TpixmapEGS src);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_PIXMAP_H
/*======================================================================*/
