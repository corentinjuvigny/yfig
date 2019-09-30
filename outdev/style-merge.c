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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: style-merge.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/style-merge.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See style.h header for documentation.                             =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/style.h"

#include "tools/color.h"

/*======================================================================*/

#define UpdateValue(m,mb,d,s,field) if ( (m&mb)!=0 ) d->field=s->field
#define UpdateColor(m,mb,d,s,field) if ( (m&mb)!=0 ) color_chgLazy(&d->field,s->field)

extern void odsty_upteTextM(Tsty_text*  d, Csty_text*  s, TstyleMask m);

extern void odsty_upteLineM(Tsty_line*  d, Csty_line*  s, TstyleMask m)
{
    UpdateValue( m , FDP_L_thick , d , s , stl_thickness);
    UpdateColor( m , FDP_L_color , d , s , stl_color);
    UpdateValue( m , FDP_L_style , d , s , stl_style);
    UpdateValue( m , FDP_L_join  , d , s , stl_join);
    UpdateValue( m , FDP_L_cap   , d , s , stl_cap);
}

extern void odsty_upteFillM(Tsty_fill*  d, Csty_fill*  s, TstyleMask m)
{
    UpdateValue( m , FDP_F_style , d , s , stf_style);
    UpdateColor( m , FDP_F_color , d , s , stf_color);
    UpdateValue( m , FDP_F_style , d , s , stf_shade);
    UpdateValue( m , FDP_F_style , d , s , stf_light);
    UpdateValue( m , FDP_F_style , d , s , stf_pattern);
}

/*======================================================================*/

#define CmpValue(ret,l,r,id,f) \
            if ( ((ret)&(id))!=0 && (l)->f==(r)->f ) ret&=~(id)

#define CmpColor(ret,l,r,id,f) CmpValue(ret,l,r,id,f)

#define CmpSLP(ret,l,r,id) \
            if (    ((ret)&(id))!=0 \
                 && (l)->dpf_shade==(r)->dpf_shade \
                 && (l)->dpf_light==(r)->dpf_light \
                 && (l)->dpf_pattern==(r)->dpf_pattern ) (ret)&=~(id)

extern TstyleMask odsty_cmpMask(TstyleMask m, Cstyle*l,  Cstyle*r)
{
    TstyleMask ret=m;
    Csty_text* tl = &l->sty_text;
    Csty_text* tr = &r->sty_text;
    Csty_line* ll = &l->sty_line;
    Csty_line* lr = &r->sty_line;
    Csty_fill* fl = &l->sty_fill;
    Csty_fill* fr = &r->sty_fill;

    // text
    CmpValue(ret,tl,tr,FDP_T_angle,stt_angle);
    CmpValue(ret,tl,tr,FDP_T_font ,stt_font);
    CmpValue(ret,tl,tr,FDP_T_pts  ,stt_pts);
    CmpColor(ret,tl,tr,FDP_T_color,stt_color);

    // line
    CmpValue(ret,ll,lr,FDP_L_thick,stl_thickness);
    CmpColor(ret,ll,lr,FDP_L_color,stl_color);
    CmpValue(ret,ll,lr,FDP_L_style,stl_style);
    CmpValue(ret,ll,lr,FDP_L_join ,stl_join);
    CmpValue(ret,ll,lr,FDP_L_cap  ,stl_cap);

    // fill
    CmpValue(ret,fl,fr,FDP_F_style,stf_style);
    CmpColor(ret,fl,fr,FDP_F_color,stf_color);
    CmpSLP  (ret,fl,fr,FDP_F_slp);

    return ret;
}

/*======================================================================*/

#define CopyValue(d,s,f)  (d)->f=(s)->f
#define CopyColor(d,s,f)  (d)->f=color_newLazy((s)->f) 
#define CopySLP(d,s)      do { CopyValue(d,s,dpf_shade); \
                               CopyValue(d,s,dpf_light); \
                               CopyValue(d,s,dpf_pattern); } while(0)

extern Tstyle odsty_merge1(TstyleMask mall, Cstyle*all,
                           TstyleMask mset, Cstyle*set )
{
    Tstyle res;
    Tsty_text* tres = &res. sty_text;
    Csty_text* tset = &set->sty_text;
    Tsty_line* lres = &res. sty_line;
    Csty_line* lset = &set->sty_line;
    Tsty_fill* fres = &res. sty_fill;
    Csty_fill* fset = &set->sty_fill;

#define FromSet(a,s,f) (f&a&s)!=0
    res = *all;

    // text
    if ( FromSet(mall,mset,FDP_T_angle) ) CopyValue(tres,tset,stt_angle);
    if ( FromSet(mall,mset,FDP_T_font)  ) CopyValue(tres,tset,stt_font);
    if ( FromSet(mall,mset,FDP_T_pts)   ) CopyValue(tres,tset,stt_pts);
    if ( FromSet(mall,mset,FDP_T_color) ) CopyColor(tres,tset,stt_color);
                                     else CopyColor(tres,tres,stt_color);

    // line
    if ( FromSet(mall,mset,FDP_L_thick) ) CopyValue(lres,lset,stl_thickness);
    if ( FromSet(mall,mset,FDP_L_color) ) CopyColor(lres,lset,stl_color);
                                     else CopyColor(lres,lres,stl_color);
    if ( FromSet(mall,mset,FDP_L_style) ) CopyValue(lres,lset,stl_style);
    if ( FromSet(mall,mset,FDP_L_join)  ) CopyValue(lres,lset,stl_join);
    if ( FromSet(mall,mset,FDP_L_cap)   ) CopyValue(lres,lset,stl_cap);

    // fill
    if ( FromSet(mall,mset,FDP_F_style) ) CopyValue(fres,fset,stf_style);
    if ( FromSet(mall,mset,FDP_F_color) ) CopyColor(fres,fset,stf_color);
                                     else CopyColor(fres,fres,stf_color);
    if ( FromSet(mall,mset,FDP_F_slp)   ) CopySLP  (fres,fset);
    return res;
}

extern Tstyle odsty_merge2(TstyleMask mall, Cstyle*all,
                                     TstyleMask mset, Cstyle*set )
{ return odsty_merge1(~mall,all,mset,set); }

/*======================================================================*/
