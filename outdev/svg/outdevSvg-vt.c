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
 *      $Id: outdevSvg-vt.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/svg/outdevSvg-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The vtable for TodSvg output device.                               =*/
/*=                                                                    =*/
/*= This file is only included by the outdevPdf.c so all definitions   =*/
/*= are static.                                                        =*/
/*======================================================================*/

#include "outdevSvg-priv.h"
#include "outdev/outdev-vt.h"
#include "outdev/resources.h"

#include "tools/file.h"
#include "tools/lenunit.h"
#include "tools/imgfmt.h"

#define STV static void

#define _VCONV_Y_D2D(od,_y)    ( (_y)+od->border)
#define _VCONV_X_D2D(od,_x)    ( (_x)+od->border)
#define _VCONV_Y_Ds2Win(od,_y) ( _VCONV_Y_D2D(od,VCONV_Y_Ds2Win(od,_y)))
#define _VCONV_X_Ds2Win(od,_x) ( _VCONV_X_D2D(od,VCONV_X_Ds2Win(od,_x)))

/*======================================================================*/
/*= Constructors & destructor                                          =*/

static void vt_svg_destructor(void* dev) { oddrv_free(dev); }
static void vt_svg_clear(const void*dev) { }

/*======================================================================*/
/*= pixmap handler                                                     =*/

static TpixmapEGS vt_svg_pxm_newFromBtm(void*dev, Cbitmap*btm, Ccolor*fgc, Ccolor* bgc) { return PXMEGS_none; }
static void       vt_svg_pxm_delete    (void*dev, TpixmapEGS pxm) { /* done in destructor */ }
static TimageEGS  vt_svg_img_load      (void*dev, CstrBrief path, CstrBrief opath)
{
    TodSvg*   od  = (TodSvg*)dev;
    TimageEGS ret = IMGEGS_none;

    // nothing to do if the image is already loaded
    Tdrvimg* img = drv_imgs_getByPath(od->imgs,path);
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
        if  ( tool_fileOpenROCheck(path)==1 ) { return drv_imgs_add(od->imgs,path,-2); }
        else if ( drv_img_isJPEG(path) )      { return drv_imgs_add(od->imgs,path,IMGFMT_JPG); }
        else if ( drv_img_isPNG(path)  )      { return drv_imgs_add(od->imgs,path,IMGFMT_PNG); }
        else if ( drv_img_isSVG(path)  )      { return drv_imgs_add(od->imgs,path,IMGFMT_SVG); }
        else                                  { return drv_imgs_add(od->imgs,path,-1); }
    }
}

static void       vt_svg_img_draw      (void*dev, Tod_GCdef gcd, CboxDble* srcbox,
                                        TimageEGS img, int h, int v, int r)
{
    TodSvg*   od      = (TodSvg*)dev;
    Tdrvimg*  drvimg  = od->imgs->eles+img.egs.sint;
    
    Cstr imgfile = drv_img_updateRP(drvimg,dev);

//TEMP_spe_printf("D0: imgfile=%s ; idx=%d ; di_path/2=%s -- %s --",imgfile, img.egs.sint, drvimg->di_path, drvimg->di_path2);
    double x  = _VCONV_X_Ds2Win(od,Box_x (*srcbox));
    double y  = _VCONV_Y_Ds2Win(od,Box_y (*srcbox));
    double dx =  VCONV_D_Ds2Win(od,Box_dx(*srcbox));
    double dy =  VCONV_D_Ds2Win(od,Box_dy(*srcbox));

    TEMP_asf_printf(r!=0 && r!=1 && r!=3,"r=%d is an unexpected rotation",r);
    TboxDble unrotbox = r==0 ?  tool_boxdble_initXYWH(x,y,dx,dy) : tool_boxdble_initXYWH(x,y,dy,dx);

    char flip[2000]; flip[0]=0;
    double tx = tool_boxdble_ctr(&unrotbox).x;
    double ty = tool_boxdble_ctr(&unrotbox).y;
    if ( h && v )
        sprintf(flip,"translate(%.2f,%.2f) scale(-1,-1) translate(%.2f,%.2f) ",+tx,+ty,-tx,-ty);
    else if ( h && !v )
        sprintf(flip,"translate(%.2f,%.2f) scale(-1, 1) translate(%.2f,%.2f) ",+tx,-ty,-tx,+ty);
    else if ( !h && v )
        sprintf(flip,"translate(%.2f,%.2f) scale( 1,-1) translate(%.2f,%.2f) ",-tx,+ty, tx,-ty);

    char rotate[200]; rotate[0]=0;
    if ( r==1 )
        sprintf(rotate,"rotate(%d %.2f %.2f) translate(%.2f,%.2f)",r*90,x,y,0.0,-dx);
    else if ( r==3 )
        sprintf(rotate,"rotate(%d %.2f %.2f) translate(%.2f,%.2f)",r*90,x,y,dx,dy);

    char transform[400]; transform[0]=0;
    if ( flip[0]!=0 || rotate[0]!=0 )
        sprintf(transform," transform=\"%s%s%s\"",rotate,flip[0]==0 ? "" :" ",flip);

#if 0
    fprintf(od->stream,"  <rect style=\"fill:none;stroke:black;stroke-width:%.3f\""
                       " x=\"%.1f\" y=\"%.1f\""
                       " width=\"%.1f\" height=\"%.1f\"/>\n",
                       LUNIT_PtToCm(2),BoxArg_XYWH(unrotbox));
#endif

    fprintf(od->stream,"  <image xlink:href=\"%s\""
                       " x=\"%.2f\" y=\"%.2f\""
                       " width=\"%.2f\" height=\"%.2f\""
                       " preserveAspectRatio=\"none\""
                       "%s"
                       "/>\n"
                     ,imgfile,BoxArg_XYWH(unrotbox)
                     ,transform
   );
}

/*======================================================================*/
/*= GC settings                                                        =*/

  STV vt_svg_setGCso(void*dev, Tod_GCdef gcd, double th, TlineStyle ls, TjoinStyle js, TcapStyle  cs, TcolorEGS fg) { }
  STV vt_svg_setGCdd(void*dev, Tod_GCdef gcd, double th, TlineStyle ls, TcolorEGS ddh) { }
//STV vt_svg_setGCfillColorBrightness(void*dev, Tod_GCdef gcd, TcolorEGS c, double coef) { }
  STV vt_svg_setGCfillColor(void*dev, Tod_GCdef gcd, TcolorEGS c) { }
  STV vt_svg_setGCfillTile (void*dev, Tod_GCdef gcd, TpixmapEGS _pxm) { }

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

STV vt_svg_draw_pointXY(const void*dev, Tod_GCdef gcd, double x, double y) { PERM_spe_NYI(0); }
STV vt_svg_draw_pointT(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb) { PERM_spe_NYI(0); }

STV vt_svg_draw_lineXY(const void*dev, Tod_GCdef gcd, double _x1, double _y1,
                                                      double _x2, double _y2)
{
    TodSvg*   od    = (TodSvg*)dev;
    TstrEaten gcdef = odsvg_getStyle0(od,gcd);

    double x1 = _VCONV_X_Ds2Win(od,_x1);
    double y1 = _VCONV_Y_Ds2Win(od,_y1);
    double x2 = _VCONV_X_Ds2Win(od,_x2);
    double y2 = _VCONV_Y_Ds2Win(od,_y2);
    fprintf(od->stream,"  <line %s x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" />\n",gcdef,x1,y1,x2,y2);
    tool_free(gcdef);
}

static Tstrfmt* vt_svg_doMLINE(TodSvg*od, int sh, Cdble2* pts, int ptNb)
{
    int i;
    Tstrfmt* str = strfmt_newLSILF(1000,sh,5);
    str->footlen = sprintf(str->foot,"\" />");

    Tdble2 pt = pts[0];
    double x  = _VCONV_X_Ds2Win(od,pt.x);
    double y  = _VCONV_Y_Ds2Win(od,pt.y);
    strfmt_printfItem(str,"    %6.2f,%6.2f",x,y);

    for (i=1 ; i<ptNb ; i+=1) {
        pt = pts[i];
        x  = _VCONV_X_Ds2Win(od,pt.x);
        y  = _VCONV_Y_Ds2Win(od,pt.y);
        strfmt_printfItem(str," %6.2f,%6.2f",x,y);
    }

    return str;
}

STV vt_svg_draw_mlineT(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb)
{
    TodSvg* od = (TodSvg*)dev;
    TstrEaten gcdef = odsvg_getStyle0(od,gcd);
    Tstrfmt* mline = vt_svg_doMLINE(od,4,pts,ptNb);

    fprintf(od->stream,"  <polyline %s points=\"\n%s\n",gcdef,mline->str);

    strfmt_delete(mline);
    tool_free(gcdef);
}

STV vt_svg_fill_mlineT(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb)
{ vt_svg_draw_mlineT(dev,gcd,pts,ptNb); }

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

STV vt_svg_draw_rectangleXYWH(const void*dev, Tod_GCdef gcd,
                              double _x,    double _y,
                              double width, double height)
{
    TodSvg* od = (TodSvg*)dev;
    double  x  =_VCONV_X_Ds2Win(od,_x);
    double  y  =_VCONV_Y_Ds2Win(od,_y);
    double  wd = VCONV_D_Ds2Win(od,width);
    double  ht = VCONV_D_Ds2Win(od,height);

    TstrEaten gcdef = odsvg_getStyle1(od,gcd);
    fprintf(od->stream,"  <rect style=\"%s\" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"/>\n",
            gcdef ,x,y, wd, ht);
    tool_free(gcdef);
}

STV vt_svg_fill_rectangleXYWH(const void*dev,Tod_GCdef gcd,double _x,double _y, double width, double height)
{ vt_svg_draw_rectangleXYWH(dev,gcd,_x,_y,width,height); }

/*======================================================================*/
/*= open and closed spline curves                                      =*/

//static void vt_svg_draw_bezier(const void*dev, Tod_GCdef gcd, Cdble2* pts, int ptNb)

/*======================================================================*/
/*= String handler.                                                    =*/

static TfontEGS vt_svg_fontLoadPts (void*dev, int fi, double pts, double*ratio)
{
    TodSvg* od = dev;
    *ratio   = 1;
    TfontEGS ret; ret.ptr=(void*)0x3030;
    return ret;
}

static void vt_svg_setGCstr(void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    TodSvg* od = dev;

    od->strFontInfo = *font;
    color_chgLazy(&od->strColor,fg);
}

/*======================================================================*/
/*= Drawing strings                                                    =*/

static char* attributes_bold[] = {
//  "medium",  "",
//  "regular", "",
//  "italic",  "",
    "bold",    "1",
//  "oblique", "",
//  "demi",    "",
//  "light",   "",
//  "narrow",  "",
    NULL };

static char* attributes_italic[] = {
//  "medium",  "",
//  "regular", "",
    "italic",  "italic",
//  "bold",    "",
    "oblique", "oblique",
//  "demi",    "",
//  "light",   "",
//  "narrow",  "",
    NULL };

static void vt_svg_drawWLr_Pix(void* dev, Tod_GCdef gcd, double _x, double _y,
                        CstrBrief text, double a)
{
    int i; char tmp[200];
    TodSvg* od = dev;
    TfontDef* fd = odres_getFontUsed(od->any.resources,od->strFontInfo.fo_index);

    char font[200];
    sprintf(font," font-family=\"%s\"",fd->family);

    char size[200];
    double height = LUNIT_PtToCm( od->strFontInfo.fo_size );
    sprintf(size," font-size=\"%.2f\"", height);

    char color[200];
    sprintf(color," fill=\"%s\"",odsvg_getColor(tmp,od,od->strColor));

    char stroke[200]; stroke[0]=0; // bold
    for ( i=0 ; attributes_bold[i]!=0 ; i+=2) {
        if ( strstr(fd->style,attributes_bold[i])!=0 ) {
            sprintf(stroke," stroke=\"%s\" stroke-width=\"%.3f\"",
                    odsvg_getColor(tmp,od,od->strColor),
                    LUNIT_PtToCm(atoi(attributes_bold[i+1]))/2);
            break;
        }
    }

    char italic[200]; italic[0]=0;
    for ( i=0 ; attributes_italic[i]!=0 ; i+=2) {
        if ( strstr(fd->style,attributes_italic[i])!=0 ) {
            sprintf(italic," font-style=\"%s\"", attributes_italic[i+1]);
            break;
        }
    }

    char rotate[200];
    if ( dble_close0(a) )
        rotate[0]=0;
    else
        sprintf(rotate," transform=\"rotate(%.1f %.1f %.1f)\"",
                RadToDeg(-a),_x+od->border,_y+od->border);

    double x= _x;
    double y= _y - height*(1-(fd->geom.asc-0.1));
    fprintf(od->stream,"  <text x=\"%.2f\" y=\"%.2f\"%s%s%s%s%s%s>%s</text>\n",
            x+od->border,y+od->border,
            font,size,color,stroke,italic,rotate,
            text);
}

/*======================================================================*/
/*= The virtual table of PDF output device.                            =*/

Toutdev_vt svg_vtable = {
    .topVT  = {
        .ident      = 0x00544558,          // acscii of TEX
        .parentVT   = &outdev_vtable, 
        .byteSize   = sizeof( TodSvg ),
        .destructor = vt_svg_destructor,
        .clear      = vt_svg_clear,
    },
    .colorVT  = {
        .black = oddrv_vt_black,
        .white = oddrv_vt_white,
    },
    .pxmVT  = {
      //.pxm_new        = vt_svg_pxm_new,
        .pxm_newFromBtm = vt_svg_pxm_newFromBtm,
      //.pxm_newFromImg = vt_svg_pxm_newFromImg,
        .pxm_delete     = vt_svg_pxm_delete,
      //.pxm_draw       = vt_svg_pxm_draw,
        .img_prefFormat = IMGFMT_PNG,
        .img_load       = vt_svg_img_load,
      //.img_free       = vt_svg_img_free,
      //.img_transform  = vt_svg_img_transform,
        .img_draw       = vt_svg_img_draw,
    },
    .ldafVT   = {
        .setGCso      = vt_svg_setGCso,
        .setGCdd      = vt_svg_setGCdd,
        .setGCfillColor           = vt_svg_setGCfillColor,
      //.setGCfillColorBrightness = vt_svg_setGCfillColorBrightness,
        .setGCfillTile            = vt_svg_setGCfillTile,
      //.setGCfillTileZ           = vt_svg_setGCfillTileZ,
      //.draw_pointXY = vt_svg_draw_pointXY,
      //.draw_pointT  = vt_svg_draw_pointT,
        .draw_lineXY  = vt_svg_draw_lineXY,
        .draw_mlineT  = vt_svg_draw_mlineT,
        .fill_mlineT  = vt_svg_fill_mlineT,
        .draw_rectangleXYWH  = vt_svg_draw_rectangleXYWH,
        .fill_rectangleXYWH  = vt_svg_fill_rectangleXYWH,
      //.draw_bezier  = vt_text_draw_bezier,
      //.fill_bezier  = vt_text_fill_bezier,
    },
    .strVT   = {
      //.initGC       = vt_svg_initGCstr,
        .fontLoadPts  = vt_svg_fontLoadPts,
      //.fontLoadPix  = vt_svg_fontLoadPix,
      //.fontUnload   = vt_svg_fontUnload ,
        .setGC        = vt_svg_setGCstr ,
      //.extentsPts   = vt_svg_extentsPts ,
      //.extentsPix   = vt_svg_extentsPix ,
      //.lineInfoPix  = vt_svg_lineInfoPix ,
      //.drawWL_Pix   = vt_svg_drawWL_Pix,
      //.drawNW_Pix   = vt_svg_drawNW_Pix,
      //.drawNWr_Pix  = vt_svg_drawNWr_Pix,
        .drawWLr_Pix  = vt_svg_drawWLr_Pix,
    },
};

/*======================================================================*/
