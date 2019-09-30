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
 *      $Id: font.c 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/font.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the font-types.h and font.h headers for documentation.         =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/font.h"

/*======================================================================*/
/*= EGS font.                                                          =*/

TfontEGS FONTEGS_none = { .slong = FONTEGS_NONE };

/*======================================================================*/
/*= font defintion set                                                 =*/

#define TPLSET_SetType TfontDefSet
#define TPLSET_SetCype CfontDefSet
#define TPLSET_EleType TfontDef*
#define TPLSET_EleCype CfontDef*
#define TPLSET_Func(f) fontdefset_##f
#define TPLSET_AddWithoutPtr
#define TPLSET_WithRefcnt
#include "tools/set-body-template.h"

/*======================================================================*/
/*= font information                                                   =*/

extern TfontInfo* fontinfo_new     (int fi, double pts, TfontEGS f, double ratio)
{
    TfontInfo* ret = tool_malloc( sizeof(*ret) );
    ret->fo_egs    = f;
    ret->fo_size   = pts;
    ret->fo_refcnt = 1;
    ret->fo_index  = fi;
    ret->fo_ratio  = ratio;
    dbg_fontinfo_new(ret);
    return ret;
}

extern TfontInfo* fontinfo_newClone(TfontInfo* fi);

extern TfontInfo* fontinfo_newLazi (CfontInfo* fi)
{
    if ( fi==0 ) return NULL;
    TfontInfo* ret = (TfontInfo*)fi;
    dbg_fontinfo_new(ret);
    ret->fo_refcnt += 1;
    return ret;
}

extern void fontinfo_delete(TfontInfo* fi,
                            TfontEGSunload unload, void* od)
{
    if ( fi==0 ) return;
    dbg_fontinfo_del(fi);
    fi->fo_refcnt -= 1;
    if ( fi->fo_refcnt>0 ) return;
    if ( unload!=0 && FONTEGS_IsDefined(fi->fo_egs) )
        unload(od, fi->fo_egs);
    free( fi );
}

extern int fontinfo_cmpIdxSz (CfontInfo* l, CfontInfo* r)
{
#define PTS_EPSILON 1e-3
    int ret;
    if ( (ret=l->fo_index-r->fo_index)!=0 ) return ret;
    if ( abs(l->fo_size-r->fo_size)<PTS_EPSILON ) return 0;
    if ( l->fo_size<r->fo_size ) return -1;
    return 1;
}

extern int fontinfo_cmpIdxSz2(CfontInfo* l, int fi, double pts)
{
    TfontInfo r = {0,.fo_egs.ulong=0,pts,fi};
    return fontinfo_cmpIdxSz(l,&r);
}

/*======================================================================*/
/*= font information set                                               =*/

static int fontset_searchIdx(TfontInfoSet* s, int fi, double pts);

static void fontset_clear (TfontInfoSet* s, TfontEGSunload unload, void* od )
{
    int i;
    for (i=0 ; i<s->eleNb ; i+=1 ) {
        TfontInfo* fd = s->eles[i];
        fontinfo_delete( s->eles[i], unload, od );
        s->eles[i]=0;   // not really required (but good for debuging)
    }
    s->eleNb = 0;
}

extern TfontInfoSet* fontset_new   ()
{
    int n = 100;
    TfontInfoSet* ret = tool_malloc(sizeof(*ret));
    //ret->gui       = gui;
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? tool_malloc(sizeof(*ret->eles)*n) : 0;
    return ret;
}

extern void fontset_delete(TfontInfoSet* s, TfontEGSunload unload, void* od)
{
    if ( s==0 ) return;
    fontset_clear( s, unload, od );
    tool_free( s->eles );
    free     ( s );
}

extern CfontInfo* fontset_add   (TfontInfoSet* s, int fi, double pts,
                                 TfontEGS f, double ratio)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    s->eles[s->eleNb] = fontinfo_new(fi,pts,f,ratio);
    s->eleNb += 1;
    return s->eles[s->eleNb-1];
}

/* LRU interface  */
extern CfontInfo* fontset_addLRU   (TfontInfoSet* s, int fi,
                                    double pts, TfontEGS f, double ratio,
                                    TfontEGSunload unload, void* od)
{
    int i;
    if ( s->eleNb == s->eleMemNb ) {
        // suppress the older element ( the 1st in table ).
        TfontInfo* older = s->eles[0];
        s->eleNb -= 1;
        for (i=0 ; i<s->eleNb ; i+=1) s->eles[i] = s->eles[i+1];
        // free the memory used by the older element.
        fontinfo_delete( older, unload, od );
    }
    return fontset_add(s,fi,pts,f,ratio);
}

extern CfontInfo* fontset_searchLRU(TfontInfoSet* s, int fi, double pts)
{
    int i;
    int index = fontset_searchIdx(s,fi,pts);
    if ( index<0 )
        return 0;
    // we handle the LRU: index is the newer, it must be
    // placed at table end.
    TfontInfo* save =  s->eles[index];
    for ( i=index ; i<(s->eleNb-1) ; i+=1) s->eles[i]=s->eles[i+1];
    s->eles[s->eleNb-1] = save;

    return s->eles[s->eleNb-1];
}

/*======================================================================*/

static int fontset_searchIdx(TfontInfoSet* s, int fi, double pts)
{
    int i;
    // reverse table order to start with the younger elements
    for ( i=s->eleNb-1 ; i>=0 ; i-=1 ) {
        if ( fontinfo_cmpIdxSz2(s->eles[i],fi,pts)==0 )
            break;
    }
    return i;
}

/*======================================================================*/
/*= Debug                                                              =*/
#if defined(TOOL_DBG_MemFnt) && TOOL_DBG_MemFnt

TfontInfo*     dbg_fontinfo;

#define DBG_fontinfo_unfreedMAX 1000
TfontInfo* dbg_fontinfo_unfreedT[DBG_fontinfo_unfreedMAX];
int        dbg_fontinfo_unfreedN;

extern void dbg_fontinfo_new(TfontInfo*fi)
{
    if (fi==0) return;
    int i;
    for (i=0 ; i<dbg_fontinfo_unfreedN ; i++) {
        if ( dbg_fontinfo_unfreedT[i]==fi ) {
            return ;
        }
    }
    // fi not found in dbg_fontinfo_unfreedT, we add it
    if (dbg_fontinfo_unfreedN==DBG_fontinfo_unfreedMAX) return;
    dbg_fontinfo_unfreedT[dbg_fontinfo_unfreedN++] = fi;
}

extern void dbg_fontinfo_del(TfontInfo*fi)
{
    if (fi==0) return;
    int i;
    for (i=0 ; i<dbg_fontinfo_unfreedN ; i++) {
        if ( dbg_fontinfo_unfreedT[i]==fi ) {
            if ( fi->fo_refcnt==1 ) {
                dbg_fontinfo_unfreedN -= 1;
                dbg_fontinfo_unfreedT[i] = dbg_fontinfo_unfreedT[dbg_fontinfo_unfreedN];
            }
            return ;
        }
    }
    // fi not found in dbg_fontinfo_unfreedT, we add it if refcount is not 1
    if ( fi->fo_refcnt==1 ) return;
    if (dbg_fontinfo_unfreedN==DBG_fontinfo_unfreedMAX) return;
    dbg_fontinfo_unfreedT[dbg_fontinfo_unfreedN++] = fi;
}

extern void           dbg_fontinfo_printUnfreed()
{
    int i;
    for (i=0 ; i<dbg_fontinfo_unfreedN ; i++) {
        TfontInfo* fi = dbg_fontinfo_unfreedT[i];
        PERM_spe_printf(" %2d/%d: fontinfo=%p: refcount is %d expected 0 : %d %4.2f",
            i,dbg_fontinfo_unfreedN, fi, fi->fo_refcnt, fi->fo_index, fi->fo_size);
    }
}

extern TfontInfoSet*  dbg_fontset;
extern void           dbg_fontset_new(TfontInfoSet*s);
extern void           dbg_fontset_del(TfontInfoSet*s);
extern void           dbg_fontset_printUnfreed();
#endif // TOOL_DBG_MemFnt

/*======================================================================*/
