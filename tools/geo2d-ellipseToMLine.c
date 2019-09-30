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
 *      $Id: geo2d-ellipseToMLine.c 389 2019-01-30 13:42:14Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/geo2d-ellipseToMLine.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Interpolation of a full ellipse or an ellipse arc into             =*/
/*=   - a muti-line.                                                   =*/
/*=                                                                    =*/
/*= See the geo2d.h header for documentation.                          =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/geo2d.h"

#if 0
#define Debug
#include "tools/lenunit.h"
#endif

/*======================================================================*/

extern Tdble2Set* tool_ellarcToMLine(CellArc* e, double opix)
{
    Tdble2Set* set = dble2set_new(1000);

    // get the counterclockwise angle in range [0:2PII[
    double angleP1 = tool_angleOfVector_PTD(&e->p1); // i,CP1
    double angleP2 = tool_angleOfVector_PTD(&e->p2); // i,CP2
    double angleCM = 0; // i,CM (middle of formers) crossing the arc
    angleP1 = tool_angleOfVector_XYD(e->p1.x-e->center.x,e->p1.y-e->center.y);
    angleP2 = tool_angleOfVector_XYD(e->p2.x-e->center.x,e->p2.y-e->center.y);
#ifdef Debug
printf("AAAA: a1=%f am=%f a2=%f c=%f/%f p1=%f/%f p2=%f/%f cm\n",
    angleP1/TOOL_2PI*360., angleCM/TOOL_2PI*360., angleP2/TOOL_2PI*360.,
    LUNIT_FuToCm(e->center.x), LUNIT_FuToCm(e->center.y),
    LUNIT_FuToCm(e->p1.x), LUNIT_FuToCm(e->p1.y),
    LUNIT_FuToCm(e->p2.x), LUNIT_FuToCm(e->p2.y));
#endif
    if ( angleP1<angleP2 )
          angleCM = (angleP1+angleP2)/2;
    else {
          angleP2+= TOOL_2PI; // now angleP1<angleP2
          angleCM = (angleP1+angleP2)/2;
    }
    // The rotation by -angleCM and center C of the ellipse containing 
    // the asked arc is:
    //   x = center.x + ra * cos(alpha)
    //   y = center.y + rb * sin(alpha)
    // On this ellipse, the image of the arc is given for the alphai:
    //     angleP1-angleCM <= alpha         <= angleP2-angleCM 
    // We have to rotate by +angleCM the arc image to get the asked arc.

    // get the number of step to make opix approximately the distance
    // between two points.
    double length = ( angleP2-angleP1) * (e->ra+e->rb)/2;
    int     nb_step = round( length/opix );
#ifdef Debug
printf("AAA : a1=%f am=%f a2=%f c=%f/%f p1=%f/%f p2=%f/%f cm\n",
    angleP1/TOOL_2PI*360, angleCM/TOOL_2PI*360, angleP2/TOOL_2PI*360,
        LUNIT_FuToCm(e->center.x), LUNIT_FuToCm(e->center.y),
        LUNIT_FuToCm(e->p1.x), LUNIT_FuToCm(e->p1.y),
        LUNIT_FuToCm(e->p2.x), LUNIT_FuToCm(e->p2.y));
#endif

    // arc starts at p1 --> we add p1 to the result point set.
    dble2set_addPT(set,e->p1);
    // add intermediate points to the result point set.
    int i;
    for (i=1 ; i<nb_step ; i++) {
        Tdble2 p;
        double alpha = angleP1 + i *  (angleP2-angleP1)/nb_step ;
#ifdef Debug
printf("AA  : a[%2d]=%f/%f length=%f ra/rb=%f/%f\n",i,alpha/TOOL_2PI*360,
        (alpha+angleCM)/TOOL_2PI*360,
LUNIT_FuToCm(length), LUNIT_FuToCm(e->ra), LUNIT_FuToCm(e->rb));
#endif
        p.x = e->center.x + e->ra * cos(alpha-angleCM);
        p.y = e->center.y + e->rb * sin(alpha-angleCM);
        ROTCC_PAXY(p,angleCM,e->center.x,e->center.y);
        dble2set_addXY(set,p.x,p.y);
    }
    // arc ends at p2 --> we add p2 to the result point set.
    dble2set_addPT(set,e->p2);

    return set;
}

/*======================================================================*/

extern Tdble2Set* tool_ellipseToMLine(Cellipse* e, double opix)
{
    double alpha;
    int    i;
    Tdble2Set* set = dble2set_new(1000);

    // get the number of step to make opix approximately the distance
    // between two points.
    double length = TOOL_2PI*sqrt( e->rx*e->rx + e->ry*e->ry );
    int    nb_step = round( length/opix );
#ifdef Debug
printf("EEEE: a=%f c=%f/%f rx=%f ry=%f cm nbstep=%d\n",
    e->angle/TOOL_2PI*360, 
        LUNIT_FuToCm(e->center.x), LUNIT_FuToCm(e->center.y),
        LUNIT_FuToCm(e->rx), LUNIT_FuToCm(e->ry),nb_step);
#endif

    // get the points of horizontal ellipse.
    for (alpha=0 ; alpha<TOOL_2PI ; alpha+= TOOL_2PI/((double)nb_step) ) {
        Tdble2 p;
        p.x = e->center.x + e->rx * cos(alpha);
        p.y = e->center.y + e->ry * sin(alpha);
        dble2set_addPT(set,p);
    }
    Tdble2 p; // original point must be add for countering approximation errors
    p.x = e->center.x + e->rx;
    p.y = e->center.y;

    dble2set_addPT(set,p);

    // rotate the points if needed
    if ( e->angle!=0 ) {
        for (i=0 ; i<set->eleNb ; i+=1) {
            ROTCC_PAXY(set->eles[i],e->angle,e->center.x,e->center.y);
        }
    }

    return set;
}

/*======================================================================*/

extern Tdble2Set* tool_ellipseToBezierLine(Cellipse* e, double opix)
{
    int    i;
    double alpha;
    Tdble2Set* set = dble2set_new(1000);

    return set;
}
/*======================================================================*/
