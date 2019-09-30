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
 *      $Id: types.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= basic flags and types of output device.                            =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_TYPES_H
#define FILE_OUTDEV_TYPES_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"

/*======================================================================*/
/*= types.                                                             =*/

typedef struct _Toutdev      Toutdev;
typedef const   Toutdev      Coutdev;
typedef struct _Toutdev_vt   Toutdev_vt;
typedef const   Toutdev_vt   Coutdev_vt;
typedef struct _Toutdev_top_vt     Toutdev_top_vt;
typedef const   Toutdev_top_vt     Coutdev_top_vt;
typedef struct _Toutdev_color_vt   Toutdev_color_vt;
typedef const   Toutdev_color_vt   Coutdev_color_vt;
typedef struct _Toutdev_pxm_vt     Toutdev_pxm_vt;
typedef const   Toutdev_pxm_vt     Coutdev_pxm_vt;
typedef struct _Toutdev_ldaf_vt    Toutdev_ldaf_vt;
typedef const   Toutdev_ldaf_vt    Coutdev_ldaf_vt;
typedef struct _Toutdev_string_vt  Toutdev_string_vt;
typedef const   Toutdev_string_vt  Coutdev_string_vt;
typedef struct _Toutdev_src_vt     Toutdev_src_vt;
typedef const   Toutdev_src_vt     Coutdev_src_vt;

typedef struct _TodResources   TodResources;
typedef const   TodResources   CodResources;

typedef struct _Tsty_text Tsty_text;
typedef const   Tsty_text Csty_text;
typedef struct _Tsty_line Tsty_line;
typedef const   Tsty_line Csty_line;
typedef struct _Tsty_fill Tsty_fill;
typedef const   Tsty_fill Csty_fill;
typedef struct _Tstyle    Tstyle;
typedef const   Tstyle    Cstyle;
typedef struct _TstyleSet TstyleSet;
typedef const   TstyleSet CstyleSet;

#ifndef FILE_TOOLS_DRAWPARAM_TYPES_H
typedef void*   Tegs_pixmap;
#endif

typedef struct _TodPdfCfg TodPdfCfg;
typedef const   TodPdfCfg CodPdfCfg;

/*======================================================================*/
/*= flags that defines styles for drawing lines and filling area.      =*/

#ifndef FILE_TOOLS_DRAWPARAM_TYPES_H
enum _TlineStyle {
    DA_LS_Solid=0,     DA_LS_Dash=1,      DA_LS_Dot=2,
    DA_LS_Dash1Dot=3,  DA_LS_Dash2Dot=4,  DA_LS_Dash3Dot=5, // obsolete
    DA_LS_Dash1Dot1=3, DA_LS_Dash1Dot2=4, DA_LS_Dash1Dot3=5,
    DA_LS_NoLine=6,
    DA_LS_None=7,
};

enum _TjoinStyle {
    DA_JS_Miter=0, DA_JS_Round=1, DA_JS_Bevel=2, DA_JS_None=3
};

enum _TcapStyle {
    DA_CS_Butt=0, DA_CS_Round=1, DA_CS_Proj=2, DA_CS_None=3
};

enum _TfillStyle {
    DA_FS_NoFill  =0x00,  // no area filling 
    DA_FS_Yes     =0x01,  // fill the area (how to fill is given
                          // by the next flags).
    DA_FS_Solid   =0x11,  // fill the area with a color
    DA_FS_ShadTile=0x21,  // tile the area with a dark bitmap
    DA_FS_LighTile=0x41,  // tile the area with a light bitmap
    DA_FS_PattTile=0x81,  // tile the area with a pattern bitmap

    DA_FS_None    =0x08   // a value indicating not a previous
                          // flag.
};

typedef enum _TlineStyle TlineStyle;
typedef const TlineStyle ClineStyle;

typedef enum _TjoinStyle TjoinStyle;
typedef const TjoinStyle CjoinStyle;

typedef enum _TcapStyle  TcapStyle;
typedef const TcapStyle  CcapStyle;

typedef enum _TfillStyle TfillStyle;
typedef const TfillStyle CfillStyle;
#endif

/*======================================================================*/
/*= Graphic Context                                                     */
/*======================================================================*/

typedef enum _Tod_GCdef {
    OD_GC_UsrDraw   =  0, // standard GC for drawing user objects
    OD_GC_UsrFill   =  1, // standard GC for filling user objects
    OD_GC_MissDraw  =  2, // standard GC for drawing and filling replacement
    OD_GC_MissFill  =  3, // for OD feature not unsuported by EGS.
                          // ex: image.fmt, fmt being an unsuported format.
                          // OD draws a filled rectangle with "image.fmt".
    OD_GC_SelArea   =  4, // Used to show a selected area
    OD_GC_Squeleton =  4, // Used to show during object creation, the 
                          // object key characteristics (eg: center for
                          // a circle, diagonal for a rectangle, ...).
                          // Redrawing the same squeleton erases it.
    OD_GC_Preview   =  5, // Used to preview object shape.
                          // redrawing the same shape erases it.

    OD_GC_Marker    =  6, // Used to draw markers.
                          // Redrawing a marker erases it.
    OD_GC_MarkerHL  =  7, // Used to draw highlighted markers.
                          // Redrawing an highlighted marker erases it.

    OD_GC_SysDraw   = 10, // for drawing and writing in main window widget
    OD_GC_SysFill   = 11, // for filling in main window widget
    OD_GC_NB        = 12, // CAUTION: not a GC just the number of
                          // predefined GC.
    OD_GC_None      = 13  // CAUTION: not a GC, it is used to disable a gc
} Tod_GCdef;

/*======================================================================*/
/*= String geometry                                                    =*/
/*=                                                                    =*/
/*=             +-----------------------+                              =*/
/*=           | |                       |  |                           =*/
/*=    ascent ¦ |   ****       ****     |  |                           =*/
/*=           ¦ |  *           *   *    |  |                           =*/
/*=           | |   ****       ****     |  | dy                        =*/
/*=             L-----------C--*--------R  |                           =*/
/*=           ¦ |              *        |  |                           =*/
/*=   descent ¦ |              *        |  |                           =*/
/*=           ¦ |                       |  |                           =*/
/*=             +-----------------------+                              =*/
/*=              ----------------------                                =*/
/*=                       dx                                           =*/
/*= L is the origin of string when OD_SORG_Left                        =*/
/*= C is the origin of string when OD_SORG_CENTER                      =*/
/*= R is the origin of string when OD_SORG_RIGHT                       =*/

#define OD_SORG_LEFT    0  // origin is at the left hand of the text
#define OD_SORG_CENTER  1  // origin is at the middle of the text
#define OD_SORG_RIGHT   2  // origin is at the right hand of the text

struct _Todstrgeo {
    Tdble2 pos;             // origin position
    double ascent,dx,dy;    // geometry in source unit
    double nl;              // the vertical shift of the next line
    int    of;              // where pos is horizontally in string,
                            // see OD_SORG_... 
    int    font;            // The used font
    double pts;             // The font size in point (1/72 Inch)
};

typedef struct _Todstrgeo Todstrgeo;
typedef const   Todstrgeo Codstrgeo;

/*======================================================================*/
/*= Debug                                                              =*/

extern const char* DBG_LS2str(TlineStyle s);
extern const char* DBG_JS2str(TjoinStyle s);
extern const char* DBG_CS2str(TcapStyle  s);
extern const char* DBG_FS2str(TfillStyle s);

extern const char* DBG_STL2str(Csty_line* s);
extern const char* DBG_STF2str(Csty_fill* s);
extern const char* DBG_STT2str(Csty_text* s);


#define DBG0GC(fmt,...)   DBG0_printf("gc",fmt,__VA_ARGS__)
#define DBG1GC(fmt,...)   DBG1_printf("gc",fmt,__VA_ARGS__)
#define DBG2GC(fmt,...)   DBG2_printf("gc",fmt,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_TYPES_H
/*======================================================================*/

