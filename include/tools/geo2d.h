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
 *      $Id: geo2d.h 389 2019-01-30 13:42:14Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/geo2d.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= 2 dimension geometric utilities                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_GEO2D_H
#define FILE_TOOLS_GEO2D_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"
#include "math-macros.h"
#include "boxes.h"

/*======================================================================*/

Inline  Tdble2  dble2_initRot(Cdble2*pt, double a, Cdble2*c) {
    Tdble2 r = { c->x + (pt->x-c->x) * cos(a) - (pt->y-c->y) * sin(a),
                 c->y + (pt->x-c->x) * sin(a) + (pt->y-c->y) * cos(a) };
   return r; }
/*======================================================================*/

/**
 * Set x1,y1 to the North-West corner, x2,y2 to the Sud-East corner
 * for the rectangle a diagonal of which is segment from (x1,y1) to (x2,y2).
**/
extern void tool_rectToNWSE_XYI (int* x1,    int* y1,    int* x2,    int* y2);
extern void tool_rectToNWSE_XYD (double* x1, double* y1, double* x2, double* y2);
extern void tool_rectToNWSE_PTD (Tdble2* pt1, Tdble2* pt2);
extern void tool_rectToNWSE_BXD (TboxDble* b);

/**
 * Return
 *   1: if the rectangles overlap (intersection of the rectangles is not empty).
 *   0: if the rectangles do'nt overlap
**/
extern int  tool_rectOverlap_XYI(int xmin1, int ymin1, int xmax1, int ymax1,
                                int xmin2, int ymin2, int xmax2, int ymax2);
extern int  tool_rectOverlap_XYD(double xmin1, double ymin1, double xmax1, double ymax1,
                                double xmin2, double ymin2, double xmax2, double ymax2);
#define     tool_rectOverlap_BXD(b1,b2) tool_boxDble_overlap(b1,b2)

// Return:
//   0: the distance between pt1 and pt2 points is greater than md.
//   1: the distance between pt1 and pt2 points is equal or less than md.
extern int tool_closeToPointPTDbl(double md, Cdble2* pt1,  Cdble2* pt2);

/**
 * Be P the orthogonal projection of the pt point on the line joining
 * the li1 and li2 points.
 *
 * Be alpha the factor between the vectors (li1-->li2) and (li1-->P):
 *    (li1-->P) = alpha (li1-->li2)
 *
 * Return:
 *  - The tool_projPtToLinePt function returns P if the li1 and li2
 *    points are different otherwise li1.
 *  - The tool_projPtToLineRa function returns alpha if the li1 and li2
 *    points are different otherwise 0.
 *  - The tool_closePtToLinePt function returns:
 *    If li1 and li2 are different:
 *      0: if the distance from pt to P is greater than md and leaves
 *         *rpt unchanged.
 *      1: if the distance from pt to P is less or equal to md and
 *         sets *rpt to P.
 *    If li1 and li2 are equal:
 *      0: if the distance from pt to li1 is greater than md and leaves
 *         *rpt unchanged.
 *      1: if the distance from pt to li1 is less or equal to md and
 *         sets *rpt to li1.
 *  - The tool_closePtToLineRa function is similar to tool_closePtToLinePt
 *    except that it updates (or not) *rra with alpha instead of *rpt.
 *  - The tool_closePtToSegm/Pt/Ra functions are similar to 
 *    tool_closePtToLine/Pt/Ra but they returns 1 only if P is on
 *    segment [li1:li2].
 *
 * Parameters:
 *  - pt,li1,li2 : point to project and line points.
 *  - md         : minimal distance.
 *  - rpt        : location for returning P or the NULL pointer.
 *  - rra        : location for returning alpha or the NULL pointer.
**/
/*======================================================================*/
extern Tdble2 tool_projPtToLinePt (Tdble2 pt, Tdble2 li1, Tdble2 li2);
extern double tool_projPtToLineRa (Tdble2 pt, Tdble2 li1, Tdble2 li2);
extern int    tool_closePtToLinePt(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, Tdble2* rpt);
extern int    tool_closePtToLineRa(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, double* rra);
extern int    tool_closePtToSegmPt(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, Tdble2* rpt);
extern int    tool_closePtToSegmRa(Tdble2 pt, Tdble2 li1, Tdble2 li2, double md, double* rra);

/**
 * Input arguments :
 *  (x,y)   : the v  vector (0,0) to (x,y)
 *  (x1,y1) : the v1 vector (0,0) to (x1,y1)
 *  (x2,y2) : the v2 vector (0,0) to (x1,y1)
 * Output arguments : none
 * Return value :
 *  The unit is radian and in the range [0, 2PI[.
 *  The angle is exprimed assuming that the (i,j) angle is PI/2.
 *
 *  In a direct orthonormal basis, the angle is counter clockwise from i to v or from v1 to v2.
 *  y   v : angle(v)=PI/4       y   v1 : angle(v1,v2)=3*PI/2 (v2-v1)
 *  ^  /                        ^  /
 *  | /                         | /
 *  |/                          |/
 *  X------------> x            X-------------> x
 *   \                           \
 *    \                           \
 *     \                           \
 *      v : angle(v)=2PI-PI/4       v2 : angle(v2,v1)=PI/2  (v1-v2=-3PI/2=PI/2)
 *
 *  In an indirect orthonormal basis, the angle is clockwise from i to v or from v1 to v2.
 *      v : angle(v)=2PI-PI/4       v1 : angle(v1,v2)=PI/2 (v2-v1=PI/4-(2PI-PI/4))
 *     /                           /
 *    /                           /
 *   /                           /
 *  X------------> x            X-------------> x
 *  |\                          |\
 *  | \                         | \
 *  V  \                        V  \
 *  y   v : angle(v)=PI/4       y   v2 : angle(v2,v1)=3*PI/2 (v1-v2=2PI-PI/4-PI/4)
**/
extern double tool_angleOfVector_XYD(double dx, double dy);
extern double tool_angleOfVector_PTD(Cdble2* v);
extern double tool_angleOfVectors_XYD(double x1,double y1, double x2,double y2);
extern double tool_angleOfVectors_PTD(Cdble2* v1, Cdble2* v2);

/**
 * Input arguments :
 *   p1, p2, p3 : p1 starts the arc, p3 ends the arc and p2 is
 *                somewhere on the arc between p1 and p3.
 * Return value :
 *  In an indirect orthonormal basis:
 *    0 : the arc (p1, p2, p3) is in clockwise direction
 *    1 : the arc (p1, p2, p3) is counterclockwise
**/
extern int tool_are3PointsClockwise_XYD(double x1, double y1,
                  double x2, double y2, double x3, double y3);
extern int tool_are3PointsClockwise_PTD( Cdble2*pt1,
                  Cdble2*pt2, Cdble2*pt3);

/**
 * New routine 12/11/00 from Thomas Henlich - better at finding center
 * 
 * Input arguments :
 *   p1, p2, p3 : p1 starts the arc, p3 ends the arc and p2 is
 *                somewhere on the arc between p1 and p3.
 * Output arguments :
 *  (*x,*y) : Center of the arc
 * Return value :
 *  0 : if p1, p2, p3 are co-linear.
 *  1 : if they are not.
**/
extern int tool_arcCenterOf3Points_XYD(double*x,double*y,
                  double x1, double y1, double x2, double y2, double x3, double y3);
extern int tool_arcCenterOf3Points_PTD(Tdble2*c,
                  Cdble2*pt1, Cdble2*pt2, Cdble2*pt3);

/*======================================================================*/
/*= Spline approximation                                               =*/

/**
 * The tool_splineToMLine function approximates the spline defined by
 * the ctls parameter by a multi-line and returns its points.
 *
 * The (ctls[i].x,ctls[i].y) is the i th control point of the spline
 * and its associated nodes is ctls[i].z.
 * The ctls set must have at least 4 elements.
 *
 * The function places the i th point of the multi-line at a opix
 * distance of the i-1 th point.
**/
extern Tdble2Set* tool_splineToMLine(Cdble3Set* ctls, double opix);

/*======================================================================*/
/*= Ellipse and ellipse arc  approximation                                               =*/

/*==  =  =  =  =  =  =  =  =  =  =  ==  =  =  =  =  =  =  =  =  =  =  ==*/

/**
 * The TellArc type defines an ellipse arc.
 *   y
 *   ^    .B
 *   |   .    ..p2
 *   |  .  ..    *
 *   | ..          *
 *   C ...-------------> x
 *    \   ...M.     *
 *     \       .... M'
 *      \          *
 *       \      *
 *       p1*  
 *
 * In a direct orthogonal basis, the relationship betwween the figure
 * and the structure fields ares:
 *  - C is the ellipse center
 *  - M is the middle of p1 and p2.
 *  - CMM' is the a radius
 *  - CB is orthogonal to CMM'line and is the b radius.
 *  - the * shows the arc (from Cp1 to Cp2, it is counterclockwise).
 *    If you invert p1 and p2, the arc will be the other part of the
 *    ellipse.
 *  - if pop is one the two segment Cp1 and Cp2 are also drawn, making a
 *    Piece Of Pie.
 *
 * In an indirect orthogonal basis, it is quite similar except that the
 * arc is drawn from p1 to p2 in the  clockwise. So to get the same arc
 * as the direct orthogonal basis, you must invert p1 and p2.
 *        .B
 *       .    ..p1
 *      .  ..    *
 *     ..          *
 *   C ...-------------> x
 *   |\   ...M.     *
 *   | \       .... M'
 *   |  \          *
 *   V   \      *
 *   y   p2*  
 *
**/

typedef struct _TellArc TellArc;
typedef const   TellArc CellArc;

struct _TellArc {
    Tdble2 center,p1,p2; // the arc center and the arc extremities
    double ra,rb;        // radius: a from center to p1,p2 middle
    int    pop;          // Piece of Pie
};

typedef struct _Tellipse Tellipse;
typedef const   Tellipse Cellipse;

struct _Tellipse {
    Tdble2 center; // the ellipse center
    double rx,ry;  // radius: rx horizontally, ry vertically
    double angle;  // rotate ellipse with angle radians,
                   // the rotation center beeing center.
};


/*==  =  =  =  =  =  =  =  =  =  =  ==  =  =  =  =  =  =  =  =  =  =  ==*/

/**
 * The tool_ellarcToMLine function generates the points of the ellipse
 * arc defined by the e parameter and returns them into a point set.
 * It assumes that the e dimensions are given in a direct orthogonal
 * basis and that the direction from e->p1 to e->p2 is counterclockwise.
 * The generated points follow the arc direction (first point is p1,
 * last point is p2).
 * The function places the i th point at a opix arc distance of the
 * i-1 th point.
**/
extern Tdble2Set* tool_ellarcToMLine(CellArc* e, double opix);

/**
 * The tool_ellipseToMLine function generates the points of the ellipse
 * defined by the e parameter and returns them into a point set.
 * The function places the i th point at a opix arc distance of the
 * i-1 th point.
 * It assumes that the e dimensions are given in a direct orthogonal
 * basis. Thus if the dimensions are in an indirect basis, you
 * must set the rotation angle to -e->angle.
**/
extern Tdble2Set* tool_ellipseToMLine(Cellipse* e, double opix);

/**
 * The tool_ellipseToBezierLine function generates the points of the
 * ellipse defined by the e parameter and returns them into a point set.
 * The function used the De Casteljau's algorithm in order to create
 * a bezier curve.
 * It assumes that the e dimensions are given in a direct orthogonal
 * basis. Thus if the dimensions are in an indirect basis, you
 * must set the rotation angle to -e->angle.
**/
extern Tdble2Set* tool_ellipseToBezierLine(Cellipse* e, double opix);
/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_GEO2D_H
/*======================================================================*/
