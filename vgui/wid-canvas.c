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
 *      $Id: wid-canvas.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-canvas.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales.                        =*/
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for the type description,        =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-canvasI.h"

#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui-efsm.h"
#include "vgui/vgui-outdev.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/wid-egs.h"

/*======================================================================*/
/*= abstract accessors                                                 =*/

extern Tegs_widget   wid_canvas_getWidEGS(Cwid_canvas*self)
{ return self->vgcm_widEGS; }

extern Tvgui_efsm* wid_canvas_getEFsm(Cwid_canvas*self)
{ return self->vgcm_efsm; }

extern Tvgui_outdev* wid_canvas_getVOD(Cwid_canvas*self)
{ return &self->vgcm_outdev->vv_vod; }

extern Toutdev* wid_canvas_getOD(Cwid_canvas*self)
{ return &self->vgcm_outdev->vv_od; }

/*======================================================================*/

static Twid_canvas* wid_canvas_newI(Tvgui_mw*vmw, CstrStatic wn,
          int mode,  int withscale,
          int daBW, int daWidth, int daHeight, int daHVSep)
{
    Tvgui*       gui = vmw->MW_gui;
    Twid_canvas* canvas = tool_malloc_0(sizeof(*canvas));

    canvas->vgcm_mw       = vmw ; 
    canvas->vgcm_wn       = wn ; 
    canvas->vgcm_mode     = mode ; 
    canvas->vgcm_scale    = withscale ; 
    canvas->vgcm_daBW     = daBW ; 
    canvas->vgcm_daWidth  = daWidth ; 
    canvas->vgcm_daHeight = daHeight ; 
    canvas->vgcm_daHVSep  = daHVSep ; 
    canvas->vgcm_drawingArea.canvas = canvas;

    canvas->cvs_hruler.pxm = PXMEGS_none;
    canvas->cvs_vruler.pxm = PXMEGS_none;

    return canvas;
}

extern Twid_canvas* widM_canvas_new(Tvgui_mw*vmw, int mode, int withscales,
                        int daBW, int daWidth, int daHeight, int daHVSep)
{ return wid_canvas_newI(vmw,wn_mainCanvas, mode, withscales,
                         daBW, daWidth, daHeight, daHVSep); }

extern Twid_canvas* widD_canvas_new(Tvgui_dw*d, int mode, int withscales,
                        int daBW, int daWidth, int daHeight, int daHVSep)
{   Tvgui_mw* vmw = vgui_dw_getMw(d);
    return wid_canvas_newI(vmw,wn_mainCanvas, mode, withscales,
                         daBW, daWidth, daHeight, daHVSep); }

extern void wid_canvas_dbf(Twid_canvas*self, Tegs_widget pw)
{
    self->vgcm_widEGS  = gui_canvas_buildTop(self,pw);

    if ( self->vgcm_scale ) {
        Twidcvs_ruler* ruler;
        int hruler_dy ;    // height of horizontal ruler
        int vruler_dx ;    // width  of vertical   ruler
        wid_canvas_rulerDxDy(self,&vruler_dx,&hruler_dy);
        
        ruler         = &self->cvs_hruler;
        ruler->canvas = self;
        ruler->widEGS = gui_canvas_buildScale(ruler, 1);
        gui_widegs_setDY(ruler->widEGS, hruler_dy);
        ruler->fixdim = hruler_dy;

        ruler         = &self->cvs_vruler;
        ruler->canvas = self;
        ruler->widEGS = gui_canvas_buildScale(ruler, 0);
        gui_widegs_setDX(ruler->widEGS, vruler_dx);
        ruler->fixdim = vruler_dx;
    }

    Tvgui_mw* vmw = self->vgcm_mw; 
    Tvgui*    gui = vmw->MW_gui;
    
    Twid_canvas_area*  area;
    area          = &self->vgcm_drawingArea;
    area->canvas  = self;
    area->widEGS  = gui_canvas_buildArea(area);
    area->outdev = vgui_odcanvas_new(gui,self);
    //vgui_device_chgClearColor(&area->device->vv_vdev, gui->vg_daBackground);

    gui_canvas_finishTop(self);
}

extern void wid_canvas_free(Twid_canvas* self)
{
    // drawing area
    vgui_odcanvas_delete( self->vgcm_outdev );
    vgui_efsm_delete( self->vgcm_automata );

    // rulers 
    Twidcvs_ruler* ruler;
    ruler = &self->cvs_hruler;
    if ( ruler!=0 ) {
        if ( PXMEGS_IsDefined( ruler->pxm) )
            gui_pixmap_free( self->cvs_gui, ruler->pxm );
        if ( ruler->pdev )
            vgui_odpixmap_delete( ruler->pdev ); 
    }
    ruler = &self->cvs_vruler;
    if ( ruler!=0 ) {
        if ( PXMEGS_IsDefined( ruler->pxm) )
            gui_pixmap_free( self->cvs_gui, ruler->pxm );
        if ( ruler->pdev )
            vgui_odpixmap_delete( ruler->pdev ); 
    }

    // canvas itself
    if ( self->cvs_grid )   grid_delete( self->cvs_grid );
    if ( self->cvs_geomcb ) free       ( self->cvs_geomcb );
    free( self );
}

extern void wid_canvas_clear (Twid_canvas* self)
{
    DBG1_CanvE("canvas=%p:",self);
    gui_canvas_clear( &self->cvs_drawingArea );
    DBG1_CanvL("canvas=%p:",self);
}

extern void wid_canvas_draw(Twid_canvas* self, void* ud)
{
    DBG1_CanvE("canvas=%p:",self);
    Toutdev* od = &self->vgcm_outdev->vv_dev;
    od_draw( od );
    DBG1_CanvL("canvas=%p:",self);
}

extern void wid_canvas_redraw(Twid_canvas* self, void* ud)
{
    DBG1_CanvE("canvas=%p:",self);
    Toutdev* od = &self->vgcm_outdev->vv_dev;
    od_redraw( od );
    DBG1_CanvL("canvas=%p:",self);
}

extern void wid_canvas_drawFsm(Twid_canvas* self)
{
    DBG1_CanvE("canvas=%p:",self);
    Tvgui_efsm* fsm = self->vgcm_efsm;
    if ( fsm )
        vgui_efsm_transition(fsm,EFSMIN_Redraw,0,0);
    DBG1_CanvL("canvas=%p:",self);
}

/*======================================================================*/
/* Quick constructors                                                  =*/

extern Twid_canvas* widM_canvas_nbf0(Tvgui_mw*vmw, Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep)
{
    Twid_canvas* self;
    self = widM_canvas_new(vmw,0,1,daBW,daWidth,daHeight,daHVSep);
    wid_canvas_dbf(self,pw);
    return self;
}

extern Twid_canvas* widM_canvas_nbf1(Tvgui_mw*vmw, Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep)
{
    Twid_canvas* self;
    self = widM_canvas_new(vmw,1,0,daBW,daWidth,daHeight,daHVSep);
    wid_canvas_dbf(self,pw);
    return self;
}

extern Twid_canvas* widD_canvas_nbf1(Tvgui_dw*d, //Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep);

extern Twid_canvas* widD_canvas_nbf2(Tvgui_dw*d, //Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep)
{
    Tegs_widget  pw = vgui_dw_getWidEGS(d);
    Twid_canvas* self;
    self = widD_canvas_new(d,2,0,daBW,daWidth,daHeight,daHVSep);
    wid_canvas_dbf(self,pw);
    return self;
}

/*======================================================================*/

#include "vgui/wid-canvas-geomcb.h"

Twidcvs_geomCB* wid_canvas_geomCB_new(Twid_canvas*self,
                    void  (*cb)(Twid_canvas*))
{
    Twidcvs_geomCB* ret = tool_malloc( sizeof(*ret) );

    ret->cb       = cb;
    ret->minXfu   = CVSGEO_Xmin;
    ret->maxXfu   = CVSGEO_Xmax;
    ret->minYfu   = CVSGEO_Ymin;
    ret->maxYfu   = CVSGEO_Ymax;
    ret->minZoom  = CVSGEO_Zmin;
    ret->maxZoom  = CVSGEO_Zmax;
    ret->stepZoom = CVSGEO_Zstep;

    self->cvs_geomcb = ret;
    return ret;
}

/*======================================================================*/
