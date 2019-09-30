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
 *      $Id: text-font.c 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/text-font.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the font for drawing     =*/
/*= text.                                                              =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"

#include "vgui/vgui.h"
#include "vgui/vgui-outdev.h"
#include "vgui/vgui-pixmap.h"

#include "outdev/resources.h"
#include "outdev/string-hl.h"

#include "tools/font.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tfontselect {
    Tdpp_l1l2base base;
    int           dt_fontid;   // the gl_fontTable[fontid] font is
                               // presently drawn in fs_dev pixmap.
                               // This is the current font.
    int*          dt_syn;      // if !=0, fontid and *syn are synchronized 
    TpixmapEGS    dt_pixmaps[FONT_NB]; // pixmaps of predefined fonts
                               // they have all the same size and the
                               // font name is centered.
} Tfontselect;

/*======================================================================*/
/*= the utility                                                        =*/

#define TFD_GetShortNameIdx(idx) TFD_GetShortName(*VG_fontdef(gui,idx))

static void dpp_textfont_free(Tdpp_l1l2base* ll);
//static void dpp_textfont_cb1 (Tdpp_l1l2base* ll, Tyfig_mw* ymw);
static void dpp_textfont_inc (Tdpp_l1l2base* ll);
static void dpp_textfont_dec (Tdpp_l1l2base* ll);
static void dpp_textfont_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x,double y,double dx,double dy);

extern Twid_button* dpp_textfont_new (
        Twid_buttonPanel* panel, int*syn)
{
    Twid_button* btn;
    CstrBrief l1  = "Text";
    CstrBrief l2  = "Font";
    Tvgui*    gui = panel->bp_gui;
    int i;

    /* we compute the the max size (mdx,mdy) for the fonts of
     * the gl_fontTable table. */
    int fontsize = gui->g_devPtsStd;
    int mdx=0,mdy=0,masc=0;
    for ( i=0 ; i<FONT_NB ; i+=1) {
        int asc2,dx2,dy2;
        Cstr str = TFD_GetShortNameIdx(i);
        gui_fonts_extents_DesUnit(gui,i,fontsize,str,
                &asc2,0,&dx2,&dy2);
        mdx  = max2(mdx ,dx2);
        mdy  = max2(mdy ,dy2);
        masc = max2(masc,asc2);
    }
    mdx += 4;

    /* we create the widget data structure */
    Tfontselect* dt = tool_malloc_0( sizeof(*dt) );
    dt->dt_syn = syn;
    if ( syn ) dt->dt_fontid=*syn;

    /* we create the font pixmaps */
    Tod_GCdef gcd = OD_GC_SysDraw;
  odres_style_pushTop(gui->vg_odResources,gcd);
    for ( i=0 ; i<FONT_NB ; i++ ) {
        int     dx2,dy2;
        Ccolor* fg       = gui->vg_black;
        Ccolor* bg       = gui->vg_white;
        Tvgui_odpixmap* dev = vgui_odpixmap_new(gui, bg, mdx,mdy, 0);

        Cstr str = TFD_GetShortNameIdx(i);
        od_str_setGC(&dev->vv_od,gcd,i,fontsize,fg);
        od_str_draw_CE(&dev->vv_od,gcd,str,mdx/2,masc-3);
        dt->dt_pixmaps[i] = vgui_odpixmap_delete2(dev);
    }
  odres_style_pop(gui->vg_odResources,gcd);

    // now create the font selection button.
    dt->dt_free   = dpp_textfont_free;
  //dt->dt_cb1    = dpp_textfont_cb1;
    dt->dt_inc    = dpp_textfont_inc;
    dt->dt_dec    = dpp_textfont_dec;
    dt->dt_redraw = dpp_textfont_draw;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, mdx,mdy);

    return btn;
}

extern void dpp_textfont_update (Twid_button* btn, Tyfig_mw* ymw)
{
    Tfontselect* self  = btn->but_ud;

    if ( self->dt_syn==0 )            return;
    if ( self->dt_fontid==*self->dt_syn) return;
    self->dt_fontid = *self->dt_syn;
    dpp_l1l2base_redrawFull( &self->base );
}

/*======================================================================*/
/*= internal utilities                                                 =*/

static void dpp_textfont_free(Tdpp_l1l2base* ll)
{
    int i;
    Tfontselect* self = (Tfontselect*)ll;
    Tvgui*       gui  = self->dt_pod->vv_gui;
    DBG0_Dp("option: %s %s fontid=%d", self->dt_label1,self->dt_label2);

    for ( i=0 ; i<FONT_NB ; i+=1 ) 
        gui_pixmap_free(gui,self->dt_pixmaps[i]);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

//static void dpp_textfont_cb1 (Tdpp_l1l2base* ll, Tyfig_mw* ymw);
static void dpp_textfont_incdec(Tdpp_l1l2base* ll, int v)
{
    Tfontselect* dt = (void*)ll;
    dt->dt_fontid = (dt->dt_fontid+v+FONT_NB)%FONT_NB;
    if ( dt->dt_syn )
        *dt->dt_syn = dt->dt_fontid;
}
static void dpp_textfont_inc (Tdpp_l1l2base* ll) { dpp_textfont_incdec(ll,+1); }
static void dpp_textfont_dec (Tdpp_l1l2base* ll) { dpp_textfont_incdec(ll,-1); }

static void dpp_textfont_draw(Tdpp_l1l2base* ll, Tvgui_outdev*vod, double x,double y,double dx,double dy)
{
    Tfontselect*    self  = (void*)ll;
    Tvgui_odpixmap* pod   = self->dt_dev;

    TpixmapEGS pixmap = self->dt_pixmaps[self->dt_fontid];
    gui_pixmap_drawPM(&pod->vv_vod, x,y,pixmap, 0,0,dx,dy);
}

/*======================================================================*/
