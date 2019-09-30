/*======================================================================
 *  Copyright 2017 Timothee Denoux 
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
 *$Software: YFIG-GTK (VGUI driver to GTK)
 *      $Id: guiqt-outdev-pixmap.cc 128 2018-04-14 15:53:02Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-pixmap.cc $
 *  $Author: Timothee Denoux (Email: timothee.denoux@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Pixmap and icon handler (EGS driver)                               =*/
/*=                                                                    =*/
/*= See the vgui/vgui-pixmap.h header for the type description and     =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guigtk.h"

#include "tools/bitmap.h"

#include "vgui/vgui-pixmap.h"

/*======================================================================*/
/*= VGUI interface for pixmap.                                         =*/

extern TpixmapEGS  gui_pixmap_create(Tvgui* gui, int wd, int ht)
{
    GdkPixbuf* pixmap  = gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,wd,ht);
    return PXMgtk2egs(pixmap);
}

extern TpixmapEGS  gui_pixmap_create_FromBitmap(Tvgui* gui,
   TcolorEGS fg, TcolorEGS bg, int width, int height, CstrBrief _bitmap)
{
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,width,height);
    GdkRGBA *fgclr = GtkCOLOR(fg);
    GdkRGBA *bgclr = GtkCOLOR(bg);
    cairo_t *cr = cairo_create (surface);
    int isrc,x,y,i;
    for (isrc=0,y=0 ; y<height ; y+=1 ) {
        for ( x=0 ; x<width ; x+=8,isrc+=1 ) {
            unsigned char v = _bitmap[isrc];
            for(i=0; i<8 && (x+i)<width ; i+=1){
                if( (v&(1<<(i)))!=0 ) {
                    cairo_set_source_rgb(cr, fgclr->red,fgclr->green,fgclr->blue);
                } else {
                    cairo_set_source_rgb(cr, bgclr->red,bgclr->green,bgclr->blue);
                }
                cairo_rectangle(cr,x+i,y,1,1);
                cairo_stroke(cr);
            }
        }
    }
    GdkPixbuf *pixmap = gdk_pixbuf_get_from_surface(surface,0,0,width,height);
    cairo_surface_destroy (surface);    
    cairo_destroy (cr);
    return PXMgtk2egs(pixmap);
}

extern void gui_pixmap_free(Tvgui* gui, TpixmapEGS pixmap)
{ if (PXMegs2gtk(pixmap)) g_object_unref(PXMegs2gtk(pixmap)); }

extern void gui_pixmap_draw(Tvgui_outdev*des, int xdes, int ydes,
                            Tvgui_outdev*src, int xsrc, int ysrc, int dx, int dy)
{
    if (src->v_ident == VGUI_ODID_Pixmap ) {
        gui_pixmap_drawPM(des,xdes,ydes,src->v_pixmap,xsrc,ysrc,dx,dy);       
    } else {
        PERM_error_NYI();
    }
}

extern void gui_pixmap_drawPM(
                Tvgui_outdev*des, int xdes, int ydes,
                TpixmapEGS  src, int xsrc, int ysrc, int dx, int dy)
{ gui_pixmap_drawPMgc(des,OD_GC_SysDraw, xdes, ydes, src, xsrc, ysrc, dx,dy); }

extern void gui_pixmap_drawPMgc(
                Tvgui_outdev*des, Tod_GCdef gcd, int xdes, int ydes,
                TpixmapEGS  src, int xsrc, int ysrc, int dx, int dy)
{   
    Tvgui_odpixmap* pod = (Tvgui_odpixmap*)des;
    Tvgui* gui = des->v_gui;
    if ( des->v_ident == VGUI_ODID_Pixmap ) {
        gui_pixmap_cpy(gui,des->v_pixmap,xdes,ydes,src,xsrc,ysrc,dx,dy);
        cairo_t *cr = cairo_create(SURFegs2gtk(pod->vv_target));
        gdk_cairo_set_source_pixbuf(cr,PXMegs2gtk(des->v_pixmap),0,0);
        cairo_paint(cr);
        cairo_destroy (cr);
    } else {
        GtkWidget*   widget = (GtkWidget *)des->v_cvsWidEGS;
        Tgui_daData* dt     = odcvs_getOdData(widget);
        GdkPixbuf *pxm = PXMegs2gtk(src);
        cairo_t *cr = cairo_create(cairo_get_target(dt->main_gc));
        double sx = dx; sx/= gdk_pixbuf_get_width(pxm);
        double sy = dy; sy/= gdk_pixbuf_get_height(pxm);
        double sxdes = xdes; sxdes /= sx;
        double sydes = ydes; sydes /= sy;
        cairo_scale(cr,sx,sy);
        gdk_cairo_set_source_pixbuf(cr,pxm,round(sxdes),round(sydes));
        cairo_paint(cr);
        cairo_destroy (cr);
    }
}

extern void gui_pixmap_cpy(Tvgui*gui,
            TpixmapEGS des, int xdes, int ydes,
            TpixmapEGS src, int xsrc, int ysrc,
            int dx, int dy)
{
    GdkPixbuf *pixmapsrc = PXMegs2gtk(src);
    GdkPixbuf *pixmapdes = PXMegs2gtk(des);
    gdk_pixbuf_copy_area(pixmapsrc,xsrc,ysrc,dx,dy,pixmapdes,xdes,ydes);
}

/*======================================================================*/
/*= VGUI interface for image (cairo surface).                          =*/

extern TimageEGS gui_image_init(Tvgui* gui, int wd, int ht)
{
    TimageEGS img;
    img.dx  = wd;
    img.dy  = ht;
    img.egs = SURFgtk2egs( cairo_image_surface_create( CAIRO_FORMAT_ARGB32, wd,ht) );
    return img;
}

extern void gui_image_free (Tvgui* gui, TimageEGS img)
{ if ( IMGEGS_IsDefined(img) ) cairo_surface_destroy( SURFegs2gtk(img) ); }

extern void gui_image_cpyPxm(Tvgui* gui, TimageEGS  des, TpixmapEGS src)
{
    cairo_t* cr = cairo_create(SURFegs2gtk(des));
    gdk_cairo_set_source_pixbuf(cr,PXMegs2gtk(src),0,0);
    cairo_paint(cr);
    cairo_destroy( cr );
}

/*======================================================================*/
/*= Interface for output device.                                       =*/

static TpixmapEGS odvt_pxm_new(void*  dev, int width, int height)
{ TVARS_VodGui(dev); return gui_pixmap_create(gui,width, height); }

static TpixmapEGS odvt_pxm_newFromBtm(void*  dev, Cbitmap*btm,
                                           Ccolor*fgc, Ccolor* bgc)
{  TVARS_VodGui(dev); return gui_pixmap_create_FromBitmap(gui,
       fgc->egs,bgc->egs, btm->dx,btm->dy, (char*)btm->bits->data); }

static TpixmapEGS odvt_pxm_newFromImg(void*dev, int width, int height,
                                      TimageEGS*img)
{
    cairo_surface_t* surface = SURFegs2gtk(*img);
    GdkPixbuf *pxm = gdk_pixbuf_get_from_surface(surface,0,0,img->dx,img->dy);
    return  PXMgtk2egs(pxm);
}

static void odvt_pxm_delete(void*dev, TpixmapEGS pxm)
{   TVARS_VodGui(dev); gui_pixmap_free(gui,pxm); }

static void odvt_pxm_draw (void*dev, Tod_GCdef gcd, double x, double y,
                           TpixmapEGS pxm, int xp, int yp, int dxp, int dyp)
{
    Toutdev* od = (Toutdev*)dev;
    int xd = CONV_X_S2D(od,x);
    int yd = CONV_Y_S2D(od,y);
    gui_pixmap_drawPMgc((Tvgui_outdev*)dev, gcd, xd, yd, pxm, xp,yp,dxp,dyp);
}


static TimageEGS  odvt_img_load(void*dev, CstrBrief path, CstrBrief opath)
{
    TimageEGS ret;
    GError *error = NULL;
    GdkPixbuf *pxm = gdk_pixbuf_new_from_file(path,&error);
    if(pxm == NULL)
        return IMGEGS_none;
    int width = gdk_pixbuf_get_width(pxm);
    int height = gdk_pixbuf_get_height(pxm);
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,width,height);
    cairo_t *cr = cairo_create(surface);
    gdk_cairo_set_source_pixbuf(cr,pxm,0,0);
    cairo_paint(cr);
    g_object_unref( G_OBJECT(pxm));
    cairo_destroy (cr);
    
    ret.dx = width; ret.dy = height; ret.egs = SURFgtk2egs(surface);
    return ret;
}

static void odvt_img_free(void*dev, TimageEGS img) { gui_image_free( ((Tvgui_outdev*)dev)->v_gui,img); }
static void odvt_img_transform(void*dev, TimageEGS* img, int h, int v, int r)
{
    cairo_surface_t* surface = SURFegs2gtk(*img);
    GdkPixbuf *pxm = gdk_pixbuf_get_from_surface(surface,0,0,img->dx,img->dy);
    GdkPixbuf *pxm2;
    if ( h==1) {
        pxm2 = gdk_pixbuf_flip(pxm,TRUE);
        g_object_unref(G_OBJECT(pxm));
        pxm = pxm2;
    }
    if ( v==1) {
        pxm2 = gdk_pixbuf_flip(pxm,FALSE);
        g_object_unref(G_OBJECT(pxm));
        pxm = pxm2;
    }
    if ( r==1 || r==3 ) {
        pxm2 = gdk_pixbuf_rotate_simple(pxm,
          r==3 ? GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE :GDK_PIXBUF_ROTATE_CLOCKWISE); 
        g_object_unref(G_OBJECT(pxm));
        pxm = pxm2;
    }
    img->dx = gdk_pixbuf_get_width(pxm);
    img->dy = gdk_pixbuf_get_height(pxm);
    cairo_surface_destroy(surface);
    surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32,img->dx,img->dy);
    cairo_t* cr = cairo_create(surface);
    gdk_cairo_set_source_pixbuf(cr,pxm,0,0);
    cairo_paint(cr);
    cairo_destroy(cr);
    g_object_unref(G_OBJECT(pxm));
    img->egs = SURFgtk2egs(surface);
}

/*======================================================================*/
