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
 *      $Id: guixt-dw.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-dw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for the type and function            =*/
/*= documentation.                                                     =*/
/*= See the guixt/guixt-util.h header for the documentation about      =*/
/*= the xt_dw_... functions.                                           =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt/guixt-top.h"
#include "guixt/guixt-create.h"
#include "guixt-dw.h"

#include "vgui/wid-buttonPanel.h"

/*======================================================================*/
/*= Runing dialog                                                      =*/

static Twid_dialStatus vt_modal_run2(Tvgui_dw*d, Tegs_widget inw, int x, int y)
{
    Tvgui*       gui = vgui_dw_getVGui(d);
    Tgui_main*   main = gui->g_main;
    Tvgui_dwTop* top  = d->top;
    TGUI_dialog* xtd  = d->guiUserdata;

    if ( inw ) {
        XtRealizeWidget(xtd->shell);
        XReparentWindow(E_Display,_XtWindow(xtd->shell),_XtWindow(inw), x, y);
        XtPopup(xtd->shell, XtGrabExclusive);
        if ( top->dt_focus )
            XtSetKeyboardFocus(vgui_dw_getWidEGS(d),top->dt_focus);
    } else {
        XtPopup(xtd->shell, XtGrabExclusive);
        XSetWMProtocols(main->m_display, XtWindow(xtd->shell), &main->m_wm_delete_window, 1);
    }

   while ( (top->dt_buttSelected&top->dt_buttsMaskCur)==0 ) {
        XEvent	    event;
        XtAppNextEvent(gui->g_main->m_appContext,&event);
//printf("%s: d=%p type=%d\n", __func__, d, event.type);
        if (  top->dt_wmX &&
             (top->dt_buttsMaskCur&WID_DIALSTATUS_Cancel)!=0 &&
              event.type == ClientMessage &&
            ((Atom)event.xclient.data.l[0]) == main->m_wm_delete_window ) {
                /* cancel is authorized and asked by WM: we accept. */
            top->dt_buttSelected = WID_DIALSTATUS_Cancel;
            break;
        } 
	    XtDispatchEvent(&event);
    }
    //XtPopdown(xtd->shell);
    return top->dt_buttSelected;
}

static void vt_modeless_start(Tvgui_dw* d)
{
    Tvgui*       gui  = vgui_dw_getVGui(d);
    Tgui_main*   main = gui->g_main;
    Tvgui_dwTop* top  = d->top;
    TGUI_dialog* xtd  = d->guiUserdata;

    top->dt_buttsMaskCur = top->dt_buttsMaskOrg;
    top->dt_buttSelected = WID_DIALSTATUS_None;

    XtPopup(xtd->shell, XtGrabNone);
    XSetWMProtocols(main->m_display, XtWindow(xtd->shell), &main->m_wm_delete_window, 1);
}

static void dw_vtInit(Tvgui_dw* d)
{
    if ( d->top==0 ) return;
    d->top->vt_modal_run2     = vt_modal_run2;
    d->top->vt_modeless_start = vt_modeless_start;
}

/*======================================================================*/
/*= Constructors and destructor                                        =*/

static void vgui_dw_cb_mapTop(Tvgui_dw*self)
{
    Tvgui_dwTop* top  = self->top;
    Twid_buttonPanel* btns = top->dt_ctlButtons;
    if ( btns!=0 && btns->bp_widEGSext!=0 )
        widget_resize_toEast(btns->bp_widEGSext,NULL);
}

extern Tegs_widget gui_dw_create(Tvgui_dw* d, CstrBrief wn,
                                int bd, int dx, int dy, int resizable)
{
    Tvgui*       gui = vgui_dw_getVGui(d);
    TGUI_dialog* xtd;
    Widget       parent;

    dw_vtInit(d);
    xtd = tool_malloc_0( sizeof(*xtd) );
    xtd->dialog     = d;
    d->guiUserdata = xtd;

    if ( d->pDialog==0 ) {
        // top dialog
        // create the shell
        xtd->shell = create_shell(gui, wn_dialShell,
                bd,dx,dy,
                EGSCOLOR_default,EGSCOLOR_default,
                d->top->dt_title
        );
        parent = xtd->shell;
    } else {
        // sub dialog
        //xtd = d->pDialog->guiUserdata;
        //d->guiUserdata = xtd;
        parent = d->pDialog->D_widEGS;
    }

    // now we create the form
    d->D_widEGS =  create_form(gui,parent,wn,
      d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 2 : 0, // hvsep
      bd>=0 ? bd : d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 1 : 0, // bd
      dx,dy,
      EGSCOLOR_default,EGSCOLOR_default
    );
    if ( d->pDialog==0 ) {
        widget_cb_addU( d->D_widEGS, d, YFIG_EVENT_Map, vgui_dw_cb_mapTop, NULL);
    }
    return d->D_widEGS;
}

extern Tegs_widget gui_dw_create2(Tvgui_dw* d, Tegs_widget parent, CstrBrief wn,
                                int bd, int dx, int dy, int resizable)
{ 
    Tvgui*       gui = vgui_dw_getVGui(d);
    TGUI_dialog* xtd;

    dw_vtInit(d);
    xtd = tool_malloc_0( sizeof(*xtd) );
    xtd->dialog     = d;
    d->guiUserdata = xtd;

    // now we create the form
    d->D_widEGS =  create_form(gui,parent,wn,
      d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 2 : 0, // hvsep
      bd>=0 ? bd : d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 1 : 0, // bd
      dx,dy,
      EGSCOLOR_default,EGSCOLOR_default
    );
#if 0
    XtVaSetValues(d->widEGS, XtNresizable,(Boolean)False, NULL);
    if ( d->pDialog==0 ) {
        widget_cb_addU( d->D_widEGS, d, YFIG_EVENT_Map, vgui_dw_cb_mapTop, NULL);
    }
#endif
    return d->D_widEGS;
}

#if 0 // TRACE_DESTROY
static void Removed_dialog(Widget w, XtPointer ud, XtPointer closure)
{ printf("%s: widget=%p ud=%p\n",__func__,w,ud); }

static void Removed_shell(Widget w, XtPointer ud, XtPointer closure)
{ printf("%s: widget=%p ud=%p\n",__func__,w,ud); }

extern void gui_dw_free(Tvgui_dw* d)
{
    TGUI_dialog* xtd  = d->guiUserdata;
    XtAddCallback( d->D_widEGS , XtNdestroyCallback, Removed_dialog, (XtPointer) d);
    if ( xtd && xtd->shell!=NULL )
        XtAddCallback( xtd->shell , XtNdestroyCallback, Removed_shell, (XtPointer) d);
    if ( xtd && xtd->shell!=NULL ) {
        //XtDestroyWidget(d->D_widEGS);
        XtDestroyWidget(xtd->shell);
    }
    tool_freeS0(&d->guiUserdata);
}
#else
extern void gui_dw_free(Tvgui_dw* d)
{
    TGUI_dialog* xtd  = d->guiUserdata;
    if ( xtd && xtd->shell!=NULL ) {
        XtDestroyWidget(xtd->shell);
    }
    tool_freeS0(&d->guiUserdata);
}
#endif

extern void gui_dw_finishSdx(Tvgui_dw*vdw, Tegs_widget*ws, int wn)
{
    if ( wn<=1 ) return;

    int i;
    Dimension dx[wn];
    XtVaGetValues(ws[0], XtNwidth,&dx[0], NULL);
    Dimension max = dx[0];
    for ( i=1 ; i<wn ; i+=1 ) {
        XtVaGetValues(ws[i], XtNwidth,&dx[i], NULL);
        if ( max<dx[i] ) max=dx[i];
    }

    for ( i=0 ; i<wn ; i+=1 ) {
        if ( dx[i]==max ) continue;
        XtVaSetValues(ws[i], XtNwidth,max, NULL);
    }
}

extern void gui_dw_finishSdy(Tvgui_dw*vdw, Tegs_widget*ws, int wn);

/*======================================================================*/
/*= Miscellaneous utilities                                              =*/

extern void            gui_dw_setTitle(Cvgui_dw* d, TstrBrief t)
{
    TGUI_dialog* xtd=d->guiUserdata;
    XtVaSetValues(xtd->shell, XtNtitle,t?t:PACKAGE, NULL);
}

/*======================================================================*/
/*= Private utilities of XT driver (see guixt-util.h)                  =*/

static void xt_dw_CBresize(TGUI_dialog* xtd, XEvent *event)
{
    Tvgui_dw* d      = xtd->dialog;
    Tvgui*    gui    = vgui_dw_getVGui(d);
    int       status = 0;

    switch ( event->type ) {
      case VisibilityNotify:
          xtd->nbVisbilyNotify += 1;
          if ( xtd->nbVisbilyNotify==1 ) {
            XVisibilityEvent* e = &event->xvisibility;
            DBG0_Dw("VisibilityNotify event (type=%d).",event->type);
            status = xtd->cbResize(d);
          }
          break;

      case ConfigureNotify:
          DBG0_Dw("ConfigureNotify event (type=%d).",event->type);
          status = xtd->cbResize(d);
          break;

      default:
        PERM_spw_printf(
            "unexpected event (type=%d) is ignored.",event->type);
        return;
    }
}

extern void xt_dw_addCBresize(Tvgui_dw*d, Txt_dw_cb_resize cb)
{
    TGUI_dialog* xtd = d->guiUserdata;
    xtd->cbResize   = cb;

    widget_cb_addU(d->D_widEGS, xtd,
        YFIG_EVENT_ResizeEvent,     xt_dw_CBresize, 
        YFIG_EVENT_VisibilityEvent, xt_dw_CBresize, 
        NULL
    );
}

/*======================================================================*/
