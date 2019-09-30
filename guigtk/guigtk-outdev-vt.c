/*======================================================================
 *  Copyright 2018 Ivan Augé
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
 *  You should have received a copy of the GNU General Public License
 *  along with YFIG. If not, see <http://www.gnu.org/licenses/>.
**======================================================================
 *$Software: YFIG-GTK (Interface to GTK)
 *      $Id: guigtk-outdev-vt.c 118 2018-01-23 22:53:24Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-outdev-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver       =*/
/*= based on fontconfig, xft and  truetype fonts).                     =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guigtk.h"
#include "widgtk-canvas.h"

#include "vgui/vgui-outdev.h"

#include "outdev/outdev-vt.h"
#include "outdev/resources.h"
#include "outdev/ldaf-hl.h"
#include "outdev/style.h"

/*======================================================================*/

#define STD_VARS \
    Tvgui_outdev* vod = dev; \
    Tvgui*        gui = vod->v_gui 

#define C_STD_VARS \
    Cvgui_outdev* vod  = dev; \
    Tvgui*        gui  = vod->v_gui

#define TVARS_VodGui(dev) \
    Tvgui_outdev* vod = (Tvgui_outdev*)dev; \
    Tvgui*        gui = vod->v_gui

#define CVARS_VodGui(dev) \
    Cvgui_outdev* vod = (Cvgui_outdev*)dev; \
    Tvgui*        gui = vod->v_gui

/*======================================================================*/
/*= Constructors and destructor and top utlities of the output device. =*/

static void odvt_ds_stdStartPxm(const void*dev)
{
    Cvgui_odpixmap *pod = dev;
    GdkPixbuf*      pxm = PXMegs2gtk(pod->vv_pixmap);
    odpxm_stdSeq_start(pxm);    
}

static void odvt_ds_stdCommitPxm(const void*dev)
{
    Tvgui_odpixmap* pod = (Tvgui_odpixmap*)dev;
    GdkPixbuf*      pxm = PXMegs2gtk(pod->vv_pixmap);
    odpxm_stdSeq_commit(pxm);
    GdkPixbuf* px = gdk_pixbuf_get_from_surface(
            SURFegs2gtk(pod->vv_target),0, 0, pod->vv_target.dx, pod->vv_target.dy);
    gdk_pixbuf_copy_area(px, 0, 0, pod->vv_target.dx, pod->vv_target.dy,pxm, 0, 0);
    g_object_unref(G_OBJECT(px));
}


/*======================================================================*/
/*= Color handler                                                      =*/

static Tcolor* odvt_black(const void*dev)
{ C_STD_VARS; return color_newLazy( gui->vg_black ); }

static Tcolor* odvt_white(const void*dev)
{ C_STD_VARS; return color_newLazy( gui->vg_white ); }

/*======================================================================*/
/*= pixmap handler                                                     =*/

#include "guigtk-outdev-pixmap.c"

/*======================================================================*/
/*= LDAF GC settings                                                   =*/

#include "guigtk-outdev-vt-ldaf-gc.c"

/*======================================================================*/
/*= LDAF drawing                                                       =*/

#include "guigtk-outdev-vt-ldaf.c"

/*======================================================================*/
/*= LDAF fonts                                                         =*/

#include "guigtk-font.c"

/*======================================================================*/
/*= String handler.                                                    =*/

extern void     odvt_fontUnload  (void*dev, TfontEGS f);
extern void     odvt_setGCstr    (void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg);
extern void     odvt_extentsPix  (void*dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent, double* dx,     double* dy);
extern void     odvt_drawNW_Pix (void* dev, Tod_GCdef gcd, double x, double y,
                                 CstrBrief text);
extern void     odvt_drawNWr_Pix(void* dev, Tod_GCdef gcd, double x, double y,
                                 CstrBrief text, double a);

/*======================================================================*/
/*= The virtual table of output device.                                =*/

static Toutdev_vt vtableCvs;

static Toutdev_vt vtablePxm = {
    .topVT  = {
        .ident      = VGUI_ODID_Pixmap,
        .parentVT   = &outdev_vtable, 
        .byteSize   = sizeof( Tvgui_odpixmap ),
        .destructor = odvt_delete,
//      .clear      = odvt_clearPxm,   // use default
        .clearColor = odvt_clearColor,
        .ds_stdStart  = odvt_ds_stdStartPxm,
        .ds_stdCommit = odvt_ds_stdCommitPxm,

    },
    .colorVT  = {
        .black = odvt_black,
        .white = odvt_white,
    },
    .pxmVT  = {
        .pxm_new        = odvt_pxm_new,
        .pxm_newFromBtm = odvt_pxm_newFromBtm,
        .pxm_newFromImg = odvt_pxm_newFromImg,
        .pxm_delete     = odvt_pxm_delete,
        .pxm_draw       = odvt_pxm_draw,
        .img_prefFormat = -1,
        .img_load       = odvt_img_load,
        .img_free       = odvt_img_free,
        .img_transform  = odvt_img_transform,
      //.img_draw       = odvt_img_draw,
    },
    .ldafVT   = {
        .setGCso                  = odvt_setGCso,
        .setGCdd                  = odvt_setGCdd,
        .setGCfillColor           = odvt_setGCfillColor,
      //.setGCfillColorBrightness = odvt_setGCfillColorBrightness,
        .setGCfillTile            = odvt_setGCfillTile,
      //.setGCfillTileZ           = odvt_setGCfillTileZ,
        .draw_pointXY        = odvt_draw_pointXY,
        .draw_pointT         = odvt_draw_pointT,
        .draw_lineXY         = odvt_draw_lineXY,
      //.draw_lineT          = odvt_draw_lineT,
        .draw_mlineT         = odvt_draw_mlineT,
        .fill_mlineT         = odvt_fill_mlineT,
        .draw_rectangleXYWH  = odvt_draw_rectangleXYWH,
        .fill_rectangleXYWH  = odvt_fill_rectangleXYWH,
      //.draw_bezier  = vt_pdf_draw_bezier,
      //.fill_bezier  = vt_pdf_fill_bezier,
    },
    .strVT   = {
      //.initGC       = vt_pdf_initGCstr,
        .fontLoadPts  = odvt_fontLoadPts,
      //.fontLoadPix  = vt_pdf_fontLoadPix,
        .fontUnload   = odvt_fontUnload,
        .setGC        = odvt_setGCstr ,
      //.extentsPts   = odvt_extentsPts ,
        .extentsPix   = odvt_extentsPix ,
      //.lineInfoPix  = vt_pdf_lineInfoPix ,
        //.drawWL_Pix   = vt_pdf_drawWL_Pix,
        .drawNW_Pix   = odvt_drawNW_Pix,
        .drawNWr_Pix  = odvt_drawNWr_Pix,
    },
};

/*======================================================================*/
/*= Interface                                                          =*/

extern void* gui_od_getVtableCvs() {
    if ( vtableCvs.topVT.ident==0 ) {
        // not yet initialized
        vtableCvs                = vtablePxm;
        vtableCvs.topVT.ident    = VGUI_ODID_Canvas;  // ascii of CanVaS
        vtableCvs.topVT.byteSize = sizeof( Tvgui_odcanvas );
//      vtableCvs.topVT.clear    = odvt_clearCvs;     // use default
        vtableCvs.topVT.preDraw  = odvt_preDrawCvs;
        vtableCvs.topVT.postDraw = odvt_postDrawCvs;
        vtableCvs.topVT.ds_stdStart       = odvt_ds_stdStartCvs;
        vtableCvs.topVT.ds_stdCommit      = odvt_ds_stdCommitCvs;
        vtableCvs.topVT.ds_xorDrawStart   = odvt_ds_xorDrawStartCvs;
        vtableCvs.topVT.ds_xorDrawCommit  = odvt_ds_xorDrawCommitCvs;
        vtableCvs.topVT.ds_xorEraseStart  = odvt_ds_xorEraseStartCvs;
        vtableCvs.topVT.ds_xorEraseCommit = odvt_ds_xorEraseCommitCvs;
    }
    return &vtableCvs;
}
extern void* gui_od_getVtablePxm() { return &vtablePxm; }

/*======================================================================*/
