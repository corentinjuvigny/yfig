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
 *      $Id: font-types.h 339 2018-11-06 14:08:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/font-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base types and macros of the font package.                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_FONT_TYPES_H
#define FILE_TOOLS_FONT_TYPES_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/

#define FONT_NB  35

/*======================================================================*/

typedef struct _TfontDef  TfontDef;
typedef const   TfontDef  CfontDef;
typedef struct _TfontInfo TfontInfo;
typedef const   TfontInfo CfontInfo;

typedef struct _TfontDefSet  TfontDefSet;
typedef const   TfontDefSet  CfontDefSet;

typedef struct _TfontInfoSet  TfontInfoSet;
typedef const   TfontInfoSet  CfontInfoSet;

/*======================================================================*/

typedef union {
    unsigned long ulong;
    signed   long slong;
    void*         ptr;
    unsigned int  uint;
    signed   int  sint;
} TfontEGS;

extern  TfontEGS          FONTEGS_none;
#define FONTEGS_NONE      ((signed long)-1) // font is undefined
#define FONTEGS_IsNone(f)    ((f).ulong==FONTEGS_NONE)
#define FONTEGS_IsDefined(f) ((f).ulong!=FONTEGS_NONE)

typedef void (*TfontEGSunload)(void*od,TfontEGS f);

/*======================================================================*/
/*= Debug                                                              =*/

#if defined(TOOL_DBG_MemFnt) && TOOL_DBG_MemFnt
extern TfontInfo*     dbg_fontinfo;
extern void           dbg_fontinfo_new(TfontInfo*fi);
extern void           dbg_fontinfo_del(TfontInfo*fi);
extern void           dbg_fontinfo_printUnfreed();
extern TfontInfoSet*  dbg_fontset;
extern void           dbg_fontset_new(TfontInfoSet*s);
extern void           dbg_fontset_del(TfontInfoSet*s);
extern void           dbg_fontset_printUnfreed();
#else
#define          dbg_fontinfo_new(fi)           while(0)
#define          dbg_fontinfo_del(fi)           while(0)
#define          dbg_fontinfo_printUnfreed()    while(0)
#define          dbg_fontset_new(fi)            while(0)
#define          dbg_fontset_del(fi)            while(0)
#define          dbg_fontset_printUnfreed()    while(0)
#endif // TOOL_DBG_MemFnt

#define DBG0_Font(fmt,...)  DBG0_printf("font",fmt,__VA_ARGS__)
#define DBG1_Font(fmt,...)  DBG1_printf("font",fmt,__VA_ARGS__)
#define DBG2_Font(fmt,...)  DBG2_printf("font",fmt,__VA_ARGS__)

#define DBG0_FontE(fmt,...) DBG0_printf("font","Enter : " fmt,__VA_ARGS__)
#define DBG0_FontM(fmt,...) DBG0_printf("font","Middle: " fmt,__VA_ARGS__)
#define DBG0_FontL(fmt,...) DBG0_printf("font","Leave : " fmt,__VA_ARGS__)
#define DBG1_FontE(fmt,...) DBG1_printf("font","Enter : " fmt,__VA_ARGS__)
#define DBG1_FontM(fmt,...) DBG1_printf("font","Middle: " fmt,__VA_ARGS__)
#define DBG1_FontL(fmt,...) DBG1_printf("font","Leave : " fmt,__VA_ARGS__)
#define DBG2_FontE(fmt,...) DBG2_printf("font","Enter : " fmt,__VA_ARGS__)
#define DBG2_FontM(fmt,...) DBG2_printf("font","Middle: " fmt,__VA_ARGS__)
#define DBG2_FontL(fmt,...) DBG2_printf("font","Leave : " fmt,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_FONT_TYPES_H
/*======================================================================*/

