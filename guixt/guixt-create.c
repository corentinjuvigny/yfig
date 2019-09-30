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
 *      $Id: guixt-create.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-create.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for creating widgets.                                    =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt/guixt-create.h"

#include "vgui/vgui-dw.h"

/*======================================================================*/
/*= label widget                                                       =*/

extern Tegs_widget create_label(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    XT_DeclareArgs(10);

    if ( text==0 ) text="";

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bdw>=0)              XT_NextArg(XtNborderWidth, bdw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNlabel, text);
    Widget w = XtCreateManagedWidget(
            wn, labelWidgetClass, pw, Args, ArgCount);
    return (Tegs_widget) w;
}

extern Widget create_label_place(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn, CstrBrief text,
        Tegs_widget lw, Tegs_widget tw)
{
        Widget w = create_label_short(gui,pw,wn,text);
        widget_place_FixedSize(w,-1,lw,-1,tw);
        return w;
}

/*======================================================================*/
/*= button widget                                                      =*/

extern Tegs_widget create_button(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, CstrBrief event, void*cb, void*ud1, void*ud2)
{
    XT_DeclareArgs(15);
    Widget w;

    if ( text==0 ) text="";
    if ( event==0 ) event=YFIG_EVENT_Button1Pressed;

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    if (text!=0)             XT_NextArg(XtNlabel, text);
    XT_NextArg(XtNjustify, XtJustifyCenter);
    XT_NextArg(XtNresize    , False);
    XT_NextArg(XtNresizable , False);
    //XT_NextArg(XtNinternalWidth, 0);
    //XT_NextArg(XtNinternalHeight, 0);
    //XT_NextArg(XtNhighlightThickness, 6);
    w = XtCreateManagedWidget( wn,
            commandWidgetClass, parent, Args, ArgCount);
    if ( cb!=0 && ud1!=0 && ud2==0 ) {
         widget_cb_addU(w,ud1, event, cb, NULL);
    } else if ( cb!=0 && ud1!=0 && ud2!=0 ) {
         widget_cb_addUU(w,ud1,ud2, event, cb, NULL);
    } else if ( cb!=0 && ud1==0 && ud2==0 ) {
        XtAddCallback(w,XtNcallback,cb,ud1);
    }
    return w;
}

extern Tegs_widget create_buttonWLPXM(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, CstrBrief event, void*cb, void*ud1, void*ud2,
        Pixmap lpxm)
{
    XT_DeclareArgs(15);
    Widget w;

    if ( text==0 ) text="";
    if ( event==0 ) event=YFIG_EVENT_Button1Pressed;

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    if (text!=0)             XT_NextArg(XtNlabel, text);
    XT_NextArg(XtNjustify, XtJustifyCenter);
    XT_NextArg(XtNresize    , False);
    XT_NextArg(XtNresizable , False);
    //XT_NextArg(XtNinternalWidth, 0);
    //XT_NextArg(XtNinternalHeight, 0);
    //XT_NextArg(XtNhighlightThickness, 6);
#if 1 // FIXME
    if ( lpxm==0 ) lpxm = gui->g_main->m_bmCmBoxOffSm;
    XT_NextArg(XtNleftBitmap, lpxm); // checkmark box
#endif
    w = XtCreateManagedWidget( wn,
            commandWidgetClass, parent, Args, ArgCount);
    if ( cb!=0 && ud1!=0 && ud2==0 ) {
         widget_cb_addU(w,ud1, event, cb, NULL);
    } else if ( cb!=0 && ud1!=0 && ud2!=0 ) {
         widget_cb_addUU(w,ud1,ud2, event, cb, NULL);
    } else if ( cb!=0 && ud1==0 && ud2==0 ) {
        XtAddCallback(w,XtNcallback,cb,ud1);
    }
    return w;
}

extern Tegs_widget create_menuButton(
        Tvgui* gui, Widget parent, CstrBrief wn,
        int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, CstrBrief menuname)
{
    XT_DeclareArgs(10);

    if ( text==0 ) text="";

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNlabel,       text);
    XT_NextArg(XtNmenuName,    menuname);
    XT_NextArg(XtNleftBitmap,  gui->g_main->m_pmMenuArrow);
    //XT_NextArg(XtNleftBitmap,  gui->g_main->m_pmMenuEntryArrow);
    return (Tegs_widget) XtCreateManagedWidget(
            wn, menuButtonWidgetClass, parent, Args, ArgCount);
}

/*======================================================================*/
/*= text input widget                                                  =*/

extern Twid_slin* create_slin(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text, Twid_cb_TU chg, Twid_cb_TU val, void*ud)
{
    Twid_slin *self = tool_malloc( sizeof(*self) );
    self->gui = gui;
    self->chg = chg;
    self->val = val;
    self->ud  = ud;

    self->widEGS = gui_slin_dbf(self,pw,wn, bdw,dx,dy, bg,fg, text);

    return self;
}

extern Twid_slin* create_slin_place(
        Tvgui* gui, Tegs_widget pw, CstrBrief wn, int dx, int dy,
        CstrBrief text, Twid_cb_TU chg, Twid_cb_TU val, void*ud,
        Tegs_widget lw, Tegs_widget tw)
{
    Twid_slin* slin = create_slin(gui,pw,wn,
            0,dx,dy,EGSCOLOR_default,EGSCOLOR_default,
            text, chg,val,ud);
    widget_place_FixedSize(slin->widEGS,-1,lw,-1,tw);
    return slin;
}

/*======================================================================*/
/*= container widgets                                                  =*/

extern Tegs_widget create_shell(
        Tvgui* gui, const char* wn,
        int bw, int x, int y, TcolorEGS bg, TcolorEGS fg,
        const char* _title)
{
    XT_DeclareArgs(15);
    Tgui_main* main = gui->g_main;
    const char *title = _title==0 ? "" : _title;

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (x>0)                 XT_NextArg(XtNx,           x);
    if (y>0)                 XT_NextArg(XtNy,           y);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNtitle, title);
    //XT_NextArg(XtNresizable,   1); // IVG
    //XT_NextArg(XtNallowShellResize,   False); // IVG
    XT_NextArg(XtNallowShellResize,   True); // IVG
    //XT_NextArg(XtNminWidth,   500); // IVG
    //XT_NextArg(XtNmaxWidth,   500); // IVG
    return (Tegs_widget) XtCreatePopupShell(
            wn, transientShellWidgetClass, main->m_topWidget, Args, ArgCount);
            //wn, overrideShellWidgetClass, main->m_topWidget, Args, ArgCount);
}

extern Tegs_widget create_form(
        Tvgui* gui, Tegs_widget parent, const char* wn,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg)
{
    XT_DeclareArgs(10);

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    XT_NextArg(XtNdefaultDistance, hvsep);
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    //XT_NextArg(XtNresize,      1); // IVG
    XT_NextArg(XtNresizable,   1); // IVG

    return (Tegs_widget) XtCreateManagedWidget(
            wn, formWidgetClass, parent, Args, ArgCount);
}

extern Tvgui_dw* create_dwform(Tvgui_dw* vdw,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg)
{
    Tvgui*    gui  = vgui_mw_getVGui(vdw->D_vmw);
    Tvgui_dw* self = tool_malloc_0( sizeof(*self) );
    self->D_vmw = vdw->D_vmw;
    self->D_wcn = vdw->D_wcn ? vdw->D_wcn : wn_dialTop;
    self->D_widEGS = create_form(
        gui, vdw->D_widEGS, self->D_wcn,
        hvsep, bw, dx, dy, bg, fg);
    self->D_pDialog = vdw;
    return self;
}

/*======================================================================*/
/*= string list widget                                                 =*/

extern Tegs_widget create_listString(Tegs_widget*viewport,
        Tvgui* gui, Tegs_widget parent, const char* wn,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        XtCallbackProc cb, void* ud)
{
    Widget _viewport,list;
    Widget* pviewport = viewport ? (Widget*)viewport : &_viewport;
    XT_DeclareArgs(20);

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bw>=0)               XT_NextArg(XtNborderWidth, bw);
    if (dx>0)                XT_NextArg(XtNwidth,       dx);
    if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNvertDistance, hvsep);
    XT_NextArg(XtNhorizDistance, hvsep);
    XT_NextArg(XtNallowVert, True);
    XT_NextArg(XtNforceBars, True);
    *pviewport = XtCreateManagedWidget("vport", viewportWidgetClass,
                    parent, Args, ArgCount);
    ArgCount=0;
    //if (dx>0)                XT_NextArg(XtNwidth,       dx);
    //if (dy>0)                XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE) XT_NextArg(XtNforeground,  fg.ulong);
    list = XtCreateManagedWidget("string_list", listWidgetClass,
                   *pviewport, Args, ArgCount);
    //widget_place_GrowToSE(list,0,NULL,0,NULL);
    //widget_place_FixedSize(list,0,NULL,0,NULL);
    if ( cb )
        XtAddCallback(list, XtNcallback,cb,ud);

    return (Tegs_widget) list;
    // make scrollbar sensitive to mouse wheel
    // This is only possible if the XT scrollbar widget supports
    // the StartScroll action (It's a XT compilation option).
    { // check if StartScroll is enabled
        int num_actions,i;
        XtActionList action_list;
        XtGetActionList(scrollbarWidgetClass, &action_list, &num_actions);
        for (i=0; i<num_actions; i++) {
            //printf("%2d: %s\n",i,action_list[i].string);
            if (strcasecmp(action_list[i].string, "startscroll") == 0)
                goto found;
        }
    }
    goto end;
found: while(0);
static const char liststring_accel[] =
    "<Btn4Down>: StartScroll(Backward)\n"
    "<Btn5Down>: StartScroll(Forward)\n"
    "<BtnUp>:    NotifyScroll(FullLength) EndScroll()\n";
static XtAccelerators liststring_acceltable = 0;
    if (liststring_acceltable == 0)
        liststring_acceltable = XtParseAcceleratorTable(liststring_accel);
    Widget scrollbar = XtNameToWidget(*pviewport,"vertical");
    XtAugmentTranslations(scrollbar,liststring_acceltable);
    XtVaSetValues(scrollbar, XtNaccelerators,liststring_acceltable, NULL);
    XtInstallAccelerators(list, scrollbar);
end:
    return (Tegs_widget) list;
}

extern void xt_listString_chgStrs(Tegs_widget list, char** strs, int nb)
{ 
    if ( nb!=0 ) {
        XawListChange(list, (char**)strs,nb, 0, TRUE);
    } else {
        char* str = NULL;
        XawListChange(list, &str,nb, 0, TRUE);
    }
}

/*======================================================================*/
