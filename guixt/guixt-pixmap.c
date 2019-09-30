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
 *      $Id: guixt-pixmap.c 285 2018-07-07 20:13:37Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-pixmap.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Pixmap and icon handler (EGS driver)                               =*/
/*=                                                                    =*/
/*= See the vgui/vgui-pixmap.h header for the type description and     =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-outdev.h"

#include "outdev/outdev-vt.h"

/*======================================================================*/
/*= VGUI interface for pixmap.                                         =*/

extern TpixmapEGS  gui_pixmap_create(Tvgui* gui, int wd, int ht)
{
    Pixmap pixmap;
    pixmap = XCreatePixmap(E_Display, 
                DefaultRootWindow(E_Display), wd, ht, E_Depth);
    return X112PXMEGS(pixmap);
}

extern TpixmapEGS  gui_pixmap_create_FromBitmap(Tvgui* gui,
                                   TcolorEGS fg, TcolorEGS bg,
                                   int width, int height, const char* bitmap)
{
    Pixmap pixmap;

    pixmap = XCreatePixmapFromBitmapData(E_Display, 
                        //XtWindow(main->m_topForm),
                         DefaultRootWindow(E_Display),
                        (char*)bitmap, width, height,
                        fg.ulong, bg.ulong, E_Depth);
    return X112PXMEGS(pixmap);
}

extern void gui_pixmap_free(Tvgui* gui, TpixmapEGS pixmap)
{
    if ( PXMEGS_IsNone(pixmap) ) return;
    Tgui_main* main = gui->g_main;
    XFreePixmap(main->m_display,PXMEGS2X11(pixmap));
}

extern void gui_pixmap_draw(
                Tvgui_outdev*dest, int xdest, int ydest,
                Tvgui_outdev*src,  int xsrc,  int ysrc,
                int dx, int dy)
{
    Tvgui* gui = dest->v_gui;
    GC gc = E_GCcache(OD_GC_SysDraw);
    XCopyArea(E_Display,
            VDEV_Drawable(src), VDEV_Drawable(dest), gc,
            xsrc,ysrc, dx,dy, xdest,ydest);
}

extern void gui_pixmap_drawPM(
                Tvgui_outdev*dest, int xdest, int ydest,
                TpixmapEGS  src,  int xsrc,  int ysrc,
                int dx, int dy)
{
    Tvgui* gui = dest->v_gui;
    GC gc = E_GCcache(OD_GC_SysDraw);
    int error = XCopyArea(E_Display,
            (Drawable)PXMEGS2X11(src), VDEV_Drawable(dest), gc,
            xsrc,ysrc, dx,dy, xdest,ydest);
    TEMP_asf_printf( error==0, "FAILURE (error=%d)",error);
}

extern void gui_pixmap_cpy(Tvgui*gui,
            TpixmapEGS des, int xdes, int ydes,
            TpixmapEGS src, int xsrc, int ysrc,
            int dx, int dy)
{
    GC gc = E_GCcache(OD_GC_SysDraw);
    XCopyArea(E_Display,
            (Drawable)PXMEGS2X11(src), (Drawable)PXMEGS2X11(des), gc,
            xsrc,ysrc, dx,dy, xdes,ydes);
}

/*======================================================================*/
/*= VGUI interface for image (cairo surface).                          =*/
/*= Note yet used in XT/XAW EGS driver                                 =*/

extern TimageEGS gui_image_init(Tvgui* gui, int wd, int ht) { return IMGEGS_none; }
extern void      gui_image_free(Tvgui* gui, TimageEGS img)      { }
extern void      gui_image_cpyPxm(Tvgui* gui, TimageEGS  des, TpixmapEGS src) { }

/*======================================================================*/
/*= creation and storage of miscleaneous pixmap used in the widgets    =*/

extern void gui_pixmaps_init(Tvgui* gui)
{
#define BITMAP_ARG3(base) base##_bits,base##_width,base##_height
    Tgui_main*  main = gui->g_main;
    Pixmap pixmap;
    TcolorEGS fg,bg;
    vgui_wn_getColorEGS(gui,wn_mainBtnOff,&fg,&bg);


    /* arrow for pull-down menus */
#   define menu_arrow_width 11
#   define menu_arrow_height 13
    static unsigned char menu_arrow_bits[] = {
      0xf8,0x00,0xd8,0x00,0xa8,0x00,0xd8,0x00,0xa8,0x00,0xd8,0x00,
      0xaf,0x07,0x55,0x05,0xaa,0x02,0x54,0x01,0xa8,0x00,0x50,0x00,
      0x20,0x00};
    main->m_pmMenuArrow  = XCreateBitmapFromData(E_Display,
        DefaultRootWindow(E_Display),BITMAP_ARG3(menu_arrow));
    pixmap = XCreatePixmapFromBitmapData(E_Display, 
                        DefaultRootWindow(E_Display),
                        BITMAP_ARG3(menu_arrow),
                        fg.ulong, bg.ulong, E_Depth);
    gui->g_pmMenuArrowDx = menu_arrow_width;
    gui->g_pmMenuArrowDy = menu_arrow_height;
    gui->g_pmMenuArrow   = X112PXMEGS(pixmap);

    /* arrow for cascade menu evgtries */
# define menu_cascade_arrow_width 10
# define menu_cascade_arrow_height 12
    static unsigned char menu_cascade_arrow_bits[] = {
       0x00, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x3e, 0x00, 0xfe, 0x00, 0xfe, 0x01,
       0xfe, 0x01, 0xfe, 0x00, 0x3e, 0x00, 0x0e, 0x00, 0x02, 0x00, 0x00, 0x00};
    main->m_pmMenuEntryArrow = XCreateBitmapFromData(main->m_display,
        DefaultRootWindow(main->m_display),BITMAP_ARG3(menu_cascade_arrow));

    /* arrows for spinner */
#define spinup_width 9
#define spinup_height 6
static char spinup_bits[] = {
   0x10, 0x00, 0x38, 0x00, 0x7c, 0x00, 0xfe, 0x00, 0xff, 0x01, 0x00, 0x00};
    main->m_pmSpinup = XCreateBitmapFromData(main->m_display,
        DefaultRootWindow(main->m_display),BITMAP_ARG3(spinup));
#define spindown_width 9
#define spindown_height 6
static char spindown_bits[] = {
   0x00, 0x00, 0xff, 0x01, 0xfe, 0x00, 0x7c, 0x00, 0x38, 0x00, 0x10, 0x00};
    main->m_pmSpindown = XCreateBitmapFromData(main->m_display,
        DefaultRootWindow(main->m_display),BITMAP_ARG3(spindown));

    /* arrows for folding and unfolding  */
#   define unfold_width 11
#   define unfold_height 13
    static unsigned char unfold_bits[] = {
      0xf8,0x00,0xd8,0x00,0xa8,0x00,0xd8,0x00,0xa8,0x00,0xd8,0x00,
      0xaf,0x07,0x55,0x05,0xaa,0x02,0x54,0x01,0xa8,0x00,0x50,0x00,
      0x20,0x00};
#   define fold_width 11
#   define fold_height 13
    static unsigned char fold_bits[] = {
      0x20,0x00,0x50,0x00,0xa8,0x00,0x54,0x01,0xaa,0x02,0x55,0x05,
      0xaf,0x07,0xd8,0x00,0xa8,0x00,0xd8,0x00,0xa8,0x00,0xd8,0x00,
      0xf8,0x00};
    //main->m_pmFold  = XCreateBitmapFromData(E_Display,
    //    DefaultRootWindow(E_Display),BITMAP_ARG3(fold));
    gui->g_pmFoldDx = fold_width;
    gui->g_pmFoldDy = fold_height;
    gui->g_pmFold   = (TpixmapEGS) XCreatePixmapFromBitmapData(
                        E_Display, DefaultRootWindow(E_Display),
                        BITMAP_ARG3(fold), fg.ulong, bg.ulong, E_Depth);
    //main->m_pmUnfold  = XCreateBitmapFromData(E_Display,
    //    DefaultRootWindow(E_Display),BITMAP_ARG3(unfold));
    gui->g_pmUnfoldDx = unfold_width;
    gui->g_pmUnfoldDy = unfold_height;
    gui->g_pmUnfold   = (TpixmapEGS) XCreatePixmapFromBitmapData(
                        E_Display, DefaultRootWindow(E_Display),
                        BITMAP_ARG3(unfold), fg.ulong, bg.ulong, E_Depth);

    /* bitmap of on/off checkmark box */
#   define checkmarkboxSm_width  13
#   define checkmarkboxSm_height 13
    static unsigned char checkmarkboxOnSm_bits[] = {
        0x00, 0x00, 0xfe, 0x0f, 0x02, 0x08, 0x02, 0x0b, 0x02, 0x0b, 0x82, 0x09,
        0xca, 0x09, 0xda, 0x08, 0xf2, 0x08, 0x62, 0x08, 0x02, 0x08, 0xfe, 0x0f,
        0x00, 0x00};
        
    static unsigned char checkmarkboxOffSm_bits[] = {
        0x00, 0x00, 0xfe, 0x0f, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08,
        0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0xfe, 0x0f,
        0x00, 0x00};
   main->m_bmCmBoxOnSm  = XCreateBitmapFromData(E_Display, DefaultRootWindow(E_Display), checkmarkboxOnSm_bits,  checkmarkboxSm_width, checkmarkboxSm_height);
   main->m_bmCmBoxOffSm = XCreateBitmapFromData(E_Display, DefaultRootWindow(E_Display), checkmarkboxOffSm_bits, checkmarkboxSm_width, checkmarkboxSm_height);
}

extern void gui_pixmaps_free(Tvgui* gui)
{
    Tgui_main*  main = gui->g_main;

    /* arrow for pull-down menus */
    gui_pixmap_free(gui,X112PXMEGS(main->m_pmMenuArrow));

    /* arrow for cascade menu evgtries */
    gui_pixmap_free(gui,X112PXMEGS(main->m_pmMenuEntryArrow));

    /* arrows for spinner */
    gui_pixmap_free(gui,X112PXMEGS(main->m_pmSpinup));
    gui_pixmap_free(gui,X112PXMEGS(main->m_pmSpindown));

    /* arrows for folding and unfolding  */

    /* small & large checkmark */
    gui_pixmap_free(gui,X112PXMEGS(main->m_bmCmBoxOffSm));
    gui_pixmap_free(gui,X112PXMEGS(main->m_bmCmBoxOnSm));
}

/*======================================================================*/
