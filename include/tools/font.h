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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: font.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/font.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= font package.                                                      =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_FONT_H
#define FILE_TOOLS_FONT_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "font-types.h"

/*======================================================================*/
/*= font defintion                                                     =*/

typedef struct _TfontGeom {
    float adx;   // pts*adx is an approximation of char width
    float idy;   // pts*adx is the inked height of char (usually 1)
    float asc;   // pts*(idy+ils-asc) gives the base line from top of char
    float ils;   // pts*ils is the inter-line space between 2 lines
  // dy=idy+ils; // pts*dy is the height of char
} TfontGeom;

struct _TfontDef {
    Cstr      family;  // family name of font
    Cstr      style;   // style of font
    TfontGeom geom;    // font geometry
    Cstr      pdffmt;  // printf format to generate the font name (PDF)
    Cstr      x11fmt;  // printf format to generate the font name (X11)
    Cstr      stdfmt;  // printf format to generate the font name (others)
    Cstr      shname;  // short name for human
    Cstr      lgname;  // long name for human
};

#define TFD_GetShortName(fd) ( (fd).shname!=0 ? (fd).shname : (fd).family)
#define TFD_GetLongName(fd)  ( (fd).lgname!=0 ? (fd).lgname : TFD_GetShortName(fd))

/*======================================================================*/
/*= font defintion set                                                 =*/

#define TPLSET_SetType TfontDefSet
#define TPLSET_SetCype CfontDefSet
#define TPLSET_EleType TfontDef*
#define TPLSET_EleCype CfontDef*
#define TPLSET_Func(f) fontdefset_##f
#define TPLSET_AddWithoutPtr
#define TPLSET_WithRefcnt
#include "set-header-template.h"

/**
 * Returns the definitions of fonts that XFIG uses.
 *
 * Note: It is not the right place but a first step to make
 * outdev, vgui and yfig independent of predefined font set.
**/
extern TfontDefSet* font_defaultDefSet();

//extern CfontDef gl_fontTable[FONT_NB];

/*======================================================================*/
/*= font information                                                   =*/

struct _TfontInfo {
    TfontEGS  fo_egs; 
    double    fo_size;    // size in points (1/72 inch)
    int       fo_index;   // font name, index in fontTable
    int       fo_refcnt;  // reference counter
    double    fo_ratio;   // fo_size / <real fo_font size>
                          //   >1 : drawing is smaller than expected
                          //   <1 : drawing is larger than expected
};

extern int fontinfo_cmpIdxSz (CfontInfo* l, CfontInfo* r);
extern int fontinfo_cmpIdxSz2(CfontInfo* l, int fi, double pts);
//extern int fontinfo_cmpEGS   (CfontInfo* l, CfontInfo* r);
//extern int fontinfo_cmpEGS2  (CfontInfo* l, TfontEGS r);
extern TfontInfo* fontinfo_new     (int fi, double pts, TfontEGS f, double ratio);
extern TfontInfo* fontinfo_newClone(TfontInfo* fi);
extern TfontInfo* fontinfo_newLazi (CfontInfo* fi);
extern void       fontinfo_delete  (TfontInfo* fi, TfontEGSunload unload, void* od);

/*======================================================================*/
/*= font information set                                               =*/

struct _TfontInfoSet {
    int          eleMemNb; // element number of the memory block eles.
    int          eleNb;    // element number of set (eleNb<eleMemNb)
    TfontInfo**  eles;     // the set elements
};

extern TfontInfoSet* fontset_new   ();
extern void          fontset_delete(TfontInfoSet* s, TfontEGSunload unload, void* od);
extern CfontInfo*    fontset_add   (TfontInfoSet* s, int fi, double pts,
                                    TfontEGS f, double ratio);
/**
 * The vgui_fontset_addLRU function adds the font (fi,pts,f) to the
 * s font set and marks it as "Last Recently Used".
 * If there is yet a free slot in the set then it is simply added
 * in this slot.
 * If the set is full, a free slot is got by freeing the least used font.
**/
extern CfontInfo* fontset_addLRU   (TfontInfoSet* s, int fi,
                                    double pts, TfontEGS f, double ratio,
                                    TfontEGSunload unload, void* od);
extern CfontInfo* fontset_searchLRU(TfontInfoSet* s, int fi, double pts);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_FONT_H
/*======================================================================*/
