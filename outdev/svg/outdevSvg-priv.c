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
 *      $Id: outdevSvg-priv.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/svg/outdevSvg-priv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities.                                           =*/
/*======================================================================*/

#include "outdevSvg-priv.h"
#include "outdev/style.h"
#include "outdev/resources.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= SVG color                                                          =*/                            

static char* odsvg_getColorRgb8(char*buffer, TodSvg*od, CcolorRGB*rgb)
{
    if ( rgb->red==0 && rgb->green==0 && rgb->blue==0 ) {
        sprintf(buffer,"black");
    } else {
        sprintf(buffer,"#%02x%02x%02x",rgb->red,rgb->green,rgb->blue);
    }
    return buffer;
}
char* odsvg_getColor(char*buffer, TodSvg*od, Ccolor*clr) { return odsvg_getColorRgb8(buffer,od,&clr->rgb); }

/*======================================================================*/
/*= style for shape and shape filling                                  =*/                            

static TstrEaten odsvg_getStyle(TodSvg*od,Tod_GCdef gcd, int style)
{
    char clr[200], linestyle[200], cap[200], join[200];
    if ( gcd==OD_GC_UsrDraw || gcd==OD_GC_MissDraw ) {
        Tsty_line stl = *od_style_topLine(&od->any,gcd);
        switch ( stl.stl_style ) {
            case DA_LS_Solid:    strcpy (linestyle,""); break;
            case DA_LS_Dot:      if (style==0) sprintf(linestyle," stroke-dasharray=\"%.4f,%.4f\"",           1*PT1, 3*PT1);
                                 else          sprintf(linestyle," stroke-dasharray:%.4f,%.4f;",              1*PT1, 3*PT1);
                                 stl.stl_cap=DA_CS_Round;
                                 break;
            case DA_LS_Dash:     if (style==0) sprintf(linestyle," stroke-dasharray=\"%.4f,%.4f\"",           5*PT1, 7*PT1);
                                 else          sprintf(linestyle," stroke-dasharray:\"%.4f,%.4f;",            5*PT1, 7*PT1);
                                 break;
            case DA_LS_Dash1Dot: if (style==0) sprintf(linestyle," stroke-dasharray=\"%.4f,%.4f,%.4f,%.4f\"", 5*PT1, 3*PT1, 1*PT1, 3*PT1);
                                 else          sprintf(linestyle," stroke-dasharray:\"%.4f,%.4f,%.4f,%.4f;",  5*PT1, 3*PT1, 1*PT1, 3*PT1);
                                 stl.stl_cap=DA_CS_Round;
                                 break;
            default:             if (style==0) sprintf(linestyle," stroke-dasharray=\"%.4f,%.4f,%.4f,%.4f\"", 5*PT1, 3*PT1, 1*PT1, 3*PT1);
                                 else          sprintf(linestyle," stroke-dasharray:\"%.4f,%.4f,%.4f,%.4f;",  5*PT1, 3*PT1, 1*PT1, 3*PT1);
                                 stl.stl_cap=DA_CS_Round;
                                 break;
        }
        switch ( stl.stl_cap ) {
            case DA_CS_Butt : if (style==0) sprintf(cap," stroke-linecap=\"butt\"");    else sprintf(cap," stroke-linecap:butt;");    break;
            case DA_CS_Proj : if (style==0) sprintf(cap," stroke-linecap=\"square\"");  else sprintf(cap," stroke-linecap:square;");  break;
            default         :
            case DA_CS_Round: if (style==0) sprintf(cap," stroke-linecap=\"round\"");   else sprintf(cap," stroke-linecap:round;");   break;
        }
        switch ( stl.stl_join ) {
            case DA_JS_Miter: if (style==0) sprintf(join," stroke-linejoin=\"miter\""); else sprintf(join," stroke-linejoin:miter;"); break;
            case DA_JS_Bevel: if (style==0) sprintf(join," stroke-linejoin=\"bevel\""); else sprintf(join," stroke-linejoin:bevel;"); break;
            default         :
            case DA_JS_Round: if (style==0) sprintf(join," stroke-linejoin=\"round\""); else sprintf(join," stroke-linejoin:round;"); break;
        }
        odsvg_getColor(clr,od,stl.stl_color);
        if ( style==0 )
            return tool_aprintf(
                "fill=\"none\" stroke=\"%s\" stroke-width=\"%.4f\""
                "%s%s%s",
                clr,((double)stl.stl_thickness)*PT1,linestyle,cap,join
               );
        else
            return tool_aprintf(
                "fill:none;stroke:%s;stroke-width:%.4f;"
                "%s%s%s",
                clr,((double)stl.stl_thickness)*PT1,linestyle,cap,join
               );
    } else if ( gcd==OD_GC_UsrFill || gcd==OD_GC_MissFill) {
        double coef; TcolorRGB rgb8;
        Csty_fill* stf = od_style_topFill(&od->any,gcd);
        switch ( stf->stf_style ) {
            case DA_FS_Solid    : odsvg_getColor(clr,od,stf->stf_color); break;
            case DA_FS_ShadTile : coef = od_getShadCoef(&od->any,stf->stf_shade);
                                  rgb8 = stf->stf_color->rgb;
                                  rgb8.red   += (rgb8.red  ) * coef;
                                  rgb8.green += (rgb8.green) * coef;
                                  rgb8.blue  += (rgb8.blue ) * coef;
                                  odsvg_getColorRgb8(clr,od,&rgb8);
                                  break;
            case DA_FS_LighTile : coef = od_getLighCoef(&od->any,stf->stf_light);
                                  rgb8 = stf->stf_color->rgb;
                                  rgb8.red   += (255-rgb8.red  ) * coef;
                                  rgb8.green += (255-rgb8.green) * coef;
                                  rgb8.blue  += (255-rgb8.blue ) * coef;
                                  odsvg_getColorRgb8(clr,od,&rgb8);
                                  break;
            default             :
            case DA_FS_PattTile : tool_errerr_printf(od->any.msgdrv,SVG_MESS("Tiling not supported, changed to solid color"));
                                  odsvg_getColor(clr,od,stf->stf_color); break;
        }
        if ( style==0 )
            return tool_aprintf("fill=\"%s\" stroke=\"none\"",clr);
        else
            return tool_aprintf("fill:%s;stroke:none;",clr);
    } else {
        TEMP_spe_printf("unexpected gcd=%d",gcd);
        return NULL;
    }
}

extern TstrEaten odsvg_getStyle0(TodSvg*od,Tod_GCdef gcd) { return odsvg_getStyle(od,gcd,0); }
extern TstrEaten odsvg_getStyle1(TodSvg*od,Tod_GCdef gcd) { return odsvg_getStyle(od,gcd,1); }

/*======================================================================*/
