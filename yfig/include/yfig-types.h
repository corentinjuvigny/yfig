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
 *$Software: YFIG
 *      $Id: yfig-types.h 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= type pre-definitions and main macros.                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YGUI_TYPES_H
#define FILE_YGUI_TYPES_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "fig/fig-types.h"
#include "vgui/vgui-types.h"

/*======================================================================*/

typedef struct _Tyfig_dsds   Tyfig_dsds;
typedef const   Tyfig_dsds   Cyfig_dsds;

typedef struct _Tyfig_mw     Tyfig_mw;
typedef const   Tyfig_mw     Cyfig_mw;
Inline Tvgui*      yfig_mw_getVGui(Cyfig_mw*ymw) { return vgui_mw_getVGui((Cvgui_mw*)ymw); }
extern Tyfig_dsds* yfig_mw_getDsds(Cyfig_mw*ymw);
extern CcolorSet*  yfig_mw_getClrs(Cyfig_mw*ymw);
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_usridt_printf,yfig_mw_usridt_vprintf,tool_usridt_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_usrmsg_printf,yfig_mw_usrmsg_vprintf,tool_usrmsg_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_inferr_printf,yfig_mw_inferr_vprintf,tool_inferr_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_wrnerr_printf,yfig_mw_wrnerr_vprintf,tool_wrnerr_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_errerr_printf,yfig_mw_errerr_vprintf,tool_errerr_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));
TOOL_MSGDRV_FandVF_redirect(Cyfig_mw,yfig_mw_ftlerr_printf,yfig_mw_ftlerr_vprintf,tool_ftlerr_vprintf,vgui_mw_getMsgDrv((Cvgui_mw*)o));

typedef struct _Tyfig_device Tyfig_device;
typedef const   Tyfig_device Cyfig_device;
extern Tyfig_mw* yfig_device_getMw(Cyfig_device*ydev);

/*======================================================================*/

typedef struct _TarrowDesc TarrowDesc;
typedef const   TarrowDesc CarrowDesc;

#define FARROW_FillParent 0  // fill style from parent
#define FARROW_FillNone   1  // no fill style
#define FARROW_FillWhite  2  // white solid fill style

struct _TarrowDesc {
    int    enabled;
    int    abslength; 
    double length;    // length of arrow in point (1/72 Inch)
    double angle;
    int    vtidx;     // figure object vtable is obj_vtable[vtidx]
    int    flag;      // see FARROW_Fill... macros
    double thickness; // line thickness (in point) when abslength is 1
};

/*======================================================================*/

typedef struct _TexportOption TexportOption;
typedef const   TexportOption CexportOption;

/*======================================================================*/

#define DBG0DRAW(fmt,...)   DBG0_printf("draw",fmt,__VA_ARGS__)
#define DBG1DRAW(fmt,...)   DBG1_printf("draw",fmt,__VA_ARGS__)
#define DBG2DRAW(fmt,...)   DBG2_printf("draw",fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YGUI_TYPES_H
/*======================================================================*/

