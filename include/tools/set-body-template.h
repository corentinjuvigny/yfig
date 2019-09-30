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
 *      $Id: set-body-template.h 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/set-body-template.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Template of standard sets.                                         =*/
/*=                                                                    =*/
/*= See the set-header-template.h header for documentation.            =*/
/*======================================================================*/

#undef  CAT2
#undef  CAT2b
#undef  CAT3
#undef  CAT3b

#define CAT2(x,y)   CAT2b(x,y)
#define CAT2b(x,y)  x##y
#define CAT3(x,y,z) CAT3b(x,y,z)
#define CAT3b(x,y,z) x##y##z

#ifdef TPLSET

#ifndef TPLSET_SetType
#define TPLSET_SetType CAT3(T,TPLSET,Set)
#endif

#ifndef TPLSET_SetCype
#define TPLSET_SetCype CAT3(C,TPLSET,Set)
#endif

#ifndef TPLSET_EleType
#define TPLSET_EleType CAT2(T,TPLSET)
#endif

#ifndef TPLSET_EleCype
#define TPLSET_EleCype CAT2(C,TPLSET)
#endif

#ifndef TPLSET_Func
#define TPLSET_Func(f) CAT3(TPLSET,set_,f)
#endif

#endif

/*======================================================================*/
/*= Constructor/Destructor                                             =*/

extern TPLSET_SetType*  TPLSET_Func(new) (int n)
{
    TPLSET_SetType* ret = tool_malloc(sizeof(*ret));
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? tool_malloc(sizeof(*ret->eles)*n) : 0;
#ifdef TPLSET_WithRefcnt
    ret->refcnt = 1;
#endif
    return ret;
}

extern TPLSET_SetType* TPLSET_Func(newTable)(TPLSET_EleCype* x, int n)
{
    TPLSET_SetType* ret = tool_malloc(sizeof(*ret));
    ret->eleMemNb  = n;
    ret->eleNb     = n;
    if ( n!=0 ) {
        int sz = sizeof(*ret->eles)*n;
        ret->eles      = tool_malloc(sz);
        memcpy(ret->eles,x,sz);
    } else
        ret->eles = 0;
#ifdef TPLSET_WithRefcnt
    ret->refcnt = 1;
#endif
    return ret;
}

extern TPLSET_SetType* TPLSET_Func(newClone)(TPLSET_SetCype* s)
{
    if ( s==0 ) return 0;
    TPLSET_SetType* ret = tool_malloc(sizeof(*ret));
    ret->eleMemNb  = s->eleNb;
    ret->eleNb     = s->eleNb;
    if ( s->eleNb!=0 ) {
        int sz = sizeof(*ret->eles)*s->eleNb;
        ret->eles      = tool_malloc(sz);
        memcpy(ret->eles,s->eles,sz);
    } else
        ret->eles = 0;
#ifdef TPLSET_WithRefcnt
    ret->refcnt = 1;
#endif
    return ret;
}

extern void      TPLSET_Func(delete)   (TPLSET_SetType* s)
{
    if (s==0) return;
#ifdef TPLSET_WithRefcnt
    if ( s->refcnt<=0 ) return;
    s->refcnt -= 1;
    if ( s->refcnt>0 ) return;
#endif
    if ( s->eles ) free(s->eles);
    free(s);
}

extern void            TPLSET_Func(appendT)(TPLSET_SetType* d, TPLSET_EleType* x, int n)
{
    int sz = sizeof(d->eles[0]);
    int to = d->eleNb;
    d->eleNb += n;
    if ( d->eleNb > d->eleMemNb ) {
        d->eleMemNb = d->eleNb;
        d->eles = tool_realloc(d->eles,sz*d->eleMemNb);
    }
    memcpy(d->eles+to,x,sz*n);
}

extern TPLSET_EleType* TPLSET_Func(first) (TPLSET_SetCype* s)
{ return s->eleNb!=0 ? s->eles+0 : 0; }

extern TPLSET_EleType* TPLSET_Func(last)  (TPLSET_SetCype* s)
{ return s->eleNb!=0 ? s->eles+s->eleNb-1 : 0; }

/*======================================================================*/
/*= Adding element.                                                    =*/

#ifndef TPLSET_AddWithoutPtr
extern void            TPLSET_Func(add)      (TPLSET_SetType* s, TPLSET_EleCype*x)
#else
extern void            TPLSET_Func(add)      (TPLSET_SetType* s, TPLSET_EleType x)
#endif
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles = tool_realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
#ifndef TPLSET_AddWithoutPtr
    s->eles[s->eleNb] = *x;
#else
    s->eles[s->eleNb] = x;
#endif
    s->eleNb += 1;
}

#ifndef TPLSET_AddWithoutPtr
extern void            TPLSET_Func(addBefore)(TPLSET_SetType* s, int pos, TPLSET_EleCype*x)
#else
extern void            TPLSET_Func(addBefore)(TPLSET_SetType* s, int pos, TPLSET_EleType x)
#endif
{
    int i;
    TEMP_asf_printf(pos<0 || s->eleNb<=pos, "pos out of range [0:%d[",s->eleNb);
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles = tool_realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    for ( i=s->eleNb ; i>pos ; i-=1 )
        s->eles[i] = s->eles[i-1];
#ifndef TPLSET_AddWithoutPtr
    s->eles[pos] = *x;
#else
    s->eles[pos] = x;
#endif
    s->eleNb += 1;
}

#ifndef TPLSET_AddWithoutPtr
extern void            TPLSET_Func(addAfter) (TPLSET_SetType* s, int pos, TPLSET_EleCype*x)
#else
extern void            TPLSET_Func(addAfter) (TPLSET_SetType* s, int pos, TPLSET_EleType x)
#endif
{
    int i;
    TEMP_asf_printf(pos<0 || s->eleNb<=pos, "pos out of range [0:%d[",s->eleNb);
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles = tool_realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    for ( i=s->eleNb ; i>(pos+1) ; i-=1 )
        s->eles[i] = s->eles[i-1];
#ifndef TPLSET_AddWithoutPtr
    s->eles[pos+1] = *x;
#else
    s->eles[pos+1] = x;
#endif
    s->eleNb += 1;
}

/*======================================================================*/
/*= Suppressing element.                                               =*/

extern void            TPLSET_Func(supLast)  (TPLSET_SetType* s)
{
    TEMP_asf_printf(s->eleNb<=0, "%s", "empty set");
    s->eleNb -= 1;
}
extern void            TPLSET_Func(supQuick) (TPLSET_SetType* s, int pos)
{
    TEMP_asf_printf(pos<0 || s->eleNb<=pos, "pos out of range [0:%d[",s->eleNb);
    s->eleNb -= 1;
    s->eles[pos] = s->eles[s->eleNb];
}

extern void            TPLSET_Func(supShift) (TPLSET_SetType* s, int pos)
{
    int i;
    TEMP_asf_printf(pos<0 || s->eleNb<=pos, "pos out of range [0:%d[",s->eleNb);
    s->eleNb -= 1;
    for ( i=pos ; i<s->eleNb ; i+=1 )
        s->eles[i] = s->eles[i+1];
}

/*======================================================================*/
/*= Moving element.                                                    =*/

extern void            TPLSET_Func(movI2Pshift) (TPLSET_SetType* s, int idx, int pos)
{
    int i; TPLSET_EleType tmp;
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

/*======================================================================*/

#undef  CAT2
#undef  CAT2b
#undef  CAT3
#undef  CAT3b

#undef TPLSET
#undef TPLSET_SetType
#undef TPLSET_SetCype
#undef TPLSET_EleType
#undef TPLSET_EleCype
#undef TPLSET_Func
#undef TPLSET_AddWithoutPtr
#undef TPLSET_WithRefcnt
#undef TPLSET_StackIntf

/*======================================================================*/
// vim: set textwidth=120:
/*======================================================================*/
