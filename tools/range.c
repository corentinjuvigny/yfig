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
 *      $Id: range.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/range.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utility for defining a value with lower and upper bounds.          =*/
/*= See the tools/range.h header for types and documentation.          =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/range.h"

/*======================================================================*/

extern void range_init0(Trange* r, double v, CstrBrief fmt)
{
    range_RfmtParse(&r->nAll,&r->nBdt,&r->nAdt,fmt);
    r->val = v;
}

extern void range_initIMM(Trange* r, double v, CstrBrief fmt,
                          double min, double max)
{
    range_init0(r,v,fmt);
    if ( min<max ) {
        r->flag |= RANGE_FL_LowBound|RANGE_FL_UppBound;
        r->min   = min;
        r->max   = max;
    }
    r->flag |= RANGE_FL_Inc;
    r->iof   = RANGE_DF_ISTEP;
}

extern void range_initFMM(Trange* r, double v, CstrBrief fmt,
                          double min, double max)
{
    range_init0(r,v,fmt);
    if ( min<max ) {
        r->flag |= RANGE_FL_LowBound|RANGE_FL_UppBound;
        r->min   = min;
        r->max   = max;
    }
    r->flag |= RANGE_FL_Mult;
    r->iof   = RANGE_DF_FSTEP;
}

/*======================================================================*/

extern Trange* range_new0  (double v, CstrBrief fmt)
{
    Trange* r = tool_malloc( sizeof(*r) );
    range_init0(r,v,fmt);
    return r;
}
extern Trange* range_newIMM(double v, CstrBrief fmt,
                            double min, double max)
{
    Trange* r = tool_malloc_0( sizeof(*r) );
    range_initIMM(r,v,fmt,min,max);
    return r;
}
extern Trange* range_newFMM(double v, CstrBrief fmt, double min, double max)
{
    Trange* r = tool_malloc_0( sizeof(*r) );
    range_initFMM(r,v,fmt,min,max);
    return r;
}
extern Trange* range_dup   (Crange*src)
{
    Trange* r = tool_malloc( sizeof(*r) );
    range_cpy(r,src);
    return r;
}
extern void    range_free  (Trange*r) { tool_free(r); }

/*======================================================================*/

extern void range_setModeInc(Trange*r, double inc)
{
    r->flag &= ~RANGE_FL_Mult;
    r->flag |=  RANGE_FL_Inc;
    r->iof   = inc;
}

extern void range_setModeMul(Trange*r, double fac)
{
    r->flag &= ~RANGE_FL_Inc;
    r->flag |=  RANGE_FL_Mult;
    r->iof   = fac;
}

extern void range_synDble(Trange*r, double*p)
{
    r->flag &= ~RANGE_FL_Syn;
    r->flag |=  RANGE_FL_SynDble;
    r->u.d   = p;
}

extern void range_synInt (Trange*r, int*   p)
{
    r->flag &= ~RANGE_FL_Syn;
    r->flag |=  RANGE_FL_SynInt;
    r->u.i   = p;
}

extern void range_synChar(Trange*r, char*  p)
{
    r->flag &= ~RANGE_FL_Syn;
    r->flag |=  RANGE_FL_SynChar;
    r->u.c   = p;
}

extern void range_update(Trange*r)
{
    double v;
    if ( (r->flag&RANGE_FL_SynChar)!=0      && r->u.c)
        r->val = *r->u.c;
    else if ( (r->flag&RANGE_FL_SynInt)!=0  && r->u.i)
       r->val = *r->u.i;
    else if ( (r->flag&RANGE_FL_SynDble)!=0 && r->u.d )
       r->val = *r->u.d;
}

/*======================================================================*/

extern Tany  range_any(Trange* r)
{
    Tany any; memset(&any,0,sizeof(any));
    if ( (r->flag&RANGE_FL_SynInt)!=0 )
        any.sint = r->val;
    else if ( (r->flag&RANGE_FL_SynChar)!=0 )
        any.schar = r->val;
    else
        any.dreal = r->val;
    return any;
}

extern char* range_dble(char* buf, Crange* r)
{
    char fmt[30];
    range_Rkey2Pfmt(fmt,r->nAll,r->nBdt,r->nAdt);
    sprintf(buf,fmt,r->val);
    return buf;
}

extern void range_setVal(Trange* r, double v)
{
    if ( (r->flag&RANGE_FL_LowBound)!=0 && v<r->min ) v=r->min;
    if ( (r->flag&RANGE_FL_UppBound)!=0 && v>r->max ) v=r->max;
    r->val = v;
    if ( (r->flag&RANGE_FL_Syn)!=0 ) {
        if ( (r->flag&RANGE_FL_SynDble)!=0 )
            *r->u.d = r->val;
        else if ( (r->flag&RANGE_FL_SynInt)!=0 )
            *r->u.i = r->val;
        else if ( (r->flag&RANGE_FL_SynChar)!=0 )
            *r->u.c = r->val;
    }
}

extern void range_inc(Trange* r)
{
    double nv;
    if ( (r->flag&RANGE_FL_Mult)!=0 ) {
        nv = r->val * r->iof;
        range_setVal(r,nv);
        return;
    }
    if ( (r->flag&RANGE_FL_Inc)!=0 ) {
        nv = r->val + r->iof;
        range_setVal(r,nv);
        return;
    }
}

extern void range_dec(Trange* r)
{
    double nv;
    if ( (r->flag&RANGE_FL_Mult)!=0 ) {
        nv = r->val / r->iof;
        range_setVal(r,nv);
        return;
    }
    if ( (r->flag&RANGE_FL_Inc)!=0 ) {
        nv = r->val - r->iof;
        range_setVal(r,nv);
        return;
    }
}

/*======================================================================*/
/*= miscellaneous range internal features                              =*/

extern void  range_RfmtParse(char*nAll, char*nBdt, char*nAdt,
                             CstrBrief rfmt)
{
    Cstr p;
    *nAll = strlen(rfmt);
    *nBdt= 0;
    *nAdt= 0;
    for ( p=rfmt; *p!=0 && *p!='.' ; p+=1) if (*p=='9') *nBdt+=1;
    if ( *p==0 ) return;
    for ( p=p+1;  *p!=0            ; p+=1) if (*p=='9') *nAdt+=1;
}

extern char* range_Rkey2Pfmt(char*dest, char nAll, char nBdt, char nAdt)
{
    sprintf(dest,"%%%d.%df",nAll,nAdt);
    return dest;
}

extern char* range_Rfmt2Pfmt(char*dest, CstrBrief rfmt)
{
    char nAll,nBdt,nAdt;
    range_RfmtParse(&nAll,&nBdt,&nAdt,rfmt);
    return range_Rkey2Pfmt(dest,nAll,nBdt,nAdt);
}

/*======================================================================*/
