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
 *      $Id: color-types.h 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/color-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base types and macros of the color package.                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_COLOR_TYPES_H
#define FILE_TOOLS_COLOR_TYPES_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
typedef Tany      TcolorEGS;
typedef TcolorEGS Tegs_color; // obsolete
typedef TcolorEGS Tgui_color; // obsolete

typedef struct _TcolorRGB TcolorRGB;
typedef const   TcolorRGB CcolorRGB;

typedef struct _TcolorDriver TcolorDriver; 
typedef const   TcolorDriver CcolorDriver; 

typedef struct _Tcolor Tcolor;
typedef const   Tcolor Ccolor;
typedef const   Tcolor cTcolor; // obsolete

extern  TcolorEGS EGSCOLOR_none;
extern  TcolorEGS EGSCOLOR_default;
#define EGSCOLOR_NONE    ((signed long)-1) // color is undefined
#define EGSCOLOR_DEFAULT ((signed long) 0) // use the default color
#define COLOREGS_IsDefined(c) ( !COLOREGS_IsNone(c) && !COLOREGS_IsDefault(c) )
#define COLOREGS_IsNone(c)    ((c).ulong==EGSCOLOR_NONE)
#define COLOREGS_IsDefault(c) ((c).ulong==EGSCOLOR_DEFAULT)

#ifndef COLOR_RGB 
#define COLOR_RGB(r,g,b) "rgb:"#r"/"#g"/"#b
#endif

typedef struct _TcolorSet TcolorSet;
typedef const   TcolorSet CcolorSet;
typedef const   TcolorSet cTcolorSet; // objects use CcolorSet

/*======================================================================*/
/*= Debug                                                              =*/

// Returns a string containig the dump of c color.
// It uses 5 static buffer. Indeed, the i+5 th invocation, overwrites
// the buffer returned by the i th invocation.
const char* DBG_Rgb2str     (CcolorRGB*c);
const char* DBG_Color2strRGB(Ccolor*c);
const char* DBG_Color2str   (Ccolor*c);
#if defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr
extern Tcolor* dbg_clr;
extern void    dbg_color_new(Tcolor*c);
extern void    dbg_color_del(Tcolor*c);
extern void    dbg_color_printUnfreed();
#else
#define     dbg_color_new(c)           while(0)
#define     dbg_color_del(c)           while(0)
#define     dbg_color_printUnfreed()   while(0)
#endif // TOOL_DBG_MemClr

#define DBG0_Color(fmt,...)  DBG0_printf("color",fmt,__VA_ARGS__)
#define DBG1_Color(fmt,...)  DBG1_printf("color",fmt,__VA_ARGS__)
#define DBG2_Color(fmt,...)  DBG2_printf("color",fmt,__VA_ARGS__)

#define DBG0_ColorE(fmt,...) DBG0_printf("color","Enter : "fmt,__VA_ARGS__)
#define DBG0_ColorM(fmt,...) DBG0_printf("color","Middle: "fmt,__VA_ARGS__)
#define DBG0_ColorL(fmt,...) DBG0_printf("color","Leave : "fmt,__VA_ARGS__)
#define DBG1_ColorE(fmt,...) DBG1_printf("color","Enter : "fmt,__VA_ARGS__)
#define DBG1_ColorM(fmt,...) DBG1_printf("color","Middle: "fmt,__VA_ARGS__)
#define DBG1_ColorL(fmt,...) DBG1_printf("color","Leave : "fmt,__VA_ARGS__)
#define DBG2_ColorE(fmt,...) DBG2_printf("color","Enter : "fmt,__VA_ARGS__)
#define DBG2_ColorM(fmt,...) DBG2_printf("color","Middle: "fmt,__VA_ARGS__)
#define DBG2_ColorL(fmt,...) DBG2_printf("color","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_COLOR_TYPES_H
/*======================================================================*/

