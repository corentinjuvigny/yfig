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
 *      $Id: fig-scene-tex.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-scene-tex.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= PDF driver.                                                        =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"
#include "fig/fig-scene.h"

#include "outdev/resources.h"
#include "outdev/outdevPdf.h"

/*======================================================================*/
/* Private utilities.                                                  =*/

/*======================================================================*/
/* Interface.                                                          =*/

extern int fig_scene_tex(Tfig_scene* sc, Tmsgdrv* msgdrv,
                         TodPdfCfg*cfg, CboxDble srcBoxCm)
{
    // create the PDF output device
    Tfigure* fig   = sc->sc_dessin;
    TodTex*  odTex = odtex_new(msgdrv,cfg, Box_dx(srcBoxCm), Box_dy(srcBoxCm));
    Toutdev* od    = (Toutdev*) odTex;

    if (od==0) return 0;
{
    Tcolor*  black = od_blackColor(od);
    od->resources  = odres_newWithBtm( black );
    color_delete(black);
}
    odres_chgFontUsedEated( od->resources, font_defaultDefSet() );
#if 0
    int i;
    for (i=0 ; i<od->resources->fontUsed->eleNb ; i++) {
        odpdf_addFont(odTex, odres_getFontUsed(od->resources,i));
    } 
#endif

    // attach the sc scene to PDF output device
    fig_scene_attachDev(sc,od,1);
    Box_x (od->boxSrc) = LUNIT_CmToIn(Box_x(srcBoxCm))*OD_ppiSrc(od);
    Box_y (od->boxSrc) = LUNIT_CmToIn(Box_y(srcBoxCm))*OD_ppiSrc(od);

    // draw figure
    fig_scene_clrEGScache( sc );
    if ( odtex_doHeader(odTex) < 0 )  return -3;
    //if ( svgwrite_colors(odTex,fig)<0 ) return -3;
    if ( fig_scene_draw(sc)<0 ) return -3;
    if ( odtex_doFooter(odTex)<0 ) return -3;
    fig_scene_clrEGScache( sc );

    // free resources before returning
    od_detach( od );
    odtex_delete( odTex );

    return 1;
}

/*======================================================================*/
