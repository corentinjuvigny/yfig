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
 *      $Id: ldaf-hl.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/ldaf-hl.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the ldaf-hl.h header for documentation.                        =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/ldaf-hl.h"

#include "tools/lenunit.h"

/*======================================================================*/

// Return: expected distance between 2 points in src unit
Inline double od_draw_getOpix(Coutdev* od)
{
    double _opix; // size of 1 output pixel in source unit.
    if ( od->ppiDes>LUNIT_PtPerInch ) {
        _opix = CONV_D_D2S(od,1);
        return _opix*8;
    } else {
        double ptInDesUnit = lu_convFromFuPpi( LUNIT_PtToFu(1), od->ppiDes);
        _opix = CONV_D_D2S(od,ptInDesUnit);
//printf("0: src/desppi=%.2f/%.2f ptInDesUnit=%.4f _opix=%.4f\n", OD_ppiSrc(od),od->ppiDes,ptInDesUnit,_opix);
        return _opix*8;
    }
}

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

extern void od_draw_pointPT(Coutdev* od, Tod_GCdef gcd, Cdble2* pt)
{ od_draw_linePT(od,gcd,pt,pt); }

extern void od_draw_linePT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pt1, Cdble2* pt2)
{ od->vtable->ldafVT.draw_linePT(od,gcd,pt1,pt2); }

extern void od_draw_mlineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts)
{ od_draw_mlineT(od,gcd,pts->eles, pts->eleNb); }

extern void od_draw_lineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts)
{
    Coutdev_ldaf_vt* vtable = &od->vtable->ldafVT;
    if ( vtable->draw_lineT!=0 )
        vtable->draw_lineT( od, gcd, pts->eles, pts->eleNb );
    else {
        int i;
        for (i=0 ; i<(pts->eleNb-1) ; i+=2 )
            od_draw_linePT(od,gcd,pts->eles+i,pts->eles+i+1);
    }
}

extern void od_draw_lineT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pts, int npts)
{
    Coutdev_ldaf_vt* vtable = &od->vtable->ldafVT;
    if ( vtable->draw_lineT!=0 )
        vtable->draw_lineT( od, gcd, pts, npts );
    else {
        int i;
        for (i=0 ; i<(npts-1) ; i+=2 )
            od_draw_linePT(od,gcd,pts+i,pts+i+1);
    }
}

extern void od_draw_mlineT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pts, int npts)
{
    if ( npts == 0 ) return;

    /* if the multi line has only one point --> point */
    if ( npts == 1 ) {
        Cdble2* p1 = pts+0;
        od_draw_pointXY(od,gcd,p1->x,p1->y);
        return;
    }
    /* if the multi line has only 2 points --> line */
    if (npts == 2) {
        Cdble2* p1 = pts+0;
        Cdble2* p2 = pts+1;
        od_draw_linePT(od,gcd,p1,p2);
        return;
    }

    od->vtable->ldafVT.draw_mlineT(od, gcd, pts, npts);
}

extern void od_fill_mlineS(Coutdev* od, Tod_GCdef gcd, Cdble2Set* pts)
{ od_fill_mlineT(od,gcd,pts->eles, pts->eleNb); }

extern void od_fill_mlineT(Coutdev* od, Tod_GCdef gcd,
                           Cdble2* pts, int npts)
{
    if (npts == 0) return;

    /* if the multi line has only one point --> point */
    if (npts == 1) {
        Cdble2* p1 = pts+0;
        od_draw_pointXY(od,gcd,p1->x,p1->y);
        return;
    }
    /* if the multi line has only 2 points --> line */
    if (npts == 2) {
        Cdble2* p1 = pts+0;
        Cdble2* p2 = pts+1;
        od_draw_linePT(od,gcd,p1,p2);
        return;
    }
    /* if the multi line has at lest 3 points --> fill */
    od->vtable->ldafVT.fill_mlineT(od, gcd, pts, npts);
}

/*======================================================================*/
/*= rectangle                                                          =*/

extern void od_draw_rectXYWH(Coutdev*od, Tod_GCdef gcd,
                             double x, double y,
                             double width, double height)
{
    TEMP_asf_printf(od->vtable->ldafVT.draw_rectangleXYWH==0,"%s","no EGS driver");
    od->vtable->ldafVT.draw_rectangleXYWH(od,gcd,x,y,width,height);
}

extern void od_fill_rectXYWH(Coutdev*od, Tod_GCdef gcd,
                               double x, double y,
                               double width, double height)
{
    TEMP_asf_printf(od->vtable->ldafVT.fill_rectangleXYWH==0,"%s","no EGS driver");
    od->vtable->ldafVT.fill_rectangleXYWH(od,gcd,x,y,width,height);
}

/*======================================================================*/
/*= open and closed spline curves                                      =*/

// return 1 if special case and drawing is done
//        0 no special case and no drawing
static int  od_spline_specialcase(Coutdev* od, Tod_GCdef gcd, Cdble2Set* ctls)
{
    if ( ctls->eleNb==0 )
        return 1;
    else if ( ctls->eleNb==1 ) {
        od_draw_pointPT(od,gcd,&ctls->eles[0]);
        return 1;
    } else if ( ctls->eleNb==2 ) {
        od_draw_linePT(od,gcd,&ctls->eles[0],&ctls->eles[1]);
        return 1;
    }
    return 0;
}

static Tdble3Set* od_splineO_getDble3set(Cdble2Set* ctls, CdbleSet* nodes)
{
    int i=0;
    Tdble3Set* spline = dble3set_new( ctls->eleNb + 10);
    dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    for (i+=1 ; i<ctls->eleNb ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    // add a dupplication of last point
    int last = spline->eleNb-1;
    dble3set_addPT (spline, &spline->eles[last]);
    return spline;
}

static Tdble3Set* od_splineC_getDble3set(Cdble2Set* ctls, CdbleSet* nodes)
{
    int i;
    Tdble3Set* spline = dble3set_new( ctls->eleNb + 10);
    for (i=0 ; i<ctls->eleNb ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    // add a dupplication of first points
    for (i=0 ; i<3 ; i+=1)
        dble3set_addPTZ(spline, &ctls->eles[i], nodes->eles[i]);
    return spline;
}

extern void od_draw_splineO(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes)
{
    if ( od_spline_specialcase(od,gcd,ctls)!=0 ) return;

    // at least 3 point --> a true spline
    Tdble3Set* spline = od_splineO_getDble3set(ctls,nodes);

    double      opix  = od_draw_getOpix(od);
    Tdble2Set*  mline = tool_splineToMLine( spline, opix);
    od_draw_mlineS(od,gcd,mline);

    dble2set_delete( mline );
    dble3set_delete( spline );
}

extern void od_draw_splineC(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes)
{
    if ( od_spline_specialcase(od,gcd,ctls)!=0 ) return;

    // at least 3 point --> a true spline
    Tdble3Set* spline = od_splineC_getDble3set(ctls,nodes);

    double      opix  = od_draw_getOpix(od);
    Tdble2Set*  mline = tool_splineToMLine( spline, opix);
    od_draw_mlineS(od,gcd,mline);

    dble2set_delete( mline );
    dble3set_delete( spline );
}

extern void od_fill_splineO(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes)
{
    if ( od_spline_specialcase(od,gcd,ctls)!=0 ) return;

    // at least 3 point --> a true spline
    Tdble3Set* spline = od_splineO_getDble3set(ctls,nodes);

    double      opix  = od_draw_getOpix(od);
    Tdble2Set*  mline = tool_splineToMLine( spline, opix);
    od_fill_mlineS(od,gcd,mline);

    dble2set_delete( mline );
    dble3set_delete( spline );
}

extern void od_fill_splineC(Coutdev* od, Tod_GCdef gcd,
                            Cdble2Set* ctls, CdbleSet* nodes)
{
    if ( od_spline_specialcase(od,gcd,ctls)!=0 ) return;

    // at least 3 point --> a true spline
    Tdble3Set* spline = od_splineC_getDble3set(ctls,nodes);

    double      opix  = od_draw_getOpix(od);
    Tdble2Set*  mline = tool_splineToMLine( spline, opix);
    od_fill_mlineS(od,gcd,mline);

    dble2set_delete( mline );
    dble3set_delete( spline );
}

/*======================================================================*/
/*= ellipses and ellipsoid arcs                                        =*/

extern void od_draw_ellarc (Toutdev* od, Tod_GCdef gcd, CellArc*e)
{
    double opix = od_draw_getOpix(od);

    // we invert p1 and p2 because source basis is indirect and
    // od_ellarcToMLine uses a direct basis.
    TellArc edirect = *e; edirect.p1=e->p2; edirect.p2=e->p1;
    Tdble2Set* mline = tool_ellarcToMLine(&edirect,opix);
    // we add to mline the lines to make the Piece of Pie.
    if ( edirect.pop ) {
        dble2set_addPT(mline,edirect.center);
        dble2set_addPT(mline,edirect.p1);
    }
    od_draw_mlineS(od,gcd,mline);

    dble2set_delete ( mline );
}

extern void od_draw_ellarc2(Toutdev* od, Tod_GCdef gcd,
    double xc, double yc, double x1, double y1, double x2,double y2,
    double ra, double rb, int pop)
{
    TellArc e;
    e.center.x = xc;
    e.center.y = yc;
    e.p1.x     = x1;
    e.p1.y     = y1;
    e.p2.x     = x2;
    e.p2.y     = y2;
    e.ra       = ra;
    e.rb       = rb;
    e.pop      = pop;
    od_draw_ellarc(od,gcd,&e);
}

extern void od_fill_ellarc (Toutdev* od, Tod_GCdef gcd, CellArc*e)
{
    double opix = od_draw_getOpix(od);

    // we invert p1 and p2 because source basis is indirect and
    // od_ellarcToMLine uses a direct basis.
    TellArc edirect = *e; edirect.p1=e->p2; edirect.p2=e->p1;
    Tdble2Set* mline = tool_ellarcToMLine(&edirect,opix);
    // we add to mline the lines to make the Piece of Pie.
    if ( edirect.pop ) {
        dble2set_addPT(mline,edirect.center);
        dble2set_addPT(mline,edirect.p1);
    }
    od_fill_mlineS(od,gcd,mline);

    dble2set_delete ( mline );
}

extern void od_fill_ellarc2(Toutdev* od, Tod_GCdef gcd,
    double xc, double yc, double x1, double y1, double x2,double y2,
    double ra, double rb, int pop)
{
    TellArc e;
    e.center.x = xc;
    e.center.y = yc;
    e.p1.x     = x1;
    e.p1.y     = y1;
    e.p2.x     = x2;
    e.p2.y     = y2;
    e.ra       = ra;
    e.rb       = rb;
    e.pop      = pop;
    od_fill_ellarc(od,gcd,&e);
}

extern void od_draw_ellipse (Toutdev* od, Tod_GCdef gcd, Cellipse*e)
{
    Coutdev_ldaf_vt* vt = &od->vtable->ldafVT;

    if ( e->angle==0 ) {
        if ( vt->draw_ellipseHV!=0 ) {
            vt->draw_ellipseHV(od,gcd,e->center.x,e->center.y,e->rx,e->ry);
            return;
        }
    }

    if ( vt->draw_ellipseWR!=0 ) {
        vt->draw_ellipseWR(od,gcd,e->center.x,e->center.y,
                e->rx,e->ry,e->angle);
        return;
    }

    double   opix = od_draw_getOpix(od);
    Tellipse edirect = *e; edirect.angle=-e->angle;
    Tdble2Set* mline = tool_ellipseToMLine(&edirect,opix);
    od_draw_mlineS(od,gcd,mline);

    dble2set_delete ( mline );
}

extern void od_draw_ellipse2(Toutdev* od, Tod_GCdef gcd, double angle,
                             double xc, double yc, double rx, double ry)
{
    Tellipse e;
    e.center.x = xc;
    e.center.y = yc;
    e.rx       = rx;
    e.ry       = ry;
    e.angle    = angle;
    od_draw_ellipse(od,gcd,&e);
}

extern void od_fill_ellipse (Toutdev* od, Tod_GCdef gcd, Cellipse*e)
{
    Coutdev_ldaf_vt* vt = &od->vtable->ldafVT;

    if ( e->angle==0 ) {
        if ( vt->fill_ellipseHV!=0 ) {
            vt->fill_ellipseHV(od,gcd,e->center.x,e->center.y,e->rx,e->ry);
            return;
        }
    }

    if ( vt->fill_ellipseWR!=0 ) {
        vt->fill_ellipseWR(od,gcd,e->center.x,e->center.y,
                e->rx,e->ry,e->angle);
        return;
    }

    double   opix = od_draw_getOpix(od);
    Tellipse edirect = *e; edirect.angle=-e->angle;
    Tdble2Set* mline = tool_ellipseToMLine(&edirect,opix);
    od_fill_mlineS(od,gcd,mline);

    dble2set_delete ( mline );
}

extern void od_fill_ellipse2(Toutdev* od, Tod_GCdef gcd, double angle,
                             double xc, double yc, double rx, double ry)
{
    Tellipse e;
    e.center.x = xc;
    e.center.y = yc;
    e.rx       = rx;
    e.ry       = ry;
    e.angle    = angle;
    od_fill_ellipse(od,gcd,&e);
}

/*======================================================================*/
