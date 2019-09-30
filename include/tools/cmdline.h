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
 *      $Id: cmdline.h 337 2018-11-05 22:44:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/cmdline.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for parsing the command line: main(argc,argv).           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_CMDLINE_H
#define FILE_TOOLS_CMDLINE_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"

typedef struct _Tsw Tsw;
typedef const   Tsw Csw;

typedef struct _TptrswSet TptrswSet;
typedef const   TptrswSet CptrswSet;

typedef struct _Tcmdline Tcmdline;
typedef const   Tcmdline Ccmdline;

/*======================================================================*/
/*= Define an option of the command line.                              =*/
/*=                                                                    =*/
/*= For instance:                                                      =*/
/*=    -c, --color str                                                 =*/
/*=    -r, --range min max                                             =*/
/*=                                                                    =*/
/*= switch kind:                                                       =*/
#define SWOK_Help      0x0001  // switch for online help
#define SWOK_MinVal    0x0002  // switch has a min value (valmin)
#define SWOK_MaxVal    0x0004  // switch has a max value (valmax)
#define SWOK_IncDec    0x0016  // switch that in/decrement a value
#define SWOK_Mandatory 0x0020  // switch is mandatory
#define SWOK_Several   0x0040  // switch can be present several time
#define SWOK_ValTypInt 0x1000  // value type is integer (sw->value.sint)
#define SWOK_ValTypStr 0x2000  // value type is char*   (sw->value.str)
#define SWOK_ValTypDle 0x4000  // value type is double  (sw->value.dreal)
#define SWOK_HasDefVal 0x0080  // switch has a default value (value field).
#define SWOK_IsDefSW   0x0180  // switch has a default value (value field)
                               // and it's the default switch of switch set.
// shortcut
#define SWOK_MinMax       (SWOK_MinVal   |SWOK_MaxVal   )
#define SWOK_HasDefValInt (SWOK_HasDefVal|SWOK_ValTypInt)
#define SWOK_HasDefValStr (SWOK_HasDefVal|SWOK_ValTypStr)
#define SWOK_HasDefValDle (SWOK_HasDefVal|SWOK_ValTypDle)
#define SWOK_IsDefSWInt   (SWOK_IsDefSW  |SWOK_ValTypInt)
#define SWOK_IsDefSWStr   (SWOK_IsDefSW  |SWOK_ValTypStr)
#define SWOK_IsDefSWDle   (SWOK_IsDefSW  |SWOK_ValTypDle)
/*=                                                                    =*/
/*= Argument format:                                                   =*/
#define SWAF_None      0  // switch has not argument
#define SWAF_Int       1  // switch arguments are integers (int)
#define SWAF_Str       2  // switch arguments are strings  (char*)
#define SWAF_Dble      3  // switch arguments are reals    (double)
#define SWAF_StrOpt    4  // a single string argument (char*)
/*=                                                                    =*/
/*= option structure:                                                  =*/
/*=  The type that defines a switch is:                                =*/
typedef struct _Tsw Tsw;
typedef const   Tsw Csw;
/*=  The _Tsw is not defined, you must define it as following          =*/
/*=  and you can optionally specialized it by adding proper fields.    =*/
/*=    struct _Tsw { SW_FIELD(); <my-data> };                          =*/
/*=                                                                    =*/
/*= switch structure fields:                                           =*/
/*=    // format of switch: -g, --debug-level=n : blabla1              =*/
/*=    //                                         blabla2              =*/
/*=    char*  shswitch;  // short switch name (-g)                     =*/
/*=    char*  lgswitch;  // long switch name  (--debug-level)          =*/
/*=    char*  arswitch;  // argument name (n)                          =*/
/*=    char*  help[3];   // 3 strings or null terminated               =*/
/*=                      // for online help (blabla1 blabla2)          =*/
/*=    char*  man;       // a long description for manual page.        =*/
/*=                                                                    =*/
/*=    // argument format and type                                     =*/
/*=    short  argfmt;  // format of argument (see SWAF_...)            =*/
/*=    short  argnb;   // number of additional arguments:              =*/
/*=                    //   0: no additional arg --> a single arg.     =*/
/*=                    //   1: an additional arg --> 2 args exactly.   =*/
/*=                    //   2: 2  additional arg --> 3 args exactly.   =*/
/*=    int    optkind; // type of values (see SWOK_...)                =*/
/*=                                                                    =*/
/*=    Tany   value;     // value of switch                            =*/
/*=    Tany   valmin;    // minimal value of switch                    =*/
/*=    Tany   valmax;    // maximal value of switch                    =*/
/*=                                                                    =*/
/*=    // Online help & documentation                                  =*/
/*=    // The hfhelp/man (h=Header, f=footer) text is inserted         =*/
/*=    //   - between the header and the switch text if header!=0,     =*/                  
/*=    //   - after the switch text if header==0.                      =*/                  
/*=    char*  header;    // section header                             =*/
/*=    char*  hfhelp[5]; // 5 strings or null terminated               =*/
/*=                      // for online help                            =*/
/*=    char*  hfman;     // a long description for manual page.        =*/
/*=                                                                    =*/
/*=    // internal use                                                 =*/
/*=    int      set;     // 1: switch is present in argv               =*/
/*=    Tcmdline cmdline;                                               =*/

#define SW_FIELD() char *shswitch,*lgswitch,*arswitch,*doc[3],*man; \
                   short argfmt,argnb; int optkind; \
                   Tany  value,valmin,valmax; \
                   Csw  *exclusive[5], *required[5]; \
                   Tcmdline* cmdline; int   set

// cmdline_switch2str:
// It writes into buf the sw switch (eg: -c # #).
// If quote is not zero the string is enclosed with the quote character.
// It returns always buf.
//
// cmdline_aswitch2str:
// Same but 1) it returns a malloced string the caller must free after usage,
// 2) It tries to replace the parameter name of the switch by the value of
// the command line.
// It is useful for fatal error messages about switches.
extern char*     cmdline_switch2str   (char*buf, Csw*sw,char quote);
Inline char*     cmdline_switch2strDQ (char*buf, Csw*sw) { return cmdline_switch2str (buf,sw,'"'); }
extern TstrEaten cmdline_aswitch2str  (Csw*sw,char quote);
Inline TstrEaten cmdline_aswitch2strDQ(Csw*sw)           { return cmdline_aswitch2str(sw,'"'); }

// 1: if set; 0:if unset
extern int    cmdline_isSwSet(Csw*sw);

// retrieve the value of an option.
// no controll is performed
extern int    cmdline_getInt  (Csw*sw);
extern double cmdline_getDle  (Csw*sw);
extern char*  cmdline_getStr  (Csw*sw);

// retrieve the value of a switch list, this value is in order
//   1) thus of the first sw that is set
//   2) thus of the first sw that is tagged with SWOK_IsDefSW.
//   3) thus of the first sw that is tagged with SWOK_HasDefVal.
//   3) dv (...Mdv) or NULL (...M).
// The list must be null terminated.
extern int   *cmdline_getIntM  (           Tsw*sw,...);
extern double*cmdline_getDleM  (           Tsw*sw,...);
extern Tstr  *cmdline_getStrM  (           Tsw*sw,...);
extern int    cmdline_getIntMdv(int    dv, Tsw*sw,...);
extern double cmdline_getDleMdv(double dv, Tsw*sw,...);
extern Tstr   cmdline_getStrMdv(char*  dv, Tsw*sw,...);

/*======================================================================*/
/*= set of switch pointers.                                            =*/

#define TPLSET ptrsw
#define TPLSET_EleType Tsw*
#define TPLSET_EleCype const Tsw*
#define TPLSET_AddWithoutPtr
#include "set-header-template.h"

/*======================================================================*/
/*= Interface.                                                         =*/

struct _Tcmdline {
    TptrswSet* cl_switches;

    // standard help format
    //    usage: prg [OPTIONS] arg1 arg2 ...
    //    h-blabla
    //    h-blabla
    //    ...
    //    OPTIONS
    //    ...
    //    f-blabla
    //    f-blabla
    //    ...
    CstrStatic cl_prgargs1; // arg1 arg2
    CstrStatic cl_prgargs2; // arg1 arg2
    CstrStatic cl_prgargs3; // arg1 arg2
    CstrStatic*cl_hhelp;    // NULL terminated for h-blabla
    CstrStatic*cl_fhelp;    // NULL terminated for f-blabla
    CstrStatic cl_hman;     // for manual (h-blabla)
    CstrStatic cl_fman;     // for manual (f-blabla)

    // For private use
    TastrSet*  cl_mallocedstr;
};

extern Tcmdline* cmdline_newSwSet(void*t,int eleSz, int eleNb,
        CstrStatic prgargs1, CstrStatic prgargs2, CstrStatic prgargs3);
extern void      cmdline_delete  (Tcmdline*cl);

/**
 * The cmdline_parse function parses the null terminated string array
 * argv using the swiches defined by sws.
 *  - It ignores the first string.
 *  - The argv strings that are recognized as option are stored in sws.
 *  - If a argv string does not exactly match a switch definition,
 *    the function prints an error message and exits with the 1 status.
 *  - If a argv string does not match any switch definition, it
 *    is ignored.
 *
 * The cmdline_finish function performs the following tasks:
 *  - It verifies the parsing integrity such as mandatory switches
 *    are set.
 *  - It suppresses all the strings of argv set that start by a
 *    character of the mask string. For these suppression, it 
 *    generates an error of errlvl level.
 *
 * For instance: 
 *    with
 *       switch="-c int" ; argv="-l aa bb -c cc dd"
 *    after cmdline_parse
 *       argv_res="-l aa bb dd" and cc stored in cl
 *    after cmdline_finish
 *      with mask="-"  ---> -l not recognized and argv="aa bb dd"
 *      with mask="ad" ---> -aa & bb not recognized and argv="-l bb"
 *
 * Return:
 *  - On success, the cmdline_parse function returns the argv strings
 *    that have not been treated into a null terminated string set.
 *    On failure it does not return.
**/

extern TastrSet* cmdline_parse (Tcmdline*cl, char*const*const argv);
extern void      cmdline_finish(Tcmdline*cl, TastrSet* argv, char*mask, int errlvl);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_CMDLINE_H
/*======================================================================*/
