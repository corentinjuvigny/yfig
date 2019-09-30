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
 *      $Id: outdev.c 278 2018-07-05 14:48:18Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdev.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See outdev.h header for documentation.                             =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/src-vt.h"
#include "outdev/resources.h"
#include "outdev/style.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"

#include "tools/lenunit.h"

/*======================================================================*/
/*= Constructors & Destructor                                          =*/

extern Toutdev* od_new0 (Toutdev_vt*vt, double ppiDes, CboxDble*boxDes)
{
    Toutdev* od = tool_malloc_0(vt->topVT.byteSize);
    od->vtable = vt;
    od->ppiDes = ppiDes;
    if ( boxDes!=0 ) {
        Box_dx(od->boxDes) = Box_dx(*boxDes);
        Box_dy(od->boxDes) = Box_dy(*boxDes);
    } else {
        Box_dx(od->boxDes) = 100;
        Box_dy(od->boxDes) = 100;
    }
}

extern Toutdev* od_new0r (Toutdev_vt*vt, double ppiDes, CboxDble*boxDes,
                          TodResources* r)
{
    Toutdev* od = od_new0(vt,ppiDes,boxDes);
    od->resources = odres_newLazy(r);
    return od;
}

extern void     od_delete(Toutdev*   od)
{
    Coutdev_vt* vt = od->vtable;
    while ( vt ) {
        if ( vt->topVT.destructor )
            vt->topVT.destructor(od);
        vt = vt->topVT.parentVT;
    }
    free(od);
}

extern void     od_attach(Toutdev*od, Coutdev_src_vt* vtSrc, double ppiSrc, void* dataSrc)
{
    od->vtSrc  = vtSrc;
    od->desPerSrc = od->ppiDes/ppiSrc;
    od->parSrc = dataSrc;
    // configuration for havingi:
    //   - (0,0) src. corner on the (0,0) des. corner
    //   - a zoom factor equal to 1.
    od->homothetySrc2Des = od->ppiDes/ppiSrc;
    Box_dx( od->boxSrc ) = Box_dx( od->boxDes ) / od->homothetySrc2Des;
    Box_dy( od->boxSrc ) = Box_dy( od->boxDes ) / od->homothetySrc2Des;
}

extern void od_detach(Toutdev*od)
{
    od->vtSrc     = 0;
    od->desPerSrc = 0;
    od->parSrc    = 0;
    od->homothetySrc2Des = 0;
    Box_dx( od->boxSrc ) = 0;
    Box_dy( od->boxSrc ) = 0;
}

#include "outdev-vt.c"

/*======================================================================*/
/*= Wrappers to operations of topVT and vtSrc.                         =*/

extern void od_clear (Coutdev*od)
{
    Coutdev_top_vt* vt = &od->vtable->topVT; 
    while ( vt->clear==0 ) {
        vt = &vt->parentVT->topVT;
    }
    vt->clear(od);
}

extern void od_draw (Coutdev*od)
{
    Coutdev_top_vt* dvt;
    Coutdev_src_vt* svt;

    // destination pre-drawing
    for (dvt=&od->vtable->topVT; dvt!=0 ;dvt=&dvt->parentVT->topVT) {
        if ( dvt->preDraw!= 0 ) { dvt->preDraw(od); break; }
    }

    // source drawing
    for (svt=od->vtSrc; svt!=0 ;svt=svt->parent) {
        if ( svt->draw!= 0 ) { svt->draw(od); break; }
    }

    // destination post-drawing
    for (dvt=&od->vtable->topVT; dvt!=0 ;dvt=&dvt->parentVT->topVT) {
        if ( dvt->postDraw!= 0 ) { dvt->postDraw(od); break; }
    }
}

extern void od_redraw (Coutdev*od) { od_clear(od); od_draw(od); }

extern Ccolor* od_clearColor(Coutdev*od)
{
    Coutdev_top_vt* vt = &od->vtable->topVT; 
    while ( vt->clearColor==0 ) {
        vt = &vt->parentVT->topVT;
    }
    return vt->clearColor(od);
}

extern Tmsgdrv* od_msgdrv(Coutdev*od)
{
    Coutdev_top_vt* vt = &od->vtable->topVT; 
    while ( vt->msgdrv==0 ) {
        vt = &vt->parentVT->topVT;
    }
    return vt!=0 ? vt->msgdrv(od) : od->msgdrv;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Drawing sequence.                                                  =*/

extern void od_ds_stdStart (Coutdev*od)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    if (vt->ds_stdStart!=0)
        vt->ds_stdStart(od);
}

extern void od_ds_stdCommit(Coutdev*od)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    if (vt->ds_stdCommit!=0)
        vt->ds_stdCommit(od);
}

extern void od_ds_xorEraseStart(Coutdev*od, int num)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    TEMP_asf_printf(num<0 || num>=OD_DS_NBXOR,
            "%d invalid xor section num (expected in [0:%d[",num,OD_DS_NBXOR);
    if (vt->ds_xorEraseStart!=0)
        vt->ds_xorEraseStart(od,num);
}

extern void od_ds_xorEraseCommit(Coutdev*od, int num)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    TEMP_asf_printf(num<0 || num>=OD_DS_NBXOR,
            "%d invalid xor section num (expected in [0:%d[",num,OD_DS_NBXOR);
    if (vt->ds_xorEraseCommit!=0)
        vt->ds_xorEraseCommit(od,num);
}

extern void od_ds_xorDrawStart(Coutdev*od, int num)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    TEMP_asf_printf(num<0 || num>=OD_DS_NBXOR,
            "%d invalid xor section num (expected in [0:%d[",num,OD_DS_NBXOR);
    if (vt->ds_xorDrawStart!=0)
        vt->ds_xorDrawStart(od,num);
}

extern void od_ds_xorDrawCommit(Coutdev*od, int num)
{
    Coutdev_top_vt* vt = &od->vtable->topVT;
    TEMP_asf_printf(num<0 || num>=OD_DS_NBXOR,
            "%d invalid xor section num (expected in [0:%d[",num,OD_DS_NBXOR);
    if (vt->ds_xorDrawCommit!=0)
        vt->ds_xorDrawCommit(od,num);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Default virtual table for source operation                         =*/

static void od_donothing (Coutdev*od)
{
    TEMP_spw_printf("does a function redefinition miss in the src vtable?",0);
}

static void od_redraw_default (Coutdev*od)
{ od_clear(od); od_draw(od); }

Toutdev_src_vt outdev_src_vt = {
    .parent = NULL,
    .draw   = od_donothing,
};

/*======================================================================*/
/*= Wrappers to operations of colorVT.                                 =*/

extern Tcolor* od_blackColor(Coutdev*od)
{
    Coutdev_color_vt* vt = &od->vtable->colorVT; 
    TEMP_asf_printf( vt->black==0, "%s", "EGS function is mandatory");
    return vt->black(od);
}

extern Tcolor* od_whiteColor(Coutdev*od)
{
    Coutdev_color_vt* vt = &od->vtable->colorVT; 
    TEMP_asf_printf( vt->white==0, "%s", "EGS function is mandatory");
    return vt->white(od);
}

/*======================================================================*/
/*= Wrappers to operations of srcVT and font cache of resources.       =*/

extern void       od_fontinfo_chgLazy (Toutdev*od, TfontInfo**fdes, CfontInfo* fsrc)
{
    if ( *fdes==fsrc ) return;
    if ( *fdes ) od_fontinfo_delete(od,*fdes);
    *fdes = fontinfo_newLazi( fsrc );
}

extern void       od_fontinfo_delete  (Toutdev*od, TfontInfo* fi)
{
    Coutdev_vt* vt = od->vtable;
    TfontEGSunload unload=0;
    while ( vt!=0 && (unload=vt->strVT.fontUnload)==0 )
        vt = vt->topVT.parentVT;
    fontinfo_delete(fi,unload,od);
}

extern CfontInfo* od_fontset_searchLRU(Toutdev*od, int fi, double pts);

extern CfontInfo* od_fontset_addLRU(Toutdev*od, int fi, double pts,
                                    TfontEGS f, double ratio)
{
    Coutdev_vt* vt = od->vtable;
    TfontEGSunload unload=0;
    while ( vt!=0 && (unload=vt->strVT.fontUnload)==0 )
        vt = vt->topVT.parentVT;
    return fontset_addLRU( od->resources->fontCache, fi, pts, f, ratio, unload, od);
}

/*======================================================================*/
/*= Changing the drawing point of view and scale.                      =*/

extern int od_updateSrcBB_scale(Toutdev*od, double r)
{
    od->homothetySrc2Des = r;
    double dx = Box_dx(od->boxDes)/od->homothetySrc2Des;
    double dy = Box_dy(od->boxDes)/od->homothetySrc2Des;
    if ( dx!=Box_dx(od->boxSrc) || dy!=Box_dy(od->boxSrc) ) {
        Box_dx(od->boxSrc) = dx;
        Box_dy(od->boxSrc) = dy;
        return 1;
    } else {
        return 0;
    }
}

extern int od_updateSrcBB (Toutdev*dev)
{
    double dx = Box_dx(dev->boxDes)/dev->homothetySrc2Des;
    double dy = Box_dy(dev->boxDes)/dev->homothetySrc2Des;
    if ( dx!=Box_dx(dev->boxSrc) || dy!=Box_dy(dev->boxSrc) ) {
        Box_dx(dev->boxSrc) = dx;
        Box_dy(dev->boxSrc) = dy;
        return 1;
    } else {
        return 0;
    }
}

extern int od_updateSrcBB_toBoxXY(Toutdev*dev, CboxDble*sbox, double x, double y)
{
    double xsrc   = CONV_D_D2S(dev, LUNIT_CmToIn( x ) * dev->ppiDes );
    double ysrc   = CONV_D_D2S(dev, LUNIT_CmToIn( y ) * dev->ppiDes );
    double scalex = (Box_dx(dev->boxDes))/(Box_dx(*sbox)+2.*xsrc);
    double scaley = (Box_dy(dev->boxDes))/(Box_dy(*sbox)+2.*ysrc);
    double scale  = min2(scalex,scaley);
    double dx  = Box_dx(dev->boxDes)/scale;
    double dy  = Box_dy(dev->boxDes)/scale;
    double nwx = Box_nwx(*sbox) - (dx-Box_dx(*sbox))/2.;
    double nwy = Box_nwy(*sbox) - (dy-Box_dy(*sbox))/2.;
    if ( scale!= dev->homothetySrc2Des ||
         nwx  != Box_nwx(dev->boxSrc)  ||
         nwy  != Box_nwy(dev->boxSrc)  ||
         dx   != Box_dx (dev->boxSrc)  ||
         dy   != Box_dy (dev->boxSrc)  ) {
         dev->homothetySrc2Des = scale;
         Box_nwx(dev->boxSrc)  = nwx; 
         Box_nwy(dev->boxSrc)  = nwy; 
         Box_dx (dev->boxSrc)  = dx ; 
         Box_dy (dev->boxSrc)  = dy ; 
         return 1;
     } else {
         return 0;
     }
}

extern int od_updateSrcBB_toBox(Toutdev*dev, CboxDble*sbox)
{
    double bd_src = CONV_D_D2S(dev, LUNIT_CmToIn( 0.5 ) * dev->ppiDes );
    double scalex = (Box_dx(dev->boxDes))/(Box_dx(*sbox)+2.*bd_src);
    double scaley = (Box_dy(dev->boxDes))/(Box_dy(*sbox)+2.*bd_src);
    double scale  = min2(scalex,scaley);
    double dx  = Box_dx(dev->boxDes)/scale;
    double dy  = Box_dy(dev->boxDes)/scale;
    double nwx = Box_nwx(*sbox) - (dx-Box_dx(*sbox))/2.;
    double nwy = Box_nwy(*sbox) - (dy-Box_dy(*sbox))/2.;
    if ( scale!= dev->homothetySrc2Des ||
         nwx  != Box_nwx(dev->boxSrc)  ||
         nwy  != Box_nwy(dev->boxSrc)  ||
         dx   != Box_dx (dev->boxSrc)  ||
         dy   != Box_dy (dev->boxSrc)  ) {
         dev->homothetySrc2Des = scale;
         Box_nwx(dev->boxSrc)  = nwx; 
         Box_nwy(dev->boxSrc)  = nwy; 
         Box_dx (dev->boxSrc)  = dx ; 
         Box_dy (dev->boxSrc)  = dy ; 
         return 1;
     } else {
         return 0;
     }
}

/*======================================================================*/

extern double od_getUsrZoom(Coutdev*od)
{
    double zoom = od->homothetySrc2Des / od->desPerSrc;
    return zoom;
}

extern int od_setUsrZoom_NW(Toutdev*od, double z)
{
    double zoom_old = od_getUsrZoom(od);
    double zoom_new = z;
    int    ret      = 0;
    if ( zoom_old!=zoom_new ) {
        double r = zoom_new * od-> desPerSrc;
        od_updateSrcBB_scale(od,r );
        ret = 1;
    }
    TEMP_ase_printf( ! dble_closeAtP(z,od_getUsrZoom(od),1e-5),
        "invalid zoom factor %f (expected z=%f)",od_getUsrZoom(od),z);
    return ret;
}

extern int od_setUsrZoom_CENTER(Toutdev*od, double z)
{
    Tdble2 oldSrcCenter;
    oldSrcCenter.x = CONV_X_D2S(od,Box_dx(od->boxDes)/2);
    oldSrcCenter.y = CONV_Y_D2S(od,Box_dy(od->boxDes)/2);
    if ( od_setUsrZoom_NW(od,z) ) {
        Tdble2 newSrcCenter;
          newSrcCenter.x = CONV_X_D2S(od,Box_dx(od->boxDes)/2);
          newSrcCenter.y = CONV_Y_D2S(od,Box_dy(od->boxDes)/2);
        Box_xmin(od->boxSrc) += oldSrcCenter.x - newSrcCenter.x;
        Box_ymin(od->boxSrc) += oldSrcCenter.y - newSrcCenter.y;
        return 1;
    } else
        return 0;
}

/*======================================================================*/
/*= Miscelaneous getters and setters                                   =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Handler of GC stack                                                =*/

extern Tsty_text* od_style_topText(Coutdev*od, Tod_GCdef gcd)
{ return &od_style_top(od,gcd)->sty_text; }
extern Tsty_line* od_style_topLine(Coutdev*od, Tod_GCdef gcd)
{ return &od_style_top(od,gcd)->sty_line; }
extern Tsty_fill* od_style_topFill(Coutdev*od, Tod_GCdef gcd)
{ return &od_style_top(od,gcd)->sty_fill; }
extern Tstyle*    od_style_top    (Coutdev*od, Tod_GCdef gcd)
{ return styleset_top(od->resources->gcStack[gcd]); }
extern void       od_style_pushTop(Toutdev*od, Tod_GCdef gcd)
{ 
    Tstyle top = odsty_copy( od_style_top( od,gcd) );
    styleset_push(od->resources->gcStack[gcd], &top);
}
extern void       od_style_pop    (Toutdev*od, Tod_GCdef gcd)
{
    Tstyle* old_gc = od_style_top( od,gcd);
    styleset_pop( od->resources->gcStack[gcd] );
    Tstyle* new_gc = od_style_top( od,gcd);
    if ( odsty_compText(&new_gc->sty_text,&old_gc->sty_text)!=0 )
        od_str_setGCP_byStyle(od,gcd,&new_gc->sty_text);
    if ( odsty_compLine(&new_gc->sty_line,&old_gc->sty_line)!=0 )
        od_ldaf_setGCddP_byStyle(od,gcd,&new_gc->sty_line);
    if ( odsty_compFill(&new_gc->sty_fill,&old_gc->sty_fill)!=0 )
        od_ldaf_setGCll_byStyle(od,gcd,1.0,&new_gc->sty_fill);
    odsty_free( old_gc );
}

/*======================================================================*/
