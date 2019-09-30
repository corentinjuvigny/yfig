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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: ldaf-vt.h 107 2018-01-07 16:50:33Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/ldaf-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing filled/unfilled lines and curves.            =*/
/*=                                                                    =*/
/*= This API allows:                                                   =*/
/*=   - to build new GC                                                =*/
/*=   - to change GC parameters                                        =*/
/*=   - to draw lines and curves (plain, doted, dashed)                =*/
/*=   - to fill shape with color or pattern.                           =*/
/*=                                                                    =*/
/*= Note: Font handling and string drawing is not handle here but      =*/
/*= there are defined in the deviceVt-str.h handler.                   =*/
/*=                                                                    =*/
/*= Notes:                                                             =*/
/*=  - GC means Graphic Context.                                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_LDAF_VT_H
#define FILE_OUTDEV_LDAF_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/boxes.h"
#include "tools/color-types.h"
#include "outdev/types.h"

/*======================================================================*/
/*= Data structure used to transmit GC parameters to EGS.              =*/

struct _TldafPar {
  // point, line curve, ... drawing
  int               ldaf_linewidth;
  TlineStyle        ldaf_linestyle;
  TjoinStyle        ldaf_joinstyle;
  TcapStyle         ldaf_capstyle;
  double            ldaf_dashlength;
  double            ldaf_dotgap;
  double            ldaf_styleval;
  Ccolor*           ldaf_pencolor;
  // area filling
  TfillStyle        ldaf_fillstyle;
  Ccolor*           ldaf_fillcolor;
};

typedef struct _TldafPar TldafPar;
typedef const   TldafPar CldafPar;

/*======================================================================*/
/*= Interface                                                          =*/
struct _Toutdev_ldaf_vt {
/*======================================================================*/

/*======================================================================*/
/*= GC feature                                                         =*/

/**
 * Create the gc graphic contex using gps graphic parameter and stores it.
 * the xor flag set to 1 indiques that drawing a pixel must xored
 * the existing one.
**/
void (*initGC)(void*dev, Tod_GCdef gcd, CldafPar*dp, int _xor);

//void (*setGCfg)(void*dev, Tod_GCdef gcd, TcolorEGS c);
//void (*setGCbg)(void*dev, Tod_GCdef gcd, TcolorEGS c);

/**
 * The setGCso function sets the drawing attributes of the gcd GC
 * for plain lines or curves (DA_LS_Solid: no dot, no dash)
 *
 * The setGCdd function sets the drawing attributes for
 * dot and dash lines or curves.
 *
 *  - th : line thickness in pixel.
 *  - ls : the dash dot style.
 *  - ddh: The color used for the hole between dot and dash.
 *         EGSCOLOR_DEFAULT means that holes are not drawn.
 *         EGSCOLOR_NONE means the current hole color is preserved.
 *         NOTE: this option is not yet implemented.
**/

void (*setGCso)(void*dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TjoinStyle js,
                TcapStyle  cs, TcolorEGS fg);
void (*setGCdd)(void*dev, Tod_GCdef gcd,
              //TlineStyle ls, char* ddt, int ddn,
                double th, TlineStyle ls, TcolorEGS ddh);

/**
 * The setGCfillColor function sets the drawing attributes of
 * the gcd GC for filling shape with the c color.
 * The setGCfillColorBrightness function sets the drawing attributes of
 * the gcd GC for filling shape with the c color lighten or darken by
 * the coef coefficient.
 * The coef must be in [-1,1] range with -1 meaning dark, 0 the c color
 * unchanged and 1 white.
 *
 * The setGCfillTile function sets the drawing attributes of
 * the gcd GC for tiling shape with the pxm pixmap.
 * The setGCfillTileZ function sets the drawing attributes of
 * the gcd GC for tiling shape with the pxm pixmap scaled by the
 * zf zoom factor.
 *
 * The setGCfillColorBrightness is optional.
 *  If it is missing it is implemened using tiling.
 * The setGCfillTileZ is optional.
**/
void (*setGCfillColor)(void*dev, Tod_GCdef gcd, TcolorEGS c);
void (*setGCfillColorBrightness)(void*dev, Tod_GCdef gcd, TcolorEGS c, double coef);

void (*setGCfillTile) (void*dev, Tod_GCdef gcd, TpixmapEGS pxm);
void (*SetGCfillTileZ)(void*dev, Tod_GCdef gcd, TpixmapEGS pxm,
                       double zf);

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

void  (*draw_pointXY)(const void*dev, Tod_GCdef gcd, double x, double y);
#define draw_pointPT(dev,gcd,pt)  draw_pointXY(dev,gcd,pt.x,pt.y)
void  (*draw_pointT) (const void*dev, Tod_GCdef gcd,
                      Cdble2* pts, int ptNb);
#define draw_pointS(dev,gcd,set)  draw_pointT(\
                             dev,gcd,set->eles,set->eleNb)

void  (*draw_lineXY)(const void*dev, Tod_GCdef gcd,
                     double x1, double y1,  double x2, double y2);
#define draw_linePT(dev,gcd,pt1,pt2) draw_lineXY(dev,gcd, \
                     pt1->x, pt1->y, pt2->x, pt2->y)

void  (*draw_lineT) (const void*dev, Tod_GCdef gcd,
                      Cdble2* pts, int ptNb);
#define draw_lineS(dev,gcd,set)  draw_lineT(\
                             dev,gcd,set->eles,set->eleNb)

// multi-line: set: p1 p2 p3 p4 ---> seg: p1-p2, p2-p3, p3-p4
void  (*draw_mlineT)(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb);
#define draw_mlineS(dev,gcd,set) draw_mlineT(dev,gcd,set->eles,set->eleNb)
void  (*fill_mlineT)(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb);
#define fill_mlineS(dev,gcd,set) fill_mlineT(dev,gcd,set->eles,set->eleNb)

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

void (*draw_rectangleXYWH)(const void*dev, Tod_GCdef gcd,
                           double x, double y,
                           double width, double height);

void (*fill_rectangleXYWH)(const void*dev, Tod_GCdef gcd,
                           double x,     double y,
                           double width, double height);

void (*fill_polygonNT)(const void*dev, Tod_GCdef gcd,
                       Cdble2* pts, int ptNb);

/*======================================================================*/
/*= open and closed curves                                             =*/

void  (*draw_bezier)(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb);
void  (*fill_bezier)(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb);

/*======================================================================*/
/*= ellipses and ellipsoid arcs                                        =*/

void (*draw_ellipseHV)(const void*dev, Tod_GCdef gcd,
                       double cx,  double cy, double rx, double ry);
void (*fill_ellipseHV)(const void*dev, Tod_GCdef gcd,
                       double cx,  double cy, double rx, double ry);
void (*draw_ellipseWR)(const void*dev, Tod_GCdef gcd,
                       double cx,  double cy, double rx, double ry,
                       double a);
void (*fill_ellipseWR)(const void*dev, Tod_GCdef gcd,
                       double cx,  double cy, double rx, double ry,
                       double a);

/*======================================================================*/
};
/*======================================================================*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_LDAF_VT_H
/*======================================================================*/
