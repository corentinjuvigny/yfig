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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: geo2d.c 144 2018-05-31 15:54:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/geo2d.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= 2 dimension geometric utilities                                    =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/geo2d.h"

/*======================================================================*/

extern int tool_closeToPointPTDbl(double md, Cdble2* pt1,  Cdble2* pt2)
{ return abs(pt1->x-pt2->x)<=md && abs(pt1->y-pt2->y)<=md ; }

/*======================================================================*/
/*= orthogonal projection                                              =*/

extern Tdble2 tool_projPtToLinePt (Tdble2 pt, Tdble2 li1, Tdble2 li2)
{
    Tdble2 ret;
  // special cases
    if (li1.x == li2.x) {
        if (li1.y == li2.y)
            return li1;      // no line (see doc)
        else
            return dble2_init(li1.x,pt.y); // horizontal line
    } else if (li1.y == li2.y) {
            return dble2_init(pt.x,li1.y); // verical line
    }
  // standard case
  // y =  a1 x + b1 la droite passant par li1 et li2
    double a1 = (li1.y-li2.y)/(li1.x-li2.x);
    double b1 = li1.y - a1 * li1.x ;
  // y = a2 x + b2  la droite perpendiculaire passant par pt
    double a2 = -1/a1;  // perpendiculaire
    //     --> pt.y = a2 pt.x + b2 ( passant par pt)
    double b2 = pt.y - a2 * pt.x ;
    // l'intersection des 2 droites est le point recherché.
    //   a2 yc     = a2 a1 xc + a2 b1        (1ere droite * a2)
    //   a1 yc     = a2 a1 xc + a1 b2        (2nd  droite * a1)
    //  (a2-a1) yc = a2 b1 - a1 b2           (1ere - 2nd)
    ret.y = (a2 * b1 - a1 * b2) / (a2 - a1);
    // en remplacant dans la premiere equations
    ret.x = (ret.y-b1)/a1;
    return ret;
}

extern double tool_projPtToLineRa (Tdble2 pt, Tdble2 li1, Tdble2 li2)
{
    if (li1.x == li2.x && li1.y == li2.y)
        return 0;
    Tdble2 p = tool_projPtToLinePt(pt,li1,li2);
    Tdble2 v12 = dble2_initVab(li1,li2);
    Tdble2 v1p = dble2_initVab(li1,p);
    // v1p = alpha v12  and v12 can not be (0,0)
    return v12.x != 0 ? v1p.x/v12.x : v1p.y/v12.y ;
}

extern int    tool_closePtToLinePt(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, Tdble2* rpt)
{
    Tdble2 p,v1p;
    if (li1.x == li2.x && li1.y == li2.y) {
        p   = li1;
        v1p = dble2_initVab(li1,pt);
    } else {
        p   = tool_projPtToLinePt(pt,li1,li2);
        v1p = dble2_initVab(p,pt);
    }
    if ( VECTOR_NormePT(v1p)<=md ) {
        if ( rpt!=0 ) *rpt=p;
        return 1;
    } else
        return 0;
}

extern int    tool_closePtToSegmPt(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, Tdble2* rpt)
{
    double ratio;
    Tdble2 p,v12,vppt;
    if (li1.x == li2.x && li1.y == li2.y) {
        ratio = 0;
    } else {
        ratio  = tool_projPtToLineRa(pt,li1,li2);
    }
    if ( ratio<0 ) return 0;
    if ( ratio>1 ) return 0;
    v12 = dble2_initVab(li1,li2);
    p.x = li1.x + ratio * v12.x;
    p.y = li1.y + ratio * v12.y;
    vppt = dble2_initVab(p,pt);
    if ( VECTOR_NormePT(vppt)<=md ) {
        if ( rpt!=0 ) *rpt=p;
        return 1;
    } else
        return 0;
}

extern int    tool_closePtToLineRa(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, double* rra);

/*======================================================================*/

extern double tool_angleOfVector_XYD(double dx, double dy)
{
    Tdble2 pt;
    pt.x = dx;
    pt.y = dy;
    return tool_angleOfVector_PTD(&pt);
}

extern double tool_angleOfVector_PTD(Cdble2* v)
{
    double  alpha;
    double  dx= v->x;
    double  dy= v->y;

    if (dx == 0) {
        if (dy > 0)
            alpha = M_PI_2;
        else
            alpha = 3 * M_PI_2;
    } else if (dy == 0) {
        if (dx > 0)
            alpha = 0;
        else
            alpha = M_PI;
    } else {
        alpha = atan(dy / dx);  /* range = -PI/2 to PI/2 */
        if (dx < 0)
            alpha += M_PI;
        else if (dy < 0)
            alpha += TOOL_2PI;
    }
    return alpha;
}

extern double tool_angleOfVectors_XYD(double x1,double y1, double x2,double y2)
{
    double alpha1 = tool_angleOfVector_XYD(x1,y1);
    double alpha2 = tool_angleOfVector_XYD(x2,y2);
    double alpha  = alpha2-alpha1;
    if ( alpha<0 ) alpha += TOOL_2PI;
    return alpha;
}

extern double tool_angleOfVectors_PTD(Cdble2* v1,Cdble2* v2)
{
    double alpha1 = tool_angleOfVector_PTD(v1);
    double alpha2 = tool_angleOfVector_PTD(v2);
    double alpha  = alpha2-alpha1;
    if ( alpha<0 ) alpha += TOOL_2PI;
    return alpha;
}
/*======================================================================*/

extern int tool_are3PointsClockwise_XYD(double x1, double y1,
                  double x2, double y2, double x3, double y3)
{
    Tdble2 pt1; pt1.x = x1; pt1.y = y1;
    Tdble2 pt2; pt2.x = x2; pt2.y = y2;
    Tdble2 pt3; pt3.x = x3; pt3.y = y3;
    return tool_are3PointsClockwise_PTD(&pt1,&pt2,&pt3);
}
extern int tool_are3PointsClockwise_PTD(
        Cdble2* pt1, Cdble2* pt2, Cdble2* pt3)
{
    double  diff, dx, dy, alpha, theta;

    dx = pt2->x - pt1->x;
    dy = pt1->y - pt2->y;       /* because origin of the screen is on the
                 * upper left corner */

    alpha = tool_angleOfVector_XYD(dx, dy);

    dx = pt3->x - pt2->x;
    dy = pt2->y - pt3->y;
    theta = tool_angleOfVector_XYD(dx, dy);

    diff = theta - alpha;
    if ((0 < diff && diff < TOOL_PI) || diff < -TOOL_PI) {
        return 1;     /* counterclockwise */
    } else
        return 0;     /* clockwise */
}

/*======================================================================*/

extern int tool_arcCenterOf3Points_XYD(double*x,double*y,
                  double x1, double y1, double x2, double y2, double x3, double y3)
{
    int ret;
    Tdble2 pt;
    Tdble2 pt1; pt1.x = x1; pt1.y = y1;
    Tdble2 pt2; pt2.x = x2; pt2.y = y2;
    Tdble2 pt3; pt3.x = x3; pt3.y = y3;
    ret = tool_arcCenterOf3Points_PTD(&pt,&pt1,&pt2,&pt3);
    *x = pt.x;
    *y = pt.y;
    return ret;
}

extern int tool_arcCenterOf3Points_PTD(Tdble2*center,
              Cdble2* pt1, Cdble2* pt2, Cdble2* pt3)
{
    double      a, b, c, d, e, f, g, h, i, j;
    double      resx, resy;

    if ((pt1->x == pt3->x && pt1->y == pt3->y) ||
    (pt1->x == pt2->x && pt1->y == pt2->y) ||
    (pt2->x == pt3->x && pt2->y == pt3->y)) {
        return 0;
    }
    a = (double)pt1->x - (double)pt2->x;
    b = (double)pt1->x + (double)pt2->x;
    c = (double)pt1->y - (double)pt2->y;
    d = (double)pt1->y + (double)pt2->y;
    e = (a*b + c*d)/2.0;

    f = (double)pt2->x - (double)pt3->x;
    g = (double)pt2->x + (double)pt3->x;
    h = (double)pt2->y - (double)pt3->y;
    i = (double)pt2->y + (double)pt3->y;
    j = (f*g + h*i)/2.0;

    if (a*h - c*f != 0.0)
        resy = (a*j - e*f)/(a*h - c*f);
    else {
        return 0;
    }
    if (a != 0.0)
        resx = (e - resy*c)/a;
    else if (f != 0.0)
        resx = (j - resy*h)/f;
    else {
        return 0;
    }

    center->x = resx;
    center->y = resy;

    return 1;
}

