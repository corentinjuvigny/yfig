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
 *      $Id: 00.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arrows/00.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Arrows of type 00.                                                 =*/
/*======================================================================*/

/*======================================================================*/

static void arrow00_getDelta(CobjAll obj, double*rdeltaL, double*rdeltaA)
{
    MVARS_Self(obj);
    TobjAll pobj = self->O_partof;
    Tfigure* fig = obj_Figure( pobj );

    if (fig==0) {
        *rdeltaL=0;
        *rdeltaA=0;
        return ;
    }

    double thickness = self->OAW_thickness;
    if ( thickness<=1 )
        *rdeltaA=0;
    else 
        *rdeltaA = (thickness-1)/2/tan(self->OAW_baseangle);
    if ( obj_StyLine(pobj)->stl_cap==DA_CS_Butt )
        *rdeltaL = *rdeltaA;
    else
        *rdeltaL = *rdeltaA + thickness/2.;
    // convert pt in figure unit
    double factor = fig->fig_ppi/LUNIT_PtPerInch;
    *rdeltaA *=  factor;
    *rdeltaL *=  factor;
}

// store pt1--pte--pt2 line into pts1 point set
static void arrow00_genDrawData(CobjAll obj, Tarrowhead* ah)
{
    MVARS_Self(obj);
    dble2set_newORclr(&self->OAW_pts1,3);
    dble2set_add(self->OAW_pts1,ah->ah_pt1);
    dble2set_add(self->OAW_pts1,ah->ah_pte);
    dble2set_add(self->OAW_pts1,ah->ah_pt2);
}

/*======================================================================*/

static void arrow00_draw_sc(CobjAll   obj, Tfig_scene* sc,
                            Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);

    if ( gcd!=OD_GC_None ) {
        if ( self->OAW_pts1 && self->OAW_pts1->eleNb )
            od_draw_mlineS(sc->sc_device,gcd,self->OAW_pts1);
    }
}

/*======================================================================*/
/*= Arrow for lines                                                    =*/

static void   arrow00_doBegLine(CobjAll obj, Tdble2Set* pts)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow00_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH4Line(&ah,1,obj,pts,deltaL,deltaA)<0 ) return;

    // build draw data
    arrow00_genDrawData(obj,&ah);
}

static void   arrow00_doEndLine(CobjAll obj, Tdble2Set* pts)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow00_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH4Line(&ah,0,obj,pts,deltaL,deltaA)<0 ) return;

    // build draw data
    arrow00_genDrawData(obj,&ah);
}

/*======================================================================*/
/*= Arrow for splines                                                  =*/

static void   arrow00_doBegSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{
    arrow00_doBegLine(obj,ctls);
    TEMP_asf_printf(ndes->eleNb < ctls->eleNb,
        "arrows added control points (nb ctl/node=%d/%d)",
        ctls->eleNb, ndes->eleNb);
    ndes->eleNb = ctls->eleNb;
}

static void   arrow00_doEndSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{
    arrow00_doEndLine(obj,ctls);
    TEMP_asf_printf(ndes->eleNb < ctls->eleNb,
        "arrows added control points (nb ctl/node=%d/%d)",
        ctls->eleNb, ndes->eleNb);
    ndes->eleNb = ctls->eleNb;
}

/*======================================================================*/
/*= Arrow for arcs                                                     =*/

static void   arrow00_doBegEndArc(CobjAll obj, Tdble2 pts[4], int dir, int atBeg)
{
    MVARS_Self(obj);
    TobjAll pobj = self->O_partof;

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow00_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH4Arc(&ah,atBeg,obj,pts,dir,deltaL,deltaA)<0 ) return;

    // build draw data
    arrow00_genDrawData(obj,&ah);
}

static void   arrow00_doBegArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow00_doBegEndArc(obj,pts,dir,1); }

static void   arrow00_doEndArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow00_doBegEndArc(obj,pts,dir,0); }

/*======================================================================*/
/*= The table                                                          =*/

#define ARROW_ID 00
#include "arrow-vtable-template.h"

/*======================================================================*/
