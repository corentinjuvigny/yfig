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
 *      $Id: guixt-font-pango.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-font-pango.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver       =*/
/*= based on fontconfig, xft and  truetype fonts).                     =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*=                                                                    =*/
/*= Notes:                                                             =*/
/*=  - Experimentally pango_layout_get_extents function returns the    =*/
/*=    same values than pango_layout_get_pixel_extents times by        =*/
/*     PANGO_SCALE (01/01/2018).                                       =*/
/*=  - The unit of returned value seems to be pixel.                   =*/
/*======================================================================*/

#include "guixt.h"
#include "pangoxft-config.h"

#include "vgui/vgui-outdev.h"

#include "outdev/resources.h"
#include "outdev/outdev-vt.h"
#include "tools/font.h"
#include "tools/lnc.h"

/*======================================================================*/

#define OnlyNew

#define STD_VARS \
    Tvgui_outdev* vod = dev; \
    Tvgui*        gui  = vod->v_gui

#define C_STD_VARS \
    Cvgui_outdev* vod = dev; \
    Tvgui*        gui  = vod->v_gui

/*======================================================================*/

#define FGBG(fg,bg) DBG_Color2strRGB(fg),DBG_Color2strRGB(bg)

#define RC(p) (p)->fgcolor.color.red, (p)->fgcolor.color.green,\
              (p)->fgcolor.color.blue,(p)->fgcolor.color.alpha

/*======================================================================*/
/*= data structure for pangoxft                                        =*/

typedef struct _Tgui_pangoGC Tgui_pangoGC;
struct _Tgui_pangoGC {
    PangoFontDescription* fegs;    // the sized font
    XftColor              fgcolor; // color for text drawing
    //XftColor bgcolor;   // not yet used
};

typedef struct _Tgui_pango Tgui_pango;

struct _Tgui_pango {
    PangoFontMap * fontmap;
    PangoContext * context;
    PangoLayout  * layout;
    PangoFontDescription
                 * font;    // the font presently set within layout
    XftDraw      * xftdraw;
    Tvgui_outdev * device;  // the device presently set within xftdraw
    Tgui_pangoGC gccache[OD_GC_NB];
    double       angle;
} ;

extern void gui_detachOutdev(Tvgui* gui, Tvgui_outdev* vod)
{
    if ( E_Pango->device==vod ) {
        XftDrawChange ( E_Pango->xftdraw, 0 );
        E_Pango->device=0;
    }
}

/*======================================================================*/
/*= font handler                                                       =*/

extern void gui_gc_create_font(Tvgui* gui, Tod_GCdef gcd,
                               int fi, double pts, Ccolor* fg, Ccolor* bg)
{
    XRenderColor xrcolor;
    Tgui_pangoGC* pgc = E_Pango->gccache+gcd;
    DBG1_FontE("gcd=%d fi=%02d pts=%f fg=%s bg=%s\n",gcd,fi,pts,FGBG(fg,bg));
    if ( fg!=0 ) {
        if ( pgc->fgcolor.pixel != EGSCOLOR_NONE ) {
            XftColorFree(E_Display, E_Visual, E_ColorMap, &pgc->fgcolor);
        }
        xrcolor.red   = fg->rgb.red<<8   | fg->rgb.red;
        xrcolor.green = fg->rgb.green<<8 | fg->rgb.green;
        xrcolor.blue  = fg->rgb.blue<<8  | fg->rgb.blue;
        xrcolor.alpha = 0xffff;
        XftColorAllocValue( E_Display, E_Visual, E_ColorMap,
               &xrcolor, &pgc->fgcolor );
        DBG2_FontM("gcd=%d fi=%02d pts=%f ; "
                   "fg color changed=%04u/%04u/%u:%04u",gcd,fi,pts,RC(pgc));
    }
    DBG1_FontL("gcd=%d fi=%02d pts=%f fg=%s bg=%s\n",gcd,fi,pts,FGBG(fg,bg));
}

extern void gui_gc_free_font(Tvgui* gui, Tod_GCdef gcd)
{
    Tgui_pangoGC* pgc = E_Pango->gccache+gcd;
    DBG1_Font("gcd=%d\n",gcd);
    if ( pgc->fgcolor.pixel != EGSCOLOR_NONE ) {
        XftColorFree(E_Display, E_Visual, E_ColorMap, &pgc->fgcolor);
    }
}

static void gui_font_unload (Tvgui* gui, TfontEGS f)
{
    PangoFontDescription* fegs = f.ptr;
    pango_font_description_free(fegs);
}

static TfontEGS gui_font_load0(Tvgui* gui, CfontDef*fd, double pts, double*ratio)
{
    char fname[300];
    tool_LNC_sprintf(fname,fd->stdfmt,pts);

    PangoFontDescription* fegs;
    fegs = pango_font_description_from_string(fname);

    if ( fegs!=0 ) {
        TfontEGS ret; ret.ptr = fegs;
        *ratio = 1;
        return ret;
    } else {
        *ratio = 0;
        return FONTEGS_none;
    }
}

static TfontEGS gui_font_load(Tvgui* gui, int fi, double pts, double*ratio)
{
    CfontDef* fd = VG_fontdef(gui,fi);
    TfontEGS  fegs;
    DBG1_Font("look for fi=%d pts=%f family=%s",fi,pts,fd->family);
    fegs = gui_font_load0(gui,fd,pts,ratio);
    DBG0_Font("look for fi=%d pts=%f family=%s: %s",fi,pts,fd->family,
            FONTEGS_IsNone(fegs) ? "NOT FOUND" : "found");
    return fegs;
}

static TfontEGS  gui_font_loadCacheEGS (Tvgui*gui, int fi, double pts)
{
    TodResources* res = gui->vg_odResources;
    CfontInfo*  finfo =  fontset_searchLRU(res->fontCache,fi,pts);
    if ( finfo==0 ) {
        // finfo = od_str_fontLoadAndCache(od,fi,pts);
        double    fontratio;
        TfontEGS  fontEGS;

        // get font
        fontEGS = gui_font_load(gui,fi, pts, &fontratio);
        if ( FONTEGS_IsNone(fontEGS) ) 
            fontEGS = gui_font_load(gui,0,pts,&fontratio);
        finfo = od_fontset_addLRU(gui->vg_od,fi,pts,fontEGS,fontratio);
    }
    return finfo->fo_egs;
}

extern TfontEGS odvt_fontLoadPts (void*dev, int fi, double pts, double*ratio)
{
    STD_VARS;
    return gui_font_load(gui,fi,pts,ratio);
}

extern void  odvt_fontUnload  (void*dev, TfontEGS f)
{
    STD_VARS;
    return gui_font_unload(gui,f);
}

/*======================================================================*/
/*= Drawing horizontal strings                                         =*/

#define DBG_Fmt "gcd=%d f=%d/%f fg=%s"
#define DBG_Arg gcd,font->fo_index,font->fo_size,DBG_Color2str(fg)

extern void odvt_setGCstr    (void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    STD_VARS;
    XRenderColor xrcolor;
    Tgui_pangoGC* pgc = E_Pango->gccache+gcd;
    DBG1_FontE(DBG_Fmt,DBG_Arg);

    pgc->fegs = font->fo_egs.ptr;
    if ( fg!=0 ) {
        if ( pgc->fgcolor.pixel != EGSCOLOR_NONE ) {
            XftColorFree(E_Display, E_Visual, E_ColorMap, &pgc->fgcolor);
        }
        xrcolor.red   = fg->rgb.red<<8   | fg->rgb.red;
        xrcolor.green = fg->rgb.green<<8 | fg->rgb.green;
        xrcolor.blue  = fg->rgb.blue<<8  | fg->rgb.blue;
        xrcolor.alpha = 0xffff;
        XftColorAllocValue( E_Display, E_Visual, E_ColorMap,
               &xrcolor, &pgc->fgcolor );
        DBG2_FontM(DBG_Fmt " : fg color changed", DBG_Arg);
    }
    DBG1_FontL(DBG_Fmt,DBG_Arg);
}

static void gui_fonts_extents_DesUnit0(Tvgui*gui, TfontEGS f,
        CstrBrief text, double*ascent, double*descent, double*dx, double*dy)
{
    PangoRectangle irect, lrect;
    PangoFontDescription* fegs = f.ptr;
    PangoLayout * layout = E_Pango->layout;
    if ( E_Pango->font!=fegs ) {
        E_Pango->font = fegs;
        pango_layout_set_font_description(layout, fegs);
    }
    pango_layout_set_text (layout, text, -1);
    pango_layout_get_pixel_extents(layout,&irect,&lrect);
#if 0
printf("ink=(%d,%d) %d %d ; log=(%d,%d) %d %d\n",
        irect.x, irect.y, irect.width, irect.height,
        lrect.x, lrect.y, lrect.width, lrect.height);
#endif
    double baselineDble = pango_layout_get_baseline(layout);
    int    baseline = round(baselineDble/PANGO_SCALE);
    int    width    = lrect.width;
    int    height   = lrect.height;
    if ( ascent  ) *ascent  = baseline;
    if ( descent ) *descent = height - baseline;
    if ( dx      ) *dx      = width;
    if ( dy      ) *dy      = height;
}

extern void gui_fonts_extents_DesUnit(Tvgui*gui, int fontid, double fontsize,
        CstrBrief str, int*asc, int*desc, int*dx, int*dy)
{
    TfontEGS  fegs = gui_font_loadCacheEGS (gui, fontid, fontsize);
    double vasc, vdesc, vdx, vdy;
    gui_fonts_extents_DesUnit0(gui, fegs, str, &vasc, &vdesc, &vdx, &vdy);
    if ( asc  ) *asc  = round( vasc );
    if ( desc ) *desc = round( vdesc);
    if ( dx   ) *dx   = round( vdx );
    if ( dy   ) *dy   = round( vdy );
}

extern void odvt_extentsPix  (void*dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent, double* dx,     double* dy)
{
    STD_VARS;
    gui_fonts_extents_DesUnit0(gui, f, text, ascent, descent, dx, dy);
}

extern void gui_fonts_lineinfo_DesUnit(Tvgui*gui, int fontid, double fontsize,
        int*idy, int*ldy, int*ils)

{
    TfontEGS  fegs = gui_font_loadCacheEGS (gui, fontid, fontsize);

    double pts;
    pts = pango_font_description_get_size(fegs.ptr);
    pts = pts/PANGO_SCALE * VG_ppiScreen(gui)/72 ;

    double dy;
    gui_fonts_extents_DesUnit0(gui,fegs," ",NULL,NULL,NULL,&dy);

    if ( idy!=0 ) *idy = round( pts );
    if ( ldy!=0 ) *ldy = round( dy );
    if ( ils!=0 ) *ils = round( dy - pts );
}

/*======================================================================*/
/*= Drawing strings and rotated strings                                =*/

#define pgo(x) ((x)*PANGO_SCALE)

#define XOR_DRAW_BEGIN \
    Pixmap pixmap; \
    int wd = Box_dx(vod->v_boxDes); \
    int ht = Box_dy(vod->v_boxDes); \
    if ( gcd==OD_GC_Preview ) { \
        pixmap = XCreatePixmap(E_Display,DefaultRootWindow(E_Display), \
                wd, ht, E_Depth); \
        GC gc; XGCValues gcv = { .function=GXclear }; \
        gc = XCreateGC(E_Display,pixmap,GCFunction,&gcv); \
        XFillRectangle(E_Display,pixmap,gc,0,0,wd,ht); \
        XFreeGC(E_Display,gc); \
        XftDrawChange ( E_Pango->xftdraw, pixmap ); \
    } 
#define XOR_DRAW_END \
    if ( gcd==OD_GC_Preview ) { \
        XCopyArea(E_Display,pixmap,VOD_Drawable(vod),E_GCcache(gcd),0,0, \
                wd,ht,0,0); \
        XFreePixmap(E_Display,pixmap); \
        XftDrawChange ( E_Pango->xftdraw, VOD_Drawable(vod) ); \
    }

static void update_GC(Tvgui*gui, Tgui_pangoGC* pgc,
        Tvgui_outdev*vod, double angle)
{
    PangoLayout * layout = E_Pango->layout;

    if (  E_Pango->device!=(void*)vod ) {
        E_Pango->device=(void*)vod;
        XftDrawChange ( E_Pango->xftdraw, VOD_Drawable(vod) );
    }
    if ( E_Pango->font!=pgc->fegs ) {
        E_Pango->font = pgc->fegs;
        pango_layout_set_font_description(layout, pgc->fegs);
    }
    //pango_layout_set_font_description(layout, pgc->fegs);
    double* pangle  = &E_Pango->angle;
    if ( *pangle!=angle ) {
        *pangle = angle;
        if ( angle==0 ) {
            pango_context_set_matrix(E_Pango->context,0);
        } else {
            double a = angle;
            PangoMatrix m = { cos(a), +sin(a), -sin(a), cos(a), 0,0 };
            pango_context_set_matrix (E_Pango->context,&m);
        }
    }
}

#undef  DBG_Fmt 
#undef  DBG_Arg
#define DBG_Fmt "gcd=%d x=%d y=%d : text=%s"
#define DBG_Arg gcd,x,y,text

extern void     odvt_drawNW_Pix (void* dev, Tod_GCdef gcd,
                    double x, double y, CstrBrief text)
{
    STD_VARS;
    Tgui_pangoGC* pgc    = E_Pango->gccache+gcd;
    PangoLayout * layout = E_Pango->layout;
  //DBG2_FontE(DBG_Fmt, DBG_Arg);
    update_GC(gui,pgc,dev,0);
    pango_layout_set_text (layout, text, -1);

    XOR_DRAW_BEGIN
    pango_xft_render_layout (E_Pango->xftdraw,
            &pgc->fgcolor, layout, round(pgo(x)), round(pgo(y)));
    XOR_DRAW_END
  //DBG2_FontL(, DBG_Arg);
}

#undef  DBG_Fmt 
#undef  DBG_Arg
#define DBG_Fmt "gcd=%.1f x=%.1f y=%d alpha=%6.2f : text=%s"
#define DBG_Arg gcd,x,y,a/TOOL_2PI*360.0,text

extern void     odvt_drawNWr_Pix(void* dev, Tod_GCdef gcd, double x, double y,
                                 CstrBrief text, double a)
{
    if ( a==0 ) {
        odvt_drawNW_Pix(dev,gcd,x,y,text);
        return;
    }
    DBG2_FontE(DBG_Fmt, DBG_Arg);
    STD_VARS;
    Tgui_pangoGC* pgc    = E_Pango->gccache+gcd;
    PangoLayout * layout = E_Pango->layout;
    PangoContext* context = E_Pango->context;
    pango_layout_set_text (layout, text, -1);
    update_GC(gui,pgc,dev,+a);

    PangoMatrix* m;
    m = (PangoMatrix*)pango_context_get_matrix(E_Pango->context);
    m->x0 = x;
    m->y0 = y;

XDrawLine(E_Display, VDEV_Drawable(vod), E_GCcache(gcd), 0,0, x,y);

    XOR_DRAW_BEGIN
    pango_xft_render_layout (E_Pango->xftdraw,
            &pgc->fgcolor, layout, 0,0); //pgo(x), pgo(y));
    XOR_DRAW_END
    DBG2_FontL(, DBG_Arg);
}

/*======================================================================*/
/*= initialization and free of module                                  =*/

//#define DO_FontGeom

#ifdef DO_FontGeom
// pts and returned values are in point (1/72 inch).
// return 0 on success otherwise -1 (loading of (fdef,pts) font failed.)
extern int gui_fonts_getInfo(Tvgui*gui, CfontDef* fdef, double pts,
        double *adx, double *idy, double *asc, double *ils)
{
    double   fontratio;
    TfontEGS fegs = gui_font_load0(gui,fdef, pts, &fontratio);

    if ( FONTEGS_IsNone(fegs) ) {
        return -1;
    }
    PangoFontDescription* pgFDes = fegs.ptr;
    PangoFontMap*         pgFMap = pango_xft_get_font_map(E_Display, E_ScreenNum);
    PangoFont*            pgFont = pango_font_map_load_font(pgFMap,E_Pango->context, pgFDes);
    PangoFontMetrics*     pgFMet = pango_font_get_metrics (pgFont,NULL);

    double app_dx;
    app_dx = pango_font_metrics_get_approximate_char_width(pgFMet);
    double ink_dy;
    ink_dy = pango_font_description_get_size(pgFDes);
    double ascent;
    ascent = pango_font_metrics_get_ascent(pgFMet);
    pango_font_metrics_unref(pgFMet);

    PangoLayout * layout = E_Pango->layout;
    pango_layout_set_font_description(layout, pgFDes);
    pango_layout_set_text (layout, "_", -1);

    PangoRectangle lrectPgu;
    pango_layout_get_extents(layout,NULL,&lrectPgu);
    double ldy = lrectPgu.height;

    double factor  = 1.0/PANGO_SCALE ; //* VG_ppiScreen(gui)/72 ;
    double factor2 = 1.0/PANGO_SCALE / VG_ppiScreen(gui) * 72 ;

    app_dx       *= factor2;
    ink_dy       *= factor;
    ascent       *= factor2;  
    ldy          *= factor2;

//printf("pts=%2.1f app_dx=%f dy=%f ink_dy=%f ascent=%f ils=%f\n",
//        pts,app_dx,ldy,ink_dy,ascent,ldy-ink_dy);
//#define CV(x) (x)/72.*96.
//printf("pts=%2.1f app_dx=%f dy=%f ink_dy=%f ascent=%f ils=%f\n",
//        pts,CV(app_dx),CV(ldy),CV(ink_dy),CV(ascent),CV(ldy-ink_dy));
    if (adx) *adx = app_dx;
    if (idy) *idy = ink_dy;
    if (asc) *asc = ascent;
    if (adx) *ils = ldy-ink_dy;
    return 0;
}
#endif

extern void gui_od_init(Tvgui*gui)
{
    int count,i,j;

    gui->g_main->m_pango = tool_malloc_0( sizeof(*E_Pango) );

    E_Pango->fontmap = pango_xft_get_font_map (E_Display,E_ScreenNum);
    E_Pango->context = pango_font_map_create_context (E_Pango->fontmap);
    E_Pango->layout  = pango_layout_new (E_Pango->context);
    E_Pango->xftdraw = XftDrawCreate( E_Display,
            RootWindow(E_Display,E_ScreenNum),
            E_Visual, E_ColorMap);
    for (i=0 ; i<OD_GC_NB ; i+=1) {
        E_Pango->gccache[i].fgcolor.pixel = EGSCOLOR_NONE;
        //E_Pango->gccache[i].bgcolor = EGSCOLOR_NONE;
    }
    
    // check for font availability

#ifdef DO_FontGeom
    // generated TfontGeom of used fonts
    for ( i=0 ; i< VG_fontdefNb(gui) ; i++) {
        TfontDef* fdef = VG_fontdef(gui,i);
        double pts=100, adx,idy,ascent,ils;
        gui_fonts_getInfo(gui,fdef,pts,&adx,&idy,&ascent,&ils);
        printf("s!.*FontGeo%02d!%39s, .geom = "
               "{ %1.3f, %1.3f, %1.3f, %1.3f } // FontGeom%02d!\n",
               i,"",adx/pts, idy/pts, ascent/pts, ils/pts,i);
    }
    exit(1);
#endif
}

extern void gui_od_free(Tvgui*gui)
{
    XftDrawDestroy( E_Pango->xftdraw );
    g_object_unref( E_Pango->layout  );
    g_object_unref( E_Pango->context );
    // E_Pango->fontmap: seems to be freed be XCloseDisplay 
    free ( E_Pango );
    gui->g_main->m_pango = 0;
}

/*======================================================================*/
