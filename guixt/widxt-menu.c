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
 *      $Id: widxt-menu.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-menu.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets (EGS driver).                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-menu.h header for documentation.                  =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-accelkey.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-menu.h"

/*======================================================================*/
/*= Private utilities                                                  =*/

// Return the number of elements added to args.
// label==0 accel_byAmpersand is not called
// label!=0 accel_byAmpersand is called and result string is put in label.
// accel may be NULL if not used.
static int guim_entry_argsOfStrItem(Cwidm_entry* me, Arg*args, char*label, TaccelDef* accel)
{
    int argCount=0;
    int leftMargin = 0;
    int vertSpace  = 10;
    TEMP_asf_printf((me->me_target&GUI_ExtSys_XT)==0,"%s","invalid call (not a XT entry)");
  //TEMP_asf_printf( me->me_kind != GUI_ME_ItemStr,  "%s","invalid call (not a string entry)");

    if ( me->me_label ) {
        if ( label==0 ) {
            XtSetArg(args[argCount],XtNlabel,me->me_label); argCount+=1;
        } else {
            int underlineIndex = accel_byAmpersand(label,me->me_label,accel,COPY_IN_AMPERSAND);
            XtSetArg(args[argCount],XtNlabel,label); argCount+=1;
#if USE_XAW3D
            if ( underlineIndex>=0 ) { /* any underline */
                XtSetArg(args[argCount],XtNunderline, underlineIndex); argCount+=1;
            }
#endif
        }
    } else {
        XtSetArg(args[argCount],XtNlabel,""); argCount+=1;
    }

    if (me->me_checkable) {
        Tvgui* gui = me->me_menu->ma_gui;
        leftMargin = 12;
        if ( me->me_checked ) {
            XtSetArg(args[argCount],XtNleftBitmap, gui->g_main->m_bmCmBoxOnSm); argCount+=1;
        } else {
            XtSetArg(args[argCount],XtNleftBitmap, gui->g_main->m_bmCmBoxOffSm); argCount+=1;
        }
    } else if ( ! PXMEGS_IsNone(me->me_pixmap) ) {
          leftMargin = me->me_pixmapDx;
          vertSpace  = -10;
          XtSetArg(args[argCount],XtNleftBitmap, PXMEGS2X11(me->me_pixmap)); argCount+=1;
    }

    if (leftMargin) { XtSetArg(args[argCount],XtNleftMargin, leftMargin); argCount+=1; }
    if (vertSpace ) { XtSetArg(args[argCount],XtNvertSpace,  vertSpace ); argCount+=1; }
    return argCount;
}

int btns_areReleased(Tvgui*gui, int*x, int*y)
{
    Window win, root_return, child_return;
    int root_x_return, root_y_return, win_x_return, win_y_return;
    int mask,mask_return;
    win = DefaultRootWindow(E_Display);
    XQueryPointer(E_Display,win,
            &root_return, &child_return,
            &root_x_return, &root_y_return, &win_x_return, &win_y_return,
            &mask_return);

    *x = root_x_return;
    *y = root_y_return;
    mask  = Button1Mask; 
    mask |= Button2Mask; 
    mask |= Button3Mask; 
    mask |= Button4Mask; 
    mask |= Button5Mask; 
    if ( (mask_return&mask)!=0 ) 
        return 0;
    else
        return 1;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
#define menupopup_entry_callback    guim_entry_cb

static void guim_entry_cb(struct _WidgetRec *w,
        void *app_data, void *specific_data)
{
    Twidm_entry* me       = app_data;
    Twidm_any*   menu     = me->me_menu;
    Cwidm_any*   submenu  = me->me_submenu;
    Tvgui*       gui      = menu->ma_gui;
    Widget* submenuPopped = (Widget*)&menu->ma_udEGS;
    int cx,cy,tx,ty,tdx,tdy,tbdw,tdepth;
    Window root;

    if ( btns_areReleased(gui,&cx,&cy) ) {
        if ( me->me_kind==GUI_ME_ItemStr ) {
            TEMP_asf_printf(*submenuPopped!=0,"%s","unexpectd submenuPopped!=0");
            if (me->me_checkable) {
                Tvgui* gui = menu->ma_gui;
                if ( !me->me_checked ) 
                    XtVaSetValues(me->me_widEGS,XtNleftBitmap, gui->g_main->m_bmCmBoxOnSm,NULL);
                else
                    XtVaSetValues(me->me_widEGS,XtNleftBitmap, gui->g_main->m_bmCmBoxOffSm,NULL);
            }
            if ( menu->ma_guiCB )
                menu->ma_guiCB(menu, me);
        } else if ( me->me_kind==GUI_ME_ItemSubmenu ) {
            if (*submenuPopped!=0 )
                XtPopdown( *submenuPopped );
            *submenuPopped = 0;
        }
    } else if ( me->me_kind==GUI_ME_ItemSubmenu ) {
        Widget submenuEGS = submenu->ma_widEGS;
        if ( ! gui_widegs_isShown(submenuEGS) )  { 
            if (*submenuPopped!=0 && *submenuPopped!=submenuEGS )
                XtPopdown( *submenuPopped );
            *submenuPopped = submenuEGS;
            XGetGeometry(E_Display,XtWindow((Widget)menu->ma_widEGS),&root,&tx,&ty,&tdx,&tdy,&tbdw,&tdepth);
            XtPopup(submenuEGS,XtGrabNonexclusive);
            XtMoveWidget(submenuEGS,tx+tdx,cy-10);
        }
    } else {
        if (*submenuPopped!=0 )
            XtPopdown( *submenuPopped );
        *submenuPopped = 0;
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void gui_menu_createBySet(Tvgui* gui, Twidm_entrySet* s,
        Tegs_widget menu, XtCallbackProc callback)
{
    XT_DeclareArgs (20);
    int i, hasSubmenu = 0;
    for (i = 0; i<s->eleNb ; i++) {
        Twidm_entry* entry = s->eles[i];
        if ( (entry->me_target&GUI_ExtSys_XT)==0 )
            continue;
        if ( entry->me_kind == GUI_ME_ItemSep ) {
          /* put in a separator line */
          XT_FirstArg(XtNlineWidth, 2);
          entry->me_widEGS = XtCreateManagedWidget("line",
                  smeLineObjectClass, menu, Args, ArgCount);
        } else if ( entry->me_kind == GUI_ME_ItemStr ) {
            char label[1000];
            ArgCount = guim_entry_argsOfStrItem(entry,Args,label,NULL);
            entry->me_widEGS = XtCreateManagedWidget("menuItem",
                    smeBSBObjectClass, menu, Args, ArgCount);
            XtAddCallback(entry->me_widEGS, XtNcallback, callback, (XtPointer) entry);
        } else if ( entry->me_kind == GUI_ME_ItemSubmenu ) {
            hasSubmenu = 1;
            ArgCount = guim_entry_argsOfStrItem(entry,Args,NULL,NULL);
            entry->me_widEGS = XtCreateManagedWidget("menuItem", smeBSBObjectClass, menu, Args, ArgCount);
            XtAddCallback(entry->me_widEGS, XtNcallback,callback,(XtPointer) entry);
    
            Twidm_any* submenu = entry->me_submenu;
            TcolorEGS fg,bg; vgui_wn_getColorEGS(gui,wn_mainMenu,&fg,&bg);
            submenu->ma_widEGS = XtVaCreatePopupShell(
                entry->me_label,
                simpleMenuWidgetClass, gui->g_main->m_topWidget,
                XtNallowShellResize, (Boolean)True,
                XtNforeground, fg,
                XtNbackground, bg,
                NULL
            );
            gui_menu_createBySet(gui,submenu->ma_entries,submenu->ma_widEGS,callback);
        } else {
            TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",entry->me_kind);
        }
    }
    if ( hasSubmenu ) {
          char* tmp="<EnterWindow>:\n"
                  //"<LeaveWindow>:MenuPopdown() unhighlight()\n"
                    "<LeaveWindow>:unhighlight()\n"
                    "<BtnMotion>:highlight() notify()\n"
                    "<BtnUp>:MenuPopdown() notify() unhighlight()\n"
                    ;
            XtUninstallTranslations(menu);
            XtOverrideTranslations (menu, XtParseTranslationTable(tmp) );
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static Tegs_widget guiXm_any_build(Twidm_any*menu, Tegs_widget parent, 
        CstrBrief label, CstrBrief menuRes, CstrBrief btnRes) 
{
    char *menuname = "DDL";
    Tvgui* gui = menu->ma_gui;
#define BUTTON_BorderWidth 1
#define BUTTON_Width       0 //60  // no effect FIXME
#define BUTTON_Height      0 //22  // no effect

    menu->ma_btnEGS = create_menuButton( //widget_menuButton_create(
        gui,parent,btnRes,
        BUTTON_BorderWidth, BUTTON_Width, BUTTON_Height,
        EGSCOLOR_default, EGSCOLOR_default,
        label, menuname);

    /* now the menu container */
    TcolorEGS fg,bg; vgui_wn_getColorEGS(gui,menuRes,&fg,&bg);
    menu->ma_widEGS = XtVaCreatePopupShell(
      menuname, // Caution ma_btnEGS finds the menu using this name.
                // So we must set this string to the XtNmenuName argument.
      simpleMenuWidgetClass, menu->ma_btnEGS,
      XtNallowShellResize, (Boolean)True,
      XtNforeground, fg,
      XtNbackground, bg,
      NULL
    );

    /* add its entries to the menu */
    gui_menu_createBySet(gui, menu->ma_entries,
        menu->ma_widEGS,menupopup_entry_callback);

    return menu->ma_btnEGS;
}

/*======================================================================*/
/*= Interface: VGUI --> GUI Interface                                  =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Entry widget                                                       =*/

static void guixtm_vt_entry_setEnablement(Cwidm_entry*me, int enabled)
{ Tegs_widget w=me->me_widEGS;
  if (enabled) gui_widegs_enable(w); else gui_widegs_disable(w); }

static void guixtm_vt_entry_update(Cwidm_entry* me)
{
    char label[1000];
    Arg args[20]; int argCount;
    if ( (me->me_target&GUI_ExtSys_XT)==0 ) return;
    if ( me->me_kind == GUI_ME_ItemStr ) {
        argCount = guim_entry_argsOfStrItem(me,args,label,NULL);
        XtSetValues(me->me_widEGS,args,argCount);
    } else if ( me->me_kind == GUI_ME_ItemSubmenu ) {
        argCount = guim_entry_argsOfStrItem(me,args,label,NULL);
        XtSetValues(me->me_widEGS,args,argCount);
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }
}

static void guixtm_vt_lastEntryAdd(Twidm_any*self)
{
    char label[1000];
    Arg args[20]; int argCount;
    Tegs_widget menu     = self->ma_widEGS;
    void*       callback = menupopup_entry_callback;
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    if ( (me->me_target&GUI_ExtSys_XT)==0 )
        return;
    if ( me->me_kind == GUI_ME_ItemSep ) {
        /* put in a separator line */
        argCount = 0;
        XtSetArg(args[argCount],XtNlineWidth, 2); argCount+=1;
        me->me_widEGS = XtCreateManagedWidget("line",
                smeLineObjectClass, menu, args, argCount);
    } else if ( me->me_kind == GUI_ME_ItemStr ) {
        char label[1000];
        argCount = guim_entry_argsOfStrItem(me,args,label,NULL);
        me->me_widEGS = XtCreateManagedWidget("menuItem",
                smeBSBObjectClass, menu, args, argCount);
        XtAddCallback(me->me_widEGS, XtNcallback, callback, (XtPointer) me);
    } else if ( me->me_kind == GUI_ME_ItemSubmenu ) {
        TEMP_spe_printf("Adding submenu item is not yet implemented (kind=GUI_ME_ItemSubmenu=%d)",me->me_kind);
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }
}

static void guixtm_vt_lastEntryDel(Twidm_any*self)
{
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    XtDestroyWidget(me->me_widEGS);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= DDL widget                                                         =*/

static void guixtm_vt_ddl_dbf(Twidm_ddl* menu,   Tegs_widget parent,
                              CstrBrief menuRes, CstrBrief btnRes) 
{
    const char* label   = menu->m_entries->eles[0]->me_label;
    guiXm_any_build(&menu->any,parent, label, menuRes, btnRes);

    // set width of button to the larger width of DDL items.
    XtRealizeWidget(menu->m_widEGS);
    int i;
    Dimension dx,dy,maxdx=0,maxdy=0;
    for (i = 0; i<menu->m_entries->eleNb ; i++) {
        Twidm_entry* entry = menu->m_entries->eles[i];
        if ( (entry->me_target&GUI_ExtSys_XT)==0 ) continue;
        if ( entry->me_kind == GUI_ME_ItemSep )    continue;
        if ( entry->me_kind == GUI_ME_ItemStr ) {
            XtVaGetValues(entry->me_widEGS, XtNwidth,&dx, XtNheight,&dy, NULL);
            if ( dx>maxdx ) maxdx = dx;
            if ( dy>maxdy ) maxdy = dy;
        }
    }
#if 1
    XtVaSetValues(  menu->m_btnEGS, XtNwidth,maxdx+15, XtNheight,maxdy, NULL);
#else
printf("%s:  maxdx/dy=%d/%d\n",__func__,maxdx,maxdy);
Dimension orgdx=0,orgdy=0;
XtVaGetValues(  menu->m_btnEGS, XtNwidth,&orgdx, XtNheight,&orgdy, NULL);
printf("%s:  orgdx/dy=%d/%d\n",__func__,orgdx,orgdy);
    XtVaSetValues(  menu->m_btnEGS, XtNwidth,maxdx+15, XtNheight,maxdy, NULL);
XtVaGetValues(  menu->m_btnEGS, XtNwidth,&orgdx, XtNheight,&orgdy, NULL);
printf("%s:  orgdx/dy=%d/%d\n",__func__,orgdx,orgdy);
#endif
}

static void guixtm_vt_ddl_setCurrentEntry(Twidm_ddl* menu)
{
    Tegs_widget self  = menu->m_btnEGS;
    if ( self==0 ) return; 

    Tvgui* gui = menu->m_gui;
    Twidm_entry*e = menu->m_entries->eles[menu->m_selectedEntry];
    if ( e->me_label!=0 )
        widget_setLabel(self,e->me_label);
    if ( ! PXMEGS_IsNone(e->me_pixmap) ) {
        Window xw = _XtWindow(self);
        if (xw!=0) {
            GC gc = E_GCcache(OD_GC_SysDraw);
            XCopyArea(E_Display,PXMEGS2X11(e->me_pixmap),xw,gc,
                0,0,e->me_pixmapDx,e->me_pixmapDx, 20,0);
        } else {
            widget_cb_addU(self,menu,
                YFIG_EVENT_Expose, guixtm_vt_ddl_setCurrentEntry,
                NULL);
        }
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Menu bar                                                           =*/

static Tegs_widget guixtm_vt_tmb_build(Twidm_bar*tmb, Tegs_widget parent)
{
    return create_form(tmb->mb_gui,parent,tmb->mb_menuRes,
        0, 1,-1,-1, EGSCOLOR_default, EGSCOLOR_default);
}

static Tegs_widget guixtm_vt_tmb_sm_dbf(Twidm_bar*tmb, Twidm_popup*menu)
{
    return guiXm_any_build(&menu->any,tmb->mb_widEGS,menu->m_label,tmb->mb_menuRes,tmb->mb_btnRes);
}

static void guixtm_vt_tmb_finish(Twidm_bar* self)
{
  int i;
  for (i=0 ; i<self->mb_menuNb ; i+=1) {
    Twidm_popup* menu = self->mb_menus[i];
    if ( i!=0 ) {
      widget_place_widget(menu->m_btnEGS,
        0,0,0,0,
        5,self->mb_menus[i-1]->m_btnEGS,
        0,0
     );
    } else {
      widget_place_widget(menu->m_btnEGS,
        0,0,0,0,
        5,0,
        0,0
     );
    }
  }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= The GUI vtable.                                                    =*/

struct _Twidm_guiVt guixt_VtMenu = {
    .entry_setEnablement = guixtm_vt_entry_setEnablement,
    .entry_update        = guixtm_vt_entry_update,
    .any_lastEntryAdd    = guixtm_vt_lastEntryAdd,
    .any_lastEntryDel    = guixtm_vt_lastEntryDel,
    .ddl_setCurrentEntry = guixtm_vt_ddl_setCurrentEntry,
    .ddl_dbf             = guixtm_vt_ddl_dbf,
    .tmb_build           = guixtm_vt_tmb_build,
    .tmb_sm_dbf          = guixtm_vt_tmb_sm_dbf,
    .tmb_finish          = guixtm_vt_tmb_finish,
};

/*======================================================================*/
