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
 *      $Id: resources.h 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/resources.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Some ressources required for drawing.                              =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_RESOURCES_H
#define FILE_OUTDEV_RESOURCES_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "types.h"
#include "tools/font-types.h"
#include "tools/bitmap.h"

/*======================================================================*/
/*= Data Structure                                                     =*/

struct _TodResources {
    // dot and dash preferences
    float dot[2];       // dot[0] :dot  dot[1] :space between dots
    float dash[2];      // dash[0]:dash dash[1]:space between dashes
    float dash1dot1[4]; // dash1dot1[0]:dash dash1dot1[1]:space till dot
                        // dash1dot1[2]:dot  dash1dot1[3]:space till dash

    // tiles for fillings
    TbitmapSet* lighBtms; // bitmap used for DA_FS_LighTile filling
    TbitmapSet* shadBtms; // bitmap used for DA_FS_ShadTile filling
    TbitmapSet* pattBtms; // bitmap used for DA_FS_PattTile filling

    // fonts
    TfontDefSet*  fontUsed;
    TfontInfoSet* fontCache;

    // GC
    TstyleSet*  gcStack[OD_GC_NB];

    int         refcnt;   // reference counter
};

/*======================================================================*/
/*= Interface for building resources                                   =*/

extern TodResources* odres_new       (Ccolor*c);
extern TodResources* odres_newWithBtm(Ccolor*c);
extern TodResources* odres_newClone  (CodResources*res);
extern TodResources* odres_newLazy   (TodResources*res);
extern void          odres_delete    (TodResources*res, TfontEGSunload f, void* od);

//extern TodResources* odres_newDefault();

/*======================================================================*/
/*= Accessors to resources                                             =*/

extern Tbitmap* odres_getLighBtm(CodResources*res, int idx);
extern Tbitmap* odres_getShadBtm(CodResources*res, int idx);
extern Tbitmap* odres_getPattBtm(CodResources*res, int idx);

extern TfontDef*odres_getFontUsed(CodResources*res, int idx);

/*======================================================================*/
/*= Change resource elements.                                          =*/

extern void odres_chgLighBtmsEated(TodResources*res,TbitmapSet* btms);
extern void odres_chgLighBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgLighBtmsLazy (TodResources*res,TbitmapSet* btms);
extern void odres_chgShadBtmsEated(TodResources*res,TbitmapSet* btms);
extern void odres_chgShadBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgShadBtmsLazy (TodResources*res,TbitmapSet* btms);
extern void odres_chgPattBtmsEated(TodResources*res,TbitmapSet* btms);
extern void odres_chgPattBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgPattBtmsLazy (TodResources*res,TbitmapSet* btms);

extern void odres_chgFontUsedEated(TodResources*res,TfontDefSet* s);
extern void odres_chgFontUsedClone(TodResources*res,CfontDefSet* s);
extern void odres_chgFontUsedLazy (TodResources*res,TfontDefSet* s);

/*======================================================================*/
/*= GC stack handler.                                                  =*/

/**
 * The odres_gcstack_new function creates the gc[i] stacks and pushes
 * in all the stacks a default GC.
 * The odres_gcstack_delete functions frees all the gc[i] that the gc[i]
 * stacks contain and then frees all the memory the gc[i] stacks allocated.
**/
extern void odres_gcstack_new   (TstyleSet* gc[OD_GC_NB], Ccolor*c);
extern void odres_gcstack_delete(TstyleSet* gc[OD_GC_NB]);

extern Tsty_text* odres_style_topText(TodResources*od, Tod_GCdef gcd);
extern Tsty_line* odres_style_topLine(TodResources*od, Tod_GCdef gcd);
extern Tsty_fill* odres_style_topFill(TodResources*od, Tod_GCdef gcd);
extern Tstyle*    odres_style_top    (TodResources*od, Tod_GCdef gcd);
extern void       odres_style_pushTop(TodResources*od, Tod_GCdef gcd);
extern void       odres_style_pop    (TodResources*od, Tod_GCdef gcd);

/*======================================================================*/
/*= Interface from output device                                       =*/

// FIXME zf is not used 
extern Tbitmap* od_getLighBtm(Toutdev*od, int idx, double zf);
extern Tbitmap* od_getShadBtm(Toutdev*od, int idx, double zf);
extern Tbitmap* od_getPattBtm(Toutdev*od, int idx, double zf);

/**
 * The od_getLighCoef function converts the britghness given
 * by the idx lighten bitmap by a real number in [0:1] with
 * 0 meaning britghness unchanged and 1 meanings the white color.
 *
 * The od_getShadCoef function converts the britghness given
 * by the idx darken bitmap by a real number in [0:1] with
 * 0 meaning britghness unchanged and 1 meanings the dark color.
**/

extern double   od_getLighCoef(Toutdev*od, int idx);
extern double   od_getShadCoef(Toutdev*od, int idx);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_RESOURCES_H
/*======================================================================*/
