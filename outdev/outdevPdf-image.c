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
 *      $Id: outdevPdf-image.c 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf-image.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Routine that creates a PDF image object from an image file.        =*/
/*=                                                                    =*/
/*=
 int pdf_img_obj_jpeg(TodPdf*od, FILE*stream, int*oid, int*dx, int*dy) 
 int pdf_img_obj_png (TodPdf*od, FILE*stream, int*oid, int*dx, int*dy)
 *=*/
/*= This file is only included by the outdevPdf-vt.c so all its        =*/
/*= definitions are static.                                            =*/
/*======================================================================*/

/*======================================================================*/
#if defined(LIBPNG_AVAILABLE) && LIBPNG_AVAILABLE==1
#include <png.h>

#define PNG_color_type_str(type) \
    type==PNG_COLOR_TYPE_GRAY      ? "gray" :\
    type==PNG_COLOR_TYPE_PALETTE   ? "palette" :\
    type==PNG_COLOR_TYPE_RGB       ? "RGB" :\
    type==PNG_COLOR_TYPE_RGB_ALPHA ? "RGBA" :\
    type==PNG_COLOR_TYPE_GRAY_ALPHA ? "grayA" : "????"

int pdf_img_obj_png(TodPdf*od, FILE*stream, int*oid, int*dx, int*dy)
{
    int status,len,i,x,y,streamlength;
    int depth;      // number of bit per pixel
    int color_type; // see the PNG_COLOR_TYPE_... macros
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;
    
    /* check if stream is a PNG description */
    char header[8]; // 8 is the maximum size that can be checked
    if ( (status=fread(header, 1, 8, stream))!=8 ) {
        return -1;
    }
    if (png_sig_cmp(header, 0, 8))
        return -1;
    
    /* initialize stuff */
    if ( (png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,
                    NULL, NULL, NULL))==0 )
        return -1;
    if ( (info_ptr=png_create_info_struct(png_ptr))==NULL ) {
           png_destroy_read_struct(&png_ptr,
                (png_infopp)NULL, (png_infopp)NULL);
        return -1;
    }
    if ( (end_info=png_create_info_struct(png_ptr))==0 ) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return -1;
    }

    // read image
    rewind(stream);
    png_init_io(png_ptr, stream);

    png_color_16 bgclr;
    bgclr.index = 0;
    bgclr.red   = (1<<16)-1;
    bgclr.green = (1<<16)-1;
    bgclr.blue  = (1<<16)-1;
    bgclr.gray  = (1<<16)-1;
    png_set_background(png_ptr, &bgclr,
           PNG_BACKGROUND_GAMMA_FILE, 0/*do not expand*/, 1);

    int png_transforms = 0;
    png_transforms |= PNG_TRANSFORM_STRIP_16;      // strip 16-bit samples to 8 bits
    png_transforms |= PNG_TRANSFORM_STRIP_ALPHA;   // no alpha
    png_transforms |= PNG_TRANSFORM_PACKING;       //  Expand 1, 2 and 4-bit samples to bytes
    png_read_png(png_ptr, info_ptr, png_transforms, NULL);
    
    *dx        = png_get_image_width (png_ptr,info_ptr);
    *dy        = png_get_image_height (png_ptr,info_ptr);
    depth      = png_get_bit_depth (png_ptr,info_ptr);
    color_type = png_get_color_type  (png_ptr,info_ptr);
//    printf("width=%d heigh=%d clrs=%d clrty=%d:%s", *dx, *dy, depth,
//        color_type,PNG_color_type_str(color_type));

    // generate the PDF image object
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

    if ( color_type==PNG_COLOR_TYPE_PALETTE ) {
        png_color* clrs;
        int        clrn;
        png_get_PLTE(png_ptr, info_ptr,&clrs,&clrn);
        // we use a PDF palette
        // notice that PDF palette are limited to 256 colors
        // as the PNG palette.
        char *pdfpalette = tool_malloc(clrn*3*(5+1)+100);
        for (len=0,i=0; i<clrn ; i+=1 ) {
            png_color* clr = clrs+i;
            len+=sprintf(pdfpalette+len,"%s%02x%02x%02x", i==0?"":" ",
                clr->red,clr->green,clr->blue);
        }
        // creation of PDF image object.
        streamlength = (*dx*depth+7)/8 * *dy;
        *oid = od_pdf_objStart(od);
        len = od_pdf_printf(od,
                "  <<\n"
                "  /Type    /XObject\n"
                "  /Subtype /Image\n"
                "  /ColorSpace [/Indexed /DeviceRGB %d <%s>]\n"
                "  /BitsPerComponent %d\n"
                "  /Width  %4d\n"
                "  /Height %4d\n"
                "  /Length %d\n"
                "  >>\n"
                "stream\n"
                ,
                (1<<depth)-1,
                pdfpalette,
                depth,
                *dx,
                *dy,
                streamlength
        );
        tool_free(pdfpalette);
        if ( len<=0 ) goto write_error;
        int bytesPerRow = (*dx*depth+7)/8;
        for (y=0;y<*dy;y+=1) {
            if ( od_pdf_write(od,row_pointers[y],bytesPerRow)<0 ) 
                goto write_error;
        }
        len = od_pdf_printf(od,"\nendstream\n");
        if ( len<=0 ) goto write_error;
        od_pdf_objEnd(od);
    } else if ( color_type==PNG_COLOR_TYPE_RGB ) {
        streamlength = *dx * *dy * 3;
        *oid = od_pdf_objStart(od);
        len = od_pdf_printf(od,
                "  <<\n"
                "  /Type    /XObject\n"
                "  /Subtype /Image\n"
                "  /ColorSpace /DeviceRGB\n"
                "  /BitsPerComponent 8\n"
                "  /Width  %4d\n"
                "  /Height %4d\n"
                "  /Length %d\n"
                "  >>\n"
                "stream\n"
                ,
                *dx,
                *dy,
                streamlength
        );
        if ( len<=0 ) goto write_error;
        for (y=0;y<*dy;y+=1) {
            if ( od_pdf_write(od,row_pointers[y],*dx*3)<0 ) 
                goto write_error;
        }
        len = od_pdf_printf(od,"\nendstream\n");
        if ( len<=0 ) goto write_error;
        od_pdf_objEnd(od);
    } else {
        PERM_spe_printf(" %d color_type (%s) not yet implemented.",
                color_type, PNG_color_type_str(color_type)); 
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;

write_error:
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return -1;
}
#endif

/*======================================================================*/
#if defined(LIBJPEG_AVAILABLE) && LIBJPEG_AVAILABLE==1
#include <jpeglib.h>
#include <setjmp.h>

typedef struct _TjpegErrDrv {
  struct jpeg_error_mgr std; /* libjpeg error driver */
  jmp_buf setjmp_buffer;     /* for return to caller */
} TjpegErrDrv;

void jpeg_errdrv_exit(j_common_ptr cinfo)
{
    TjpegErrDrv* errdrv = (TjpegErrDrv*) cinfo->err;
    longjmp(errdrv->setjmp_buffer, 1);
}

int pdf_img_obj_jpeg(TodPdf*od, FILE*stream, int*oid, int*dx, int*dy)
{
    char buf[4<<10];
    int len,status;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
   struct stat info;
   if ( fstat(fileno(stream),&info)==-1 )
       return -1;
   int streamlength = info.st_size;
   if (streamlength<=0 || streamlength>(1<<30) )
       return -1;

    TjpegErrDrv errdrv;
    cinfo.err = jpeg_std_error(&errdrv.std);
    errdrv.std.error_exit = jpeg_errdrv_exit;

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, stream);
    if (setjmp(errdrv.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return -1;
    }

    status=jpeg_read_header(&cinfo, TRUE);
    printf("status=%d ",status);
    printf("width=%d ",cinfo.image_width);
    printf("heigh=%d ",cinfo.image_height);
    printf("#clrs=%d ",cinfo.num_components);
    printf("cspac=%d=%s\n",
            cinfo.jpeg_color_space,
            cinfo.jpeg_color_space==JCS_GRAYSCALE ? "grayscale" :
            cinfo.jpeg_color_space==JCS_RGB       ? "RGB" :
            cinfo.jpeg_color_space==JCS_YCbCr     ? "YCbCr" :
            cinfo.jpeg_color_space==JCS_CMYK      ? "CMYK" :
            cinfo.jpeg_color_space==JCS_YCCK      ? "YCCK" :
            cinfo.jpeg_color_space==JCS_BG_RGB    ? "BG_RGB" :
            cinfo.jpeg_color_space==JCS_BG_YCC    ? "BG_YCC" : 
            "???"
   );
   *dx = cinfo.image_width;
   *dy = cinfo.image_height;
   jpeg_destroy_decompress(&cinfo);

   rewind(stream);

   *oid = od_pdf_objStart(od);
   len = od_pdf_printf(od,
           "  <<\n"
           "  /Type    /XObject\n"
           "  /Subtype /Image\n"
           "  /Filter  /DCTDecode\n"
           "  /ColorSpace /DeviceRGB\n"
           "  /BitsPerComponent 8\n"
           "  /Width  %4d\n"
           "  /Height %4d\n"
           "  /Length %d\n"
           "  >>\n"
           "stream\n"
           ,
           *dx,
           *dy,
           streamlength
   );
   if ( len<=0 ) return -1;
   while ( (len=fread(buf,1,sizeof(buf),stream))>0 ) {
       if ( od_pdf_write(od,buf,len)<0 ) 
           return -1;
   }
   if ( !feof(stream) ) return -1;
   len = od_pdf_printf(od,"\nendstream\n");
   if ( len<=0 ) return -1;
   od_pdf_objEnd(od);
   return 0;
}
#endif

/*======================================================================*/

#include <tools/file.h>

FILE* vt_pdf_img_convert(CstrBrief from, CstrBrief fmt)
{
    TstrEaten to,cmd;
    FILE* ret;

    ret = tool_tempfile(&to);
    if ( ret==NULL ) return ret;

    if ( strcmp(fmt,"jpeg")==0 )
        cmd = tool_aprintf("convert -background '#ffffff' %s -flatten %s:%s",from,fmt,to);
    else
        cmd = tool_aprintf("convert -background '#ffffff' %s %s:%s",from,fmt,to);
    if ( system(cmd)!=0 ) {
        fclose(ret);
        unlink(to);
        tool_free(cmd);
        tool_free(to);
        return NULL;
    } else {
        unlink(to);
        tool_free(cmd);
        tool_free(to);
        return ret;
    }
}

/*======================================================================*/
