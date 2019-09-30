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
 *      $Id: lenunit.h 349 2018-11-11 13:04:44Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/lenunit.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This header defines the length units used in the YFIG software.    =*/
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
#ifndef FILE_TOOLS_LENUNIT_H
#define FILE_TOOLS_LENUNIT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

/* the unit identifier */
#define LUNIT_ID_None  -1  // a value to indicate that the unit is unspecified
#define LUNIT_ID_FU     0  // a pivot unit
#define LUNIT_ID_Inch   1
#define LUNIT_ID_Cm     2  // centimeter
#define LUNIT_ID_Pt     3  // point ( 1/72 Inch)
#define LUNIT_ID_IsValid(id) (0<=(id) && (id)<=3)

/* the human name of units */
#define LUNIT_HN_FU     "FU"
#define LUNIT_HN_Inch   "in"
#define LUNIT_HN_Cm     "cm"
#define LUNIT_HN_Pt     "pt"

/* the conversion from FU to human unit and vice-versa */
#define   LUNIT_FuDefault  LUNIT_FuPerInch
#define   LUNIT_FuPerInch  1200    // 1/1200 Inch
#define   LUNIT_CmPerInch  2.54    // 2.54 cm = 1 inch
#define   LUNIT_PtPerInch  72.     // standard point unit: 72 points = 1 inch

//#define LUNIT_FACT_FuToFu    1.
#define   LUNIT_FACT_FuToPt   (1./((double)LUNIT_FuDefault)*LUNIT_PtPerInch)
#define   LUNIT_FACT_FuToCm   (1./((double)LUNIT_FuDefault)*LUNIT_CmPerInch)
#define   LUNIT_FACT_FuToIn   (1./((double)LUNIT_FuDefault))

#define   LUNIT_FACT_PtToFu   (1./LUNIT_FACT_FuToPt)
//#define LUNIT_FACT_PtToPt   1.
#define   LUNIT_FACT_PtToCm   (1./LUNIT_PtPerInch*LUNIT_CmPerInch)
#define   LUNIT_FACT_PtToIn   (1./LUNIT_PtPerInch)

#define   LUNIT_FACT_CmToFu   (1./LUNIT_FACT_FuToCm)
#define   LUNIT_FACT_CmToPt   (1./LUNIT_FACT_PtToCm)
//#define LUNIT_FACT_CmToCm   1.
#define   LUNIT_FACT_CmToIn   (1./LUNIT_CmPerInch)

#define   LUNIT_FACT_InToFu   (1./LUNIT_FACT_FuToIn)
#define   LUNIT_FACT_InToPt   (1./LUNIT_FACT_PtToIn)
#define   LUNIT_FACT_InToCm   (1./LUNIT_Fact_CmToIn)
//#define LUNIT_FACT_InToIn   1.


#define LUNIT_FuToPt(d)   ((d)*LUNIT_FACT_FuToPt)
#define LUNIT_FuToCm(d)   ((d)*LUNIT_FACT_FuToCm)
#define LUNIT_FuToIn(d)   ((d)*LUNIT_FACT_FuToIn)

#define LUNIT_PtToFu(d)   ((d)*LUNIT_FACT_PtToFu)
#define LUNIT_PtToCm(d)   ((d)*LUNIT_FACT_PtToCm)
#define LUNIT_PtToIn(d)   ((d)*LUNIT_FACT_PtToIn)

#define LUNIT_CmToFu(d)   ((d)*LUNIT_FACT_CmToFu)
#define LUNIT_CmToPt(d)   ((d)*LUNIT_FACT_CmToPt)
#define LUNIT_CmToIn(d)   ((d)*LUNIT_FACT_CmToIn)

#define LUNIT_InToFu(d)   ((d)*LUNIT_FACT_InToFu)
#define LUNIT_InToPt(d)   ((d)*LUNIT_FACT_InToPt)
#define LUNIT_InToCm(d)   ((d)*LUNIT_FACT_InToCm)

/**
 * The lu_convToFu function converts the v value in uu unit into FU.
 * The lu_convFomFu function converts the v value in FU into the uu unit.
 *
 * uu must be a LUNIT_ID_... macro, otherwise the function return the v value.
**/
extern double lu_convToFu  (double v, int uu);
extern double lu_convFromFu(double v, int uu);

Inline double lu_convToFuPpi  (double v, double ppi) { return v/ppi*LUNIT_FuPerInch; }
Inline double lu_convFromFuPpi(double v, double ppi) { return v*ppi/LUNIT_FuPerInch; }

/*======================================================================*/
/*= user unit                                                          =*/

struct _TuserUnit {
    int    uu_id;      // base unit used (see LUNIT_ID_...
    double uu_factor;  // "id*factor" is the user unit
    double uu_sub;     // sub division of user unit.
                       // 1 user unit is splited into sub parts
};

typedef struct _TuserUnit TuserUnit;
typedef const   TuserUnit CuserUnit;

extern void   lu_init(TuserUnit*uu, int u, double f, double sdn);

extern double lu_UsrToFu(CuserUnit* uu, double v);
extern double lu_FuToUsr(CuserUnit* uu, double v);

extern TstrBrief lu_FuToUsrSta(CuserUnit* uu, double v, TstrBrief buf);
extern TstrEaten lu_FuToUsrDyn(CuserUnit* uu, double v);

extern double lu_LenPpiToUsr     (CuserUnit* uu, double v, double vdpi);
extern char*  lu_LenPpiToUsrStr  (CuserUnit* uu, double v, double vdpi, char*buf);
extern char*  lu_LenPpiToUsrStrNU(CuserUnit* uu, double v, double vdpi, char*buf);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_LENUNIT_H
/*======================================================================*/

