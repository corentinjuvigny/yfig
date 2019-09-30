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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: geo2d-splineToMLine.c 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/geo2d-splineToMLine.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Interpolation of a spline curve into a multi-line.                 =*/
/*=                                                                    =*/
/*= See the geo2d.h header for documentation.                          =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/geo2d.h"

#if 0
#include "tools/lenunit.h"
#define Debug
#endif

/*======================================================================*/

#define COPY_CONTROL_POINT(P0, S0, P1, S1) \
      P0 = P1; \
      S0 = S1

#define NEXT_CONTROL_POINTS(P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, P1, S1); \
      COPY_CONTROL_POINT(P1, S1, P2, S2); \
      COPY_CONTROL_POINT(P2, S2, P3, S3); \
      COPY_CONTROL_POINT(P3, S3, P3->next, S3->next)

#define INIT_CONTROL_POINTS(SPLINE, P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, SPLINE->OSP_points, SPLINE->OSP_sfactors); \
      COPY_CONTROL_POINT(P1, S1, P0->next, S0->next);               \
      COPY_CONTROL_POINT(P2, S2, P1->next, S1->next);               \
      COPY_CONTROL_POINT(P3, S3, P2->next, S2->next)

#define SPLINE_SEGMENT_LOOP(K, P0, P1, P2, P3, S1, S2, PREC) \
      step = step_computing(K, PREC, P0, P1, P2, P3, S1, S2);    \
      spline_segment_computing(result,step, K, P0, P1, P2, P3, S1, S2)

#if 1
static double step_computing( int k, double precision,
        Cdble3 *p0, Cdble3 *p1, Cdble3 *p2, Cdble3 *p3,
        double s1, double s2);

static void spline_segment_computing(Tdble2Set* result,
        double step, int k,
        Cdble3 *p0, Cdble3 *p1, Cdble3 *p2, Cdble3 *p3,
        double s1, double s2);
#else
static double step_computing( int k, double precision,
        F_point *p0, F_point *p1, F_point *p2, F_point *p3,
        double s1, double s2);

static void spline_segment_computing(Tdble2Set* result,
        double step, int k,
        F_point *p0, F_point *p1, F_point *p2, F_point *p3,
        double s1, double s2);
#endif

/*======================================================================*/

extern Tdble2Set* tool_splineToMLine(Cdble3Set* ctls, double opix)
{
  int       k;
  double    step;
  double    precision = opix;

  Tdble2Set* result = dble2set_new(20000);

  for (k = 0 ; k<ctls->eleNb-3 ; k++) {
      Tdble3* org = ctls->eles + k;
      SPLINE_SEGMENT_LOOP(k, &org[0], &org[1], &org[2], &org[3],
                             org[1].z, org[2].z, precision);
  }

  //dble2set_addXY(result,p3->x, p3->y);
  
  return result;
}

#if 0
static Tdble2Set* compute_open_spline(CobjSpline *spline, double precision)
{
  int       k;
  float     step;
  F_point   *p0, *p1, *p2, *p3;
  F_sfactor *s0, *s1, *s2, *s3;

  Tdble2Set* result = dble2set_new(20000);

  int DONE = 0;

  COPY_CONTROL_POINT(p0, s0, spline->OSP_points, spline->OSP_sfactors);
  COPY_CONTROL_POINT(p1, s1, p0, s0);
  /* first control point is needed twice for the first segment */
  COPY_CONTROL_POINT(p2, s2, p1->next, s1->next);

  if (p2->next == NULL) {
      COPY_CONTROL_POINT(p3, s3, p2, s2);
  } else {
      COPY_CONTROL_POINT(p3, s3, p2->next, s2->next);
  }


  for (k = 0 ;  ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
      if (DONE)
        break;
      if (p3->next == NULL)
	    break;
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
  }
  /* last control point is needed twice for the last segment */
  if (!DONE) {
    COPY_CONTROL_POINT(p0, s0, p1, s1);
    COPY_CONTROL_POINT(p1, s1, p2, s2);
    COPY_CONTROL_POINT(p2, s2, p3, s3);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
  }
  
  dble2set_addXY(result,p3->x, p3->y);
  
  return result;
}

static Tdble2Set* compute_closed_spline(CobjSpline *spline, double precision)
{
  int k, i;
  double    step;
  F_point   *p0, *p1, *p2, *p3, *first;
  F_sfactor *s0, *s1, *s2, *s3, *s_first;

  Tdble2Set* result = dble2set_new(20000);

  int DONE = 0;

  INIT_CONTROL_POINTS(spline, p0, s0, p1, s1, p2, s2, p3, s3);
  COPY_CONTROL_POINT(first, s_first, p0, s0); 

  for (k = 0 ; p3 != NULL ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
      if (DONE)
        break;
  }
  /* when we are at the end, join to the beginning */
  if (!DONE) {
    COPY_CONTROL_POINT(p3, s3, first, s_first);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
    for (i=0; i<2; i++) {
        if (DONE)
          break;
        k++;
        NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
        SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
    }
  }

  // add close point
  dble2set_addXY(result,result->eles[0].x,result->eles[0].y);

  return result;
}

extern Tdble2Set* obj_spline_toLines(CobjSpline*self,
                                         double precision)
{
    if (open_spline(self))
        return compute_open_spline  (self, precision);
    else
        return compute_closed_spline(self, precision);
}

#endif

/*======================================================================*/

#define Q(s)  (-0.5 * (s))

#define EQN_NUMERATOR(dim) \
  (A_blend[0]*p0->dim+A_blend[1]*p1->dim+A_blend[2]*p2->dim+A_blend[3]*p3->dim)

static inline void point_computing(double *A_blend,
        Cdble3 *p0, Cdble3 *p1, Cdble3 *p2, Cdble3 *p3,
        double *x, double *y)
{
  double weights_sum;

  weights_sum = A_blend[0] + A_blend[1] + A_blend[2] + A_blend[3];

  *x = round(EQN_NUMERATOR(x) / (weights_sum));
  *y = round(EQN_NUMERATOR(y) / (weights_sum));
}

static inline double f_blend(double numerator, double denominator)
{
  double p = 2 * denominator * denominator;
  double u = numerator / denominator;
  double u2 = u * u;
  return (u * u2 * (10 - p + (2*p - 15)*u + (6 - p)*u2));
}

static inline double g_blend(double u, double q)  /* p equals 2 */
{
  // qu + 2quu +8uuu - 12quuu + 14quuuu - 11uuuu + 4uuuuu - 5quuuuu
  return(u*(q + u*(2*q + u*(8 - 12*q + u*(14*q - 11 + u*(4 - 5*q))))));
}

static inline double h_blend(double u, double q)
{
  double u2=u*u;
  return (u * (q + u * (2 * q + u2 * (-2*q - u*q))));
}

static inline
void negative_s1_influence(double t, double s1, double *A0, double *A2)
{
  *A0 = h_blend(-t, Q(s1));
  *A2 = g_blend(t, Q(s1));
}

static inline
void negative_s2_influence(double t, double s2, double *A1, double *A3)
{
  *A1 = g_blend(1-t, Q(s2));
  *A3 = h_blend(t-1, Q(s2));
}

static inline
void positive_s1_influence(int k, double t, double s1, double *A0, double *A2)
{
  double Tk;
  
  Tk = k+1+s1;
  *A0 = (t+k+1<Tk) ? f_blend(t+k+1-Tk, k-Tk) : 0.0;
  
  Tk = k+1-s1;
  *A2 = f_blend(t+k+1-Tk, k+2-Tk);
}

static inline
void positive_s2_influence(int k, double t, double s2, double *A1, double *A3)
{
  double Tk;

  Tk = k+2+s2; 
  *A1 = f_blend(t+k+1-Tk, k+1-Tk);
  
  Tk = k+2-s2;
  *A3 = (t+k+1>Tk) ? f_blend(t+k+1-Tk, k+3-Tk) : 0.0;
}

/* This function computes the step used to draw the segment (p1, p2)
   (xv1, yv1) : coordinates of the vector from middle to origin
   (xv2, yv2) : coordinates of the vector from middle to extremity */

//static double step_computing( int k, double precision,
//        F_point *p0, F_point *p1, F_point *p2, F_point *p3, double s1, double s2)
static double step_computing( int k, double precision,
        Cdble3 *p0, Cdble3 *p1, Cdble3 *p2, Cdble3 *p3,
        double s1, double s2)
{
  double A_blend[4];
  double    xstart, ystart, xend, yend, xmid, ymid, xlength, ylength;
  double    start_to_end_dist, number_of_steps;
  double  step, angle_cos, scal_prod, xv1, xv2, yv1, yv2, sides_length_prod;

  if ((s1 == 0) && (s2 == 0))
    return(1.0);          /* only one step in case of linear segment */

  /* compute coordinates of the origin */
  if (s1>0) {
      if (s2<0) {
	    positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	    negative_s2_influence(0.0, s2, &A_blend[1], &A_blend[3]); 
      } else {
	    positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	    positive_s2_influence(k, 0.0, s2, &A_blend[1], &A_blend[3]); 
      }
      point_computing(A_blend, p0, p1, p2, p3, &xstart, &ystart);
  } else {
      xstart = p1->x;
      ystart = p1->y;
  }
  
  /* compute coordinates  of the extremity */
  if (s2>0) {
      if (s1<0) {
	    negative_s1_influence(1.0, s1, &A_blend[0], &A_blend[2]);
	    positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]);
      } else {
	    positive_s1_influence(k, 1.0, s1, &A_blend[0], &A_blend[2]);
	    positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]); 
      }
      point_computing(A_blend, p0, p1, p2, p3, &xend, &yend);
  } else {
      xend = p2->x;
      yend = p2->y;
  }

  /* compute coordinates  of the middle */
  if (s2>0) {
      if (s1<0) {
	    negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
	    positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]);
      } else {
	    positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
	    positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]); 
	}
  } else if (s1<0) {
      negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  } else {
      positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  }
  point_computing(A_blend, p0, p1, p2, p3, &xmid, &ymid);

#if 1
  double xc,yc,length,alpha;
  if ( tool_arcCenterOf3Points_XYD(&xc,&yc,
              xstart,ystart,xmid,ymid,xend,yend)==0 )
      length = VECTOR_Norme(xend-xstart,yend-ystart);
  else {
      alpha  = tool_angleOfVectors_XYD(xstart-xc,ystart-yc, xend-xc,yend-yc);
      // if ( tool_are3PointsClockwise_XYD(xstart,ystart,xmid,ymid,xend,yend)==0 )
      //   this is true but tool_are3PointsClockwise_XYD invert the y
      //   coordinates assuming (0,0) is NW.  FIXME
      if ( tool_are3PointsClockwise_XYD(xstart,ystart,xmid,ymid,xend,yend)==1 )
          alpha = TOOL_2PI - alpha;
      length = alpha * VECTOR_Norme(xstart-xc,ystart-yc);
  }
  double nb_steps = length/precision;
#ifdef Debug
  printf("d=%f/%f m=%f/%f e=%f/%f ; c=%f/%f ; alpha=%f len=%f\n",
        LUNIT_FuToCm(xstart),LUNIT_FuToCm(ystart),
        LUNIT_FuToCm(xmid),LUNIT_FuToCm(ymid),
        LUNIT_FuToCm(xend),LUNIT_FuToCm(yend),
        LUNIT_FuToCm(xc),LUNIT_FuToCm(yc),
        alpha/TOOL_2PI*360.,
        LUNIT_FuToCm(length)
  );
#endif
  return 1./nb_steps;
#else
  xv1 = xstart - xmid;
  yv1 = ystart - ymid;
  xv2 = xend - xmid;
  yv2 = yend - ymid;

  scal_prod = xv1*xv2 + yv1*yv2;
  
  sides_length_prod = sqrt((xv1*xv1 + yv1*yv1)*(xv2*xv2 + yv2*yv2));

  /* compute cosinus of origin-middle-extremity angle, which approximates the
     curve of the spline segment */
  if (sides_length_prod == 0.0)
    angle_cos = 0.0;
  else
    angle_cos = scal_prod/sides_length_prod; 

  xlength = xend - xstart;
  ylength = yend - ystart;

  start_to_end_dist = sqrt((double)xlength*(double)xlength +
                           (double)ylength*(double)ylength);

  /* more steps if segment's origin and extremity are remote */
  number_of_steps = sqrt(start_to_end_dist)/2;

  /* more steps if the curve is high */
  number_of_steps += (int)((1 + angle_cos)*10);

  if (number_of_steps == 0)
    step = 1;
  else
    step = precision/number_of_steps;
  
  if ((step > MAX_SPLINE_STEP) || (step == 0))
    step = MAX_SPLINE_STEP;
  return (step);
#endif
}

#if 0
static void spline_segment_computing(Tdble2Set* result,
        double step, int k,
        F_point *p0, F_point *p1, F_point *p2, F_point *p3,
        double s1, double s2)
#endif
static void spline_segment_computing(Tdble2Set* result,
        double step, int k,
        Cdble3 *p0, Cdble3 *p1, Cdble3 *p2, Cdble3 *p3,
        double s1, double s2)
{
  double A_blend[4];
  double t;

#define point_adding(A_blend, p0, p1, p2, p3) do { \
        double weights_sum = A_blend[0] + A_blend[1] \
                           + A_blend[2] + A_blend[3]; \
        double x = round(EQN_NUMERATOR(x) / (weights_sum)); \
        double y = round(EQN_NUMERATOR(y) / (weights_sum)); \
        dble2set_addXY(result,x,y); \
    } while (0)
                          
  if (s1<0) {  
     if (s2<0) {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     } else {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     }
  } else if (s2<0) {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      }
  } else {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      } 
  }
}

/*======================================================================*/
