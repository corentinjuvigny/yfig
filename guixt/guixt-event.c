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
 *      $Id: guixt-event.c 48 2017-03-17 11:24:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-event.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities to add event callback to a widget.                       =*/
/*= See the "guixt/event.h" header for type and function documentation.=*/
/*======================================================================*/

#include "guixt.h"

/*======================================================================*/
/*= implementation of widget_cb_addU/U callback functions              =*/

static void widget_cb_CB_FU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void* ud);
    void* ud;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p",&func,&ud)!=2,
        "can not find func or ud (params=%p *param=%s)\n",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p",&func,&ud);
    if ( func!=0 ) func(ud);
}

static void widget_cb_CB_FUU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void* ud1, void* ud3);
    void *ud1,*ud2;;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2)!=3,
        "can not find func or ud (params=%p *param=%s)",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2);
    if ( func!=0 )
        func(ud1,ud2);
}

static void widget_cb_CB_wmpFU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void* ud, Atom a);
    void* ud;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p",&func,&ud)!=2,
        "can not find func or ud (params=%p *param=%s)",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p",&func,&ud);
    if ( func!=0 )
        func(ud,(Atom)event->xclient.data.l[0]);
}

static void widget_cb_CB_wmpFUU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void*ud1,void*ud2, Atom a);
    void *ud1,*ud2;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2)!=3,
        "can not find func or ud (params=%p *param=%s)",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2);
    if ( func!=0 )
        func(ud1,ud2,(Atom)event->xclient.data.l[0]);
}

static void widget_cb_CB_eveFU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void* ud, XEvent* e);
    void* ud;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p",&func,&ud)!=2,
        "can not find func or ud (params=%p *param=%s)",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p",&func,&ud);
    if ( func!=0 )
        func(ud,event);
}

static void widget_cb_CB_eveFUU(Widget w,
        XEvent *event, String *params, Cardinal *num_params)
{
    void (*func)(void* ud1, void* ud2, XEvent* e);
    void *ud1,*ud2;
    TEMP_asf_printf( params==0 || sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2)!=3,
        "can not find func or ud (params=%p *param=%s)",
        params, params==0 || *params==0 ? "???" : *params);
    sscanf(*params,"%p:%p:%p",&func,&ud1,&ud2);
    if ( func!=0 )
        func(ud1,ud2,event);
}

/*======================================================================*/

extern void widget_cb_init (Tvgui*gui)
{
    Tgui_main* main = gui->g_main;
    static XtActionsRec actions[] = {
          {"widget_cb_CB_FU",     widget_cb_CB_FU},
          {"widget_cb_CB_wmpFU",  widget_cb_CB_wmpFU},
          {"widget_cb_CB_eveFU",  widget_cb_CB_eveFU},
          {"widget_cb_CB_FUU",    widget_cb_CB_FUU},
          {"widget_cb_CB_wmpFUU", widget_cb_CB_wmpFUU},
          {"widget_cb_CB_eveFUU", widget_cb_CB_eveFUU},
    };
    XtAppAddActions(main->m_appContext, actions, XtNumber(actions));
}

/*======================================================================*/

static const char * widget_cb_id_tableU[10] = {
    "<Message>WM_PROTOCOLS: widget_cb_CB_wmpFU",
    "<ConfigureNotify>: widget_cb_CB_eveFU",
    "<VisibilityNotify>: widget_cb_CB_eveFU",
};

static const char * widget_cb_id_tableUU[10] = {
    "<Message>WM_PROTOCOLS: widget_cb_CB_wmpFUU",
    "<ConfigureNotify>: widget_cb_CB_eveFUU",
    "<VisibilityNotify>: widget_cb_CB_eveFUU",
};

extern void widget_cb_addU(Widget w, void* ud, ...)
{
    const char** id_table = widget_cb_id_tableU;

    char    tmp[20*(75+2*20)]; // 20 entries (id+func_pt+ud_pt)
    char*   p=tmp;
    char*   id;
    va_list ap;

    va_start(ap,ud);
    while ( (id=va_arg(ap,char*))!=0 ) {
        void* cb = va_arg(ap,void*);
        if ( *id=='<' )
            p += sprintf(p,"%s: widget_cb_CB_FU(%p:%p)\n",id,cb,ud);
        else
            p += sprintf(p,"%s(%p:%p)\n",id_table[*id-'0'],cb,ud);
    }
    va_end(ap);

    if (p!=tmp)
        XtOverrideTranslations(w, XtParseTranslationTable(tmp));
}

extern void widget_cb_addUU(Widget w, void* ud1, void* ud2,...)
{
    const char** id_table = widget_cb_id_tableUU;

    char    tmp[20*(75+2*20)]; // 20 entries (id+func_pt+ud_pt)
    char*   p=tmp;
    char*   id;
    va_list ap;

    va_start(ap,ud2);
    while ( (id=va_arg(ap,char*))!=0 ) {
        void* cb = va_arg(ap,void*);
        if ( *id=='<' )
            p += sprintf(p,"%s: widget_cb_CB_FUU(%p:%p:%p)\n",id,cb,ud1,ud2);
        else
            p += sprintf(p,"%s(%p:%p:%p)\n",id_table[*id-'0'],cb,ud1,ud2);
    }
    va_end(ap);

    if (p!=tmp)
        XtOverrideTranslations(w, XtParseTranslationTable(tmp));
}

/*======================================================================*/
