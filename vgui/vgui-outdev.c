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
 *      $Id: vgui-outdev.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-outdev.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing into a Twid_canvas and into a pixmap.        =*/
/*=                                                                    =*/
/*= See the vgui/vgui-oudev.h header for the type description, the     =*/
/*= function description and the documentation.                        =*/
/*======================================================================*/

/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-egs.h"

#include "outdev/pxm-hl.h"
#include "tools/color.h"

/*======================================================================*/

extern void od_outdev_chgClearColor(Tvgui_outdev*dev, Ccolor*c)
{ color_chgLazy(&dev->v_clearcolor,c); }

extern void vgui_outdev_free(Tvgui_outdev*vod)
{
    color_delete( vod->clearcolor );
    if ( PXMEGS_IsDefined(vod->v_pixmap) )
        gui_pixmap_free( vod->v_gui, vod->v_pixmap );
}

/*======================================================================*/

extern Tvgui_odcanvas*
            vgui_odcanvas_new    (Tvgui*gui, Twid_canvas* cvs)
{
    TboxDble box;
    Tegs_widget widEGS = cvs->cvs_drawingArea.widEGS;
    Box_x(box)  = 0;
    Box_y(box)  = 0;
    Box_dx(box) = vgui_widegs_getDX(widEGS);
    Box_dy(box) = vgui_widegs_getDY(widEGS);

    Toutdev* od = od_new0r(
            gui->vg_odVtableCvs,
            VG_odPPI(gui),
            &box,
            gui->vg_odResources
    );
    od->parDes   = gui;

    Tvgui_odcanvas* self = (Tvgui_odcanvas*)od;
    od_outdev_chgClearColor(&self->vv_vdev,gui->vg_daBackground);
    self->vv_gui       = gui; 
    self->vv_cvsWidEGS = widEGS;
    self->vv_pixmap    = PXMEGS_none;
    self->vv_canvas    = cvs;
    
    //od_clear(od);

    return self;
}

extern void vgui_odcanvas_delete (Tvgui_odcanvas* dev)
{ Tvgui_outdev* od=&dev->any; od_delete( &od->any ); }

/*======================================================================*/
extern Tvgui_odpixmap* vgui_odpixmap_newRes(Tvgui* gui,
        Ccolor*c ,int width, int height, void* ud, TodResources* res)

{
    TboxDble box;
    Box_x(box) = 0;       Box_y(box) = 0;
    Box_dx(box)= width;   Box_dy(box)= height;

    Toutdev* od = od_new0r(
            gui->vg_odVtablePxm,
            VG_odPPI(gui),
            &box,
            res
    );
    od->parDes   = gui;
    od->userdata = ud;

    Tvgui_odpixmap* self = (Tvgui_odpixmap*)od;
    od_outdev_chgClearColor(&self->vv_vdev,c);
    self->vv_gui    = gui; 
    self->vv_pixmap = od_pxm_new(od,width,height); 
    self->vv_target = gui_image_init(gui,width,height);
    
    od_attach(od, NULL, VG_odPPI(gui), NULL);
    return self;
}
extern Tvgui_odpixmap* vgui_odpixmap_new  (Tvgui* gui,
        Ccolor*c ,int width, int height, void* ud)
{
    Tvgui_odpixmap* self;
    self =  vgui_odpixmap_newRes(gui,c,width,height,ud, gui->vg_odResources);
    od_clear(&self->vv_od);
    return self;
}

extern void             vgui_odpixmap_delete (Tvgui_odpixmap* dev)
{
    Tvgui*        gui = dev->vv_gui;
    Tvgui_outdev* od  =&dev->any;
    if ( PXMEGS_IsDefined(dev->vv_pixmap) ) {
        gui_pixmap_free(gui,dev->vv_pixmap );
        dev->vv_pixmap = PXMEGS_none;
    }
    if ( IMGEGS_IsDefined(dev->vv_target) ) {
        gui_image_free(gui,dev->vv_target );
        dev->vv_target = IMGEGS_none;
    }
    od_delete( &od->any );
}

extern TpixmapEGS      vgui_odpixmap_delete2(Tvgui_odpixmap* dev)
{
    TpixmapEGS pixmap = dev->vv_pixmap;
    dev->vv_pixmap = PXMEGS_none;
    vgui_odpixmap_delete (dev);
    return pixmap;
}

extern TpixmapEGS vgui_odpixmap_getPxm(Tvgui_odpixmap* dev)
{
    Toutdev*   od  = &dev->vv_dev;
    TpixmapEGS pxm = dev->vv_pixmap;
    dev->vv_pixmap = od_pxm_new( od, Box_dx(od->boxDes),  Box_dy(od->boxDes));
    od_clear( od );
    return pxm;
}

extern TpixmapEGS vgui_odpixmap_dupPxm(Tvgui_odpixmap* dev)
{
    Tvgui*     gui = dev->vv_gui;
    Toutdev*   od  = &dev->vv_dev;
    int        dx  = Box_dx(od->boxDes);
    int        dy  = Box_dy(od->boxDes);
    TpixmapEGS pxm = od_pxm_new( od, dx,  dy);
    gui_pixmap_cpy(gui, pxm,0,0, dev->vv_pixmap,0,0, dx,dy);
    return pxm;
}

extern void             vgui_odpixmap_setPxm(Tvgui_odpixmap* dev,
                                             TpixmapEGS      pxm)
{
#if 1
    PERM_fatal_NYI();
#else
    Tvgui* gui = dev->vv_gui;
    gui_pixmap_free(gui,dev->vv_pixmap);
    dev->vv_pixmap = pxm;
#endif
}

extern void             vgui_odpixmap_cpyPxm(Tvgui_odpixmap* dev,
                                             TpixmapEGS     pxm)
{
    Tvgui*     gui = dev->vv_gui;
    Toutdev*   od  = &dev->vv_dev;
    int        dx  = Box_dx(od->boxDes);
    int        dy  = Box_dy(od->boxDes);
    gui_pixmap_cpy  (gui,dev->vv_pixmap,0,0, pxm,0,0, dx,dy);
    gui_image_cpyPxm(gui,dev->vv_target,pxm);
}

/*======================================================================*/
/*= Output device drivers.                                             =*/

extern void    odvt_delete     ( void*dev)
{ Tvgui_outdev*vod=dev; vgui_outdev_free(vod); }

extern void    odvt_clearCvs   (const void*dev)
{ 
    Cvgui_odcanvas* cvs = (Cvgui_odcanvas*)dev;
    wid_canvas_clear( cvs->vv_canvas );
}

extern Ccolor* odvt_clearColor (const void*dev)
{ Cvgui_outdev*vod=dev; return vod->v_clearcolor; }

extern void    odvt_preDrawCvs (const void*dev)
{ 
    Cvgui_odcanvas* cvs = (Cvgui_odcanvas*)dev;
    wid_canvas_drawGrid( cvs->vv_canvas );
    wid_canvas_drawAxis( cvs->vv_canvas );
}

extern void    odvt_postDrawCvs(const void*dev)
{ 
    Cvgui_odcanvas* cvs = (Cvgui_odcanvas*)dev;
    wid_canvas_drawFsm( cvs->vv_canvas );
}

/*======================================================================*/
