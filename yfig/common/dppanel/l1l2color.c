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
 *      $Id: l1l2color.c 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/l1l2color.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that shows and allows an user to modify the color           =*/
/*= parameter.                                                         =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "dppanel.h"
#include "yfig-mw.h"
#include "ydw/ydw.h"

#include "vgui/vgui.h"
#include "vgui/vgui-outdev.h"

#include "fig/figure.h"

#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"

#include "tools/color.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tl1l2color Tl1l2color;
typedef const   Tl1l2color Cl1l2color;

struct _Tl1l2color {
    Tdpp_l1l2base base;

    // dt_color gives the color that is presently shown
    //   - pen or text color: it is never null
    //   - fill color: The null value indicate the NoFill fill style
    Tcolor*       dt_color;

    CcolorSet*    dt_pcolors; // the predefined colors
    TcolorSet*    dt_ucolors; // the user colors
    int           dt_index;   // the index of dt_color, its range is
                              // [0:dt_pcolors->eleNb+dt_ucolors->eleNb[

    // the next fields give the color that the widget must update
    // they are exclusively NULL
    Tcolor**      dt_syncC;   // case of pen or text color
    Tsty_fill*    dt_syncF;   // case of fill color
};

// configuration
#define BTN_DX DPP_V3STD_DX
#define BTN_DY DPP_V3STD_DY

// shortcut
#define FCLR(dt) (dt->dt_syncF->dpf_style==DA_FS_NoFill ? NULL : dt->dt_syncF->dpf_color)
#define CSNB(s) ((s)==0 ? 0 : (s)->eleNb)
#define RGB_DIST(a,b) ( abs((a)->rgb.red   - (b)->rgb.red) + \
                        abs((a)->rgb.green - (b)->rgb.green) + \
                        abs((a)->rgb.blue  - (b)->rgb.blue) )
#define FIRSTCOLOR(s) ( s->dt_pcolors->eles[0] )
#define LASTCOLOR(s)  ( CSNB(s->dt_ucolors)!=0 \
                        ? s->dt_ucolors->eles[s->dt_ucolors->eleNb-1] \
                        : s->dt_pcolors->eles[s->dt_pcolors->eleNb-1] )

// debug
#define DLLC_FMT     "option: %s %s : c=%s : " 
#define DLLC_PSE     self->dt_label1,self->dt_label2,DBG_Color2str(self->dt_color)
#define DLLC_PSEC(c) DLLC_PSE,DBG_Color2str(c)
#define DLLC_PDT     dt->ll_label1,dt->ll_label2, DBG_Color2str(NULL)
#define DLLC_PDTC(c) DLLC_PDT,DBG_Color2str(c)

/*======================================================================*/
/*= Interface                                                          =*/

//static void dpp_l1l2color_free   (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_cb1CLR (Tdpp_l1l2base* _dt, Tyfig_mw* ymw);
static void dpp_l1l2color_incCLR (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_decCLR (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_drawCLR(Tdpp_l1l2base* _dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);
//static void dpp_l1l2color_free   (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_cb1DPF (Tdpp_l1l2base* _dt, Tyfig_mw* ymw);
static void dpp_l1l2color_incDPF (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_decDPF (Tdpp_l1l2base* _dt);
static void dpp_l1l2color_drawDPF(Tdpp_l1l2base* _dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);

// return the array color corresponding to the dt_index.
// Note: self->dt_color is not used and not modified.
static Tcolor* l1l2color_getColor(Tl1l2color* self);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_button* dpp_l1l2color_newCLR( Twid_buttonPanel* panel,
                         CstrStatic l1, CstrStatic l2, Tcolor**sync)
{
    Twid_button* btn;
    int dxb=BTN_DX,dyb=BTN_DY;
    Tl1l2color* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_syncC = sync;
    dt->dt_color = *sync;

  //dt->dt_free   = dpp_l1l2color_free;
    dt->dt_cb1    = dpp_l1l2color_cb1CLR;
    dt->dt_inc    = dpp_l1l2color_incCLR;
    dt->dt_dec    = dpp_l1l2color_decCLR;
    dt->dt_redraw = dpp_l1l2color_drawCLR;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, dxb,dyb);

    return btn;
}

extern Twid_button* dpp_l1l2color_newDPF( Twid_buttonPanel* panel,
                         CstrStatic l1, CstrStatic l2, Tsty_fill*sync)
{
    Tvgui* gui = panel->vgbpm_gui;

    int dxv3=BTN_DX,dyv3=BTN_DY; l1l2_none_adjust(gui,&dxv3,&dyv3);

    // build the button
    Twid_button* btn;
    Tl1l2color* dt = tool_malloc_0( sizeof(*dt) );

    dt->dt_syncF = sync;
    dt->dt_color = (Tcolor*)FCLR(dt);

  //dt->dt_free   = dpp_l1l2color_free;
    dt->dt_cb1    = dpp_l1l2color_cb1DPF;
    dt->dt_inc    = dpp_l1l2color_incDPF;
    dt->dt_dec    = dpp_l1l2color_decDPF;
    dt->dt_redraw = dpp_l1l2color_drawDPF;
    btn = dpp_l1l2base_initBtnStdPXM3(&dt->base, panel, l1,l2, dxv3,dyv3);

    return btn;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void     dpp_l1l2color_update( Twid_button* self, Tyfig_mw* ymw)
{
    int i;
    Ccolor* c;
    Tl1l2color* dt  = self->but_ud;
    Tfigure*    fig = YMW_figure( ymw );
    
    dt->dt_pcolors  = fig->colors.predefinedColors;
    dt->dt_ucolors  = fig->colors.privateColors;

    // special NoFill case for fill color
    if ( dt->dt_syncF && dt->dt_syncF->dpf_style==DA_FS_NoFill ) {
        dt->dt_index = 0;
        dt->dt_color = NULL;
        dpp_l1l2base_redrawFull( &dt->base );
        return;
    }

    // get the new color
    Ccolor* newcolor=0;
    if ( dt->dt_syncF!=0 ) {
        newcolor = dt->dt_syncF->dpf_color;
    }
    if ( dt->dt_syncC!=0 )
        newcolor = *dt->dt_syncC;
    TEMP_asf_printf( newcolor==0, "upading color asked with %p color",newcolor);

    // search for the new color in the color arrays
    // and get the corresponding index.
    int index=0;
    int mindist=-1;
    int minindex=-1;
    for (i=0 ; i<CSNB(dt->dt_pcolors) ; i+=1,index+=1) {
        c = dt->dt_pcolors->eles[i];
        if ( newcolor==c || color_cmp(newcolor,c)==0 )
            goto found;
        int dist = RGB_DIST(newcolor,c);
        if ( mindist==-1 || dist<mindist ) {
            mindist  = dist;
            minindex = index;
        }
    }
    for (i=0 ; i<CSNB(dt->dt_ucolors) ; i+=1,index+=1) {
        c = dt->dt_ucolors->eles[i];
        if ( newcolor==c || color_cmp(newcolor,c)==0 )
            goto found;
        int dist = RGB_DIST(newcolor,c);
        if ( mindist==-1 || dist<mindist ) {
            mindist  = dist;
            minindex = index;
        }
    }
    index =  mindist<=30 ? minindex : 0;
found:
    dt->dt_index    = index;
    dt->dt_color    = l1l2color_getColor(dt);
    dpp_l1l2base_redrawFull( &dt->base );
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static Tcolor* l1l2color_getColor(Tl1l2color* self)
{
    int nbpcolors = CSNB(self->dt_pcolors);
    int nbucolors = CSNB(self->dt_ucolors);
    int nbcolors  = nbpcolors + nbucolors;

    if ( self->dt_index < nbpcolors ) 
        return self->dt_pcolors->eles[self->dt_index];
    if ( self->dt_index < nbcolors ) 
        return self->dt_ucolors->eles[self->dt_index-nbpcolors];
    return NULL;
}

static void dpp_l1l2color_cb1LFT (Tdpp_l1l2base* dt, Tyfig_mw* ymw, int isel)
{
#define dpp_l1l2color_getVal(btn) (((Tl1l2color*)btn->but_ud)->dt_color)
    Twid_button* btn;
    Tl1l2color*  self = (Tl1l2color*)dt;
    Tl1l2color*  self2;

    DBG0_DpL(DLLC_FMT,DLLC_PSE);

    Twid_dialStatus status;
    Tyfig_drawparam* param = &ymw->ymw_currYDP;
    Tcolor* penOrg =  dpp_l1l2color_getVal(param->dp_linecolor);
    Tcolor* filOrg =  dpp_l1l2color_getVal(param->dp_fillcolor);
    Tcolor* txtOrg =  dpp_l1l2color_getVal(param->dp_textcolor);
    Tcolor* penNew =  penOrg;
    Tcolor* filNew =  filOrg;
    Tcolor* txtNew =  txtOrg;

    status = ydw_clrPFT_start(ymw, &YMW_figure(ymw)->colors,
                              isel, &penNew, &filNew, &txtNew);

    if ( status==WID_DIALSTATUS_Ok ) {
        DBG2_DpM(DLLC_FMT"dialog Ok",DLLC_PSE);
        if ( penOrg!=penNew ) {
            DBG2_DpM(DLLC_FMT"updating pen color to %s", DLLC_PSEC(penNew));
            btn  = param->dp_linecolor;
            self2 = btn->but_ud;
            if ( self2->dt_syncC ) color_chgLazy(self2->dt_syncC,penNew);
            dpp_l1l2color_update(btn,ymw);
        }
        if ( filOrg!=filNew ) {
            DBG2_DpM(DLLC_FMT"updating fill color to %s", DLLC_PSEC(filNew));
            btn  = param->dp_fillcolor;
            self2 = btn->but_ud;
            if ( self2->dt_syncF ) {
                if ( filNew==0 ) {
                    self2->dt_syncF->dpf_style = DA_FS_NoFill;
                    // we must always have a true color in style (for DA_FS_NoFill
                    // the color is not used, so any valid color may be used.
                    color_chgLazy(&self2->dt_syncF->dpf_color,ymw->ymw_gui->vg_black);
                } else {
                    if ( self2->dt_syncF->dpf_style==DA_FS_NoFill )
                        self2->dt_syncF->dpf_style = DA_FS_Solid;
                    color_chgLazy(&self2->dt_syncF->dpf_color,filNew);
                }
            }
            dpp_l1l2color_update(btn,ymw);
            dpp_filltile_update(param->dp_filltile,ymw);
        }
        if ( txtOrg!=txtNew ) {
            DBG2_DpM(DLLC_FMT"updating text color to %s", DLLC_PSEC(txtNew));
            btn  = param->dp_textcolor;
            self2 = btn->but_ud;
            if ( self2->dt_syncC ) color_chgLazy(self2->dt_syncC,txtNew);
            dpp_l1l2color_update(btn,ymw);
        }
    } else 
        DBG2_DpM(DLLC_FMT"dialog canceled",DLLC_PSE);

    DBG1_DpL(DLLC_FMT,DLLC_PSE);
}

// update dt_index and dt_color
//   - advancing in the array color if next!=0
//   - advancing backward in the array color if next==0
// If dt_color is not a color array, the function resets dt_color
// to a color array and sets dt_index to this color.
static void dpp_l1l2color_nextprev(Tl1l2color* dt, int next)
{
    int nbcolors = CSNB(dt->dt_pcolors) + CSNB(dt->dt_ucolors);

    if ( nbcolors==0 )
        return;

    // check if dt_index is already valid
    if ( dt->dt_index>=nbcolors ) {
        // dt_index out of range (color arrays have been changed). 
        dt->dt_index = nbcolors-1;
        dt->dt_color = l1l2color_getColor(dt);
        return ;
    }

    // check if dt_color is always corresponding to index colors
    Tcolor* cByIndex = l1l2color_getColor(dt);
    if ( dt->dt_color!=cByIndex ) {
        // the color pointed does not exist anymore
        // it has been removed of the color arrays.
        // we just update the button to this color.
        dt->dt_color = cByIndex;
        return;
    }

    // dt_index color and dt_color are corresponding.
    if (next)
        dt->dt_index = dt->dt_index==(nbcolors-1) ? 0 : dt->dt_index+1;
    else
        dt->dt_index = dt->dt_index==0 ? nbcolors-1 : dt->dt_index-1;
    dt->dt_color = l1l2color_getColor(dt);
}

/*======================================================================*/
/*= Specialization for pen or text color                               =*/

static void dpp_l1l2color_cb1CLR (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{
    Tyfig_drawparam* param = &ymw->ymw_currYDP;
    Tdpp_l1l2base*   line  = param->dp_linecolor->but_ud;

    if ( dt==line )
        dpp_l1l2color_cb1LFT(dt,ymw,'P');
    else
        dpp_l1l2color_cb1LFT(dt,ymw,'T');
}

static void dpp_l1l2color_incCLR (Tdpp_l1l2base* dt)
{
    Tl1l2color* self = (Tl1l2color*)dt;
    dpp_l1l2color_nextprev(self,1);
    if ( self->dt_syncC ) color_chgLazy(self->dt_syncC,self->dt_color);
}

static void dpp_l1l2color_decCLR (Tdpp_l1l2base* dt)
{
    Tl1l2color* self = (Tl1l2color*)dt;
    dpp_l1l2color_nextprev(self,0);
    if ( self->dt_syncC ) color_chgLazy(self->dt_syncC,self->dt_color);
}

static void dpp_l1l2color_drawCLR(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy)
{
    Toutdev*    od   = &vdev->v_od;
    Tvgui*      gui  = vdev->v_gui;
    Tl1l2color* self = (Tl1l2color*)dt;
    DBG2_Dp(DLLC_FMT"fill rect %dx%d",DLLC_PSE, dx,dy);
    od_ldaf_setGCll (od,OD_GC_SysFill, 0.0, DA_FS_Solid,self->dt_color,0,0,0);
    od_fill_rectXYWH(od,OD_GC_SysFill, x,y,dx,dy);
}

/*======================================================================*/
/*= Specialization for fill color                                      =*/

static void dpp_l1l2color_cb1DPF (Tdpp_l1l2base* dt, Tyfig_mw* ymw)
{ dpp_l1l2color_cb1LFT(dt,ymw,'F'); }

static void dpp_l1l2color_incDPF (Tdpp_l1l2base* dt)
{
    Tl1l2color* self     = (Tl1l2color*)dt;
    int         nbcolors = CSNB(self->dt_pcolors) + CSNB(self->dt_ucolors);
    int         lastidx  = nbcolors-1;
    if ( self->dt_color==0 ) {
        // After NoFill --> 1st color
        self->dt_color = FIRSTCOLOR(self);
        self->dt_index = 0;
    } else if (  self->dt_index>=lastidx ) {
        // After last color --> NoFill
        self->dt_color = NULL;
        self->dt_index = 0;
    } else {
        // the next color exists
        dpp_l1l2color_nextprev(self,1);
    }
    // now update syncF
    if ( self->dt_color==NULL ) {
        self->dt_syncF->dpf_style = DA_FS_NoFill;
        self->dt_syncF->dpf_color = NULL;
    } else if ( self->dt_syncF->dpf_style==DA_FS_NoFill ) {
        self->dt_syncF->dpf_style = DA_FS_Solid;
        self->dt_syncF->dpf_color = self->dt_color;
    } else {
        self->dt_syncF->dpf_color = self->dt_color;
    }

    Tvgui_mw*         vmw   = wid_button_getMw( self->dt_btn );
    Tyfig_mw*         ymw   = (Tyfig_mw*) vmw;
    Tyfig_drawparam* param = &ymw->ymw_currYDP;
    dpp_filltile_update(param->dp_filltile,ymw);
}

static void dpp_l1l2color_decDPF (Tdpp_l1l2base* dt)
{
    Tl1l2color* self     = (Tl1l2color*)dt;
    int         nbcolors = CSNB(self->dt_pcolors) + CSNB(self->dt_ucolors);
    int         lastidx  = nbcolors-1;
    if ( self->dt_color==0 ) {
        // before NoFill --> last color
        self->dt_color = LASTCOLOR(self);
        self->dt_index = lastidx;
    } else if (  self->dt_index==0 ) {
        // before first color --> NoFill
        self->dt_color = NULL;
        self->dt_index = 0;
    } else {
        // the previous color exists
        dpp_l1l2color_nextprev(self,0);
    }
    // now update syncF
    if ( self->dt_color==NULL ) {
        self->dt_syncF->dpf_style = DA_FS_NoFill;
        self->dt_syncF->dpf_color = NULL;
    } else if ( self->dt_syncF->dpf_style==DA_FS_NoFill ) {
        self->dt_syncF->dpf_style = DA_FS_Solid;
        self->dt_syncF->dpf_color = self->dt_color;
    } else {
        self->dt_syncF->dpf_color = self->dt_color;
    }
}

static void dpp_l1l2color_drawDPF(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy)
{
    Tvgui*      gui  = vdev->v_gui;
    Tl1l2color* self = (Tl1l2color*)dt;
    if ( self->dt_color ) {
        dpp_l1l2color_drawCLR(dt,vdev,x,y,dx,dy);
    } else {
        dpp_none_draw(vdev,x,y,dx,dy);
        DBG2_Dp(DLLC_FMT"draw %s",DLLC_PDT, dpp_none);
    }
}

/*======================================================================*/
