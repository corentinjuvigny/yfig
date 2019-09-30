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
 *      $Id: pxm-hl.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/pxm-hl.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the pxm-hl.h header for documentation.                         =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/string-hl.h"
#include "outdev/pxm-hl.h"

#include "tools/file.h"
#include "tools/strings.h"
#include "tools/bitmap.h"
#include "tools/imgfmt.h"
#include "tools/globalvars.h"

/*======================================================================*/
/*= Constructors and destructor                                        =*/

TpixmapEGS od_bpi_pxm_new       (Toutdev*od, int width, int height)
{
    TEMP_asf_printf( od->vtable->pxmVT.pxm_new==0, "%s","no EGS driver");
    return od->vtable->pxmVT.pxm_new(od, width, height);
}

TpixmapEGS od_bpi_pxm_newFromBtm(Toutdev*od, Cbitmap*btm,Ccolor*fgc,Ccolor*bgc)
{
    TEMP_asf_printf( od->vtable->pxmVT.pxm_newFromBtm==0, "%s","no EGS driver");
    return od->vtable->pxmVT.pxm_newFromBtm(od, btm, fgc, bgc);
}

void       od_bpi_pxm_delete    (Toutdev*od, TpixmapEGS pxm)
{
    TEMP_asf_printf( od->vtable->pxmVT.pxm_delete==0, "%s","no EGS driver");
    od->vtable->pxmVT.pxm_delete(od, pxm);
}

void       od_bpi_pxm_draw(Toutdev*od, Tod_GCdef gcd, double x, double y,
                           TpixmapEGS pxm, int xp, int yp, int dxp, int dyp)
{
    TEMP_asf_printf( od->vtable->pxmVT.pxm_draw==0, "%s","no EGS driver");
    od->vtable->pxmVT.pxm_draw(od, gcd, x,y, pxm, xp,yp,dxp,dyp);
}

/*======================================================================*/
/*= Image.                                                             =*/

#define OPT_FileName  0x01
#define OPT_DrawBox   0x02
#define OPT_FillBox   0x04

static void od_bpi_img_drawXYHW_default(int opt, Toutdev*od, Cimage* img,
                                double x, double y, double dx, double dy)
{
    if ( opt==0 ) opt=OPT_FileName|OPT_DrawBox|OPT_FillBox;
    Tod_GCdef gcd = OD_GC_MissDraw;
    Tod_GCdef gcf = OD_GC_MissFill;

    if ( (opt&OPT_FillBox)!=0 ) 
        od_fill_rectXYWH(od,gcf,x,y,dx,dy);
    if ( (opt&OPT_DrawBox)!=0 )
        od_draw_rectXYWH(od,gcd,x,y,dx,dy);
    if ( (opt&OPT_FileName)!=0 ) {
        //int    fid = 0;
        //double fsz = 12 / od_getUsrZoom(od);
        char*  bn = img->img_path==0 ? 0 : tool_basename(img->img_path);
        char* str = bn==0 ? "image" : bn;
        // check if the box is large enough to contain the string
        Todstrgeo g = od_str_extents_gc(od,gcd,str,OD_SORG_LEFT,0,0);
        if (g.dx<dx && g.dy<dy) {
            od_str_draw_IBxyCCg(od,gcd,&g,str,x,y,dx,dy);
        }
        tool_free( bn );
    }
}

extern void      od_bpi_img_free0    (Toutdev*od, TimageEGS*img )
{
    if ( IMGEGS_IsNone(*img) ) return;
    if ( od->vtable->pxmVT.img_free!=0 ) {
        od->vtable->pxmVT.img_free(od,*img);
    }
    *img = IMGEGS_none;
}
extern void      od_bpi_img_free     (Toutdev*od, Timage*   img )
{ od_bpi_img_free0( od, &img->img_egs ); }

static void od_bpi_img_transform(Toutdev*od, TimageEGS* img, int ope)
{
    TEMP_asw_printf( od->vtable->pxmVT.img_transform==0, "%s","no EGS driver: img_transform");
    if ( od->vtable->pxmVT.img_transform==0 ) return;
    int fx,fy,rot; image_canonical_hvr(ope,&fx,&fy,&rot);
    od->vtable->pxmVT.img_transform(od,img,fx,fy,rot);
}

// return
//   -  0: success
//   - -1: format not supported.
//   - -2: file does not exist.
//   - -3: no driver.
static int od_bpi_img_drawXYHW_I(Toutdev*od, Tod_GCdef gcd, Timage* img,
                                 double x, double y, double dx, double dy)
{
    Coutdev_pxm_vt* vtable = &od->vtable->pxmVT;

    if ( IMGEGS_IsDefined(img->img_egs) && img->img_ope!=img->img_egs.ope)
        od_bpi_img_free(od,img);

    if ( IMGEGS_IsNone(img->img_egs) ) {
        if ( tool_fileOpenROCheck(img->img_path)!=0 ) {
            return -2;
        } else {
            // try to load the image
            if ( od->vtable->pxmVT.img_load==0 )
                return -3;
            if ( img->img_path2==0 ) {
                img->img_egs = od->vtable->pxmVT.img_load(od,img->img_path ,NULL);
            } else {
                img->img_egs = od->vtable->pxmVT.img_load(od,img->img_path2,img->img_path);
            }

            if ( IMGEGS_IsNone(img->img_egs) ) {
                // image loading fails (image format not supported by EGS)
                return -1;
            }
            if ( od->vtable->pxmVT.img_draw==0 )
                od_bpi_img_transform(od,&img->img_egs,img->img_ope);
            img->img_egs.ope = img->img_ope;
        }
    }

    // the image is loaded in img->img_egs
    if ( od->vtable->pxmVT.img_draw!=0 ) {
        TboxDble srcbox = { {x,y} , {dx,dy} };
        int fx,fy,rot; image_canonical_hvr(img->img_ope,&fx,&fy,&rot);
        od->vtable->pxmVT.img_draw(od,gcd,&srcbox,img->img_egs,fx,fy,rot);
    } else if ( vtable->pxm_newFromImg && vtable->pxm_draw ) {
        double dxpix= round( CONV_D_S2D(od,dx) );
        double dypix= round( CONV_D_S2D(od,dy) );
        TpixmapEGS pxm = vtable->pxm_newFromImg(od,dxpix,dypix,&img->img_egs);
        od_bpi_pxm_draw(od,gcd,x,y,pxm,0,0,dxpix,dypix);
        od_bpi_pxm_delete(od,pxm);
    } else {
        return -3;
    }
}

extern void od_bpi_img_drawXYHW(Toutdev*od, Tod_GCdef gcd, Timage* img,
                                double x, double y, double dx, double dy)
{
#if 0
#define DBG_MESS(mess) TEMP_spe_printf("status=%+2d ; imgEGS=%d path=%s (%s)",\
        status,IMGEGS_IsDefined(img->img_egs),img->img_path,mess)
#else
#define DBG_MESS(mess) while (0)
#endif

    int status,fmt;
    while ( 1 ) {
        status=od_bpi_img_drawXYHW_I(od,gcd,img,x,y,dx,dy);
        if ( status==0 ) { DBG_MESS("SUCCES");
            return ;
        } else if ( status==-1 ) { DBG_MESS("FMT NOT SUPP");
            if ( (fmt=od->vtable->pxmVT.img_prefFormat)<0 || img->img_path2!=0 ) {
                if ( img->img_error_fmtUnsupported==0 ) {
                    tool_errerr_printf(od->msgdrv,"unsupported format for the image file: %s",img->img_path);
                    img->img_error_fmtUnsupported = 1;
                }
                break;
            }
            // try to convert it into fmt format
            if ( img->img_error_failedToConvert!=0 ) break;
            if ( img->img_path2==0 ) {
                FILE* stream = tool_tempfile(&img->img_path2);
                fclose( stream );
            }
            if ( tool_imgfmt_convIM(&gl_msgdrv_silence, img->img_path,-1, img->img_path2,fmt, 0,0) ) {
                if ( img->img_error_failedToConvert==0 ) {
                    tool_errerr_printf(od->msgdrv,"failed to convert to %s the image file: %s",
                            tool_imgfmt_getByKd(fmt)->shortname,img->img_path);
                    img->img_error_failedToConvert = 1;
                }
                // unlink(img->img_path2);
                break;
            }
            continue;
        } else if ( status==-2 ) { DBG_MESS("NO FILE");
            if ( img->img_error_noFile==0 ) {
                tool_errerr_printf(od->msgdrv,"failed to get the image file: %s",img->img_path);
                img->img_error_noFile = 1;
            }
            break;
        } else if ( status==-3 ) { DBG_MESS("NO DRIVER");
            if ( od->error_imgLoad_NoEgs==0 ) {
                tool_wrnerr_printf(od->msgdrv,"no driver for loading image.");
                od->error_imgLoad_NoEgs = 1;
            }
            break;
        } else {
            PERM_spe_printf("%d is an unexpected status (%s image not loaded)",status,img->img_path);
            break;
        }
    }
    // image file can not be drawn --> switch to default drawing
    od_bpi_img_drawXYHW_default(7,od,img,x,y,dx,dy);
}

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/

/*======================================================================*/
