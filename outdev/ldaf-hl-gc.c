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
 *      $Id: ldaf-hl-gc.c 142 2018-05-28 16:35:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/ldaf-hl-gc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the ldaf-gc.h header for documentation.                        =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/style.h"
#include "outdev/outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/ldaf-hl.h"
#include "outdev/resources.h"

#include "tools/color.h"

/*======================================================================*/

extern void od_ldaf_setGCsoP(Toutdev*od, Tod_GCdef gcd, double thp,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg)
{
    Tsty_line* topLine = od_style_topLine(od,gcd);
    TlineStyle ls = DA_LS_Solid;
    int dth = round( thp );
    if ( dth==0 ) dth = 1;
    if ( fg==0 )
        od->vtable->ldafVT.setGCso(od,gcd,dth,ls,js,cs,EGSCOLOR_none);
    else {
        od->vtable->ldafVT.setGCso(od,gcd,dth,ls,js,cs,fg->egs);
        color_chgLazy(& topLine->stl_color, fg);
    }
                        topLine->stl_thickness = thp;
                        topLine->stl_style     = DA_LS_Solid;
    if (js!=DA_JS_None) topLine->stl_join      = js;
    if (cs!=DA_CS_None) topLine->stl_cap       = cs;
}

extern void od_ldaf_setGCsoZ(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg)
{
    double zoom       = od_getUsrZoom(od); 
    double zthickness = thz*zoom ;
    DBG0GC(
     "setting GC %d, w=%f-->%f style=? join=%d cap=%d fg=%p",
      gcd,thz,zthickness, js,cs,fg);
    od_ldaf_setGCsoP(od,gcd, zthickness, js, cs, fg);
}

static void od_ldaf_setGCddI(double zoom,Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg,
                             TlineStyle ls, Ccolor* ddh)
{
    Tsty_line* topLine = od_style_topLine(od,gcd);
//printf("%s:0 top=%p : %s\n",__func__,topLine,DBG_STL2str(topLine));
    double zthickness = thz*zoom ;
    int    dth        = round( zthickness );
    if ( dth==0 ) dth = 1;
    if ( fg==0 ) {
        od->vtable->ldafVT.setGCso(od,gcd,dth,ls,js,cs,EGSCOLOR_none);
    } else {
        od->vtable->ldafVT.setGCso(od,gcd,dth,ls,js,cs,fg->egs);
        color_chgLazy(& topLine->stl_color, fg);
    }
                        topLine->stl_thickness = zthickness;
                        topLine->stl_style     = DA_LS_Solid;
    if (js!=DA_JS_None) topLine->stl_join      = js;
    if (cs!=DA_CS_None) topLine->stl_cap       = cs;
    
    // set attribut for dot or dash line if needed 
    if ( ls==DA_LS_None || ls==DA_LS_Solid )
        return ;
    od->vtable->ldafVT.setGCdd(od,gcd,dth,ls,EGSCOLOR_none);
    topLine->stl_style = ls;
}

extern void od_ldaf_setGCddP(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg,
                             TlineStyle ls, Ccolor* ddh)
{
    double zoom = 1;
    od_ldaf_setGCddI(zoom,od,gcd,thz,js,cs,fg,ls,ddh);
}
extern void od_ldaf_setGCddZ(Toutdev*od, Tod_GCdef gcd, double thz,
                             TjoinStyle js, TcapStyle  cs, Ccolor* fg,
                             TlineStyle ls, Ccolor* ddh)
{
    double zoom = od_getUsrZoom(od); 
    od_ldaf_setGCddI(zoom,od,gcd,thz,js,cs,fg,ls,ddh);
}

extern void od_ldaf_setGCll(Toutdev*od, Tod_GCdef gcd, double zf,
                            TfillStyle fs, Ccolor* c,
                            int sidx, int lidx, int pidx)
{
    Tsty_fill* topFill = od_style_topFill(od,gcd);
    TpixmapEGS  pxm;
    Cbitmap*    btm;
    Tcolor      *fg, *bg;
    Ccolor      *fgc,*bgc;

    DBG0GC("style=%s c=%s zf=%f s/l/p=%d/%d/%d",DBG_FS2str(fs),
            DBG_Color2str(c),zf,sidx,lidx,pidx);
    TEMP_asf_printf(fs==DA_FS_None,
            "None kind (0x%x) is invalid for this function",fs);

    if ( fs==DA_FS_NoFill )
        return;
    topFill->stf_style = fs;
    color_chgLazy(&topFill->stf_color,c);
    topFill->stf_shade = 0;
    topFill->stf_light = 0;
    topFill->stf_pattern = 0;
    if ( fs==DA_FS_Solid ) {
        od->vtable->ldafVT.setGCfillColor(od,gcd,c->egs);
        return;
    }
    if ( fs==DA_FS_ShadTile ) {
      topFill->stf_shade = sidx;
      if ( od->vtable->ldafVT.setGCfillColorBrightness ) {
        double coef = od_getShadCoef(od,sidx);
        od->vtable->ldafVT.setGCfillColorBrightness(od,gcd,c->egs,-coef);
        return;
      } else {
        fgc = c;
        bg  = od->vtable->colorVT.black(od);
        if ( zf==1 ) {
            DBG2GC("USE SHAD PXM %d (no zoom)",sidx);
            goto setGCShadTile;
        } else if ( od->vtable->ldafVT.SetGCfillTileZ!=0 ) {
            // let EGS to perform the zoom, it will do it better than me
            DBG2GC("USE SHAD PXM %d using EGS zoom (zf=%f)",sidx,zf);
            btm = od_getShadBtm(od,sidx,1.0);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,fgc,bg);
            od->vtable->ldafVT.SetGCfillTileZ(od,gcd,pxm,zf);
        } else {
            // we do the zoom.
            DBG2GC("USE SHAD PXM %d using YFIG zoom (zf=%f)",sidx,zf);
setGCShadTile:
            btm = od_getShadBtm(od,sidx,zf);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,fgc,bg);
            od->vtable->ldafVT.setGCfillTile(od,gcd,pxm);
        }
        od->vtable->pxmVT.pxm_delete(od,pxm);
        color_delete( bg );
        return;
      }
    }
    if ( fs==DA_FS_LighTile ) {
      topFill->stf_light = lidx;
      int idx = lidx;
      if ( od->vtable->ldafVT.setGCfillColorBrightness ) {
        double coef = od_getLighCoef(od,idx);
        od->vtable->ldafVT.setGCfillColorBrightness(od,gcd,c->egs,coef);
        return;
      } else {
        fgc = c;
        bg  = od->vtable->colorVT.white(od);
        if ( zf==1 ) {
            DBG2GC("USE LIGHT PXM %d (no zoom)",idx);
            goto setGCLighTile;
        } else if ( od->vtable->ldafVT.SetGCfillTileZ!=0 ) {
            // let EGS to perform the zoom, it will do it better than me
            DBG2GC("USE LIGHT PXM %d using EGS zoom (zf=%f)",idx,zf);
            btm = od_getLighBtm(od,idx,1.0);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,bg,fgc);
            od->vtable->ldafVT.SetGCfillTileZ(od,gcd,pxm,zf);
        } else {
            // we do the zoom.
            DBG2GC("USE LIGHT PXM %d using YFIG zoom (zf=%f)",idx,zf);
setGCLighTile:
            btm = od_getLighBtm(od,idx,zf);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,fgc,bg);
            od->vtable->ldafVT.setGCfillTile(od,gcd,pxm);
        }
        od->vtable->pxmVT.pxm_delete(od,pxm);
        color_delete( bg );
        return;
      }
    }
    if ( fs==DA_FS_PattTile ) {
        topFill->stf_pattern = pidx;
        fg  = od->vtable->colorVT.black(od);
        bgc = c;
        if ( zf==1 ) {
            DBG2GC("USE PATTERN PXM %d (no zoom)",pidx);
            goto setGCfillTile;
        } else if ( od->vtable->ldafVT.SetGCfillTileZ!=0 ) {
            // let EGS to perform the zoom, it will do it better than me
            DBG2GC("USE PATTERN PXM %d using EGS zoom (zf=%f)",pidx,zf);
            btm = od_getPattBtm(od,pidx,1.0);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,fg,bgc);
            od->vtable->ldafVT.SetGCfillTileZ(od,gcd,pxm,zf);
        } else {
            // we do the zoom.
            DBG2GC("USE PATTERN PXM %d using YFIG zoom (zf=%f)",pidx,zf);
setGCfillTile:
            btm = od_getPattBtm(od,pidx,zf);
            pxm = od->vtable->pxmVT.pxm_newFromBtm(od,btm,fg,bgc);
            od->vtable->ldafVT.setGCfillTile(od,gcd,pxm);
        }
        od->vtable->pxmVT.pxm_delete(od,pxm);
        color_delete( fg );
        return;
    }
    TEMP_spe_printf("unexpected fill kind (0x%x)",fs);
}

/*======================================================================*/

extern void od_ldaf_setGCsoP_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s)
{ od_ldaf_setGCsoP(od,gcd,s->dpl_thickness,
        s->dpl_join,s->dpl_cap, s->dpl_color); }

extern void od_ldaf_setGCsoZ_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s)
{ od_ldaf_setGCsoZ(od,gcd,s->dpl_thickness,
        s->dpl_join,s->dpl_cap, s->dpl_color); }

extern void od_ldaf_setGCddP_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s)
{ od_ldaf_setGCddP(od,gcd,s->dpl_thickness,
        s->dpl_join,s->dpl_cap, s->dpl_color,
        s->dpl_style, NULL); }
extern void od_ldaf_setGCddZ_byStyle(Toutdev*od, Tod_GCdef gcd, Csty_line* s)
{ od_ldaf_setGCddZ(od,gcd,s->dpl_thickness,
        s->dpl_join,s->dpl_cap, s->dpl_color,
        s->dpl_style, NULL); }

extern void od_ldaf_setGCll_byStyle(Toutdev*od, Tod_GCdef gcd, double zf,
                                    Csty_fill* s)
{
    od_ldaf_setGCll(od,gcd,zf,s->dpf_style,s->dpf_color,
            s->dpf_shade,s->dpf_light,s->dpf_pattern);
}

/*======================================================================*/
