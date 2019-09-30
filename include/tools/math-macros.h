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
 *      $Id: math-macros.h 361 2018-11-18 22:53:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/math-macros.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= some mathematic oriented macros                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_MATH_MACROS_H
#define FILE_TOOLS_MATH_MACROS_H
/*======================================================================*/

/*======================================================================*/
/*= some constants                                                     =*/

#define TOOL_PI_2 1.57079632679489661923
#define TOOL_PI   3.14159265358979323846
#define TOOL_2PI  6.28318530717958647692

#define        RadToDeg(a)           ( (a)/TOOL_PI*180. )
Inline  double RadToDeg180(double r) { double d=RadToDeg(r); while (d>180) d-=360; while (d<=-180) d+=360; return d; }
Inline  double RadToDeg360(double r) { double d=RadToDeg(r); while (d>360) d-=360; while (d<    0) d+=360; return d; }
#define        DegToRad(a)           ( (a)*TOOL_PI/180. )

/*======================================================================*/
/*= some basic ones                                                    =*/

#define dble_close0(x)  abs(x)<1e-10
#define dble_close(x,y) dble_close0((x)-(y))

#define dble_close0AtP(x,p)  abs(x)<(p)
#define dble_closeAtP(x,y,p) dble_close0AtP((x)-(y),p)

#define dble2_close0(pt)    (dble_close0(pt.x)     && dble_close0(pt.y))
#define dble2_close(p1,p2)  (dble_close(p1.x,p2.x) && dble_close(p1.y,p2.y))

#define VECTOR_Norme2XY(x,y) ( (x)*(x) + (y)*(y) )
#define VECTOR_NormeXY(x,y)  sqrt(VECTOR_Norme2(x,y))
#define VECTOR_Norme2PT(p)   VECTOR_Norme2XY((p).x,(p).y)
#define VECTOR_NormePT(p)    VECTOR_NormeXY((p).x,(p).y)
#define VECTOR_Norme2(x,y) ( (x)*(x) + (y)*(y) )
#define VECTOR_Norme(x,y)  sqrt(VECTOR_Norme2(x,y))

/*======================================================================*/
/*= some geometric ones                                                =*/

#define TRANSLATE_VAL(v,t)     do { (v)+=t; } while (0)
#define TRANSLATE_PtX(p,tx,ty) do { (p).x+=tx; } while (0)
#define TRANSLATE_PtY(p,tx,ty) do { (p).y+=ty; } while (0)
#define TRANSLATE_PXY(p,tx,ty) do { (p).x+=tx; (p).y+=ty; } while (0)
#define TRANSLATE_Box(b,tx,ty) do { Box_x(b)+=tx; Box_y(b)+=ty; } while (0)

// scaling from center (0,0)
#define SCALE00_Dim(d,f)     do { (d)*=f; } while (0)
#define SCALE00_VeX(p,fx,fy) do { (p).x*=fx; } while (0)
#define SCALE00_VeY(p,fx,fy) do { (p).y*=fy; } while (0)
#define SCALE00_VXY(p,fx,fy) do { (p).x*=fx; (p).y*=fy; } while (0)
#define SCALE00_Box(b,fx,fy) do { Tdble2 __pt0 = tool_boxdble_pt1(&(b)); \
                                  Tdble2 __pt1 = tool_boxdble_pt3(&(b)); \
                                  SCALE00_VXY(__pt0,fx,fy); \
                                  SCALE00_VXY(__pt1,fx,fy); \
                                  tool_boxDble_initPT(&(b),__pt0,__pt1); } while (0)

// scaling from center (cx,cy)
#define SCALECC_Coo(v,f,c)         do { (v) = (c) + ((v)-(c))*(f); } while (0)
#define SCALECC_PtX(p,fx,fy,cx,cy) do { SCALECC_Coo((p).x,fx,cx); } while (0)
#define SCALECC_PtY(p,fx,fy,cx,cy) do { SCALECC_Coo((p).y,fy,cy); } while (0)
#define SCALECC_PXY(p,fx,fy,cx,cy) do { SCALECC_PtX(p,fx,fy,cx,cy); \
                                        SCALECC_PtY(p,fx,fy,cx,cy); } while (0)
#define SCALECC_Box(b,fx,fy,cx,cy) do { Tdble2 __pt0 = tool_boxdble_pt1(&(b)); \
                                        Tdble2 __pt1 = tool_boxdble_pt3(&(b)); \
                                        SCALECC_PXY(__pt0,fx,fy,cx,cy); \
                                        SCALECC_PXY(__pt1,fx,fy,cx,cy); \
                                        tool_boxDble_initPT(&(b),__pt0,__pt1); } while (0)

// I image of M by the rotation of a angle from center (cx,cy)
//  Be OM=(x,y)      a point in orthonormal basis (O, i, j) 
//  Be CM=(x',y')    the M point in the orthonormal basis (C, i, j)
//     OM = OC + CM --> CM = OM - OC --> x'=x-cx & y'=y-cy
//  BE CI=(ix',iy') the rotation of M point (a angle)
//         ix'= x'  cos(a) + y' -sin(a)
//         iy'= x'  sin(a) + y'  cos(a)
// So OI = OC + CI -->
//   ix = cx + { x' cos(a) + y' -sin(a) }
//   iy = cy + { x' sin(a) + y'  cos(a) }
#define ROTCC_PAXY(p,a,cx,cy)  do { \
   double __xx = (cx) + ((p).x-(cx)) * cos(a) - ((p).y-(cy)) * sin(a); \
   double __yy = (cy) + ((p).x-(cx)) * sin(a) + ((p).y-(cy)) * cos(a); \
   (p).x=__xx; (p).y=__yy; \
   } while (0)
#define ROTCC_PAP(p,a,c)  ROTCC_PAXY(p,a,(c).x,(c).y)

// I image of M by the symmetry of axis y=cy (horizontal)
//   Be OM=(x,y) a point in orthonormal basis (O, i, j) 
//   CM=CO+OM=(-cx+x,-cy+y)    M in the basis (C, i, j)
//   CI=(-cx+x,+cy-y)  the I point in the orthonormal basis (C, i, j)
//   OI=OC+CI=(x,2cy-y)
// I image of M by the symmetry of axis x=cx (vertical)
//   OI=OC+CI=(2cx-x,y)
#define FLIPH_PXY(p,cx,cy) do { (p).x=2*(cx)-(p).x; } while (0)
#define FLIPV_PXY(p,cx,cy) do { (p).y=2*(cy)-(p).y; } while (0)
#define FLIP_PHVRXY(p,h,v,r,cx,cy) do { \
    if ( (h)!=0 ) FLIPH_PXY(p,cx,cy); \
    if ( (v)!=0 ) FLIPV_PXY(p,cx,cy); \
    if ( r!=0   ) ROTCC_PAXY(p,DegToRad(r*90),cx,cy); \
    } while (0)

/*======================================================================*/
#endif // FILE_TOOLS_MATH_MACROS_H
/*======================================================================*/
