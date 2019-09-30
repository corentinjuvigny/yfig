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
 *      $Id: style.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/style.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Drawing parameters.                                                =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_STYLE_H
#define FILE_OUTDEV_STYLE_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "types.h"
#include "tools/color-types.h"

/*======================================================================*/
/*= The attributes for drawing text                                    =*/

struct _Tsty_text {
    double      stt_angle;
    int         stt_font;
    double      stt_pts;       // size in inch/72
    Tcolor*     stt_color;
};
#define dpt_angle stt_angle
#define dpt_pts   stt_pts
#define dpt_font  stt_font
#define dpt_color stt_color

/*======================================================================*/
/*= The attributes for drawing curves and lines                        =*/

#define OV_THICKNESS_MIN   0   // 1 pixel of the the output device
#define OV_THICKNESS_MAX 100

struct _Tsty_line {
    int          stl_thickness;  // in pt unit (1/72 inch)
    Tcolor*      stl_color;
    TlineStyle   stl_style;
    float        stl_dotdash; // dot style: length of space between dots.
                              // dash style: length of dash and space
                              //             between dash.
    TjoinStyle   stl_join;
    TcapStyle    stl_cap;
};
#define dpl_thickness stl_thickness
#define dpl_color     stl_color
#define dpl_style     stl_style
#define dpl_dotdash   stl_dotdash
#define dpl_join      stl_join
#define dpl_cap       stl_cap

/*======================================================================*/
/*= The attributes for filling an area.                                =*/
/*=                                                                    =*/
/*= The field values of the Tsty_fill structure depend of              =*/
/*= the dpf_style kind. The next table summarizes the correct          =*/
/*= combinations, the 0 value means that the field is not used.        =*/
/*=                                                                    =*/
/*=     style     color shade light pattern                            =*/
/*=     NoFill      0     0     0     0                                =*/
/*=     Solid       *     0     0     0                                =*/
/*=     ShadTile    *     *     0     0                                =*/
/*=     LighTile    *     0     *     0                                =*/
/*=     PattTile    *     0     0     *                                =*/
/*=                                                                    =*/
/*= The next table indicates the meaning of shate and light for a      =*/
/*= c color.                                                           =*/
/*=                                                                    =*/
/*=                 0                 10            20           21    =*/
/*=                                 between                            =*/
/*= stf_shade      black          black and c   c a few darken   c     =*/
/*= stf_light  c a few lighten    c and white      white        none   =*/

struct _Tsty_fill {
    TfillStyle stf_style;    // filling type
    Tcolor*    stf_color;    // fill color
    int        stf_shade;    // index of predefined tile bitmap
    int        stf_light;    // index of predefined tile bitmap
    int        stf_pattern;  // index of predefined tile pattern bitmap
};
#define dpf_style   stf_style
#define dpf_color   stf_color
#define dpf_shade   stf_shade
#define dpf_light   stf_light
#define dpf_pattern stf_pattern

/*======================================================================*/
/*= The Tstyle type groups all the attributes for drawing object in a  =*/
/*= single structure.                                                  =*/

struct _Tstyle {
    Tsty_text   sty_text;
    Tsty_line   sty_line;
    Tsty_fill   sty_fill;
};

/*======================================================================*/
/*= Interface.                                                         =*/

/**
 * The odsty_init/Xxx function initialize the corresponding
 * structure to default values. The main default are:
 *   - thickness :  1
 *   - line style:  DA_LS_Solid
 *   - join style:  DA_JS_Miter
 *   - cap style :  DA_CS_Butt
 *   - fill style:  DA_FS_NoFill for odsty_initFill
 *                  DA_FS_Solid  for odsty_initFillS
 *   - font      :  0
 *   - pts       :  12
 *   - color:       c
**/
extern void      odsty_initText (Tsty_text*  d, Ccolor* c);
extern Tsty_text odsty_copyText (Csty_text*  s);
extern void      odsty_upteText (Tsty_text*  d, Csty_text*  s);
extern void      odsty_uegsText (Tsty_text*  d, CcolorDriver*drv);
extern void      odsty_freeText (Tsty_text*  s);
extern int       odsty_compText (Csty_text*  l, Csty_text*  r);

extern void      odsty_initLine (Tsty_line*  d, Ccolor* c);
extern Tsty_line odsty_copyLine (Csty_line*  s);
extern void      odsty_upteLine (Tsty_line*  d, Csty_line*  s);
extern void      odsty_uegsLine (Tsty_line*  d, CcolorDriver*drv);
extern void      odsty_freeLine (Tsty_line*  s);
extern int       odsty_compLine (Csty_line*  l, Csty_line*  r);

extern void      odsty_initFill (Tsty_fill*  d, Ccolor* c);
extern void      odsty_initFillS(Tsty_fill*  d, Ccolor* c);
extern Tsty_fill odsty_copyFill (Csty_fill*  s);
extern void      odsty_upteFill (Tsty_fill*  d, Csty_fill*  s);
extern void      odsty_uegsFill (Tsty_fill*  d, CcolorDriver*drv);
extern void      odsty_freeFill (Tsty_fill*  s);
extern int       odsty_compFill (Csty_fill*  l, Csty_fill*  r);

extern void      odsty_init     (Tstyle*     s, Ccolor* c);
extern Tstyle    odsty_copy     (Cstyle*     s);
extern void      odsty_upte     (Tstyle*     d, Cstyle*  s);
extern void      odsty_free     (Tstyle*     s);
extern int       odsty_comp     (Cstyle*     l, Cstyle*  r);

/*======================================================================*/
/*= The TstyleSet implements a set of Tstyle that can be also uses as  =*/
/*= a Tstyle stack.                                                    =*/

#define TPLSET style
#define TPLSET_StackIntf
#include "tools/set-header-template.h"

/*======================================================================*/
/*= The TstyleMask defines a key that  identifies the style parameter. =*/
/*= It is uses to define different merge kind of 2 Tstyle.             =*/

typedef enum _TstyleMask {
    // text
    FDP_T_angle = 0x00000001,
    FDP_T_font  = 0x00000002,
    FDP_T_pts   = 0x00000004,
    FDP_T_color = 0x00000008,
    FDP_T_all   = 0x0000000F,

    // curve/line
    FDP_L_thick = 0x00000100,
    FDP_L_color = 0x00000200,
    FDP_L_style = 0x00000400,
    FDP_L_join  = 0x00000800,
    FDP_L_cap   = 0x00001000,
    FDP_L_all   = 0x00001F00,
    FDP_C_all   = 0x00000700,  // curve

    // shape filling
    FDP_F_style = 0x00010000,
    FDP_F_color = 0x00020000,
    FDP_F_slp   = 0x00040000, // shade, light, pattern together
    FDP_F_all   = 0x00070000,

    // 8 free user flags
    // The outdev device API does not handled and used them.
    FDP_U_01    = 0x01000000,
    FDP_U_02    = 0x02000000,
    FDP_U_04    = 0x04000000,
    FDP_U_08    = 0x08000000,
    FDP_U_10    = 0x10000000,
    FDP_U_20    = 0x20000000,
    FDP_U_40    = 0x40000000,
    FDP_U_80    = 0x80000000,
    FDP_U_all   = 0xFF000000,
} TstyleMask;

/*======================================================================*/

/**
 * The odsty_upteLineM function updates the fields of the d structure
 * that are tagged in the m mask with the corresponding values found
 * in the s structure.
 * The fields of the d structure  that are not tagged in the m mask
 * are left unchanged.
 **/
extern void odsty_upteTextM(Tsty_text*  d, Csty_text*  s, TstyleMask m);
extern void odsty_upteLineM(Tsty_line*  d, Csty_line*  s, TstyleMask m);
extern void odsty_upteFillM(Tsty_fill*  d, Csty_fill*  s, TstyleMask m);

/**
 * The odsty_cmpMask function compares the values in the l and r
 * structures specified by the m parameter.
 * 
 * Return
 * It returns the mask (sub-mask of the initial m parameter) that
 * defines the drawing parameters that are different.
**/
extern TstyleMask odsty_cmpMask(TstyleMask m, Cstyle*l,  Cstyle*r);

/**
 * The odsty_merge1 and odsty_merge2 functions merge into a style
 * structure the drawing parameters that are defined
 * in the all and set styles.
 * The merging rules for a P drawing parameter are summarized  in
 * the next tables:
 *         odsty_merge1                 odsty_merge2
 *   (update all dp from obj dp)    (update obj dp from all dp)
 *   mall      mset         res      mall     mset         res
 *     Y        Y          set.P      Y        Y          all.P
 *     Y        N          all.P      Y        N          all.P 
 *     N        Y          all.P      N        Y          set.P
 *     N        N          all.P      N        N          all.P
 *
 * Return:
 *  Both functions return the merged style.
**/
// FIXME Both functions assume that the res structure is uninitialized. 
// FIXME The fields structures that are not handled by the mask (such
// FIXME as fdp_vtable) are copied into the res structure from the all
// FIXME structure.
extern Tstyle odsty_merge1(TstyleMask mall, Cstyle*all,
                           TstyleMask mset, Cstyle*set ); 
extern Tstyle odsty_merge2(TstyleMask mall, Cstyle*all,
                           TstyleMask mset, Cstyle*set ); 

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_STYLE_H
/*======================================================================*/
