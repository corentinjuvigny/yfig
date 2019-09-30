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
 *      $Id: guiqt-outdev-vt.cc 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-vt.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver       =*/
/*= based on fontconfig, xft and  truetype fonts).                     =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guiqt.h"

#include "tools/font-types.h"
#include "outdev/outdev-vt.h"

/*======================================================================*/

#define TVARS_VodGui(dev) \
    Tvgui_outdev* vod = (Tvgui_outdev*)dev; \
    Tvgui*        gui = vod->v_gui

#define CVARS_VodGui(dev) \
    Cvgui_outdev* vod = (Cvgui_outdev*)dev; \
    Tvgui*        gui = vod->v_gui

#define TVARS_VodGuiOd(dev) TVARS_VodGui(dev); Toutdev* od = (Toutdev*)dev;
#define CVARS_VodGuiOd(dev) CVARS_VodGui(dev); Coutdev* od = (Coutdev*)dev;

/*======================================================================*/
/*= Constructors and destructor and top utlities of the output device. =*/

// we use such as the default odvt_... of vgui-outdev.h 

/*======================================================================*/
/*= Color handler                                                      =*/

static Tcolor* odvt_black(const void*od)
{ CVARS_VodGui(od); return color_newLazy( gui->vg_black ); }

static Tcolor* odvt_white(const void*od)
{ CVARS_VodGui(od); return color_newLazy( gui->vg_white ); }

/*======================================================================*/
/*= pixmap handler                                                     =*/

#include "guiqt-outdev-pixmap.cc"

/*======================================================================*/
/*= LDAF GC settings                                                   =*/

#include "guiqt-outdev-ldaf-gc.cc"

/*======================================================================*/
/*= LDAF drawing                                                       =*/

#include "guiqt-outdev-ldaf.cc"

/*======================================================================*/
/*= String handler.                                                    =*/

#include "guiqt-outdev-font.cc"

/*======================================================================*/
/*= The virtual table of XT output device.                             =*/

static Toutdev_vt vtableCvs;

static Toutdev_vt vtablePxm = {
    topVT  : {
        ident      : VGUI_ODID_Pixmap,
        parentVT   : &outdev_vtable, 
        byteSize   : sizeof( Tvgui_odpixmap ),
        destructor : odvt_delete,
        clear      : 0, // odvt_clearPxm,
        clearColor : odvt_clearColor,
    },
    colorVT  : {
        black : odvt_black,
        white : odvt_white,
    },
    pxmVT  : {
        pxm_new        : odvt_pxm_new,
        pxm_newFromBtm : odvt_pxm_createFromBtm,
        pxm_newFromImg : odvt_pxm_newFromImg,
        pxm_delete     : odvt_pxm_delete,
        pxm_draw       : odvt_pxm_draw,
        img_prefFormat : -1,
        img_load       : odvt_img_load,
        img_free       : odvt_img_free,
        img_transform  : odvt_img_transform,
    },
    ldafVT   : {
        initGC                   : 0, // odvt_initGC,
        setGCso                  : odvt_setGCso,
        setGCdd                  : odvt_setGCdd,
        setGCfillColor           : odvt_setGCfillColor,
        setGCfillColorBrightness : 0, // odvt_setGCfillColorBrightness,
        setGCfillTile            : odvt_setGCfillTile,
        SetGCfillTileZ           : 0, // odvt_setGCfillTileZ,
        draw_pointXY        : odvt_draw_pointXY,
        draw_pointT         : odvt_draw_pointT,
        draw_lineXY         : odvt_draw_lineXY,
        draw_lineT          : 0, // odvt_draw_lineT,
        draw_mlineT         : odvt_draw_mlineT,
        fill_mlineT         : odvt_fill_mlineT,
        draw_rectangleXYWH  : odvt_draw_rectangleXYWH,
        fill_rectangleXYWH  : odvt_fill_rectangleXYWH,
      //draw_bezier         : odvt_draw_bezier,
      //fill_bezier         : odvt_fill_bezier,
    },
    strVT   : {
        initGC       : 0, //vt_pdf_initGCstr,
        fontLoadPts  : odvt_fontLoadPts,
        fontLoadPix  : 0,
        fontUnload   : odvt_fontUnload,
        setGC        : odvt_setGCstr,
        extentsPts   : 0,
        extentsPix   : odvt_extentsPix,
        lineInfoPix  : 0,
        drawWL_Pix   : odvt_drawWL_Pix,
        drawNW_Pix   : 0, // odvt_drawNW_Pix,
        drawNWr_Pix  : 0, // odvt_drawNWr_Pix,
        drawWLr_Pix  : odvt_drawWLr_Pix,
    },
};

/*======================================================================*/
/*= Interface                                                          =*/

extern void gui_od_init(Tvgui*gui) {}
extern void gui_od_free(Tvgui*gui) {}

extern void* gui_od_getVtableCvs() {
    if ( vtableCvs.topVT.ident==0 ) {
        // not yet initialized
        vtableCvs                = vtablePxm;
        vtableCvs.topVT.ident    = VGUI_ODID_Canvas;  // ascii of CanVaS
        vtableCvs.topVT.byteSize = sizeof( Tvgui_odcanvas );
        vtableCvs.topVT.clear    = odvt_clearCvs;
        vtableCvs.topVT.preDraw  = odvt_preDrawCvs;
        vtableCvs.topVT.postDraw = odvt_postDrawCvs;
    }
    return &vtableCvs;
}
extern void* gui_od_getVtablePxm() { return &vtablePxm; }

/*======================================================================*/
