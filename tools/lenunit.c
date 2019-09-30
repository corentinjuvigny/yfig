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
 *      $Id: lenunit.c 144 2018-05-31 15:54:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/lenunit.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Length unit implementation.                                        =*/
/*=                                                                    =*/
/*= See the tools/lenunit.h header for documentation.                  =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/lenunit.h"

/*======================================================================*/

extern double lu_convToFu(double v, int uu)
{
    if ( uu==LUNIT_ID_Inch ) return LUNIT_InToFu(v);
    if ( uu==LUNIT_ID_Cm   ) return LUNIT_CmToFu(v);
    if ( uu==LUNIT_ID_Pt   ) return LUNIT_PtToFu(v);
    TEMP_ase_printf(uu!=LUNIT_ID_FU,
            "unexpected length unit: %d (set to FU)",uu);
    return v;
}

extern double lu_convFromFu(double v, int uu)
{
    if ( uu==LUNIT_ID_Inch ) return LUNIT_FuToIn(v);
    if ( uu==LUNIT_ID_Cm   ) return LUNIT_FuToCm(v);
    if ( uu==LUNIT_ID_Pt   ) return LUNIT_FuToPt(v);
    TEMP_ase_printf(uu!=LUNIT_ID_FU,
            "unexpected length unit: %d (set to FU)",uu);
    return v;
}

/*======================================================================*/

extern void   lu_init(TuserUnit*uu, int u, double f, double sdn)
{
    uu->uu_id     = u;
    uu->uu_factor = f;
    uu->uu_sub    = sdn;
}

extern double lu_UsrToFu(CuserUnit* uu, double v)
{
    int    unit = uu->uu_id;
    double vv   = v*uu->uu_factor;
    if ( unit==LUNIT_ID_Cm )
        return LUNIT_CmToFu(vv);
    else if ( unit==LUNIT_ID_Inch )
        return LUNIT_InToFu(vv);
    else  {
        TEMP_ase_printf(unit!=LUNIT_ID_FU,
                "unexpected length unit: %d",unit);
        return vv;
    }
}

extern double lu_FuToUsr(CuserUnit* uu, double v)
{
    int    unit = uu->uu_id;
    double vv   = v/uu->uu_factor;
    if ( unit==LUNIT_ID_Cm )
        return LUNIT_FuToCm(vv);
    else if ( unit==LUNIT_ID_Inch )
        return LUNIT_FuToIn(vv);
    else  {
        TEMP_ase_printf(unit!=LUNIT_ID_FU,
                "unexpected length unit: %d",unit);
        return vv;
    }
}

extern TstrBrief lu_FuToUsrSta(CuserUnit* uu, double v, TstrBrief buf);
extern TstrEaten lu_FuToUsrDyn(CuserUnit* uu, double v);

static double lu_factorsIn[] = {
    LUNIT_FACT_FuToIn, 1, LUNIT_FACT_CmToIn, LUNIT_FACT_PtToIn
};
static Cstr lu_names[] = {
    LUNIT_HN_FU, LUNIT_HN_Inch, LUNIT_HN_Cm, LUNIT_HN_Pt
};
static Cstr lu_fmts[] = {
    "%.0f", "%.2f", "%.2f", "%.1f"
};

extern double lu_LenPpiToUsr   (CuserUnit* uu, double v, double vdpi)
{
    double vinch = v/vdpi;
    double vuu   = vinch/lu_factorsIn[uu->uu_id];
    return vuu*uu->uu_factor;
}

extern char*  lu_LenPpiToUsrStr(CuserUnit* uu, double v, double vdpi, char*buf)
{
    char fmt[100];
    sprintf(fmt,"%s %s",lu_fmts[uu->uu_id],lu_names[uu->uu_id]);
    sprintf(buf,fmt,lu_LenPpiToUsr(uu,v,vdpi));
    return buf;
}

extern char*  lu_LenPpiToUsrStrNU(CuserUnit* uu, double v, double vdpi, char*buf)
{
    sprintf(buf,lu_fmts[uu->uu_id],lu_LenPpiToUsr(uu,v,vdpi));
    return buf;
}

/*======================================================================*/
