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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: widxt-buttonPanel.c 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-buttonPanel.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Lines, rows and matrix of buttons (EGS driver).                    =*/
/*=                                                                    =*/
/*= See the vgui/wid-buttonPanel.h header for the type and function    =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-button.h"

/*======================================================================*/

static void conv_accelKey2XTstr(char* buf, TaccelDef accelkey)
{
    int len; char* plus;

    buf[0]=0;
    if ( accelkey==0)
        return;
    plus="";
    if ( accelkey&GL_ACCEL_Meta ) {
        strcat(buf,plus);
        strcat(buf,"Meta");
        plus = "+";
    }
    if (   accelkey&GL_ACCEL_Control ) {
        strcat(buf,plus);
        strcat(buf,"Ctrl");
        plus = "+";
    }
    strcat(buf,"<Key>");
    len = strlen(buf);
    if (   accelkey&GL_ACCEL_Shift )
        buf[len]   = toupper(  accelkey&0xff );
    else
        buf[len]   = tolower(  accelkey&0xff );
    buf[len+1] = 0;
}

/*======================================================================*/
/*= The On/Off button                                                  =*/

static void gui_buttonPanel_accel(Widget widget, XEvent *event,
        String *argv, Cardinal* argc)
{
    Twid_buttonPanel* panel;
    Twid_button*      button;
    unsigned int      key0,key1;
    int                       i;

    DBG0_BPE("w=%p ev=%p : argv=%p argc=%p argc=%d",
        widget,event,argv,argc,argc?*argc:-1);
    TEMP_asf_printf( argc==0 || *argc!=3, 
        "%d unexpected argc (expected 3)", argc?*argc:-1);
    DBG1_BPM("w=%p ev=%p : argv[0]=%s argv[1]=%s argv[2]=%s",
            widget,event,argv[0], argv[1], argv[2]);

    sscanf(argv[0],"%p",&panel);
    sscanf(argv[1],"%x",&key0);
    sscanf(argv[2],"%x",&key1);

    for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1) {
        button = panel->vgbpm_buttons[i];
        if ( button->but_kind == WID_BUT_KD_OrnLab ) continue;
        if ( button->but_accelKey0==key0 && button->but_accelKey1==key1 )
            break;
    }
    if ( i==panel->vgbpm_buttonNb ) {
        DBG0_BPL("w=%p ev=%p : no button found", widget,event);
        return;
    }
    XButtonEvent e;
    e.type    = ButtonPress;
    e.button  = Button1;
    gui_button_onoff_cb(0,button,(XEvent*)&e,0);
    DBG0_BPL("w=%p ev=%p", widget,event);
}

/*======================================================================*/

static XtActionsRec mode_actions[] = {
    {"ModeEllipseR", gui_buttonPanel_accel}
};

extern void gui_buttonPanel_resize(Twid_buttonPanel*panel, int width, int height)
{
    int i,dx,dy,bd;
    /* First resize the label widget width to the panel width */
    for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1) {
        Twid_button* button = panel->vgbpm_buttons[i];
        if ( button->but_kind == WID_BUT_KD_OrnLab ) {
            widget_size(button->but_widEGS,&dx,&dy);
            bd = widget_bdw(button->but_widEGS);
            XtResizeWidget(button->but_widEGS,width-2*bd,dy-2*bd,bd);
        }
    }
    /* second resize the panel */
    bd = widget_bdw(panel->bp_widEGSext);
    XtResizeWidget(panel->bp_widEGSext,width-2*bd,height-2*bd,bd);
}

extern Tegs_widget gui_buttonPanel_build_SB(
        Twid_buttonPanel* panel, Tegs_widget parent)
{
    if ( panel->bp_scroll==0 ) return 0;

    Widget viewport;
    Boolean force = (panel->bp_scroll&2)!=0;
    viewport = XtVaCreateManagedWidget("vport", viewportWidgetClass, parent,
//        XtNwidth,      (Dimension)100,
//        XtNheight,     (Dimension)50,
        XtNallowHoriz, panel->bp_type=='H' ? True : False,
        XtNallowVert,  panel->bp_type=='V' ? True : False,
        XtNforceBars,  force,
        XtNuseBottom,  True,
        NULL);
    return viewport;
}

extern Tegs_widget gui_buttonPanel_build (
        Twid_buttonPanel* panel, Tegs_widget parent)
{
    Widget widget;
    Tvgui*      gui  = wid_buttonPanel_getVGui(panel);
    TcolorEGS bg=EGSCOLOR_default, fg=EGSCOLOR_default;
    vgui_wn_getColorEGS(gui, panel->vgbpm_wn, &fg, &bg);
    if ( parent==0 && panel->vgbpm_vmw)
        parent = vgui_mw_getWidEGS(panel->vgbpm_vmw);
    if ( parent==0 && panel->vgbpm_vdw)
        parent = vgui_dw_getWidEGS(panel->vgbpm_vdw);
    TEMP_asf_printf( parent==0 ,"no parent widget.",0);

    Widget viewport = gui_buttonPanel_build_SB(panel,parent);
    panel->bp_widEGSext = viewport;

    XT_DeclareArgs(20);
    ArgCount = 0;
    XT_NextArg(XtNborderWidth, 0);
    XT_NextArg(XtNforeground,   fg.ulong);
    XT_NextArg(XtNbackground,   bg.ulong);
    if ( panel->vgbpm_type=='P' ) {
        XT_NextArg(XtNhSpace,      panel->vgbpm_buttonHVsep);
        XT_NextArg(XtNvSpace,      panel->vgbpm_buttonHVsep);
        if ( WID_BP_RaCa_IsAlc(panel) ) {
            XT_NextArg(XtNresizable,    False);
            //XT_NextArg(XtNresize,       False);
            XT_NextArg(XtNorientation,  XtorientVertical); // expand vertically
        } else {
            //XT_NextArg(XtNresizable,    False);
            //XT_NextArg(XtNresize,       True);
            XT_NextArg(XtNorientation,  XtorientVertical); /* expand vertically */
            XT_NextArg(XtNborderWidth,  0);
            XT_NextArg(XtNwidth,      410); // updated later
        }
    } else  if ( panel->vgbpm_type=='H' ) {
        XT_NextArg(XtNhSpace,      panel->bp_buttonHVsep);
        XT_NextArg(XtNvSpace,      2);
        XT_NextArg(XtNresizable,    True);
        XT_NextArg(XtNorientation,  XtorientHorizontal); /* expand horizontally */
        //XT_NextArg(XtNwidth,      300); // updated later
    } else  if ( panel->vgbpm_type=='V' ) {
        XT_NextArg(XtNhSpace,      2);
        XT_NextArg(XtNvSpace,      panel->vgbpm_buttonHVsep);
        XT_NextArg(XtNresizable,   True);
        XT_NextArg(XtNorientation, XtorientVertical); /* use several rows */
    } else {
        TEMP_spf_printf( "0x%02x panel type not recognized.",
                panel->vgbpm_type);
    }
    widget = XtCreateManagedWidget("button_panel", boxWidgetClass,
                viewport ? viewport : parent, 
                Args, ArgCount);
//printf("%s: panel=%p ext=%p int=%p widget=%p\n",__func__,panel,panel->bp_widEGSext,panel->bp_widEGSint,widget);
    XtAppAddActions(E_AppContext, mode_actions, XtNumber(mode_actions));
    return widget;
}

#include <X11/IntrinsicI.h>

static void gui_buttonPanel_finishAccel(Twid_buttonPanel* panel)
{
    Tvgui*     gui  = wid_buttonPanel_getVGui(panel);
    Tgui_main* main = gui->g_main;
    char tmp[5000],key[100];
    int i,mask;
    tmp[0]=0;

    for (mask=GL_ACCEL_ALL>>8 ; mask>=0 ; mask--) {
      // Caution: Order is important:
      //    The line "Shift<Key>c:..." must precede
      //    the line "<Key>c:...".
      // It's the rule of the mask loop.
      for (i = 0; i < panel->vgbpm_buttonNb ; ++i) {
        Twid_button* b =  panel->vgbpm_buttons[i];
        if ( b->but_kind != WID_BUT_KD_OnOff )
            continue;
        if ( b->but_accelKey0!=0 && (b->but_accelKey0>>8)==mask ) {
            conv_accelKey2XTstr(key,b->but_accelKey0);
            sprintf(tmp+strlen(tmp),"%s:ModeEllipseR(%p,0x%04x,0x%04x)\n",
                key,panel,b->but_accelKey0,b->but_accelKey1);
        }
        if ( b->but_accelKey1!=0 && (b->but_accelKey1>>8)==mask ) {
            conv_accelKey2XTstr(key,b->but_accelKey1);
            sprintf(tmp+strlen(tmp),"%s:ModeEllipseR(%p,0x%04x,0x%04x)\n",
                key,panel,b->but_accelKey0,b->but_accelKey1);
        }
      }
    }
    //main->modePanelTable = XtParseAcceleratorTable(tmp);
    XtAccelerators accels = XtParseAcceleratorTable(tmp);
    for (i = 0; i < panel->vgbpm_buttonNb ; ++i) {
        Twid_button* b =  panel->vgbpm_buttons[i];
        XtOverrideTranslations(b->but_widEGS, accels);
        //XtOverrideTranslations(b->but_widEGS, XtParseAcceleratorTable(tmp));
    }
    //accels->stateTreeTbl[0]->simple.refCount -= 1;
}

#if 1
void gui_buttonPanel_finishRaCaFlc_cb_map(void* _panel)
{
    Twid_buttonPanel* panel = _panel;
#if 0
Dimension dx,dy;
printf("%s: panel=%p ext=%p int=%p\n",__func__,panel,panel->bp_widEGSext,panel->bp_widEGSint);
XtVaGetValues(panel->bp_widEGSint, XtNwidth,&dx, XtNheight,&dy, NULL);
printf("%s: panel=%p r/c=%d/%d dx/dy=%d/%d\n",__func__,panel,
panel->bp_rownb,panel->bp_colnb, dx,dy);
#endif
#if 0
    dx=350; dy=75;
    XtVaSetValues(panel->bp_widEGSint, XtNwidth,dx, XtNheight,dy, NULL);
    XtResizeWidget(panel->bp_widEGSint,dx,dy,2);
#else
    if ( panel->bp_btnNb>0 ) {
    int pdx,pdy,lx,ly,ldx,ldy;
    widget_resize_toEast(panel->bp_widEGSext,NULL);
    Widget last = panel->bp_btns[panel->bp_btnNb-1]->but_widEGS;
    widget_size(panel->bp_widEGSext,&pdx,&pdy);
    widget_geometry(last,&lx,&ly,&ldx,&ldy);
    pdy = ly+ldy+panel->bp_buttonHVsep;
    XtResizeWidget(panel->bp_widEGSext,pdx,pdy,2);
    }
#endif
#if 0
    XtVaGetValues(panel->bp_widEGSint, XtNwidth,&dx, XtNheight,&dy, NULL);
    printf("%s: panel=%p r/c=%d/%d dx/dy=%d/%d\n",__func__,panel,
            panel->bp_rownb,panel->bp_colnb, dx,dy);
#endif
}
#endif

// when buttons have different size
extern void gui_buttonPanel_finishRaCaFlc(Twid_buttonPanel* panel)
{
    widget_cb_addU(panel->bp_widEGSint, panel, YFIG_EVENT_Map, gui_buttonPanel_finishRaCaFlc_cb_map, NULL);
//printf("%s:a: panel=%p r/c=%d/%d btn=%d\n",__func__,panel, panel->bp_rownb,panel->bp_colnb,panel->bp_btnNb);
    int colnb,rownb;
    if ( panel->bp_colnb!=0 && panel->bp_rownb==0 ) {
        colnb = panel->bp_colnb;
        rownb = (panel->bp_btnNb+colnb-1)/colnb;
    } else if ( panel->bp_colnb==0 && panel->bp_rownb!=0 ) {
        rownb = panel->bp_rownb;
        colnb = (panel->bp_btnNb+rownb-1)/rownb;
    } else if ( panel->bp_colnb!=0 && panel->bp_rownb!=0 ) {
        colnb = panel->bp_colnb;
        rownb = panel->bp_rownb;
    } else {
        PERM_spf_printf("either col or row number must be specified",0);
    }
//printf("%s:b: panel=%p r/c=%d/%d\n",__func__,panel, rownb,colnb);
widget_place_GrowToSE(panel->bp_widEGSint,0,NULL,0,NULL);
#if 0 // no effect cause the boxWidgetClass size is fully handled by Xaw ....
    int i,ii; Widget w;
    Dimension dx = panel->bp_buttonHVsep;
    if ( panel->bp_btnNb==0 ) {
        dx += 25*colnb + panel->bp_buttonHVsep*colnb;
    } else for (i=0,ii=0 ; i<colnb ; i++) {
        Dimension bdx;
        w = panel->bp_btns[ii]->but_widEGS;
        XtVaGetValues(w, XtNwidth,&bdx, NULL);
        dx += bdx + panel->bp_buttonHVsep
                  + 2*panel->bp_buttonBorder;
        if ( ii<(panel->bp_btnNb-1) ) ii+=1;
    }
printf("c:%s: panel=%p dx=%d\n",__func__,panel,dx);
//  Tvgui*     gui  = wid_buttonPanel_getVGui(panel);
//  for (i=0; i<panel->bp_btnNb ; i++) {
//      w = panel->bp_btns[i]->but_widEGS;
//      XtUnmanageChild(w);
//  }
    //XtResizeWidget(panel->bp_widEGSint,dx,100,5);
    XtVaSetValues(panel->bp_widEGSint,
            XtNwidth,        dx,
          XtNheight,       (Dimension)200,
//          XtNresizable,    True,
//          XtNorientation,  XtorientVertical, /* expand vertically */
//          XtNborderWidth,  (Dimension)3,
//          XtNforeground,   (unsigned long)gui->vg_foreground->gcolor,
//          //XT_NextArg(XtNbackground,   gui->vg_background->gcolor);
//          XtNbackground,   (unsigned long)gui->vg_black->gcolor,
//          XtNborderColor,  (unsigned long)gui->vg_daPrevColor->gcolor,
//          //XT_NextArg(XtNmappedWhenManaged, False);
            NULL);
//  for (i=0; i<panel->bp_btnNb ; i++) {
//      w = panel->bp_btns[i]->but_widEGS;
//      XtManageChild(w);
//  }
#endif
}

// when buttons have the same size
extern void gui_buttonPanel_finishRaCaAlc(Twid_buttonPanel* panel)
{
    Widget panelEGS = panel->bp_widEGSint;
    if ( panel->bp_colnb!=0 && panel->bp_rownb==0 ) {
        int rownb = wid_buttonPanel_getLayout(panel, panel->bp_colnb,0,0,0);
        Dimension dx = wid_buttonPanel_getWidth(panel,panel->bp_colnb);
        Dimension dy = wid_buttonPanel_getHeight(panel,rownb);
        XtVaSetValues(panelEGS, XtNwidth,dx, XtNheight,dy, NULL);
        return ;
    }
    if ( panel->bp_colnb==0 && panel->bp_rownb!=0 ) {
        int colnb = (panel->bp_btnNb+panel->bp_rownb-1)/panel->bp_rownb;
        int rownb = wid_buttonPanel_getLayout(panel, colnb,0,0,0);
        Dimension dx = wid_buttonPanel_getWidth (panel,colnb);
        Dimension dy = wid_buttonPanel_getHeight(panel,rownb);
        XtVaSetValues(panelEGS, XtNwidth,dx, XtNheight,dy, NULL);
        return ;
    }
    if ( panel->bp_colnb!=0 && panel->bp_rownb!=0 ) {
        Dimension dx = wid_buttonPanel_getWidth(panel,panel->bp_colnb);
        Dimension dy = wid_buttonPanel_getHeight(panel,panel->bp_rownb);
        XtVaSetValues(panelEGS, XtNwidth,dx, XtNheight,dy, NULL);
        return ;
    }
}

extern void gui_buttonPanel_finishRaCa(Twid_buttonPanel* panel)
{
    gui_buttonPanel_finishAccel(panel);
//printf("%s: panel=%p row=%d com=%d nb=%d\n",__func__,
//panel, panel->bp_rownb, panel->bp_colnb, panel->bp_btnNb);

    if WID_BP_RaCa_IsFlc(panel) {
        gui_buttonPanel_finishRaCaFlc(panel);
    } else {
        TEMP_ase_printf(
            panel->bp_buttonWidth==0 || panel->bp_buttonHeight==0,
           "bp_buttonWidth=%d bp_buttonHVsep=%d : must be both !=0",
            panel->bp_buttonWidth, panel->bp_buttonHeight);
        gui_buttonPanel_finishRaCaAlc(panel);
    }
}

extern void gui_buttonPanel_finishR1Cn(Twid_buttonPanel* panel)
{
    int i;
    gui_buttonPanel_finishAccel(panel);
    if ( panel->bp_adjustR1C1 && panel->bp_btnNb>1 ) {
        // button must have the same height
        Widget btn; Dimension max,tmp;
        i=0;
        // get max height
        btn = panel->bp_btns[i]->but_widEGS;
        XtVaGetValues(btn, XtNheight,&max, NULL);
        for (i=i+1;i<panel->bp_btnNb;i+=1) {
            btn = panel->bp_btns[i]->but_widEGS;
            XtVaGetValues(btn, XtNheight,&tmp, NULL);
            if ( tmp>max ) max=tmp;
        }
        // update the button height.
        for (i=0;i<panel->bp_btnNb;i+=1) {
            btn = panel->bp_btns[i]->but_widEGS;
            XtVaSetValues(btn, XtNheight,max, NULL);
        }
    }
    if ( panel->bp_scroll!=0 ) {
        Dimension pdx,pdy,bdx,bdy;
        pdy = 0;
        pdx = panel->bp_buttonHVsep; 
        for (i=0;i<panel->bp_btnNb;i+=1) {
            XtVaGetValues(panel->bp_btns[i]->but_widEGS,
                XtNwidth,&bdx, XtNheight,&bdy, NULL);
            pdx += bdx + panel->bp_buttonHVsep;
            if ( bdy>pdy ) pdy = bdy;
        }
#if 1
        XtVaSetValues(panel->bp_widEGSint,
                XtNwidth,pdx, XtNheight,pdy, NULL);
        pdx+=50;
        pdy+=15;
        XtVaSetValues(panel->bp_widEGSext,
                XtNwidth,pdx, XtNheight,pdy, NULL);
#endif
    }
}

extern void gui_buttonPanel_finishRnC1(Twid_buttonPanel* panel)
{
    gui_buttonPanel_finishAccel(panel);
    if ( panel->bp_scroll!=0 ) {
        int i;
        Dimension pdx,pdy,bdx,bdy;
        pdx = 0;
        pdy = panel->bp_buttonHVsep; 
        for (i=0;i<panel->bp_btnNb;i+=1) {
            XtVaGetValues(panel->bp_btns[i]->but_widEGS,
                XtNwidth,&bdx, XtNheight,&bdy, NULL);
            pdy += bdy + panel->bp_buttonHVsep;
            if ( bdx>pdx ) pdx = bdx;
        }
        XtVaSetValues(panel->bp_widEGSint,
                XtNwidth, pdx,
                //XtNheight,pdy,
                NULL);
        pdx+=25;
        //pdy+=50;
        XtVaSetValues(panel->bp_widEGSext,
                XtNwidth,pdx,
                //XtNheight,pdy,
                NULL);
    }
}

extern void gui_buttonPanel_update(Twid_buttonPanel* panel)
{
#if 0 // no effect
    int i;
    for (i=0 ; i<panel->bp_btnNb ; i+=1 ) {
        XtUnmanageChild( panel->bp_btns[i]->but_widEGS );
    }
    for (i=0 ; i<panel->bp_btnNb ; i+=1 ) {
        XtManageChild( panel->bp_btns[i]->but_widEGS );
    }
#endif
}

/*======================================================================*/

