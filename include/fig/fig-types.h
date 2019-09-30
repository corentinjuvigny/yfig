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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: fig-types.h 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/fig-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base types and macros of FIG package.                              =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_FIG_TYPES_H
#define FILE_FIG_TYPES_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"
#include "outdev/style.h"

/*======================================================================*/
/*= The macros                                                         =*/

#define FIG_MIN_DEPTH    0
#define FIG_MAX_DEPTH 1000

#define FIG_THICKNESS_MIN   0   // 1    pixel of the the output device
#define FIG_THICKNESS_MAX 100   // 100  pixel of the the output device

/*======================================================================*/
/*= Object kind mask                                                   =*/

// a table of virtual tables (VTT means Virtual Table Table).
// The FIG API assumes that the FIG_KD_... identifier is the index in a VTT
typedef void** TobjVTT;

#define FIG_KDM_ARC 0x00000001 
#define FIG_KDM_ELL 0x00000002 
#define FIG_KDM_LIN 0x00000010 
#define FIG_KDM_SPL 0x00000020 
#define FIG_KDM_TEX 0x00000400 
#define FIG_KDM_DIR 0x00000800 
#define FIG_KDM_BHV 0x00001000 
#define FIG_KDM_AOW 0x00100000  // can have arrow
#define FIG_KDM_ALL 0x00ffffff

#define FSC_MM_ARC FIG_KDM_ARC 
#define FSC_MM_ELL FIG_KDM_ELL 
#define FSC_MM_LIN FIG_KDM_LIN 
#define FSC_MM_SPL FIG_KDM_SPL 
#define FSC_MM_TEX FIG_KDM_TEX 
#define FSC_MM_DIR FIG_KDM_DIR 
#define FSC_MM_BHV FIG_KDM_BHV
#define FSC_MM_ALL FIG_KDM_ALL

/*======================================================================*/
/*= The types                                                          =*/

#include "tools/color-types.h"

typedef struct _Tfigure Tfigure;
typedef const   Tfigure Cfigure;

typedef struct _Tfig_objCounters  Tfig_objCounters;
typedef const   Tfig_objCounters  Cfig_objCounters;

typedef struct _Tfig_colors  Tfig_colors;
typedef const   Tfig_colors  Cfig_colors;

typedef struct _Tfig_scene  Tfig_scene;
typedef const   Tfig_scene  Cfig_scene;

typedef struct _Tfig_histo  Tfig_histo;
typedef const   Tfig_histo  Cfig_histo;
typedef struct _Tfig_hele   Tfig_hele;
typedef const   Tfig_hele   Cfig_hele;

typedef struct _TobjCompound* TobjDir;
typedef const   TobjDir       CobjDir;

typedef struct _TobjVtable TobjVtable;
typedef const   TobjVtable CobjVtable;

typedef struct _TobjAny TobjAny;
typedef const   TobjAny CobjAny;

typedef struct _TobjArc TobjArc;
typedef const   TobjArc CobjArc;

typedef struct _TobjEllipse TobjEllipse;
typedef const   TobjEllipse CobjEllipse;

typedef struct _TobjLine TobjLine;
typedef const   TobjLine CobjLine;

typedef struct _TobjSpline TobjSpline;
typedef const   TobjSpline CobjSpline;

typedef struct _TobjText TobjText;
typedef const   TobjText CobjText;

typedef struct _TobjBoxhv TobjBoxhv;
typedef const   TobjBoxhv CobjBoxhv;

typedef struct _TobjArrow TobjArrow;
typedef const   TobjArrow CobjArrow;

typedef struct _TobjCompound TobjCompound;
typedef const   TobjCompound CobjCompound;

typedef struct _TobjAny*   Tobject;
typedef const   Tobject    Cobject;
typedef union  _TobjAll    TobjAll;
typedef const   TobjAll    CobjAll;
typedef struct _TobjAllSet TobjAllSet;
typedef const   TobjAllSet CobjAllSet;
typedef struct _TobjectSet TobjectSet;
typedef const   TobjectSet CobjectSet;
union _TobjAll {
    TobjAny*      any;
    TobjArc*      arc;
    TobjEllipse*  ell;
    TobjLine*     line;
    TobjSpline*   spline;
    TobjText*     text;
    TobjBoxhv*    boxhv;
    TobjArrow*    arrow;
    TobjCompound* compound;
    TobjDir       dir;
};
extern  TobjAll OBJ_none;
#define OBJ_IsNone(oa)    (oa).any==0
#define OBJ_IsDefined(oa) (oa).any!=0

// drawing parameters
typedef struct _Tfdp       Tfdp;
typedef const   Tfdp       Cfdp;
struct _Tfdp {
    Tstyle      fdp_style;
    int         fdp_depth; // drawing layer 
    int         fdp_dir;   // direction for text (see STY_T_...)
    TobjVTT     fdp_vtt;   // Table of object Virtual Table      
#define         fdp_text   fdp_style.sty_text
#define         fdp_line   fdp_style.sty_line
#define         fdp_fill   fdp_style.sty_fill
};
#define STY_T_LEFT    0  // position is at the left hand of the text
#define STY_T_CENTER  1  // position is in middle of the text
#define STY_T_RIGHT   2  // position is at the right hand of the text

// Mask of extra parameters
#define FDP_G_depth FDP_U_01
#define FDP_G_dir   FDP_U_02

// Used in YFIG, defined here avoid conflict, see yfig-drawparam.h
#define FDP_A_mode      FDP_U_10
#define FDP_A_beg       FDP_U_20
#define FDP_A_end       FDP_U_40
#define FDP_A_all  (FDP_A_mode|FDP_A_beg|FDP_A_end)

// Miscellaneous
typedef       Tsint32 Tscdepth;
typedef const Tsint32 Cscdepth;

/*======================================================================*/
/*= Debug                                                              =*/
#if defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj
extern TobjAll dbg_obj;
extern void    dbg_obj_new(TobjAll obj);
extern void    dbg_obj_del(TobjAll obj);
extern void    dbg_obj_printUnfreed();
#else
#define     dbg_obj_add(obj)         while(0)
#define     dbg_obj_printUnfreed()   while(0)
#endif // TOOL_DBG_MemObj

#define DBG0_DrawE(fmt,...) DBG0_printf("draw","Enter : "fmt,__VA_ARGS__)
#define DBG0_DrawM(fmt,...) DBG0_printf("draw","      : "fmt,__VA_ARGS__)
#define DBG0_DrawL(fmt,...) DBG0_printf("draw","Leave : "fmt,__VA_ARGS__)
#define DBG1_DrawE(fmt,...) DBG1_printf("draw","Enter : "fmt,__VA_ARGS__)
#define DBG1_DrawM(fmt,...) DBG1_printf("draw","      : "fmt,__VA_ARGS__)
#define DBG1_DrawL(fmt,...) DBG1_printf("draw","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_FIG_TYPES_H
/*======================================================================*/
