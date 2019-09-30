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
 *      $Id: guixt-outdev-vt-ldaf.c 113 2018-01-09 18:35:22Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-outdev-vt-ldaf.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= output device implementation (ldaf)                                =*/
/*=                                                                    =*/
/*= This file is included by guixt-outdevice-vt.c.                     =*/
/*======================================================================*/

#define STV static void

/*======================================================================*/
/*= local tools                                                        =*/

#define NOALLOC 100

static XPoint* CONV_PTS_Ds2Win(Coutdev*od, XPoint*dest,
                               Cdble2* src ,int n)
{
      int i;
      XPoint* ret = n<=NOALLOC ? dest : malloc(n*sizeof(*ret));
      for (i=0 ; i<n ; i+=1) {
          ret[i].x = CONV_X_S2D(od,src[i].x);
          ret[i].y = CONV_Y_S2D(od,src[i].y);
      }
      return ret;
}

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

STV odvt_draw_pointXY(const void*dev, Tod_GCdef gcd, double _x, double _y)
{
    C_STD_VARS;
    int x = VCONV_X_S2D(vod,_x);
    int y = VCONV_Y_S2D(vod,_y);
    XDrawPoint(E_Display,VDEV_Drawable(vod),E_GCcache(gcd),x,y);
}

STV odvt_draw_pointT (const void*dev, Tod_GCdef gcd,
                      Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    XPoint  ptsStatic[NOALLOC];
    XPoint* pts;
    pts = CONV_PTS_Ds2Win(dev,ptsStatic,_pts,ptNb);
    XDrawPoints(E_Display,VDEV_Drawable(vod),E_GCcache(gcd),
            pts,ptNb,CoordModeOrigin);
    if (pts!=ptsStatic) free(pts);
}

STV odvt_draw_lineXY(const void*dev, Tod_GCdef gcd,
                     double _x1, double _y1,  double _x2, double _y2)
{
    C_STD_VARS;
    int x1 = VCONV_X_S2D(vod,_x1);
    int y1 = VCONV_Y_S2D(vod,_y1);
    int x2 = VCONV_X_S2D(vod,_x2);
    int y2 = VCONV_Y_S2D(vod,_y2);
    XDrawLine(E_Display, VDEV_Drawable(vod), E_GCcache(gcd), x1,y1,x2,y2);
}

// lines: set: p1 p2 p3 p4 p5 p6 ---> seg: p1-p2, p3-p4, p5-p6
//STV odvt_draw_lineT (const void*dev, Tod_GCdef gcd,
//                     Cdble2* pts, int ptNb);

// multi-line: set: p1 p2 p3 p4 ---> seg: p1-p2, p2-p3, p3-p4
STV odvt_draw_mlineT(const void*dev, Tod_GCdef gcd,
                     Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    XPoint  ptsStatic[NOALLOC];
    XPoint* pts = CONV_PTS_Ds2Win(dev,ptsStatic,_pts,ptNb);
    XDrawLines(E_Display,VDEV_Drawable(vod),E_GCcache(gcd),
               pts,ptNb,CoordModeOrigin);
    if (pts!=ptsStatic) free(pts);
}

STV odvt_fill_mlineT(const void*dev, Tod_GCdef gcd,
                     Cdble2* _pts, int ptNb)
{
    C_STD_VARS;
    XPoint  ptsStatic[NOALLOC];
    XPoint* pts = CONV_PTS_Ds2Win(dev,ptsStatic,_pts,ptNb);
    XFillPolygon(E_Display, VDEV_Drawable(vod), E_GCcache(gcd),
            pts, ptNb, Complex, CoordModeOrigin);
    if (pts!=ptsStatic) free(pts);
}

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

STV odvt_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                            double _x,    double _y,
                            double width, double height)
{
    C_STD_VARS;
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);
    XDrawRectangle(E_Display, VDEV_Drawable(vod), E_GCcache(gcd),
                    x,y,wd,ht);
}

STV odvt_fill_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                            double _x,     double _y,
                            double width, double height)
{
    C_STD_VARS;
    int x  = VCONV_X_S2D(vod,_x);
    int y  = VCONV_Y_S2D(vod,_y);
    int wd = VCONV_D_S2D(vod,width);
    int ht = VCONV_D_S2D(vod,height);
    XFillRectangle(E_Display, VDEV_Drawable(vod), E_GCcache(gcd),
                    x,y,wd,ht);
}

/*======================================================================*/
