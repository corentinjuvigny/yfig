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
 *      $Id: range.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/range.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utility for defining a value with lower and upper bounds.          =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_RANGE_H
#define FILE_TOOLS_RANGE_H
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
/*= Some default values                                                =*/ 

#define RANGE_DF_ISTEP  1       // step when auto inc/dec/rement
#define RANGE_DF_FSTEP  (9./8.) // step when timed or divised

/*======================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*======================================================================*/

typedef struct _Trange Trange;
typedef const   Trange Crange;

#define RANGE_FL_Mult       0x001   // iof is a factor (value is
                                    // timed or divised by iof)
#define RANGE_FL_Inc        0x002   // iof is an increment.
#define RANGE_FL_LowBound   0x010   // has a lower bound
#define RANGE_FL_UppBound   0x020   // has a upper bound
#define RANGE_FL_SynDble    0x100   // syncrhronize with an external
#define RANGE_FL_SynInt     0x200   // double, int or
#define RANGE_FL_SynChar    0x400   // char variable.
#define RANGE_FL_Syn        0x700

/*======================================================================*/

struct _Trange {
    int         flag;     // see RANGE_FL_xxx
    char        nAll;     // nb digits (dot included)
    char        nBdt;     // nb digits before the dot
    char        nAdt;     // nb digits after the dot.
                          // nAdt==0 --> integer
    double      val;      // value of spiner.
    double      min;      // minimum allowed value (included)
    double      max;      // maximum allowed value (included)
    double      iof;      // Increment Or Factor
    union {               // pointer to external variable that 
      double*   d;        // is synchronized with val.
      int*      i;        // This synchronization is controlled
      char*     c;        // by the RANGE_FL_SynXxx flags
    } u;
};

#ifdef HAS_INLINE
Inline void range_cpy(Trange*des, Crange*src) { *des = *src; }
#else
#define range_cpy(des,src) *(des)=*(src)
#endif

/*======================================================================*/

/**
 * The range_init0 function sets r->val to v and initializes
 * the nAll, nBdt, nAdt fields of r range using the fmt format.
 * The format is something like 9999.9 or 999.
 * For instance 99.999 gives 6 (nAll), 2 (nBdt) and
 * 3 (nAdt) values.
 *
 * The range_initIMM function does the same initializations
 * than the range_init0 function. Then it sets the range
 * to [min:max] and in the inc/dec mode by 1.
 * Furthermore if max is less or equal to min the bounds are
 * not set.
 *
 * The range_initFMM function does the same initializations
 * than the range_init0 function. Then it sets the range
 * to [min:max] and in the mult/div mode by 9/8.
 * Furthermore if max is less or equal to min the bounds are
 * not set.
 *
 * Ensure:
 *  fmt must not be null or the empty string ("").
 *  fmt must contains at least 1 character '9'.
**/
extern void range_init0  (Trange* r, double v, CstrBrief fmt);
extern void range_initIMM(Trange* r, double v, CstrBrief fmt,
                          double min, double max);
extern void range_initFMM(Trange* r, double v, CstrBrief fmt,
                          double min, double max);

extern Trange* range_new0  (double v, CstrBrief fmt);
extern Trange* range_newIMM(double v, CstrBrief fmt,
                            double min, double max);
extern Trange* range_newFMM(double v, CstrBrief fmt,
                            double min, double max);
extern Trange* range_dup   (Crange*r);
extern void    range_free  (Trange*r);

extern void range_setModeInc(Trange*r, double inc);
extern void range_setModeMul(Trange*r, double fac);

/**
 * The range_synDble function indicates to r numeric to synchronize
 * the variable pointed to by p with its self value. Every time
 * the r numeric value changes *p is updated.
 * The function overwrites the current synchronization if any.
 * If p is the null pointer the current synchronization is disabled.
 *
 * The range_synInt and range_synChar functions only differ from the
 * range_synDble function by the type of the p parameter.
 *
 * The range_update function synchronizes the value of the r range
 * with the external value pointed by the p parameter given by the
 * range_syn... functions.
 * In this case the external value is left unchanged and the internal
 * value is modified.
**/
extern void range_synDble(Trange*r, double*p);
extern void range_synInt (Trange*r, int*   p);
extern void range_synChar(Trange*r, char*  p);
extern void range_update(Trange*r);

/**
 * The range_any function sets into an Tany container the current value
 * of the r range according to the synchronization type.
 *    RANGE_FL_SynDble: any.dreal
 *    RANGE_FL_SynInt : any.sint
 *    RANGE_FL_SynChar: any.schar
 *    others          : any.dreal
 * The Tany container is initialized for allowing comparison:
 *    Trange r1; // value type is X
 *    Trange r2; // value type is X
 *    int eq = memcmp( &range_any(r1), &range_any(r2), sizeof(Tany))==0; // OK
 * but be beware, the next example does not work:
 *    Trange r1; // value type is X
 *    Trange r2; // value type is Y
 *    int eq = memcmp( &range_any(r1), &range_any(r2), sizeof(Tany))==0; // BUG
 * 
 * The range_dble function prints the current value of the r range
 * into the buf buffer according to the range format (nAll,nBdt, nAdt).
 *
 * Return:
 *   The range_any function returns a Tany container.
 *   The range_dble function always returns the buf pointer.
**/
extern Tany  range_any(Trange* r);
extern char* range_dble(char* buf, Crange* r);

extern void  range_setVal(Trange* r, double v);
extern void  range_inc(Trange* r);
extern void  range_dec(Trange* r);

/*======================================================================*/
/*= miscellaneous range internal features                              =*/

extern void  range_RfmtParse(char*nAll, char*nBdt, char*nAdt,
                             CstrBrief rfmt);
extern char* range_Rkey2Pfmt(char*dest, char nAll, char nBdt, char nAdt);
extern char* range_Rfmt2Pfmt(char*dest, CstrBrief rfmt);

/*======================================================================*/
#ifdef __cplusplus
}
#endif
/*======================================================================*/
#endif  // FILE_TOOLS_RANGE_H
/*======================================================================*/

