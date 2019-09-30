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
 *      $Id: outdevPdf-page.c 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf-page.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Handle the PDF page tree.                                          =*/
/*=                                                                    =*/
/*= See outdev.h and outdevPdf.h headers for documentation.            =*/
/*======================================================================*/

#include "outdevPdf-priv.h"

#include "tools/string-builder.h"

/*======================================================================*/

/**
 * returns into boxes the differents geometries required
 * to generate page objects.
 *  - Box_x/y(box)   is the SW corner in PDF coordinate system 
 *    the SW corner in PDF coordinate system 
 *  - Box_dx/dy(box) give the width (to E) and height to N) of box.
 *  - Box_x(b)+Box_dx(b) and Box_y(b)+Box_dy(b) are the NE
 *    corner of the box.
 *
 *  Layout of pages (in PDF output device unit)
 *  y           c=0          c=1         c=2
 *  ^      +------------+------------+------------+
 *  |      |************|************|************| ^
 *  |      |************|************|************| |
 *  |      |**       ***|**       ***|**       ***| | ppDy
 *  |  l=0 |**  n=0  ***|**  n=1  ***|**  n=2  ***| |
 *  |      |**       ***|**       ***|**       ***| |
 *  |      |************|************|************| v
 *  |      +------------+------------+------------+
 *  |      |************|************|************|
 *  |      |************|************|************|
 *  |      |**       ***|**       ***|**       ***| ^
 *  |  l=1 |**  n=3  ***|**  n=4  ***|**  n=5  ***| | ppDyDa
 *  |    0 |**       ***|**       ***|**       ***| V
 *  |      |************|************|************|
 *  |      +------------+------------+------------+
 *  |         0                         <----->        
 *  |                                     ppDxDa           
 *  |                                                  
 *  +-----------------------------------------------------> x
 *
 *  Layout of source picture (in PDF output device unit)
 *        picDx            
 *      <------->           
 *  y  +---------+  
 *  ^  |abbbbbbba| ^
 *  |  |accccccca| | picDy
 *  |  |addddddda| |
 *  | 0|aeeeeeeea| V
 *  |  +---------+  
 *  |   0                
 *  +---------------> x
 *
 * position of picture in page layout with DirWR_SW
 *  y           c=0          c=1         c=2
 *  ^      +------------+------------+------------+
 *  |      |************|************|************| ^
 *  |      |************|************|************| |
 *  |      |**       ***|**       ***|**       ***| | ppDy
 *  |  l=0 |**  n=0  ***|**   n=1 ***|**  n=2  ***| |
 *  |      |**abbbbbb***|**ba     ***|**       ***| |
 *  |      |************|************|************| v
 *  |      +------------+------------+------------+
 *  |      |************|************|************|
 *  |      |************|************|************|
 *  |      |**acccccc***|**ca     ***|**       ***| ^
 *  |  l=1 |**adddddd***|**da     ***|**  n=5  ***| | ppDyDa
 *  |    0 |**aeeeeee***|**ea     ***|**       ***| V
 *  |      |************|************|************|
 *  |      +------------+------------+------------+
 *  |         0                         <----->        
 *  +--------> x                         ppDxDa           
 *  ====> tx=0 ; ty=0
 *
 * position of picture in page layout with DirWR_NW
 *  y           c=0          c=1         c=2
 *  ^      +------------+------------+------------+
 *  |      |************|************|************| ^
 *  |      |************|************|************| |
 *  |      |**abbbbbb***|**ba     ***|**       ***| | ppDy
 *  |  l=0 |**acccccc***|**ca n=1 ***|**  n=2  ***| |
 *  |      |**adddddd***|**da     ***|**       ***| |
 *  |      |************|************|************| v
 *  |      +------------+------------+------------+
 *  |      |************|************|************|
 *  |      |************|************|************|
 *  |      |**aeeeeee***|**ea     ***|**       ***| ^
 *  |  l=1 |**  n=3  ***|**  n=4  ***|**  n=5  ***| | ppDyDa
 *  |    0 |**       ***|**       ***|**       ***| V
 *  |      |************|************|************|
 *  |      +------------+------------+------------+
 *  |         0                         <----->        
 *  +--------> x                         ppDxDa           
 *  ====> tx=0 ; ty=layDy-picDy; 
 *
 * position of picture in page layout with DirWR_CC
 *  y           c=0          c=1         c=2
 *  ^      +------------+------------+------------+
 *  |      |************|************|************| ^
 *  |      |************|************|************| |
 *  |      |**       ***|**       ***|**       ***| | ppDy
 *  |  l=0 |**      a***|**bbbbbbb***|**a n=2  ***| |
 *  |      |**      a***|**ccccccc***|**a      ***| |
 *  |      |************|************|************| v
 *  |      +------------+------------+------------+
 *  |      |************|************|************|
 *  |      |************|************|************|
 *  |      |**      a***|**ddddddd***|**a      ***| ^
 *  |  l=1 |**      a***|**eeeeeee***|**a n=5  ***| | ppDyDa
 *  |    0 |**       ***|**       ***|**       ***| V
 *  |      |************|************|************|
 *  |      +------------+------------+------------+
 *  |         0                         <----->        
 *  +--------> x                         ppDxDa           
 *  ====> tx=(layDx-picDx)/2 ; ty=(layDy-picDy)/2; 
 *
**/

static int odpdf_getPageGeom(TodPdf* od, int n,
        TboxDble*   media,
        TboxDble*   bleed,
        TboxDble*   clip,
        double*tx,  double*ty
)
{
    int nbcol = od->pgNb/od->pgByLine;
    int l     = n/od->pgByLine;
    int c     = n%od->pgByLine;

    // (pgX,pgY) is the coordinate of the SW corner of the
    // page drawing area in the picture
    double pgX = c*od->ppDxDa;
    double pgY = (nbcol-1-l)*od->ppDyDa;

    // media box
    Box_x (*media) = pgX - od->bdL;
    Box_y (*media) = pgY - od->bdB;
    Box_dx(*media) = od->ppDx;
    Box_dy(*media) = od->ppDy;

    // bleed is media without borders
    *bleed = *media;
    Box_x (*bleed) += od->bdL;
    Box_y (*bleed) += od->bdB;
    Box_dx(*bleed)  = od->ppDxDa; // or -= od->bdL + od->bdR
    Box_dy(*bleed)  = od->ppDyDa; // or -= od->bdB + od->bdT

    // clip box is the same as the bleed box.
    *clip = *bleed;

    // Set up the translation according to th gravity.
    switch ( od->gravity ) {
        case DirWR_NN: *tx = (od->layDx-od->picDx)/2.;
                       *ty = od->layDy-od->picDy;
                       break;
        case DirWR_NE: *tx = od->layDx-od->picDx;
                       *ty = od->layDy-od->picDy;
                       break;
        case DirWR_EE: *tx = od->layDx-od->picDx;
                       *ty = (od->layDy-od->picDy)/2.;
                       break;
        case DirWR_SE: *tx = od->layDx-od->picDx;
                       *ty = 0;
                       break;
        case DirWR_SS: *tx = (od->layDx-od->picDx)/2.;
                       *ty = 0;
                       break;
        case DirWR_SW: *tx = 0; *ty = 0; break;
        case DirWR_WW: *tx = 0;
                       *ty = (od->layDy-od->picDy)/2;
                       break;
        case DirWR_NW: *tx = 0;
                       *ty = od->layDy-od->picDy;
                       break;
        case DirWR_CC: *tx = (od->layDx-od->picDx)/2. ;
                       *ty = (od->layDy-od->picDy)/2. ;
                       break;
        default:
          TEMP_spe_printf("invalid gravity (%d), set it to SW (%d).",
                od->gravity, DirWR_SW);
          *tx = 0; *ty = 0; break;
    }
}

/*======================================================================*/

#define NB_OID_BY_PAGE 2

extern TstrEaten odpdf_page_getOID(TodPdf* od, int oid, int*next_oid)
{
    int i;
    int   nbp = od->pgNb;
    char* ret = tool_malloc_0( 10*nbp );
    char* p   = ret;
    for ( i=0 ; i<nbp ; i+=1 ) {
        p += sprintf(p,"%4d 0 R",
                oid+NB_OID_BY_PAGE*i+NB_OID_BY_PAGE-1);
    }
    if ( next_oid )
        *next_oid = oid + nbp*NB_OID_BY_PAGE;
    return ret;
}

#define PDF_RECT_OF_BOX(b) \
     Box_x(b),Box_y(b), Box_x(b)+Box_dx(b),Box_y(b)+Box_dy(b)

extern int odpdf_page_addObject(TodPdf* od, int oid_pagedir)
{
    int i;
    TboxDble media,bleed,clip;
    double   tx,ty;
    int      oid;
    int   nbp = od->pgNb;

    for ( i=0 ; i<nbp ; i+=1 ) {
        odpdf_getPageGeom(od,i,&media,&bleed,&clip,&tx,&ty);
        oid=od_pdf_streamObjPrintf(od,
            "  %5.1f %5.1f m  %%\n"
            "  %5.1f %5.1f l  %%\n"
            "  %5.1f %5.1f l  %%\n"
            "  %5.1f %5.1f l  %%\n"
            "  h W n          %%\n"
            "  1 0 0 1 %5.1f %5.1f cm %%\n"
            ,
            Box_x(clip)               , Box_y(clip),
            Box_x(clip)               , Box_y(clip)+Box_dy(clip),
            Box_x(clip)+Box_dx(clip)  , Box_y(clip)+Box_dy(clip),
            Box_x(clip)+Box_dx(clip)  , Box_y(clip),
            tx                        , ty
        );
        if ( oid<0 ) return -1;

        od_pdf_addStartObjToPage(od,oid);
        oid = od_pdf_objPrintf(od,
             "  << /Type /Page\n"
             "     /MediaBox [ %6.1f %6.1f %6.1f %6.1f ]\n"
             "%%    /CropBox  [ %6.1f %6.1f %6.1f %6.1f ]\n"
             "%%    /BleedBox [ %6.1f %6.1f %6.1f %6.1f ]\n"
             "%%    /TrimBox  [ %6.1f %6.1f %6.1f %6.1f ]\n"
             "     /Rotate %d\n"
             "     /Parent %d 0 R\n"
             "     /Contents [\n"
//           "       2 0 R\n"
             "%s"
             "     ]\n"
             "     /Resources <<\n"
             "       /ProcSet [ /PDF /Text /ImageC]\n"
             "       /Pattern <<\n"
             "         /P1     3 0 R\n"
             "         /P2     4 0 R\n"
             "%s"
             "       >>\n"
             "       /Font <<\n"
             "%s"
             "       >>\n"
             "       /XObject <<\n"
             "%s"
             "       >>\n"
             "%%      /ColorSpace << /Cs12 [/Pattern /DeviceRGB] >>\n"
             "     >>\n"
             "  >>\n"
             ,
             PDF_RECT_OF_BOX(media),
             PDF_RECT_OF_BOX(media),
             PDF_RECT_OF_BOX(bleed),
             PDF_RECT_OF_BOX(bleed),
             od->landscape ? 90 : 0,
             oid_pagedir,  // parent
             od->pageobjs->str,
             od->pagepats->str,
             od->pagefonts->str,
             od->pageimgs->str
        );
        if ( oid<0 ) return -1;
    }
    return 0;
}

/*======================================================================*/
