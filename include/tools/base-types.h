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
 *      $Id: base-types.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/base-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= base types and base type sets.                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_BASE_TYPES_H
#define FILE_TOOLS_BASE_TYPES_H
#include "CPLUSPLUS.h"
/*======================================================================*/

// These macros are defined in the config.h header
// #define TOOL_DBG_SpOn   // enable the TEMP_...printf macros
// #define TOOL_DBG_Trace  // enable the DBG._printf macros
// #define TOOL_DBG_MemClr // enable the checking of color allocation
// #define TOOL_DBG_MemFnt // enable the checking of font allocation
// #define TOOL_DBG_MemObj // enable the checking of fig. object allocation
// #define TOOL_DBG_Dev    // enable the development features

/*======================================================================*/

#define Inline static inline
#define TOOL_TABLE_NbEle(t) ( sizeof(t)/sizeof(t[0]) )
#define TOOL_TABLE_ARG(t)   t,TOOL_TABLE_NbEle(t)

#define min2(x,y)  ((x)<(y)?(x):(y))
#define max2(x,y)  ((x)>(y)?(x):(y))
#define min3(x,y,z) min2(x,min2(y,z))
#define max3(x,y,z) max2(x,max2(y,z))
#define min4(a,b,c,d) min2(min2(a,b),min2(c,d))
#define max4(a,b,c,d) max2(max2(a,b),max2(c,d))
#define abs(x)  ((x)<0 ? -(x) : (x))

#define TOOL_SET_FIELDS(TeleType) int eleMemNb; int eleNb; TeleType*eles
#define TOOL_SET_LOOP(s,i,job) do { if ((s)!=0) { int i; for (i=0 ; i<(s)->eleNb ; i+=1) job; } } while(0)

/*======================================================================*/
/*= base numeric types                                                 =*/

typedef unsigned char  Tuint8;
typedef const Tuint8   Cuint8;
typedef signed char    Tsint8;
typedef const Tsint8   Csint8;

typedef unsigned short Tuint16;
typedef const Tuint16  Cuint16;
typedef signed short   Tsint16;
typedef const Tsint16  Csint16;

typedef unsigned int   Tuint32;
typedef const Tuint32  Cuint32;
typedef signed int     Tsint32;
typedef const Tsint32  Csint32;

typedef unsigned long  Tuint64;
typedef const Tuint64  Cuint64;
typedef signed long    Tsint64;
typedef const Tsint64  Csint64;

typedef struct _TsintSet     TsintSet;
typedef const   TsintSet     CsintSet;
typedef struct _TdbleSet     TdbleSet;
typedef const   TdbleSet     CdbleSet;

struct  _Tsint2 { Tsint32 x,y; };
typedef struct _Tsint2        Tsint2;
typedef const   Tsint2        Csint2;
typedef struct _Tsint2Set     Tsint2Set;
typedef const   Tsint2Set     Csint2Set;

Inline Tsint2 sint2_init   (double x,double y) { Tsint2 r={x,y}; return r; }
Inline Tsint2 sint2_initVab(Csint2 a,Csint2 b) { return sint2_init(b.x-a.x,b.y-a.y); }

struct  _Tdble2 { double x,y; };
typedef struct _Tdble2    Tdble2;
typedef const   Tdble2    Cdble2;
typedef struct _Tdble2Set Tdble2Set;
typedef const   Tdble2Set Cdble2Set;

extern Cdble2 dble2_origin;
Inline Tdble2 dble2_init   (double x,double y) { Tdble2 r={x,y}; return r; }
Inline Tdble2 dble2_initVab(Cdble2 a,Cdble2 b) { return dble2_init(b.x-a.x,b.y-a.y); }

struct  _Tdble3 { double x,y,z; };
typedef struct _Tdble3    Tdble3;
typedef const   Tdble3    Cdble3;
typedef struct _Tdble3Set Tdble3Set;
typedef const   Tdble3Set Cdble3Set;

Inline Tdble3 dble3_init   (double x,double y,double z) { Tdble3 r={x,y,z}; return r; }
Inline Tdble3 dble3_initVab(Cdble3 a,Cdble3 b) { return dble3_init(b.x-a.x,b.y-a.y,b.z-a.z); }

typedef struct _TboxDble  TboxDble;
typedef const   TboxDble  CboxDble;

typedef struct _TrecentFiles TrecentFiles;
typedef const   TrecentFiles CrecentFiles;

/*======================================================================*/
/*= miscellaneous base types container                                 =*/

typedef union _Tany Tany;
typedef const  Tany Cany;
union _Tany {
    unsigned long ulong;
    signed   long slong;
    void*         ptr;
    unsigned int  uint;
    signed   int  sint;
    unsigned char uchar;
    signed   char schar;
    float         freal;
    double        dreal;
    char*         str;
};
extern const Tany tool_any_null;
Inline Tany any_initBySInt(int  v) { Tany ret=tool_any_null; ret.sint = v; return ret; }
Inline Tany any_initBylong(long v) { Tany ret={ .ulong=0 }; return ret; }
Inline int  any_cmp(Tany l, Tany r) { return memcmp(&l,&r,sizeof(Tany)); }

/*======================================================================*/
/*= string types                                                       =*/
/*=                                                                    =*/
/*= Tstr      : shortcut to "char*".                                   =*/
/*= Cstr      : shortcut to "const char*".                             =*/
/*= TstrEaten  : memory allocated string.                              =*/
/*= C/TstrBrief: temporary string.                                     =*/
/*= CstrStatic : string has a long life time.                          =*/
/*=                                                                    =*/
/*= TstrEaten:                                                         =*/
/*=  - as function parameter                                           =*/
/*=    * the caller must provide a memory allocated string and must    =*/
/*=      never free it.                                                =*/
/*=    * the callee has the charge to free it after use.               =*/
/*=  - as return type of a function                                    =*/
/*=    * the caller has the charge to free it after use.               =*/
/*=    * the callee must return a memory allocated string and must     =*/
/*=      never free it.                                                =*/
/*=  - as structure field                                              =*/
/*=    the string must be free when the field is changed and when      =*/
/*=    the structure is freed.                                         =*/
/*=                                                                    =*/
/*= TstrBrief & CstrBrief:                                             =*/
/*=  - as function parameter                                           =*/
/*=    * the caller can change it as soon as the callee returns.       =*/
/*=    * the callee can use it, but does not store it directly.        =*/
/*=  - as return type of a function                                    =*/
/*=    calling the function may change the string returned by a        =*/
/*=    previous call to the function.                                  =*/
/*=  - as structure field                                              =*/
/*=    no meaning use char* or "const char*" or Tstr or Cstr instead.  =*/
/*=                                                                    =*/
/*= CstrBrief & TstrBrief:                                             =*/
/*=  - as function parameter                                           =*/
/*=    * the caller preserve the string until the free of the data     =*/
/*=      structure used by the callee.                                 =*/
/*=    * the callee can use it and stores it such as in its data       =*/
/*=      structure.                                                    =*/
/*=  - as return type of a function                                    =*/
/*=    * the caller can use it and stores it such as in its data       =*/
/*=      structure.                                                    =*/
/*=    * the callee preserve the string until the free of the data     =*/
/*=      structure used by the callee.                                 =*/
/*=  - as structure field                                              =*/
/*=    the string must not be free when the field is changed or when   =*/
/*=    the structure is freed.                                         =*/
typedef char*        Tstr; 
typedef const char*  Cstr; 
typedef char*        TstrEaten; 
typedef char*        TstrBrief; 
typedef const char*  CstrBrief; 
typedef       char*  TstrStatic; 
typedef const char*  CstrStatic; 

/**
 * The TOOL_strIsEmpty macro is true if s is null or s is empty
 * otherwise it is false.
 *
 * The TOOL_strIsEmptyLine macro as the tool_strIsEmptyLine function
 * is true when one of the next conditions is true otherwise it is false.
 *  - s is the null pointer.
 *  - s contains only characters among: ' ', '\t', '\r', '\n'.
 *
 * The tool_strfreeS0 function does nothing if *pp is the NULL pointer
 * otherwise it frees *pp and sets *pp to the NULL pointer.
 * The tool_strdup function is similar to the strdup function but:
 *    - it prints an error message and exits when no more memory is available.
 *    - and "tool_strdup(NULL)" returns NULL.
 * The tool_strchgEaten function sets *des to src freeing *des if needed.
 * This function assumes that both *des and src are "malloced" strings.
 * The tool_strchgClone function sets *des to a clone of src freeing
 * *des if needed.
 *
 * The tool_aprintf function is similar to sprintf except that the buffer
 * is dynamically allocated and returned.
 * Return value: the buffer on success, otherwise the null pointer.
 * The tool_vaprintf function is similar to tool_aprintf except that the list
 * of arguments to print are given through the ap parameter.
**/
#define          TOOL_strIsEmpty(s) ((s)==0 || *(s)==0)
#define          TOOL_strIsEmptyLine(s) ((s)==0 || *(s)==0 || tool_strIsEmptyLine(s) )
extern int       tool_strIsEmptyLine(CstrBrief s);

Inline void      tool_strfreeS0(char**pp) { if (*pp!=0) { free(*pp); *pp=0; } }
extern TstrEaten tool_strdup(CstrBrief str);
Inline int       tool_strcmp(CstrBrief l, CstrBrief r)          { return strcmp(l?l:"",r?r:""); }
Inline void      tool_strchgEaten(TstrEaten*des, TstrEaten src) { if ( *des==src ) return; if (*des) free(*des); *des=src; }
Inline void      tool_strchgClone(TstrEaten*des, CstrBrief src) { if (*des) free(*des); *des=tool_strdup(src); }
extern TstrEaten tool_aprintf (CstrBrief fmt, ...);
extern TstrEaten tool_vaprintf(CstrBrief fmt, va_list ap);

/*======================================================================*/
/*= Direction based on wind rose                                       =*/

typedef enum {
    DirWR_NN=0x01, DirWR_EE=0x02, DirWR_SS=0x04, DirWR_WW=0x08, // North, East ...
    DirWR_NE=DirWR_NN|DirWR_EE,  // North-East
    DirWR_NW=DirWR_NN|DirWR_WW,  // North-West
    DirWR_SE=DirWR_SS|DirWR_EE,  // Sud-East
    DirWR_SW=DirWR_SS|DirWR_WW,  // Sud-West
    DirWR_CC=0x10                // center
} TdirWR;

/**
 * The tool_dirWR_toStr function returns a static string that is human readable
 * corresponding to the wrd direction (eg: North, North-East,...).
 *
 * The tool_dirWR_table function returns a static array of the wind rose
 * directions.
 * The size of the returned array is TOOL_DirWR_NB (9).
 * The DirWR_CC (center) direction is placed in first position.
 *
 * The tool_dirWR_toStrTable function returns a dynamically allocated array of
 * static strings that are human readable (got with tool_dirWR_toStr).
 * The size of the returned array is TOOL_DirWR_NB (9) and NULL terminated.
 * The direction orders of the returned array and of the array returned by the
 * tool_dirWR_table function are the same.
 * The caller must free the array after use using the free or tool_free
 * function.
**/
#define TOOL_DirWR_NB 9

extern const TdirWR*  tool_dirWR_table     ();
extern const char*    tool_dirWR_toStr     (TdirWR dwr);
extern const char**   tool_dirWR_toStrTable();

/*======================================================================*/
/*= memory allocation and freeing                                      =*/
/*=                                                                    =*/
/**
 * The tool_malloc function is similar to malloc but prints a fatal
 * message using the tool_spf_printf function when the memory
 * allocation failed.
 *
 * The tool_malloc_0 function is similar to tool_malloc but fills
 * up all the bytes of the memory block with low value ('\0').
 *
 * The tool_free macro frees the memory location pointed to
 * by the p pointer if the p pointer is not NULL. Otherwise
 * it does nothing.
 *
 * The tool_freeS0 function frees the memory location pointed to
 * by the *p pointer and then it sets the *p pointer to NULL.
 * if the *p pointer is NULL, the function does nothing.
**/
extern void* tool_malloc  (size_t n);
extern void* tool_malloc_0(size_t n);
extern void* tool_realloc (void*p, size_t n);

#define     tool_free(p)   do { if ( (p)!=0 ) free(p); } while (0)
Inline void tool_freeS0 (void**pp) { if ( *pp ) { free(*pp); *pp=0; } }

/**
 * The tool_free_addPU (resp: tool_free_addUP) stores the triplet
 * (p,freefunc,ud) until the tool_free_doit(p) is called that performs
 * freefunc(p,ud) (resp: freefunc(ud,p)).
 * The freefunc(p) function does nothing if no (p,freefunc,ud) triplet
 * exists.
**/
extern void tool_free_addPU(void*p,void*freefunc,void*ud);
extern void tool_free_addUP(void*p,void*freefunc,void*ud);
extern void tool_free_doit (void*p);

/*======================================================================*/
/*= base type sets.                                                    =*/

#define TPLSET sint
#define TPLSET_EleType int
#define TPLSET_EleCype const int
#define TPLSET_AddWithoutPtr
#include "set-header-template.h"

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
#define TPLSET dble
#define TPLSET_EleType double
#define TPLSET_EleCype const double
#define TPLSET_AddWithoutPtr
#include "set-header-template.h"

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
extern int dble2_cmpXY(Cdble2*l, double rx, double ry);
extern int dble2_cmpPT(Cdble2*l, Cdble2*r);
extern int dble2_closeAtP(Cdble2*l, Cdble2*r, double p);

extern Tdble2 dble2_minXY       (Cdble2*s, int nb);
extern Tdble2 dble2_maxXY       (Cdble2*s, int nb);
extern void   dble2_minmaxXY    (Cdble2*s, int nb, Tdble2* min, Tdble2* max);
extern Tdble2 dble2_barycentre  (Cdble2*s, int nb);
extern void   dble2_translation (Tdble2*s, int nb, double tx, double ty);
extern void   dble2_homothety00 (Tdble2*s, int nb, double f);
extern void   dble2_homothety   (Tdble2*s, int nb, double f, double cx, double cy);
extern void   dble2_scale00     (Tdble2*s, int nb, double fx, double fy);
extern void   dble2_scale       (Tdble2*s, int nb, double fx, double fy, double cx, double cy);
extern void   dble2_rotate      (Tdble2*s, int nb, double a, double cx, double cy);
extern void   dble2_flip        (Tdble2*s, int nb, int h, int v, int r, double cx, double cy);

/**
 * The dble2_searchFirstXY function searches from s[0] to s[nb-1] the first
 * P point that verifies: distance(P,(x,y))<=tolerance.
 *
 * Return:
 *   On success (a such P point exists), it returns its index in s and
 *   P into *px, *py.
 *   Otherwise (a such P point does not exist), it returns -1 and *px
 *   and *py are left unchanged.
**/
extern int    dble2_searchFirstXY (Cdble2* s, int nb, double tolerance,
                                   double x, double y,
                                   double *px, double *py);
Inline int    dble2_searchFirstPT (Cdble2* s, int nb, double tolerance,
                                   Tdble2 pt, Tdble2* rpt)
{ return dble2_searchFirstXY(s,nb,tolerance,pt.x,pt.y,&rpt->x,&rpt->y); }

#define TPLSET dble2
#define TPLSET_AddWithoutPtr
#include "set-header-template.h"

#define       dble2set_clear(s)   (s)->eleNb=0
extern void   dble2set_addXY      (Tdble2Set* s, double x, double y);
extern void   dble2set_addBeforeXY(Tdble2Set* s, int pos, double x, double y);
extern void   dble2set_addAfterXY (Tdble2Set* s, int pos, double x, double y);
#define       dble2set_addPT       dble2set_add
#define       dble2set_addBeforePT dble2set_addBefore
#define       dble2set_addAfterPT  dble2set_addAfter

Inline Tdble2 dble2set_minXY       (Cdble2Set*s) { return dble2_minXY(s->eles,s->eleNb); }
Inline Tdble2 dble2set_maxXY       (Cdble2Set*s) { return dble2_maxXY(s->eles,s->eleNb); }
extern void   dble2set_minmaxXY    (Cdble2Set*s, Tdble2* min, Tdble2* max);
Inline Tdble2 dble2set_barycentre  (Cdble2Set*s) { return dble2_barycentre(s->eles, s->eleNb); }
extern void   dble2set_translation (Tdble2Set*s, double tx, double ty);
extern void   dble2set_homothety00 (Tdble2Set*s, double f);
extern void   dble2set_homothety   (Tdble2Set*s, double f, double cx, double cy);
extern void   dble2set_scale00     (Tdble2Set*s, double fx, double fy);
extern void   dble2set_scale       (Tdble2Set*s, double fx, double fy, double cx, double cy);
Inline void   dble2set_rotate      (Tdble2Set*s, double a, double cx, double cy)            { if (s) dble2_rotate(s->eles,s->eleNb,a,cx,cy); }
Inline void   dble2set_flip        (Tdble2Set*s, int h, int v, int r, double cx, double cy) { if (s) dble2_flip  (s->eles,s->eleNb,h,v,r,cx,cy); }

/**
 * The dble2set_searchFirstXY function is a wrapper to the 
 * dble2_searchFirstXY function.
 **/
Inline int dble2set_searchFirstXY (Cdble2Set* s, double tolerance,
                                   double x, double y,
                                   double *px, double *py)
{ return dble2_searchFirstXY(s->eles,s->eleNb,tolerance,x,y,px,py); }

Inline int dble2set_searchFirstPT (Cdble2Set* s, double tolerance,
                                   Tdble2 pt, Tdble2* rpt)
{ return dble2_searchFirstPT(s->eles,s->eleNb,tolerance,pt,rpt); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
extern int dble3_cmpXY(Cdble3*l, double rx, double ry, double rz);
extern int dble3_cmpPT(Cdble3*l, Cdble3*r);

#define TPLSET dble3
#include "set-header-template.h"

#define     dble3set_clear(s) (s)->eleNb=0
extern void dble3set_addPTZ (Tdble3Set* s, Cdble2* p,          double z);
extern void dble3set_addXYZ (Tdble3Set* s, double x, double y, double z);

#define dble3set_addPT dble3set_add

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= pointer set                                                        =*/

typedef struct _TptrSet     TptrSet;
typedef const   TptrSet     CptrSet;

#define TPLSET ptr
#define TPLSET_EleType void*
#define TPLSET_EleCype void*const
#define TPLSET_AddWithoutPtr
#include "set-header-template.h"

/**
 * The ptrrset_nullTerminated function appends a null pointer to the s
 * pointer set. The size of the set is left unchanged.
**/
extern void       ptrset_nullTerminated(TptrSet* s);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= string set                                                         =*/
/*=                                                                    =*/
/*= TastrSet: set of Allocated STRings that are freed when an element  =*/
/*=           is suppressed or when the set is deleted.                =*/

// make TsrSet & TptrSet incompatible
struct  _TastrSet { TOOL_SET_FIELDS(TstrEaten); };
typedef struct _TastrSet     TastrSet;
typedef const   TastrSet     CastrSet;

typedef char*const* CstrBriefT;

Inline TastrSet*  astrset_new             (int n)              { return (TastrSet*)ptrset_new(n); }
extern TastrSet*  astrset_newTable        (CstrBriefT x, int n);
Inline TastrSet*  astrset_newClone        (CastrSet* s)        { return s==0 ?  astrset_new(5) : astrset_newTable(s->eles,s->eleNb); } 
Inline void       astrset_newORclr        (CastrSet**s,int n)  { TOOL_SET_LOOP(*s,i,tool_free((*s)->eles[i])); ptrset_newORclr((TptrSet**)s,n); }
Inline void       astrset_clear           (TastrSet* s)        { TOOL_SET_LOOP( s,i,tool_free(  s ->eles[i])); ptrset_clear((TptrSet*)s); }
Inline void       astrset_delete          (TastrSet* s)        { TOOL_SET_LOOP( s,i,tool_free(  s ->eles[i])); ptrset_delete  ((TptrSet*) s); }
Inline void       astrset_deleteS0        (TastrSet**s)        { TOOL_SET_LOOP(*s,i,tool_free((*s)->eles[i])); ptrset_deleteS0((TptrSet**)s); }
extern void       astrset_appendT         (TastrSet* d, CstrBriefT x, int n); 
Inline void       astrset_append          (TastrSet* d, CastrSet* s)          { if (s!=0) astrset_appendT(d,s->eles,s->eleNb); }
Inline void       astrset_copy            (TastrSet* d, CastrSet* s)          { astrset_clear(d); astrset_append(d,s); }
Inline TstrEaten* astrset_first           (CastrSet* s)                       { return s->eleNb!=0 ? s->eles+0 : 0; }
Inline TstrEaten* astrset_last            (CastrSet* s)                       { return s->eleNb!=0 ? s->eles+s->eleNb-1 : 0; }
Inline void       astrset_addEaten        (TastrSet* s, TstrEaten x)          { ptrset_add((TptrSet*)s,x); }
Inline void       astrset_addClone        (TastrSet* s, CstrBrief x)          {  astrset_addEaten(s,tool_strdup(x) ); }
Inline void       astrset_add             (TastrSet* s, CstrBrief x)          {  astrset_addEaten(s,tool_strdup(x) ); }
Inline void       astrset_addBeforeEaten  (TastrSet* s, int pos, TstrEaten x) { ptrset_addBefore((TptrSet*)s,pos,x); }
Inline void       astrset_addBeforeClone  (TastrSet* s, int pos, CstrBrief x) { ptrset_addBefore((TptrSet*)s,pos,tool_strdup(x)); }
Inline void       astrset_addBefore       (TastrSet* s, int pos, CstrBrief x) { ptrset_addBefore((TptrSet*)s,pos,tool_strdup(x)); }
Inline void       astrset_addAfterEaten   (TastrSet* s, int pos, TstrEaten x) { ptrset_addAfter ((TptrSet*)s,pos,x); }
Inline void       astrset_addAfterClone   (TastrSet* s, int pos, CstrBrief x) { ptrset_addAfter ((TptrSet*)s,pos,tool_strdup(x)); }
Inline void       astrset_addAfter        (TastrSet* s, int pos, CstrBrief x) { ptrset_addAfter ((TptrSet*)s,pos,tool_strdup(x)); }
Inline int        astrset_getPos          (TastrSet* s, CstrBrief x)          { TOOL_SET_LOOP( s,i,if (strcmp(x,s->eles[i])==0) return i;); return -1; }
extern void       astrset_supLast         (TastrSet* s);
Inline void       astrset_supQuick        (TastrSet* s, int pos)              { tool_free(s->eles[pos]); ptrset_supQuick((TptrSet*)s,pos); }
Inline void       astrset_supShift        (TastrSet* s, int pos)              { tool_free(s->eles[pos]); ptrset_supShift((TptrSet*)s,pos); }
extern void       astrset_movI2Pshift     (TastrSet* s, int idx, int pos);

/**
 * The astrset_newStrWords function returns a string set that contains
 * all the words of the str strings. The word delimiter is ' '.
 *
 * The astrset_add_printf function builds a string as printf does it
 * and adds it to the s string set.
 *
 * The astrset_nullTerminated function appends a null pointer to the s
 * string set. The size of the set is left unchanged.
**/
extern TastrSet*  astrset_newStrWords   (CstrBrief str);
extern void       astrset_add_printf    (TastrSet* s, CstrBrief fmt, ...);
Inline void       astrset_nullTerminated(TastrSet* s) { ptrset_nullTerminated((TptrSet*)s); }

/*======================================================================*/
/*= Interface for printing messages                                    =*/
/*=                                                                    =*/
/*= The tool_usridt_printf function builds a string using the fmt      =*/
/*= and the ... arguments as printf does. Then it sends the string to  =*/
/*= all the message drivers of the drv message driver list.            =*/
/*= Those message drivers print the string somewhere or do not print   =*/
/*= it depending on their configuration.                               =*/
/*= See the messagedriver.h header for documentation about the message =*/
/*= driver configuration.                                              =*/
/*=                                                                    =*/
/*= The tool_usrmsg_printf function is similar to the                  =*/
/*= tool_usridt_printf function but each invocation ends a line.       =*/
/*=                                                                    =*/
/*= The tool_xxxerr_printf functions are similar to the                =*/
/*= tool_usridt_printf function except that                            =*/
/*=   - Each invocation ends a line.                                   =*/
/*=   - xxx gives the error level (inf=information, wrn=warning,       =*/
/*=     err=error, ftl=fatal).                                         =*/
/*=   - Depending on the driver configuration the application name     =*/
/*=     and the error level can be printed before the message          =*/
/*=   - The message printing depends of the verbosity level of the     =*/
/*=     message driver,                                                =*/
/*=                                                                    =*/
/*= void tool_usridt_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*= void tool_usrmsg_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*= void tool_inferr_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*= void tool_wrnerr_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*= void tool_errerr_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*= void tool_ftlerr_printf(Tmsgdrv*drv,CstrBrief fmt, ...);           =*/
/*=                                                                    =*/
/*= The tool_xxxxxx_vprintf functions are similar to their             =*/
/*= corresponding tool_xxxxxx_vprintf function except that the message =*/
/*= arguments are grouped into the ap parameter.                       =*/
/*=                                                                    =*/
/*= void tool_usridt_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*= void tool_usrmsg_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*= void tool_inferr_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*= void tool_wrnerr_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*= void tool_errerr_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*= void tool_ftlerr_vprintf(Tmsgdrv*drv,CstrBrief fmt, ...);          =*/
/*=                                                                    =*/

typedef struct _Tmsgdrv Tmsgdrv;
typedef const   Tmsgdrv Cmsgdrv;

#define TOOL_MSGDRV_FandVF_inline(t,f,vf) \
    extern void vf(t*drv,CstrBrief fmt,va_list ap); \
    Inline void f (t*drv,CstrBrief fmt,...) {\
        va_list ap; va_start(ap,fmt); vf(drv,fmt,ap); va_end(ap); }

TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_usridt_printf,tool_usridt_vprintf)
TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_usrmsg_printf,tool_usrmsg_vprintf)
TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_inferr_printf,tool_inferr_vprintf)
TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_wrnerr_printf,tool_wrnerr_vprintf)
TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_errerr_printf,tool_errerr_vprintf)
TOOL_MSGDRV_FandVF_inline(Tmsgdrv,tool_ftlerr_printf,tool_ftlerr_vprintf)

extern void tool_errerr_nomem  (Tmsgdrv* drv);
extern void tool_ftlerr_nomem  (Tmsgdrv* drv);

#define TOOL_MSGDRV_FandVF_redirect(t,f,vf,tvf,get) \
    Inline void  f (t*o,CstrBrief fmt,...) {\
        va_list ap; va_start(ap,fmt); tvf(get,fmt,ap); va_end(ap); } \
    Inline void vf(t*o,CstrBrief fmt,va_list ap) { tvf(get,fmt,ap); }

/*======================================================================*/
/*= bug messages, must be used when Software Problems occur.           =*/

/* all the next macros use this function */
extern void tool_bug_printf(int stop, CstrBrief level, CstrBrief file,
                            int line, CstrBrief func,  CstrBrief fmt,...);

/* use these macros for a permanent track */
#define PERM_spw_printf(fmt,...) tool_bug_printf(0,"Warning",__FILE__,__LINE__,__func__,fmt,__VA_ARGS__)
#define PERM_spe_printf(fmt,...) tool_bug_printf(0,"Error  ",__FILE__,__LINE__,__func__,fmt,__VA_ARGS__)
#define PERM_spf_printf(fmt,...) tool_bug_printf(1,"Fatal  ",__FILE__,__LINE__,__func__,fmt,__VA_ARGS__)
#define PERM_spe_NYI(...)        PERM_spe_printf("not yet implemented",__VA_ARGS__)
#define PERM_spf_NYI(...)        PERM_spf_printf("not yet implemented",__VA_ARGS__)
#define PERM_error_NYI()         PERM_spe_printf("%s","not yet implemented")
#define PERM_fatal_NYI()         PERM_spf_printf("%s","not yet implemented")
/* use these macros for a track in debug mode */
#if defined(TOOL_DBG_SpOn) && TOOL_DBG_SpOn
#define TEMP_spw_printf(fmt,...) PERM_spw_printf(fmt,__VA_ARGS__)
#define TEMP_spe_printf(fmt,...) PERM_spe_printf(fmt,__VA_ARGS__)
#define TEMP_spf_printf(fmt,...) PERM_spf_printf(fmt,__VA_ARGS__)
#else
#define TEMP_spw_printf(fmt,...) while(0)
#define TEMP_spe_printf(fmt,...) while(0)
#define TEMP_spf_printf(fmt,...) while(0)
#endif
/* same as former but conditional (message is triggered if cond is true) */
#define PERM_asw_printf(cond,fmt,...) do if (cond) PERM_spw_printf(fmt,__VA_ARGS__); while(0)
#define PERM_ase_printf(cond,fmt,...) do if (cond) PERM_spe_printf(fmt,__VA_ARGS__); while(0)
#define PERM_asf_printf(cond,fmt,...) do if (cond) PERM_spf_printf(fmt,__VA_ARGS__); while(0)
#if defined(TOOL_DBG_SpOn) && TOOL_DBG_SpOn
#define TEMP_asw_printf(cond,fmt,...) do if (cond) PERM_spw_printf(fmt,__VA_ARGS__); while(0)
#define TEMP_ase_printf(cond,fmt,...) do if (cond) PERM_spe_printf(fmt,__VA_ARGS__); while(0)
#define TEMP_asf_printf(cond,fmt,...) do if (cond) PERM_spf_printf(fmt,__VA_ARGS__); while(0)
#else
#define TEMP_asw_printf(cond,fmt,...) while(0)
#define TEMP_ase_printf(cond,fmt,...) while(0)
#define TEMP_asf_printf(cond,fmt,...) while(0)
#endif

/*======================================================================*/
/*= debug messages, must be used for printing debug information.       =*/

/* all the next macros use this function */
extern int  tool_dbg_level;     // default is 0
extern void tool_dbg_printf(
    CstrBrief tag,   // message is printed if tag==0 or getenv(tag)!=0
    int       level, // message is printed if level < tool_dbg_level
    CstrBrief file,  // the file name
    int       line,  // the line number in file
    CstrBrief func,  // the function name
    CstrBrief fmt,   // the format (same as printf)
    ...);            // the arguments specified by fmt
extern void tool_dbg_vprintf(CstrBrief tag,  int level, CstrBrief file,
                             int line, CstrBrief func, CstrBrief fmt,
                             va_list ap);

#if defined(TOOL_DBG_Trace) && TOOL_DBG_Trace
#define DBG_printf( tag,level,fmt,...) tool_dbg_printf(tag,level,__FILE__,__LINE__,__PRETTY_FUNCTION__,fmt,__VA_ARGS__)
#else
#define DBG_printf( tag,level,fmt,...) while(0)
#endif
#define DBGS_printf(          fmt,...) DBG_printf( (void*)0,0,fmt,__VA_ARGS__)
#define DBG0_printf(tag,      fmt,...) DBG_printf( tag,0,fmt,__VA_ARGS__)
#define DBG1_printf(tag,      fmt,...) DBG_printf( tag,1,fmt,__VA_ARGS__)
#define DBG2_printf(tag,      fmt,...) DBG_printf( tag,2,fmt,__VA_ARGS__)

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif  // FILE_TOOLS_BASE_TYPES_H
/*======================================================================*/
/*= vim: set textwidth=0:                                              =*/
