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
 *      $Id: color.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/color.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Color package.                                                     =*/
/*= See the color-types.h and color.h headers for documentation.       =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/color.h"

/*======================================================================*/

#define COLOR_UndefName      "@undefined@"
#define COLOR_IsUndefined(c)  ((c)->name==0 || \
                               strcmp((c)->name,COLOR_UndefName)==0 )
#define RGB_IsDefined(rgb)    (rgb.red>=0 && rgb.green>=0 && rgb.blue>=0)
#define COLOR_IsDefinedByRGB(c)  RGB_IsDefined((c)->rgb)

#define RGB_DIST(a,b) ( abs((a)->red   - (b)->red) + \
                        abs((a)->green - (b)->green) + \
                        abs((a)->blue  - (b)->blue) )

TcolorEGS EGSCOLOR_none    = { .ulong = EGSCOLOR_NONE };
TcolorEGS EGSCOLOR_default = { .ulong = EGSCOLOR_DEFAULT };


/*======================================================================*/
/*= color constructors and destructors                                 =*/

extern Tcolor* color_new(const char* name,
                         int red, int green, int blue, TcolorEGS ce)
{
    Tcolor* ret  = tool_malloc(sizeof(*ret));
    ret->driver    = 0;
    ret->name      = tool_strdup(name);
    if ( red>=0 && green>=0 && blue>=0 ) {
        ret->rgb.red   = red&0xFFFF;
        ret->rgb.green = green&0xFFFF;
        ret->rgb.blue  = blue&0xFFFF;
        if (name==0 ) {
            char buf[100];
            sprintf(buf,"rgb:%04x/%04x/%04x",
                    ret->rgb.red, ret->rgb.green, ret->rgb.blue);
            ret->name = strdup(buf);
        }
    } else {
        ret->rgb.red   = -1;
        ret->rgb.green = -1;
        ret->rgb.blue  = -1;
        if ( name==0 )
            ret->name = strdup(COLOR_UndefName);
    }
    ret->ukey      = -1;
    ret->egs       = ce;
    ret->cnt       = 1;
    ret->ukey      = 0;
    dbg_color_new(ret);
    return ret;
}

Tcolor*dbg_clr;
extern void    color_delete(Tcolor*c)
{
    if (c==0) return;
    dbg_color_del(c);
if ( dbg_clr==c ) {
    printf("%s:DBG-0 %p cnt=%d\n",__func__,c,c->cnt);
    printf("%s:DBG-1 %p\n",__func__,c);
}
    c->cnt -= 1;
    if ( c->cnt!=0 ) 
        return;
    if ( c->driver && COLOREGS_IsDefined(c->egs) )
        c->driver->free( c->driver->egs, c->egs);
    if (c->name) free(c->name);
    free(c);
}
extern void    color_deleteS0(Tcolor**pc)
{ color_delete(*pc); *pc=0; }

extern Tcolor* color_newLazy  (Ccolor*c)
{
    if ( c==NULL ) return NULL;
if ( dbg_clr==c ) {
    printf("%s:DBG-0 %p cnt=%d\n",__func__,c,c->cnt);
    printf("%s:DBG-1 %p\n",__func__,c);
}
    ((Tcolor*)c)->cnt += 1;
    return (Tcolor*)c;
}

extern Tcolor* color_newPriv(Ccolor*c)
{
    Tcolor* ret    = tool_malloc(sizeof(*ret));
    ret->driver    = c->driver;
    ret->name      = c->name!=0 ? strdup(c->name) : 0;
    ret->rgb       = c->rgb;
    if ( c->driver==0 || COLOREGS_IsNone(c->egs) )
        ret->egs = EGSCOLOR_none;
    else
        ret->egs = c->driver->dup( c->driver->egs, c->egs );
    ret->cnt        = 1;
    ret->ukey       = 0;
    dbg_color_new(ret);
    return ret;
}

extern Tcolor* color_newByXor(Ccolor*c1, Ccolor*c2)
{
    Tcolor* ret  = tool_malloc(sizeof(*ret));
    ret->driver    = c1->driver;
    ret->rgb.red   = c1->rgb.red   ^ c2->rgb.red;
    ret->rgb.green = c1->rgb.green ^ c2->rgb.green;
    ret->rgb.blue  = c1->rgb.blue  ^ c2->rgb.blue;
    if ( c1->driver==0)
        ret->egs = EGSCOLOR_none;
    else
        ret->egs    = c1->driver->newByRGB08(c1->driver->egs,
                ret->rgb.red,ret->rgb.green,ret->rgb.blue);
    char buf[100];
    sprintf(buf,"rgb:%04x/%04x/%04x",
            ret->rgb.red, ret->rgb.green, ret->rgb.blue);
    ret->name   = strdup(buf);
    ret->cnt        = 1;
    ret->ukey       = 0;
    dbg_color_new(ret);
    return ret;
}

extern void    color_chgLazy  (Tcolor**dd,Ccolor*c)
{
    if ( *dd==c ) return;
    color_delete( *dd );
    *dd = color_newLazy( c );
}

extern void    color_chgEaten  (Tcolor**dd,Tcolor*c)
{
    if ( *dd==c ) { color_delete( c ); return; }
    color_delete( *dd );
    *dd = c;
}


extern Tcolor* color_newByStr  (CcolorDriver*drv, CstrBrief str)
{
    double sc;
    int n,sh;
    int r,g,b;
    Tcolor*   ret;
    TcolorEGS egs;
    if ( (egs=drv->newByName(drv->egs,str)).ulong!=EGSCOLOR_NONE ) {
        drv->getRGB08(drv->egs,egs,&r,&g,&b);
        ret = color_new(str,r,g,b,egs);
        ret->driver = drv;
        return ret;
    }
    // try if str matches same X11 color name or some RGB syntax
    if ( strncasecmp(str,"gray",4)==0 && sscanf(str+4,"%d",&n)==1 && 0<=n && n<=100 ) {
        static Tuint8 grays[] = {
            /* gray000-009 */ 0x00, 0x03, 0x05, 0x08, 0x0A, 0x0D, 0x0F, 0x12, 0x14, 0x17,
            /* gray010-019 */ 0x1A, 0x1C, 0x1F, 0x21, 0x24, 0x26, 0x29, 0x2B, 0x2E, 0x30,
            /* gray020-029 */ 0x33, 0x36, 0x38, 0x3B, 0x3D, 0x40, 0x42, 0x45, 0x47, 0x4A,
            /* gray030-039 */ 0x4D, 0x4F, 0x52, 0x54, 0x57, 0x59, 0x5C, 0x5E, 0x61, 0x63,
            /* gray040-049 */ 0x66, 0x69, 0x6B, 0x6E, 0x70, 0x73, 0x75, 0x78, 0x7A, 0x7D,
            /* gray050-059 */ 0x7F, 0x82, 0x85, 0x87, 0x8A, 0x8C, 0x8F, 0x91, 0x94, 0x96,
            /* gray060-069 */ 0x99, 0x9C, 0x9E, 0xA1, 0xA3, 0xA6, 0xA8, 0xAB, 0xAD, 0xB0,
            /* gray070-079 */ 0xB3, 0xB5, 0xB8, 0xBA, 0xBD, 0xBF, 0xC2, 0xC4, 0xC7, 0xC9,
            /* gray080-089 */ 0xCC, 0xCF, 0xD1, 0xD4, 0xD6, 0xD9, 0xDB, 0xDE, 0xE0, 0xE3,
            /* gray090-100 */ 0xE5, 0xE8, 0xEB, 0xED, 0xF0, 0xF2, 0xF5, 0xF7, 0xFA, 0xFC, 0xFF
        };
        r = g = b = grays[n];
    } else if   ( sscanf(str,"rgb:%1x/%1x/%1x",&r,&g,&b)==3 ) {
        r+=(r<<4); g+=(g<<4); b+=(b<<4); 
//printf("%s: AA1 : rgb=%04x/%04x/%04x\n",__func__,r,g,b);
    } else if ( sscanf(str,"rgb:%02x/%02x/%02x",&r,&g,&b)==3 ) {
        sh=0;  r >>= sh; g >>=sh ; b >>= sh;
    } else if ( sscanf(str,"rgb:%03x/%03x/%03x",&r,&g,&b)==3 ) {
        sh=4;  r >>= sh; g >>=sh ; b >>= sh;
    } else if ( sscanf(str,"rgb:%04x/%04x/%04x",&r,&g,&b)==3 ) {
        sh=8;  r >>= sh; g >>=sh ; b >>= sh;
    } else if ( sscanf(str,"#%01x%01x%01x",&r,&g,&b)==3 ) {
        sh=4;  r <<= sh; g <<=sh ; b <<= sh;
    } else if ( sscanf(str,"#%02x%02x%02x",&r,&g,&b)==3 ) {
        sh=0;  r >>= sh; g >>=sh ; b >>= sh;
    } else if ( sscanf(str,"#%03x%03x%03x",&r,&g,&b)==3 ) {
        sh=4;  r >>= sh; g >>=sh ; b >>= sh;
    } else if ( sscanf(str,"#%04x%04x%04x",&r,&g,&b)==3 ) {
        sh=8;  r >>= sh; g >>=sh ; b >>= sh;
    } else {
        return 0;
    }
    if ( (egs=drv->newByRGB08(drv->egs,r,g,b)).ulong!=EGSCOLOR_NONE ) {
        ret = color_new(str,r,g,b,egs);
        ret->driver = drv;
        return ret;
    }
    return 0;
}

extern Tcolor* color_newByRGB(CcolorDriver*drv,
                              int red, int green, int blue)
{
    Tcolor* ret = color_new(0,red,green,blue,
                    drv->newByRGB08(drv->egs,red,green,blue));
    ret->driver = drv;
    return ret;
}

extern void color_updateEGS(CcolorDriver*drv, Tcolor*c)
{
    c->driver = drv;
    if ( c->egs.ulong==EGSCOLOR_NONE )
        c->egs = drv->newByRGB08(drv->egs,
                c->rgb.red,c->rgb.green,c->rgb.blue);
}

/*======================================================================*/
// full order: return <0:l<r ; 0:l=r ; >0:l>r
extern int color_cmp(Ccolor*l, Ccolor*r)
{
    int ret;
    if ( (ret=l->rgb.red-r->rgb.red)!=0 ) return ret;
    if ( (ret=l->rgb.green-r->rgb.green)!=0 ) return ret;
    return l->rgb.blue-r->rgb.blue;
}
extern int color_cmpRGB(CcolorRGB*l, CcolorRGB*r)
{
    int ret;
    if ( (ret=l->red-r->red)!=0 ) return ret;
    if ( (ret=l->green-r->green)!=0 ) return ret;
    return l->blue-r->blue;
}

// return 1 if l and r colors are close otherwise 0
extern int color_close(Ccolor*l, Ccolor*r);

// return 1 if l and r colors are close otherwise 0
// see tolerance comment in TcolorSet
extern int color_close2(Ccolor*l, Ccolor*r, int tolerance);

/*======================================================================*/

extern TcolorSet*    colorset_newDNT (CcolorDriver*drv,int n,int tolerance)
{
    TcolorSet* ret = tool_malloc(sizeof(*ret));
    ret->driver    = drv;
    ret->tolerance = tolerance;
    ret->uc        = 0;
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? tool_malloc(ret->eleMemNb * sizeof(*ret->eles)) : 0;
    return ret;
}

extern void          colorset_free (TcolorSet* s)
{
    if ( s==0 ) return;
    if ( s->eles ) {
        int i;
        for (i=0 ; i<s->eleNb ; i+=1) {
            color_delete(s->eles[i]);
        }
        free(s->eles);
    }
    free(s);
}

static Ccolor* colorset_addSearch (CcolorSet* s, Ccolor* c)
{
    Ccolor* found;
    // first case: the undefined color
    if ( COLOR_IsUndefined(c) ) {
        return s->uc;
    }
    // second case: no RGB specified (but name is defined
    //              otherwise it's the first case)
    if ( COLOR_IsDefinedByRGB(c)==0 ) {
        found = colorset_getByName(s,c->name);
        if ( found!=0 )
            return found;
    } 
    // third case: RGB is specified
    found = colorset_getByRGB(s,c->rgb);
    return found;
}

static void colorset_addBase (TcolorSet* s, Ccolor* c)
{
    if ( s->eleNb == s->eleMemNb ) {
        int inc = (s->eleMemNb*15)/100;
        if      ( inc<4 )   inc=4;
        else if ( inc>1000) inc = 1000;
        s->eleMemNb += inc;
        s->eles =realloc(s->eles,s->eleMemNb*sizeof(*s->eles));
    }
    s->eles[s->eleNb] = (Tcolor*)c;
    s->eleNb += 1;
}

extern Ccolor* colorset_addLazy (TcolorSet* s, Ccolor* c)
{
    Ccolor* found = colorset_addSearch(s,c);
    if ( found!=0 )
        return found;
    color_newLazy( c );
    if ( COLOR_IsUndefined(c) )
        s->uc = (Tcolor*)c;
    else
        colorset_addBase(s,c);
    return c;
}

extern Ccolor* colorset_addPriv (TcolorSet* s, Ccolor* c)
{
    Ccolor* found = colorset_addSearch(s,c);
    if ( found!=0 )
        return found;
    Tcolor* nc = color_newPriv( c );
    if ( COLOR_IsUndefined(c) )
        s->uc = nc;
    else
        colorset_addBase(s,nc);
    return nc;
}

extern Ccolor* colorset_addEated(TcolorSet* s, Ccolor* c)
{
    Ccolor* found = colorset_addSearch(s,c);
    if ( found!=0 ) {
        color_delete( (Tcolor*)c );
        return found;
    }
    if ( COLOR_IsUndefined(c) )
        s->uc = (Tcolor*)c;
    else
        colorset_addBase(s,c);
    return c;
}

extern const Tcolor* colorset_addByStr(TcolorSet* s, CstrBrief name)
{
    Tcolor* c;
    if ( s->driver!=0 )
        c = color_newByStr(s->driver,name);
    else
        c = color_new(name,-1,-1,-1,EGSCOLOR_none);
    if ( c==0 ) return 0;
    return colorset_addEated(s,c);
}

extern const Tcolor* colorset_addByRGB(TcolorSet* s, int r,int g,int b)
{
    Tcolor* c;
    if ( s->driver!=0 )
        c = color_newByRGB(s->driver,r,g,b);
    else
        c = color_new(0,r,g,b,EGSCOLOR_none);
    if ( c==0 ) return 0;
    return colorset_addEated(s,c);
}

extern Tcolor* colorset_getByPtr(CcolorSet* s, Ccolor* key)
{
    if ( s==0 ) return 0;
    int i;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( s->eles[i]==key )
            return s->eles[i];
    }
    return 0;
}
extern Ccolor* colorset_getByRGB (CcolorSet* s, TcolorRGB key)
{
    if ( s==0 || s->eleNb==0 ) return 0;
    int i;
    int minind = 0;
    int mindis = RGB_DIST(&s->eles[minind]->rgb,&key);
    for (i=1 ; i<s->eleNb ; i+=1) {
        int dis = RGB_DIST(&s->eles[i]->rgb,&key);
        if ( dis<mindis ) {
            mindis = dis;
            minind = i;
        }
    }
    return mindis<=s->tolerance ? s->eles[minind] : 0;
}
extern Ccolor* colorset_getByName(CcolorSet* s, CstrBrief key)
{
    if ( s==0 ) return 0;
    int i;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( strcmp(s->eles[i]->name,key)==0 )
            return s->eles[i];
    }
    return 0;
}
extern Ccolor* colorset_getByUKey(CcolorSet* s, int key)
{
    if ( s==0 ) return 0;
    int i;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( s->eles[i]->ukey==key )
            return s->eles[i];
    }
    return 0;
}

extern void          colorset_remove(TcolorSet* s, Tcolor* c)
{
    int i;
    for (i=0 ; i<s->eleNb ; i+=1) {
        if ( s->eles[i]==c ) {
            s->eleNb -= 1;
            s->eles[i] = s->eles[s->eleNb];
            return ;
        }
    }
}

extern void colorset_update(TcolorSet* s, CcolorDriver* drv)
{
    if ( s==0 ) return;
    int i;
    s->driver = drv;
    for (i=0 ; i<s->eleNb ; i+=1) {
        color_updateEGS(drv,s->eles[i]);
    }
}
/*======================================================================*/
