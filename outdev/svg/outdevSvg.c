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
 *      $Id: outdevSvg.c 387 2019-01-26 22:53:16Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/svg/outdevSvg.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See outdev.h and outdevPdf.h headers for documentation.            =*/
/*======================================================================*/

#include "outdevSvg-priv.h"
#include "outdev/outdev.h"
#include "outdev/resources.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"
#include "outdev/style.h"

#include "tools/lenunit.h"

/*======================================================================*/
/*= local utilities                                                    =*/

/*======================================================================*/
/*= Constructor and destructor                                         =*/

extern TodSvg* odsvg_new (Tmsgdrv*md,  FILE* stream, CodPdfCfg* cfg,
                          double srcDx, double srcDy)
{
    TodSvg* ret;
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

  /* sanity check */
    if ( daDx<1 || daDy<1 ) {
        tool_errerr_printf(md,"Invalid parameters for SVG generation: %s",
                "borders too large for paper");
        return NULL;
    }

  /* creation of output device */
    TboxDble box;
    Box_x (box)  = 0;
    Box_y (box)  = 0;
    Box_dx(box)  = daDx;
    Box_dy(box)  = daDy;
    ret = (TodSvg*)od_new0( &svg_vtable, LUNIT_CmPerInch, &box );

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
    ret->stream    = stream;

    return ret;
}

extern void    odsvg_delete(TodSvg* od) { od_delete( &od->any ); }

extern int     odsvg_doHeader(TodSvg* od)
{
    int status;

    od_setUsrZoom_NW(&od->any,od->scale);
    oddrv_initInHead( od );
    DBG_OdDrvDump(od,od->scale,od->picDx,od->picDy,od->ppDx,od->ppDy,od->daDx,od->daDy);

    status = fprintf(od->stream,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
        "<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
        "    x=\"0cm\" y=\"0cm\" width=\"%.2fcm\" height=\"%.2fcm\" viewBox=\"%.2f %.2f %.2f %.2f\"\n"
//      "    enable-background=\"new 0 0 %.2f %.2f\"\n"
//      "    xml:space=\"preserve\"\n"
//      "    style=\"border:0.03cm solid #FF0000\"\n"
        ">\n"
        ,od->ppDx,od->ppDy
        ,0.0,0.0,od->ppDx,od->ppDy
    );
    return 0;
}

extern int     odsvg_doFooter(TodSvg* od)
{
    int status;
    status = fprintf(od->stream,"%s",
        "</svg>\n"
    );
    oddrv_genImgFile( (TodDrv*)od );
    if ( status <= 0 ) return -1;
    return 0;
}

/*======================================================================*/
