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
 *      $Id: text.c 114 2018-01-10 18:01:03Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widgets that show and allow user to modify the drawing parameters. =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

/*======================================================================*/

//T_l1l2v3 textflags  = { "Text Flags" ,  0      ,   "9"   };
//T_l1l2v3 textstep   = { "Text"       , "Step"  ,   "9.9" };
static Tdpp_l1l2v3 textsize   = {
    "Text"       , "Size"       , "99.9",
    .initval=12  , .minval=5    , .maxval=50 };
static Tdpp_l1l2v3 textangle  = {
    "Text"       , "Angle"      , "999.9",
    .initval=0   , .minval=-180 , .maxval=+180 };
static Tdpp_l1l2v3 textLSF  = {
    "Line"       , "Spacing"    , "99.99",
    .initval=0.5 , .minval=-0   , .maxval=10,
    .incr=.25 };

/*======================================================================*/

extern Twid_button* dpp_textcolor_new  (
        Twid_buttonPanel* panel, Tcolor**syn)
{
    Twid_button* button;
    button = dpp_l1l2color_newCLR(panel,"Text","Color", syn);
    return button;
}
extern void dpp_textcolor_update(Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2color_update(self,ymw); }

extern Twid_button* dpp_textsize_new (
        Twid_buttonPanel* panel, double* syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newDble(panel,&textsize, syn);
    return button;
}
extern void dpp_textsize_update (Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2v3_update(self,ymw); }

extern Twid_button* dpp_textangle_new(
        Twid_buttonPanel* panel,double*syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newDble(panel,&textangle, syn);
    return button;
}
extern void dpp_textangle_update(Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2v3_update(self,ymw); }

extern Twid_button* dpp_textLSF_new (
        Twid_buttonPanel* panel, double* syn)
{
    Twid_button* button;
    button = dpp_l1l2v3_newDble(panel,&textLSF, syn);
    return button;
}
extern void dpp_textLSF_update  (Twid_button* self, Tyfig_mw* ymw)
{ dpp_l1l2v3_update(self,ymw); }

/*======================================================================*/
