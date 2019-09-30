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
 *      $Id: bitmap-types.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/bitmap-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base types and macros of the bitmap package.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_BITMAP_TYPES_H
#define FILE_TOOLS_BITMAP_TYPES_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
/*= bitmap                                                             =*/

#ifndef TOOL_TPIXMAPEGS
#define TOOL_TPIXMAPEGS
union _TpixmapEGS {
    unsigned long ulong;
    signed   long slong;
    void*         ptr;
    unsigned int  uint;
    signed   int  sint;
};
typedef union  _TpixmapEGS TpixmapEGS;
typedef const   TpixmapEGS CpixmapEGS;

#define PXMEGS_NONE         ((signed long)-1) // color is undefined
#define PXMEGS_IsNone(c)    ((c).ulong==PXMEGS_NONE)
#define PXMEGS_IsDefined(c) ((c).ulong!=PXMEGS_NONE)
extern  TpixmapEGS PXMEGS_none;
#endif

typedef struct _Tbitmap Tbitmap;
typedef const   Tbitmap Cbitmap;

typedef struct _TbitmapSet TbitmapSet;
typedef const   TbitmapSet CbitmapSet;

/*======================================================================*/
/*= Image.                                                             =*/

struct _TimageEGS {
    int    dx,dy; // image size in pixel
    int    ope;   // see IMG_OP_...
    Tany   egs;   // the image at EGS level
};
typedef struct _TimageEGS TimageEGS;
typedef const   TimageEGS CimageEGS;

#define IMGEGS_NONE         ((signed long)-1) // color is undefined
#define IMGEGS_IsNone(c)    ((c).egs.ulong==IMGEGS_NONE)
#define IMGEGS_IsDefined(c) ((c).egs.ulong!=PXMEGS_NONE)
extern  TimageEGS IMGEGS_none;

typedef struct _Timage Timage;
typedef const   Timage Cimage;

/*======================================================================*/
/*= Image Set                                                          =*/

//typedef struct _TimageSet TimageSet;
//typedef const   TimageSet CimageSet;

//#define TPLSET image
//#define TPLSET_EleType Timage*
//#define TPLSET_EleCype Cimage*
//#include "set-header-template.h"

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_BITMAP_TYPES_H
/*======================================================================*/
