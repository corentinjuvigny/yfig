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
 *      $Id: vdw-color.c 316 2018-07-19 15:29:19Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-color.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widgets for selecting, creating colors.                     =*/
/*=                                                                    =*/
/*= See the wid-dialog.h header for function documentation.            =*/
/*======================================================================*/

#include "vgui-config.h"

#include "vgui/vgui.h"
#include "vgui/vdw-color.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

#include "tools/color.h"

/*======================================================================*/
/*= Color selector                                                     =*/

static void    vdw_colorsel_btn_cb(Tvgui_dw*vdw, int uid, void*ud);
static void    vdw_colorsel_addT (Tvdw_colorSel* self, Tcolor**cs, int nb, int finish);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Constructors & destructor                                          =*/

extern Tvdw_colorSel*
               vdwD_colorsel_new (Tvgui_dw* vdw, TstrEaten str,
                                  Tvdw_colorSel_cb cb, void* ud,
                                  Tcolor**cs, int nb)
{
    Tvdw_colorSel* self = (Tvdw_colorSel*)
                           vgui_dw_newSub(sizeof(*self), vdw, str);
    self->btns = widD_buttonPanel_newRaCa( SelfA, 30, 20);
    self->btns->bp_rownb=1;
    self->cb   = cb; 
    self->ud   = ud; 
    vdw_colorsel_addT(self,cs,nb,0);
    return self;
}

extern Tvdw_colorSel*
              vdwD_colorsel_newWN(Tvgui_dw* vdw, TstrEaten str,
                                  Tvdw_colorSel_cb cb, void* ud,
                                  CstrBrief nonefirst,
                                  Tcolor**cs, int nb,
                                  CstrBrief nonelast)
{
    Tvdw_colorSel* self = (Tvdw_colorSel*)
                           vgui_dw_newSub(sizeof(*self), vdw, str);
    self->btns = widD_buttonPanel_newRaCa( SelfA, 30, 20);
    self->btns->bp_rownb=1;
    self->cb   = cb; 
    self->ud   = ud; 
    if ( nonefirst ) {
        Twid_button* btn = widD_button_std_newLab3(SelfA,
                NULL, // help
                vdw_colorsel_btn_cb,NULL,NULL, // cb ud free
                nonefirst,NULL);
        btn->but_dx = self->btns->bp_buttonWidth;
        btn->but_dy = self->btns->bp_buttonHeight;
        wid_buttonPanel_addButton(self->btns,btn);
    }
    vdw_colorsel_addT(self,cs,nb,0);
    if ( nonelast ) {
        Twid_button* btn = widD_button_std_newLab3(SelfA,
                NULL, // help
                vdw_colorsel_btn_cb,NULL,NULL, // cb ud free
                nonelast,NULL);
        btn->but_dx = self->btns->bp_buttonWidth;
        btn->but_dy = self->btns->bp_buttonHeight;
        wid_buttonPanel_addButton(self->btns,btn);
    }
    return self;
}

extern void    vdw_colorsel_dbf  (Tvdw_colorSel* self, Tegs_widget pw)
{
    int i;
    wid_buttonPanel_build (self->btns, self->dw_widEGS);
    wid_buttonPanel_finish(self->btns);
  //vgui_dw_finishIC( SelfA, VDWLF_EH, self->btns->bp_widEGSext, -1);
    vgui_dw_finishIC( SelfA, 0,        self->btns->bp_widEGSext, -1);
    for (i=0 ; i<self->btns->bp_btnNb ; i+=1) {
        Twid_button* b = self->btns->bp_btns[i];
        Tcolor*      c = b->but_ud;
        if ( c!=0 )
            gui_widegs_setBgColor(b->but_widEGS,c->egs);
    }
}

extern void    vdw_colorsel_free (Tvdw_colorSel* self)
{
    wid_buttonPanel_free(self->btns);
    vgui_dw_free(SelfA);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Operations                                                         =*/

extern void    vdw_colorsel_set  (Tvdw_colorSel* self,
                                  int col, int row, int bdx, int bdy)
{
    if ( col>0 ) self->btns->bp_colnb = col; 
    if ( row>0 ) self->btns->bp_rownb = row; 
    if ( bdx>0 ) self->btns->bp_buttonWidth  = bdx; 
    if ( bdy>0 ) self->btns->bp_buttonHeight = bdy; 
}

static void    vdw_colorsel_btn_cb(Tvgui_dw*vdw, int uid, void*ud)
{
    Tvdw_colorSel*   self = (Tvdw_colorSel*) vdw;
    Tcolor*          c    = ud;
    DBG0_CdwE("panel=%p color=%s",vdw,DBG_Color2str(c));
    Tvdw_colorSel_cb cb   = self->cb;
    if ( cb!=0 ) {
        DBG2_CdwM("panel=%p color=%s : CB invocation ",vdw,DBG_Color2str(c));
        cb(self,c,self->ud);
    }
    DBG1_CdwL("panel=%p color=%s",vdw,DBG_Color2str(c));
}

extern void    vdw_colorsel_addC (Tvdw_colorSel* self, Ccolor* c, int finish)
{
    Twid_button* btn = widD_button_std_newLab3(SelfA,
                        NULL, // help
                        vdw_colorsel_btn_cb,(Tcolor*)c,NULL, // cb ud free
                        NULL,NULL);
    btn->but_dx = self->btns->bp_buttonWidth;
    btn->but_dy = self->btns->bp_buttonHeight;
    wid_buttonPanel_addButton(self->btns,btn);
    wid_buttonPanel_build(self->btns,NULL);
    if (finish) gui_buttonPanel_update(self->btns);//FIXME:REMOVE
    gui_widegs_setBgColor(btn->but_widEGS,c->egs);
}

extern void    vdw_colorsel_supC (Tvdw_colorSel* self, Ccolor* c);

static void    vdw_colorsel_addT (Tvdw_colorSel* self,
                                  Tcolor**cs, int nb, int finish)
{
    int i;
    for ( i=0 ; i<nb ; i+=1 )
        vdw_colorsel_addC(self,cs[i],finish);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Quick constructors                                                 =*/

extern Tvdw_colorSel*
               vdwD_colorsel_nbf  (Tvgui_dw* vdw, TstrEaten str,
                                   Tcolor**cs, int nb,
                                   Tvdw_colorSel_cb cb, void* ud,
                                   int col, int row)
{
    return vdwD_colorsel_nbfWN(vdw,str,cs,nb,cb,ud,col,row,0,NULL);
}

extern Tvdw_colorSel*
               vdwD_colorsel_nbfWN(Tvgui_dw* vdw, TstrEaten str,
                                   Tcolor**cs, int nb,
                                   Tvdw_colorSel_cb cb, void* ud,
                                   int col, int row,
                                   int pos, CstrBrief none)
{
    int inc = ((pos&1)!=0) + ((pos&2)!=0);
    Tvdw_colorSel* self = vdwD_colorsel_newWN(vdw,str,cb,ud,
            pos&1 ? none : NULL,
            cs,nb,
            pos&2 ? none : NULL);

    if (col==0 && row==0) col=10;
    if (col==0 ) col = (inc+nb+row-1)/row + 1; 
    if (row==0 ) row = (inc+nb+col-1)/col + 1; 

    // update panel parameters and finish the panel
    vdw_colorsel_set(self,col,row,0,0);
    //wid_buttonPanel_dbf(self->btns,NULL);

    // finish the sub dialog
    vdw_colorsel_dbf(self,0);
    return self;
}

/*======================================================================*/
/*= Color creator                                                      =*/


#define IRED       0
#define IGREEN     1
#define IBLUE      2
#define ILOCK      3
#define IHUE       4
#define ISAT       5
#define IVAL       6
#define NBW        7

#define WU_Default 23
#define HU_Default 120

#define DBG_slider2str(n) ( (n)==0 ? "red"  : (n)==1 ? "green" : \
                           (n)==2 ? "blue" : (n)==3 ? "lock" : \
                           (n)==4 ? "H"    : (n)==5 ? "S" : \
                           (n)==6 ? "V"    : "???" )

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Constructors & destructor                                          =*/

static void vdw_colorcre_update(Tvdw_colorCre* self, Tcolor* nc);
static void vdw_colorcre_cb_scroll(Twid_slider* slider, void *ud, double v);
static void vdw_colorcre_cb_lock(Tvgui_dw* self, int uid, void *ud);

extern Tvdw_colorCre* vdwD_colorcre_nbf  (Tvgui_dw* vdw, TstrEaten str,
                                   Tvdw_colorCre_cb cb, void* ud,
                                   Tcolor*ic, int wu, int hu)
{
    int i;
    Tvdw_colorCre* self = (Tvdw_colorCre*)
                           vgui_dw_newSub(sizeof(*self), vdw, str);
    if (ic)
        self->color = color_newLazy(ic);
    else {
        Tvgui* gui = vgui_dw_getVGui(SelfA);
        self->color = color_newLazy(gui->vg_black);
    }
    self->cb   = cb; 
    self->ud   = ud; 

    int dx = wu<=0 ? WU_Default : wu;
    int dy = hu<=0 ? HU_Default : hu;
    int bdw= 2;

    // create the sub widgets
    self->boxes[0] = widD_button_onoff_newLab(SelfA,
            NULL,vdw_colorcre_cb_lock,(void*)1,NULL,"R","X");
    self->boxes[1] = widD_button_onoff_newLab(SelfA,
            NULL,vdw_colorcre_cb_lock,(void*)2,NULL,"G","X");
    self->boxes[2] = widD_button_onoff_newLab(SelfA,
            NULL,vdw_colorcre_cb_lock,(void*)4,NULL,"B","X");
    self->boxes[3] = widD_button_orn_new(SelfA, "Lock",NULL);
    self->boxes[4] = widD_button_orn_new(SelfA, "H",NULL);
    self->boxes[5] = widD_button_orn_new(SelfA, "S",NULL);
    self->boxes[6] = widD_button_orn_new(SelfA, "V",NULL);
    for (i=0 ; i<NBW ; i++) {
        self->boxes[i]->but_dx = i!=ILOCK ? dx : dx+12;
        self->boxes[i]->but_dy = dx;
    }
    self->boxes[0]->but_widEGS = gui_button_onoff_dbf(self->boxes[0],self->dw_widEGS);
    self->boxes[1]->but_widEGS = gui_button_onoff_dbf(self->boxes[1],self->dw_widEGS);
    self->boxes[2]->but_widEGS = gui_button_onoff_dbf(self->boxes[2],self->dw_widEGS);
    self->boxes[3]->but_widEGS = gui_button_orn_dbf(self->boxes[3],self->dw_widEGS);
    self->boxes[4]->but_widEGS = gui_button_orn_dbf(self->boxes[4],self->dw_widEGS);
    self->boxes[5]->but_widEGS = gui_button_orn_dbf(self->boxes[5],self->dw_widEGS);
    self->boxes[6]->but_widEGS = gui_button_orn_dbf(self->boxes[6],self->dw_widEGS);

    for (i=0 ; i<NBW ; i++) {
        self->sliders[i] = widD_slider_nbf(SelfA,1.0,1./255.,
                                           vdw_colorcre_cb_scroll,self);
        wid_slider_setSize(self->sliders[i],dx,dy);
    }

    self->viewer = widD_label_nbfBox(SelfA,bdw,dx+dx/2,dx);

    // do the layout
//#define WBox(i) i,self->boxes[i]
#define WBox(i)  i,self->boxes[i]  ->but_widEGS
#define WSli(i)  i,self->sliders[i]->widEGS
#define Wvie(i)  i,self->viewer
#define Wnul(i)  i,NULL
#define LN(x) VDWLF_ROW(x)
    vgui_dw_finishMTX(SelfA
     ,LN(0) ,WBox(0) ,WBox(1) ,WBox(2) ,WBox(3) ,WBox(4) ,WBox(5) ,WBox(6)
     ,LN(1) ,WSli(0) ,WSli(1) ,WSli(2) ,WSli(3) ,WSli(4) ,WSli(5) ,WSli(6)
     ,LN(2) ,Wnul(0) ,Wvie(1) ,Wvie(2) ,Wvie(3) ,Wvie(4) ,Wvie(5) ,Wnul(6)
     ,-1);

    vdw_colorcre_update(self,NULL);

    return self;
}

extern void    vdw_colorcre_free (Tvdw_colorCre* self)
{
    int i;
    color_delete( self->color );
    for ( i=0 ; i<7 ; i++ ) {
        wid_button_delete( self->boxes[i] );
        wid_slider_free( self->sliders[i] );
    }
    vgui_dw_free(SelfA);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Operations                                                         =*/

extern void    vdw_colorcre_enable  (Tvdw_colorCre* self, Ccolor* c)
{
    if ( c==0 ) {
        Tvgui* gui = vgui_dw_getVGui( SelfA );
        c = gui->vg_black;
    }
    Tcolor* nc = color_newLazy(c);
    vdw_colorcre_update( self, nc);
    gui_widegs_enable( self->dw_widEGS );
}

extern void    vdw_colorcre_viewer  (Tvdw_colorCre* self, Ccolor* c)
{
    Tcolor* nc = color_newLazy(c);
    vdw_colorcre_update( self, nc);
}

extern void    vdw_colorcre_disable (Tvdw_colorCre* self)
{ gui_widegs_disable( self->dw_widEGS ); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Private utilities                                                  =*/

#define CONV_ratioTo256(r) ((int)round(255.*(1.-r)))
#define CONV_256ToRatio(r) (1.-((double)r)/255.)

#define CONV_ratioToHue(r) (360.*(1.-r))
#define CONV_hueToRatio(r) (1.-((double)r)/360.)

#define CONV_ratioToSv(r) (1.-r)
#define CONV_svToRatio(r) (1.-r)

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void vdw_colorcre_update(Tvdw_colorCre* self, Tcolor* nc)
{
    TcolorRGB oldrgb = self->color->rgb;
    TcolorRGB newrgb = nc ? nc->rgb : oldrgb;
    if ( nc!=0 && self->color!=nc ) {
        color_delete( self->color );
        self->color = nc;
    }
    // RGB8 sliders
    if ( newrgb.red!=oldrgb.red )
        wid_slider_set( self->sliders[IRED],   CONV_256ToRatio(newrgb.red));
    if ( newrgb.green!=oldrgb.green )
        wid_slider_set( self->sliders[IGREEN], CONV_256ToRatio(newrgb.green));
    if ( newrgb.blue!=oldrgb.blue )
        wid_slider_set( self->sliders[IBLUE],  CONV_256ToRatio(newrgb.blue));

    // HSV sliders
    double oldhue,oldsat,oldval;
    double newhue,newsat,newval;
    color_rgb8ToHsv(&oldhue,&oldsat,&oldval,&oldrgb);
    color_rgb8ToHsv(&newhue,&newsat,&newval,&newrgb);
    if ( newhue!=oldhue )
        wid_slider_set( self->sliders[IHUE], CONV_hueToRatio(newhue));
    if ( newsat!=oldsat )
        wid_slider_set( self->sliders[ISAT], CONV_svToRatio(newsat));
    if ( newval!=oldval )
        wid_slider_set( self->sliders[IVAL], CONV_svToRatio(newval));

    // Lock slider
    if ( self->lockmask==0 ) {
        self->lock255max = 0;
        self->lock255min = 0;
        wid_slider_disable( self->sliders[ILOCK] );
    } else {
        int max = max3(
            (self->lockmask&1)==0 ? 0   : self->color->rgb.red,
            (self->lockmask&2)==0 ? 0   : self->color->rgb.green,
            (self->lockmask&4)==0 ? 0   : self->color->rgb.blue);
        int min = min3(
            (self->lockmask&1)==0 ? max : self->color->rgb.red,
            (self->lockmask&2)==0 ? max : self->color->rgb.green,
            (self->lockmask&4)==0 ? max : self->color->rgb.blue);
        self->lock255max = max;
        self->lock255min = self->lock255max-min;
    }
    wid_slider_set( self->sliders[ILOCK], CONV_256ToRatio(self->lock255max));

    // Update color viewer
    gui_widegs_setBgColor( self->viewer, self->color->egs );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Callback                                                           =*/

static void vdw_colorcre_cb_lock(Tvgui_dw* _self, int uid, void *ud)
{
    Tvdw_colorCre* self = (Tvdw_colorCre*)_self;
    int mask = (intptr_t)ud;
    if ( (self->lockmask&mask)!=0 )
        self->lockmask &= ~mask;
    else
        self->lockmask |=  mask;
    vdw_colorcre_update(self,NULL);
}

static void vdw_colorcre_cb_scroll(Twid_slider* slider, void *ud, double v)
{
    Tvdw_colorCre* self = ud;
    DBG0_CdwE("dialog=%p slider=%p ratio=%f",self,slider,v);
    int i;
    TcolorRGB rgb;
    int    lock255;
    double hsvnew;
    double hue,sat,val;

    // search for the slider index
    int islider=-1;
    for (i=0 ; i<NBW ; i+=1) {
        if ( slider==self->sliders[i] ) {
            islider = i;
            break;
        }
    }
    DBG0_CdwM("dialog=%p slider=%p ratio=%f : %d (%s)",self,slider,v,
              islider, DBG_slider2str(islider) );
    TEMP_asf_printf(islider==-1,"%p slider not found",slider);
    // compute the new RGV value
    switch ( islider ) {
        case IRED :
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : red <-- %04x",
                       self,slider,v,CONV_ratioTo256(v));
            rgb = self->color->rgb;
            rgb.red = CONV_ratioTo256(v);
            break;
        case IGREEN :
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : green <-- %04x",
                       self,slider,v,CONV_ratioTo256(v));
            rgb = self->color->rgb;
            rgb.green = CONV_ratioTo256(v);
            break;
        case IBLUE :
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : blue <-- %04x",
                       self,slider,v,CONV_ratioTo256(v));
            rgb = self->color->rgb;
            rgb.blue = CONV_ratioTo256(v);
            break;
        case ILOCK :
            // notice that if we enter here, self->lockmask is not null.
            // while self->lockmask==0 --> slider[ILOCK] is disbled.
            // see the vdw_colorcre_update function
            lock255 = CONV_ratioTo256(v);
            if ( lock255<self->lock255min ) {
                DBG0_CdwM("dialog=%p slider=%p ratio=%f : "
                          "%d too small (min=%d start=%d",
                       self,slider,v, lock255,self->lock255min,self->lock255max);
                lock255 = self->lock255min;
                wid_slider_set( self->sliders[ILOCK], CONV_256ToRatio(lock255) );
            }
            int diff = lock255 - self->lock255max;
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : lock diff=%d mask=%d",
                       self,slider,v,diff,self->lockmask);
            rgb = self->color->rgb;
            if ( (self->lockmask&1)!=0 ) rgb.red    += diff; 
            if ( (self->lockmask&2)!=0 ) rgb.green  += diff; 
            if ( (self->lockmask&4)!=0 ) rgb.blue   += diff; 
            break;
        case IHUE :
            hsvnew = CONV_ratioToHue(v*360.);
            color_rgb8ToHsv(&hue,&sat,&val,&self->color->rgb);
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : HUE %f <-- %f",
                    self,slider,v,hsvnew,hue);
            color_hsvToRgb8(&rgb,hsvnew,sat,val);
            break;
        case ISAT :
            hsvnew = CONV_ratioToSv(v);
            color_rgb8ToHsv(&hue,&sat,&val,&self->color->rgb);
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : SAT %f <-- %f",
                    self,slider,v,hsvnew,sat);
            color_hsvToRgb8(&rgb,hue,hsvnew,val);
            break;
        case IVAL :
            hsvnew = CONV_ratioToSv(v);
            color_rgb8ToHsv(&hue,&sat,&val,&self->color->rgb);
            DBG0_CdwM("dialog=%p slider=%p ratio=%f : VAL %f <-- %f",
                    self,slider,v,hsvnew,val);
            color_hsvToRgb8(&rgb,hue,sat,hsvnew);
            break;
    }
    DBG0_CdwM("dialog=%p slider=%p ratio=%f : %s --> %s",self,slider,v,
        DBG_Rgb2str(&self->color->rgb),DBG_Rgb2str(&rgb));

    if ( color_cmpRGB(&self->color->rgb,&rgb)!=0 ) {
        Tvgui*  gui      = vgui_dw_getVGui(SelfA);
        Tcolor* newcolor = color_newByRGB(gui->colorDriver,
                                rgb.red,rgb.green,rgb.blue);
        vdw_colorcre_update(self,newcolor);
        if ( self->cb )
            self->cb(self,newcolor,self->ud);
    }

    DBG1_CdwL("dialog=%p slider=%p ratio=%f",self,slider,v);
}

/*======================================================================*/
