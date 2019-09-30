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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: text-dir.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/text-dir.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that shows and allows the user to modify the direction      =*/
/*= for drawing text.                                                  =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

#include "vgui/vgui-outdev.h"
#include "outdev/ldaf-hl.h"
#include "fig/object.h"

/*======================================================================*/
/*= data structure                                                     =*/

#define WRITE_RIGHT   0  // the text is at the right hand of the position
#define WRITE_LEFT    1  // the text is at the left  hand of the position
#define WRITE_CENTER  2  // center text around the (x,y) position

typedef struct _Ttextdir Ttextdir;
typedef const   Ttextdir Ctextdir;

struct _Ttextdir {
    Tdpp_l1l2base base;
    int           dt_dir;      // see WRITE_XXX defines above
    int*          dt_syn;      // if !=0, dir and *syn are synchronized 
#define DIR_OBJ2DPP(d) ( (d)==FIGOTE_T_RIGHT ? WRITE_LEFT : \
        (d)==FIGOTE_T_CENTER ? WRITE_CENTER : WRITE_RIGHT)
#define DIR_DPP2OBJ(d) ( (d)==WRITE_LEFT ? FIGOTE_T_RIGHT : \
        (d)==WRITE_CENTER ? FIGOTE_T_CENTER : FIGOTE_T_LEFT)

};

#if 1
#define DATA_DX 22
#define DATA_DY 18
#else
#define DATA_DX 44
#define DATA_DY 60
#endif

static int linelengths[] = {
    DATA_DX,
    DATA_DX - (2*DATA_DX)/3,
    DATA_DX - (1*DATA_DX)/3,
    DATA_DX - (2*DATA_DX)/3,
};
#define linelengthsNB TOOL_TABLE_NbEle(linelengths)

/*======================================================================*/
/*= Interface                                                          =*/

//static void dpp_textdir_free(Tdpp_l1l2base* ll);
//static void dpp_textdir_cb1 (Tdpp_l1l2base* ll, Tyfig_mw* ymw);
static void dpp_textdir_inc (Tdpp_l1l2base* ll);
static void dpp_textdir_dec (Tdpp_l1l2base* ll);
static void dpp_textdir_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x0, double y0, double dx0, double dy0);

extern Twid_button* dpp_textdir_new (
        Twid_buttonPanel* panel, int*syn)
{
    Twid_button* btn;
    int       dxb = DATA_DX;
    int       dyb = DATA_DY;
    CstrBrief l1  = "Text";
    CstrBrief l2  = "Just.";
    Tvgui*    gui = panel->bp_gui;

    Ttextdir* dt = tool_malloc_0( sizeof(*dt) );
    dt->dt_syn = syn;
    if ( syn!=0 )
        dt->dt_dir = DIR_OBJ2DPP(*syn);
    else
        dt->dt_dir = WRITE_RIGHT;

  //dt->dt_free   = dpp_textdir_free;
  //dt->dt_cb1    = dpp_textdir_cb1;
    dt->dt_inc    = dpp_textdir_inc;
    dt->dt_dec    = dpp_textdir_dec;
    dt->dt_redraw = dpp_textdir_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, dxb,dyb);

    return btn;
}

extern void dpp_textdir_update  (Twid_button* btn, Tyfig_mw* ymw)
{
    Ttextdir* self  = btn->but_ud;

    if ( self->dt_syn == 0 )          return;
    int nv = DIR_OBJ2DPP( *self->dt_syn );
    if (nv == self->dt_dir ) return;
    self->dt_dir = nv;
    dpp_l1l2base_redrawFull( &self->base );
}

/*======================================================================*/
/*= internal utilities                                                 =*/

//static void dpp_textdir_free(Tvgui*gui, void* _dt);
//static void dpp_textdir_cb1 (Tdpp_l1l2base* _dt, Tyfig_mw* ymw);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void dpp_textdir_incdec(Tdpp_l1l2base* ll, int v)
{
    Ttextdir* dt = (void*)ll;
    dt->dt_dir = (dt->dt_dir+v)%3;
    if ( dt->dt_syn )
        *dt->dt_syn = DIR_DPP2OBJ(dt->dt_dir);
}
static void dpp_textdir_inc (Tdpp_l1l2base* ll) { dpp_textdir_incdec(ll,+1); }
static void dpp_textdir_dec (Tdpp_l1l2base* ll) { dpp_textdir_incdec(ll,-1); }

static void dpp_textdir_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x0, double y0, double dx0, double dy0)
{
    Ttextdir*   self = (Ttextdir*)ll;
    Toutdev*    od   = &vod->v_od;
    Tod_GCdef   gcd  = OD_GC_SysDraw;

    int i,x,y;
    for ( i=0,y=y0+2 ; y<(y0+DATA_DY) ; y+=4,i+=1 ) {
        int dx = linelengths[i%linelengthsNB];
        x = x0; // set for WRITE_RIGHT
        if ( self->dt_dir==WRITE_LEFT )
            x = x0 + DATA_DX - dx;
        else if ( self->dt_dir==WRITE_CENTER )
            x = x0 + (DATA_DX - dx)/2;
        od_draw_lineXY(od,gcd,x,y,x+dx,y);
    }
}

/*======================================================================*/
