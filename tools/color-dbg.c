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
 *      $Id: color-dbg.c 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/color-dbg.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Color package.                                                     =*/
/*=                                                                    =*/
/*= Implementation of the DBG_Color2str function.                      =*/
/*=                                                                    =*/
/*= See the color-types.h and color.h headers for documentation.       =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/color.h"

/*======================================================================*/

#define NBBUF 5

typedef char Tbuffer[32];

static Tbuffer buffers[NBBUF];
static int nextbuffer=0;

const char* DBG_Rgb2str(CcolorRGB*c)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    if ( c!=0 )
        sprintf(buffer, "#%04x.%04x.%04x", c->red, c->green, c->blue);
    else
        sprintf(buffer, "#%-14s", "undefined");
    return buffer;
}

const char* DBG_Color2strRGB(Ccolor*c)
{
    DBG_Rgb2str( c!=0 ? &c->rgb : NULL );
}

const char* DBG_Color2str(Ccolor*c)
{
    char* buffer = buffers[nextbuffer];
    nextbuffer = nextbuffer==(NBBUF-1) ? 0 : nextbuffer+1 ;
    if ( c!=0 )
        sprintf(buffer, "#%04x.%04x.%04x cnt=%d egs=%c",
            c->rgb.red, c->rgb.green, c->rgb.blue, c->cnt,
            COLOREGS_IsNone(c->egs) ? 'N' : 
            COLOREGS_IsDefault(c->egs) ? 'D' : 'I'
        );
    else
        sprintf(buffer, "nil");
    return buffer;
}

/*======================================================================*/

#if defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr

#define DBG_color_unfreedMAX 1000

Tcolor* dbg_color_unfreedT[DBG_color_unfreedMAX];
int     dbg_color_unfreedN;

extern void dbg_color_new(Tcolor*c)
{
    if (c==0) return;
    int i;
    for (i=0 ; i<dbg_color_unfreedN ; i++) {
        if ( dbg_color_unfreedT[i]==c ) {
            return ;
        }
    }
    // c not found in dbg_color_unfreedT, we add it if refcount is not 1
    if (dbg_color_unfreedN==DBG_color_unfreedMAX) return;
    dbg_color_unfreedT[dbg_color_unfreedN++] = c;
}

extern void dbg_color_del(Tcolor*c)
{
    if (c==0) return;
    int i;
    for (i=0 ; i<dbg_color_unfreedN ; i++) {
        if ( dbg_color_unfreedT[i]==c ) {
            if ( c->cnt==1 ) {
                dbg_color_unfreedN -= 1;
                dbg_color_unfreedT[i] = dbg_color_unfreedT[dbg_color_unfreedN];
            }
            return ;
        }
    }
    // c not found in dbg_color_unfreedT, we add it if refcount is not 1
    if ( c->cnt==1 ) return;
    if (dbg_color_unfreedN==DBG_color_unfreedMAX) return;
    dbg_color_unfreedT[dbg_color_unfreedN++] = c;
}

extern void dbg_color_printUnfreed()
{
    int i;
    for (i=0 ; i<dbg_color_unfreedN ; i++) {
        Tcolor* c = dbg_color_unfreedT[i];
        PERM_spe_printf(" %2d/%d: color=%p: refcount is %d expected 0 : %s",
            i,dbg_color_unfreedN, c, c->cnt, DBG_Color2str(c));
    }
}

#endif // TOOL_DBG_MemClr

/*======================================================================*/
