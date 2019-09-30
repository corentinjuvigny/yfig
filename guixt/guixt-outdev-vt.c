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
 *      $Id: guixt-outdev-vt.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-outdev-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver       =*/
/*= based on fontconfig, xft and  truetype fonts).                     =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"
#include "pangoxft-config.h"

#include "vgui/vgui-outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/

#define STD_VARS \
    Tvgui_outdev* vod = dev; \
    Tvgui*        gui  = vod->v_gui

#define C_STD_VARS \
    Cvgui_outdev* vod = dev; \
    Tvgui*        gui  = vod->v_gui

/*======================================================================*/
/*= Constructors and destructor and top utlities of the output device. =*/

extern void gui_detachOutdev(Tvgui* gui, Tvgui_outdev* vod);

static void odvt_deleteXT(void*dev)
{
    STD_VARS;
    gui_detachOutdev(gui,vod);
    odvt_delete(vod);
}

static void odvt_clearPxm(const void*dev)
{
    C_STD_VARS;
    Coutdev* od = &vod->any;
    Ccolor* clr = vod->v_clearcolor;
    od_ldaf_setGCll((Toutdev*)od, OD_GC_SysFill, 0.0, DA_FS_Solid, clr, 0,0,0);
    od_fill_rectBB( od, OD_GC_SysFill, od->boxDes);
}

/*======================================================================*/
/*= Color handler                                                      =*/

static Tcolor* odvt_black(const void*dev)
{ C_STD_VARS; return color_newLazy( gui->vg_black ); }

static Tcolor* odvt_white(const void*dev)
{ C_STD_VARS; return color_newLazy( gui->vg_white ); }

/*======================================================================*/
/*= pixmap handler                                                     =*/

#include "guixt-outdev-vt-pxm.c"

/*======================================================================*/
/*= LDAF GC settings                                                   =*/

#include "guixt-outdev-vt-ldaf-gc.c"

/*======================================================================*/
/*= LDAF drawing                                                       =*/

#include "guixt-outdev-vt-ldaf.c"

/*======================================================================*/
/*= String handler.                                                    =*/

extern TfontEGS odvt_fontLoadPts (void*dev, int fi, double pts, double*ratio);
extern void     odvt_fontUnload  (void*dev, TfontEGS f);
extern void     odvt_setGCstr    (void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg);
extern void     odvt_extentsPix  (void*dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent, double* dx,     double* dy);
extern void     odvt_drawNW_Pix (void* dev, Tod_GCdef gcd, double x, double y,
                                 CstrBrief text);
extern void     odvt_drawNWr_Pix(void* dev, Tod_GCdef gcd, double x, double y,
                                 CstrBrief text, double a);

/*======================================================================*/
/*= The virtual table of XT output device.                             =*/

static Toutdev_vt vtableCvs;

static Toutdev_vt vtablePxm = {
    .topVT  = {
        .ident      = VGUI_ODID_Pixmap,
        .parentVT   = &outdev_vtable, 
        .byteSize   = sizeof( Tvgui_odpixmap ),
        .destructor = odvt_deleteXT,
        .clear      = odvt_clearPxm,
        .clearColor = odvt_clearColor,
    },
    .colorVT  = {
        .black = odvt_black,
        .white = odvt_white,
    },
    .pxmVT  = {
        .pxm_new        = odvt_pxm_new,
        .pxm_newFromBtm = odvt_pxm_createFromBtm,
        .pxm_delete     = odvt_pxm_delete,
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
        vtableCvs.topVT.clear    = odvt_clearCvs;
        vtableCvs.topVT.preDraw  = odvt_preDrawCvs;
        vtableCvs.topVT.postDraw = odvt_postDrawCvs;
    }
    return &vtableCvs;
}
extern void* gui_od_getVtablePxm() { return &vtablePxm; }

/*======================================================================*/
