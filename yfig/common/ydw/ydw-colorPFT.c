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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-colorPFT.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-colorPFT.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Full dialog that allows to the user to select the pen, fill and    =*/
/*= text colors.                                                       =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "ydw/ydw.h"
#include "vgui/vgui.h"
#include "vgui/vgui-names.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-egs.h"
#include "vgui/vdw-color.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "tools/color.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tydw_clrViewer {
    Tvgui*         gui;
    Twid_button*   lab;    // On/Off button  "Xxx color"
    Tegs_widget    view;   // Label showing the color
    Tegs_widget    rgb;    // label showing the rgb of the color
    Ccolor*        color;  // the color dumped in view and rgb
    int            isact;  // 1: the viewer is active; 0:it is unactive
    int            wnone;  // 1: accept NULL color.
} Tydw_clrViewer;

/*======================================================================*/

typedef struct _Tydw_clrPFT Tydw_clrPFT;
typedef const   Tydw_clrPFT Cydw_clrPFT;

struct _Tydw_clrPFT {
    Tvgui_dw          any;
    Tfig_colors*      dw_figcolors;

    /* Second column */
    Tvgui_dw*         dw_colors;
    Tvdw_colorCre*    dw_create;     // color creator dialog
    Twid_buttonPanel* dw_btns;       // contains the buttons below
    Twid_button*   dw_btnAddCreate;  // button adding a color via the
                                     // dw_create widget
    Twid_button*   dw_btnAddPickup;  // button adding a color by cliking
                                     // it on the screen.
    Twid_button*   dw_btnCleanup;    // button removing unused private
                                     // colors.
    Twid_button*   dw_btnCancel;     // button canceling the current
                                     // color creation.
    /* First column */
    Tydw_clrViewer  dw_pencolor,dw_fillcolor, dw_textcolor;
    Tydw_clrViewer* dw_active;       // The selected color, points to
                                    // one of the dw_pen/fill/textcolor
                                    // widget.

    /* state of widget (all to null at start time) */
    int dw_stateCreating;   // 1 means that the creator widget is working
                            // active->color is a creator color.
    int dw_statePicking;    // 1 means that the picking has just run.
                            // active->color is a new allocated color
    Ccolor* dw_stateColor;  // The color to restore if color creation is
                            // canceled. 
};

// shortcuts
#define dw_pencolorLab   dw_pencolor.lab->but_widEGS
#define dw_pencolorView  dw_pencolor.view
#define dw_pencolorRgb   dw_pencolor.rgb
#define dw_fillcolorLab  dw_fillcolor.lab->but_widEGS
#define dw_fillcolorView dw_fillcolor.view
#define dw_fillcolorRgb  dw_fillcolor.rgb
#define dw_textcolorLab  dw_textcolor.lab->but_widEGS
#define dw_textcolorView dw_textcolor.view
#define dw_textcolorRgb  dw_textcolor.rgb

/*======================================================================*/
/*= Base action                                                        =*/

#define DBG_IdFmt "w=%p act=%15s : "
#define DBG_IdPar self,self->dw_active->lab->but_onoff_textOff

// Show the c color into the self->dw_active color viewer.
static void ydw_clrPFT_showAct(Tydw_clrPFT*self, Ccolor*c);

// If there is an current color adding then it adds the color to 
// the private color set and panel, resets the dialog state to the
// initial state.
// If there is not a current color adding, it does nothing.
static void ydw_clrPFT_addAction(Tydw_clrPFT*self);

// If there is an current color adding then it cancels it,
// and resets the dialog state to the initial state.
// If there is not a current color adding, it does nothing.
static void ydw_clrPFT_cancelAction(Tydw_clrPFT*self);

// Turns on the widgets to the initial state.
static void ydw_clrPFT_goToInitState(Tydw_clrPFT*self);

// Turns on the widgets to the adding state.
static void ydw_clrPFT_goToAddState(Tydw_clrPFT*self,
                                    int create, int pickup);

/*======================================================================*/
/*= Interface                                                          =*/

static void        ydw_clrViewer_free( Tydw_clrViewer* self);

static Tydw_clrPFT* ydw_clrPFT_nbf(Tyfig_mw* ymw,
                                 Tfig_colors* figcolors,
                                 int sel, // P, F or T
                                 Tcolor*pen,Tcolor*fill,Tcolor*text);

Twid_dialStatus ydw_clrPFT_start(Tyfig_mw* ymw,
                   Tfig_colors* figcolors,
                   int isel, // P, F or T
                   Tcolor**pen,Tcolor**fill,Tcolor**text)
{
    // build and start the dialog
    Tydw_clrPFT* dialog = ydw_clrPFT_nbf(ymw,figcolors,isel,*pen,*fill,*text);
    int          status = vgui_dw_run(&dialog->any);

    // return the selected color
    if ( status==WID_DIALSTATUS_Ok ) {
        *pen  = (Tcolor*)dialog->dw_pencolor .color;
        *fill = (Tcolor*)dialog->dw_fillcolor.color;
        *text = (Tcolor*)dialog->dw_textcolor.color;
        if (dialog->dw_stateCreating) {
            dialog->dw_active->color = color_newLazy(dialog->dw_active->color);
        }
    }
    // cleanup
    ydw_colors_free( dialog->dw_colors );
    vdw_colorcre_free(  dialog->dw_create );
    ydw_clrViewer_free( &dialog->dw_pencolor );
    ydw_clrViewer_free( &dialog->dw_fillcolor );
    ydw_clrViewer_free( &dialog->dw_textcolor );
    vgui_dw_free( &dialog->any );

    return status;
}

/*======================================================================*/
/*= Tydw_clrViewer utilities                                           =*/

static void ydw_clrViewer_update(Tydw_clrViewer*self)
{
    char    rgbbuf[100];
    Ccolor* viewcolor;
    if ( self->color!=0 ) {
        viewcolor = self->color;
        sprintf(rgbbuf,"#%02x%02x%02x",viewcolor->rgb.red,
                viewcolor->rgb.green,viewcolor->rgb.blue);
    } else {
        viewcolor = vgui_wn_getBgColor(self->gui,wn_dialLabel);
        sprintf(rgbbuf,"  none  ");
    }

    if ( self->isact ) 
        wid_button_setOn( self->lab );
    else    
        wid_button_setOff( self->lab );
    wid_label_setBgColor(self->view,viewcolor);
    wid_label_setText(self->rgb,rgbbuf);
}


// callback when the ud color viewer is selected 
static void ydw_clrViewer_cb(Tvgui_dw*vdw, int uid, void*ud)
{
    Tydw_clrPFT*    ydw     = (Tydw_clrPFT*)vdw;
    Tydw_clrViewer* self  = ud;
    ydw_clrPFT_addAction(ydw);

    ydw->dw_pencolor.isact  = 0;
    ydw->dw_fillcolor.isact = 0;
    ydw->dw_textcolor.isact = 0;
    ydw->dw_active = self;
    self->isact = 1;
    ydw_clrViewer_update( &ydw->dw_pencolor );
    ydw_clrViewer_update( &ydw->dw_fillcolor );
    ydw_clrViewer_update( &ydw->dw_textcolor );
}

static void ydw_clrViewer_init(Tydw_clrPFT*ydw, Tydw_clrViewer* self,
                CstrBrief title, Tcolor*c)
{
    Tvgui* gui      = vgui_dw_getVGui(&ydw->any);
    char*  width    = "MMMMMMMM";
    int    pixwidth = vgui_string_width(gui, NULL, width);
    self->gui       = gui;
    self->lab       = widD_button_onoff_nbfLab(&ydw->any
                            ,"Click me then click a color to change me"
                            ,ydw_clrViewer_cb,self,NULL
                            ,title,NULL);
    self->view  = widD_label_nbfBox(&ydw->any,0,pixwidth,35);
    self->rgb   = widD_label_nbf(&ydw->any,"#??????",width);
    self->color = c;
    self->isact = 0;
    self->wnone = 0;
}

static void        ydw_clrViewer_free( Tydw_clrViewer* self)
{
    wid_button_delete( self->lab );
}

/*======================================================================*/
/*= Tydw_clrPFT utilities                                               =*/

// Show the c color into the self->dw_active color viewer.
static void ydw_clrPFT_showAct(Tydw_clrPFT*self, Ccolor*c)
{
    Tydw_clrViewer*   active = self->dw_active;

    DBG0_CdwE(DBG_IdFmt,DBG_IdPar);
    TEMP_asf_printf(active==0,"unexpected NULL dw_active",0);
    active->color = c;
    ydw_clrViewer_update( self->dw_active );
    DBG1_CdwL(DBG_IdFmt,DBG_IdPar);
}

static void ydw_clrPFT_addAction(Tydw_clrPFT*self)
{
    Tydw_clrViewer*   active = self->dw_active;
    Ccolor*          c      = active->color;

    DBG0_CdwE(DBG_IdFmt,DBG_IdPar);

    if ( self->dw_stateCreating==0 && self->dw_statePicking==0 ) {
        DBG1_CdwL(DBG_IdFmt "not in add state (nothing done)",DBG_IdPar);
        return;
    }

    // get the color to add
    Ccolor* ac = active->color;

    // add the ac color to the private color set and panel
    if ( fig_colors_add0(self->dw_figcolors,ac)==0 ) {
        // ac not added (already existing in self->dw_figcolors
        ac = fig_colors_get0(self->dw_figcolors,ac);
    } else {
        ydw_colors_addClr(self->dw_colors,color_newLazy(ac));
    }

    // update the color widget
    ydw_clrPFT_showAct(self,ac);

    ydw_clrPFT_goToInitState(self);

    DBG1_CdwL(DBG_IdFmt,DBG_IdPar);
}

static void ydw_clrPFT_cancelAction(Tydw_clrPFT*self)
{
    Tydw_clrViewer*   active = self->dw_active;
    Ccolor*          c      = active->color;

    DBG0_CdwE(DBG_IdFmt,DBG_IdPar);
    if ( self->dw_stateCreating==0 && self->dw_statePicking==0 ) {
        DBG1_CdwL(DBG_IdFmt "not in add state (nothing done)",DBG_IdPar);
        return;
    }
    // free allocated color if any
    if ( self->dw_statePicking ) {
        color_deleteS0( (Tcolor**)&active->color );
    }
    // restore the color in the active viewer 
    ydw_clrPFT_showAct(self,self->dw_stateColor);

    // reinitialize the init state
    ydw_clrPFT_goToInitState(self);

    DBG1_CdwL(DBG_IdFmt,DBG_IdPar);
}

static void ydw_clrPFT_goToInitState(Tydw_clrPFT*self)
{
        self->dw_stateCreating = 0;
        self->dw_statePicking  = 0;
        self->dw_stateColor    = NULL;
        wid_button_enable( self->dw_btnAddCreate );
        wid_button_enable( self->dw_btnAddPickup );
        wid_button_enable( self->dw_btnCleanup );
        wid_button_disable(  self->dw_btnCancel );
//        gui_widegs_unshow(  self->dw_btnCancel->but_widEGS );
//        gui_widegs_show(  self->dw_btnCancel->but_widEGS );
        vdw_colorcre_disable(self->dw_create);
}

static void ydw_clrPFT_goToAddState(Tydw_clrPFT*self,
                                    int create, int pickup)
{
        self->dw_stateCreating = create;
        self->dw_statePicking  = pickup;
        self->dw_stateColor    = self->dw_active->color;
        wid_button_disable( self->dw_btnAddCreate );
        wid_button_disable( self->dw_btnAddPickup );
        wid_button_disable( self->dw_btnCleanup );
        wid_button_enable(  self->dw_btnCancel );
        if ( create )
            vdw_colorcre_enable(self->dw_create,self->dw_stateColor);
}

// In the ud color dialog, the c color of the cs color panel
// has been selected.
static void ydw_clrPFT_cb_sel(Tvdw_colorSel*cs, Ccolor*c, void*ud)
{
    Tydw_clrPFT*      self  = ud;

    DBG0_CdwE(DBG_IdFmt "color=%s", DBG_IdPar,DBG_Color2str(c));
    ydw_clrPFT_cancelAction(self);
    if ( c!=0 || (c==0 && self->dw_active->wnone==1) ) {
        ydw_clrPFT_showAct(self,c);
    }
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

// In the ud color dialog, user changed the current color to the c
// color in the cs color creator.
static void ydw_clrPFT_cb_newColor(Tvdw_colorCre*cw, Ccolor*c, void*ud)
{
    Tydw_clrPFT*      self  = ud;

    DBG0_CdwE(DBG_IdFmt "color=%s", DBG_IdPar,DBG_Color2str(c));
    ydw_clrPFT_showAct(self,c);
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

// In the i_self color dialog, user clicked the "add color" button.
// We start the color creator (ud is NULL)
static void ydw_clrPFT_cb_btnAdd(Tvgui_dw*_self, int uid, void*ud)
{
    Tydw_clrPFT*      self  = (void*)_self;
    Tydw_clrViewer*   active = self->dw_active;
    Ccolor*          c      = active->color;

    DBG0_CdwE(DBG_IdFmt, DBG_IdPar);
    ydw_clrPFT_goToAddState(self,1,0);
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

static void ydw_clrPFT_cb_btnPickup(Tvgui_dw*_self, int uid, void*ud)
{
    Tydw_clrPFT*      self  = (void*)_self;

    DBG0_CdwE(DBG_IdFmt, DBG_IdPar);
    PERM_spe_NYI(0);
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

// cancel the current color creation (_self is a color widget, ud is NULL)
static void ydw_clrPFT_cb_btnCancel(Tvgui_dw*_self, int uid, void*ud)
{
    Tydw_clrPFT*      self  = (void*)_self;

    DBG0_CdwE(DBG_IdFmt, DBG_IdPar);
    ydw_clrPFT_cancelAction(self);
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

static void ydw_clrPFT_cb_btnCleanup(Tvgui_dw*_self, int uid, void*ud)
{
    Tydw_clrPFT*      self  = (void*)_self;

    DBG0_CdwE(DBG_IdFmt, DBG_IdPar);
    PERM_spe_NYI(0);
    DBG1_CdwL(DBG_IdFmt, DBG_IdPar);
}

static Tydw_clrPFT* ydw_clrPFT_nbf(Tyfig_mw* ymw,
                                 Tfig_colors* figcolors,
                                 int isel, // P, F or T
                                 Tcolor*pen,Tcolor*fill,Tcolor*text)
{
    Tydw_clrPFT* self = (Tydw_clrPFT*)
                          vgui_dw_newTop(sizeof(*self), &ymw->any,
                            tool_strdup("color"),
                            WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok);

    self->dw_figcolors = figcolors;

    /* Second column */
    self->dw_colors = ydw_colors_nbfWN(SelfA,ymw,NULL,ydw_clrPFT_cb_sel,self,NULL);
    self->dw_create = vdwD_colorcre_nbf(&self->any
                            ,tool_strdup("Color management")
                            ,ydw_clrPFT_cb_newColor,self
                            ,NULL,0,0);

    self->dw_btns = widD_buttonPanel_newRaCa(SelfA,0,0);
    self->dw_btns->bp_colnb = 3;
    //self->dw_btns->bp_rownb = 4;
    self->dw_btnAddCreate = widD_button_std_newLab(SelfA,
            NULL,
            ydw_clrPFT_cb_btnAdd,NULL,NULL,
            "Add color",NULL);
    self->dw_btnAddPickup = widD_button_std_newLab(SelfA,
            NULL,
            ydw_clrPFT_cb_btnPickup,NULL,NULL,
            "Add color\nby Locking up",NULL);
    self->dw_btnCancel   = widD_button_std_newLab(SelfA,
            NULL,
            ydw_clrPFT_cb_btnCancel,NULL,NULL,
            "cancel color\ncreation",NULL);
    self->dw_btnCleanup   = widD_button_std_newLab(SelfA,
            NULL,
            ydw_clrPFT_cb_btnCleanup,NULL,NULL,
            "Remove\nunused colors",NULL);
    wid_buttonPanel_addButton(self->dw_btns, self->dw_btnAddCreate);
    wid_buttonPanel_addButton(self->dw_btns, self->dw_btnAddPickup);
    wid_buttonPanel_addButton(self->dw_btns, self->dw_btnCancel);
    wid_buttonPanel_addButton(self->dw_btns, self->dw_btnCleanup);
    wid_buttonPanel_dbf(self->dw_btns,NULL);

    /* first column */
    ydw_clrViewer_init(self,&self->dw_pencolor, "Pen color",  pen);
    ydw_clrViewer_init(self,&self->dw_fillcolor,"Fill color", fill);
    ydw_clrViewer_init(self,&self->dw_textcolor,"Text color", text);
    self->dw_fillcolor.wnone = 1;

    /* layout */
    vgui_dw_finishIC(&self->any
        //,0 ,WID_VDW_SSZ
        ,0|VDWLF_SEP2,self->dw_pencolorLab     ,1|VDWLF_SEP1,self->dw_colors->D_widEGS
        ,0           ,self->dw_pencolorView    ,1|VDWLF_SEP1,self->dw_btns->bp_widEGSext
        ,0           ,self->dw_pencolorRgb     ,1|VDWLF_SEP1,self->dw_create->dw_widEGS
        ,0|VDWLF_SEP3,self->dw_fillcolorLab
        ,0           ,self->dw_fillcolorView    
        ,0           ,self->dw_fillcolorRgb     
        ,0|VDWLF_SEP3,self->dw_textcolorLab     
        ,0           ,self->dw_textcolorView
        ,0           ,self->dw_textcolorRgb
    ,-1);
    self->dw_active = isel=='P' ? &self->dw_pencolor :
                      isel=='F' ? &self->dw_fillcolor : &self->dw_textcolor;
    self->dw_active->isact = 1;
    ydw_clrViewer_update( &self->dw_pencolor   );
    ydw_clrViewer_update( &self->dw_fillcolor  );
    ydw_clrViewer_update( &self->dw_textcolor  );

    vdw_colorcre_disable( self->dw_create );
    wid_button_disable( self->dw_btnCancel );

    return self;
}

/*======================================================================*/
