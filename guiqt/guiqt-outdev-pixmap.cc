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
 *      $Id: guiqt-outdev-pixmap.cc 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-pixmap.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Pixmap and icon handler (EGS driver)                               =*/
/*=                                                                    =*/
/*= See the vgui/vgui-pixmap.h header for the type description and     =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "tools/bitmap.h"
#include "vgui/vgui-pixmap.h"

/*======================================================================*/
/*= VGUI interface for pixmap.                                         =*/

extern TpixmapEGS  gui_pixmap_create(Tvgui* gui, int wd, int ht)
{
    QPixmap* pixmap  = new QPixmap(wd,ht);
    return PXMqt2egs(pixmap);
}

extern TpixmapEGS  gui_pixmap_create_FromBitmap(Tvgui* gui,
   TcolorEGS fg, TcolorEGS bg, int width, int height, CstrBrief _bitmap)
{
    Tgui_main* main  = gui->g_main;
    char tmp[10000];
    int i,len;
    len  = sprintf(tmp     ,"#define x_width  %d\n",width);
    len += sprintf(tmp+len ,"#define x_height %d\n",height);
    len += sprintf(tmp+len ,"static unsigned char x_bits[] = {\n");
    for ( i=0; i<VGUI_BTM_DataSize(width,height) ; i+=1)
        len +=sprintf(tmp+len,"0x%02x,",_bitmap[i]&0xFF);
    len+=sprintf(tmp+len,"0x00\n};\n");

    QBitmap* bitmap = new QBitmap(width,height);
    if ( bitmap->loadFromData( (const unsigned char*)tmp, len, "XBM")==0 ) {
        vgui_ftlerr_printf(gui,"can not allocate pixmap.");
        exit(1);
    }

//printf("%s: wd=%d ht=%d, (%d %d) len=%d\n",__func__,bitmap->width(),bitmap->height(),width,height,len);
#ifdef QT_VERS_4
    QPixmap* pixmap = new QPixmap(bitmap->size());
    pixmap->fill( QtCOLOR(bg) );
    QPainter painter(pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(QtCOLOR(fg));                    // 1-bits in bitmap fg  
    painter.setBackgroundMode(Qt::TransparentMode); // 0-bits in bitmap transparent
    painter.drawPixmap(0,0,*bitmap,0,0,width,height);
    painter.end();
#endif
#ifdef QT_VERS_5
    QPixmap pixmapfg(bitmap->size());
    pixmapfg.fill( QtCOLOR(fg) );
    pixmapfg.setMask( *bitmap );
    QPixmap* pixmap = new QPixmap(bitmap->size());
    pixmap->fill( QtCOLOR(bg) );
    QPainter painter(pixmap);
    painter.drawPixmap(0,0,pixmapfg,0,0,width,height);
    painter.end();
#endif
    delete bitmap;

    return PXMqt2egs(pixmap);
}

extern void gui_pixmap_free(Tvgui* gui, TpixmapEGS pixmap)
{ delete PXMegs2qt(pixmap); }

extern void          gui_pixmap_draw(Tvgui_outdev*des, int xdes, int ydes,
                                     Tvgui_outdev*src, int xsrc, int ysrc,
                                     int dx, int dy)
{
    int gcd = OD_GC_SysDraw;
    Tvgui*       gui  = des->v_gui;
    TGUI_gc*     gc   = E_GC(gcd); // main->m_gccache+gcd;
    QPixmap* srcpixmap;
    if ( src->v_ident == VGUI_ODID_Canvas ) {
        TGUI_DrawingArea* area = (TGUI_DrawingArea*)src->v_cvsWidEGS;
        srcpixmap = area->drawnPicture;
    } else {
        srcpixmap = PXMegs2qt(src->v_pixmap);
    }
    if ( des->v_ident == VGUI_ODID_Canvas ) {
        TGUI_DrawingArea* area = (TGUI_DrawingArea*)des->v_cvsWidEGS;
        gc->t_painterOpen(area->painter,area->drawnPicture);
          area->painter->drawPixmap(xdes,ydes,*srcpixmap,xsrc,ysrc,dx,dy);
        gc->t_painterClose(area->painter,area);
    } else {
        QPainter painter;
        QPixmap* pixmap = PXMegs2qt(des->v_pixmap);
        gc->painterOpen(&painter, pixmap);
          painter.drawPixmap(xdes,ydes,*srcpixmap,xsrc,ysrc,dx,dy);
        gc->painterClose(&painter);
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
    Tvgui*       gui   = des->v_gui;
    TGUI_gc*     gc    = E_GC(gcd); // main->m_gccache+gcd;
    QPixmap* srcpixmap = PXMegs2qt(src);
    if ( des->v_ident == VGUI_ODID_Canvas ) {
        TGUI_DrawingArea* area = (TGUI_DrawingArea*)des->v_cvsWidEGS;
        gc->t_painterOpen(area->painter,area->drawnPicture);
          area->painter->drawPixmap(xdes,ydes,*srcpixmap,xsrc,ysrc,dx,dy);
        gc->t_painterClose(area->painter,area);
    } else {
        QPainter painter;
        QPixmap* pixmap = PXMegs2qt(des->v_pixmap);
        gc->painterOpen(&painter, pixmap);
          painter.drawPixmap(xdes,ydes,*srcpixmap,xsrc,ysrc,dx,dy);
        gc->painterClose(&painter);
    }
}

extern void gui_pixmap_cpy(Tvgui*gui,
            TpixmapEGS des, int xdes, int ydes,
            TpixmapEGS src, int xsrc, int ysrc,
            int dx, int dy)
{
    QPainter painter;
    int      gcd   = OD_GC_SysDraw;
    TGUI_gc* gc    = E_GC(gcd); // main->m_gccache+gcd;
    QPixmap* srcpixmap = PXMegs2qt(src);
    QPixmap* despixmap = PXMegs2qt(des);
    gc->painterOpen(&painter, despixmap);
      painter.drawPixmap(xdes,ydes,*srcpixmap,xsrc,ysrc,dx,dy);
    gc->painterClose(&painter);
}

/*======================================================================*/
/*= VGUI interface for image (cairo surface).                          =*/

extern TimageEGS gui_image_init(Tvgui* gui, int wd, int ht) { return IMGEGS_none; }
extern void      gui_image_free(Tvgui* gui, TimageEGS img)  { if ( IMGEGS_IsDefined(img) ) delete ((QImage*)img.egs.ptr); }
extern void      gui_image_cpyPxm(Tvgui* gui, TimageEGS  des, TpixmapEGS src) { }

/*======================================================================*/
/*= Interface for output device.                                       =*/

static TpixmapEGS odvt_pxm_new(void*  dev, int width, int height)
{ TVARS_VodGui(dev); return gui_pixmap_create(gui,width, height); }

static TpixmapEGS odvt_pxm_createFromBtm(void*  dev, Cbitmap*btm,
                                           Ccolor*fgc, Ccolor* bgc)
{  TVARS_VodGui(dev); return gui_pixmap_create_FromBitmap(gui,
        fgc->egs,bgc->egs, btm->dx,btm->dy, (char*)btm->bits->data); }

static TpixmapEGS odvt_pxm_newFromImg(void*dev, int width, int height,
                                      TimageEGS*img)
{
    QImage* qimg   = (QImage*)img->egs.ptr;
    QImage  scaled = qimg->scaled(width,height); 
    QPixmap*pxm    =  new QPixmap( QPixmap::fromImage(scaled) );
    return  PXMqt2egs(pxm);
}

static void odvt_pxm_delete(void*dev, TpixmapEGS pxm)
{ TVARS_VodGui(dev); gui_pixmap_free(gui,pxm); }

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
    QImage* qimg = new QImage(path);

    if ( qimg->isNull() ) {
        delete qimg;
        return IMGEGS_none;
    } else {
        TimageEGS img;
        img.dx      = qimg->width();
        img.dy      = qimg->height();
        img.egs.ptr = qimg; 
        return img;
    }
}

static void odvt_img_free(void*dev, TimageEGS img) { gui_image_free( ((Tvgui_outdev*)dev)->v_gui,img); }
static void odvt_img_transform(void*dev, TimageEGS* img, int h, int v, int r)
{
    QImage* nqimg;
    QImage* qimg  = (QImage*)img->egs.ptr;
    if ( h==1 || v==1 ) {
        nqimg = new QImage( qimg->mirrored(h==1,v==1) );
        delete qimg;
        qimg         = nqimg;
        img->egs.ptr = qimg;
    }
    if ( r==1 || r==3 ) {
        QMatrix rm;
        rm.rotate(r==1 ? 90 : -90);
        nqimg = new QImage( qimg->transformed(rm) );
        delete qimg;
        qimg         = nqimg;
        img->egs.ptr = qimg;
        img->dx = qimg->width();
        img->dy = qimg->height();
    }
}

/*======================================================================*/
