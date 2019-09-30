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
 *      $Id: set-header-template.h 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/set-header-template.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Template of standard sets.                                         =*/
/*=                                                                    =*/
/*= For creating the header of a simple set:                           =*/
/*=                                                                    =*/
//=   struct _TxxxSet {
//=       int   eleMemNb; // element number of the memory block eles.
//=       int   eleNb;    // element number of set (eleNb<=eleMemNb)
//=       Txxx* eles;     // the set elements
//=   };
//=   extern TxxxSet*    xxxset_new        (int n);
//=   extern TxxxSet*    xxxset_newTable   (Cxxx*x, int n);
//=   extern TxxxSet*    xxxset_newClone   (CxxxSet* s);
//=   Inline void        xxxset_newORclr   (CxxxSet**s,int n); // new if *s==0 otherwise clear *s (empty set)
//=   Inline void        xxxset_clear      (TxxxSet* s);
//=   extern void        xxxset_delete     (TxxxSet* s);
//=   Inline void        xxxset_deleteS0   (TxxxSet**s); // delete(*s); *s=0
//=   extern void        xxxset_appendT    (TxxxSet* d, Cxxx* x, int n);
//=   Inline void        xxxset_append     (TxxxSet* d, CxxxSet* s);
//=   Inline void        xxxset_copy       (TxxxSet* d, CxxxSet* s);
//=   extern Txxx*       xxxset_first      (CxxxSet* s);
//=   extern Txxx*       xxxset_last       (CxxxSet* s);
//=   extern void        xxxset_add        (TxxxSet* s, Txxx*x);
//=   extern void        xxxset_addBefore  (TxxxSet* s, int pos, Txxx*x);
//=   extern void        xxxset_addAfter   (TxxxSet* s, int pos, Txxx*x);
//=   extern void        xxxset_supLast    (TxxxSet* s);
//=   extern void        xxxset_supQuick   (TxxxSet* s, int pos);
//=   extern void        xxxset_supShift   (TxxxSet* s, int pos);
//=   extern void        xxxset_movI2Pshift(TxxxSet* s, int idx, int pos);
/*=                                                                    =*/
/*= Usage 1:                                                           =*/
/*=     #define TPLSET_SetType TxxxSet                                 =*/
/*=     #define TPLSET_SetCype CxxxSet                                 =*/
/*=     #define TPLSET_EleType TxxxEle                                 =*/
/*=     #define TPLSET_EleCype CxxxEle                                 =*/
/*=     #define TPLSET_Func(f) xxxset_##f                              =*/
/*=     #include "set-header-template.h"                               =*/
/*=                                                                    =*/
/*= Usage 2:                                                           =*/
/*=     #define TPLSET xxx // create former macros that are undefined  =*/
/*=     #include "set-header-template.h"                               =*/
/*=                                                                    =*/
/*= Supported options:                                                 =*/
/*=                                                                    =*/
/*=  TPLSET_AddWithoutPtr: transmit the x element in all the add       =*/
/*=    functions by copy instead by reference.                         =*/
/*=      extern void  ...add... (..., TPLSET_EleType x);               =*/
/*=                                                                    =*/
/*=  TPLSET_WithRefcnt: add a reference counter in the set             =*/
/*=     Inline TxxxSet* xxxset_newLazy  (CxxxSet* s) { .. }            =*/
/*=                                                                    =*/
/*=                                                                    =*/
/*=  TPLSET_StackIntf: add a stack interface                           =*/
/*=     Inline Txxx*    xxxset_top    (CxxxSet* s) { .. }              =*/
/*=     Inline void     xxxset_push   (CxxxSet* s, Txxx*x) { .. }      =*/
/*=     Inline void     xxxset_pop    (CxxxSet* s) { .. }              =*/
/*=                                                                    =*/
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

struct CAT2(_,TPLSET_SetType) {
    int     eleMemNb;    // element number of the memory block eles.
    int     eleNb;       // element number of set (eleNb<eleMemNb)
    TPLSET_EleType*eles; // the set elements
#ifdef TPLSET_WithRefcnt
    int     refcnt;      // reference counter (-1: static allocated)
#endif
};

extern TPLSET_SetType* TPLSET_Func(new)     (int n);
extern TPLSET_SetType* TPLSET_Func(newTable)(TPLSET_EleCype* x, int n);
extern TPLSET_SetType* TPLSET_Func(newClone)(TPLSET_SetCype* s);
Inline void            TPLSET_Func(newORclr)(TPLSET_SetType**s, int n) { if (*s) s[0]->eleNb=0; else *s = TPLSET_Func(new)(n); }
Inline void            TPLSET_Func(clear)   (TPLSET_SetType* d) { if (d) d->eleNb=0; }
extern void            TPLSET_Func(delete)  (TPLSET_SetType* s);
Inline void            TPLSET_Func(deleteS0)(TPLSET_SetType**s) { TPLSET_Func(delete)(*s); *s=0; }
#ifdef TPLSET_WithRefcnt
Inline TPLSET_SetType* TPLSET_Func(newLazy)(TPLSET_SetCype* s) { TPLSET_SetType*ws=(TPLSET_SetType*)s; ws->refcnt+=1; return ws; }
#endif
extern void            TPLSET_Func(appendT)(TPLSET_SetType* d, TPLSET_EleType* x, int n);
Inline void            TPLSET_Func(append) (TPLSET_SetType* d, TPLSET_SetCype* s) { if (s==0) return; TPLSET_Func(appendT)(d,s->eles,s->eleNb); }
Inline void            TPLSET_Func(copy)   (TPLSET_SetType* d, TPLSET_SetCype* s) { d->eleNb = 0; TPLSET_Func(append)(d,s); }
extern TPLSET_EleType* TPLSET_Func(first)  (TPLSET_SetCype* s);
extern TPLSET_EleType* TPLSET_Func(last)   (TPLSET_SetCype* s);
#ifndef TPLSET_AddWithoutPtr
extern void            TPLSET_Func(add)      (TPLSET_SetType* s, TPLSET_EleCype*x);
extern void            TPLSET_Func(addBefore)(TPLSET_SetType* s, int pos, TPLSET_EleCype*x);
extern void            TPLSET_Func(addAfter) (TPLSET_SetType* s, int pos, TPLSET_EleCype*x);
#else
extern void            TPLSET_Func(add)      (TPLSET_SetType* s, TPLSET_EleType x);
extern void            TPLSET_Func(addBefore)(TPLSET_SetType* s, int pos, TPLSET_EleType x);
extern void            TPLSET_Func(addAfter) (TPLSET_SetType* s, int pos, TPLSET_EleType x);
#endif
extern void            TPLSET_Func(supLast)  (TPLSET_SetType* s);
extern void            TPLSET_Func(supQuick) (TPLSET_SetType* s, int pos);
extern void            TPLSET_Func(supShift) (TPLSET_SetType* s, int pos);

/*======================================================================*/
#ifdef TPLSET_StackIntf
Inline TPLSET_EleType* TPLSET_Func(top)  (TPLSET_SetType* s)
{
    return s->eles + s->eleNb-1;
}

Inline void            TPLSET_Func(pop)  (TPLSET_SetType* s)
{
     TPLSET_Func(supLast)( s );
}

Inline void            TPLSET_Func(push) (TPLSET_SetType* s, TPLSET_EleCype*x)
{
     TPLSET_Func(add)( s,x );
}
#endif

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

#ifdef  TPLSET_AddWithOutPtr
#error .............. TPLSET_AddWithOutPtr .........
#endif
/*======================================================================*/
// vim: set textwidth=120:
/*======================================================================*/
