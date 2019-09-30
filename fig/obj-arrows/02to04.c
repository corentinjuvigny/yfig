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
 *      $Id: 02to04.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/obj-arrows/02to04.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Arrows of type 02 03 04 (base on a quadrilateral).                 =*/
/*======================================================================*/

/*======================================================================*/

static void arrow02to04_getDelta(CobjAll obj, double*rdeltaL, double*rdeltaA)
{ arrow00_getDelta(obj,rdeltaL,rdeltaA); }

// store pt1--pte--pt2--pts--pt1 line into pts1 point set
static void arrow02to04_genDrawData(CobjAll obj, Tarrowhead* ah)
{
    MVARS_Self(obj);
    dble2set_newORclr(&self->OAW_pts1,5);
    dble2set_add(self->OAW_pts1,ah->ah_pt1);
    dble2set_add(self->OAW_pts1,ah->ah_pte);
    dble2set_add(self->OAW_pts1,ah->ah_pt2);
    dble2set_add(self->OAW_pts1,ah->ah_pts);
    dble2set_add(self->OAW_pts1,ah->ah_pt1);
}

/*======================================================================*/
/*= Arrow for lines, splines and arcs                                  =*/

static void arrow02to04_doBegEndLine(CobjAll obj, Tdble2Set* pts, int atBeg, double coef)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow01_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH5Line(&ah,atBeg,obj,pts,deltaL,deltaA,coef)<0 ) return;

    // build draw data
    arrow02to04_genDrawData(obj,&ah);
}

static void arrow02to04_doBegEndSpline(CobjAll obj, Tdble2Set* ctls,
                                       TdbleSet* ndes, int atBeg, double coef)
{
    arrow02to04_doBegEndLine(obj,ctls,atBeg,coef);
    TEMP_asf_printf(ndes->eleNb < ctls->eleNb,
        "arrows added control points (nb ctl/node=%d/%d)", ctls->eleNb, ndes->eleNb);
    ndes->eleNb = ctls->eleNb;
}

static void   arrow02to04_doBegEndArc(CobjAll obj, Tdble2 pts[4],
                                      int dir, int atBeg, double coef)
{
    MVARS_Self(obj);

    // get the arrow position shifts 
    double deltaL, deltaA;
    arrow02to04_getDelta(obj,&deltaL,&deltaA);

    // get arrow data
    Tarrowhead ah;
    if ( arrow_doAH5Arc(&ah,atBeg,obj,pts,dir,deltaL,deltaA,coef)<0 ) return;

    // build draw data
    arrow02to04_genDrawData(obj,&ah);
}

/*======================================================================*/
/*= The tables                                                         =*/

#define COEF_02 0.60

static void   arrow02_doBegLine(CobjAll obj, Tdble2Set* pts)
{ arrow02to04_doBegEndLine(obj,pts,1,COEF_02); }

static void   arrow02_doEndLine(CobjAll obj, Tdble2Set* pts)
{ arrow02to04_doBegEndLine(obj,pts,0,COEF_02); }

static void   arrow02_doBegSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{ arrow02to04_doBegEndSpline(obj,ctls,ndes,1,COEF_02); }

static void   arrow02_doEndSpline(CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes)
{ arrow02to04_doBegEndSpline(obj,ctls,ndes,0,COEF_02); }

static void   arrow02_doBegArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow02to04_doBegEndArc(obj,pts,dir,1,COEF_02); }

static void   arrow02_doEndArc(CobjAll obj, Tdble2 pts[4], int dir)
{ arrow02to04_doBegEndArc(obj,pts,dir,0,COEF_02); }

#define ARROW_ID 02
#define ARROW_draw_sc arrow01_draw_sc
#include "arrow-vtable-template.h"

/*======================================================================*/
