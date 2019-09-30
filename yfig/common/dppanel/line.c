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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: line.c 114 2018-01-10 18:01:03Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/line.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the drawing parameter:   =*/
/*=  line thickness, line color                                        =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

/*======================================================================*/

static Tdpp_l1l2v3 linewidth   = {
    "Line"       , "Width"                   , "99",
    .initval=1   , .minval=FIG_THICKNESS_MIN , .maxval=FIG_THICKNESS_MAX
};

/*======================================================================*/

extern Twid_button* dpp_linewidth_new (
        Twid_buttonPanel* panel, int* syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newInt(panel,&linewidth, syn);
    return button;
}

extern void dpp_linewidth_update(Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2v3_update(self,ymw); }

extern Twid_button* dpp_linecolor_new  (
        Twid_buttonPanel* panel, Tcolor** syn)
{
    Twid_button* button;
    button = dpp_l1l2color_newCLR(panel,"Pen","Color", syn);
    return button;
}

extern void dpp_linecolor_update(Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2color_update(self,ymw); }

/*======================================================================*/
