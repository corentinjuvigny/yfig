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
 *      $Id: bitmap.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/bitmap.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Definition of a bitmap and bitmap set.                             =*/
/*=                                                                    =*/
/*= A bitmap is a rectangular pixel map whose pixel are on (1) or off. =*/
/*= It is implemented as an unsigned char array that gives the bit     =*/
/*= sequence. We uses the usual XBITMAP convention. For instance with  =*/
/*= dx=20 and dy=3 we get the 9 bytes array below (under hexadecimal)  =*/
/*= where B means used bits and U means unused bits.                   =*/
/*=    BB BB BU   // ligne 1 (y=0)                                     =*/
/*=    BB BB BU   // ligne 2 (y=1)                                     =*/
/*=    BB BB BU   // ligne 3 (y=2)                                     =*/
/*=                                                                    =*/
/*= Definition of an image.                                            =*/
/*=                                                                    =*/
/*= A image is rectangular pixel map whose pixel are a color.          =*/
/*= At this level, it is a type that contains:                         =*/
/*=     - a file name (the image data).                                =*/
/*=     - an opaque type at EGS level.                                 =*/
/*=     - a time-stamp (the date of the file when egs data have been   =*/
/*=       loaded.                                                      =*/
/*======================================================================*/

#include "bitmap-types.h"

/*======================================================================*/
#ifndef FILE_TOOLS_BITMAP_H
#define FILE_TOOLS_BITMAP_H
#include "CPLUSPLUS.h"
/*======================================================================*/

/*======================================================================*/
/*= bitmap                                                             =*/

typedef unsigned char Tuchar;
typedef const Tuchar  Cuchar;
typedef unsigned int Tuint;
typedef const Tuint  Cuint;

typedef struct _Trawdata Trawdata;
typedef const   Trawdata Crawdata;

struct _Trawdata {
    int    refcnt;   // the reference counter (-1 means static allocation)
    int    bytesize; // the byte size of data
    Tuchar data[];   // the data
};

Trawdata* rdt_newBitEated ( int bs, Tuchar* bits);
Trawdata* rdt_newBitClone ( int bs, Cuchar* bits);
Trawdata* rdt_newBitStatic( int bs, Cuchar* bits);

Trawdata* rdt_newClone( Crawdata* rdt );
Trawdata* rdt_newLazy ( Crawdata* rdt );
void      rdt_delete  ( Trawdata* rdt );

/*======================================================================*/
/*= The bitmap structure                                               =*/

#define BTM_RawSize(dx)     (((dx)+7)/8)
#define BTM_ByteSize(dx,dy) (BTM_RawSize(dx)*(dy) )

struct _Tbitmap {
    int	       dx,dy;    // the geometry in bit of bitmap
    Trawdata*  bits;     // the bits under XBITMAP format.
    int        refcnt;   // the reference counter (-1 means static allocation)
};

extern Tbitmap* btm_newBitEated (int dx, int dy, Tuchar* bits);
extern Tbitmap* btm_newBitClone (int dx, int dy, Cuchar* bits);
extern Tbitmap* btm_newBitStatic(int dx, int dy, Cuchar* bits);
extern Tbitmap* btm_newClone (Cbitmap* btm);
extern Tbitmap* btm_newLazy  (Cbitmap* btm);
extern void     btm_delete   (Tbitmap* btm);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= The bitmap set                                                     =*/

struct _TbitmapSet {
    int      eleMemNb; // element number of the memory block eles.
    int      eleNb;    // element number of set (eleNb<=eleMemNb)
    Tbitmap**eles;     // the set elements
    int      refcnt;   // the reference counter FIXME not used
};

extern TbitmapSet* btmset_new      (int n);
extern TbitmapSet* btmset_newClone (CbitmapSet* s);
extern TbitmapSet* btmset_newLazy  (CbitmapSet* s);
extern void        btmset_delete   (TbitmapSet* s);

extern void btmset_addEated (TbitmapSet* s, Tbitmap*btm );
extern void btmset_addClone (TbitmapSet* s, Cbitmap*btm );
extern void btmset_addLazy  (TbitmapSet* s, Tbitmap*btm );

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Some conversion                                                    =*/

/**
 * The btm_XBM2BitSeq function returns a byte sequence that contains
 * the bits of the btm bitmap behind each other. For instance with
 * dx=20 and dy=3, the XBM format gives the next 9 bytes:
 *     BB BB XU   // ligne 1 (y=0)
 *     BB BB YU   // ligne 2 (y=1)
 *     BB BB ZU   // ligne 3 (y=2)
 * The byte sequence that is returned contains 8 bytes:
 *     BB BB XB BB BY BB BB ZU
 *
 * Notes that for dx%8==0 the initial and resulting byte sequence
 * are the same. So calling a duplication fonction will be more
 * efficient.
 *
 * The caller must free the returned Trawdata after use by invoking
 * the rdt_delete function.
**/
extern Trawdata* btm_XBM2BitSeq(Cbitmap*btm);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Default bitmap set                                                 =*/

/**
 * A few bitmaps for filling shapes (wall, hexagone, ...)
 *
 * The btm_defaultFillPattern returns a bitmap set containing
 * the btm_fill_pattern_t bitmaps.
 * The caller must delete the returned set after use.
**/
extern const Tbitmap  btm_fill_pattern_t[];
extern const int      btm_fill_pattern_n;
extern TbitmapSet*    btm_defaultFillPattern();

/**
 * A few bitmaps for filling shapes with a color that allows to darken
 * or lighten it.
 * The btm_fill_lightShade bitmaps have increasing number of bits set
 * to on (1) from 0/1024 (btm_fill_lightShade[0]) to 1024/1024.
 *
 * The btm_defaultShadPattern function returns the btm_fill_lightShade_t
 * bitmaps.
 * The btm_defaultLightPattern function returns the btm_fill_lightShade_t
 * bitmaps except the last one (all the bits set to on) in reverse order.
 *
 * Setting the off pixels to the black color and
 *         the on pixels to the c color gives:
 *   defaultShadPattern[0]    ---> black color
 *   defaultShadPattern[i]    ---> lighter than defaultShadPattern[0]
 *   defaultShadPattern[i+1]  ---> lighter than defaultShadPattern[i]
 *   defaultShadPattern[last] ---> the c color
 *
 * Setting the off pixels to the white color and
 *         the on pixels to the c color gives:
 *   defaultLightPattern[0]    ---> lighter than defaultShadPattern[last]
 *                                 (c color except of 40 white pixels)
 *   defaultLightPattern[i]    ---> lighter than defaultLightPattern[0]
 *   defaultLightPattern[i+1]  ---> lighter than defaultLightPattern[i]
 *   defaultLightPattern[last] ---> the white color
**/
extern const Tbitmap  btm_fill_lightShade_t[];
extern const int      btm_fill_lightShade_n;
extern TbitmapSet*    btm_defaultShadePattern();
extern TbitmapSet*    btm_defaultLightPattern();

/*======================================================================*/
/*= Image.                                                             =*/

#define IMG_OP_NoOpe      0x00  // no operation
#define IMG_OP_FLIP_H     0x10  // horizontal flip (x --> -x)
#define IMG_OP_FLIP_V     0x20  // vertical   flip (y --> -y)
#define IMG_OP_ROTA_P000  0x00  // no rotation
#define IMG_OP_ROTA_P090  0x01  // rotation of  90 degrees
#define IMG_OP_ROTA_P180  0x02  // rotation of 180 degrees
#define IMG_OP_ROTA_P270  0x03  // rotation of -90 degrees
#define IMG_OP_ROTA       0x03  // ope&IMG_OP_ROTA --> nb of quarter turns

#define IMG_OP_FLIP_HV    0x30            // vertical+horizontal flip

/**
 * These macros defines the 8 transformations that exists.
 * They are called the canonical transformations.
**/
#define IMG_OP_CAN_FX0FY0R000 (IMG_OP_ROTA_P000)
#define IMG_OP_CAN_FX1FY0R000 (IMG_OP_ROTA_P000|IMG_OP_FLIP_H)
#define IMG_OP_CAN_FX0FY1R000 (IMG_OP_ROTA_P000|IMG_OP_FLIP_V)
#define IMG_OP_CAN_FX1FY1R000 (IMG_OP_ROTA_P000|IMG_OP_FLIP_HV)
#define IMG_OP_CAN_FX0FY0R090 (IMG_OP_ROTA_P090)
#define IMG_OP_CAN_FX1FY0R090 (IMG_OP_ROTA_P090|IMG_OP_FLIP_H)
#define IMG_OP_CAN_FX0FY1R090 (IMG_OP_ROTA_P090|IMG_OP_FLIP_V)
#define IMG_OP_CAN_FX1FY1R090 (IMG_OP_ROTA_P090|IMG_OP_FLIP_HV)

/**
 * The image_canonical function returns the canonical form
 * (see IMG_CAN_FX.FY.P0.. macros) of the ope transformation.
 * ope is assumed as a logic or of IMG_OP_... macros.
 *
 * The image_canonical_hvr is similar to the image_canonical
 * function except that it returns the ope under hvr format:
 *   IMG_OP_CAN_FX0FY0R000 --> fx=0 fy=0 rot=0
 *   IMG_OP_CAN_FX1FY0R000 --> fx=1 fy=0 rot=0
 *   IMG_OP_CAN_FX0FY1R000 --> fx=0 fy=1 rot=0
 *   IMG_OP_CAN_FX1FY1R000 --> fx=1 fy=1 rot=0
 *   IMG_OP_CAN_FX0FY0R090 --> fx=0 fy=0 rot=1
 *   IMG_OP_CAN_FX1FY0R090 --> fx=1 fy=0 rot=1
 *   IMG_OP_CAN_FX0FY1R090 --> fx=0 fy=1 rot=1
 *   IMG_OP_CAN_FX1FY1R090 --> fx=1 fy=1 rot=1
**/
extern int  image_canonical(int ope);
extern void image_canonical_hvr(int ope, int*fx, int*fy, int*rot);

struct _Timage {
    TstrEaten img_path;    // The original path of the image file
    TstrEaten img_path2;   // NULL or a file conatining the same image
                           // under an other image format.
    TimageEGS img_egs;     // The image loaded by EGS.
    time_t    img_time;    // modification date of image file when it was
                           // loaded into egs.
    int       img_ope;     // see IMG_OP_...
    int       img_refcnt;  // the reference counter (-1 means static allocation)
    char      img_error_noFile;          // 1:error message is already printed
    char      img_error_fmtUnsupported;  // 1:error message is already printed
    char      img_error_failedToConvert; // 1:error message is already printed
#define       img_dx      img_egs.dx // image width  in pixel (if loaded)
#define       img_dy      img_egs.dy // image height in pixel (if loaded)
};

extern Timage* image_new      (CstrBrief path, int ope);
extern Timage* image_newClone (Cimage* img);
extern Timage* image_newLazy  (Cimage* img);
extern void    image_delete   (Timage* img);

extern const char* DBG_image(Cimage* img);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_BITMAP_H
/*======================================================================*/
