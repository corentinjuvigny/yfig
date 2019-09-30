/*======================================================================
 *  Copyright 2018 Corentin Juvigny
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
 *$Software: YFIG-GTK (VGUI driver to Gtk)
 *      $Id: guixt-font-pango.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-font-pango.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
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

#include "tools/lnc.h"

#define FGBG(fg,bg) DBG_Color2strRGB(fg),DBG_Color2strRGB(bg)

#define RC(p) (p)->fgcolor.color.red, (p)->fgcolor.color.green,\
              (p)->fgcolor.color.blue,(p)->fgcolor.color.alpha

/*======================================================================*/
/*= local tools                                                        =*/

static void gc_init_text(Toutdev* od, Tvgui* gui, cairo_t* gc, Tod_GCdef gcd, Csty_text* stt, double a)
{
    double red   = stt->stt_color ? stt->stt_color->rgb.red / 255. : 0.;
    double green = stt->stt_color ? stt->stt_color->rgb.green / 255. : 0.;
    double blue  = stt->stt_color ? stt->stt_color->rgb.blue / 255. : 0.;
    cairo_set_source_rgb(gc, red, green, blue);

    PangoLayout* layout = E_Pango->layout;
    if ( E_Pango->font!=E_PangoFontDesc(gcd) ) {
        E_Pango->font = E_PangoFontDesc(gcd);
        pango_layout_set_font_description(layout, E_PangoFontDesc(gcd));
    }
    double* pangle  = &E_Pango->contextangle;
    if ( *pangle!=a ) {
        *pangle = a;
        if ( a==0 ) {
            pango_context_set_matrix(E_Pango->context,0);
        } else {
            double ang = a;
            PangoMatrix m = { cos(ang), +sin(ang), -sin(ang), cos(ang), 0,0 };
            pango_context_set_matrix (E_Pango->context,&m);
        }
    }
}

static cairo_t*gc_init_pxm_text(Tvgui*gui, Tvgui_odpixmap* pod, Tod_GCdef gcd, double a)
{
    cairo_t*   gc  = cairo_create( SURFegs2gtk(pod->vv_target) );
    Csty_text* stt = od_style_topText(&pod->vv_od, gcd);
    gc_init_text(&pod->vv_od, gui, gc, gcd, stt, a);
    return gc;
}
static void gc_free_pxm_text(Tvgui* gui, Tvgui_odpixmap* pod, Tod_GCdef gcd, cairo_t* cr)
{
    TEMP_asf_printf( IMGEGS_IsNone(pod->vv_target),"%s","unexpected null for associated cairo surface");
    cairo_destroy(cr);
    GdkPixbuf *pxm = PXMegs2gtk(pod->vv_pixmap);
    if (!odpxm_stdSeq_isStarted(pxm)) {
        GdkPixbuf* px = gdk_pixbuf_get_from_surface(
                SURFegs2gtk(pod->vv_target),0, 0, pod->vv_target.dx, pod->vv_target.dy);
        gdk_pixbuf_copy_area(px, 0, 0, pod->vv_target.dx, pod->vv_target.dy,pxm, 0, 0);
        g_object_unref(G_OBJECT(px));
    }
}

static cairo_t*gc_init_cvs_text(Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, double a)
{
    GtkWidget* widget = (GtkWidget *)vod->v_cvsWidEGS;
    cairo_t*   gc     =  odcvs_getOdData(widget)->main_gc;
    Csty_text* stt    = od_style_topText(&vod->v_od, gcd);
    gc_init_text(&vod->v_od, gui, gc, gcd, stt, a);
    return gc;
}
static void gc_free_cvs_text(Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, cairo_t* cr)
{
    GtkWidget*   widget = (GtkWidget *)vod->v_cvsWidEGS;
    Tgui_daData* dt     = odcvs_getOdData(widget);
    
    TEMP_asf_printf( dt->in_xes!=0,"%s","unexpected call inside  xorEraseSection");
    if ( dt->in_ms==0 && dt->in_xds==0 ) {
        gtk_widget_queue_draw_area(widget,0,0,Box_dx(vod->v_boxDes),Box_dy(vod->v_boxDes));
    }
}

/*======================================================================*/
/*= font handler                                                       =*/

extern void gui_gc_create_font(Tvgui* gui, Tod_GCdef gcd, 
                                int fi, double pts, Ccolor* fg, Ccolor* bg)
{ }

extern void gui_gc_free_font(Tvgui* gui, Tod_GCdef gcd)
{ }

static void gui_font_unload(Tvgui* gui, TfontEGS f)
{
    PangoFontDescription* fegs = f.ptr;
    pango_font_description_free(fegs);
}

static TfontEGS gui_font_load0(Tvgui* gui, CfontDef* fd, double pts,  double* ratio)
{
    char fname[300];
    tool_LNC_sprintf(fname,fd->stdfmt,pts);
    Cstr att = fd->style;

    /*Creation of the font*/
    PangoFontDescription* fegs;
    fegs = pango_font_description_from_string(fname);

    /*Set up of styles*/
    PangoStyle style = PANGO_STYLE_NORMAL;
    if ( strstr(att, "italic") )  style = PANGO_STYLE_ITALIC;
    if ( strstr(att, "oblique") ) style = PANGO_STYLE_OBLIQUE;
    pango_font_description_set_style(fegs, style);

    PangoWeight weight = PANGO_WEIGHT_NORMAL;
    if ( strstr(att, "medium") ) weight = PANGO_WEIGHT_MEDIUM;
    if ( strstr(att, "light") )  weight = PANGO_WEIGHT_LIGHT;
    if ( strstr(att, "demi") )   weight = PANGO_WEIGHT_SEMIBOLD;
    if ( strstr(att, "bold") )   weight = PANGO_WEIGHT_BOLD;
    pango_font_description_set_weight(fegs, weight);

    //pango_font_description_set_size(fegs, pts);
    
    if ( fegs!=0 ) {
        TfontEGS ret; 
        ret.ptr = fegs;
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
        //finfo = od_str_fontLoadAndCache(od,fi,pts);
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
//#define DBG_Arg gcd,font->fo_index,font->fo_size,DBG_Color2str(fg)
extern void odvt_setGCstr(void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    STD_VARS;
    E_PangoFontDesc(gcd) = font->fo_egs.ptr;
}

static void gui_fonts_extents_DesUnit0(Tvgui*gui, TfontEGS f,
        CstrBrief text, int*ascent, int*descent, int*dx, int*dy)
{
    PangoRectangle irect, lrect;
    PangoFontDescription* fegs = f.ptr;
    PangoLayout * layout = E_Pango->layout;
    if ( E_Pango->font!=fegs ) {
        E_Pango->font = fegs;
        pango_layout_set_font_description(layout, fegs);
    }
    if ( text )
        pango_layout_set_text (layout, text, -1);
    pango_layout_get_pixel_extents(layout,&irect,&lrect);
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
    TfontEGS fegs = gui_font_loadCacheEGS(gui, fontid, fontsize);
    gui_fonts_extents_DesUnit0(gui, fegs, str, asc, desc, dx, dy);
}

extern void odvt_extentsPix(void* dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent, double* dx, double* dy)
{
    STD_VARS;
    int vascent, vdescent, vdx, vdy;
    gui_fonts_extents_DesUnit0(gui, f, text, &vascent, &vdescent, &vdx, &vdy);
    if ( ascent  ) *ascent  =  vascent ;
    if ( descent ) *descent =  vdescent;
    if ( dx      ) *dx      =  vdx ;
    if ( dy      ) *dy      =  vdy ;
}

extern void gui_fonts_lineinfo_DesUnit(Tvgui*gui, int fontid, double fontsize,
        int*idy, int*ldy, int*ils)

{
    TfontEGS  fegs = gui_font_loadCacheEGS (gui, fontid, fontsize);

    double pts;
    pts = pango_font_description_get_size(fegs.ptr);
    pts = pts/PANGO_SCALE * VG_ppiScreen(gui)/72 ;

    int dy;
    gui_fonts_extents_DesUnit0(gui,fegs," ",NULL,NULL,NULL,&dy);

    if ( idy!=0 ) *idy = round( pts );
    if ( ldy!=0 ) *ldy = round( dy );
    if ( ils!=0 ) *ils = round( dy - pts );
}

/*======================================================================*/
/*= Drawing strings and rotated strings                                =*/

#define pgo(x) ((x)*PANGO_SCALE)

extern void odvt_drawNW_Pix(void* dev, Tod_GCdef gcd, double x, double y, CstrBrief text)
{
    STD_VARS;
    GC_INIT_TEXT(gui,vod,gcd,0);
    PangoLayout*  layout = E_Pango->layout;

    if ( text )
        pango_layout_set_text(layout, text, -1);

    cairo_move_to(gc, x, y);
    pango_cairo_show_layout(gc, layout);
    GC_FREE_TEXT(gui,vod,gcd,gc);
}

extern void odvt_drawNWr_Pix(void* dev, Tod_GCdef gcd, double x, double y, CstrBrief text, double a)
{
    if ( a==0 ) {
        odvt_drawNW_Pix(dev,gcd,x,y,text);
        return;
    }
    //DBG2_FontE(DBG_Fmt, DBG_Arg);
    STD_VARS;
    GC_INIT_TEXT(gui,vod,gcd,-a);
    PangoLayout * layout = E_Pango->layout;
    PangoContext* context = E_Pango->context;
    if ( text )
        pango_layout_set_text (layout, text, -1);

    PangoMatrix* m;
    m = (PangoMatrix*)pango_context_get_matrix(E_Pango->context);
    m->x0 = x;
    m->y0 = y;

    cairo_set_matrix(gc,(cairo_matrix_t*) m);
    pango_cairo_show_layout(gc, layout);
    cairo_identity_matrix(gc);
    GC_FREE_TEXT(gui,vod,gcd,gc);
}

/*======================================================================*/
/*= initialization and free of module                                  =*/
extern int gui_fonts_getInfo(Tvgui*gui, CfontDef* fdef, double pts,
        double *adx, double *idy, double *asc, double *ils)
{
    double   fontratio;
    TfontEGS fegs = gui_font_load0(gui,fdef, pts, &fontratio);

    if ( FONTEGS_IsNone(fegs) ) {
        return -1;
    }
    PangoFontDescription* pgFDes = fegs.ptr;
    PangoFontMap*         pgFMap = pango_font_get_font_map(NULL);
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

extern void gui_od_init(Tvgui*gui)
{
    int count,i,j;

    gui->g_main->m_pango = tool_malloc_0( sizeof(*E_Pango) );

#if defined(HAVE_GDK_PANGO_CONTEXT_GET_FOR_DISPLAY) && HAVE_GDK_PANGO_CONTEXT_GET_FOR_DISPLAY==1
    E_Pango->context = gdk_pango_context_get_for_display(E_Display);
#else
{
    GtkWidget* widget = gtk_label_new("");
    E_Pango->context = gtk_widget_get_pango_context(widget);
    gtk_widget_destroy(widget);
}
#endif
    E_Pango->layout  = pango_layout_new(E_Pango->context);

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

extern void gui_od_free(Tvgui* gui)
{
    g_object_unref( E_Pango->layout  );
    g_object_unref( E_Pango->context );
    free ( E_Pango );
    gui->g_main->m_pango = NULL;
}
