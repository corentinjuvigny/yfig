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
 *      $Id: 01.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arrows/01.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Arrows of type 01.                                                 =*/
/*======================================================================*/

/*======================================================================*/

static void arrow01_getDelta(CobjAll obj, double*rdeltaL, double*rdeltaA)
{ arrow00_getDelta(obj,rdeltaL,rdeltaA); }

// store pt1--pte--pt2--pt1 line into pts1 point set
static void arrow01_genDrawData(CobjAll obj, Tarrowhead* ah)
{
    MVARS_Self(obj);
    dble2set_newORclr(&self->OAW_pts1,4);
    dble2set_add(self->OAW_pts1,ah->ah_pt1);
    dble2set_add(self->OAW_pts1,ah->ah_pte);
    dble2set_add(self->OAW_pts1,ah->ah_pt2);
    dble2set_add(self->OAW_pts1,ah->ah_pt1);
}

/*======================================================================*/

static void arrow01_draw_sc(CobjAll   obj, Tfig_scene* sc,
                          Tod_GCdef gcd, Tod_GCdef gcf)
{
    MVARS_Self(obj);
    if ( self->OAW_pts1 && self->OAW_pts1->eleNb ) {
        if ( gcf!=OD_GC_None )
            od_fill_mlineS(sc->sc_device,gcf,self->OAW_pts1);
        if ( gcd!=OD_GC_None ) 
            od_draw_mlineS(sc->sc_device,gcd,self->OAW_pts1);
    }
}

/*======================================================================*/
/*= Arrow for lines                                                    =*/

static void arrow01_doBegEndLine(CobjAll obj, Tdble2Set* pts, int atBeg)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow01_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH4Line(&ah,atBeg,obj,pts,deltaL,deltaA)<0 ) return;

    // build draw data
    arrow01_genDrawData(obj,&ah);
}

static void   arrow01_doBegLine(CobjAll obj, Tdble2Set* pts)
{ arrow01_doBegEndLine(obj,pts,1); }

static void   arrow01_doEndLine(CobjAll obj, Tdble2Set* pts)
{ arrow01_doBegEndLine(obj,pts,0); }

/*======================================================================*/
/*= Arrow for splines                                                  =*/

static void   arrow01_doBegSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{
    arrow01_doBegLine(obj,ctls);
    TEMP_asf_printf(ndes->eleNb < ctls->eleNb,
        "arrows added control points (nb ctl/node=%d/%d)",
        ctls->eleNb, ndes->eleNb);
    ndes->eleNb = ctls->eleNb;
}

static void   arrow01_doEndSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{
    arrow01_doEndLine(obj,ctls);
    TEMP_asf_printf(ndes->eleNb < ctls->eleNb,
        "arrows added control points (nb ctl/node=%d/%d)",
        ctls->eleNb, ndes->eleNb);
    ndes->eleNb = ctls->eleNb;
}

/*======================================================================*/
/*= Arrow for arcs                                                     =*/

static void   arrow01_doBegEndArc(CobjAll obj, Tdble2 pts[4], int dir, int atBeg)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow01_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH4Arc(&ah,atBeg,obj,pts,dir,deltaL,deltaA)<0 ) return;

    // build draw data
    arrow01_genDrawData(obj,&ah);
}

static void   arrow01_doBegArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow01_doBegEndArc(obj,pts,dir,1); }

static void   arrow01_doEndArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow01_doBegEndArc(obj,pts,dir,0); }

/*======================================================================*/
/*= The table                                                          =*/

#define ARROW_ID 01
#include "arrow-vtable-template.h"

/*======================================================================*/
