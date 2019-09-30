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
 *      $Id: yfig-export-option.h 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-export-option.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Option for exporting and printing figure under standard image      =*/
/*= formats.                                                           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_EXPORT_OPTION_H
#define FILE_YFIG_EXPORT_OPTION_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-types.h"
#include "tools/papersize.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef enum _TexportOptionSrcBB {
    ExpOptSrcBB_AllDepth,    // the BB of figure with all the depths
    ExpOptSrcBB_UsedDepth,   // the BB of figure with the used depths
    ExpOptSrcBB_AllDepth00,  // extends the BB of ExpOptSrcBB_AllDepth
                             // to the north and/or west to contains (0,0)
                             // Notice that if the (0,0) point is at south/east
                             // the BB is left unchanged.
} TexportOptionSrcBB;

typedef enum _TexportOptionPaKd {
    ExpOptPaKd_SrcBB,       // set to src BB + border with zoom=1 (real size)
    ExpOptPaKd_SrcBBZ,      // set to src BB + border with zoom set by user
    ExpOptPaKd_SetByUser    // set by the user
} TexportOptionPaKd;

typedef enum _TexportOptionOrient {
    ExpOptOrient_Portrait,
    ExpOptOrient_Landscape,
    ExpOptOrient_Auto   // set portrait or landscape
                        //  - to maximize the figure when ExpOptPaKd_SetByUser
                        //    & ExpOptPlace_ZoomUser are set.
                        // set to portrait in other cases.
} TexportOptionOrient;

typedef enum _TexportOptionPlace {
    ExpOptPlace_ZoomToFit,   // the zoom factor is computed to make the figure
                             // as large as possible and to fit the paper.
    ExpOptPlace_ZoomUser,    // the zoom factor is given by the user.
    ExpOptPlace_ZoomUserAPM, // APM= Adjust Page Matrix
                             // the zoom factor is given by the user and
                             // the paper matrix is adjusted to contain
                             // fully the figure.
                             // Only valid for multi-page PDF document.
} TexportOptionPlace;

struct _TexportOption {
    /**
     * What to export:
     *  1: all the depths of figure
     *  0: only the selected depths of figure
    **/
    int                srcAllDepths;
    TexportOptionSrcBB srcBBKind;

    /**
     * size of exported document.
     * Borders are excluded for kinds: ExpOptPaKd_SrcBB/SrcBBZ
     * Borders are included for kinds: ExpOptPaKd_SetByUser
    **/
    TexportOptionPaKd   paperKind;
    Tpaperdef*          paperDef;    // null for ExpOptPaKd_SrcBB/SrcBBZ
    TexportOptionOrient paperOrient; // orientation (see TexportOptionOrient type above)
    double              paperBdCm;   // border around the figure in cm

    /**
     * how to place the figure on the paper.
     *  - when placeKind is ExpOptPlace_ZoomToFit the "placeWhere" 
     *    field has no really meaning.
     *  - when placeKind is ExpOptPlace_ZoomToFit placeZoom is not used.
    **/
    TexportOptionPlace  placeKind;
    TdirWR              placeWhere;
    double              placeZoom;

    /**
     * parameters for raster image format:
     *  - ppi     : pixel per inch.
     *  - quality : for lossy format in range [0.1:1] with 0.1 bad and 1 high.
    **/
    int                 ppi;
    double              quality; 

    /**
     * parameters for multi-page PDF document:
     *  - nbline,nbcolumn : define a paper matrix
     * the layout of the document pages is:
     *    pagee0 page1 page2 page3
     *    pagee4 page5 page6 page7
    **/
    int nbline, nbcolumn;
};

/*======================================================================*/
/*= Interface                                                          =*/

TexportOption  yfig_expopt_initDefault();
TexportOption  yfig_expopt_initClone   (CexportOption* src);
void           yfig_expopt_free        (TexportOption* eo);

TexportOption* yfig_expopt_newDefault ();
TexportOption* yfig_expopt_newClone   (CexportOption* eo);
void           yfig_expopt_delete     (TexportOption* eo);

void           yfig_expopt_cpy        (TexportOption* eo, CexportOption* src);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_YFIG_EXPORT_OPTION_H
/*======================================================================*/

