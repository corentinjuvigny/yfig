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
 *      $Id: outdevTex-priv.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/latex/outdevTex-priv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities.                                           =*/
/*=                                                                    =*/
/*= See outdevPdf-priv.h header for documentation.                     =*/
/*======================================================================*/

#include "outdevTex-priv.h"
#include "outdev/resources.h"

/*======================================================================*/
/*= GC handler                                                         =*/                            

#include "outdev/style.h"
char* odtex_gc_getColorRgb8(char*buffer, TodTex*od, CcolorRGB*rgb)
{
//    if ( rgb->red==0 && rgb->green==0 && rgb->blue==0 ) {
//        sprintf(buffer,"black");
//    } else {
        sprintf(buffer,"\\definecolor{yfigclr}{RGB}{%3d,%3d,%3d}",rgb->red,rgb->green,rgb->blue);
//    }
    return buffer;
}
char* odtex_gc_getColor(char*buffer, TodTex*od, Ccolor*clr) { return odtex_gc_getColorRgb8(buffer,od,&clr->rgb); }

TstrEaten odtex_gc_get(char*clr,TodTex*od,Tod_GCdef gcd)
{
    char style[200];
    if ( gcd==OD_GC_UsrDraw || gcd==OD_GC_MissDraw ) {
        Csty_line* stl = od_style_topLine(&od->any,gcd);
        switch ( stl->stl_style ) {
            case DA_LS_Solid: strcpy(style,"");       break;
            case DA_LS_Dot:   strcpy(style,"dotted"); break;
            case DA_LS_Dash:  strcpy(style,"dashed"); break;
            default:          strcpy(style,"dashed"); break;
        }
        odtex_gc_getColor(clr,od,stl->stl_color);
        return tool_aprintf("draw=yfigclr,line width=%.1fpt%s",
                (double)stl->stl_thickness, style[0]==0 ? "" : ",",style);
    } else if ( gcd==OD_GC_UsrFill || gcd==OD_GC_MissFill ) {
        double coef; TcolorRGB rgb8;
        Csty_fill* stf = od_style_topFill(&od->any,gcd);
        switch ( stf->stf_style ) {
            case DA_FS_Solid    : odtex_gc_getColor(clr,od,stf->stf_color); break;
            case DA_FS_ShadTile : coef = od_getShadCoef(&od->any,stf->stf_shade);
                                  rgb8 = stf->stf_color->rgb;
                                  rgb8.red   += (rgb8.red  ) * coef;
                                  rgb8.green += (rgb8.green) * coef;
                                  rgb8.blue  += (rgb8.blue ) * coef;
                                  odtex_gc_getColorRgb8(clr,od,&rgb8);
                                  break;
            case DA_FS_LighTile : coef = od_getLighCoef(&od->any,stf->stf_light);
                                  rgb8 = stf->stf_color->rgb;
                                  rgb8.red   += (255-rgb8.red  ) * coef;
                                  rgb8.green += (255-rgb8.green) * coef;
                                  rgb8.blue  += (255-rgb8.blue ) * coef;
                                  odtex_gc_getColorRgb8(clr,od,&rgb8);
                                  break;
            default             :
            case DA_FS_PattTile : tool_errerr_printf(od->any.msgdrv,TEX_MESS("tiling not supported, changed to solid color"));
                                  odtex_gc_getColor(clr,od,stf->stf_color); break;
        }
        return tool_aprintf("fill=yfigclr",clr);
    } else {
        TEMP_spe_printf("unexpected gcd=%d",gcd);
        return NULL;
    }
}


/*======================================================================*/
