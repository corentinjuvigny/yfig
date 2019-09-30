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
 *      $Id: style-dbg.c 367 2018-11-18 23:50:25Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/style-dbg.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Implementation of the DBG_LS2str, DBG_JS2str, DBG_CS2str,          =*/
/*= DBG_FS2str functions.                                              =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/style.h"

#include "tools/math-macros.h"

/*======================================================================*/

extern const char* DBG_LS2str(TlineStyle s)
{
    switch (s) {
        case DA_LS_Solid    : return "Solid";
        case DA_LS_Dash     : return "Dash";
        case DA_LS_Dot      : return "Dot";
        case DA_LS_Dash1Dot : return "Dash1Dot";
        case DA_LS_Dash2Dot : return "Dash2Dot";
        case DA_LS_Dash3Dot : return "Dash3Dot";
        case DA_LS_None     : return "None";
        default             : return "????";  
    }
}

extern const char* DBG_JS2str(TjoinStyle s)
{
    switch (s) {
        case DA_JS_Miter : return "Miter";
        case DA_JS_Round : return "Round";
        case DA_JS_Bevel : return "Bevel";
        case DA_JS_None  : return "None";
        default          : return "????";  
    }
}

extern const char* DBG_CS2str(TcapStyle  s)
{
    switch (s) {
        case DA_CS_Butt  : return "Butt";
        case DA_CS_Round : return "Round";
        case DA_CS_Proj  : return "Proj";
        case DA_CS_None  : return "None";
        default          : return "????";  
    }
}

extern const char* DBG_FS2str(TfillStyle s)
{
    switch (s) {
        case DA_FS_NoFill  : return "NoFill";
        case DA_FS_Solid   : return "Solid";
        case DA_FS_ShadTile: return "Shade";
        case DA_FS_LighTile: return "Light";
        case DA_FS_PattTile: return "Pattern";
        case DA_FS_None    : return "None";
        default            : return "????";  
    }
}

/*======================================================================*/

#define NBBUF 5

typedef char Tbuffer[256];

static Tbuffer buffers[NBBUF];
static int nextbuffer=0;

extern const char* DBG_STL2str(Csty_line* s)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    sprintf(buffer, "th=%d style=%d(%s) js=%d(%s) cs=%d(%s) c=%s", 
            s->stl_thickness,
            s->stl_style, DBG_LS2str(s->stl_style),
            s->stl_join,  DBG_JS2str(s->stl_join),
            s->stl_cap,   DBG_CS2str(s->stl_cap),
            DBG_Color2str(s->stl_color));
    return buffer;
}

extern const char* DBG_STF2str(Csty_fill* s)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    sprintf(buffer, "style=%s c=%s sha=%d lig=%d pat=%d", 
            DBG_FS2str(s->stf_style), DBG_Color2str(s->stf_color),
            s->stf_shade, s->stf_light, s->stf_pattern);
    return buffer;
}

extern const char* DBG_STT2str(Csty_text* s)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    sprintf(buffer, "angle=%.1f font=%d pts=%.2f color=%s", 
            RadToDeg(s->stt_angle), s->stt_font, s->stt_pts,
            DBG_Color2str(s->stt_color));
    return buffer;
}

/*======================================================================*/
