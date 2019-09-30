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
 *      $Id: ldaf-hl.h 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/ldaf-hl.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing unfilled or filled curves, lines into a      =*/
/*= Tvgui_device.                                                      =*/ 
/*= HL means High Level.                                               =*/
/*=                                                                    =*/
/*= This API is a front-head to the Low Level drawing API (EGS) that   =*/
/*= takes into account special cases like for instance:                =*/
/*=   - the line thickness (too thick)                                 =*/
/*=   - drawing a  multi-line with zero or only 1 point.               =*/
/*=                                                                    =*/
/*= So it is safer to use these functions (vgui_draw_...) instead of   =*/
/*= their low level equivalents ( gui_draw_...).                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_LDAF_HL_H
#define FILE_OUTDEV_LDAF_HL_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"
#include "tools/color-types.h"
#include "tools/geo2d.h"
#include "outdev/types.h"

/*======================================================================*/

/**
 * The od_ldaf_setGCsoP/Z functions set the drawing attributes of the gcd
 * GC for plain lines or curves (no dot, no dash).
 *
 * The od_ldaf_setGCddZ function sets the drawing attributes for
 * doted and//or dashed lines or curves.
 *
 * The od_ldaf_setGCff function sets the drawing attributes for
 * filling a shape. The used parameters depends of the fs types:
 *      fs         | sidx | lidx | pidx | zf | c  | white | black
 *  DA_FS_Solid    |      |      |      |    | *  |       |
 *  DA_FS_ShadTile |  *   |      |      |  * | bg |  fg   | 
 *  DA_FS_LighTile |      |  *   |      |  * | fg |  bg   | 
 *  DA_FS_PattTile |      |      |  *   |  * | bg |       |  fg
 *
 * Prarameters:
 *  - thp : The thickness of lines or curves.
 *         It must be given in point (1/72 inch).
 *         A 0 value means 1 pixel.
 *  - thz: The thickness of lines or curves.
 *         Be zf the zoom factor, thz*zf is the thickness in
 *         point.
 *         A 0 value means 1 pixel.
 *  - ls,js,cd : join and cup style.
 *         The DA_JS_None and DA_CS_None value mean to keep
 *         the current values.
 *  - fg : The drawing color.
 *         The NULL pointer means to keep the current value.
 *  - ddh: The color used for the hole between dot and dash.
 *         The NULL pointer means that holes are not drawn.
 *         NOTE: this option is not yet implemented.
**/

extern void od_ldaf_setGCsoP(Toutdev*od, Tod_GCdef gcd, double thp,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg);
extern void od_ldaf_setGCsoZ(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg);
extern void od_ldaf_setGCddP(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg,
                             TlineStyle ls, Ccolor* ddh);
extern void od_ldaf_setGCddZ(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg,
                             TlineStyle ls, Ccolor* ddh);
extern void od_ldaf_setGCll(Toutdev*od, Tod_GCdef gcd, double zf,
                            TfillStyle fs, Ccolor* c,
                            int sidx, int lidx, int pidx);

/**
 * The xxx_byStyle functions are wrappers to the xxx functions.
**/
extern void od_ldaf_setGCsoP_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s);
extern void od_ldaf_setGCsoZ_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s);
extern void od_ldaf_setGCddP_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s);
extern void od_ldaf_setGCddZ_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s);
extern void od_ldaf_setGCll_byStyle(Toutdev*od, Tod_GCdef gcd, double zf,
                                    Csty_fill* s);

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

extern void od_draw_pointPT(Coutdev* od, Tod_GCdef gcd, Cdble2* pt);
#define     od_draw_pointXY(dev,gcd,x,y) do { Cdble2 __pt = {x,y}; \
                            od_draw_pointPT(dev,gcd,&__pt); } while (0)
extern void od_draw_linePT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pt1, Cdble2* pt2);
#define     od_draw_lineXY(dev,gcd,x1,y1,x2,y2) do { \
                                Cdble2 __pt1 = {x1,y1}; \
                                Cdble2 __pt2 = {x2,y2}; \
                                od_draw_linePT(dev,gcd,&__pt1,&__pt2); \
                              } while (0)

/**
 * The od_draw_lineS and od_draw_lineT functions draw the lines
 * from pts[i] to pts[i+1] of the pts point set into the od device
 * using the gcd graphic context.
 * If the point number is odd the last point of the set is ignored.
 *
 * Parameters:
 *   - od  :     the device.
 *   - gcd :     the graphic context identifier.
 *   - pts :     the points of the multi-line in source coordinate system.
 *   - pts,npts: the points of the multi-line in source coordinate system.
**/
extern void od_draw_lineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts);
extern void od_draw_lineT(Coutdev* od, Tod_GCdef gcd,
                          Cdble2* pts, int npts);

/**
 * The od_draw_mlineS and od_draw_mlineT functions draw the multi-line
 * chaining the points specified by the pts point set into the od device
 * using the gcd graphic context.
 * It takes in account the special cases like a multi_lines with 
 * 0 or only 1 point
 *
 * Parameters:
 *   - od  :     the device.
 *   - gcd :     the graphic context identifier.
 *   - pts :     the points of the multi-line in source coordinate system.
 *   - pts,npts: the points of the multi-line in source coordinate system.
**/
extern void od_draw_mlineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts);
extern void od_draw_mlineT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pts, int npts);

extern void od_fill_mlineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts);
extern void od_fill_mlineT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pts, int npts);

#if 0
// needed while line object of figure used zXPoint.
// transform the zXPoint list in TpointDbleSet and then calls the
// former vgui_draw_lines function.
extern void vgui_draw_lines_zXPoint(Cvgui_device* dev, Tvgui_GCdef gcd,
                                    zXPoint *points, int npoints);
#endif


/*======================================================================*/
/*= rectangle                                                          =*/

extern void od_draw_rectXYWH(Coutdev*od, Tod_GCdef gcd,
                             double x, double y,
                             double width, double height);
#define     od_draw_rectBB(dev,gcd,b) od_draw_rectXYWH(\
                        dev,gcd, Box_x(b), Box_y(b), Box_dx(b), Box_dy(b))

extern void od_fill_rectXYWH(Coutdev*dev, Tod_GCdef gcd,
                             double x, double y,
                             double width, double height);
#define     od_fill_rectBB(dev,gcd,b) od_fill_rectXYWH(\
                        dev,gcd, Box_x(b), Box_y(b), Box_dx(b), Box_dy(b))

/*======================================================================*/
/*= open and closed spline curves                                      =*/

/**
 * The od_draw_splineS and od_draw_splineT functions draw the multi-line
 * chaining the points specified by the pts point set into the od device
 * using the gcd graphic context.
 * It takes in account the special cases like a multi_lines with 
 * 0 or only 1 point
 *
 * Parameters:
 *   - od  :     the device.
 *   - gcd :     the graphic context identifier.
 *   - pts :     the points of the spline curve in source coordinate system.
 *   - pts,npts: the points of the spline curve in source coordinate system.
**/
extern void od_draw_splineO(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes);
extern void od_draw_splineC(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet*  nodes);
extern void od_fill_splineO(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes);
extern void od_fill_splineC(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet*  nodes);

/*======================================================================*/
/*= ellipses and ellipsoid arcs                                        =*/

/**
 * The od_draw_ellarc function draws on the od output device the ellipse
 * that the e parameter specifies.
 * The lengths (a and b radius) and coordinates (c,p1,p2 points) of the
 * e arc are given in source device unit.
 * The source basis is assumed indirect and orthogonal.
 *
 * The od_draw_ellarc2 function is similar to the od_draw_ellarc function
 * except that the arc parameters are given as function parameters.
**/
extern void od_draw_ellarc (Toutdev* od, Tod_GCdef gcd, CellArc*e);
extern void od_draw_ellarc2(Toutdev* od, Tod_GCdef gcd,
    double xc, double yc, double x1, double y1, double x2,double y2,
    double ra, double rb, int pop);

extern void od_fill_ellarc (Toutdev* od, Tod_GCdef gcd, CellArc*e);
extern void od_fill_ellarc2(Toutdev* od, Tod_GCdef gcd,
    double xc, double yc, double x1, double y1, double x2,double y2,
    double ra, double rb, int pop);

extern void od_draw_ellipse (Toutdev* od, Tod_GCdef gcd, Cellipse*e);
extern void od_draw_ellipse2(Toutdev* od, Tod_GCdef gcd, double angle,
                             double xc, double yc, double rx, double ry);
extern void od_fill_ellipse (Toutdev* od, Tod_GCdef gcd, Cellipse*e);
extern void od_fill_ellipse2(Toutdev* od, Tod_GCdef gcd, double angle,
                             double xc, double yc, double rx, double ry);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_LDAF_HL_H
/*======================================================================*/
