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
 *      $Id: outdev-drv.h 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdev-drv.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base class of Pdf Tex Svg ... driver.                              =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEVDRV_H
#define FILE_OUTDEVDRV_H
/*======================================================================*/

#include "outdev/outdev.h"
#include "tools/bitmap-types.h"

typedef struct _Tdrvimg Tdrvimg;
typedef const   Tdrvimg Cdrvimg;
typedef struct _TodDrv TodDrv;
typedef const   TodDrv CodDrv;

/*======================================================================*/
/*= Drivers for images.                                                =*/

/**
 * The drv_img_isPNG function returns true (1) if the path file
 * contains an image under the PNG format.
 * Otherwise it returns false (0).
 * 
 * If a driver for checking the file format exists, it is used
 * otherwise the format detection is based on the path extension.
 * The next table summarizes the function result for a given file
 *    |           file             | driver| drv_img_isPNG |
 *    | exist | PNG ext. | PNG fmt | exist |               |
 *    ------------------------------------------------------
 *    |    0  |    0     |     *   |  *    |    0          |
 *    |    0  |    1     |     *   |  *    |    1          |
 *    ------------------------------------------------------
 *    |    1  |    0     |     *   |  0    |    0          |
 *    |    1  |    1     |     *   |  0    |    1          |
 *    ------------------------------------------------------
 *    |    1  |    *     |     0   |  1    |    0          |
 *    |    1  |    *     |     1   |  1    |    1          |
 *    ------------------------------------------------------
 *
 * The other functions drv_imgisXXX are similar to the drv_img_isPNG
 * function but for the XXX image format.
**/
 
extern int drv_img_isPNG (CstrBrief path);
extern int drv_img_isJPEG(CstrBrief path);
extern int drv_img_isPDF (CstrBrief path);
extern int drv_img_isSVG (CstrBrief path);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

struct _Tdrvimg {
      TstrEaten di_path;    // file of image
      int       di_imgfmt;  // file format of path
                            //   -2: file does not exist 
                            //   -1: unknown format
                            //   IMGFMT_...: format
      TimageEGS di_imgegs;  // pointer to it self  

      // data used by TEX & SVG driver
      TstrEaten di_path2;   // file of image to use if defined
      int       di_imgfmt2; // file format of path2 (coded as di_imgfmt)
      TstrEaten di_rpath;   // relative path to use for referencing this image 

      // data used by PDF driver
      TstrEaten di_pdfidstr; // PDF ident of image: /I...
      int       di_oid;      // PDF oid (0: when path is unavailable)
};

/**
 * drv_img_init   : intialize img to default using path & fmt for di_path & di_imgfmt.
 * drv_img_update2: update di_path2 & di_imgfmt2  using path & fmt.
 * drv_img_free   : free the memory used by img (img is not freed).
**/
extern void      drv_img_init    (Tdrvimg*img, CstrBrief path, int fmt, int egs);
extern void      drv_img_update2 (Tdrvimg*img, CstrBrief path, int fmt, int egs);
extern CstrBrief drv_img_updateRP(Tdrvimg*img, CodDrv* od);
extern void      drv_img_free    (Tdrvimg*img);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

typedef struct _TdrvimgSet TdrvimgSet;
typedef const   TdrvimgSet CdrvimgSet;
#define TPLSET drvimg
#include "tools/set-header-template.h"

/*======================================================================*/

Inline TdrvimgSet* drv_imgs_new        ()                { return drvimgset_new(10); }
extern void        drv_imgs_free       (TdrvimgSet* imgs);
extern TimageEGS   drv_imgs_add        (TdrvimgSet* imgs, CstrBrief path, int fmt);
extern Tdrvimg*    drv_imgs_getByPath  (TdrvimgSet* imgs, CstrBrief path);
extern int         drv_imgs_getIdxOfEle(TdrvimgSet* imgs, Cdrvimg*  img);

/*======================================================================*/
/*= Output Device                                                      =*/

#define ODDRV_FIELDS() \
    Toutdev any; TstrEaten outFile,imgDir; FILE* stream; \
    Tcolor *black,*white; \
    Tcolor *usrMissDrawClr,*usrMissFillClr,*usrMissTextClr; \
    TdrvimgSet*imgs

struct _TodDrv {
    ODDRV_FIELDS();

#define VCONV_D_Ds2Win(dev,d)  CONV_D_Ds2Win(&((dev)->any),d) 
#define VCONV_X_Ds2Win(dev,x)  CONV_X_Ds2Win(&((dev)->any),x) 
#define VCONV_Y_Ds2Win(dev,y)  CONV_Y_Ds2Win(&((dev)->any),y) 
#define VCONV_D_Win2Ds(dev,d)  CONV_D_Win2Ds(&((dev)->any),d) 
#define VCONV_X_Win2Ds(dev,x)  CONV_X_Win2Ds(&((dev)->any),x) 
#define VCONV_Y_Win2Ds(dev,y)  CONV_Y_Win2Ds(&((dev)->any),y) 
#define VCONV_D_S2D(dev,d)  CONV_D_Ds2Win(&((dev)->any),d) 
#define VCONV_X_S2D(dev,x)  CONV_X_Ds2Win(&((dev)->any),x) 
#define VCONV_Y_S2D(dev,y)  CONV_Y_Ds2Win(&((dev)->any),y) 
};

extern void DBG_OdDrvCfg (CodPdfCfg* cfg, double srcDx, double srcDy);
extern void DBG_OdDrvDump(void* dev, double scale, double picDx, double picDy,
                                                   double  ppDx, double  ppDy,
                                                   double  daDx, double  daDy);

extern void oddrv_initInNew (void* drv, CstrBrief outFile); // TodDrv
extern void oddrv_initInHead(void* drv); // TodDrv
extern void oddrv_free      (void* drv); // TodDrv

extern void oddrv_genImgFile(void* drv); // TodDrv

extern Tcolor* oddrv_vt_black(const void*dev);
extern Tcolor* oddrv_vt_white(const void*dev);

/*======================================================================*/
#endif // FILE_OUTDEVDRV_H
/*======================================================================*/
