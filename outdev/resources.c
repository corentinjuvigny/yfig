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
 *      $Id: resources.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/resources.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See outdev.h header for documentation.                             =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/outdev.h"
#include "outdev/resources.h"
#include "outdev/style.h"

#include "tools/font.h"

/*======================================================================*/
/*= Interface for building resources                                   =*/

extern TodResources* odres_new(Ccolor*c)
{
    TodResources* res = tool_malloc_0( sizeof(*res) );
  //res->dot [0] = 0;
    res->dot [1] = 3.5;
    res->dash[0] = 4;
    res->dash[1] = 7;
    res->dash1dot1[0] = 3.5;
    res->dash1dot1[1] = 2.5;
  //res->dash1dot1[2] = 0;
    res->dash1dot1[3] = 2.5;
    res->refcnt = 1;
    res->fontCache = fontset_new();
    odres_gcstack_new(res->gcStack,c);
    return res;
}

extern TodResources* odres_newWithBtm(Ccolor*c)
{
    TodResources* res = odres_new(c);
    odres_chgLighBtmsEated( res, btm_defaultLightPattern() );
    odres_chgShadBtmsEated( res, btm_defaultShadePattern() );
    odres_chgPattBtmsEated( res, btm_defaultFillPattern()  );
    return res;
}

extern TodResources* odres_newClone(CodResources*res);

extern TodResources* odres_newLazy (TodResources*res)
{
    res->refcnt += 1;
    return res;
}

extern void          odres_delete    (TodResources*res, TfontEGSunload f, void* od)
{
    res->refcnt -= 1;
    if ( res->refcnt!=0 ) return;

    if ( res->lighBtms!=0 ) btmset_delete( res->lighBtms );
    if ( res->shadBtms!=0 ) btmset_delete( res->shadBtms );
    if ( res->pattBtms!=0 ) btmset_delete( res->pattBtms );
    fontdefset_delete( res->fontUsed );
    fontset_delete( res->fontCache , f , od );
    odres_gcstack_delete( res->gcStack );
    free( res );
}

/*======================================================================*/
/*= Accessors to resources                                             =*/

static Trawdata btm_cross_bits = { -1, 8, {
    0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81 } };
static Tbitmap btm_cross = { 8, 8, &btm_cross_bits, -1 };

static Trawdata btm_50pc_bits = { -1, 8, {
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 } };
static Tbitmap btm_50pc  = { 8, 8, &btm_50pc_bits, -1 };

extern Tbitmap* odres_getLighBtm(CodResources*res, int idx)
{
    Tbitmap* org;
    if ( res == 0 ) {
        org = &btm_50pc;
    } else if ( res->lighBtms == 0 ) {
        org = &btm_50pc;
    } else if ( idx >= res->lighBtms->eleNb ) {
        org = &btm_50pc;
    } else 
        org = res->lighBtms->eles[idx];
    return btm_newLazy( org );
}

extern Tbitmap* odres_getShadBtm(CodResources*res, int idx)
{
    Tbitmap* org;
    if ( res == 0 ) {
        org = &btm_50pc;
    } else if ( res->shadBtms == 0 ) {
        org = &btm_50pc;
    } else if ( idx >= res->shadBtms->eleNb ) {
        org = &btm_50pc;
    } else 
        org = res->shadBtms->eles[idx];
    return btm_newLazy( org );
}

extern Tbitmap* odres_getPattBtm(CodResources*res, int idx)
{
    Tbitmap* org;
    if ( res == 0 ) {
        org = &btm_cross;
    } else if ( res->pattBtms == 0 ) {
        org = &btm_cross;
    } else if ( idx >= res->pattBtms->eleNb ) {
        org = &btm_cross;
    } else 
        org = res->pattBtms->eles[idx];
    return btm_newLazy( org );
}

extern TfontDef*odres_getFontUsed(CodResources*res, int idx)
{
    TEMP_asf_printf(res==0,"%s","res is null");
    TfontDefSet* s = res->fontUsed;
    TEMP_asf_printf(s==0,"%s","res->fontUsed is null");
    TEMP_asf_printf(idx<0 || s->eleNb<=idx,"idx=%d not in [0,%d[",idx, s->eleNb);
    return s->eles[idx];
}

/*======================================================================*/
/*= Change resource elements.                                          =*/

extern void odres_chgLighBtmsEated(TodResources*res,TbitmapSet* btms)
{
    if ( res->lighBtms!=0 ) btmset_delete( res->lighBtms );
    res->lighBtms = btms;
}
extern void odres_chgLighBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgLighBtmsLazy (TodResources*res,TbitmapSet* btms);

extern void odres_chgShadBtmsEated(TodResources*res,TbitmapSet* btms)
{
    if ( res->shadBtms!=0 ) btmset_delete( res->shadBtms );
    res->shadBtms = btms;
}
extern void odres_chgShadBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgShadBtmsLazy (TodResources*res,TbitmapSet* btms);

extern void odres_chgPattBtmsEated(TodResources*res,TbitmapSet* btms)
{
    if ( res->pattBtms!=0 ) btmset_delete( res->pattBtms );
    res->pattBtms = btms;
}
extern void odres_chgPattBtmsClone(TodResources*res,CbitmapSet* btms);
extern void odres_chgPattBtmsLazy (TodResources*res,TbitmapSet* btms);


extern void odres_chgFontUsedEated(TodResources*res,TfontDefSet* s)
{
    if ( res->fontUsed!=0 ) fontdefset_delete( res->fontUsed );
    res->fontUsed = s;
}
extern void odres_chgFontUsedClone(TodResources*res,CfontDefSet* s);
extern void odres_chgFontUsedLazy (TodResources*res,TfontDefSet* s);

/*======================================================================*/
/*= GC stack handler.                                                  =*/

extern void odres_gcstack_new   (TstyleSet* gc[OD_GC_NB], Ccolor* c)
{
    Tstyle sty; int i;
    for ( i=0 ; i<OD_GC_NB ; i+=1) {
        gc[i] = styleset_new(1);
        odsty_init(&sty,c);
        styleset_push(gc[i],&sty);
    }
}

extern void odres_gcstack_delete(TstyleSet* gc[OD_GC_NB])
{
    int i,j;
    for ( i=0 ; i<OD_GC_NB ; i+=1) {
        for ( j=0 ; j<gc[i]->eleNb ; j+=1) {
            Tstyle* s =  &gc[i]->eles[j];
            odsty_free(s);
        }
        styleset_delete(gc[i]);
    }
}

extern Tsty_text* odres_style_topText(TodResources*od, Tod_GCdef gcd)
  { return &odres_style_top(od,gcd)->sty_text; }
extern Tsty_line* odres_style_topLine(TodResources*od, Tod_GCdef gcd)
  { return &odres_style_top(od,gcd)->sty_line; }
extern Tsty_fill* odres_style_topFill(TodResources*od, Tod_GCdef gcd)
  { return &odres_style_top(od,gcd)->sty_fill; }
extern Tstyle*    odres_style_top    (TodResources*od, Tod_GCdef gcd)
  { return styleset_top(od->gcStack[gcd]); }
extern void       odres_style_pushTop(TodResources*od, Tod_GCdef gcd)
  { Tstyle sty = odsty_copy(odres_style_top(od,gcd));
    styleset_push( od->gcStack[gcd], &sty); } 
extern void       odres_style_pop    (TodResources*od, Tod_GCdef gcd)
  { odsty_free(odres_style_top(od,gcd));
    styleset_pop( od->gcStack[gcd]); } 

/*======================================================================*/
/*= Interface from output device                                       =*/

extern Tbitmap* od_getLighBtm(Toutdev*od, int idx, double zf)
{
//printf("%s: idx=%d zf=%f\n",__func__,idx,zf);
    return odres_getLighBtm(od->resources,idx);
}
extern Tbitmap* od_getShadBtm(Toutdev*od, int idx, double zf)
{
//printf("%s: idx=%d zf=%f\n",__func__,idx,zf);
    return odres_getShadBtm(od->resources,idx);
}

extern Tbitmap* od_getPattBtm(Toutdev*od, int idx, double zf)
{
//printf("%s: idx=%d zf=%f\n",__func__,idx,zf);
    return odres_getPattBtm(od->resources,idx);
}

extern double   od_getLighCoef(Toutdev*od, int idx)
{
    // idx=0       (a few lighten) ---> 1.0/eleNb
    // idx=eleNb-1 (white)         ---> 1.0
    //
    // idx  ---> alpha idx + beta
    //   - beta  = (n-1)/n / (n-1) = 1/n
    //   - alpha = (1-1/n) / (n-1) = (n-1)/n / (n-1) = 1/n
    // 
    // idx ---> idx/n + 1/n = (idx+1)/eleNb
    double ret;
    if ( od->resources == 0 ) {
        ret = 0;
    } else if ( od->resources->lighBtms == 0 ) {
        ret = 0;
    } else if ( idx >= od->resources->lighBtms->eleNb ) {
        ret = 0; // 1 ?
    } else {
        ret  = idx+1;
        ret /= (double)od->resources->lighBtms->eleNb;
//printf("CCCC %d/%d --> %f\n",idx,od->resources->lighBtms->eleNb,ret);
    }
    return ret;
}
extern double   od_getShadCoef(Toutdev*od, int idx)
{
    // idx=0       (black)     ---> 1.0
    // idx=eleNB-1 (unchanged) ---> 0.0
    // idx                     ---> (eleNb-1-idx)/(eleNb-1)
    double ret;
    if ( od->resources == 0 ) {
        ret = 0;
    } else if ( od->resources->shadBtms == 0 ) {
        ret = 0;
    } else if ( idx >= od->resources->shadBtms->eleNb ) {
        ret = 0; // 1 ?
    } else {
        ret  = od->resources->shadBtms->eleNb-1-idx;
        ret /= (double)(od->resources->shadBtms->eleNb-1);
    }
    return ret;
}

/*======================================================================*/
