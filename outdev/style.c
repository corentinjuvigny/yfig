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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: style.c 120 2018-01-28 16:19:13Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/style.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See style.h header for documentation.                             =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/style.h"

#include "tools/color.h"

/*======================================================================*/

#define CMP_int(l,r,field)   if ( (l)->field < (r)->field ) return -1; \
                             if ( (l)->field > (r)->field ) return +1;
#define CMP_dble(l,r,field)  if ( (l)->field < (r)->field ) return -1; \
                             if ( (l)->field > (r)->field ) return +1;
#define CMP_color(l,r,field) if ( (l)->field < (r)->field ) return -1; \
                             if ( (l)->field > (r)->field ) return +1;

/*======================================================================*/
/*= Attributes for drawing text                                        =*/

extern void odsty_initText (Tsty_text*  s, Ccolor* c)
{
    s->stt_angle = 0;
    s->stt_font  = 0;
    s->stt_pts   = 12.0;
    s->stt_color = color_newLazy(c);
}
extern Tsty_text odsty_copyText (Csty_text*  s)
{
    Tsty_text ret = *s;
    ret.stt_color = color_newLazy(ret.stt_color); 
    return ret;
}
extern void odsty_upteText (Tsty_text*  d, Csty_text*  s)
{
    d->stt_angle =     s->stt_angle;
    d->stt_font  =     s->stt_font;
    d->stt_pts   =     s->stt_pts;
    color_chgLazy(&d->dpt_color,s->stt_color);
}
extern void odsty_uegsText (Tsty_text*  d, CcolorDriver*drv) { color_updateEGS(drv, d->stt_color); }
extern void odsty_freeText (Tsty_text*  s) { color_delete( s->stt_color); }
extern int  odsty_compText (Csty_text*  l, Csty_text*  r)
{
    CMP_dble (l,r,stt_angle);
    CMP_int  (l,r,stt_font);
    CMP_dble (l,r,stt_pts);
    CMP_color(l,r,stt_color);
    return  0;
}

/*======================================================================*/
/*= Attributes for drawing curves and lines.                           =*/

extern void odsty_initLine (Tsty_line*  s, Ccolor* c)
{
    s->stl_thickness = 1;
    s->stl_color     = color_newLazy(c);
    s->stl_style     = DA_LS_Solid;
    s->stl_dotdash   = 0.0;
    s->stl_join      = DA_JS_Miter;
    s->stl_cap       = DA_CS_Butt;
}
extern Tsty_line odsty_copyLine (Csty_line*  s)
{
    Tsty_line ret = *s;
    ret.stl_color = color_newLazy(ret.stl_color); 
    return ret;
}
extern void odsty_upteLine (Tsty_line*  d, Csty_line*  s)
{
    d->stl_thickness = s->stl_thickness;
    color_chgLazy(&d->stl_color,s->stl_color);
    d->stl_style     = s->stl_style    ;
    d->stl_dotdash   = s->stl_dotdash    ;
    d->stl_join      = s->stl_join     ;
    d->stl_cap       = s->stl_cap      ;
}
extern void odsty_uegsLine (Tsty_line*  d, CcolorDriver*drv) { color_updateEGS(drv, d->stl_color); }
extern void odsty_freeLine (Tsty_line*  s) { color_delete( s->stl_color); }
extern int  odsty_compLine (Csty_line*  l, Csty_line*  r)
{
    CMP_int  (l,r,stl_thickness);
    CMP_color(l,r,stl_color);
    CMP_int  (l,r,stl_style);
    CMP_dble (l,r,stl_dotdash);
    CMP_int  (l,r,stl_join);
    CMP_int  (l,r,stl_cap);
    return  0;
}

/*======================================================================*/
/*= Attributes for filling an area.                                =*/

extern void odsty_initFill (Tsty_fill*  d, Ccolor* c)
{
    d->stf_style   = DA_FS_NoFill;
    d->stf_color   = color_newLazy(c);
    d->stf_shade   = 0;
    d->stf_light   = 0;
    d->stf_pattern = 0;
}
extern void odsty_initFillS(Tsty_fill*  d, Ccolor* c) { odsty_initFill(d,c); d->stf_style=DA_FS_Solid; }
extern Tsty_fill odsty_copyFill (Csty_fill*  s)
{
    Tsty_fill ret = *s;
    ret.stf_color = color_newLazy(ret.stf_color); 
    return ret;
}
extern void odsty_upteFill (Tsty_fill*  d, Csty_fill*  s)
{
    d->stf_style    =     s->stf_style   ;
    color_chgLazy(&d->stf_color,s->stf_color);
    d->stf_shade    =     s->stf_shade   ;
    d->stf_light    =     s->stf_light   ;
    d->stf_pattern  =     s->stf_pattern ;
}
extern void odsty_uegsFill (Tsty_fill*  d, CcolorDriver*drv) { color_updateEGS(drv, d->stf_color); }
extern void odsty_freeFill (Tsty_fill* s) { color_delete( s->stf_color); }
extern int  odsty_compFill (Csty_fill*  l, Csty_fill*  r)
{
    CMP_int  (l,r,stf_style);
    CMP_color(l,r,stf_color);
    CMP_int  (l,r,stf_shade);
    CMP_int  (l,r,stf_light);
    CMP_int  (l,r,stf_pattern);
    return  0;
}

/*======================================================================*/
/*= All the attributes.                                                =*/
/*= single structure.                                                  =*/

extern void odsty_init     (Tstyle*    s, Ccolor* c)
{
    odsty_initText(&s->sty_text,c);
    odsty_initLine(&s->sty_line,c);
    odsty_initFill(&s->sty_fill,c);
}
extern Tstyle    odsty_copy     (Cstyle*     s)
{
    Tstyle ret;
    ret.sty_text = odsty_copyText(&s->sty_text);
    ret.sty_line = odsty_copyLine(&s->sty_line);
    ret.sty_fill = odsty_copyFill(&s->sty_fill);
    return ret;
}
extern void    odsty_upte    (Tstyle*     d, Cstyle*  s);
extern void    odsty_free    (Tstyle* s)
{
    odsty_freeText(&s->sty_text);
    odsty_freeLine(&s->sty_line);
    odsty_freeFill(&s->sty_fill);
}
extern int     odsty_comp    (Cstyle*     l, Cstyle*  r);

/*======================================================================*/
/*= The TstyleSet implements a set of Tstyle that can be also used as  =*/
/*= a Tstyle stack.                                                    =*/

#define TPLSET style
#define TPLSET_StackIntf
#include "tools/set-body-template.h"

/*======================================================================*/
