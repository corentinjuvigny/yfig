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
 *      $Id: yfig-dsds.h 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-dsds.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data shared by all Tyfig_mw main widgets                           =*/
/*=                                                                    =*/
/*= It defines the default attributes when a new YFIG widget is        =*/
/*= created or a new figure is loaded.                                 =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_DSDS_H
#define FILE_YFIG_DSDS_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-types.h"

#include "tools/papersize.h"
#include "tools/lenunit.h"
#include "outdev/style.h"

/*======================================================================*/
/*= Data structucure                                                   =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= General parameters                                                 =*/

typedef struct _TdsdsParam { 
    const Tpaperdef *dsp_paperSize;  // definition of paper page

    // user unit: In YFIG all the geometric coordonates and lengths
    // are in FU, so all the computationsare done in FU.
    // When such data are printed to the user, they are often
    // printed in user unit.
    TuserUnit dsp_userUnit;

    // default GC
    Tsty_text   dsp_textDraw;   // default param for drawing line
    Tsty_line   dsp_lineDraw;   // default param for drawing line
    Tsty_fill   dsp_fillDraw;   // default param for filling area
} TdsdsParam;

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Export & print parameters                                          =*/

#include "yfig-export-option.h"
#include "tools/imgfmt.h"

typedef struct _TdsdsExPr TdsdsExPr; 
typedef const   TdsdsExPr CdsdsExPr; 

struct _TdsdsExPr { 
    // default format and option for exporting figure.
    CimgFmtDt*    dse_expFmt; // image format for exporting
    TexportOption dse_expOpt; // options for exporting
    TexportOption dse_prtOpt; // options for printing
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= All the parameters.                                                =*/

struct _Tyfig_dsds {
    Tvgui*       gui;
    TcolorSet*   ds_baseColors;  // base color set for figure.
                                 // they are shared by all the figure.
    Tsty_fill    ds_fillWhite;   // fill style Solid and white color
    TarrowDesc   ds_begArrow;    // begin arrow at start time
    TarrowDesc   ds_endArrow;    // end arrow at start time
    TdsdsParam   ds_defaults;    // default parameters
#define ds_paperSize ds_defaults.dsp_paperSize
#define ds_userUnit  ds_defaults.dsp_userUnit
#define ds_styText   ds_defaults.dsp_textDraw
#define ds_styLine   ds_defaults.dsp_lineDraw
#define ds_styFill   ds_defaults.dsp_fillDraw
    TdsdsExPr   ds_exportPrint;  // default parameters for exporting and printing
#define ds_expFmt   ds_exportPrint.dse_expFmt
#define ds_expOpt   ds_exportPrint.dse_expOpt
#define ds_prtOpt   ds_exportPrint.dse_prtOpt
    TrecentFiles*ds_recentFiles; // maximum number of paths in ds_lastPaths
};

extern Tyfig_dsds* yfig_dsds_new (Tvgui* gui);
extern void        yfig_dsds_free(Tyfig_dsds* dsds);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YFIG_DSDS_H
/*======================================================================*/
