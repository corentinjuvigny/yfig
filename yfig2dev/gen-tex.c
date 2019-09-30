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
 *$Software: YFIG2DEV
 *      $Id: gen-tex.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/gen-tex.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Convert the figure into TEX format.                                =*/
/*======================================================================*/

#include "yfig2dev-config.h"

#include "tools/lenunit.h"
#include "tools/file.h"
#include "tools/boxes.h"

#include "outdev/outdevPdf.h"

#include "fig/fig-scene.h"

static TodPdfCfg gen_tex_getPdfCfg (Tds*ds,CboxDble*srcBoxCm);

/*======================================================================*/
/*= Interface                                                          =*/

/* creation of TEX figure into a temporary file */
extern void generate_tex(Tds*ds)
{
    int status;
    FILE* stream;

  /* create and open a  temporary file */
    stream = tool_fileOpenWO(NULL,ds->outname);
    if ( stream==NULL )
      tool_ftlerr_printf(NULL,
        "conversion aborted: unable to create and open a temporary file: %s", strerror(errno));

  /* conversion of figure in TEX into the stream temporary file */
    TboxDble  srcBoxCm = ds_getSrcBBCm(ds);
    TodPdfCfg cfg      = gen_tex_getPdfCfg(ds,&srcBoxCm);
    cfg.cfg_stream  = stream;
    cfg.cfg_outFile = ds->outname;
    status = fig_scene_tex(ds->scene,ds->wMsgdrv,&cfg,srcBoxCm);
    fclose(stream);

    if ( status!=1 )
        tool_ftlerr_printf(NULL,"%s","conversion aborted: TEX generation of figure failed");
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static void gen_tex_getPaperSizeCm(Tds*ds, double*dx, double*dy)
{
    TboxDble       box;
    int            paperKind = ds->opt_paperKind;
    double         paperBdCm = userParam_getBorderThicknessCm();
    Cpaperdef*     paperDef;
    double         placeZoom;

    switch ( paperKind ) {
        case PaKd_SrcBB:
alldepth:
            placeZoom = 1;
            box = ds_getSrcBBCm(ds);
            *dx = placeZoom*Box_dx(box) + 2*paperBdCm;
            *dy = placeZoom*Box_dy(box) + 2*paperBdCm;
            break;
        case PaKd_SrcBBZ:
            placeZoom = ds_getZoomFactor(ds);
            box = ds_getSrcBBCm(ds);
            *dx = placeZoom*Box_dx(box) + 2*paperBdCm;
            *dy = placeZoom*Box_dy(box) + 2*paperBdCm;
            break;
        case PaKd_SetByUser:
            paperDef = userParam_getPaperDef();
            *dx  = paperDef->wcm;
            *dy  = paperDef->hcm;
            break;
        default:
            PERM_spf_printf("invalid paper kind %d",paperKind);
    }
}
 
static TodPdfCfg gen_tex_getPdfCfg (Tds*ds,CboxDble*srcBoxCm)
{
    // get paper size
    double paperDx,paperDy;
    gen_tex_getPaperSizeCm(ds,&paperDx,&paperDy);

    // get zoom & gravity & page matrix size
    int    setZoom     = 0; // set it to 1 to set cfg_zoom
    double zoom        = 1;
    int    gravity     = DirWR_NW;
    int    nbline      = 1;
    int    nbcolumn    = 1;
    if ( ds->opt_paperKind==PaKd_SetByUser ) {
        if ( ds->opt_placeKind==PlaceKd_ZoomUser ) {
            setZoom = 1;
            zoom    = ds_getZoomFactor(ds);
            gravity = ds_getGravity(ds);
          //nbline   = eo->nbline;
          //nbcolumn = eo->nbcolumn;
        } else if ( ds->opt_placeKind==PlaceKd_ZoomUserAPM ) {
            double paperBdCm = userParam_getBorderThicknessCm();
            setZoom = 1;
            zoom    = ds_getZoomFactor(ds);
            gravity = ds_getGravity(ds);
            // we search for nb: nb*(paper-2*bd) >= zoom*img
            //                 : nb >= (zoom*img) / (paper-2*bd)
            if ( ds_isLandscape(ds) ) {
                // landscape (see documentation)
                nbline   = ceil( zoom * Box_dy(*srcBoxCm) / (paperDx-2*paperBdCm) );
                nbcolumn = ceil( zoom * Box_dx(*srcBoxCm) / (paperDy-2*paperBdCm) );
            } else {
                // portrait
                nbline   = ceil( zoom * Box_dy(*srcBoxCm) / (paperDy-2*paperBdCm) );
                nbcolumn = ceil( zoom * Box_dx(*srcBoxCm) / (paperDx-2*paperBdCm) );
            } 
        } else if ( ds->opt_placeKind==PlaceKd_ZoomToFit ) {
            int nblineTemp,nbcolumnTemp;
            userParam_getRowColZTF(&nblineTemp,&nbcolumnTemp);
            double paperBdCm = userParam_getBorderThicknessCm();
            gravity = ds_getGravity(ds);
            if ( nblineTemp>=1 && nbcolumnTemp>=1 ) {
                nbline   = nblineTemp;
                nbcolumn = nbcolumnTemp;
            }
            if ( ds_isLandscape(ds) ) {
                // landscape (see documentation)
                double devDx = nbcolumn * (paperDy - 2*paperBdCm);
                double devDy = nbline   * (paperDx - 2*paperBdCm);
                setZoom = 1;
                zoom = min2( devDx/Box_dx(*srcBoxCm) , devDy/Box_dy(*srcBoxCm) );
            } else {
                // portrait
                double devDx = nbcolumn * (paperDx - 2*paperBdCm);
                double devDy = nbline   * (paperDy - 2*paperBdCm);
                setZoom = 1;
                zoom = min2( devDx/Box_dx(*srcBoxCm) , devDy/Box_dy(*srcBoxCm) );
            }
        } else {
            PERM_spf_printf("opt_placeKind=%d is unexpected",ds->opt_placeKind);
        }
    } else if ( ds->opt_paperKind==PaKd_SrcBBZ ) {
            zoom    = ds_getZoomFactor(ds);
    } else  {
        // last case ( ds->opt_paperKind==PaKd_SrcBB ) defaults are good 
        TEMP_asf_printf( ds->opt_paperKind!=PaKd_SrcBB, "opt_paperKind=%d is unexpected",ds->opt_paperKind);
    }

    // init cfg
    TodPdfCfg cfg; memset(&cfg,0,sizeof(cfg));
    if ( nbline>1 || nbcolumn>1 ) {
        cfg.cfg_npLine   = nbline;
        cfg.cfg_npColumn = nbcolumn;
    } else
        cfg.cfg_mode  |= OD_PDFCFG_1Page;
    if ( setZoom ) {
        cfg.cfg_mode |= OD_PDFCFG_ScaleG;
        cfg.cfg_scale = zoom;
    }
    cfg.cfg_paperDx = paperDx;
    cfg.cfg_paperDy = paperDy;
    cfg.cfg_gravity = gravity;
    if ( userParam_getBorderThicknessCm()!=0 ) {
      cfg.cfg_mode |= OD_PDFCFG_Bd1;
      cfg.cfg_bd1   = userParam_getBorderThicknessCm();
    }
    if ( ds->opt_paperKind==PaKd_SetByUser ) {
        // A printable document, that makes sens to handle
        // page orientation.
        if ( ds_isLandscape(ds) )
            cfg.cfg_mode |= OD_PDFCFG_Land;
    }

    return cfg;
}
 
/*======================================================================*/
