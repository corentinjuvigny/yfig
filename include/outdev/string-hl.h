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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: string-hl.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/string-hl.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for drawing strings into an output device (Toutdev).     =*/
/*= HL means High Level.                                               =*/
/*=                                                                    =*/
/*= This API is a front-head to the Low Level drawing API (EGS) that   =*/
/*= (Toutdev->vtable->strVT) that takes into account special cases.    =*/
/*=                                                                    =*/
/*= So it is safer to use these functions (od_str_...) instead of      =*/
/*= their low level equivalents.                                       =*/
/*=                                                                    =*/
/*= The standard way for writing the string "hello" at (1000,2000)     =*/
/*= source position using the 10 th font at 15.5 points (1/72 Inch)    =*/
/*= with the OD_GC_UsrDraw graphic context is:                         =*/
/*=     // to do once                                                  =*/
/*=     Todstrgeo geom  = { 1000, 2000, 0,0,0, OD_SDO_WBL, 10, 15.5 }; =*/
/*=     od_str_extents(od, &geom, "hello");                            =*/
/*=                                                                    =*/
/*=     // set the graphic context                                     =*/
/*=     Tsty_text style = { 0.0, 10, 15.5, &black };                   =*/
/*=     od_str_setGC_byStyle(od,OD_GC_UsrDraw,&style);                 =*/
/*=                                                                    =*/
/*=     // draw the string                                             =*/
/*=     od_str_draw(od,OD_GC_UsrDraw,&geom,"hello", 0.0);              =*/
/*= At this point you can draw the same string at an other source      =*/
/*= position and from right to left.                                   =*/
/*=     geom->x = 1100; geom->y = 2100; g->of = OD_SORG_RIGHT;         =*/
/*=     od_str_draw(od,OD_GC_UsrDraw,&geom,"hello", 0.0);              =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_STRING_HL_H
#define FILE_OUTDEV_STRING_HL_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/color-types.h"
#include "tools/font-types.h"
#include "outdev/types.h"

/*======================================================================*/
/*= Drawing strings (GC & font settings)                               =*/

/**
 * The od_str_fontLoad function loads at the EGS level, the fi th font
 * sized to pts points (1/72 inch).
 * It is a wrapper to od->strVT.fondLoadPix/Pts.
 *
 * Return:
 *   FONTEGS_IsNone(return value) means that the font is not loaded.
 *  !FONTEGS_IsNone(return value) means that the font is loaded successfully.
 *   When font is loaded successfully, ratio contains the factor
 *   between the size of the asked font and loaded font.
 *     - ratio==1.0 means that the font is loaded with the asked size.
 *     - ratio!=1.0 means that the font is loaded with a different size
 *       than the asked size. We have: pts = ratio * loaded-font-size.
**/
extern TfontEGS od_str_fontLoad(Toutdev* od, int fi, double pts, double* ratio);

extern void od_str_fontUnload(Toutdev* od, TfontEGS f);

/**
 * The od_str_setGC_byStyle function sets the font
 * and the drawing color of the gcd th graphic context.
 *
 * The font is set to the dp->dpt_font th font of the device.
 * The font is sized to dp->dpt_pts points (1 point is 1/72 inch). 
 * The drawing color is set to the dp->dpt_color color.
 * The background color for string drawings is not modified.
**/
extern void od_str_setGC_byStyle(Toutdev* od, Tod_GCdef gcd, Csty_text*dp);
extern void od_str_setGC (Toutdev* od, Tod_GCdef gcd,
                          int fid, double pts, Ccolor*c);

extern void od_str_setGCP_byStyle(Toutdev* od, Tod_GCdef gcd, Csty_text*dp);
extern void od_str_setGCP (Toutdev* od, Tod_GCdef gcd,
                           int fid, double pts, Ccolor*c);

/*======================================================================*/
/*= Drawing strings (extents)                                          =*/

/**
 * The od_str_extents_DesUnit function does not draw anything.
 * It just returns the geometric parameters of the text string
 * drawn using the fi font at pts size (pts is in point).
 *  - ascent is the distance between the top border and the base line.
 *  - descent is the distance between the bottom border and the base line.
 *  - dx is the width of the text.
 *  - dy is the height of the text.
 * All the returned distance are given in pixel of the output device.
 *
 * The od_str_extents_SrcUnit function is similar to the 
 * od_str_extents_DesUnit function except that it returns the geometric
 * parameters in the data source unit.
 * 
 * The od_str_extents_DesUnit_gc and od_str_extents_SrcUnit_gc are 
 * similar to the formers except that they use the font and font size
 * of the gcd graphics context.
 *
 * You can set the ascent, descent, dx and dy pointers to the null
 * pointer when you do not need the corresponding  values.
 *
 * Return:
 *  The functions returns:
 *    - 0 the returned values have been calculated with the accurate
 *      geometric parameters of font.
 *    - 1 the returned values have been estimated using the
 *      od_str_extents_default function. This occurs only when no
 *      font driver is available like in the PDF output device.
**/
extern int od_str_extents_DesUnit(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy);
extern int od_str_extents_DesUnitI(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        int* ascent, int* descent,
                        int* dx,     int* dy);
extern int od_str_extents_SrcUnit(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy);

extern int od_str_extents_DesUnit_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy);
extern int od_str_extents_SrcUnit_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy);

extern Todstrgeo od_str_extents_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text, int sens, double x, double y);

/*======================================================================*/
/*= Drawing strings                                                    =*/

/**
 * The od_str_draw function draws into the od output device the text
 * string using the font and colors specified by the gcd graphic context.
 * The string is drawn at a (x.y) position that is computed using using
 * g.>x, g->y and g->of (see the Todstrgeo in outdev/types.h).
 *
 * If the a angle is not null, the drawing is rotated of a radians,
 * the rotation center being (x,y).
 *            +-----------------------------> x
 *            |         / a=-45 degree
 *            |        /  
 *            | (x,y) X--------------- a=0 degree
 *            V        \
 *            y         \ a=+45 degree
 *
 * Parameters:
 *   g->x, g->y: in source unit
 *   r         : in radian
 *   flag      : see OD_STRDRAW_... macros
**/
extern void od_str_draw(   Toutdev*  od, Tod_GCdef  gcd, Codstrgeo*g,
                           CstrBrief text, double a);

#define OD_STRDRAW_Default    0  // draw X or bounding box depending of size.
#define OD_STRDRAW_Rect       1  // Text bounding box (line or rectangle)
#define OD_STRDRAW_Xxx        2  // draw X instead of char
#define OD_STRDRAW_RectInk    4  // Inked text bonding box (line or rectangle)
#define OD_STRDRAW_Rect0      8  // Same as OD_STRDRAW_Rect    with a=0
#define OD_STRDRAW_Xxx0      16  // Same as OD_STRDRAW_Xxx     with a=0
#define OD_STRDRAW_RectInk0  32  // Same as OD_STRDRAW_RectInk with a=0
#define OD_STRDRAW_BaseLine  64  // draw text base line

/**
 * The od_str_drawXXX function is similar to the od_str_draw function
 * except that it writes a X characters instead of text characters or
 * text bounding boxes depending of the flag parameter.
 *
 * The standard use is with the OD_STRDRAW_Default (0) flag.
 * The other flags are only for debugging purposes.
 *
 * Note: The function does not change the gcd graphic context. It only
 * writes lines so line thickness, line join style must be set in the
 * gcd graphic context.
**/
extern void od_str_drawXXX(Toutdev*  od, Tod_GCdef  gcd, Codstrgeo*g,
                           CstrBrief text, double a, int flag);

/*======================================================================*/
/*= drawing strings (quick)                                            =*/

/**
 * The od_str_draw_LR/RL/CE functions draw the text string on the od
 * output device
 *  - at position (x,y) given in the source unit,
 *  - y is the base line,
 *  - x gives the left side (resp: right and center) of the text for the
 *    od_str_draw_LR function (resp: od_str_draw_RL/CE).
 *  - the font and the font size are those that the gcd th GC
 *    currently defines.
**/
extern void od_str_draw_LR(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y);
extern void od_str_draw_RL(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y);
extern void od_str_draw_CE(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y);
/**
 * The od_str_draw_IBxy function draws the text string on the od
 * output device inside a rectangle.
 *  - Its vertices are (x,y), (x+dx,y), (x+dx,y+dy) and (x,y+dy).
 *  - w gives the place in the rectangle (North, North-West, ...).
 *  - the font and the font size are those that the gcd GC
 *    currently defines.
 * NOTICE: The w place is interpreted assuming that the x axis is
 * from right to left hand and that the y axis is from top to bottom.
 *
 * The od_str_draw_IBxyg function is similar to the od_str_draw_IBxy
 * function but it uses the text geometric information found in the
 * structure pointed to by g.
 * The function sets g->of to OD_SORG_LEFT and updates g->pos. 
 *
 * The od_str_draw_IBox/g functions are similar to the od_str_draw_IBxy/g
 * functions except that the rectangle is given by a TboxDble* parameter.
 **/
extern void od_str_draw_IBxy (Toutdev*  od, Tod_GCdef gcd,
                              CstrBrief text, TdirWR w,
                              double x, double y, double dx, double dy);
extern void od_str_draw_IBxyg(Toutdev*  od, Tod_GCdef gcd, Todstrgeo*g,
                              CstrBrief text, TdirWR w,
                              double x, double y, double dx, double dy);
extern void od_str_draw_IBox (Toutdev*  od, Tod_GCdef  gcd,
                              CstrBrief text, TdirWR w, CboxDble* box);
extern void od_str_draw_IBoxg(Toutdev*  od, Tod_GCdef gcd, Todstrgeo*g,
                              CstrBrief text, TdirWR w, CboxDble* box);

#define BOX_EXT  double x, double y, double dx, double dy
Inline void od_str_draw_IBxyNN(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_NN,x,y,dx,dy); }
Inline void od_str_draw_IBxyNW(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_NW,x,y,dx,dy); }
Inline void od_str_draw_IBxyNE(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_NE,x,y,dx,dy); }
Inline void od_str_draw_IBxySS(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_SS,x,y,dx,dy); }
Inline void od_str_draw_IBxySW(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_SW,x,y,dx,dy); }
Inline void od_str_draw_IBxySE(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_SE,x,y,dx,dy); }
Inline void od_str_draw_IBxyWW(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_WW,x,y,dx,dy); }
Inline void od_str_draw_IBxyEE(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_EE,x,y,dx,dy); }
Inline void od_str_draw_IBxyCC(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, BOX_EXT) { od_str_draw_IBxy(od,gcd,text,DirWR_CC,x,y,dx,dy); }

Inline void od_str_draw_IBxyNNg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_NN,x,y,dx,dy); }
Inline void od_str_draw_IBxyNWg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_NW,x,y,dx,dy); }
Inline void od_str_draw_IBxyNEg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_NE,x,y,dx,dy); }
Inline void od_str_draw_IBxySSg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_SS,x,y,dx,dy); }
Inline void od_str_draw_IBxySWg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_SW,x,y,dx,dy); }
Inline void od_str_draw_IBxySEg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_SE,x,y,dx,dy); }
Inline void od_str_draw_IBxyWWg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_WW,x,y,dx,dy); }
Inline void od_str_draw_IBxyEEg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_EE,x,y,dx,dy); }
Inline void od_str_draw_IBxyCCg(Toutdev*  od, Tod_GCdef  gcd, Todstrgeo*g, CstrBrief text, BOX_EXT) { od_str_draw_IBxyg(od,gcd,g,text,DirWR_CC,x,y,dx,dy); }
#undef BOX_EXT


/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_STRING_HL_H
/*======================================================================*/
/*= vim: set textwidth=0:                                              =*/
/*======================================================================*/
