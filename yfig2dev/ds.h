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
 *      $Id: ds.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/ds.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data structure of YFIG2DEV.                                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_DS_H
#define FILE_DS_H
/*======================================================================*/

#include "fig/fig-types.h"

#include "tools/boxes.h"
#include "tools/imgfmt.h"

/*======================================================================*/
/*= The YFIG2DEV data structure                                        =*/

// which kind of figure bounding box must be drawn
#define SrcBB_AllDepth   1 // the BB of figure with all the depths
#define SrcBB_UsedDepth  2 // the BB of figure with the used depths
#define SrcBB_AllDepth00 3 // extends the BB of SrcBB_AllDepth
                           // to the north and/or west to contains (0,0)
                           // Notice that if the (0,0) point is at south/east
                           // the BB is left unchanged.

// which kind (size) of output paper is used
#define   PaKd_SrcBB     1 // set to src BB + border with zoom=1 (real size)
#define   PaKd_SrcBBZ    2 // set to src BB + border with zoom set by user
#define   PaKd_SetByUser 3 // set by the user

// How the figure is placed on the paper or on the papers.
// These macros specify sub cases of PaKd_SetByUser.
#define PlaceKd_ZoomToFit   1 // the zoom factor is computed to make the figure
                              // as large as possible and to fit the paper.
#define PlaceKd_ZoomUser    2 // the zoom factor is given by the user.
#define PlaceKd_ZoomUserAPM 3 // APM= Adjust Page Matrix
                              // the zoom factor is given by the user and
                              // the paper matrix is adjusted to contain
                              // fully the figure.
                              // Only valid for multi-page PDF document.

typedef struct _Tds {
    Tmsgdrv*  wMsgdrv;   // mess. driver to use for external jobs.
    char*     inname;
    FILE*     instream;
    char*     outname;
    FILE*     outstream;
    char*     outformat;
    
    CimgFmtDt* opt_imgfmt;
    int        opt_srcBBKind;  // see SrcBB_... macros
    int        opt_paperKind;  // see PaKd_...  macros
    int        opt_placeKind;  // see PlaceKd_... macros

    Tfig_scene* scene;
    char*      pdfname;
} Tds;

// initialization & freeing
extern Tds ds_init0();              // stdin    & stdout
extern Tds ds_init1(CstrBrief in);  // fig file & stdout
extern Tds ds_init2(CstrBrief in, CstrBrief out);  // fig & out files
extern Tds ds_free (Tds* ds);

// update the opt_... fields using the loaded figure and the
// user options.
extern void ds_updateOption(Tds*ds);

// smart getters (must be called after ds_updateOption).
extern double    ds_getZoomFactor(Tds*ds);
extern TboxDble  ds_getSrcBBCm   (Tds*ds);
extern int       ds_isLandscape  (Tds*ds);
extern TdirWR    ds_getGravity   (Tds*ds);

/*======================================================================*/
/*= utilities                                                          =*/

extern void generate_tex(Tds*ds);
extern void generate_svg(Tds*ds);
extern void generate_pdf(Tds*ds);

/*======================================================================*/
#endif //  FILE_DS_H
/*======================================================================*/

