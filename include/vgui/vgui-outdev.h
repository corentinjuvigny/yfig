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
 *      $Id: vgui-outdev.h 182 2018-06-07 14:32:01Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-outdev.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing into a Twid_canvas and into a pixmap.        =*/
/*=                                                                    =*/
/*= This header defines the types:                                     =*/
/*=  - Tvgui_outdev: It is the top class.                              =*/
/*=          It gives an abstract view of an output device and         =*/
/*=          it is the first parameters of all the drawing routines.   =*/
/*=  - Tvgui_odcanvas: It is an implementation of Tvgui_device that    =*/
/*=          is attached to a canvas widget and so the drawing         =*/
/*=          routines draw into the window of the canvas widget.       =*/
/*=  - Tvgui_odpixmap: It is an implementation of Tvgui_device that    =*/
/*=          is attached to a pixmap and so the drawing routines draw  =*/
/*=          into this pixmap.                                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_OUTDEV_H
#define FILE_VGUI_OUTDEV_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "outdev/outdev.h"
#include "tools/bitmap-types.h"

/*======================================================================*/

struct _Tvgui_outdev {
    Toutdev      any;
#define VCONV_D_S2D(dev,d)  CONV_D_S2D(&((dev)->any),d) 
#define VCONV_X_S2D(dev,x)  CONV_X_S2D(&((dev)->any),x) 
#define VCONV_Y_S2D(dev,y)  CONV_Y_S2D(&((dev)->any),y) 
#define VCONV_D_D2S(dev,d)  CONV_D_D2S(&((dev)->any),d) 
#define VCONV_X_D2S(dev,x)  CONV_X_D2S(&((dev)->any),x) 
#define VCONV_Y_D2S(dev,y)  CONV_Y_D2S(&((dev)->any),y) 
    Tvgui*       gui;
    Tcolor*      clearcolor;  // color to set when device is cleared
    // caution: Only one among pixmap, widEGS must be not null.
    //          They are exclusively null.
    TpixmapEGS   pixmap;      // output pixmap
    Tegs_widget  widEGS;      // output widget
};
#define v_dev             any
#define v_od              any
#define v_boxDS           any.boxSrc // obsolete
#define v_boxSrc          any.boxSrc
#define v_boxPIX          any.boxDes // obsolete
#define v_boxDes          any.boxDes
#define v_ppiDes          any.ppiDes
#define v_homothetyDS2PIX any.homothetySrc2Des
#define v_clear           any.clear
#define v_draw            any.draw
#define v_esrc            any.esrc
#define v_userdata        any.userdata
#define v_gui             gui // v_canvas->vgcm_gui
#define v_clearcolor      clearcolor
#define v_cvsWidEGS       widEGS
#define v_pixmap          pixmap
//#define v_widEGS          widEGS
#define v_vtable          any.vtable
#define v_ident           v_vtable->topVT.ident

#define VGUI_ODID_Canvas 0x00435653  // ascii of CanVaS
#define VGUI_ODID_Pixmap 0x0050584D  // ascii of PXM

/**
 * The vgui_outdev_chgClearColor function sets the clear color of
 * the dev output device to the c color.
**/
extern void vgui_outdev_chgClearColor(Tvgui_outdev*dev, Ccolor*c);

extern void vgui_outdev_free(Tvgui_outdev*vod);

/*======================================================================*/

struct _Tvgui_odcanvas {
    Tvgui_outdev any;
    Twid_canvas* canvas;    // the canvas widget
};
#define vv_vod             any
#define vv_od              any.v_od
#define vv_vdev            any
#define vv_dev             any.v_dev
#define vv_boxDS           any.v_dev.boxSrc
#define vv_boxSrc          any.v_dev.boxSrc
#define vv_boxPIX          any.v_dev.boxDes
#define vv_boxDes          any.v_dev.boxDes
#define vv_homothetyDS2PIX any.v_dev.homothetySrc2Des
#define vv_clear           any.v_dev.clear
#define vv_draw            any.v_dev.draw
#define vv_esrc            any.v_dev.esrc
#define vv_userdata        any.v_dev.userdata
#define vv_gui             any.v_gui
#define vv_clearcolor      any.v_clearcolor
#define vv_pixmap          any.v_pixmap
#define vv_cvsWidEGS       any.v_cvsWidEGS
#define vv_canvas          canvas

extern Tvgui_odcanvas*
            vgui_odcanvas_new    (Tvgui*gui, Twid_canvas* w);
extern void vgui_odcanvas_delete (Tvgui_odcanvas* dev);

/*======================================================================*/

struct _Tvgui_odpixmap {
    Tvgui_outdev any;
    TimageEGS    target;  // EGS structure where EGS really draws.
                          // If EGS can draw directely into a pixmap,
                          // it is not used and target.egs is set to IMGEGS_none.
                          // (For GTK it's cairo_surface_t)
} ;
#define vv_dx(d)     Box_dx(d->vv_boxPIX)
#define vv_dy(d)     Box_dy(d->vv_boxPIX)
#define vv_target    target
#define vv_any       any

/**
 * The vgui_odpixmap_new function creates an output device and
 * attaches to it a pixmap of dx pixel width and dy pixel height.
 * The source and destination boxes are set to the pixmap size and
 * the homothetie ratio is set to 1.
 * The clear function of device is set and the draw function is
 * set to the null function.
 * The output device resources is set to gui->vg_odResources.
 * Furthermore all the pixmap pixels are set to the c color. 
 *
 * The vgui_odpixmap_newRes function is similar to the vgui_odpixmap_new
 * function except that the resources of device is set res.
 *
 * Return:
 *  The vgui_odpixmap_new returns the created output device.
**/

extern Tvgui_odpixmap* vgui_odpixmap_newRes  (Tvgui*gui, Ccolor* c, 
                                              int dx, int dy, void* ud,
                                              TodResources* res);
extern Tvgui_odpixmap* vgui_odpixmap_new     (Tvgui*gui, Ccolor* c, 
                                              int dx, int dy, void* ud);
extern void            vgui_odpixmap_delete (Tvgui_odpixmap* dev);
extern TpixmapEGS      vgui_odpixmap_delete2(Tvgui_odpixmap* dev);

/**
 * The vgui_odpixmap_getPxm function returns the pixmap of the
 * dev device. The output device is reinitialized with a new pixmap
 * that is set to the background color.
 *
 * The vgui_odpixmap_dupPxm function duplicates the pixmap of the
 * dev device and returns it.
 *
 * The vgui_odpixmap_setPxm function frees the current pixmap of
 * the dev device and replaces it by pxm.
 *
 * The vgui_odpixmap_cpyPxm function copies the pxm pixmap into
 * the pixmap of the dev device.
 *
 * Insure:
 *   The pxm pixmap nust have the same size than the device pixmap. 
**/
extern TpixmapEGS vgui_odpixmap_getPxm(Tvgui_odpixmap* dev);
extern TpixmapEGS vgui_odpixmap_dupPxm(Tvgui_odpixmap* dev);
extern void       vgui_odpixmap_setPxm(Tvgui_odpixmap* dev,
                                       TpixmapEGS      pxm);
extern void       vgui_odpixmap_cpyPxm(Tvgui_odpixmap* dev,
                                       TpixmapEGS      pxm);

/*======================================================================*/
/*= Output device drivers.                                             =*/

extern void    odvt_delete     (      void*dev);
extern void    odvt_clearCvs   (const void*dev);
extern Ccolor* odvt_clearColor (const void*dev);
extern void    odvt_preDrawCvs (const void*dev);
extern void    odvt_postDrawCvs(const void*dev);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_OUTDEV_H
/*======================================================================*/

