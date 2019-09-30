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
 *      $Id: outdevPdf.h 387 2019-01-26 22:53:16Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/outdevPdf.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Abstract type for translating something into PDF.                  =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_OUTDEVPDF_H
#define FILE_OUTDEV_OUTDEVPDF_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "outdev/outdev.h"
#include "tools/color-types.h"
#include "tools/strings.h"
#include "tools/papersize.h"
#include "tools/font.h"

/*======================================================================*/
/*= Types                                                              =*/

typedef struct _TodPdf TodPdf;
typedef const   TodPdf CodPdf;

/* configuration mode */
#define OD_PDFCFG_1Page   0x0001   // generate a single page
#define OD_PDFCFG_MPage   0x0002   // generate a page marix with
                                   //   - cfg_npLine   pages by columm
                                   //   - cfg_npColumn pages by line
#define OD_PDFCFG_ScaleG  0x0004   // scale is given and
                                   // 1 source cm = cfg_scale cm on page.
#define OD_PDFCFG_Land    0x0008   // use page layout in landscape mode
#define OD_PDFCFG_Bd1     0x0010   // use cfg_bd1 for the four borders
#define OD_PDFCFG_Bd2     0x0020   // use cfg_bdH for left and right borders
                                   // use cfg_bdV for top and bottom borders
#define OD_PDFCFG_Bd4     0x0020   // use cfg_bdL/R for left and right borders
                                   // use cfg_bdT/B for top and bottom borders

/**
 * Configuration parameters of PDF output device.
 *  - Unused parameter must be set to low value.
 *  - Default value of parameter is low value.
**/
struct _TodPdfCfg {
    FILE*         cfg_stream;
    Cstr          cfg_outFile;
    unsigned      cfg_mode;  // see OD_PDFCFG_... macros

    double        cfg_scale; // used if cfg_mode&OD_PDFCFG_ScaleG

  /* paper geometry including borders
   * you must provide either cfg_paper or cfg_paperDx/Dy */
    Cpaperdef*    cfg_paper;               // in cm
    double        cfg_paperDx,cfg_paperDy; // in cm

  /* gravity: only used if image is smaller than the page drawing area */
    TdirWR        cfg_gravity;  // see OD_PDFGRAV_...

  /* defines the drawing page area by adding borders in cm */
#define           cfg_bd1 bd[0] // used if cfg_mode&OD_PDFCFG_BD1
#define           cfg_bdH bd[0] // used if cfg_mode&OD_PDFCFG_BD2
#define           cfg_bdV bd[2]
#define           cfg_bdL bd[0] // used if cfg_mode&OD_PDFCFG_BD4
#define           cfg_bdR bd[1]
#define           cfg_bdT bd[2]
#define           cfg_bdB bd[3]
    double        bd[4]; // do not use it but former macros

  /* define the page matrix */
    int cfg_npLine;   // The number of page vertically   (>1)
    int cfg_npColumn; // The number of page horizontally (>1)
} ;

#define ODPDF_CFG_RetD1(func,x)   Inline double func(CodPdfCfg*cfg) { return x; }
#define ODPDF_CFG_RetD2(func,x,y) Inline Tdble2 func(CodPdfCfg*cfg) { Tdble2 ret=dble2_init(x,y); return ret; }
#define ODPDF_CFG_RetI2(func,x,y) Inline Tsint2 func(CodPdfCfg*cfg) { Tsint2 ret=sint2_init(x,y); return ret; }
ODPDF_CFG_RetI2(odpdf_cfg_getLiCo, (cfg->cfg_mode&OD_PDFCFG_1Page)!=0?1:cfg->cfg_npLine,(cfg->cfg_mode&OD_PDFCFG_1Page)!=0?1:cfg->cfg_npColumn)
ODPDF_CFG_RetD1(odpdf_cfg_getBdL,  (cfg->cfg_mode&OD_PDFCFG_Bd1)!=0?cfg->cfg_bd1: (cfg->cfg_mode&OD_PDFCFG_Bd2)!=0?cfg->cfg_bdH:(cfg->cfg_mode&OD_PDFCFG_Bd4)!=0?cfg->cfg_bdL:0)
ODPDF_CFG_RetD1(odpdf_cfg_getBdR,  (cfg->cfg_mode&OD_PDFCFG_Bd1)!=0?cfg->cfg_bd1: (cfg->cfg_mode&OD_PDFCFG_Bd2)!=0?cfg->cfg_bdH:(cfg->cfg_mode&OD_PDFCFG_Bd4)!=0?cfg->cfg_bdR:0)
ODPDF_CFG_RetD1(odpdf_cfg_getBdT,  (cfg->cfg_mode&OD_PDFCFG_Bd1)!=0?cfg->cfg_bd1: (cfg->cfg_mode&OD_PDFCFG_Bd2)!=0?cfg->cfg_bdV:(cfg->cfg_mode&OD_PDFCFG_Bd4)!=0?cfg->cfg_bdT:0)
ODPDF_CFG_RetD1(odpdf_cfg_getBdB,  (cfg->cfg_mode&OD_PDFCFG_Bd1)!=0?cfg->cfg_bd1: (cfg->cfg_mode&OD_PDFCFG_Bd2)!=0?cfg->cfg_bdV:(cfg->cfg_mode&OD_PDFCFG_Bd4)!=0?cfg->cfg_bdB:0)
ODPDF_CFG_RetD2(odpdf_cfg_getPapSz, cfg->cfg_paper!=0?cfg->cfg_paper->wcm:cfg->cfg_paperDx, cfg->cfg_paper!=0?cfg->cfg_paper->hcm:cfg->cfg_paperDy)
ODPDF_CFG_RetD2(odpdf_cfg_getPapDa, odpdf_cfg_getPapSz(cfg).x-odpdf_cfg_getBdL(cfg)-odpdf_cfg_getBdR(cfg), odpdf_cfg_getPapSz(cfg).y-odpdf_cfg_getBdT(cfg)-odpdf_cfg_getBdB(cfg))
ODPDF_CFG_RetD2(odpdf_cfg_getDocDa, odpdf_cfg_getLiCo(cfg).y*(cfg->cfg_mode&OD_PDFCFG_Land ? odpdf_cfg_getPapDa(cfg).y : odpdf_cfg_getPapDa(cfg).x),
                                    odpdf_cfg_getLiCo(cfg).x*(cfg->cfg_mode&OD_PDFCFG_Land ? odpdf_cfg_getPapDa(cfg).x : odpdf_cfg_getPapDa(cfg).y) )
Inline double   odpdf_cfg_getZomm(CodPdfCfg*cfg,double srcDxCm, double srcDyCm) {
    if ( (cfg->cfg_mode&OD_PDFCFG_ScaleG)!=0 ) return cfg->cfg_scale;
    if ( srcDxCm==0 || srcDyCm==0 )   return 1;
    double layDx = odpdf_cfg_getDocDa(cfg).x;
    double layDy = odpdf_cfg_getDocDa(cfg).y;
    //return cfg->cfg_mode&OD_PDFCFG_Land ? min2(layDx/srcDyCm,layDy/srcDxCm) : min2(layDx/srcDxCm,layDy/srcDyCm);
    return min2(layDx/srcDxCm,layDy/srcDyCm);
}

/*======================================================================*/
/*= Constructor and destructor                                         =*/

/**
 * The odpdf_new function creates a PDF output device for a source
 * picture the size of which is srcDx and srcDy centimeters.
 * The PDF output device is configurated by the cfg configuration.
 *
 * The odpdf_delete function deletes a PDF output device by freeing all
 * the memory that it allocated.
 *
 *    +---------------+----------+----------+-----------------------+
 *    | cfg_fields    | ...1Page | ...MPage | default               |
 *    +---------------+----------+----------+-----------------------+
 *    | scale         | *        | *        | the greatest possible |
 *    +---------------+----------+----------+-----------------------+
 *    | paper or      | m        | m        |                       |
 *    | paperDx/Dy    |          |          |                       |
 *    +---------------+----------+----------+-----------------------+
 *    | gravity       | m        | m        | 0=OD_PDFGRAV_NW       |
 *    +---------------+----------+----------+-----------------------+
 *    | bd1/HV/LRBT   | *        | *        | zero                  |
 *    +---------------+----------+----------+-----------------------+
 *    | npLine/Column |          | m        | none                  |
 *    +---------------+----------+----------+-----------------------+
 *      m: mandatory
**/

extern TodPdf* odpdf_new   (Tmsgdrv*md, FILE* stream, CodPdfCfg* cfg,
                             double srcDx, double srcDy);
extern void    odpdf_delete(TodPdf* od);

/**
 * The odpdf_doHeader prints into the od stream the PDF header.
 * It must be invoked once before writing anything in the od stream.
 *
 * The odpdf_doFooter prints into the od stream the PDF footer that
 * makes a single page the contents of which is the object given by
 * the od_pdf_getPageObject string.
 *
 * Return:
 *  These functions return 0 on success and -1 on I/O error.
**/
extern int     odpdf_doHeader(TodPdf* od);
extern int     odpdf_doFooter(TodPdf* od);

/*======================================================================*/
/*= Utilities                                                          =*/
 
/**
 * The odpdf_write function writes into the od->stream the first datalen
 * bytes of the data buffer.
 *
 * The odpdf_puts function prints into the od->stream the str string.
 *
 * The od_pdf_printf function generates a string with the fmt format
 * and the ... arguments as printf does. Then it invokes
 * "od_pdf_puts(od,string)".
 *
 * The od_pdf_vprintf function is equivalent to the od_pdf_printf
 * function except that it uses a va_list instead of a variable number
 * of arguments.
 *
 * On success these functions update the od->offset variable that
 * matches the current size of the PDF stream.
 *
 * Return:
 *  On success, these functions return the number of bytes written into
 *  the stream otherwise they return -1 (I/O problem).
**/
extern int  od_pdf_write  (TodPdf*od, const void* data, int datalen);
extern int  od_pdf_puts   (TodPdf*od, CstrBrief str);
extern int  od_pdf_printf (TodPdf*od, CstrBrief fmt,...);
extern int  od_pdf_vprintf(TodPdf*od, CstrBrief fmt, va_list ap);

/**
 * These functions append to the od->stream stream a PDF object:
 *   N 0 obj
 *     <object-data>
 *   endobj
 *
 * The od_pdf_objWrite function uses the first datalen bytes of the data
 * buffer for the <object-data>.
 * The od_pdf_objPuts function uses the str string for the <object-data>.
 *
 * The od_pdf_objPrintf function generates a string with the fmt format
 * and the ... arguments as printf does and uses it for the <object-data>.
 *
 * The od_pdf_objVPrintf function is equivalent to the od_pdf_objPrintf
 * function except that it uses a va_list instead of a variable number
 * of arguments.
 *
 * Return:
 *  On success, the functions return the PDF object identifier (N) of the
 *  PDF object that is appended to the stream
 *  otherwise they return -1 (I/O problem).
**/
extern int od_pdf_objStart  (TodPdf*od);
extern int od_pdf_objEnd   (TodPdf*od);
extern int od_pdf_objWrite  (TodPdf*od, const void* data, int datalen);
extern int od_pdf_objPuts   (TodPdf*od, CstrBrief str);
extern int od_pdf_objPrintf (TodPdf*od, CstrBrief fmt, ...);
extern int od_pdf_objVPrintf(TodPdf*od, CstrBrief fmt, va_list ap);

/**
 * These functions append to the od->stream stream a PDF stream object:
 *   N 0 obj
 *      << /Length <stream-data-len> >>\n"
 *   stream
 *   <stream-data>
 *   endstream
 *   endobj
 *
 * The od_pdf_streamObjWrite function uses the first datalen bytes
 * of the data buffer for the <stream-data>.
 * The od_pdf_streamObjPuts function uses the str string for the <stream-data>.
 *
 * The od_pdf_streamObjPrintf function generates a string with the fmt format
 * and the ... arguments as printf does and uses it for the <stream-data>.
 *
 * The od_pdf_streamObjVPrintf function is equivalent to the
 * od_pdf_streamObjPrintf function except that it uses a va_list instead
 * of a variable number of arguments.
 *
 * Return:
 *  On success, the functions return the PDF stream object identifier (N) of the
 *  PDF object that is appended to the stream
 *  otherwise they return -1 (I/O problem).
**/
extern int od_pdf_streamObjWrite  (TodPdf*od,const void* data, int datalen);
extern int od_pdf_streamObjPuts   (TodPdf*od,CstrBrief str);
extern int od_pdf_streamObjPrintf (TodPdf*od,CstrBrief fmt,...);
extern int od_pdf_streamObjVPrintf(TodPdf*od,CstrBrief fmt, va_list ap);

extern int od_pdf_streamObjPrintfWithDico (TodPdf*od, CstrBrief dico,
                                           CstrBrief fmt,...);
extern int od_pdf_streamObjVPrintfWithDico(TodPdf*od, CstrBrief dico,
                                           CstrBrief fmt, va_list ap);

extern char* od_pdf_rgb8TOstr(char*buf, CcolorRGB*rgb8);
extern void od_pdf_addColor(TodPdf*od, Ccolor* c);

/*======================================================================*/
/*= Fonts                                                              =*/

/**
 * The odpdf_addFont function records the f font into the od PDF output
 * device.
 * All the fonts that will be used must be recorded at start time (before
 * invoking any string drawing function.
 *
 * The function returns the PDF number of odpdf_addFont invocation from
 * the od creation. This number becomes the font identifier for the PDF
 * output device.
**/
extern int  odpdf_addFont (TodPdf*od, CfontDef*f);

/*======================================================================*/
/*= LaTeX driver                                                       =*/

/**
 * The odtex_new function creates a TEX output device for a source
 * picture the size of which is srcDx and srcDy centimeters.
 * The TEX output device is configurated by the cfg configuration.
 *
 * The odtex_delete function deletes a TEX output device by freeing all
 * the memory that it allocated.
 *
 * The cfg configuration is limited. Only OD_PDFCFG_ScaleG and OD_PDFCFG_Bd1
 * are supported for the mode.
 * 
 *    +---------------+----------+-----------------------+
 *    | cfg_fields    |          | default               |
 *    +---------------+----------+-----------------------+
 *    | mode          | *        | zero                  |
 *    +---------------+----------+-----------------------+
 *    | scale         | *        | the greatest possible |
 *    +---------------+----------+-----------------------+
 *    | paper or      | m        |                       |
 *    | paperDx/Dy    |          |                       |
 *    +---------------+----------+-----------------------+
 *    | gravity       | nu       |                       |
 *    +---------------+----------+-----------------------+
 *    | bd1           | *        | zero                  |
 *    +---------------+----------+-----------------------+
 *    | npLine/Column | nu       |                       |
 *    +---------------+----------+-----------------------+
 *      m: mandatory;  nu: not used
**/

typedef struct _TodTex TodTex;
typedef const   TodTex CodTex;

extern TodTex* odtex_new   (Tmsgdrv*md, CodPdfCfg* cfg,
                            double srcDx, double srcDy);
extern void    odtex_delete(TodTex* od);

extern int     odtex_doHeader(TodTex* od);
extern int     odtex_doFooter(TodTex* od);

/*======================================================================*/
/*= SVG driver                                                       =*/

typedef struct _TodSvg TodSvg;
typedef const   TodSvg CodSvg;

extern TodSvg* odsvg_new   (Tmsgdrv*md, FILE* stream, CodPdfCfg* cfg,
                            double srcDx, double srcDy);
extern void    odsvg_delete(TodSvg* od);

extern int     odsvg_doHeader(TodSvg* od);
extern int     odsvg_doFooter(TodSvg* od);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_OUTDEVPDF_H
/*======================================================================*/

