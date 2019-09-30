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
 *      $Id: outdevPdf.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See outdev.h and outdevPdf.h headers for documentation.            =*/
/*======================================================================*/

#include "outdevPdf-priv.h"
#include "outdev/string-hl.h"
#include "outdev/style.h"

#include "tools/string-builder.h"
#include "tools/color.h"
#include "tools/font.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= local utilities                                                    =*/

#if 0 // NO-PXM
// returns index in pxms
static int odpdf_addPixmap(void*  dev, Cbitmap*btm, Ccolor*fgc, Ccolor* bgc)
{
    TodPdf* od = dev;

    if ( od->pxmn == PDF_PXM_MAX ) {
        tool_errerr_printf(od_msgdrv(&od->any),PDF_MESS("Too many pixmaps (max is %d)"),PDF_PXM_MAX);
        return PDF_PXM_MAX-1;
    }

    int ret = od->pxmn++;
    TpxmPdf* pxm = &od->pxms[ret]; 
    pxm->fg = color_newLazy( fgc );
    pxm->bg = color_newLazy( bgc );
    pxm->zf = 1.0;
    pxm->btm = btm_newLazy( btm );
    return ret;
}

static void odpdf_freePixmap(TpxmPdf*pxm)
{
    color_delete( pxm->fg  );
    color_delete( pxm->bg  );
    btm_delete  ( pxm->btm );
}
#endif

// returns index in fonts
extern int  odpdf_addFont (TodPdf* od, CfontDef*font)
{
    //TodPdf* od = dev;
    if ( od->fontn == PDF_FONT_MAX ) {
        tool_errerr_printf(od_msgdrv(&od->any),PDF_MESS("Too many fonts (max is %d)"),PDF_PXM_MAX);
        return 0; // often the default font
    }

    int ret = od->fontn++;
    TfontPdf* f = &od->fonts[ret]; 
    f->font = font;
    return ret;
}
static void odpdf_freeFont(TfontPdf*font)
{
    tool_free( font->pdfidstr );
}

/*======================================================================*/
/* The vtable                                                          =*/
#include "outdevPdf-vt.c"

/*======================================================================*/
/*= Constructor and destructor                                         =*/

extern TodPdf* odpdf_new   (Tmsgdrv*md, FILE* stream, CodPdfCfg*cfg,
                            double srcDx, double srcDy)
{
    TodPdf* ret;
    unsigned int mode = cfg->cfg_mode;

    DBG_OdDrvCfg(cfg,srcDx,srcDy);

  /* Get the paper geometry */
    double ppDx, ppDy;          // paper size (in pt)
    double daDx, daDy;          // drawing area size (in pt)
    double bdL,bdR,bdT,bdB;     // border thickness (in pt)
    if ( cfg->cfg_paper!=0 ) {
        ppDx = cfg->cfg_paper->wcm;
        ppDy = cfg->cfg_paper->hcm;
    } else {
        ppDx = cfg->cfg_paperDx;
        ppDy = cfg->cfg_paperDy;
    }
    ppDx = LUNIT_CmToPt( ppDx );
    ppDy = LUNIT_CmToPt( ppDy );
    if ( (mode&OD_PDFCFG_Bd1)!=0 ) {
        bdL=bdR=bdT=bdB = cfg->cfg_bd1;
    } else if ( (mode&OD_PDFCFG_Bd2)!=0 ) {
        bdL=bdR = cfg->cfg_bdH;
        bdT=bdB = cfg->cfg_bdV;
    } else if ( (mode&OD_PDFCFG_Bd4)!=0 ) {
        bdL= cfg->cfg_bdL; bdR= cfg->cfg_bdR;
        bdT= cfg->cfg_bdT; bdB= cfg->cfg_bdB;
    } else {
        bdL=bdR=bdT=bdB = 0;
    }
    bdL = LUNIT_CmToPt( bdL ); bdR = LUNIT_CmToPt( bdR );
    bdT = LUNIT_CmToPt( bdT ); bdB = LUNIT_CmToPt( bdB );

    daDx = ppDx - bdL - bdR;
    daDy = ppDy - bdB - bdT;

  /* Compute the page number and the layout geometry. */
    int linNb,colNb;
    if ( (mode&OD_PDFCFG_1Page)!=0 ) {
        linNb = 1;               colNb = 1;
    } else {
        linNb = cfg->cfg_npLine; colNb = cfg->cfg_npColumn;
        TEMP_asf_printf(linNb<1||colNb<1,
                "page matrix size is unexpected: %d%d",linNb,colNb);
    }
    double layDx = colNb * (mode&OD_PDFCFG_Land ? daDy : daDx); // in pt
    double layDy = linNb * (mode&OD_PDFCFG_Land ? daDx : daDy); // in pt

  /* Compute the scale used from source device to PDF output device
   * and the picture size in PDF unit. */
    double scale;
    double picDx = LUNIT_CmToPt( srcDx );
    double picDy = LUNIT_CmToPt( srcDy );
    if ( (mode&OD_PDFCFG_ScaleG)!=0 ) {
        scale = cfg->cfg_scale;
    } else if ( srcDx==0 || srcDy==0 ) {
        scale = 1;
    } else /* if ( (mode&OD_PDFCFG_ScaleG)==0 ) */ {
        scale = min2(layDx/picDx,layDy/picDy);
    }
    picDx = scale*picDx;
    picDy = scale*picDy;

  /* sanity check */
    if ( layDx<1 || layDy<1 ) {
        tool_errerr_printf(md,"Invalid parameters for PDF generation: %s",
                "border too large for paper");
        return NULL;
    }

  /* creation of output device */
    TboxDble box;
    Box_x (box)  = 0;
    Box_y (box)  = 0;
    Box_dx(box)  = layDx;
    Box_dy(box)  = layDy;
    ret = (TodPdf*)od_new0( &pdf_vtable, LUNIT_PtPerInch, &box );
    ret->any.msgdrv = md;

  /* initialisation of output device */
    ret->stream    = stream;
    ret->ppDx      = ppDx;  ret->ppDy   = ppDy;
    ret->ppDxDa    = daDx;  ret->ppDyDa = daDy;
    ret->bdL       = bdL;   ret->bdR    = bdR;
    ret->bdT       = bdT;   ret->bdB    = bdB;
    ret->layDx     = layDx; ret->layDy  = layDy;
    ret->picDx     = picDx; ret->picDy  = picDy;
    ret->mode      = mode;
    ret->scale     = scale;
    if ( (cfg->cfg_mode&OD_PDFCFG_Land)==0 ) {
        // portrait
        ret->landscape = 0;
    } else {
        // landscape (rotation of 90)
        ret->landscape = 1;
        ret->ppDx      = ppDy;  ret->ppDy   = ppDx;
        ret->ppDxDa    = daDy;  ret->ppDyDa = daDx;
        ret->bdL       = bdB;   ret->bdR    = bdT;
        ret->bdT       = bdL;   ret->bdB    = bdR;
    }
    ret->gravity   = cfg->cfg_gravity;
    ret->pgNb      = linNb*colNb;
    ret->pgByLine  = colNb;

#define CM(x) LUNIT_PtToCm(x)
DBG0_printf("pdf","%s","-----------\n");
DBG0_printf("pdf","scale=%.2f gravity=%d land=%d pgNb=%d pgByLine=%d\n",
        ret->scale,ret->gravity,ret->landscape,ret->pgNb,ret->pgByLine);
DBG0_printf("pdf","ppDX=bbL+ppDxDa+bdR=%f=%f=%f+%f+%f\n",
        CM(ret->ppDx),CM(ret->bdL)+CM(ret->ppDxDa)+CM(ret->bdR),
                      CM(ret->bdL),CM(ret->ppDxDa),CM(ret->bdR));
DBG0_printf("pdf","ppDY=bbT+ppDyDa+bdB=%f=%f=%f+%f+%f\n",
        CM(ret->ppDy),CM(ret->bdT)+CM(ret->ppDyDa)+CM(ret->bdB),
                      CM(ret->bdT),CM(ret->ppDyDa),CM(ret->bdB));
DBG0_printf("pdf","%s","------------------------------------------\n");

    // object numbered 0 is the "null" PDF object.
    ret->objnb  = 1;

    oddrv_initInNew( ret , cfg->cfg_outFile );

    od_pdf_addColor(ret,ret->black);
    od_pdf_addColor(ret,ret->white);
    od_pdf_addColor(ret,ret->usrMissDrawClr);
    od_pdf_addColor(ret,ret->usrMissFillClr);
    od_pdf_addColor(ret,ret->usrMissTextClr);

    ret->pageobjs  = strfmt_newLSILF(1000,5, 8);
    // reserve first item for for the page clip object
    od_pdf_addStartObjToPage(ret,0);
    ret->pagepats  = strfmt_newLSILF(1000,8, 8);
    ret->pagefonts = strfmt_newLSILF(1000,9, 8);
    ret->pageimgs  = strfmt_newLSILF(1000,9, 8);

    odpdf_gc_init (&ret->gc);

    return ret;
}

extern void    odpdf_delete(TodPdf* od)
{ od_delete( &od->any ); }

extern int     odpdf_doHeader(TodPdf* od)
{
    int status;

    od_setUsrZoom_NW(&od->any,od->scale);

    status = od_pdf_puts(od,"%PDF-1.7\n\n");
    if ( status<=0 ) return -1;

    oddrv_initInHead( od );

    return 0;
}

extern int     odpdf_doFooter(TodPdf* od)
{
    int i,status,oid;
    int oid_outline, oid_catalog, oid_pagedirs, oid_page;

    oid_outline = od_pdf_objPuts(od,
             "  << /Type /Outlines\n"
             "     /Count 0\n"
             "  >>\n"
    );
    if ( oid_outline<0 ) return -1;

    TstrEaten pageoids = odpdf_page_getOID(od,
                            oid_outline+2, &oid_pagedirs);

    //oid_pagedirs=oid_outline+2;
    oid_catalog = od_pdf_objPrintf(od,
             "  << /Type /Catalog\n"
             "     /Outlines %d 0 R\n"
             "     /Pages %d 0 R\n"
             "  >>\n"
             ,
             oid_outline,oid_pagedirs
    );
    if ( oid_catalog<0 ) return -1;

    if ( odpdf_page_addObject(od,oid_pagedirs)<0 ) return -1;

    oid = od_pdf_objPrintf(od,
             "   << /Type /Pages\n"
             "      /UserUnit  1\n"
             "      /Kids [ %s ]\n"
             "      /Count %d\n"
             "  >>\n"
             ,
             pageoids,
             od->pgNb
    );
    if ( oid<0 ) return -1;
    TEMP_asf_printf( oid!=oid_pagedirs,
        "found %d oid for pagedirs object, %d was expected",oid,oid_pagedirs);
    free( pageoids ); 

    // footer
    int xrefOffset = od->offset;
    status = od_pdf_printf(od,
        "xref\n0 %d\n"
        "0000000000 65535 f \n"
        ,od->objnb
    );
    if ( status < 0 ) return -1;
    for (i=1 ; i<od->objnb ; i+=1) {
      status = od_pdf_printf(od,"%010d %05d n \n",od->offsets[i],0);
      if ( status < 0 ) return -1;
    }
    status = od_pdf_printf(od,"\n");
    if ( status < 0 ) return -1;

    status = od_pdf_printf(od,
        "trailer\n"
        "    << /Size %d\n"
        "       /Root %d 0 R\n"
        "    >>\n"
        "startxref\n"
        "%d\n"
        "%%%%EOF\n"
        ,od->objnb, oid_catalog, xrefOffset
    );
    if ( status < 0 ) return -1;
    
    return 0;
}

/*======================================================================*/
/*= Write data in od->stream                                           =*/

extern int od_pdf_write (TodPdf*od, const void* data, int datalen)
{
    int wlen = fwrite(data,1,datalen,od->stream);
    if ( wlen<0 || wlen<datalen )
        return -1;
    od->offset += datalen;
    return wlen;
}

extern int od_pdf_puts   (TodPdf*od, CstrBrief str)
{ return od_pdf_write(od,str,strlen(str)); }

extern int od_pdf_printf (TodPdf*od, CstrBrief fmt,...)
{
    int ret;
    va_list ap;
    va_start(ap,fmt);
    ret = od_pdf_vprintf(od,fmt,ap);
    va_end(ap);
    return ret;
}

extern int od_pdf_vprintf(TodPdf*od, CstrBrief fmt, va_list ap)
{
    int   len;
    char *data = tool_vaprintf(fmt,ap);
    len = od_pdf_puts(od,data);
    free( data );
    return len;
}

/*======================================================================*/
/*= Write PDF objects into od->stream                                  =*/

extern int od_pdf_objStart  (TodPdf*od)
{
    int   len,oid = od->objnb;
    od->offsets[od->objnb++]=od->offset;

    len = fprintf(od->stream, "%d 0 obj\n",oid);
    if ( len<=0 ) return -1;
    od->offset += len;
    return oid;
}

extern int od_pdf_objEnd (TodPdf*od)
{
    int len;
    len = fprintf(od->stream, "endobj\n\n");
    if ( len<=0 ) return -1;
    od->offset += len;
    return od->objnb-1;
}

extern int od_pdf_objWrite  (TodPdf*od, const void* data, int datalen)
{
    int   len,len0,oid = od->objnb;

    len = fprintf(od->stream, "%d 0 obj\n",oid);
    if ( len<=0 ) return -1;

    len0 = fwrite(data,1,datalen,od->stream);
    if ( len0<0 ) return -1;
    len += len0;

    len0 = fprintf(od->stream,
        "endobj\n\n");
    if ( len0<=0 ) return -1;
    len += len0;

    if ( len<=0 ) return -1;
    od->offsets[od->objnb++]=od->offset;
    od->offset += len;
    return oid;
}

extern int od_pdf_objPuts(TodPdf*od, CstrBrief str)
{
    int   len,oid = od->objnb;
    len = fprintf(od->stream,
        "%d 0 obj\n"
        "%s"
        "endobj\n\n"
        ,oid,str);
    if ( len<=0 )
        return -1;
    od->offsets[od->objnb++]=od->offset;
    od->offset += len;
    return oid;
}

extern int od_pdf_objPrintf(TodPdf*od,CstrBrief fmt,...)
{
    int ret;
    va_list ap;
    va_start(ap,fmt);
    ret = od_pdf_objVPrintf(od,fmt,ap);
    va_end(ap);
    return ret;
}

extern int od_pdf_objVPrintf(TodPdf*od, CstrBrief fmt, va_list ap)
{
    int   oid;
    char *data = tool_vaprintf(fmt,ap);

    oid = od_pdf_objPuts(od,data);
    free( data );

    return oid;
}

/*======================================================================*/
/*= Write PDF stream objects into od->stream                           =*/

extern int od_pdf_streamObjWrite  (TodPdf*od,const void* data, int datalen);
extern int od_pdf_streamObjPuts   (TodPdf*od,CstrBrief str)
{
    int oid, len=strlen(str);

    oid = od_pdf_objPrintf(od,
            "    << /Length %d >>\n"
            "stream\n"
            "%s"
            "endstream\n"
            ,len,str
    );
    return oid;
}

extern int od_pdf_streamObjPrintf(TodPdf*od,CstrBrief fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    char *data = tool_vaprintf(fmt,ap);
    va_end(ap);

    int oid, datalen=strlen(data);

    oid = od_pdf_objPrintf(od,
            "    << /Length %d >>\n"
            "stream\n"
            "%s"
            "endstream\n"
            ,datalen,data
    );
    free( data );
    return oid;
}

extern int od_pdf_streamObjPrintfWithDico(TodPdf*od,CstrBrief dico,CstrBrief fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    char *data = tool_vaprintf(fmt,ap);
    va_end(ap);

    int oid, datalen=strlen(data);

    oid = od_pdf_objPrintf(od,
            "  <<\n"
            "%s"
            "    /Length %d\n"
            "  >>\n"
            "stream\n"
            "%s"
            "endstream\n"
            ,dico,datalen,data
    );
    free( data );
    return oid;
}

/*======================================================================*/

extern char* od_pdf_rgb8TOstr(char*buf, CcolorRGB*rgb8)
{ double r=rgb8->red, g=rgb8->green, b=rgb8->blue;
  sprintf(buf,"%0.3f %0.3f %0.3f",r/255.,g/255.,b/255.);
  return buf;
}

extern void od_pdf_addColor(TodPdf*od, Ccolor* c)
{
    if ( od->clrn == PDF_CLR_MAX ) {
        tool_errerr_printf(od_msgdrv(&od->any),PDF_MESS("Too many colors (max is %d)"),PDF_CLR_MAX);
        return ;
    }
    char temp[100];
    TclrPdf* clr = &od->clrs[od->clrn]; 
    clr->clr     = (Tcolor*)c;
    clr->egssave = c->egs; clr->clr->egs.uint = od->clrn;
    clr->rgbstr  = tool_strdup( od_pdf_rgb8TOstr(temp,&c->rgb) );
    od->clrn += 1;
}

/*======================================================================*/

extern TimageEGS odpdf_img_add (TodPdf* od, CstrBrief path, int oid, int dx, int dy)
{
    Tdrvimg img;
    drv_img_init(&img,path,oid,od->imgs->eleNb);
    img.di_oid  = oid;
    if ( oid>0 ) {
        // valid image
        img.di_imgegs.dx       = dx;
        img.di_imgegs.dy       = dy;
        img.di_pdfidstr = tool_aprintf("/I%03d",img.di_imgegs.egs.sint);
        strfmt_printfItem(od->pageimgs," %s %3d 0 R",img.di_pdfidstr,img.di_oid);
    }
    drvimgset_add(od->imgs,&img);
    return img.di_imgegs;
}

/*======================================================================*/

void odpdf_gc_init(TgcPdf* gc)
{
    gc->gcd     = OD_GC_UsrDraw;
    gc->tiles   = pdfTileset_new(100);
}

void odpdf_gc_free(TgcPdf* gc)
{
    int i;
    for (i=0 ; i<gc->tiles->eleNb ; i+=1 )
        color_delete( gc->tiles->eles[i].bgclr );
    pdfTileset_delete( gc->tiles );
    tool_free( gc->previous );
}

#include "outdev/style.h"

#define TPLSET pdfTile
#include "tools/set-body-template.h"

#define IsUsrDF(gcd) ((gcd)==OD_GC_UsrDraw || (gcd)==OD_GC_UsrFill)

static TstrEaten odpdf_gc_getPriv(TodPdf* od, Tod_GCdef gcd)
{
    int i;
    TgcPdf* gc = &od->gc;
    CodResources* res = od->any.resources;
    Csty_line* stl = od_style_topLine(&od->any,gcd);
    Csty_fill* stf = od_style_topFill(&od->any,gcd);
    double th = stl->stl_thickness;

    int cs = stl->stl_cap==DA_CS_Butt  ? 0 :
             stl->stl_cap==DA_CS_Round ? 1 :
             stl->stl_cap==DA_CS_Proj  ? 2 : 0;
    int js = stl->stl_join==DA_JS_Miter  ? 0 :
             stl->stl_join==DA_JS_Round  ? 1 :
             stl->stl_join==DA_JS_Bevel  ? 2 : 0;

    char dds[1000]; const float* t;
    switch (stl->stl_style) {
      case DA_LS_Dash :
        t = res->dash;
        sprintf(dds,"[%f %f] 2 d",t[0]*th,t[1]*th);
        break;
      case DA_LS_Dot :
        t = res->dot;
        sprintf(dds,"[%f %f] 2 d",t[0]*th,t[1]*th);
        cs=1;
        break;
      case DA_LS_Dash1Dot1 :
dash1dot1:
        t = res->dash1dot1;
        sprintf(dds,"[%f %f %f %f] 4 d",t[0]*th,t[1]*th,t[2]*th,t[3]*th);
        cs=1;
        break;
      case DA_LS_Dash1Dot2 :
      case DA_LS_Dash1Dot3 :
        PERM_spe_printf("%d dash/dot style not yet implemented, changed to Dash1Dot1",stl->stl_style);
        goto  dash1dot1;
      default:
        sprintf(dds,"[] 0 d");
    }

    char* draw_clr = od->clrs[stl->stl_color->egs.uint].rgbstr;

    char fill_data[1000]; double coef; TcolorRGB rgb8; char rgb8buf[100];
    switch ( stf->stf_style ) {
      case DA_FS_Solid: 
        sprintf(fill_data,"%s rg",od->clrs[stf->stf_color->egs.uint].rgbstr);
        break;
      case DA_FS_ShadTile:
        coef = od_getShadCoef(&od->any,stf->stf_shade);
        rgb8 = stf->stf_color->rgb;
        rgb8.red   += (rgb8.red  ) * coef;
        rgb8.green += (rgb8.green) * coef;
        rgb8.blue  += (rgb8.blue ) * coef;
        sprintf(fill_data,"%s rg",od_pdf_rgb8TOstr(rgb8buf,&rgb8));
        break;
      case DA_FS_LighTile:
        coef = od_getLighCoef(&od->any,stf->stf_light);
        rgb8 = stf->stf_color->rgb;
        rgb8.red   += (255-rgb8.red  ) * coef;
        rgb8.green += (255-rgb8.green) * coef;
        rgb8.blue  += (255-rgb8.blue ) * coef;
        sprintf(fill_data,"%s rg",od_pdf_rgb8TOstr(rgb8buf,&rgb8));
        break;
      case DA_FS_PattTile: {
          int pattern_oid;
          TpdfTileSet* tiles = gc->tiles;
          int idx = stf->stf_pattern;
          for (i=0 ; i<tiles->eleNb ; i+=1) {
              TpdfTile*tile = tiles->eles+i;
              if ( tile->idx==idx && color_cmp(tile->bgclr,stf->stf_color)==0) {
                  pattern_oid = tile->oid;
                  goto pattern_oid_found;
              }
          }
          Cbitmap* bmp = od_getPattBtm(&od->any,idx,1.0);
          int      dx  = bmp->dx;
          int      dy  = bmp->dy;
          char dico[1000];
          sprintf(dico,
              "    /Type /Pattern\n"
              "    /PatternType 1\n"
              "    /PaintType 1\n"
              "    /TilingType 2\n"
              "    /BBox [ 0 0 %d %d ]\n"
              "    /XStep %d\n"
              "    /YStep %d\n"
              "    /Resources << >>\n"
              "%%   /Matrix [ 0.4 0.0 0.0 0.4 0.0 0.0 ]\n"
              "    /Interpolate false\n"
              "%%   /Shading <<\n"
              "%%      /ShadingType 1\n"
              "%%      /AntiAlias true\n"
              "%%   >>\n"
              ,
              dx,dy,dx,dy
          );
          Trawdata* rdt = bmp->bits;
          Tstrfmt*  str = strfmt_newLSILF(1000,6,4);
          str->foot[0]  = '>';
          strfmt_restart(str);
          int i;
          for (i=0; i<rdt->bytesize ; i+=1) {
            Tuint c=rdt->data[i], ret=0, cmask=1, rmask=0x80;
            while ( rmask!=0 ) {
                if ( (cmask&c)!=0 ) ret |= rmask;
                cmask <<= 1;
                rmask >>= 1;
            }
            strfmt_printfItem(str," %02x",ret);
          }
          TcolorRGB bgrgb = stf->stf_color->rgb;
          pattern_oid = od_pdf_streamObjPrintfWithDico(od,dico,
              "   q\n"
              "   %d 0 0 %d 0 0 cm\n"
              "   BI\n"
              "%%   /IM true\n"
              "%%   /CS/RGB\n"
              "    /CS [/Indexed /DeviceRGB 1 <%06x %06x>]\n"
              "    /W %d\n"
              "    /H %d\n"
              "    /BPC 1\n"
              "%%   /D[1 0]\n"
              "    /F/ASCIIHexDecode\n"
              "  ID\n"
              "%s"
              "\n  EI\n"
              "  Q\n"
              ,
              dx,dy,
              (bgrgb.red*256+bgrgb.green)*256+bgrgb.blue,
              0, //(fgrgb.red*256+fgrgb.green)*256+fgrgb.blue,
              dx,dy,
              str->str
          );
          strfmt_delete(str);
          TpdfTile new_tile;
          new_tile.idx   = idx;
          new_tile.bgclr = color_newLazy(stf->stf_color);
          new_tile.oid   = pattern_oid;
          pdfTileset_add(tiles,&new_tile);
          od_pdf_addPatternToPage(od,pattern_oid);
          
pattern_oid_found:
          sprintf(fill_data,"/Pattern cs /P%03d scn",pattern_oid);
        }
        break;
      default:
        fill_data[0]=0;
        break;
    }
    
    TstrEaten gc_str = tool_aprintf(
            "  %s" // "Q" or "q" or empty
            " %.2f w %d J %d j" // thickness, cap and join style
            " %s"    // dash/dot specification "[d d d ...] n d"
            " %s RG" // fg color:   "r g b RG"
            " %s"    // fill color or pattern: "r g b rg" or "/Pattern cs /Pnnn scn"
            ,
              IsUsrDF(gc->gcd) && ! IsUsrDF(gcd) ? "q" :
            ! IsUsrDF(gc->gcd) &&   IsUsrDF(gcd) ? "Q" : "",
            th, cs, js,
            dds,draw_clr,fill_data
    );
    gc->gcd = gcd;
    return gc_str;
}

TstrEaten odpdf_gc_get0(TodPdf* od, Tod_GCdef gcd)
{
    TstrEaten ret = odpdf_gc_getPriv(od,gcd);
    TgcPdf* gc = &od->gc;
    if ( IsUsrDF(gcd) ) {
        tool_strchgEaten(&gc->previous,tool_strdup(ret) );
    }
    return ret;
}

TstrEaten odpdf_gc_get(TodPdf* od, Tod_GCdef gcd)
{
    TstrEaten ret = odpdf_gc_getPriv(od,gcd);
    TgcPdf* gc = &od->gc;
    if ( IsUsrDF(gcd) ) {
        if ( gc->previous && strcmp(ret,gc->previous)==0 ) {
            tool_free( ret );
            return NULL;
        }
        tool_strchgEaten(&gc->previous,tool_strdup(ret) );
    }
    return ret;
}

/*======================================================================*/
