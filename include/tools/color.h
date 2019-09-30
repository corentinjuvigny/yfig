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
 *      $Id: color.h 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/color.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This file defines a color making abstraction of the EGS (External  =*/
/*= Graphice System). The main types are:                              =*/
/*=   - Tegs_color : an opaque type that defines the color for EGS.    =*/
/*=   - Tegs_driver: an type that allows the creation and the deletion =*/
/*=                  of a color in the EGS.                            =*/
/*=   - Tcolor:      The color itself. It consists of a name, a RGV    =*/
/*=                  value, an an EGS driver.                          =*/
/*=                                                                    =*/
/*= The package can works without EGS driver, in this case just provide=*/
/*= a NULL EGS driver.                                                 =*/
/*= The functions suffixed by "updateEGS" allow to connect existing    =*/
/*= color objects to an EGS.                                           =*/
/*=                                                                    =*/
/*= The package provides also a basic color set.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_COLOR_H
#define FILE_COLOR_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "color-types.h"

/*======================================================================*/
/*=  The EGS color driver.                                             =*/

struct _TcolorDriver {
    void * egs;
    // the EGS must delete the c color.
    void      (*free)      (void*egs, TcolorEGS c); 
    // the EGS must duplicate the c color.
    TcolorEGS (*dup)       (void*egs, TcolorEGS c); 
    // the EGS must return the n named color or EGSCOLOR_NONE.
    TcolorEGS (*newByName) (void*egs, CstrBrief n);
    // the EGS must return the r,g,b color or EGSCOLOR_NONE.
    TcolorEGS (*newByRGB08)(void*egs, int r, int g, int b);
    TcolorEGS (*newByRGB16)(void*egs, int r, int g, int b);
    // the EGS must return the r,g,b RGB values of c color.
    void      (*getRGB08)  (void*egs, TcolorEGS c, int*r, int*g, int*b);
    void      (*getRGB16)  (void*egs, TcolorEGS c, int*r, int*g, int*b);
};

/*======================================================================*/
/*=  A basic RGB color.                                                =*/

struct _TcolorRGB { int red,green,blue; };

extern void color_rgb8ToHsv(double*    hue, double* sat, double* val,
                            CcolorRGB* rgb);
extern void color_hsvToRgb8(TcolorRGB* rgb,
                            double     hue, double  sat, double  val);

/*======================================================================*/
/*=  The color used in fig data structure.                             =*/

struct _Tcolor {
    CcolorDriver*driver; // specify the access to EGS
    TstrEaten    name;   // name of color (never NULL)
    TcolorRGB    rgb;    // always set and under RGB-8 format 
    TcolorEGS    egs;    // the color for the external graphic system
    int          cnt;    // reference counter (>0:referenced)
    int          ukey;   // user key: not handled, initialized to -1.
} ;

/**
 * The color_new function creates a new color and returns it.
 *  - if one of the RGV values is negative RGB fields are set to -1.
 *  - if the RGB values are valid (all the values greater or equal to 0),
 *    RGB fields are truncated to 16 bits.
 *  - if name is null and the RGB values are valid, it is set to
 *    rgb:hhhh/hhhh/hhhh otherwise to "undefined".
 *  - ukey is set to -1.
 *  - The color driver is set to null.
**/
extern Tcolor* color_new(const char* name,
                         int red, int green, int blue, TcolorEGS ce);

/**
 * The color_delete function decrements the reference counter of
 * the c color. If this last reaches zero, the function frees all
 * the memory used by the c color.
 *
 * The color_deleteS0 function does the same works as color_delete
 * and then sets *pc to null.
 *
 * The color_newLazi function increments the reference counter
 * of the c color.
 *
 * The color_newPriv function duplicates the c color setting the
 * the reference counter of the cloned color to 1.
 *
 * The color_newByXor function creates a new color with RGB values
 * set to a xor between the RGB values of c1 and c2 colors.
 * The EGS driver of the new color is set to the EGS
 * driver of the c1 color.
 * The reference counter of the new color is set to 1.
 *
 * Return:
 *   The color_newPriv and color_newByXor functions return
 *   the created color.
 *
 * Parameter:
 *   - c : it can be the null pointer in color_delete
 *   - *pc : it can be the null pointer in color_freeS0
**/
extern void    color_delete   (Tcolor*c);
extern void    color_deleteS0 (Tcolor**c);
extern Tcolor* color_newLazy  (Ccolor*c);
extern Tcolor* color_newPriv  (Ccolor*c);
extern Tcolor* color_newByXor (Ccolor*c1, Ccolor*c2);
extern void    color_chgLazy  (Tcolor**dd,Ccolor*c);
extern void    color_chgEaten (Tcolor**dd,Tcolor*c);

/**
 * CAUTION: the egs parameter for these function must not be null,
 * use color_new instead.
**/
extern Tcolor* color_newByStr  (CcolorDriver*drv, CstrBrief str);
extern Tcolor* color_newByRGB  (CcolorDriver*drv,
                                int red, int green, int blue);
extern void    color_updateEGS (CcolorDriver*drv, Tcolor*c);

/**
 * Color comparison
**/

// full order: return <0:l<r ; 0:l=r ; >0:l>r
extern int color_cmp(Ccolor*l, Ccolor*r);
extern int color_cmpRGB(CcolorRGB*l, CcolorRGB*r);

// return 1 if l and r colors are close otherwise 0
extern int color_close(Ccolor*l, Ccolor*r);

// return 1 if l and r colors are close otherwise 0
// see tolerance comment in TcolorSet
extern int color_close2(Ccolor*l, Ccolor*r, int tolerance);

/*======================================================================*/
/*=  color set                                                         =*/

struct _TcolorSet {
    CcolorDriver* driver;
    int           tolerance; // be x,y two colors,
                             // be d= |xr-yr| + |xg-yg| + |xb-yb|,
                             // then d<=tolerance means x==y
    Tcolor*       uc;        // undefined color
    int           eleMemNb; // element number of the memory block eles.
    int           eleNb;    // element number of set (eleNb<eleMemNb)
    Tcolor**      eles;     // the set elements
};

extern TcolorSet*    colorset_newDNT (CcolorDriver*drv,int n,int tolerance);
#define              colorset_new(drv)     colorset_newDNT(drv,0,0)
#define              colorset_newDT(drv,t) colorset_newDNT(drv,0,t)
#define              colorset_newDN(drv,n) colorset_newDNT(drv,n,0)

/**
 * free the s set and all the colors that it contains.
**/
extern void          colorset_free (TcolorSet* s);

/**
 * The colorset_add... functions allow to add colors to the s set.
 * If the s set contains a X color close to the c color (see
 * tolerance in TcolorSet), c is not added to the color set and
 * the X color is returned.
 * If the s set does not contain a color close to the c color
 * (see tolerance in TcolorSet), c is added to the color set and
 * returned.
 * 
 * The 3 functions differ in memory allocation of the
 * c and added colors.
 *                     | when addition          | when not
 *                     | c          added in s  | c
 *   ------------------+------------------------+----------
 *   colorset_addLazy  | newLazi(c) c           | unchanged
 *   colorset_addPriv  | unchanged  newPriv(c)  | unchanged
 *   colorset_addEated | unchanged  c           | delete(c)
 *
 * Return:
 *  The  colorset_add function returns the closest color of c
 *  in the s set that respect the tolerance.
**/
extern Ccolor* colorset_addLazy (TcolorSet* s, Ccolor* c);
extern Ccolor* colorset_addPriv (TcolorSet* s, Ccolor* c);
extern Ccolor* colorset_addEated(TcolorSet* s, Ccolor* c);

/**
 * The colorset_addByStr function creates a new color using the
 * color_newByStr function and then adds it to the s set.
 *
 * The colorset_addByRGB function creates a new color using the
 * color_newByRGB function and then adds it to the s set.
 *
 * Return:
 *  On success these function return the new color otherwise they
 *  return the null pointer.
**/
extern Ccolor* colorset_addByStr(TcolorSet* s, CstrBrief name);
extern Ccolor* colorset_addByRGB(TcolorSet* s, int r,int g,int b);

/**
 * colorset_getByUKey returns the color of s set the ukey of which
 * is key otherwise the null pointer.
 *
 * In the case there are several colors with the same user key, the
 * function returns the first corresponding color that it finds.
**/
extern Tcolor* colorset_getByPtr (CcolorSet* s, Ccolor* key);
extern Ccolor* colorset_getByRGB (CcolorSet* s, TcolorRGB key);
extern Ccolor* colorset_getByName(CcolorSet* s, CstrBrief key);
extern Ccolor* colorset_getByUKey(CcolorSet* s, int key);

/**
 * Remove the c color of s set, c is not freed.
 * The set colors are compared to c by pointer.  
 * Note that if the c color does not belong to the set nothing is done.
**/
extern void          colorset_remove(TcolorSet* s, Tcolor* c);

extern void          colorset_update(TcolorSet* s, CcolorDriver* drv);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif  // FILE_COLOR_H
/*======================================================================*/
