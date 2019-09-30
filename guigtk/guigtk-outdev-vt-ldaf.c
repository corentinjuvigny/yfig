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
 *$Software: YFIG-GTK
 *      $Id: guigtk-outdev-vt-ldaf.c 113 2018-01-09 18:35:22Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-outdev-vt-ldaf.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= output device implementation (ldaf)                                =*/
/*=                                                                    =*/
/*= This file is included by guigtk-outdevice-vt.c.                    =*/
/*======================================================================*/

#define STV static void

/*======================================================================*/
/*= local tools                                                        =*/

#define GC_INIT_STROKEFILL(gui,vod,gcd, stroke) \
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas && \
         odcvs_getOdData(vod->v_cvsWidEGS)->in_xes!=0 ) return; \
    cairo_t *gc;\
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas && \
         odcvs_getOdData(vod->v_cvsWidEGS)->in_xds!=0 ) { \
         gc = odcvs_getOdData(vod->v_cvsWidEGS)->xor_gc;\
         gc_init(&vod->v_od,gui,gc,gcd,od_style_top(&vod->v_od, gcd),stroke); \
    } else \
        gc = vod->v_vtable->topVT.ident == VGUI_ODID_Canvas \
        ? gc_init_cvs(gui,(Tvgui_outdev*)  vod,gcd,stroke) \
        : gc_init_pxm(gui,(Tvgui_odpixmap*)vod,gcd,stroke)
#define GC_INIT_STROKE(gui,vod,gcd) GC_INIT_STROKEFILL(gui,vod,gcd,1)
#define GC_INIT_FILL(gui,vod,gcd)   GC_INIT_STROKEFILL(gui,vod,gcd,0)
#define GC_INIT_TEXT(gui,vod,gcd,a) \
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas && \
         odcvs_getOdData(vod->v_cvsWidEGS)->in_xes!=0 ) return; \
    cairo_t *gc;\
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas && \
         odcvs_getOdData(vod->v_cvsWidEGS)->in_xds!=0 ) { \
         gc = odcvs_getOdData(vod->v_cvsWidEGS)->xor_gc;\
         gc_init_text(&vod->v_od,gui,gc,gcd,od_style_topText(&vod->v_od, gcd),a); \
    } else \
        gc = vod->v_vtable->topVT.ident == VGUI_ODID_Canvas \
           ? gc_init_cvs_text(gui,(Tvgui_outdev*) vod,gcd,a) \
           : gc_init_pxm_text(gui,(Tvgui_odpixmap*)vod,gcd,a)
#define GC_FREE_STROKE(gui,vod,gcd,gc) \
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas ) \
         gc_free_cvs_stroke(gui,(Tvgui_outdev*)  vod,gcd,gc); \
    else gc_free_pxm_stroke(gui,(Tvgui_odpixmap*)vod,gcd,gc)
#define GC_FREE_FILL(gui,vod,gcd,gc) \
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas ) \
         gc_free_cvs_fill(gui,(Tvgui_outdev*)  vod,gcd,gc); \
    else gc_free_pxm_fill(gui,(Tvgui_odpixmap*)vod,gcd,gc)
#define GC_FREE_TEXT(gui,vod,gcd,gc) \
    if ( vod->v_vtable->topVT.ident == VGUI_ODID_Canvas ) \
         gc_free_cvs_text(gui,(Tvgui_outdev*)  vod,gcd,gc); \
    else gc_free_pxm_text(gui,(Tvgui_odpixmap*)vod,gcd,gc)

/*======================================================================*/
static void gc_init(Coutdev* od, Tvgui* gui, cairo_t* gc, Tod_GCdef gcd, Cstyle* sty, int stroke) 
{
    double dash[100];

    Csty_text* stt = &sty->sty_text;
    Csty_line* stl = &sty->sty_line;
    Csty_fill* stf = &sty->sty_fill;

    cairo_set_line_width(gc, stl->stl_thickness <= 0 ? 1 : stl->stl_thickness);
    cairo_set_line_cap(gc, conv_capStyle_OdGtk(stl->stl_cap));
    cairo_set_line_join(gc, conv_joinStyle_Od2Gtk(stl->stl_join));
    cairo_set_fill_rule(gc, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_set_operator(gc, CAIRO_OPERATOR_SOURCE);

    //TEMP_ase_printf(stl->stl_style != DA_LS_Solid, "invalid linestyle (%d)", stl->stl_style); 
    switch (stl->stl_style) {
        case DA_LS_Solid :
            cairo_set_dash(gc, NULL, 0, 0);
            break;
        case DA_LS_Dot :
            dash[0] = od->resources->dot[0];
            dash[1] = od->resources->dot[1];
            cairo_set_dash(gc, dash, 2, 0);
            break;
        case DA_LS_Dash :
            dash[0] = od->resources->dash[0];
            dash[1] = od->resources->dash[1];
            cairo_set_dash(gc, dash, 2, 0);
            break;
        case DA_LS_Dash1Dot :
dash1_dot1 :
        dash[0] = od->resources->dash1dot1[0];
        dash[1] = od->resources->dash1dot1[1];
        dash[2] = od->resources->dash1dot1[2];
        dash[3] = od->resources->dash1dot1[3]; 
        cairo_set_dash(gc, dash, 4, 0);
        break;
    case DA_LS_Dash2Dot :
        tool_errerr_printf(od->msgdrv, "%s", "Style 1 Dashs 2 Dots not implemented, use 1 dash 1 dot instead");
        goto dash1_dot1;
    case DA_LS_Dash3Dot :
        tool_errerr_printf(od->msgdrv, "%s", "Style 1 Dashs 3 Dots not implemented, use 1 dash 1 dot instead");
        goto dash1_dot1;    
    default :
        PERM_spe_printf("unexpected line style %d", stl->stl_style);
        goto dash1_dot1;
    }
    if ( stroke || stf->stf_style == DA_FS_Solid ) {
        Tcolor *drawclr = stroke ? stl->stl_color : stf->stf_color ;
        double r = drawclr->rgb.red / 255.;
        double g = drawclr->rgb.green / 255.;
        double b = drawclr->rgb.blue / 255.;
        cairo_set_source_rgb(gc,r,g,b);
    } else if (stf->stf_style == DA_FS_ShadTile ||
        stf->stf_style == DA_FS_LighTile ||
        stf->stf_style == DA_FS_PattTile ) {
        //On a un pixbuf
        GdkPixbuf* pxb = PXMegs2gtk(E_PXM(gcd));    
        cairo_surface_t* s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 
                                                    gdk_pixbuf_get_width(pxb), 
                                                   gdk_pixbuf_get_height(pxb));
        cairo_t* cr = cairo_create(s);
        gdk_cairo_set_source_pixbuf(cr, pxb, 0, 0);
        cairo_paint(cr);
        cairo_pattern_t *pat = cairo_pattern_create_for_surface(s);
        cairo_pattern_set_extend(pat,CAIRO_EXTEND_REPEAT);
        cairo_set_source(gc,pat);
        cairo_destroy(cr);
        cairo_surface_destroy(s);
    }
}

static cairo_t*gc_init_pxm(Tvgui*gui, Tvgui_odpixmap* pod, Tod_GCdef gcd, int stroke)
{
    cairo_t* gc = cairo_create( SURFegs2gtk(pod->vv_target) );
    Tstyle* sty = od_style_top(&pod->vv_od, gcd);
    gc_init(&pod->vv_od, gui, gc, gcd, sty, stroke);
    return gc;
}

static void gc_free_pxm(Tvgui* gui, Tvgui_odpixmap* pod, Tod_GCdef gcd,
                        cairo_t* cr, int stroke)
{
    TEMP_asf_printf( IMGEGS_IsNone(pod->vv_target),"%s","unexpected null for associated cairo surface");
    if(stroke) cairo_stroke(cr); else cairo_fill(cr);
    cairo_destroy(cr);
    GdkPixbuf *pxm = PXMegs2gtk(pod->vv_pixmap);
    if (!odpxm_stdSeq_isStarted(pxm)) {
        GdkPixbuf* px = gdk_pixbuf_get_from_surface(
                SURFegs2gtk(pod->vv_target),0, 0, pod->vv_target.dx, pod->vv_target.dy);
        gdk_pixbuf_copy_area(px, 0, 0, pod->vv_target.dx, pod->vv_target.dy,pxm, 0, 0);
        g_object_unref(G_OBJECT(px));
    }
}
static void gc_free_pxm_stroke(Tvgui* gui, Tvgui_odpixmap* pod, Tod_GCdef gcd, cairo_t* cr) {gc_free_pxm(gui,pod,gcd,cr,1);}
static void gc_free_pxm_fill  (Tvgui* gui, Tvgui_odpixmap* pod, Tod_GCdef gcd, cairo_t* cr) {gc_free_pxm(gui,pod,gcd,cr,0);}

static cairo_t*gc_init_cvs(Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, int stroke)
{
    GtkWidget*   widget = (GtkWidget *)vod->v_cvsWidEGS;
    Tgui_daData* dt     = odcvs_getOdData(widget);
    Tstyle*      sty    = od_style_top(&vod->v_od, gcd);
    gc_init(&vod->v_od, gui, dt->main_gc, gcd, sty, stroke);
    return dt->main_gc;
}
static void gc_free_cvs(Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, cairo_t* cr, int stroke)
{
    GtkWidget*   widget = (GtkWidget *)vod->v_cvsWidEGS;
    Tgui_daData* dt     = odcvs_getOdData(widget);
    
    TEMP_asf_printf( dt->in_xes!=0,"%s","unexpected call inside  xorEraseSection");
    if(stroke) cairo_stroke(cr); else cairo_fill(cr);
    if ( dt->in_ms==0 && dt->in_xds==0 ) {
        int dx = Box_dx(vod->v_boxDes);
        int dy = Box_dy(vod->v_boxDes);
        gtk_widget_queue_draw_area(widget,0,0,Box_dx(vod->v_boxDes),Box_dy(vod->v_boxDes));
    }
}
static void gc_free_cvs_stroke(Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, cairo_t* cr) {gc_free_cvs(gui,vod,gcd,cr,1);}
static void gc_free_cvs_fill  (Tvgui* gui, Tvgui_outdev* vod, Tod_GCdef gcd, cairo_t* cr) {gc_free_cvs(gui,vod,gcd,cr,0);}

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/
STV odvt_draw_pointXY(const void* dev, Tod_GCdef gcd, double _x, double _y)
{
    C_STD_VARS;
    GC_INIT_STROKE(gui,vod,gcd);
    int x = VCONV_X_S2D(vod,_x);
    int y = VCONV_Y_S2D(vod,_y);
    
    cairo_move_to(gc, x, y);
    cairo_line_to(gc, x, y);
    GC_FREE_STROKE(gui,vod,gcd,gc);
}

STV odvt_draw_pointT (const void* dev, Tod_GCdef gcd,
                      Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    GC_INIT_STROKE(gui,vod,gcd);
    Tdble2 pts;
    int i, x, y;
    for (i = 0; i < ptNb; i++) {
        pts = _pts[i];
        x = VCONV_X_S2D(vod, pts.x);
        y = VCONV_Y_S2D(vod, pts.y);
        cairo_move_to(gc, x, y);
        cairo_line_to(gc, x, y);
    }
    GC_FREE_STROKE(gui,vod,gcd,gc);
}

STV odvt_draw_lineXY(const void* dev, Tod_GCdef gcd,
                     double _x1, double _y1,  double _x2, double _y2)
{
    C_STD_VARS;
    
    GC_INIT_STROKE(gui,vod,gcd);
    int x1 = VCONV_X_S2D(vod, _x1);
    int y1 = VCONV_Y_S2D(vod, _y1);
    int x2 = VCONV_X_S2D(vod, _x2);
    int y2 = VCONV_Y_S2D(vod, _y2);
    
    cairo_move_to(gc, x1, y1);
    cairo_line_to(gc, x2, y2);
    GC_FREE_STROKE(gui,vod,gcd,gc);
}

// lines: set: p1 p2 p3 p4 p5 p6 ---> seg: p1-p2, p3-p4, p5-p6
//STV odvt_draw_lineT (const void*dev, Tod_GCdef gcd,
//                     Cdble2* pts, int ptNb);

// multi-line: set: p1 p2 p3 p4 ---> seg: p1-p2, p2-p3, p3-p4
STV odvt_draw_mlineT(const void* dev, Tod_GCdef gcd,
                     Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    GC_INIT_STROKE(gui,vod,gcd);
    int i, x, y;
    Tdble2 pts;

    pts = _pts[0];
    x = VCONV_X_S2D(vod, pts.x);    
    y = VCONV_Y_S2D(vod, pts.y);    
    cairo_move_to(gc, x, y);
    for (i = 1; i < ptNb; i++) {
        pts = _pts[i];
        x = VCONV_X_S2D(vod, pts.x);    
        y = VCONV_Y_S2D(vod, pts.y);    
        cairo_line_to(gc, x, y);
    }
    GC_FREE_STROKE(gui,vod,gcd,gc);
}

STV odvt_fill_mlineT(const void* dev, Tod_GCdef gcd,
                     Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    GC_INIT_FILL(gui,vod,gcd);
    int i, x, y;
    Tdble2 pts;

    pts = _pts[0];
    x = VCONV_X_S2D(vod, pts.x);    
    y = VCONV_Y_S2D(vod, pts.y);    
    cairo_move_to(gc, x, y);

    for (i = 1; i < ptNb; i++) {
        pts = _pts[i];
        x = VCONV_X_S2D(vod, pts.x);    
        y = VCONV_Y_S2D(vod, pts.y);    
        cairo_line_to(gc, x, y);
    }
    
    GC_FREE_FILL(gui,vod,gcd,gc);
}

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

STV odvt_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                            double _x,    double _y,
                            double width, double height)
{
    C_STD_VARS;
    GC_INIT_STROKE(gui,vod,gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);

    cairo_rectangle(gc, x, y, wd, ht);

    GC_FREE_STROKE(gui,vod,gcd,gc);
}

STV odvt_fill_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                            double _x,     double _y,
                            double width, double height)
{
    Coutdev *od = dev;
    C_STD_VARS; 
    GC_INIT_FILL(gui,vod,gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);

    cairo_rectangle(gc, x, y, wd, ht);
    cairo_fill(gc);
    GC_FREE_FILL(gui,vod,gcd,gc);
}

/*======================================================================*/
