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
 * $Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: bpi-image.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/bpi-image.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Image implementation.                                              =*/
/*=                                                                    =*/
/*= See the bpi-types.h and bpi.h headers for documentation.           =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/bitmap.h"

/*======================================================================*/

TimageEGS IMGEGS_none = { .dx=0, .dy=0, .ope=0, .egs.ulong = IMGEGS_NONE };

/*======================================================================*/
/*= Interface to Timage                                                =*/

extern Timage* image_new      (CstrBrief path, int ope)
{
    Timage* img     = tool_malloc( sizeof(*img) );
    img->img_path   = tool_strdup( path );
    img->img_path2  = 0;
    img->img_egs    = IMGEGS_none;
    img->img_time   = 0;
    img->img_ope    = ope;
    img->img_refcnt = 1;
    img->img_error_noFile          = 0;
    img->img_error_fmtUnsupported  = 0;
    img->img_error_failedToConvert = 0;
    return img;
}

extern Timage* image_newClone (Cimage* src)
{
    Timage* img     = tool_malloc( sizeof(*img) );
    img->img_path   = tool_strdup( src->img_path );
    img->img_path2  = NULL;
    img->img_egs    = IMGEGS_none;
    img->img_time   = 0;
    img->img_ope    = src->img_ope;
    img->img_refcnt = 1;
    img->img_error_noFile          = src->img_error_noFile;
    img->img_error_fmtUnsupported  = 0;
    img->img_error_failedToConvert = 0;
    return img;
}

extern Timage* image_newLazy  (Cimage* _img)
{ Timage* img=(Timage*)_img; img->img_refcnt+=1; return img; }

extern void    image_delete   (Timage* img)
{
    if ( img->img_refcnt > 0 ) {
        img->img_refcnt -= 1;
        if ( img->img_refcnt == 0 ) {
            if ( img->img_path2 ) unlink( img->img_path2 );
            tool_free( img->img_path );
            tool_free( img->img_path2);
            if ( IMGEGS_IsDefined(img->img_egs) ) {
                PERM_spe_printf("%s","unable to free EGS image");
            }
            tool_free( img );
        }
    }
}

#define NBBUF 5
typedef char Tbuffer[256];
static Tbuffer buffers[NBBUF];
static int nextbuffer=0;
extern Cstr DBG_image(Cimage* img)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    int pos=sprintf(buffer,"addr=%p",img);
    if ( img==0 ) return buffer;
    pos += sprintf(buffer+pos," loaded=%c", IMGEGS_IsNone(img->img_egs) ? 'N' : 'Y');
    pos += sprintf(buffer+pos," dx/dy=%4d/%4d",img->img_dx, img->img_dy);
    pos += sprintf(buffer+pos," ope=%02x",img->img_ope);
    if (img->img_path==0)
        sprintf(buffer+pos," path=nil");
    else {
        int pathlen = strlen(img->img_path);
        int maxlen  = 256-pos - 2;
        if ( maxlen>=pathlen )
            sprintf(buffer+pos," path=%s",img->img_path);
        else
            sprintf(buffer+pos," path=%c...%s",
                    img->img_path[0], img->img_path+pathlen-maxlen-4);
    }
    return buffer;
}

/*======================================================================*/
