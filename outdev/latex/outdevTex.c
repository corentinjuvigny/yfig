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
 *      $Id: outdevTex.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/latex/outdevTex.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See outdev.h and outdevPdf.h headers for documentation.            =*/
/*======================================================================*/

#include "outdevTex-priv.h"
#include "outdev/resources.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"
#include "outdev/style.h"

#include "tools/lenunit.h"

/*======================================================================*/
/*= local utilities                                                    =*/

/*======================================================================*/
/*= Constructor and destructor                                         =*/

extern TodTex* odtex_new   (Tmsgdrv*md, CodPdfCfg* cfg,
                            double srcDx, double srcDy)
{
    TodTex* ret;
    unsigned int mode = cfg->cfg_mode;

    DBG_OdDrvCfg(cfg,srcDx,srcDy);

  /* Get the paper geometry */
    double ppDx, ppDy;          // paper size (in cm)
    double daDx, daDy;          // drawing area size (in cm)
    double bd;                  // border thickness (in cm)
    if ( cfg->cfg_paper!=0 ) {
        ppDx = cfg->cfg_paper->wcm;
        ppDy = cfg->cfg_paper->hcm;
    } else {
        ppDx = cfg->cfg_paperDx;
        ppDy = cfg->cfg_paperDy;
    }
    if ( (mode&OD_PDFCFG_Bd1)!=0 ) {
        bd = cfg->cfg_bd1;
    } else {
        bd = 0;
    }

    daDx = ppDx - 2*bd;
    daDy = ppDy - 2*bd;

  /* Compute the scale used from source device to PDF output device
   * and the picture size in PDF unit. */
    double scale;
    double picDx = srcDx; // LUNIT_CmToPt( srcDx );
    double picDy = srcDy; //LUNIT_CmToPt( srcDy );
    if ( (mode&OD_PDFCFG_ScaleG)!=0 ) {
        scale = cfg->cfg_scale;
    } else if ( srcDx==0 || srcDy==0 ) {
        scale = 1;
    } else /* if ( (mode&OD_PDFCFG_ScaleG)==0 ) */ {
        scale = min2(daDx/picDx,daDy/picDy);
    }
    //picDx = scale*picDx;
    //picDy = scale*picDy;

  /* sanity check */
    if ( daDx<1 || daDy<1 ) {
        tool_errerr_printf(md,"Invalid parameters for TEX generation: %s",
                "border too large for paper");
        return NULL;
    }

  /* creation of output device */
    TboxDble box;
    Box_x (box)  = 0;
    Box_y (box)  = 0;
    Box_dx(box)  = daDx;
    Box_dy(box)  = daDy;
    ret = (TodTex*)od_new0( &tex_vtable, LUNIT_CmPerInch, &box );

  /* initialisation of output device */
    ret->any.msgdrv = md;
    ret->picDx     = picDx;
    ret->picDy     = picDy;
    ret->ppDx      = ppDx;
    ret->ppDy      = ppDy;
    ret->daDx      = daDx;
    ret->daDy      = daDy;
    ret->border    = bd;
    ret->scale     = scale;
    ret->stream    = cfg->cfg_stream;

    oddrv_initInNew( ret , cfg->cfg_outFile );

    return ret;
}

extern void    odtex_delete(TodTex* od) { od_delete( &od->any ); }

extern int     odtex_doHeader(TodTex* od)
{
    int status;

    od_setUsrZoom_NW(&od->any,od->scale);
    oddrv_initInHead( od );
    DBG_OdDrvDump(od,od->scale,od->picDx,od->picDy,od->ppDx,od->ppDy,od->daDx,od->daDy);

    status = fprintf(od->stream,"\\begin{tikzpicture}[x=%.3fcm,y=%.3fcm]\n" ,1., 1.);
    status = fprintf(od->stream,"  \\definecolor{yfigclr}{RGB}{255,255,255}"
        "\\draw[draw=yfigclr,line width=0.0pt] (0,0) rectangle (%5.2f,%5.2f) ;\n",
        od->ppDx,od->ppDy);
    return 0;
}

extern int     odtex_doFooter(TodTex* od)
{
    int status;
    status = fprintf(od->stream,"%s", "\\end{tikzpicture}\n");
    oddrv_genImgFile( (TodDrv*)od );
    if ( status <= 0 ) return -1;
    return 0;
}

/*======================================================================*/
