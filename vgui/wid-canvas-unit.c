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
 *      $Id: wid-canvas-unit.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-canvas-unit.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales.                        =*/
/*= Implementation of ruler, grid and magnetic grid.                   =*/    
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for the type description,        =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "wid-canvasI.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-egs.h"

#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"

#define SCALE_FontId gui->g_devFid
#define SCALE_FontPt gui->g_devPtsStd
#define SCALE_MajorLen 15 //10
#define SCALE_MinorLen  7
#define SCALE_TopSep    2
#define SCALE_LeftSep   2
#define SCALE_MidSep    2

/*======================================================================*/
/*= Some converters                                                    =*/

Inline double wid_canvas_getFu2Pix(Cvgui_odcanvas* vod)
{
    Coutdev* od =  &vod->vv_od;
    return od->ppiDes / LUNIT_FuDefault * od_getUsrZoom(od);
}

Inline double wid_canvas_getSrc2Fu(Cvgui_odcanvas* vod, double v)
{ return v / OD_ppiSrc(&vod->vv_od) * LUNIT_FuDefault; }

Inline double wid_canvas_getFu2Src(Cvgui_odcanvas* vod, double v)
{ return v * OD_ppiSrc(&vod->vv_od) / LUNIT_FuDefault; }

/*======================================================================*/
/*= Interface                                                          =*/

extern void wid_canvas_axisEnable(Twid_canvas*self, int on) { self->cvs_drawAxis=on; }

extern void wid_canvas_unitSet(Twid_canvas* self, CuserUnit* uu)
{
    if ( self->cvs_grid==0 )
        self->cvs_grid = grid_new(uu->uu_id,uu->uu_factor,uu->uu_sub);
    else
        grid_init(self->cvs_grid,uu->uu_id,uu->uu_factor,uu->uu_sub);
    grid_updateRuler( self->cvs_grid, wid_canvas_getFu2Pix(self->vgcm_outdev) );
    wid_canvas_drawRulers(self);
}

extern void wid_canvas_unitSetGrid (Cwid_canvas*self, double stn)
{
    if ( self->cvs_grid==0 ) return;
    grid_updateGrid( self->cvs_grid, wid_canvas_getFu2Pix(self->vgcm_outdev), stn);
}

extern void wid_canvas_unitSetMagnet(Cwid_canvas*self, double stn)
{
    if ( self->cvs_grid==0 ) return;
    grid_updateMagnet( self->cvs_grid, stn);
}

extern void wid_canvas_unitEnableGrid (Cwid_canvas*self, int on)
{
    if ( self->cvs_grid==0 ) return;
    grid_enableGrid(self->cvs_grid,on);
}
extern void wid_canvas_unitEnableMagnet(Cwid_canvas*self, int on)
{
    if ( self->cvs_grid==0 ) return;
    grid_enableMagnet(self->cvs_grid,on);
}

extern void wid_canvas_unitDevGeomChanged(Cwid_canvas*self);

extern int wid_canvas_setZoomNW (Cwid_canvas*self, double zf);
extern int wid_canvas_setZoomCE (Cwid_canvas*self, double zf)
{
    Toutdev* od = &self->vgcm_outdev->vv_od;
    int ret = od_setUsrZoom_CENTER(od,zf);
    if ( ret ) {
        grid_updateRuler( self->cvs_grid, wid_canvas_getFu2Pix(self->vgcm_outdev) );
        wid_canvas_drawRulers(self);
    }
    return ret;
}

/*======================================================================*/
/*= Interface to wid-canvas.c (Private canvas function)                =*/

// writes in buf the label correspondind to fu and returns buf.
static char*     drawRuler_getLabel(char*buf,Cgrid*grid,double fu);
static TsintSet* drawRuler_convPts2Pix(TdbleSet*pts, Cvgui_odcanvas* od,int x);
// lblstart,lblstep: ticks with labels (labelstart+k*labelstep)
// lblpix:           len of label in pixel.
// x:                1:horizontal ; 0:vertical 
// return set:       the pts points converted into pixel
static TsintSet* drawRuler_mainWork( int*lblstart,int*lblstep,
        Cvgui_odcanvas* od, TdbleSet*pts, int lblpix, int x);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= GC initialization                                                  =*/

static void wid_canvas_setGC_axis(Tvgui* gui, Toutdev*od)
{
    Tcolor*    fg = gui->vg_axisLineForeground;
    double     lw = 0;
    TjoinStyle js = DA_JS_Round;
    TcapStyle  cs = DA_CS_Butt;
    od_ldaf_setGCsoP(od, OD_GC_SysDraw,lw,js,cs,fg);
}

static void wid_canvas_setGC_grid(Tvgui* gui, Toutdev*od)
{
    Tcolor*    fg = gui->vg_gridLineForeground;
    double     lw = 0;
    TjoinStyle js = DA_JS_Round;
    TcapStyle  cs = DA_CS_Butt;
    od_ldaf_setGCsoP(od, OD_GC_SysDraw,lw,js,cs,fg);
}

// return the bg color of rulers
static Tcolor* wid_canvas_setGC_rulers(Tvgui* gui, Toutdev*od)
{
    Tcolor*    fg = gui->vg_black;
    Tcolor*    bg = gui->vg_white;
    double     lw = 0;
    TjoinStyle js = DA_JS_Round;
    TcapStyle  cs = DA_CS_Butt;
    od_ldaf_setGCsoP(od, OD_GC_SysDraw,lw,js,cs,fg);

    int    fontid   = SCALE_FontId;
    double fontsize = SCALE_FontPt;
    od_str_setGCP(od,OD_GC_SysDraw,fontid,fontsize,NULL);

    return bg;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= axis management                                                    =*/
 
extern void wid_canvas_drawAxis(Cwid_canvas*self)
{
    Tvgui*          gui   = self->vgcm_gui;
    Tvgui_odcanvas* odcvs = self->vgcm_outdev;
    Toutdev*        od    = &odcvs->vv_od;

    if ( self->cvs_drawAxis==0 ) return;

    Tdble2 pt1 = tool_boxdble_pt1( &od->boxSrc );
    Tdble2 pt3 = tool_boxdble_pt3( &od->boxSrc );

    if ( (pt1.y<=0 && 0<=pt3.y) || (pt1.x<=0 && 0<=pt3.x) ) {
        wid_canvas_setGC_axis(gui,od);
        if ( pt1.y<=0 && 0<=pt3.y ) // draw X axis
            od_draw_lineXY(od,OD_GC_SysDraw,0,pt1.y,0,pt3.y);
        if ( pt1.x<=0 && 0<=pt3.x ) // draw Y axis
            od_draw_lineXY(od,OD_GC_SysDraw,pt1.x,0,pt3.x,0);
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= grid management                                                    =*/
 
extern void wid_canvas_drawGrid(Cwid_canvas*self)
{
    Tvgui*          gui   = self->vgcm_gui;
    Tvgui_odcanvas* odcvs = self->vgcm_outdev;
    Toutdev*        od    = &odcvs->vv_od;
    Cgrid*          grid = self->cvs_grid;
    double x,y;
    int i;
    TdbleSet* pts;

    if ( grid==0 ) return;
    if ( grid_isThereGridToDraw(grid)==0 ) return;

    wid_canvas_setGC_grid( gui, od );

    // in Source unit
    double x1   = Box_nwx(od->boxSrc) ;
    double y1   = Box_nwy(od->boxSrc) ;
    double x2   = Box_sex(od->boxSrc) ;
    double y2   = Box_sey(od->boxSrc) ;

    // horizontal lines
    pts = grid_getGridTicks(grid,wid_canvas_getSrc2Fu(odcvs,y1),
                                 wid_canvas_getSrc2Fu(odcvs,y2));
    if ( pts!=0 ) {
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            y = wid_canvas_getFu2Src( odcvs, pts->eles[i] ); 
            od_draw_lineXY(od,OD_GC_SysDraw,x1,y,x2,y);
        }
        dbleset_delete(pts);
    }

    // vertical lines
    pts = grid_getGridTicks(grid,wid_canvas_getSrc2Fu(odcvs,x1),
                                 wid_canvas_getSrc2Fu(odcvs,x2));
    if ( pts!=0 ) {
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            x = wid_canvas_getFu2Src( odcvs, pts->eles[i] ); 
            od_draw_lineXY(od,OD_GC_SysDraw,x,y1,x,y2);
        }
        dbleset_delete(pts);
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= ruler management                                                   =*/
 
extern void wid_canvas_rulerDxDy(Cwid_canvas*self, int*dx, int*dy)
{
    Tvgui*   gui      = self->vgcm_gui;
    int      ticklen  = SCALE_MajorLen ; // legnth of major ticks
    int      fontid   = SCALE_FontId;
    double   fontsize = SCALE_FontPt;
    gui_fonts_extents_DesUnit(gui,fontid,fontsize, "9999", NULL,NULL,dx,dy);
    *dy = SCALE_TopSep + *dy + SCALE_MidSep + ticklen;
    *dx = ticklen + SCALE_MidSep + *dx + SCALE_LeftSep ;
}

static double drawRuler_getLabelDxPix(Tvgui*gui, Toutdev*od, Cgrid*grid,double fu)
{
    int dx;
    char text[100];
    int      fontid   = SCALE_FontId;
    double   fontsize = SCALE_FontPt;
    drawRuler_getLabel(text,grid,fu);
    gui_fonts_extents_DesUnit(gui,fontid,fontsize, text, NULL,NULL,&dx,NULL);
    return dx;
}

static void wid_canvas_drawRulerH(Twid_canvas_scale*ruler, Cvgui_odcanvas*cvs, Cgrid* grid)
{
    char text[100];
    int i,ix;
    TsintSet*  ptspix;
    TdbleSet* pts;

    Twid_canvas*     canvas = ruler->canvas;
    Tvgui*           gui    = canvas->vgcm_gui;
    Tvgui_odpixmap*  pod    = ruler->pdev;
    Toutdev*         od     = &pod->vv_od;
    int              dy     = vv_dy(pod);

    od_clear(od);

    // range of ruler in FU
    double x1   = wid_canvas_getSrc2Fu( cvs, Box_nwx(cvs->vv_boxSrc) );
    double x2   = wid_canvas_getSrc2Fu( cvs, Box_sex(cvs->vv_boxSrc) );
od_ds_stdStart(od);

    // draw the major ticks
    pts = grid_getRulerTicksMajor(grid,x1,x2);
    if ( pts!=0 ) {
        // get the max dx of labels.
        // notice that the larger one is either the first one
        //    -100 -99 ... 0
        // or the last one
        //     -1 0 1 ... 10
        int dx_t = 10 + max2(
               drawRuler_getLabelDxPix(gui,od,grid,pts->eles[0]),
               drawRuler_getLabelDxPix(gui,od,grid,pts->eles[pts->eleNb-1]));
        // get the font ascent
        int    ascent;
        int    fontid   = SCALE_FontId;
        double fontsize = SCALE_FontPt;
        od_str_extents_DesUnitI(od,fontid,fontsize, "999", &ascent,NULL,NULL,NULL);
        // draw the tick labels
        int labelstart,labelstep;
        ptspix = drawRuler_mainWork(&labelstart,&labelstep,cvs,pts,dx_t,1);
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            ix  = ptspix->eles[i];
            od_draw_lineXY(od,OD_GC_SysDraw,ix,dy-SCALE_MajorLen-1,ix,dy);
            if ( (i%labelstep)==labelstart ) {
                drawRuler_getLabel(text,grid,pts->eles[i]);
                od_str_draw_CE(od,OD_GC_SysDraw,text,ix,SCALE_TopSep+ascent);
            }
        }
        sintset_delete(ptspix);
        dbleset_delete(pts);
    }

    // draw the minor ticks
    pts = grid_getRulerTicksMinor(grid,x1,x2);
    if ( pts!=0 ) {
        ptspix = drawRuler_convPts2Pix(pts,cvs,1);
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            ix = ptspix->eles[i];
            od_draw_lineXY(od,OD_GC_SysDraw,ix,dy-SCALE_MinorLen-1,ix,dy);
        }
        sintset_delete(ptspix);
        dbleset_delete(pts);
    }
od_ds_stdCommit(od);
    // draw the pixmap and then free the device
    gui_widegs_setBgPxm(ruler->widEGS,pod->vv_pixmap);
    gui_pixmap_free(gui,ruler->pxm);
    ruler->pxm = vgui_odpixmap_dupPxm(pod);
}

static void wid_canvas_drawRulerV(Twid_canvas_scale*ruler, Cvgui_odcanvas*cvs, Cgrid* grid)
{
    char text[100];
    int i,iy;
    TsintSet*  ptspix;
    TdbleSet* pts;

    Twid_canvas*     canvas = ruler->canvas;
    Tvgui*           gui    = canvas->vgcm_gui;
    Tvgui_odpixmap*  pod    = ruler->pdev;
    Toutdev*         od     = &pod->vv_od;

    od_clear(od);

    // range of ruler in FU
    double y1   = wid_canvas_getSrc2Fu( cvs, Box_nwy(cvs->vv_boxSrc) );
    double y2   = wid_canvas_getSrc2Fu( cvs, Box_sey(cvs->vv_boxSrc) );

od_ds_stdStart(od);
    // draw the major ticks
    pts = grid_getRulerTicksMajor(grid,y1,y2);
    if ( pts!=0 ) {
        int    fontid   = SCALE_FontId;
        double fontsize = SCALE_FontPt;
        int dy_t,descent_t;
        od_str_extents_DesUnitI(od,fontid,fontsize, "999",
                NULL,&descent_t,NULL,&dy_t);
        int labelstart,labelstep;
        ptspix = drawRuler_mainWork(&labelstart,&labelstep,cvs,pts,dy_t,0);
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            iy = ptspix->eles[i];
            od_draw_lineXY(od,OD_GC_SysDraw,0,iy,SCALE_MajorLen,iy);
            if ( (i%labelstep)==labelstart ) {
                drawRuler_getLabel(text,grid,pts->eles[i]);
                int xt = SCALE_MajorLen + SCALE_TopSep;
                int yt = iy+descent_t;
                od_str_draw_LR(od,OD_GC_SysDraw,text,xt,yt);
            }
        }
        sintset_delete(ptspix);
        dbleset_delete(pts);
    }

    // draw the minor ticks
    pts = grid_getRulerTicksMinor(grid,y1,y2);
    if ( pts!=0 ) {
        ptspix = drawRuler_convPts2Pix(pts,cvs,0);
        for ( i=0 ; i<pts->eleNb ; i+=1 ) {
            iy = ptspix->eles[i];
            od_draw_lineXY(od,OD_GC_SysDraw,0,iy,SCALE_MinorLen,iy);
        }
        sintset_delete(ptspix);
        dbleset_delete(pts);
    }
od_ds_stdCommit(od);
    // draw the pixmap and then free the device
    gui_widegs_setBgPxm(ruler->widEGS,pod->vv_pixmap);
    gui_pixmap_free(gui,ruler->pxm);
    ruler->pxm = vgui_odpixmap_dupPxm(pod);
}

static void wid_canvas_rulerSetDev(Twid_canvas_scale*ruler, Tcolor*bg)
{
    int dx,dy;
    Tvgui* gui  = ruler->canvas->vgcm_gui;
    dx = vgui_widegs_getDX(ruler->widEGS);
    dy = vgui_widegs_getDY(ruler->widEGS);
    if ( ruler->pdev==0 ) {
        ruler->pdev = vgui_odpixmap_new(gui,bg,dx,dy,NULL);
        return;
    }

    if (vv_dx(ruler->pdev)==dx && vv_dy(ruler->pdev)==dy )
        return;

    // create a pixmap device 
    vgui_odpixmap_delete( ruler->pdev );
    ruler->pdev = vgui_odpixmap_new(gui,bg,dx,dy,NULL);
    gui_pixmap_free(gui,ruler->pxm);
    ruler->pxm = PXMEGS_none;
}

extern void wid_canvas_drawRulers(Cwid_canvas* self)
{
    Twid_canvas_scale* ruler;
    Tvgui*             gui   = self->vgcm_gui;
    Cgrid*             grid  = self->cvs_grid;
    Tvgui_odcanvas*    odcvs = self->vgcm_outdev;
    Toutdev*           od    = &odcvs->vv_od;

    Tcolor* bg;
    int GCisPushed = 0;
    if ( self->vgcm_scale==0 ) return;
    if ( self->cvs_grid==0 ) return;
    ruler = (Twid_canvas_scale*)&self->vgcm_horizontalScale;
    if ( gui_widegs_isShown( ruler->widEGS ) ) {
        if ( GCisPushed==0 ) {
            GCisPushed = 1;
            od_style_pushTop(od,OD_GC_SysDraw);
            bg = wid_canvas_setGC_rulers(gui,od);
        }
        wid_canvas_rulerSetDev(ruler, bg);
        wid_canvas_drawRulerH(ruler,odcvs,self->cvs_grid);
    }
    ruler = (Twid_canvas_scale*)&self->vgcm_verticalScale;
    if ( gui_widegs_isShown( ruler->widEGS ) ) {
        if ( GCisPushed==0 ) {
            GCisPushed = 1;
            od_style_pushTop(od,OD_GC_SysDraw);
            bg = wid_canvas_setGC_rulers(gui,od);
        }
        wid_canvas_rulerSetDev(ruler, bg);
        wid_canvas_drawRulerV(ruler,odcvs,self->cvs_grid);
    }
    if ( GCisPushed )
        od_style_pop(od,OD_GC_SysDraw);
}

extern void wid_canvas_drawRulersMkr(Cwid_canvas* self, int x, int y)
{
    Twid_canvas_scale* ruler;
    Tvgui*        gui  = self->vgcm_gui;
    if ( self->vgcm_scale==0 ) return;
    if ( self->cvs_grid==0 ) return;
    ruler = (Twid_canvas_scale*)&self->vgcm_horizontalScale;
    if ( ! PXMEGS_IsNone(ruler->pxm) ) {
        Toutdev* od = &ruler->pdev->vv_od;
        wid_canvas_setGC_rulers(gui,od);
        int y = vv_dy(ruler->pdev)-SCALE_MinorLen+2;
        Tdble2 pts[] = {
            {x,y}, {x+7,y-10}, {x-7,y-10}, { x,y }
        };
        vgui_odpixmap_cpyPxm(ruler->pdev,ruler->pxm);
        od_draw_mlineT(od,OD_GC_SysDraw,TOOL_TABLE_ARG(pts));
        gui_widegs_setBgPxm(ruler->widEGS,ruler->pdev->vv_pixmap);
        ruler->currMkrPos = x;
    }
    ruler = (Twid_canvas_scale*)&self->vgcm_verticalScale;
    if ( ! PXMEGS_IsNone(ruler->pxm) ) {
        Toutdev* od = &ruler->pdev->vv_od;
        wid_canvas_setGC_rulers(gui,od);
        int x = 2;
        Tdble2 pts[] = {
            {x,y}, {x+10,y-7}, {x+10,y+7}, { x,y }
        };
        vgui_odpixmap_cpyPxm(ruler->pdev,ruler->pxm);
        od_draw_mlineT(od,OD_GC_SysDraw,TOOL_TABLE_ARG(pts));
        gui_widegs_setBgPxm(ruler->widEGS,ruler->pdev->vv_pixmap);
        ruler->currMkrPos = y;
    }
}

extern void wid_canvas_clearRulersMkr(Cwid_canvas* self)
{
    Twid_canvas_scale* ruler;
    if ( self->vgcm_scale==0 ) return;
    if ( self->cvs_grid==0 ) return;

    ruler = (Twid_canvas_scale*)&self->vgcm_horizontalScale;
    if ( ! PXMEGS_IsNone(ruler->pxm) ) {
        vgui_odpixmap_cpyPxm(ruler->pdev,ruler->pxm);
        gui_widegs_setBgPxm(ruler->widEGS,ruler->pdev->vv_pixmap);
    }
    ruler = (Twid_canvas_scale*)&self->vgcm_verticalScale;
    if ( ! PXMEGS_IsNone(ruler->pxm) ) {
        vgui_odpixmap_cpyPxm(ruler->pdev,ruler->pxm);
        gui_widegs_setBgPxm(ruler->widEGS,ruler->pdev->vv_pixmap);
    }
}

/*======================================================================*/
/*= Private utilities for rulers drawing                               =*/

static char* drawRuler_getLabel(char*buf,Cgrid*grid,double fu)
{
    int usr;
    usr  = round( lu_FuToUsr(&grid->unit,fu) );
    sprintf(buf,"%d",usr);
    return buf;
}

static TsintSet* drawRuler_convPts2Pix(TdbleSet*pts, Cvgui_odcanvas*cvs, int x)
{
    double v; int    i;
    TsintSet* ret = sintset_new( pts->eleNb );
    for (i=0 ; i<pts->eleNb ; i+=1) {
        v  = wid_canvas_getFu2Src( cvs, pts->eles[i] );
        v  = x ? CONV_X_S2D(&cvs->vv_od,v) : CONV_Y_S2D(&cvs->vv_od,v); 
        sintset_add(ret,round(v));
    }
    return ret;
}

// lblstart,lblstep: ticks with labels (labelstart+k*labelstep)
// lblpix:           len of label in pixel.
// pts:              tick set in FU
// x:                1:horizontal ; 0:vertical 
// return set:       the pts ticks converted into pixel
static TsintSet* drawRuler_mainWork( int*lblstart,int*lblstep,
        Cvgui_odcanvas* cvs, TdbleSet*pts, int lblpix, int x)
{
    double v; int nbe = pts->eleNb;
    if (nbe<=1) {
        *lblstart = 0;
        *lblstep  = 1;
        goto convert;
    }

    // at least 2 points start (s) and end (e) are different and
    // ptsstep is not null.
    double s = wid_canvas_getFu2Src( cvs, pts->eles[0] );
    double e = wid_canvas_getFu2Src( cvs, pts->eles[pts->eleNb-1] );

    double ptsstep = wid_canvas_getFu2Src( cvs, pts->eles[1]-pts->eles[0] );

    // get *lblstep
    double lbllen  = CONV_D_D2S(&cvs->vv_od,lblpix);
    for ( *lblstep=1 ; (*lblstep*ptsstep)<lbllen ; *lblstep+=1 );

    if ( 0<s || e<0 ) {
        *lblstart=0;
        goto convert;
    }

    // zero is in pts, we want to label it, so we have to choose
    // the lblstart such as idx0 = lblstart + k*lblstep
    int idx0;
    for ( idx0=0 ; idx0<pts->eleNb ; idx0+=1 ) {
        v = pts->eles[idx0];
        if ( v==0 ) break;
        TEMP_ase_printf( dble_close0(v), "zero=%e is missed",v);
    }
    if ( idx0==pts->eleNb )
        *lblstart=0;
    else {
        *lblstart=idx0%*lblstep;
    }

convert:
    return drawRuler_convPts2Pix(pts,cvs,x);
}


