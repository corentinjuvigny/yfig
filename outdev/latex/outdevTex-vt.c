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
 *      $Id: outdevTex-vt.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/latex/outdevTex-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The vtable for TodTex output device.                               =*/
/*=                                                                    =*/
/*= This file is only included by the outdevPdf.c so all definitions   =*/
/*= are static.                                                        =*/
/*======================================================================*/

#include "outdevTex-priv.h"
#include "outdev/outdev-vt.h"
#include "outdev/resources.h"

#include "tools/file.h"
#include "tools/lenunit.h"
#include "tools/imgfmt.h"

#define STV static void

#define _VCONV_Y_D2D(od,_y)    ( od->daDy-(_y)+od->border)
#define _VCONV_X_D2D(od,_x)    (          (_x)+od->border)
#define _VCONV_Y_Ds2Win(od,_y) ( _VCONV_Y_D2D(od,VCONV_Y_Ds2Win(od,_y)))
#define _VCONV_X_Ds2Win(od,_x) ( _VCONV_X_D2D(od,VCONV_X_Ds2Win(od,_x)))

/*======================================================================*/
/*= Constructors & destructor                                          =*/

static void vt_tex_destructor(void* dev) { oddrv_free(dev); }
static void vt_tex_clear(const void*dev) { }

/*======================================================================*/
/*= pixmap handler                                                     =*/

static TpixmapEGS vt_tex_pxm_newFromBtm(void*  dev, Cbitmap*btm, Ccolor*fgc, Ccolor* bgc) { return PXMEGS_none; }
static void       vt_tex_pxm_delete(void*dev, TpixmapEGS pxm)                             { /* done in destructor */ }
static TimageEGS  vt_tex_img_load  (void*dev, CstrBrief path, CstrBrief opath)
{
    TodTex*   od  = (TodTex*)dev;
    TimageEGS ret = IMGEGS_none;

    // nothing to do if the image is already loaded
    Tdrvimg* img  = drv_imgs_getByPath(od->imgs,path);
    if ( img!=0 ) 
        return img->di_imgegs;

    if ( opath!=0 ) {
        // path is a PNG conversion of opath (see img_prefFormat in vtable) 
        img = drv_imgs_getByPath(od->imgs,opath);
        if ( img!=0 && img->di_path2) 
            return img->di_imgegs;
        if ( tool_fileOpenROCheck(path)==1 ) drv_img_update2(img,path,-2        ,-1); // -1 not used
        else if ( drv_img_isPNG(path) )      drv_img_update2(img,path,IMGFMT_PNG,drv_imgs_getIdxOfEle(od->imgs,img));
        else                                 drv_img_update2(img,path,-1        ,-1);
        return img->di_imgegs;
    } else {
        if ( tool_fileOpenROCheck(path)==1 ) {
            return drv_imgs_add(od->imgs,path,-2);
        } else if ( drv_img_isJPEG(path) )  {
            return drv_imgs_add(od->imgs,path,IMGFMT_JPG);
        } else if ( drv_img_isPNG(path) )  {
            return drv_imgs_add(od->imgs,path,IMGFMT_PNG);
        } else if ( drv_img_isPDF(path) )  {
            return drv_imgs_add(od->imgs,path,IMGFMT_PDF);
        } else {
            return drv_imgs_add(od->imgs,path,-1);
        }
    }
#if 0
    if ( img==0 ) {
        drv_imgs_add(od->imgs,path,-1);
        img = drv_imgs_getByPath(od->imgs,path);
        img->di_imgegs.egs = any_initBySInt( od->imgs->eleNb-1 );
    }
    return img->di_imgegs;
#endif
}
static void       vt_tex_img_draw  (void*dev, Tod_GCdef gcd, CboxDble* srcbox,
                                    TimageEGS img, int h, int v, int r)
{
    TodTex*   od      = (TodTex*)dev;
    Tdrvimg*  drvimg  = od->imgs->eles+img.egs.sint;
    
    Cstr imgfile = drv_img_updateRP(drvimg,dev);

//TEMP_spe_printf("D0: imgfile=%s ; idx=%d ; di_path/2=%s -- %s --",imgfile, img.egs.sint, drvimg->di_path, drvimg->di_path2);
    double x  = _VCONV_X_Ds2Win(od,Box_x (*srcbox));
    double y  = _VCONV_Y_Ds2Win(od,Box_y (*srcbox));
    double dx =  VCONV_D_Ds2Win(od,Box_dx(*srcbox));
    double dy =  VCONV_D_Ds2Win(od,Box_dy(*srcbox));

    TEMP_asf_printf(r!=0 && r!=1 && r!=3,"r=%d is an unexpected rotation",r);

    char flipbeg[200];
    char flipend[200];
    if ( h || v ) {
        sprintf(flipbeg,"\\scalebox{%2d}[%2d]{",h?-1:1,v?-1:1);
        flipend[0]='}';
        flipend[1]=0;
    } else {
        flipbeg[0]=0;
        flipend[0]=0;
    }

    char rotate[200]; rotate[0]=0;
    if ( r==1 || r==3 ) {
        if ( h==1 && v==1 )
            sprintf(rotate,",origin=0cm 0cm,angle=%3d",-r*90);
        else if ( h==0 && v==0 )
            sprintf(rotate,",origin=0cm 0cm,angle=%3d",-r*90);
        else 
            sprintf(rotate,",origin=0cm 0cm,angle=%3d", r*90);
        double tmp=dx; dx=dy; dy=tmp;
    }

    fprintf(od->stream,"  \\draw (%6.2f,%6.2f) "
                       "node[anchor=north west,inner sep=0cm]"
                       "{%s\\includegraphics[width=%6.2fcm,height=%6.2fcm%s]{%s}%s} ;\n",
            x,y,flipbeg,dx,dy,rotate,imgfile,flipend);
}

/*======================================================================*/
/*= GC settings                                                        =*/

  STV vt_tex_setGCso(void*dev, Tod_GCdef gcd, double th, TlineStyle ls, TjoinStyle js, TcapStyle  cs, TcolorEGS fg) { }
  STV vt_tex_setGCdd(void*dev, Tod_GCdef gcd, double th, TlineStyle ls, TcolorEGS ddh) { }
  STV vt_tex_setGCfillColor(void*dev, Tod_GCdef gcd, TcolorEGS c) { }
//STV vt_tex_setGCfillColorBrightness(void*dev, Tod_GCdef gcd, TcolorEGS c, double coef) { }
  STV vt_tex_setGCfillTile (void*dev, Tod_GCdef gcd, TpixmapEGS _pxm) { }

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

static void vt_tex_draw_pointXY(const void*dev, Tod_GCdef gcd, double x, double y) { PERM_spe_NYI(0); }
static void vt_tex_draw_pointT(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb) { PERM_spe_NYI(0); }

static void vt_tex_draw_lineXY(const void*dev, Tod_GCdef gcd,
                    double _x1, double _y1,  double _x2, double _y2)
{
    char      clr[200];
    TodTex*   od    = (TodTex*)dev;
    TstrEaten gcdef = odtex_gc_get(clr,od,gcd);

    double x1 = _VCONV_X_Ds2Win(od,_x1);
    double y1 = _VCONV_Y_Ds2Win(od,_y1);
    double x2 = _VCONV_X_Ds2Win(od,_x2);
    double y2 = _VCONV_Y_Ds2Win(od,_y2);
    fprintf(od->stream,"  %s\\draw[%s] (%f,%f) -- (%f,%f);\n",clr,gcdef,x1,y1,x2,y2);
    tool_free(gcdef);
}

static Tstrfmt* vt_tex_doMLINE(TodTex*od, int sh, Cdble2* pts, int ptNb)
{
    int i;
    Tstrfmt* str = strfmt_newLSILF(1000,sh,5);

    Tdble2 pt = pts[0];
    double x  = _VCONV_X_Ds2Win(od,pt.x);
    double y  = _VCONV_Y_Ds2Win(od,pt.y);
    strfmt_printfItem(str,"    (%6.2f,%6.2f)",x,y);

    for (i=1 ; i<ptNb ; i+=1) {
        pt = pts[i];
        x  = _VCONV_X_Ds2Win(od,pt.x);
        y  = _VCONV_Y_Ds2Win(od,pt.y);
        strfmt_printfItem(str," -- (%6.2f,%6.2f)",x,y);
    }

    return str;
}

static void vt_tex_draw_mlineT(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb)
{
    char      clr[200];
    TodTex*   od    = (TodTex*)dev;
    TstrEaten gcdef = odtex_gc_get(clr,od,gcd);
    Tstrfmt*  mline = vt_tex_doMLINE(od,4,pts,ptNb);

    fprintf(od->stream,"  %s\\draw[%s]\n",clr,gcdef);
    fprintf(od->stream,"%s",mline->str);
    fprintf(od->stream,"  ;\n");

    strfmt_delete(mline);
    tool_free(gcdef);
}

static void vt_tex_fill_mlineT(const void*dev, Tod_GCdef gcd,
                    Cdble2* pts, int ptNb)
{
    char      clr[200];
    TodTex*   od    = (TodTex*)dev;
    TstrEaten gcdef = odtex_gc_get(clr,od,gcd);
    Tstrfmt*  mline = vt_tex_doMLINE(od,4,pts,ptNb);

    fprintf(od->stream,"  %s\\fill[%s]\n",clr,gcdef);
    fprintf(od->stream,"%s",mline->str);
    fprintf(od->stream,"  ;\n");

    strfmt_delete(mline);
    tool_free(gcdef);
}

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

STV vt_tex_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                              double _x,    double _y,
                              double width, double height)
{
    char    clr[200];
    TodTex* od =  (TodTex*)dev;
    double  x  = _VCONV_X_Ds2Win(od,_x);
    double  y  = _VCONV_Y_Ds2Win(od,_y+height);
    double  wd =  VCONV_D_Ds2Win(od,width);
    double  ht =  VCONV_D_Ds2Win(od,height);

    TstrEaten gcdef = odtex_gc_get(clr,od,gcd);
    fprintf(od->stream,"  %s\\draw[%s] (%.2f,%.2f) rectangle (%.2f,%.2f) ;\n",
            clr,gcdef,x,y, x+wd, y+ht);
    tool_free(gcdef);
}

STV vt_tex_fill_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                              double _x,     double _y,
                              double width, double height)
{
    char    clr[200];
    TodTex* od =  (TodTex*)dev;
    double  x  = _VCONV_X_Ds2Win(od,_x);
    double  y  = _VCONV_Y_Ds2Win(od,_y+height);
    double  wd =  VCONV_D_Ds2Win(od,width);
    double  ht =  VCONV_D_Ds2Win(od,height);

    TstrEaten gcdef = odtex_gc_get(clr,od,gcd);
    fprintf(od->stream,"  %s\\fill[%s] (%.2f,%.2f) rectangle (%.2f,%.2f) ;\n",
            clr,gcdef,x,y, x+wd, y+ht);
    tool_free(gcdef);
}

/*======================================================================*/
/*= open and closed spline curves                                      =*/

//static void vt_tex_draw_bezier(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb)

/*======================================================================*/
/*= String handler.                                                    =*/

static char* families[] = {
    "times"                  , "ptm" ,
    "avantgarde"             , ""    ,
    "bookman"                , "pbk" ,
    "courier"                , "pcr" ,
    "helvetica"              , "phv" ,
    "new century schoolbook" , ""    ,
    "palatino"               , "ppl" ,
    "symbol (greek)"         , ""    ,
    "itc zapf chancery"      , ""    ,
    "itc zapf dingbats"      , ""    ,
    NULL };

static char* attributes[] = {
    "medium",  "\\mdseries",
//    "regular", "",
    "italic",  "\\itshape",
    "bold",    "\\bfseries",
    "oblique", "\\itshape",
//    "demi",    "demi",
//    "light",   "Light",
//    "narrow",  "narrow",
    NULL };

static TfontEGS vt_tex_fontLoadPts (void*dev, int fi, double pts, double*ratio)
{
    TodTex* od = dev;
    *ratio   = 1;
    TfontEGS ret; ret.ptr=(void*)0x3030;
    return ret;
}

static void vt_tex_setGCstr(void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    TodTex* od = dev;

    od->strFontInfo = *font;
    color_chgLazy(&od->strColor,fg);
}

/*======================================================================*/
/*= Drawing strings                                                    =*/

STV vt_tex_drawNWr_Pix(void* dev, Tod_GCdef gcd, double _x, double _y, CstrBrief text, double a)
{
    char*     p;
    int       i;
    TodTex*   od = dev;
    TfontDef* fd = odres_getFontUsed(od->any.resources,od->strFontInfo.fo_index);
    double height = LUNIT_PtToCm( od->strFontInfo.fo_size );

    char size[200];
    sprintf(size,"\\fontsize{%.1fpt}{%.1fpt}\\selectfont ",od->strFontInfo.fo_size,od->strFontInfo.fo_size);

    char font[200];
    for ( i=0,p=font,*p=0 ; families[i]!=0 ; i+=2) {
        if (strstr(fd->family,families[i])!=0 ) {
            p += sprintf(p,"\\fontfamily{%s}",families[i+1]);
            break;
        }
    }
    if ( font[0]==0 ) sprintf(p,"\\fontfamily{cmr}");

    char series[200];
    for ( i=0,p=series,*p=0 ; attributes[i]!=0 ; i+=2) {
        if (strstr(fd->style,attributes[i])!=0 ) {
                p += sprintf(p,"%s",attributes[i+1]);
        }
    }

#if 0
    if ( text[2]=='6') {
        double x=_x,y=od->picDy-_y;
        double bl = height*(fd->geom.idy+fd->geom.ils-fd->geom.asc);
        fprintf(od->stream,"\\definecolor{yfigclr}{RGB}{255,0,0}\\draw[draw=yfigclr,line width=1.0pt](  1.00,  %.2f) -- (  8.00,  %.2f) ;",y,y);
        fprintf(od->stream,"\\definecolor{yfigclr}{RGB}{ 0,255,0}\\draw[draw=yfigclr,line width=1.0pt](  1.00,  %.2f) -- (  8.00,  %.2f) ;",y-height,y-height);
        fprintf(od->stream, "\\definecolor{yfigclr}{RGB}{0,255,255}\\draw[draw=yfigclr,line width=1.0pt](  1.00,  %.2f) -- (  8.00,  %.2f) ;",y-height+bl,y-height+bl);
        printf("%s: pts=%4.1f height=%4.2f idy=%4.2f asc=%4.2f ils=%4.2f\n",text,od->strFontInfo.fo_size,height,fd->geom.idy,fd->geom.asc,fd->geom.ils);
    }
#endif
    char color[200];
    double x0=_x,y0=od->daDy-_y;
    double bl = height*(fd->geom.ils+fd->geom.asc-1.);
    double x=x0, y = y0-bl;
    fprintf(od->stream,"  %s\\draw (%5.2f,%5.2f) "
            "node[yfigclr,anchor=north west,inner sep=0cm,rotate=%.1f]"
            "{%s%s%s",
            odtex_gc_getColor(color,od,od->strColor),x+od->border,y+od->border,RadToDeg(a),
            series,font,size);
    if ( strstr(fd->family,"dingbats")!=0 ) {
        odtex_str_print_ding(od->any.msgdrv,od->stream,text);
    } else {
        odtex_str_print_std(od->any.msgdrv,od->stream,text);
    }
    fprintf(od->stream,"} ;\n");
#if 0
    Tdble2 pt = dble2_init(0,-bl);
    if ( a!=0 ) {
        ROTCC_PAXY(pt,a,x0,y0);
    }
    fprintf(od->stream,"  %s\\draw (%5.2f,%5.2f) "
            "node[yfigclr,anchor=north west,inner sep=0cm,shift={(%.2f,%.2f)},rotate=%.1f]"
            "{%s%s%s%s} ;\n",
            odtex_gc_getColor(color,od,od->strColor),x,y,pt.x,pt.y,RadToDeg(a),
            series,font,size,text);
#endif
}

/*======================================================================*/
/*= The virtual table of PDF output device.                            =*/

Toutdev_vt tex_vtable = {
    .topVT  = {
        .ident      = 0x00544558,          // acscii of TEX
        .parentVT   = &outdev_vtable, 
        .byteSize   = sizeof( TodTex ),
        .destructor = vt_tex_destructor,
        .clear      = vt_tex_clear,
    },
    .colorVT  = {
        .black = oddrv_vt_black,
        .white = oddrv_vt_white,
    },
    .pxmVT  = {
      //.pxm_new        = vt_tex_pxm_new,
        .pxm_newFromBtm = vt_tex_pxm_newFromBtm,
      //.pxm_newFromImg = vt_tex_pxm_newFromImg,
        .pxm_delete     = vt_tex_pxm_delete,
      //.pxm_draw       = vt_tex_pxm_draw,
        .img_prefFormat = IMGFMT_PNG,
        .img_load       = vt_tex_img_load,
      //.img_free       = vt_tex_img_free,
      //.img_transform  = vt_tex_img_transform,
        .img_draw       = vt_tex_img_draw,
    },
    .ldafVT   = {
        .setGCso      = vt_tex_setGCso,
        .setGCdd      = vt_tex_setGCdd,
        .setGCfillColor           = vt_tex_setGCfillColor,
      //.setGCfillColorBrightness = vt_tex_setGCfillColorBrightness,
        .setGCfillTile            = vt_tex_setGCfillTile,
      //.setGCfillTileZ           = vt_tex_setGCfillTileZ,
      //.draw_pointXY = vt_tex_draw_pointXY,
      //.draw_pointT  = vt_tex_draw_pointT,
        .draw_lineXY  = vt_tex_draw_lineXY,
        .draw_mlineT  = vt_tex_draw_mlineT,
        .fill_mlineT  = vt_tex_fill_mlineT,
        .draw_rectangleXYWH  = vt_tex_draw_rectangleXYWH,
        .fill_rectangleXYWH  = vt_tex_fill_rectangleXYWH,
      //.draw_bezier  = vt_text_draw_bezier,
      //.fill_bezier  = vt_text_fill_bezier,
    },
    .strVT   = {
      //.initGC       = vt_tex_initGCstr,
        .fontLoadPts  = vt_tex_fontLoadPts,
      //.fontLoadPix  = vt_tex_fontLoadPix,
      //.fontUnload   = vt_tex_fontUnload ,
        .setGC        = vt_tex_setGCstr ,
      //.extentsPts   = vt_tex_extentsPts ,
      //.extentsPix   = vt_tex_extentsPix ,
      //.lineInfoPix  = vt_tex_lineInfoPix ,
      //.drawWL_Pix   = vt_tex_drawWL_Pix,
      //.drawNW_Pix   = vt_tex_drawNW_Pix,
        .drawNWr_Pix  = vt_tex_drawNWr_Pix,
      //.drawWLr_Pix  = vt_tex_drawWLr_Pix,
    },
};

/*======================================================================*/
