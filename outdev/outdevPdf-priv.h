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
 *      $Id: outdevPdf-priv.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf-priv.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= PDF output device structure.                                       =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev-drv.h"
#include "outdev/outdevPdf.h"
#include "tools/bitmap.h"

/*======================================================================*/

#define PDF_OBJ_MAX  10000
#define PDF_CLR_MAX  100
#define PDF_PXM_MAX  100
#define PDF_FONT_MAX 100

#define PDF_MESS(mess) "PDF handler: " mess

/*======================================================================*/
/*= Some base Type                                                     =*/

struct _TclrPdf {
    Tcolor*    clr;
    TcolorEGS  egssave;
    TstrEaten  rgbstr;
};
typedef struct _TclrPdf TclrPdf;
typedef const   TclrPdf CclrPdf;

#if 0 // NO-PXM
struct _TpxmPdf {
    Tcolor*  fg;
    Tcolor*  bg;
    double   zf;
    Tbitmap* btm;
    int      oid;
};
typedef struct _TpxmPdf TpxmPdf;
typedef const   TpxmPdf CpxmPdf;
static int  odpdf_addPixmap(void*  dev, Cbitmap*btm, Ccolor*fgc, Ccolor* bgc);
static void odpdf_freePixmap(TpxmPdf*pxm);
#endif

// must be moved to outder-drv.h/c
extern TimageEGS odpdf_img_add (TodPdf* od, CstrBrief path, int oid, int dx, int dy);

struct _TfontPdf {
    CfontDef* font;
    TstrEaten pdfidstr; // PDF ident of font: /FontId
                        // NULL if font is not yet defined.
    int       oid;      // >1 defined so fontstr can be used
                        // eg: BT <pdfidstr> 12 T ... ET
};
typedef struct _TfontPdf TfontPdf;
typedef const   TfontPdf CfontPdf;
extern int  odpdf_addFont (TodPdf* od, CfontDef*font);
static void odpdf_freeFont(TfontPdf*font);

/*======================================================================*/
/*= Graphic context                                                    =*/

struct _TpdfTile {
    int     idx;        // stf_pattern 
    Tcolor* bgclr;      // stf_color
    int     oid;        // PDF OID of this tile
};
typedef struct _TpdfTile TpdfTile;
typedef const   TpdfTile CpdfTile;

typedef struct _TpdfTileSet TpdfTileSet;
typedef const   TpdfTileSet CpdfTileSet;
#define TPLSET pdfTile
#include "tools/set-header-template.h"

struct _TgcPdf {
    int          gcd;      // current gcd
    TstrEaten    previous; // previous for OD_GC_UsrDraw/Fill
    TpdfTileSet* tiles;   // fill patterns already used
};
typedef struct _TgcPdf TgcPdf;
typedef const   TgcPdf CgcPdf;

extern void odpdf_gc_init(TgcPdf* gc);
extern void odpdf_gc_free(TgcPdf* gc);

/**
 * odpdf_gc_get0 returns the string that sets the GC for drawing lines
 * and filling shape (no text).
 * odpdf_gc_get is similar to odpdf_gc_get0 except that it returns the NULL
 * pointer if the gcd GC has not changed since the previous invocation.
 * 
 * The caller must free the returned string.
**/
TstrEaten odpdf_gc_get0(TodPdf* od, Tod_GCdef gcd);
TstrEaten odpdf_gc_get (TodPdf* od, Tod_GCdef gcd);

/*======================================================================*/
/*= PDF output device type                                             =*/

struct _TodPdf {
    ODDRV_FIELDS();

    // Paper geometries in PDF unit (point=1/72 Inch)
    //  - They follow the orientation (eg: bdT in landscape mode is the left
    //    border of the paper (in portrait))
    //  - the relationship between these lengths is:
    //      bdL + ppDxDa + bdR = ppDx
    //      bdB + ppDyDa + bdT = ppDy
    double   ppDx,   ppDy;    // paper geometry including borders
    double   bdL,bdR,bdT,bdB; // thickness of borders (Left Right Top Bottom)
    double   ppDxDa, ppDyDa;  // paper geometry of paper drawing area.

    // Picture geometries in PDF unit (point=1/72 Inch)
    // They are used to place picture on the pages using gravity.
    //  - Note that if they are equals, they have no effect.
    //  - we have the following relationships:
    //     Box_dx(od->boxDes) = layDx
    //     Box_dy(od->boxDes) = layDy
    double   layDx, layDy;    // the layout size got
                              // by the union of the page drawing areas.
    double   picDx, picDy;    // source picture size 

    unsigned mode;
    double   scale;
    int      landscape;       // 0:portrait ; 1:landscape
    int      gravity;

    int      pgNb;            // number of page (multiple of pgByLine)
    int      pgByLine;        // number of page by line (>0)

    int objnb;
    int offset;
    int offsets[PDF_OBJ_MAX];

    TclrPdf    clrs[PDF_CLR_MAX];
    int        clrn;

#if 0 // NO-PXM
    TpxmPdf    pxms[PDF_PXM_MAX];
    int        pxmn;
#endif

    TfontPdf   fonts[PDF_FONT_MAX];
    int        fontn;

    Tstrfmt*   pageobjs;
    Tstrfmt*   pagepats;
    Tstrfmt*   pagefonts;
    Tstrfmt*   pageimgs;

    TgcPdf     gc;

    int      GCstrFontIdx;   // index in fonts
    double   GCstrFontSz;    // in pts
    int      GCstrColorIdx;  // index clrs
};

/*======================================================================*/
/*= Miscellaneous utilities (outdefPdf-priv.c)                         =*/

/**
 * The od_pdf_addPatternToPage function appends the item
 * "/P<oid> <oid> 0 R" to the od->pagepats string.
 * The type of the oid PDF object must be a /pattern.
 *
 * The od_pdf_addObjToPage function appends the item "<*oid> 0 R" to the
 * od->pageobjs string and then sets *oid to zero.
 * The oid PDF object must be a drawing object.
 * The function does nothing if oid is less than zero.
 *
 * The od_pdf_addStartObjToPage function behavior depends on the oid value:
 *  - oid<1 : it appends the item "0 0 R" to the od->pageobjs string.
 *  - oid>=1: it rewrites the first item of the od->pageobjs string by
 *            "<*oid> 0 R".
 * So its standard use is:
 *  - to invoke "od_pdf_addStartObjToPage(od,0)" at initialization time
 *    of the output device for starting the od->pageobjs string by a
 *    dummy object reference.
 *  - then to invoke "od_pdf_addStartObjToPage(od,page_start_oid)" for
 *    every page. That replaces the dummy object reference by
 *    a reference to the page_start_oid object.
**/
extern void od_pdf_addPatternToPage(TodPdf*od, int oid);

extern void od_pdf_addObjToPage(TodPdf*od, int*oid);

extern void od_pdf_addStartObjToPage(TodPdf*od, int oid);

/*======================================================================*/
/*= Interface to outdevPDF-pages.c                                     =*/

/**
 * The odpdf_page_getOID function returns a string that contains
 * PDF object references to all the page PDF object.
 * It assumes that the first OID to use is oid.
 * It returns into *next_oid the OID that is free after the page objects.
 *
 * The returned string must be freed by caller after use.
 *
 * Parameters:
 *  od   : the PDF output device.
 *  oid  : the page OID start.
 *  next_oid : the first OID free after page objects.
 *             It may be the null pointer.
**/
extern TstrEaten odpdf_page_getOID(TodPdf* od, int oid, int*next_oid);

/**
 * The odpdf_page_addObject function adds to the PDF stream the pages
 * objects.
 * The OID of the parent directory of pages is oid_pagedir.
 *
 * Return:
 *  The function returns 0 on success and -1 otherwise.
**/
extern int odpdf_page_addObject(TodPdf* od, int oid_pagedir);


/*======================================================================*/
