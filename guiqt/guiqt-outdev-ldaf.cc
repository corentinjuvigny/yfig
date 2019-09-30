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
 *      $Id: guiqt-outdev-ldaf.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-ldaf.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing unfilled or filled curves, lines into a      =*/
/*= Tvgui_outdev.                                                      =*/ 
/*=                                                                    =*/
/*= See the "vgui/vgui-ldaf-ll.h" header for the function desĉription  =*/
/*= and for the documentation.                                         =*/
/*======================================================================*/

#define STV static void

#define CVARS_VodGuiGc(dev,gcd) CVARS_VodGui(dev); TGUI_gc* gc=E_GC(gcd)

/*======================================================================*/
/*= local tools                                                        =*/

#define NOALLOC 100

static QPoint* CONV_PTS_S2D(Cvgui_outdev*vod, QPoint*dest,
                               Cdble2* src ,int n)
{
      int i;
      QPoint* ret = n<=NOALLOC ? dest : new QPoint [n];
      for (i=0 ; i<n ; i+=1) {
          ret[i].setX( VCONV_X_S2D(vod,src[i].x) );
          ret[i].setY( VCONV_Y_S2D(vod,src[i].y) );
      }
      return ret;
}

static QPoint* CONV_PTS2SEG_S2D(Cvgui_outdev*vod, QPoint*dest,
                               Cdble2* src ,int n)
{
      int i,j;
      int len = (n-1)*2;
      QPoint* ret = len<=NOALLOC ? dest : new QPoint [len];

      ret[0].setX( VCONV_X_S2D(vod,src[0].x) );
      ret[0].setY( VCONV_Y_S2D(vod,src[0].y) );
      for (i=j=1 ; i<n-1 ; i+=1,j+=2) {
          ret[j].setX( VCONV_X_S2D(vod,src[i].x) );
          ret[j].setY( VCONV_Y_S2D(vod,src[i].y) );
          ret[j+1]=ret[j];
      }
      ret[j].setX( VCONV_X_S2D(vod,src[i].x) );
      ret[j].setY( VCONV_Y_S2D(vod,src[i].y) );
      return ret;
}

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

STV odvt_draw_pointXY(const void*dev, Tod_GCdef gcd, double _x, double _y)
{
    CVARS_VodGuiGc(dev,gcd);
    int x = VCONV_X_S2D(vod,_x);
    int y = VCONV_Y_S2D(vod,_y);
    QPainter* painter = gc->painterOpen(vod);
      painter->drawPoint(x,y);
    gc->painterClose(vod);
}

STV odvt_draw_pointT(const void*dev, Tod_GCdef gcd, Cdble2* _pts, int ptNb)
{
    CVARS_VodGuiGc(dev,gcd);
    QPoint  ptsStatic[NOALLOC];
    QPoint* pts = CONV_PTS_S2D(vod,ptsStatic,_pts,ptNb);
    QPainter* painter = gc->painterOpen(vod);
      painter->drawPoints(pts,ptNb);
    gc->painterClose(vod);
    if (pts!=ptsStatic) delete[] pts;
}

STV odvt_draw_lineXY (const void*dev, Tod_GCdef gcd,
        double _x1, double _y1,  double _x2, double _y2)
{
    CVARS_VodGuiGc(dev,gcd);
    int x1 = VCONV_X_S2D(vod,_x1);
    int y1 = VCONV_Y_S2D(vod,_y1);
    int x2 = VCONV_X_S2D(vod,_x2);
    int y2 = VCONV_Y_S2D(vod,_y2);
    QPainter* painter = gc->painterOpen(vod);
      painter->drawLine(x1,y1,x2,y2);
    gc->painterClose(vod);
}

// lines: set: p1 p2 p3 p4 p5 p6 ---> seg: p1-p2, p3-p4, p5-p6
//STV odvt_draw_lineT (const void*dev, Tod_GCdef gcd,
//                     Cdble2* pts, int ptNb);

// multi-line: set: p1 p2 p3 p4 ---> seg: p1-p2, p2-p3, p3-p4
STV odvt_draw_mlineT (const void*dev, Tod_GCdef gcd, Cdble2* _pts, int ptNb)
{
    CVARS_VodGuiGc(dev,gcd);
    QPoint  ptsStatic[NOALLOC];
    QPoint* pts = CONV_PTS_S2D(vod,ptsStatic,_pts,ptNb);
    QPainter* painter = gc->painterOpen(vod);
      painter->drawPolyline(pts,ptNb);
    gc->painterClose(vod);
    if (pts!=ptsStatic) delete[] pts;
}

STV odvt_fill_mlineT(const void*dev, Tod_GCdef gcd, Cdble2* _pts, int ptNb)
{
    CVARS_VodGuiGc(dev,gcd);
    QPoint  ptsStatic[NOALLOC];
    QPoint* pts = CONV_PTS_S2D(vod,ptsStatic,_pts,ptNb);
    QPainter* painter = gc->painterOpen(vod);
      painter->setPen(Qt::NoPen);
      painter->setBrush(gc->fillBrush);
      painter->drawPolygon(pts,ptNb);
    gc->painterClose(vod);
    if (pts!=ptsStatic) delete[] pts;
}

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

STV odvt_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                       double _x, double _y, double width, double height)
{
    CVARS_VodGuiGc(dev,gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);
    QPainter* painter = gc->painterOpen(vod);
      painter->drawRect(x,y,wd,ht);
    gc->painterClose(vod);
}

STV odvt_fill_rectangleXYWH(const void*dev, Tod_GCdef gcd,
        double _x, double _y, double width, double height)
{
    CVARS_VodGuiGc(dev,gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);
    QPainter* painter = gc->painterOpen(vod);
      painter->setPen(Qt::NoPen);
      painter->fillRect(x,y,wd,ht, gc->fillBrush);
    gc->painterClose(vod);
}

/*======================================================================*/
/*= ellipses and ellipsoid arcs                                        =*/

#if 0
STV odvt_draw_arcXY(const void*dev, Tod_GCdef gcd,
                double _x, double _y,
                double _wd, double _ht, double a1, double a2)
{
    Tvgui*   gui = dev->v_gui;
    TGUI_gc* gc  = E_GC(gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,_wd);
    int ht = VCONV_D_S2D(vod,_ht);
    QPainter* painter = gc->painterOpen(dev);
      painter->drawChord(x,y,wd,ht,a1,a2);
    gc->painterClose(dev);
}

STV odvt_fill_arcXY  (const void*dev, Tod_GCdef gcd,
                double _x, double _y,
                double _wd, double _ht, double a1, double a2)
{
    Tvgui*   gui = dev->v_gui;
    TGUI_gc* gc  = E_GC(gcd);
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,_wd);
    int ht = VCONV_D_S2D(vod,_ht);
    QPainter* painter = gc->painterOpen(dev);
      painter->setPen(Qt::NoPen);
      painter->setBrush(gc->fillBrush);
      painter->drawChord(x,y,wd,ht,a1,a2);
    gc->painterClose(dev);
}
#endif

/*======================================================================*/
