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
 *$Software: YFIG
 *      $Id: yfig-dsds.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-dsds.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data shared by all Tyfig_mw main widgets                           =*/
/*=                                                                    =*/
/*= See the "yfig-dsds.h" header for documentation.                    =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-dsds.h"
#include "yfig-pixmap.h"

#include "vgui/vgui.h"
#include "fig/figure.h"

#include "tools/globalvars.h"
#include "tools/file.h"

/*======================================================================*/

extern Tyfig_dsds* yfig_dsds_new (Tvgui* gui)
{
    Tyfig_dsds* dsds = tool_malloc_0( sizeof(*dsds) );

    dsds->gui = gui;
    dsds->ds_baseColors = fig_colors_newSharedColor(gui->colorDriver);
    odsty_initFillS(&dsds->ds_fillWhite , fig_colors_getWhite(dsds->ds_baseColors));
    dsds->ds_begArrow = *pxm_arrow_ref(); dsds->ds_begArrow.abslength = 0;
    dsds->ds_endArrow = *pxm_arrow_ref(); dsds->ds_endArrow.abslength = 0;

    // ds_defaults
                     dsds->ds_paperSize = tool_paperdef_get("A4");
    lu_init        (&dsds->ds_userUnit,LUNIT_ID_Cm,1.0,10.);
    odsty_initText (&dsds->ds_styText , fig_colors_getBlack(dsds->ds_baseColors));
    odsty_initLine (&dsds->ds_styLine , fig_colors_getRed  (dsds->ds_baseColors));
    odsty_initFill (&dsds->ds_styFill , fig_colors_getBlue4(dsds->ds_baseColors));

    // ds_exportPrint
    dsds->ds_expFmt = tool_imgfmt_getByKd( IMGFMT_JPG );
    dsds->ds_expOpt = yfig_expopt_initDefault();
#if 1
    dsds->ds_prtOpt = yfig_expopt_initDefault();
    dsds->ds_prtOpt.paperKind = ExpOptPaKd_SetByUser;
    tool_paperdef_chgClone(&dsds->ds_prtOpt.paperDef,dsds->ds_paperSize);
    dsds->ds_prtOpt.paperBdCm = 1.5;
    dsds->ds_prtOpt.placeKind = ExpOptPlace_ZoomToFit;
#else
    dsds->ds_prtOpt = yfig_expopt_initDefault();
    dsds->ds_prtOpt.paperKind = ExpOptPaKd_SetByUser;
    tool_paperdef_chgCustomCm(&dsds->ds_prtOpt.paperDef,7.2,10.0);
    dsds->ds_prtOpt.paperBdCm = 1.5;
    dsds->ds_prtOpt.placeKind = ExpOptPlace_ZoomUserAPM;
#endif
    Tstr rf_fn = tool_aprintf("%s/recent-files",gl_appRcDir);
    dsds->ds_recentFiles = tool_rf_newByFile(8,30,NULL,rf_fn);
    tool_free(rf_fn);

    return dsds;
}

extern void        yfig_dsds_free(Tyfig_dsds* dsds)
{
    odsty_freeFill( &dsds->ds_fillWhite );

    odsty_freeText( &dsds->ds_styText );
    odsty_freeLine( &dsds->ds_styLine );
    odsty_freeFill( &dsds->ds_styFill );

    //               dsds->ds_expFmt // static
    yfig_expopt_free(&dsds->ds_expOpt);
    yfig_expopt_free(&dsds->ds_prtOpt);

#if 0 && defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr
{
    int i;
    for (i=0 ; i<dsds->ds_baseColors->eleNb ; i+=1) {
        Tcolor* c = dsds->ds_baseColors->eles[i];
        int refcnt = c->cnt;
        if ( refcnt!=1 ) {
            PERM_spe_printf(" %2d/%d color=%p: refcount is %d expected 1 : %s",
                    i,dsds->ds_baseColors->eleNb, c, refcnt, DBG_Color2str(c));
        }
    }
}
#endif
    colorset_free ( dsds->ds_baseColors );

    Tstr rf_fn = tool_aprintf("%s/recent-files",gl_appRcDir);
    tool_rf_save  (dsds->ds_recentFiles,NULL,rf_fn);
    tool_rf_delete(dsds->ds_recentFiles);
    tool_free(rf_fn);

    free (dsds);
}

/*======================================================================*/
