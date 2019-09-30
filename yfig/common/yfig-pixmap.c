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
 *$Software: YFIG
 *      $Id: yfig-pixmap.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-pixmap.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the pixmaps that the YFIG GUI requires.                =*/
/*=                                                                    =*/
/*= Those are either generated from static bitmap data or drawn in a   =*/
/*= pixmap device.                                                     =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-pixmap.h"
#include "yfig-device.h"

#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/

#define LJC_DX 52
#define LJC_DY 30

#define ARROW_DX 52  // in pixel
#define ARROW_DY 30  // in pixel

/*======================================================================*/

static Tvgui_odpixmap* pxm_devOpen( Tvgui* gui, Tod_GCdef gcd,
        int dx, int dy,
        int* lw, TlineStyle* ls, TjoinStyle* js, TcapStyle* cs)
{
    Ccolor *fg, *bg;
    vgui_wn_getColor(gui,wn_dialSub,&fg,&bg);
    //vgui_wn_getColor(gui,wn_dialLabel,&fg,&bg);
    Tvgui_odpixmap* pdev = vgui_odpixmap_new(gui,bg,dx,dy,NULL);
    Toutdev*        od   = &pdev->vv_dev;
    od_style_pushTop(od,gcd);
    *lw = 2;
    *js = DA_JS_Miter;
    *cs = DA_CS_Butt;
    od_ldaf_setGCsoP(od, gcd, *lw,*js,*cs,fg);
    *ls = DA_LS_None;
    *js = DA_JS_None;
    *cs = DA_CS_None;
    return pdev;
}

static void pxm_devClose( Tvgui_odpixmap*pdev, Tod_GCdef gcd)
{ od_style_pop(&pdev->vv_od,gcd); vgui_odpixmap_delete( pdev ); }

/*======================================================================*/
/*= line style: solid, dot, dash ...                                   =*/

static TlineStyle lineStyleRef[] = {
    DA_LS_Solid,    DA_LS_Dash,     DA_LS_Dot,
    DA_LS_Dash1Dot, DA_LS_Dash2Dot, DA_LS_Dash3Dot };

extern int          pxm_lineStyle_dx () { return LJC_DX; }
extern int          pxm_lineStyle_dy () { return LJC_DY; }
extern TlineStyle*  pxm_lineStyle_ref() { return lineStyleRef; }
extern int          pxm_lineStyle_nbe(Tvgui* gui)
{
    if ( (gui->g_externalSystem&GUI_ExtSys_QT)!=0 )
        return TOOL_TABLE_NbEle(lineStyleRef)-1; // Dash3Dot not supported
    else
        return TOOL_TABLE_NbEle(lineStyleRef);
}
extern TpixmapEGS* pxm_lineStyle_pxm(Tvgui* gui, int dx, int dy, Ccolor* bg)
{
    TpixmapEGS* table = tool_malloc( sizeof(table[0])*pxm_lineStyle_nbe(gui));
    int gcd = OD_GC_SysDraw;
    int i,lw; TlineStyle ls; TjoinStyle js; TcapStyle cs;
    if ( dx<=0 ) dx = pxm_lineStyle_dx();
    if ( dy<=0 ) dy = pxm_lineStyle_dy();
    Tvgui_odpixmap* pdev = pxm_devOpen(gui,gcd,dx,dy,&lw,&ls,&js,&cs);
    Toutdev*        od   = &pdev->vv_dev;
    js = DA_JS_Round;
    cs = DA_CS_Round;
    for ( i=0 ; i<pxm_lineStyle_nbe(gui) ; i++ ) {
        //dev_clear(dev,NULL);
        od_ldaf_setGCddZ(od, gcd,
                lw, js, cs, gui->vg_black,
                pxm_lineStyle_ref()[i], NULL);
        od_draw_lineXY(od, gcd, 1,dy/2, dx-1.,dy/2.);
        table[i] = vgui_odpixmap_getPxm(pdev);
    }
    pxm_devClose( pdev, gcd );
    return table;
}

/*======================================================================*/
/*= join style: miter, round, ...                                      =*/

static TjoinStyle joinStyleRef[] = {
    DA_JS_Miter,    DA_JS_Round,     DA_JS_Bevel };

extern int          pxm_joinStyle_dx () { return LJC_DX; }
extern int          pxm_joinStyle_dy () { return LJC_DY; }
extern TjoinStyle*  pxm_joinStyle_ref() { return joinStyleRef; }
extern int          pxm_joinStyle_nbe()
{ return TOOL_TABLE_NbEle(joinStyleRef); }
extern TpixmapEGS* pxm_joinStyle_pxm(Tvgui* gui)
{
    TpixmapEGS* table = tool_malloc( sizeof(table[0])*pxm_joinStyle_nbe());
    int gcd = OD_GC_SysDraw;
    int i,lw; TlineStyle ls; TjoinStyle js; TcapStyle cs;
    int dx = pxm_joinStyle_dx();
    int dy = pxm_joinStyle_dy();
    Tvgui_odpixmap* pdev = pxm_devOpen(gui,gcd,dx,dy,&lw,&ls,&js,&cs);
    Toutdev*        od   = &pdev->vv_dev;
    Tdble2         pts[] = { {8,10}, {dx-12,10}, {dx/3,dy-8} };
    lw=7;
    for ( i=0 ; i<pxm_joinStyle_nbe() ; i++ ) {
        //dev_clear(dev,NULL);
        od_ldaf_setGCsoP(od, gcd, lw, 
                pxm_joinStyle_ref()[i], cs, gui->vg_black);
        od_draw_mlineT(od,gcd,pts,3);
        table[i] =  vgui_odpixmap_getPxm(pdev);
    }
    pxm_devClose( pdev, gcd );
    return table;
}

/*======================================================================*/
/*= cap style: butt, round, ...                                        =*/

static TcapStyle capStyleRef[] = {
    DA_CS_Butt,    DA_CS_Round,     DA_CS_Proj };

extern int          pxm_capStyle_dx () { return LJC_DX; }
extern int          pxm_capStyle_dy () { return LJC_DY; }
extern TcapStyle*  pxm_capStyle_ref()  { return capStyleRef; }
extern int          pxm_capStyle_nbe()
{ return TOOL_TABLE_NbEle(capStyleRef); }
extern TpixmapEGS* pxm_capStyle_pxm(Tvgui* gui)
{
    TpixmapEGS* table = tool_malloc( sizeof(table[0])*pxm_capStyle_nbe());
    int gcd = OD_GC_SysDraw;
    int i,lw; TlineStyle ls; TjoinStyle js; TcapStyle cs;
    int dx = pxm_capStyle_dx();
    int dy = pxm_capStyle_dy();
    Tvgui_odpixmap* pdev = pxm_devOpen(gui,gcd,dx,dy,&lw,&ls,&js,&cs);
    Toutdev*        od   = &pdev->vv_dev;
    lw=10;
    for ( i=0 ; i<pxm_capStyle_nbe() ; i++ ) {
        //dev_clear(dev,NULL);
        od_ldaf_setGCsoP(od, gcd, lw, 
                js, pxm_capStyle_ref()[i], gui->vg_black);
        od_draw_lineXY(od,gcd, 8,dy/2, dx-8,dy/2);
        od_ldaf_setGCsoP(od, gcd, 1, 
                js, DA_CS_Butt, gui->vg_black);
        od_draw_lineXY(od,gcd, 8,3,    8,dy-3);
        od_draw_lineXY(od,gcd, dx-8,3, dx-8,dy-3);
        table[i] = vgui_odpixmap_getPxm(pdev);
    }
    pxm_devClose( pdev, gcd );
    return table;
}

/*======================================================================*/
/*= begin and end arrows.                                              =*/

#undef FP
#undef FW
#undef FP
#define FP FARROW_FillParent
#define FN FARROW_FillNone
#define FW FARROW_FillWhite
#define ADX    8 // in point
#define D2R    (30./180.*TOOL_PI) // 35 degree in radian
#define IDX(n) (FIG_KD_ArrowT00+(n))

static TarrowDesc arrowRef[] = {
  { .flag=FN, .abslength=1, .length=ADX, .angle=D2R, .vtidx=IDX(0), },
  { .flag=FW, .abslength=1, .length=ADX, .angle=D2R, .vtidx=IDX(1), },
  { .flag=FP, .abslength=1, .length=ADX, .angle=D2R, .vtidx=IDX(1), },
  { .flag=FW, .abslength=1, .length=ADX, .angle=D2R, .vtidx=IDX(2), },
  { .flag=FP, .abslength=1, .length=ADX, .angle=D2R, .vtidx=IDX(2), },
};

extern int          pxm_arrow_dx () { return ARROW_DX; }
extern int          pxm_arrow_dy () { return ARROW_DY; }
extern int          pxm_arrow_nbe() { return TOOL_TABLE_NbEle(arrowRef); }
extern TarrowDesc*  pxm_arrow_ref() { return arrowRef; }
extern TpixmapEGS*  pxm_arrow_pxm(Tvgui* gui, int atBeg)
{
    TpixmapEGS* table = tool_malloc( sizeof(table[0])*pxm_arrow_nbe());
    double thickness = 2.5;
    int gcd = OD_GC_SysDraw;
    int i,lw; TlineStyle ls; TjoinStyle js; TcapStyle cs;
    int dx = pxm_capStyle_dx();
    int dy = pxm_capStyle_dy();

    Tvgui_odpixmap* pod   = pxm_devOpen(gui,gcd,dx,dy,&lw,&ls,&js,&cs);
    Toutdev*        od    = &pod->vv_od;
    Tyfig_device*   fsod  = fsod_attachPt(&pod->vv_vod);
    TobjAll         obj   = fsod_addLineH(fsod,thickness,5);

    double scale = fsod_scaArrowAbsDU(fsod,arrowRef+0,dx/2.5,dy-8);
    for ( i=0 ; i<pxm_arrow_nbe() ; i++ ) {
        TarrowDesc ad = arrowRef[i];
        ad.length *= scale;
        fsod_chgArrowAbs(fsod,obj,atBeg,&ad,thickness);
        od_draw( od );
        table[i] = vgui_odpixmap_getPxm(pod);
    }
    fsod_detach(fsod);
    pxm_devClose( pod, gcd );
    return table;
}

/*======================================================================*/
/*= for arrow mode.                                                    =*/

extern int          pxm_arrowmode_dx () { return LJC_DX; }
extern int          pxm_arrowmode_dy () { return LJC_DY; }

extern TpixmapEGS*  pxm_arrowmode_pxm(Tvgui* gui, int idx)
{
static TpixmapEGS table[pxm_arrowmode_nbe()];
    double thickness = 2.5;
    int gcd = OD_GC_SysDraw;
    int i,lw; TlineStyle ls; TjoinStyle js; TcapStyle cs;
    int dx = pxm_capStyle_dx();
    int dy = pxm_capStyle_dy();

    Tvgui_odpixmap* pod   = pxm_devOpen(gui,gcd,dx,dy,&lw,&ls,&js,&cs);
    Toutdev*        od    = &pod->vv_od;
    Tyfig_device*   fsod  = fsod_attachPt(&pod->vv_vod);
    TobjAll         obj   = fsod_addLineH(fsod,thickness,5);

    TarrowDesc ad = arrowRef[idx];
    double scale = fsod_scaArrowAbsDU(fsod,&ad,dx/4,dy-8);
    ad.length *= scale;
    for ( i=0 ; i<pxm_arrowmode_nbe() ; i++ ) {
        if ( (i&1)!=0 ) 
            fsod_chgArrowAbs(fsod,obj,0,&ad,thickness);
        else
            obj_arrow_delete(obj,0);
        if ( (i&2)!=0 ) 
            fsod_chgArrowAbs(fsod,obj,1,&ad,thickness);
        else
            obj_arrow_delete(obj,1);
        od_draw( od );
        table[i] = vgui_odpixmap_getPxm(pod);
    }
    fsod_detach(fsod);
    pxm_devClose( pod, gcd );
    return table;
}

/*======================================================================*/
