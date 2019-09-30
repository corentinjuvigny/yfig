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
 *      $Id: outdev-drv.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdev-drv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base class of Pdf Tex Svg ... driver.                              =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev-drv.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"
#include "outdev/outdevPdf.h"

#include "tools/imgfmt.h"
#include "tools/globalvars.h"
#include "tools/file.h"
#include "tools/color.h"
#include "tools/bitmap.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= Drivers for images.                                                =*/

static int drv_img_isFmtByExtension(CstrBrief path,TimgFmtKd fmt)
{
    TstrEaten ext = tool_basenameExt( path );
    if ( tool_imgfmt_isExtOfKd(fmt,ext)==0 ) {
        tool_free( ext );
        return 0;
    } else {
        tool_free( ext );
        return 1;
    }
}

#if defined(LIBPNG_AVAILABLE)  && LIBPNG_AVAILABLE==1 
#include <png.h>
#endif

extern int drv_img_isPNG(CstrBrief path)
{
#if defined(LIBPNG_AVAILABLE)  && LIBPNG_AVAILABLE==1 
    // check if it's really a PNG file
    FILE* stream;
    if ( (stream=tool_fileOpenRO(&gl_msgdrv_silence,path))==0 ) {
        return drv_img_isFmtByExtension(path,IMGFMT_PNG);
    }
    char header[8]; // 8 is the maximum size that can be checked
    if ( fread(header, 1, 8, stream)!=8 ) {
        fclose(stream);
        return 0;
    }
    fclose(stream);
    if ( png_sig_cmp(header, 0, 8) ) {
        return 0;
    }
    return 1;
#else
    // check only the extension
    return drv_img_isFmtByExtension(path,IMGFMT_PNG);
#endif
}

#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
#include <jpeglib.h>
#include <setjmp.h>

typedef struct _TjpegErrDrv {
  struct jpeg_error_mgr std; /* libjpeg error driver */
  jmp_buf setjmp_buffer;     /* for return to caller */
} TjpegErrDrv;

static void jpeg_errdrv_exit(j_common_ptr cinfo)
{
    TjpegErrDrv* errdrv = (TjpegErrDrv*) cinfo->err;
    longjmp(errdrv->setjmp_buffer, 1);
}
#endif

extern int drv_img_isJPEG(CstrBrief path)
{
#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
    // check if it's really a PNG file
    FILE* stream;
    if ( (stream=tool_fileOpenRO(&gl_msgdrv_silence,path))==0 ) {
        return drv_img_isFmtByExtension(path,IMGFMT_JPG);
    }

    struct jpeg_decompress_struct cinfo;
    TjpegErrDrv errdrv;
    cinfo.err = jpeg_std_error(&errdrv.std);
    errdrv.std.error_exit = jpeg_errdrv_exit;

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, stream);
    if (setjmp(errdrv.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return 0;
    } else {
        jpeg_read_header(&cinfo, TRUE);
        jpeg_destroy_decompress(&cinfo);
        return 1;
    }
#else
    // check only the extension
    return drv_img_isFmtByExtension(path,IMGFMT_JPG);
#endif
}


extern int drv_img_isPDF(CstrBrief path)
{
    // check if it's really a PDF file
    FILE* stream;
    if ( (stream=tool_fileOpenRO(&gl_msgdrv_silence,path))==0 ) {
        return drv_img_isFmtByExtension(path,IMGFMT_PDF);
    }
    char header[10]; 
    if ( fread(header, 1, 10, stream)!=10 ) {
        fclose(stream);
        return 0;
    }
    fclose(stream);
    if ( header[0]=='%' && header[1]=='P' && header[2]=='D' && header[3]=='F' &&
         header[4]=='-' && header[5]>='1' && header[5]<='1' && header[6]=='.' )
        return 1;
    else
        return 0;
}

extern int drv_img_isSVG(CstrBrief path)
{
    return drv_img_isFmtByExtension(path,IMGFMT_SVG);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void drv_img_init   (Tdrvimg*img, CstrBrief path, int fmt, int egs)
{
    img->di_path      = tool_strdup(path);
    img->di_imgfmt    = fmt;
    if ( fmt<0 ) {
        img->di_imgegs = IMGEGS_none;
    } else {
        img->di_imgegs.dx  = 0;
        img->di_imgegs.dy  = 0;
        img->di_imgegs.ope = IMG_OP_NoOpe;
        img->di_imgegs.egs = any_initBySInt( egs );
    }
    img->di_path2     = NULL;
    img->di_imgfmt2   = -2;
    img->di_rpath     = NULL;
    img->di_pdfidstr  = NULL;
    img->di_oid       = 0;
}

extern void drv_img_update2(Tdrvimg*img, CstrBrief path, int fmt, int egs)
{
    tool_strchgClone(&img->di_path2,path);
    img->di_imgfmt2 = fmt;
    if ( fmt<0 )
        img->di_imgegs = IMGEGS_none;
    else
        img->di_imgegs.egs = any_initBySInt( egs );
}

extern CstrBrief drv_img_updateRP(Tdrvimg*img, CodDrv* od)
{
    if ( img->di_rpath==NULL ) {
        TstrEaten tmp;
        if ( img->di_path2==0 ) {
            tmp = tool_basename(img->di_path);
            img->di_rpath = tool_aprintf("%s/%s",od->imgDir,tmp);
        } else {
            tmp = tool_basenameNoExt(img->di_path);
            img->di_rpath = tool_aprintf("%s/%s.png",od->imgDir,tmp);
        }
        tool_free(tmp);
    }
    return img->di_rpath;
}

extern void drv_img_free   (Tdrvimg*img)
{
    tool_free( img->di_path  );
    tool_free( img->di_path2 );
    tool_free( img->di_rpath );
    tool_free( img->di_pdfidstr );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

#define TPLSET drvimg
#include "tools/set-body-template.h"

extern void      drv_imgs_free     (TdrvimgSet* imgs)
{
    int i;
    for (i=0 ; i<imgs->eleNb ; i+=1 ) drv_img_free( &imgs->eles[i] );
    drvimgset_delete( imgs );
}

extern TimageEGS drv_imgs_add(TdrvimgSet* imgs, CstrBrief path, int fmt)
{
    Tdrvimg img;
    drv_img_init(&img,path,fmt,imgs->eleNb);
    drvimgset_add(imgs,&img);
    return img.di_imgegs;
}

extern Tdrvimg*  drv_imgs_getByPath(TdrvimgSet* imgs, CstrBrief path)
{
    int i;
    for (i=0 ; i<imgs->eleNb ; i+=1 ) 
        if ( strcmp(path,imgs->eles[i].di_path)==0 ) return &imgs->eles[i];
    return NULL;
}

extern int         drv_imgs_getIdxOfEle(TdrvimgSet* imgs, Cdrvimg*  img)
{
    int i;
    for (i=0 ; i<imgs->eleNb ; i+=1 ) 
        if ( strcmp(img->di_path,imgs->eles[i].di_path)==0 ) return i;
    return -1;
}

/*======================================================================*/
/*= Output Device                                                      =*/

extern void DBG_OdDrvCfg(CodPdfCfg* cfg, double srcDx, double srcDy)
{
    DBG0_printf("pdf","%s","------------------------------------------\n");
    DBG0_printf("pdf","cfg: mode=0x%03x scale=%.2f paper=%p=(%.2f/%.2f) paperDx/Dy=%.2f/%.2f",
      cfg->cfg_mode,cfg->cfg_scale, cfg->cfg_paper,
      cfg->cfg_paper==0 ? 0.0 : cfg->cfg_paper->wcm,
      cfg->cfg_paper==0 ? 0.0 : cfg->cfg_paper->hcm,
      cfg->cfg_paperDx,cfg->cfg_paperDy);
    DBG0_printf("pdf"," grav=%d bd=%.2f page-matrix=%dx%d ; srcDx/Dy=%.2f/%.2f\n",
      cfg->cfg_gravity,cfg->bd[0],cfg->cfg_npLine,cfg->cfg_npColumn,
      srcDx,srcDy);
    DBG0_printf("pdf","%s","------------------------------------------\n");
}

extern void DBG_OdDrvDump(void* dev, double scale, double picDx, double picDy,
                                                   double  ppDx, double  ppDy,
                                                   double  daDx, double  daDy)
{
    Toutdev* od  = dev;
    DBG0_printf("pdf","%s","------------------------------------------\n");
    DBG0_printf("pdf","od:  src=%5.2f %5.2f %5.2f %5.2f",BoxArg_XYWH(od->boxSrc));
    DBG0_printf("pdf","od:  des=%5.2f %5.2f %5.2f %5.2f",BoxArg_XYWH(od->boxDes));
    DBG0_printf("pdf","od:  ppisrc=%5.2f ppides=%5.2f zoom=%5.2f",OD_ppiSrc(od),od->ppiDes,od_getUsrZoom(od));
    DBG0_printf("pdf","od:  src=%5.2f %5.2f %5.2f %5.2f cm",
            LUNIT_FuToCm( lu_convToFuPpi(Box_x (od->boxSrc),OD_ppiSrc(od)) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_y (od->boxSrc),OD_ppiSrc(od)) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_dx(od->boxSrc),OD_ppiSrc(od)) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_dy(od->boxSrc),OD_ppiSrc(od)) ));
    DBG0_printf("pdf","od:  des=%5.2f %5.2f %5.2f %5.2f cm",
            LUNIT_FuToCm( lu_convToFuPpi(Box_x (od->boxDes),od->ppiDes) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_y (od->boxDes),od->ppiDes) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_dx(od->boxDes),od->ppiDes) ),
            LUNIT_FuToCm( lu_convToFuPpi(Box_dy(od->boxDes),od->ppiDes) ));
    DBG0_printf("pdf","od:  scale=%5.2f",scale);
    DBG0_printf("pdf","od:  pic=%5.2f %5.2f lay=%5.2f %5.2f da=%5.2f %5.2f",
            picDx,picDy, ppDx,ppDy, daDx,daDy);
    DBG0_printf("pdf","%s","------------------------------------------\n");
}

extern void oddrv_initInNew (void* drv, CstrBrief outFile)
{
    TodDrv* od = drv;

    od->outFile   = tool_strdup ( outFile );
  {
    TstrEaten basename = tool_basenameNoExt( od->outFile );
    od->imgDir    = tool_aprintf("./%s-img",basename);
    tool_free(basename);
  }

    od->black          = color_new( NULL      ,  0,  0,  0,EGSCOLOR_none);
    od->white          = color_new( NULL      ,255,255,255,EGSCOLOR_none);
  //od->usrMissDrawClr = color_new("gray90"   ,229,229,229,EGSCOLOR_none);
  //od->usrMissDrawClr = color_new("gray50"   ,127,127,127,EGSCOLOR_none);
    od->usrMissDrawClr = color_new("lightcyan",224,255,255,EGSCOLOR_none);
    od->usrMissFillClr = color_new("lightcyan",224,255,255,EGSCOLOR_none);
    od->usrMissTextClr = color_newLazy(od->black);

    od->imgs           = drv_imgs_new();
}

extern void oddrv_initInHead(void* drv)
{
    TodDrv* od = drv;

    // configure OD_GC_UsrMiss GC
    od_ldaf_setGCsoP(&od->any,OD_GC_MissDraw ,2.0, DA_JS_Miter, DA_CS_Butt, od->usrMissDrawClr);
    od_ldaf_setGCll (&od->any,OD_GC_MissFill ,0, DA_FS_LighTile,od->usrMissFillClr,0,10,0);
    od_str_setGC    (&od->any,OD_GC_MissDraw ,0, 11.0, od->usrMissTextClr);
}

extern void oddrv_free      (void* drv)
{
    TodDrv* od = drv;

    drv_imgs_free (od->imgs);

    color_delete  ( od->black );
    color_delete  ( od->white );
    color_delete  ( od->usrMissDrawClr );
    color_delete  ( od->usrMissFillClr );
    color_delete  ( od->usrMissTextClr );

    tool_free     ( od->outFile );
    tool_free     ( od->imgDir  );
}

extern void oddrv_genImgFile(void* drv)
{
    int i; Cstr srcfile;
    TodDrv*   od  = drv;
    TstrEaten tmp;
    TstrEaten dir = tool_dirname(od->outFile);
    for (i=0 ; i<od->imgs->eleNb ; i+=1 ) {
        Tdrvimg* img = &od->imgs->eles[i];
        if ( img->di_rpath==NULL ) continue;
        tmp = tool_aprintf("%s/%s",dir,img->di_rpath);
        srcfile = img->di_path2 ? img->di_path2 : img->di_path;
        tool_fileMkdirOfPathR(&gl_msgdrv_silence,tmp);
        tool_fileCopy        (&gl_msgdrv_silence,srcfile,tmp);
        tool_free(tmp);
    }
    tool_free(dir);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Tcolor* oddrv_vt_black(const void*dev) { CodDrv* od = dev; return color_newLazy( od->black ); }
extern Tcolor* oddrv_vt_white(const void*dev) { CodDrv* od = dev; return color_newLazy( od->white ); }

/*======================================================================*/
