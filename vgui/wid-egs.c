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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: wid-egs.c 62 2017-04-15 15:05:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-egs.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* Operations on EGS widget.                                           =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-egs.h"

/*======================================================================*/
/*= EGS widget set                                                     =*/

extern Tegs_widSet*  egswidset_new  (int n)
{
    Tegs_widSet* ret = malloc(sizeof(*ret));
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? malloc(sizeof(*ret->eles)*n) : 0;
    return ret;
}

extern void          egswidset_free (Tegs_widSet* s)
{
    if ( s->eles ) free(s->eles);
    free(s);
}

extern void          egswidset_add  (Tegs_widSet* s, Tegs_widget p)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(*s->eles)*s->eleMemNb);
    }
    s->eles[s->eleNb] = p;
    s->eleNb += 1;
}

extern Tegs_widget*  egswidset_first(Cegs_widSet* s)
{ return s->eleNb!=0 ? s->eles+0 : 0; }
extern Tegs_widget*  egswidset_last (Cegs_widSet* s)
{ return s->eleNb!=0 ? s->eles+s->eleNb-1 : 0; }

/*======================================================================*/

extern int  vgui_widegs_getX  (Tegs_widget w)
{ int x;    gui_widegs_getGeometry(w,&x,0,0,0,0); return x; }

extern int  vgui_widegs_getY  (Tegs_widget w)
{ int y;    gui_widegs_getGeometry(w,0,&y,0,0,0); return y; }

extern int  vgui_widegs_getXSE(Tegs_widget w)
{ int x,dx; gui_widegs_getGeometry(w,&x,0,&dx,0,0); return x+dx; }

extern int  vgui_widegs_getYSE(Tegs_widget w)
{ int y,dy; gui_widegs_getGeometry(w,0,&y,0,&dy,0); return y+dy; }

extern int  vgui_widegs_getBDT(Tegs_widget w);

extern int  vgui_widegs_getDX (Tegs_widget w)
{ int dx;   gui_widegs_getGeometry(w,0,0,&dx,0,0); return dx; }

extern int  vgui_widegs_getDY (Tegs_widget w)
{ int dy;   gui_widegs_getGeometry(w,0,0,0,&dy,0); return dy; }

/*======================================================================*/

extern int   vgui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text)
{ return gui_string_width(gui, wn, text); }

/*======================================================================*/
