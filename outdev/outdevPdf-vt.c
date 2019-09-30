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
 *      $Id: outdevPdf-vt.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The vtable for TodPdf output device.                               =*/
/*=                                                                    =*/
/*= This file is only included by the outdevPdf.c so all definitions   =*/
/*= are static.                                                        =*/
/*======================================================================*/

#include "outdev/outdev-vt.h"
#include "outdev/resources.h"

#include "tools/charset.h"

#define _VCONV_Y_D2D(od,_y)    ( od->picDy-(_y))
#define _VCONV_Y_Ds2Win(od,_y) ( _VCONV_Y_D2D(od,VCONV_Y_Ds2Win(od,_y)))

/*======================================================================*/
/*= Constructors & destructor                                          =*/

static void vt_pdf_destructor(void* dev)
{
    int i;
    TodPdf* od = dev;

    for (i=0 ; i<od->clrn ; i++) {
        od->clrs[i].clr->egs = od->clrs[i].egssave;
        free( od->clrs[i].rgbstr );
    }

#if 0 // NO-PXM
    for (i=0 ; i<od->pxmn ; i++) {
        odpdf_freePixmap( od->pxms+i );
    }
#endif

    for (i=0 ; i<od->fontn ; i++) {
        odpdf_freeFont( od->fonts+i );
    }

    strfmt_delete ( od->pageobjs );
    strfmt_delete ( od->pagepats );
    strfmt_delete ( od->pagefonts);
    strfmt_delete ( od->pageimgs);

    oddrv_free( dev );
    odpdf_gc_free (&od->gc );
}

static void vt_pdf_clear(const void*dev) { }

/*======================================================================*/
/*= pixmap handler                                                     =*/

static TpixmapEGS vt_pdf_pxm_newFromBtm(void*  dev, Cbitmap*btm,
                                        Ccolor*fgc, Ccolor* bgc)
{
#if 0 // NO-PXM
    TpixmapEGS ret;
    ret.uint = odpdf_addPixmap(dev,btm,fgc,bgc);
    return ret;
#else
    return PXMEGS_none;
#endif
}

static void vt_pdf_pxm_delete(void*dev, TpixmapEGS pxm)
{ /* done in destructor */ }

#include "outdevPdf-image.c"

static TimageEGS  vt_pdf_img_load  (void*dev, CstrBrief path, CstrBrief opath)
{
    TodPdf* od = dev;
//printf("%s:0: path=%s\n",__func__,path);
    TimageEGS ret=IMGEGS_none;

    // nothing to do if the image is already loaded
    Tdrvimg* img = drv_imgs_getByPath(od->imgs,path);
    if ( img!=0 ) 
        return img->di_imgegs;

#if ! ( (defined(LIBPNG_AVAILABLE)  && LIBPNG_AVAILABLE==1) ||\
        (defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1) )
        return ret;
#else
    // try to load path image.
    int oid, dx, dy;
    FILE* stream = fopen(path,"r");
    if ( stream==0 )
        return ret;

    // try jpeg format
#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
    rewind(stream);
    if ( pdf_img_obj_jpeg(od,stream,&oid, &dx, &dy)==0 ) 
        goto found;
#endif
#if defined(LIBPNG_AVAILABLE) && LIBPNG_AVAILABLE==1
    // try png format
    rewind(stream);
    if ( pdf_img_obj_png(od,stream,&oid, &dx, &dy)==0 )
        goto found;
#endif

    fclose(stream);
    // try to convert file into JPEG (firstly) or PNG format
#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
    stream = vt_pdf_img_convert(path,"jpeg");
#else //  defined(LIBPNG_AVAILABLE) && LIBPNG_AVAILABLE==1
    stream = vt_pdf_img_convert(path,"png");
#endif
    if (stream==0) 
        return ret;
    rewind(stream);
#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
    int status = pdf_img_obj_jpeg(od,stream,&oid, &dx, &dy);
#else //  defined(LIBPNG_AVAILABLE) && LIBPNG_AVAILABLE==1
    int status = pdf_img_obj_png(od,stream,&oid, &dx, &dy);
#endif
    if (status!=0 )
        goto end;

found:
printf("%s: path=%s oid=%d dx=%d dy=%d\n",__func__,path,oid,dx,dy);
    ret = odpdf_img_add(od,path,oid,dx,dy);
end:
    fclose(stream);
    return ret;
#endif
}

static void vt_pdf_img_draw(void*dev, Tod_GCdef gcd, CboxDble* srcbox,
                            TimageEGS img, int h, int v, int r)
{
    TodPdf* od = dev;
    Tdrvimg* imgpdf = od->imgs->eles+img.egs.sint;
    int oid_draw;
    double x   = VCONV_X_Ds2Win(od,Box_x(*srcbox));
    double y   =_VCONV_Y_Ds2Win(od,Box_y(*srcbox)+Box_dy(*srcbox));
    double dx = VCONV_D_S2D(od,Box_dx(*srcbox));
    double dy = VCONV_D_S2D(od,Box_dy(*srcbox));

    double f00=0,f01=0,f02=0,f10=0,f11=0,f12=0;
    char* rotmat;
    if ( r==0 ) {
        rotmat = "1  0  0  1   0   0 cm";
        if      ( h &&!v ) { f00=-dx; f11=+dy; f02=x+dx; f12=y   ; }
        else if (!h && v ) { f00=+dx; f11=-dy; f02=x   ; f12=y+dy; }
        else if ( h && v ) { f00=-dx; f11=-dy; f02=x+dx; f12=y+dy; }
        else               { f00=+dx; f11=+dy; f02=x   ; f12=y   ; }
    } else if ( r==1 ) {
        rotmat = "0 -1  1  0   0   0 cm";
        if      ( h &&!v ) { f00=-dy; f11=+dx; f02=-y   ; f12=+x   ; }
        else if (!h && v ) { f00=+dy; f11=-dx; f02=-y-dy; f12=+x+dx; }
        else if ( h && v ) { f00=-dy; f11=-dx; f02=-y   ; f12=+x+dx; }
        else               { f00=+dy; f11=+dx; f02=-y-dy; f12=+x   ; }
    } else {
        PERM_spf_printf("unexpected image operation: h=%d v=%d rot=%d.\n",h,v,r);
    }
    oid_draw = od_pdf_streamObjPrintf(od,
            "  q %s %6.3f %6.3f %6.3f %6.3f %+5.1f %+5.1f cm %s Do Q\n",
            rotmat,
            f00,f01,f10,f11, f02,f12,
            imgpdf->di_pdfidstr
    );
    od_pdf_addObjToPage(od,&oid_draw);
}

/*======================================================================*/
/*= GC settings                                                        =*/

static void vt_pdf_setGCso(void*dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TjoinStyle js,
                TcapStyle  cs, TcolorEGS fg)
{ }

static void vt_pdf_setGCdd(void*dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TcolorEGS ddh)
{ }

static void vt_pdf_setGCfillColorBrightness(void*dev, Tod_GCdef gcd, TcolorEGS c, double coef)
{ }

static void vt_pdf_setGCfillColor(void*dev, Tod_GCdef gcd, TcolorEGS c)
{ }

Tuint byte_bitswap(Tuint c)
{
    Tuint ret=0;
    Tuint cmask=1;
    Tuint rmask=0x80;

    while ( rmask!=0 ) {
        if ( (cmask&c)!=0 ) ret |= rmask;
        cmask <<= 1;
        rmask >>= 1;
    }
    return ret;
}

static void vt_pdf_setGCfillTile (void*dev, Tod_GCdef gcd, TpixmapEGS _pxm)
{ }

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

static void vt_pdf_draw_pointXY(const void*dev, Tod_GCdef gcd,
                                double x, double y)
{ PERM_spe_NYI(0); }

static void vt_pdf_draw_pointT(const void*dev, Tod_GCdef gcd,
                               Cdble2* pts, int ptNb)
{ PERM_spe_NYI(0); }

static void vt_pdf_draw_lineXY(const void*dev, Tod_GCdef gcd,
                    double _x1, double _y1,  double _x2, double _y2)
{ 
    TodPdf* od = (TodPdf*)dev;
    int oid,oid_draw,oid_obj;

    double x1 =  VCONV_X_Ds2Win(od,_x1);
    double y1 = _VCONV_Y_Ds2Win(od,_y1);
    double x2 =  VCONV_X_Ds2Win(od,_x2);
    double y2 = _VCONV_Y_Ds2Win(od,_y2);
//printf("x,y = %f %f --> %f %f : src/dx/dy=%f/%f h=%f dps=%f des/dx/dy=%f/%f\n", _x1,_y1,x1,y1,
//        Box_dx(od->any.boxSrc), Box_dy(od->any.boxSrc),
//        od->any.homothetySrc2Des, od->any.desPerSrc,
//        Box_dx(od->any.boxDes), Box_dy(od->any.boxDes)
//        );
    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    oid_draw = od_pdf_streamObjPrintf(od,
            "  %s%s"
            "%f %f m "
            "%f %f l "
            "S "
            "\n"
            ,
            gcdef ? gcdef : "", gcdef ? " " : "",
            x1,y1,x2,y2
    );
    tool_free(gcdef);
    od_pdf_addObjToPage(od,&oid_draw);
}

static Tstrfmt* vt_pdf_doMLINE(TodPdf*od, int sh, Cdble2* pts, int ptNb)
{
    int i;
    Tstrfmt* str = strfmt_newLSILF(1000,sh,5);

    Tdble2 pt = pts[0];
    double     x  =  VCONV_X_Ds2Win(od,pt.x);
    double     y  = _VCONV_Y_Ds2Win(od,pt.y);
    strfmt_printfItem(str," %f %f m",x,y);

    for (i=1 ; i<ptNb ; i+=1) {
        pt = pts[i];
        x  =  VCONV_X_Ds2Win(od,pt.x);
        y  = _VCONV_Y_Ds2Win(od,pt.y);
        strfmt_printfItem(str," %f %f l",x,y);
    }

    return str;
}

static void vt_pdf_draw_mlineT(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb)
{
    TodPdf* od = (TodPdf*)dev;
    int oid,oid_draw,oid_obj;

    Tstrfmt* mline = vt_pdf_doMLINE(od,2,pts,ptNb);

    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    if ( gcdef==0 )
        oid_draw = od_pdf_streamObjPrintf(od,
                "%s"
                "  S\n"
                ,
                mline->str
        );
    else
        oid_draw = od_pdf_streamObjPrintf(od,
                "  %s\n"
                "%s"
                "  S\n"
                ,
                gcdef,
                mline->str
        );
    strfmt_delete(mline);
    tool_free(gcdef);
    od_pdf_addObjToPage(od,&oid_draw);
}

static void vt_pdf_fill_mlineT(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb)
{
    TodPdf* od = (TodPdf*)dev;

    int oid_draw;

    Tstrfmt* mline = vt_pdf_doMLINE(od,2,pts,ptNb);

    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    if ( gcdef==0 )
        oid_draw = od_pdf_streamObjPrintf(od,
                "%s"
                "  f\n"
                ,
                mline->str
        );
    else
        oid_draw = od_pdf_streamObjPrintf(od,
                "  %s\n"
                "%s"
                "  f\n"
                ,
                gcdef,
                mline->str
        );
    strfmt_delete(mline);
    tool_free(gcdef);
    od_pdf_addObjToPage(od,&oid_draw);
}

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

#define STV static void

STV vt_pdf_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                              double _x,    double _y,
                              double width, double height)
{
    TodPdf* od = (TodPdf*)dev;
    int oid_draw;
    double x  = VCONV_X_Ds2Win(od,_x);
    double y  =_VCONV_Y_Ds2Win(od,_y+height);
    double wd = VCONV_D_Ds2Win(od,width);
    double ht = VCONV_D_Ds2Win(od,height);

    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    oid_draw = od_pdf_streamObjPrintf(od,
            "  %s%s%f %f %f %f re S\n",
            gcdef ? gcdef : "", gcdef ? " " : "",
            x,y,wd,ht
    );
    tool_free(gcdef);
    od_pdf_addObjToPage(od,&oid_draw);
}

STV vt_pdf_fill_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                              double _x,     double _y,
                              double width, double height)
{
    TodPdf* od = (TodPdf*)dev;
    int oid_draw;
    double x  = VCONV_X_Ds2Win(od,_x);
    double y  =_VCONV_Y_Ds2Win(od,_y+height);
    double wd = VCONV_D_Ds2Win(od,width);
    double ht = VCONV_D_Ds2Win(od,height);

    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    oid_draw = od_pdf_streamObjPrintf(od,
            "  %s%s%f %f %f %f re f\n",
            gcdef ? gcdef : "", gcdef ? " " : "",
            x,y,wd,ht
    );
    tool_free(gcdef);
    od_pdf_addObjToPage(od,&oid_draw);
}

/*======================================================================*/
/*= open and closed spline curves                                      =*/

static Tstrfmt* vt_pdf_doBEZIER(TodPdf*od, int sh, Cdble2* pts, int ptNb)
{
    int i;
    Tstrfmt* str = strfmt_newLSILF(1000,sh,5);

    Tdble2 pt = pts[0];
    double     x  =  VCONV_X_Ds2Win(od,pt.x);
    double     y  = _VCONV_Y_Ds2Win(od,pt.y);
    strfmt_printfItem(str," %f %f m",x,y);

    for (i=1 ; i<ptNb ; i+=1) {
        pt = pts[i];
        x  =  VCONV_X_Ds2Win(od,pt.x);
        y  = _VCONV_Y_Ds2Win(od,pt.y);
        strfmt_printfItem(str," %f %f c",x,y);
    }

    return str;
}


static void vt_pdf_draw_bezier(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb)
{
    TodPdf* od = (TodPdf*)dev;
    int oid,oid_draw,oid_obj;

    Tstrfmt* mline = vt_pdf_doBEZIER(od,2,pts,ptNb);

    TstrEaten gcdef = odpdf_gc_get(od,gcd);
    if ( gcdef )
        oid_draw = od_pdf_streamObjPrintf(od,
                "  %s\n"
                "%s"
                "  S\n"
                ,
                gcdef, mline->str
        );
    else
        oid_draw = od_pdf_streamObjPrintf(od,
                "%s"
                "  S\n"
                ,
                mline->str
        );
    strfmt_delete(mline);
    tool_free(gcdef);

    od_pdf_addObjToPage(od,&oid_draw);
}

/*======================================================================*/
/*= String handler.                                                    =*/

static char* families[] = {
    "times", "Times-Roman",
    "avantgarde", "Avantgarde",
    "itc zapf dingbats", "ZapfDingbats",
    NULL };

static char* attributes[] = {
    "medium",  "Medium",
    "regular", "Regular",
    "italic",  "Italic",
    "bold",    "Bold",
    "oblique", "Oblique",
//    "demi",    "demi",
    "light",   "Light",
//    "narrow",  "narrow",
    NULL };

TfontEGS vt_pdf_fontLoadPts (void*dev, int fi, double pts, double*ratio)
{
    TodPdf* od = dev;

    TfontEGS ret;
    memset(&ret,0,sizeof(ret));
    ret.uint = fi;
    *ratio   = 1;

    TfontPdf* fpdf = od->fonts+fi;

    if ( fpdf->oid==0 ) {
        int i;
        // get PDF family
        const char* family = fpdf->font->family;
        for ( i=0 ; families[i]!=0 ; i+=2) {
            if (strcmp(family,families[i])==0 ) {
                family = families[i+1];
                break;
            }
        }
        // get fonts attribute
        char att[1000],*p=att;
        for ( i=0 ; attributes[i]!=0 ; i+=2) {
            if (strstr(fpdf->font->style,attributes[i])!=0 ) {
                p += sprintf(p,"-%s",attributes[i+1]);
            }
        }
        *p=0;
#if 0
        fdoid = od_pdf_objPrintf(od,
                "  <<\n"
                "    /Type        /FontDescriptor\n"
                "    /FontName    %s%s\n"
                "    /FontFamily  %s\n"
                "%%o /FontStretch %s\n" // UltraCondensed , ExtraCondensed , Condensed , SemiCondensed , Normal , SemiExpanded , Expanded , ExtraExpanded or UltraExpanded . 
                "%%o /FontWeight  %s\n" // 100, 200, 300,  400,  500, 600, 700, 800, 900
                                        //                normal           bold
                "    /Flags       %d\n"
                "    /FontBox     [ %f %f %f %f ]\n"  // inked BB of all glyphs
                "    /ItalicAnge  %f\n"  // in degree (negative for standard italic font)
                "    /Ascent      %f\n"
                "    /Descent     %f\n"
                "%%o /Leading     %f\n" // default 0
                "    /CapHeight   %f\n"
                "%%o /XHeight     %f\n" // height of x char
                "    /StemV       %f\n" // thickness of vertical segment
                "%%o /StemH       %f\n" // thickness of horizontal segment
                "%%o /AvgWidth    %f   /MaxWidth %f /MissingWidth %f\n" 
                "%%o /CharSet     ??\n" 
                ,
                family,att,
                family
        );
#endif
        char* Encoding;
        if ( strcmp(family,"ZapfDingbats")==0 ) 
            Encoding = "%";
        else
            Encoding = " ";

        fpdf->oid = od_pdf_objPrintf(od,
                "  <<\n"
                "    /Type /Font\n"
                "    /Subtype /Type1\n"
                "    /BaseFont  /%s%s\n"
                "%%  /FirstChar 32                                  %%\n"
                "%%  /LastCher  1023                                %%\n"
                "%%  /Widths [                                      %%\n"
                "%%  ]                                              %%\n"
                "%%  /FontDescriptor nnnn 0 R                       %%\n"
//              "%%  /Encoding  names or dictionary                 %%\n"
                "%s  /Encoding  /WinAnsiEncoding                    %%\n"
                "%%  /ToUnicode stream                              %%\n"
                "  >>\n"
                , family,att
                , Encoding
        );
        fpdf->pdfidstr = tool_aprintf("/F%03d",fpdf->oid);
        
        strfmt_printfItem(od->pagefonts," %s %3d 0 R",fpdf->pdfidstr,fpdf->oid);
    }
    return ret;
}

void vt_pdf_setGCstr(void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    TodPdf* od = dev;

    od->GCstrFontIdx  = font->fo_egs.uint;
    od->GCstrFontSz   = font->fo_size;
    od->GCstrColorIdx = fg->egs.uint;
}

/*======================================================================*/
/*= Drawing strings                                                    =*/

void vt_pdf_drawWL_Pix(void* dev, Tod_GCdef gcd, double x, double y, CstrBrief text)
{
    int oid;
    TodPdf* od = dev;
    TclrPdf*  cpdf = &od->clrs [od->GCstrColorIdx];
    TfontPdf* fpdf = &od->fonts[od->GCstrFontIdx];
    CfontDef* fdef = fpdf->font;
    char*     pdftext;

    if ( strstr(fdef->family,"dingbats")!=0 )
        pdftext = cs_conv_Utf8toZdb8(text);
    else
        pdftext = cs_conv_Utf8toCp1252(text);

    oid = od_pdf_streamObjPrintf(od,
        "  BT\n"
        "    %s rg\n"
        "    %s %f Tf\n"
        "    %d %d Td\n"
        "    (%s)  Tj\n"
        "  ET\n"
        ,
        cpdf->rgbstr,
        fpdf->pdfidstr, od->GCstrFontSz,
        (int)(round(x)), (int)(round(_VCONV_Y_D2D(od,y))),
        pdftext
    );

    od_pdf_addObjToPage(od, &oid);
    free(pdftext);
}

void vt_pdf_drawWLr_Pix(void* dev, Tod_GCdef gcd, double x, double y,
                        CstrBrief text, double a)
{
    int oid;
    TodPdf* od = dev;
    TclrPdf*  cpdf = &od->clrs [od->GCstrColorIdx];
    TfontPdf* fpdf = &od->fonts[od->GCstrFontIdx];

    oid = od_pdf_streamObjPrintf(od,
        "  BT\n"
        "    %f %f %f %f %d %d Tm\n"
        "%%   %d %d Td\n"
        "    %s rg\n"
        "    %s %f Tf\n"
        "    (%s)  Tj\n"
        "  ET\n"
        ,
         cos(a),sin(a),
        -sin(a),cos(a),
        (int)(round(x)), (int)(round(_VCONV_Y_D2D(od,y))),
        (int)(round(x)), (int)(round(_VCONV_Y_D2D(od,y))),
        cpdf->rgbstr,
        fpdf->pdfidstr, od->GCstrFontSz,
        text
    );

    od_pdf_addObjToPage(od, &oid);
}

/*======================================================================*/
/*= The virtual table of PDF output device.                            =*/

static Toutdev_vt pdf_vtable = {
    .topVT  = {
        .ident      = 0x00504446,          // acscii of PDF
        .parentVT   = &outdev_vtable, 
        .byteSize   = sizeof( TodPdf ),
        .destructor = vt_pdf_destructor,
        .clear      = vt_pdf_clear,
    },
    .colorVT  = {
        .black = oddrv_vt_black,
        .white = oddrv_vt_white,
    },
    .pxmVT  = {
      //.pxm_new        = vt_pdf_pxm_new,
        .pxm_newFromBtm = vt_pdf_pxm_newFromBtm,
      //.pxm_newFromImg = vt_pdf_pxm_newFromImg,
        .pxm_delete     = vt_pdf_pxm_delete,
        .img_prefFormat = -1, // IMGFMT_JPG,
      //.pxm_draw       = vt_pdf_pxm_draw,
        .img_load       = vt_pdf_img_load,
      //.img_free       = vt_pdf_img_free,
      //.img_transform  = vt_pdf_img_transform,
        .img_draw       = vt_pdf_img_draw,
    },
    .ldafVT   = {
        .setGCso      = vt_pdf_setGCso,
        .setGCdd      = vt_pdf_setGCdd,
        .setGCfillColor           = vt_pdf_setGCfillColor,
        .setGCfillColorBrightness = vt_pdf_setGCfillColorBrightness,
        .setGCfillTile            = vt_pdf_setGCfillTile,
      //.setGCfillTileZ           = vt_pdf_setGCfillTileZ,
        .draw_pointXY = vt_pdf_draw_pointXY,
        .draw_pointT  = vt_pdf_draw_pointT,
        .draw_lineXY  = vt_pdf_draw_lineXY,
        .draw_mlineT  = vt_pdf_draw_mlineT,
        .fill_mlineT  = vt_pdf_fill_mlineT,
        .draw_rectangleXYWH  = vt_pdf_draw_rectangleXYWH,
        .fill_rectangleXYWH  = vt_pdf_fill_rectangleXYWH,
        .draw_bezier  = vt_pdf_draw_bezier,
      //.fill_bezier  = vt_pdf_fill_bezier,
    },
    .strVT   = {
      //.initGC       = vt_pdf_initGCstr,
        .fontLoadPts  = vt_pdf_fontLoadPts,
      //.fontLoadPix  = vt_pdf_fontLoadPix,
      //.fontUnload   = vt_pdf_fontUnload ,
        .setGC        = vt_pdf_setGCstr ,
      //.extentsPts   = vt_pdf_extentsPts ,
      //.extentsPix   = vt_pdf_extentsPix ,
      //.lineInfoPix  = vt_pdf_lineInfoPix ,
        .drawWL_Pix   = vt_pdf_drawWL_Pix,
      //.drawNW_Pix   = vt_pdf_drawNW_Pix,
        .drawWLr_Pix  = vt_pdf_drawWLr_Pix,
    },

};

/*======================================================================*/
