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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: base-types.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/base-types.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= base types and base type sets.                                     =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/boxes.h"
#include "tools/math-macros.h"

/*======================================================================*/
/*= miscellaneous base types container                                 =*/

Cdble2     dble2_origin;
const Tany tool_any_null;

/*======================================================================*/
/*= string types                                                       =*/

static TstrEaten tool_newString(int len) { return tool_malloc_0( len + 1 * sizeof(char) ); }

extern int       tool_strIsEmptyLine(CstrBrief s)
{
    char c;
    if ( s==0 ) return 1;
    while ( (c=*s++)!=0 ) {
        switch (c) {
            case ' ':  case '\t':
            case '\r': case '\n':
                break;
            default:
                return 0;
        }
    }
    return 1;
}

extern TstrEaten tool_strdup(CstrBrief str)
{
    char *s;
    if (str==NULL)
	    return NULL;
    s=strdup(str);
    if ( s==NULL ) { 
	    tool_ftlerr_nomem(NULL);
    }
    return s;
}

extern TstrEaten tool_aprintf (CstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    tool_vaprintf(fmt,ap);
    va_end(ap);
}

extern TstrEaten tool_vaprintf(CstrBrief fmt, va_list ap)
{
    va_list ap1;
    size_t size;
    char *buffer;
  
    va_copy(ap1, ap);
    size = vsnprintf(NULL, 0, fmt, ap1) + 1;
    va_end(ap1);

    if ( (buffer=tool_newString(size))==0 ) {
        return 0;
    }
    vsnprintf(buffer, size, fmt, ap);
    return buffer;
}

/*======================================================================*/
/*= Rose wind direction                                                =*/

extern const TdirWR * tool_dirWR_table     ()
{
    static const TdirWR table[TOOL_DirWR_NB] = {
        DirWR_CC,
        DirWR_NN, DirWR_NE, DirWR_EE, DirWR_SE,
        DirWR_SS, DirWR_SW, DirWR_WW, DirWR_NW
    };
    return table;
}

extern const char*        tool_dirWR_toStr     (TdirWR dwr)
{
    switch ( dwr ) {
      case DirWR_NN: return "North";
      case DirWR_EE: return "East";
      case DirWR_SS: return "South";
      case DirWR_WW: return "West";
      case DirWR_NE: return "North-East";
      case DirWR_NW: return "North-West";
      case DirWR_SE: return "South-East";
      case DirWR_SW: return "South-West";
      default:       return "Center";
    }
}

extern const char** tool_dirWR_toStrTable()
{
    int i; const TdirWR * dirs = tool_dirWR_table();
    const char** table = tool_malloc( (TOOL_DirWR_NB+1)*sizeof(table[0]) );
    for (i=0 ; i<TOOL_DirWR_NB ; i+=1 )
        table[i] = tool_dirWR_toStr( dirs[i] );
    table[i] = 0;
    return table;
}

/*======================================================================*/
/*= memory allocation and freeing                                      =*/

extern void* tool_malloc  (size_t n)
{
    void *ret = malloc(n);
    if ( ret==0 )
        tool_ftlerr_nomem(NULL);
    return ret;
}

extern void* tool_malloc_0(size_t n)
{
    void *ret = malloc(n);
    if ( ret==0 )
        tool_ftlerr_nomem(NULL);
    else
        bzero(ret,n);
    return ret;
}
extern void* tool_realloc (void*p, size_t n)
{
    void *ret = realloc(p,n);
    if ( ret==0 )
        tool_ftlerr_nomem(NULL);
    return ret;
}

/*======================================================================*/
/*= base type sets.                                                    =*/

#define TPLSET sint
#define TPLSET_EleType int
#define TPLSET_EleCype const int
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
#define TPLSET dble
#define TPLSET_EleType double
#define TPLSET_EleCype const double
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
extern int dble2_cmpXY(Cdble2*l, double rx, double ry)
{
    double ret = l->x-rx;
    if ( ret==0 )
        ret = l->y-ry;
    if ( ret<0 )  return -1;
    if ( ret>0 )  return  1;
    return 0;
}

extern int dble2_cmpPT(Cdble2*l, Cdble2*r)
{ return dble2_cmpXY(l,r->x,r->y); }

extern int dble2_closeAtP(Cdble2*l, Cdble2*r, double p)
{
    double d = VECTOR_Norme2( l->x-r->x, l->y-r->y);
    return d<(p*p);
}

extern Tdble2 dble2_minXY        (Cdble2*s, int nb)
{
    TEMP_asf_printf(s==0 || nb<1,"expecting at least 1 element (found: s=%p nb=%d)",s,nb);
    int    i;
    Tdble2 min = s[0];
    for (i=1 ; i<nb ; i+=1) {
        if ( s[i].x<min.x ) min.x= s[i].x;
        if ( s[i].y<min.y ) min.y= s[i].y;
    }
    return min;
}

extern Tdble2 dble2_maxXY        (Cdble2*s, int nb)
{
    TEMP_asf_printf(s==0 || nb<1,"expecting at least 1 element (found: s=%p nb=%d)",s,nb);
    int    i;
    Tdble2 max = s[0];
    for (i=1 ; i<nb ; i+=1) {
        if ( s[i].x>max.x ) max.x= s[i].x;
        if ( s[i].y>max.y ) max.y= s[i].y;
    }
    return max;
}

extern void   dble2_minmaxXY    (Cdble2*s, int nb, Tdble2* _min, Tdble2* _max)
{
    TEMP_asf_printf(s==0 || nb<1,"expecting at least 1 element (found: s=%p nb=%d)",s,nb);
    int    i;
    Tdble2 min = s[0];
    Tdble2 max = s[0];
    for (i=1 ; i<nb ; i+=1) {
        if ( s[i].x<min.x ) min.x=s[i].x; else if ( s[i].x>max.x ) max.x=s[i].x;
        if ( s[i].y<min.y ) min.y=s[i].y; else if ( s[i].y>max.y ) max.y=s[i].y;
    }
    *_min = min;
    *_max = max;
}

extern Tdble2 dble2_barycentre  (Cdble2*s, int nb)
{
    TEMP_asf_printf(s==0 || nb<1,"expecting at least 1 element (found: s=%p nb=%d)",s,nb);
    int    i;
    Tdble2 center = s[0];
    for (i=1 ; i<nb ; i+=1) {
        center.x += s[i].x;
        center.y += s[i].y;
    }
    center.x /= nb;
    center.y /= nb;
    return center;
}

extern void   dble2_translation (Tdble2*s, int nb, double tx, double ty)
{ int i; for (i=0 ; i<nb ; i+=1) TRANSLATE_PXY(s[i],tx,ty); }

extern void   dble2_homothety00 (Tdble2*s, int nb, double f)
{ int i; for (i=0 ; i<nb ; i+=1) SCALE00_VXY(s[i],f,f); }

extern void   dble2_homothety   (Tdble2*s, int nb, double f, double cx, double cy)
{ int i; for (i=0 ; i<nb ; i+=1) SCALECC_PXY(s[i],f,f,cx,cy); }

extern void   dble2_scale00     (Tdble2*s, int nb, double fx, double fy)
{ int i; for (i=0 ; i<nb ; i+=1) SCALE00_VXY(s[i],fx,fy); }

extern void   dble2_scale       (Tdble2*s, int nb, double fx, double fy, double cx, double cy)
{ int i; for (i=0 ; i<nb ; i+=1) SCALECC_PXY(s[i],fx,fy,cx,cy); }

extern void   dble2_rotate      (Tdble2*s, int nb, double a, double cx, double cy)
{ int i; for (i=0 ; i<nb ; i+=1) ROTCC_PAXY(s[i],a,cx,cy); }

extern void   dble2_flip        (Tdble2*s, int nb, int h, int v, int r, double cx, double cy)
{ int i; for (i=0 ; i<nb ; i+=1) FLIP_PHVRXY(s[i],h,v,r,cx,cy); }

extern int dble2_searchFirstXY(Cdble2* s, int nb, double tolerance,
                               double x, double y, double *px, double *py)
{
    int i;
    Tdble2 pt = dble2_init(x,y);
    tolerance *= tolerance;
    for (i=0 ; i<nb ; i+=1) {
        Tdble2 v = dble2_initVab(pt,s[i]);
        if ( VECTOR_Norme2PT(v)<=tolerance ) {
            *px = s[i].x;
            *py = s[i].y;
            return i;
        }
    }
    return -1;
}

#define TPLSET dble2
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

extern void dble2set_addXY (Tdble2Set* s, double x, double y)
{ Tdble2 p = {x,y}; dble2set_add(s,p); }

extern void dble2set_addAfterXY (Tdble2Set* s, int pos, double x, double y)
{ Tdble2 p = {x,y}; dble2set_addAfter(s,pos,p); }

extern void   dble2set_minmaxXY   (Cdble2Set*s, Tdble2* min, Tdble2* max)
{ dble2_minmaxXY(s->eles,s->eleNb,min,max); }

extern void dble2set_translation  (Tdble2Set*s, double tx, double ty)
{
    int i;
    if (s==0) return ;
    for (i=0 ; i<s->eleNb ; i+=1) TRANSLATE_PXY(s->eles[i],tx,ty);
}

extern void dble2set_homothety00  (Tdble2Set*s, double f)
{
    int i;
    if (s==0) return ;
    for (i=0 ; i<s->eleNb ; i+=1) SCALE00_VXY(s->eles[i],f,f);
}

extern void dble2set_homothety    (Tdble2Set*s, double f, double cx, double cy)
{
    int i;
    if (s==0) return ;
    for (i=0 ; i<s->eleNb ; i+=1) SCALECC_PXY(s->eles[i],f,f,cx,cy);
}

extern void dble2set_scale00      (Tdble2Set*s, double fx, double fy)
{
    int i;
    if (s==0) return ;
    for (i=0 ; i<s->eleNb ; i+=1) SCALE00_VXY(s->eles[i],fx,fy);
}

extern void dble2set_scale        (Tdble2Set*s, double fx, double fy,
                                                double cx, double cy)
{
    int i;
    if (s==0) return ;
    for (i=0 ; i<s->eleNb ; i+=1) SCALECC_PXY(s->eles[i],fx,fy,cx,cy);
}

#if 0
extern TboxDble dble2set_bbox   (Cdble2Set*s)
{
    int i;
    if (s==0 || s->eleNb==0) return tool_boxDble_undef;
    Tdble2 ptmin=s->eles[0];
    Tdble2 ptmax=s->eles[0];
    for (i=1 ; i<s->eleNb ; i+=1) {
        Tdble2 pt = s->eles[i];
        if (pt.x<ptmin.x) ptmin.x=pt.x; else if (pt.x>ptmax.x) ptmax.x=pt.x;
        if (pt.y<ptmin.y) ptmin.y=pt.y; else if (pt.y>ptmax.y) ptmax.y=pt.y;
    }
    TboxDble ret; tool_boxDble_initPT(&ret,ptmin,ptmax); return ret;
}

extern TboxDble*dble2set_bboxptr(Cdble2Set*s,TboxDble*bb)
{ *bb=dble2set_bbox(s); return bb; }
#endif

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
#define TPLSET dble3
#include "tools/set-body-template.h"

extern void dble3set_addPTZ (Tdble3Set* s, Cdble2* p2,         double z)
{ Tdble3 p = {p2->x,p2->y,z}; dble3set_add(s,&p); }

extern void dble3set_addXYZ (Tdble3Set* s, double x, double y, double z)
{ Tdble3 p = {x,y,z}; dble3set_add(s,&p); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= pointer set                                                        =*/

#define TPLSET ptr
#define TPLSET_EleType void*
#define TPLSET_EleCype void*const
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

extern void       ptrset_nullTerminated(TptrSet* s)
{
    if ( (s->eleNb+1)<s->eleMemNb )
        s->eles[s->eleNb]=0;
    else {
        ptrset_add(s,NULL);
        s->eleNb -=1;
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= string set                                                         =*/

extern TastrSet*  astrset_newTable (CstrBriefT x, int n)
{
    TptrSet* s=ptrset_newTable((void**)x,n);
    TOOL_SET_LOOP(s,i,s->eles[i]=tool_strdup(s->eles[i]));
    return (TastrSet*)s;
}

extern void       astrset_appendT  (TastrSet* d, CstrBriefT x, int n)
{
    int i,nb=d->eleNb;
    ptrset_appendT((TptrSet*)d,(void**)x,n);
    for (i=0 ; i<n ; i+=1) 
        d->eles[nb+i]=tool_strdup(d->eles[nb+i]);
}

extern void       astrset_supLast  (TastrSet* s)
{
    int pos=s->eleNb-1;
    tool_free(s->eles[pos]);
    ptrset_supLast ((TptrSet*)s);
}

extern void       astrset_movI2Pshift(TastrSet* s, int idx, int pos)
{
    int i; char* tmp;
    TEMP_asf_printf(idx<0 || s->eleNb<=idx, "idx out of range [0:%d[",s->eleNb);
    TEMP_asf_printf(pos<0 || s->eleNb<=pos, "pos out of range [0:%d[",s->eleNb);
    if ( idx<pos ) {
        //   ----I-------P--------
        tmp = s->eles[idx];
        for ( i=idx ; i<pos ; i+=1 )
            s->eles[i] = s->eles[i+1];
        s->eles[pos] = tmp;
    } else if (idx>pos) {
        //   ----P-------I--------
        tmp = s->eles[idx];
        for ( i=idx ; i>pos ; i-=1 )
            s->eles[i] = s->eles[i-1];
        s->eles[pos] = tmp;
    }
}

extern TastrSet*  astrset_newStrWords(CstrBrief _str)
{
    TastrSet *s = astrset_new(10);
    if ( TOOL_strIsEmpty(_str) )
        return s;
    char *str = tool_strdup(_str);
    char* start = str;
    while (1) {
        char c,*end;
        while ( *start==' ' ) start+=1;
        if ( *start==0 ) break;
        for ( end=start+1 ; *end!=' ' && *end!=0 ; end+=1 ) ;
        c=*end; *end=0;
        astrset_add(s,start);
        *end=c;
        start=end;
        if (*start==0) break;
    }
    tool_free(str);
    return s;
}

extern void       astrset_add_printf    (TastrSet* s, CstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    TstrEaten str = tool_vaprintf(fmt,ap);
    va_end(ap);
    astrset_addEaten(s,str);
}

/*======================================================================*/
