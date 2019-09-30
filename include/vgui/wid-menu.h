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
 *      $Id: wid-menu.h 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-menu.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets.                                   =*/
/*=                                                                    =*/
/*=   popup menu     : classic menu used in tmb meus                   =*/
/*=   drop down list : a label. The user can change its value through  =*/
/*=                    a popup menu that drops down when the label is  =*/
/*=                    clicked.                                        =*/
/*=   menu bar       : an horizontal bar with label. Each label        =*/
/*=                    controls a popup menu.                          =*/
/*=                                                                    =*/
/*= The module defines the following types:                            =*/
/*=   description       : type           : function prefix             =*/
/*=   menu  entry       : Twidm_entry    : widm_entry_...              =*/
/*=   menu  entry set   : Twidm_entrySet : widm_es_...                 =*/
/*=   any   menu        : Twidm_any      : widm_any_...                =*/
/*=   popup menu        : Twidm_popup    : widm_popup_...              =*/
/*=   drop down list    : Twidm_ddl      : widm_ddl...                 =*/
/*=   menu bar          : Twidm_bar      : widm_ bar_...               =*/
/*=                                                                    =*/
/*= These menus are based on an internal data structure: Twidm_any.    =*/
/*= This structure allows to set:                                      =*/
/*=   - a client callback (ma_CB or m_CB) that is called every time    =*/
/*=     a menu item is selected. Its prototype is:                     =*/
/*=        void cb(Twidm_any* m, Twidm_entry* e);                      =*/
/*=   - a client callback (me_CB) for each menu item that is called    =*/
/*=     every time the menu item is selected. Its prototype is:        =*/
/*=        void cb(Twidm_any* m, Twidm_entry* e);                      =*/
/*=     This callback is performed after the former.                   =*/
/*=   - a client user data (ma_UD or m_UD)                             =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_MENU_H
#define FILE_WID_MENU_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

typedef void (*Twidm_cb)(Twidm_any*m, Twidm_entry* me);

/*======================================================================*/
/*= The menu entry to create DDL table                                 =*/

typedef struct _Twidm_item Twidm_item;
typedef const   Twidm_item Cwidm_item;

struct _Twidm_item {
    // parameters that client must provide.
    char        me_target;      /* target gui, see GUI_ExtSys_... */
    char        me_kind;        /* kind of entry, see GUI_ME_... */
    char        me_checkable;   /* whether a check mark is put at the
                                 * left hand of the label */
    char        me_checked;     /* 1/0: checkmark is checked/unchecked */
    const char* me_label;       /* title of entry */
    TaccelDef   me_accelerator; /* accelerator keys. See GL_ACCEL_Xxx. */
    Twidm_cb    me_CB;          /* called when entry is selected */
    char**      me_smstrs;      /* table of submenu labels */
    Tany        me_ident;       /* entry identifier */
};

#define WIDM_ITEM_STR(   label,checkable,checked,cb)      { .me_target=GUI_ExtSys_ALL, .me_kind=GUI_ME_ItemStr, .me_checkable=checkable, .me_checked=checked, .me_label=label , .me_CB=cb }
#define WIDM_ITEM_STR_SM(label,checkable,checked,cb,strs) { .me_target=GUI_ExtSys_ALL, .me_kind=GUI_ME_ItemStr, .me_checkable=checkable, .me_checked=checked, .me_label=label , .me_CB=cb,\
                                                            .me_smstrs=strs }
#define WIDM_ITEM_STR_IV(label,checkable,checked,cb,iv)   { .me_target=GUI_ExtSys_ALL, .me_kind=GUI_ME_ItemStr, .me_checkable=checkable, .me_checked=checked, .me_label=label , .me_CB=cb,\
                                                            .me_ident.sint=iv }
#define WIDM_ITEM_SEP()                                   { .me_target=GUI_ExtSys_ALL, .me_kind=GUI_ME_ItemSep, .me_checkable=0        , .me_checked=0      , .me_label=0     , .me_CB=0  }

/*======================================================================*/
/*= The interface to GUI.                                              =*/

struct _Twidm_guiVt {
    void (*entry_setEnablement)(Cwidm_entry* me, int enabled);
    void (*entry_update       )(Cwidm_entry* me);
    void (*any_lastEntryAdd   )(Twidm_any*   self);
    void (*any_lastEntryDel   )(Twidm_any*   self);
    void (*ddl_setCurrentEntry)(Twidm_ddl*   self);
    void (*ddl_dbf            )(Twidm_ddl*   self,Tegs_widget parent,
                                CstrBrief menuRes, CstrBrief btnRes);

    Tegs_widget (*tmb_build   )(Twidm_bar* self,Tegs_widget parent);
    Tegs_widget (*tmb_sm_dbf  )(Twidm_bar*self,Twidm_popup*menu);
    void        (*tmb_finish  )(Twidm_bar* self);
};

/*======================================================================*/
/*= The menu entry                                                     =*/

#define GUI_ME_ItemSep  1    // menu item is a separator
#define GUI_ME_ItemStr  2    // menu item is a string (gme_label)
#define GUI_ME_ItemSubmenu 4 // gme_label is a null terminated array of

struct _Twidm_entry {
    // parameters that client must provide.
    char        me_target;      /* target gui, see GUI_ExtSys_... */
    char        me_kind;        /* kind of entry, see GUI_ME_... */
    char        me_checkable;   /* whether a check mark is put at the
                                 * left hand of the label */
    char        me_checked;     /* 1/0: checkmark is checked/unchecked */
    const char* me_label;       /* title of entry */
    TaccelDef   me_accelerator; /* accelerator keys. See GL_ACCEL_Xxx. */
    TpixmapEGS  me_pixmap;      /* pixmap (replace the label) */
    int         me_pixmapDx;    /* pixmap width  in pixel */
    int         me_pixmapDy;    /* pixmap height in pixel */
    Tany        me_ident;       /* ident the entry (user data) */
    void*       me_submenu;     /* the submenu that the entry must show
                                 * when it is selected. */
//    Twidm_cb    me_guiCB;       // callback gui must call
    Twidm_cb    me_appCB;       // client callback function
    void*       me_appCBud;     // client callback user data
    union {
        int     iud;
        double  dud;
        void*   vud;
        Ccolor* cud;
    } ud;
#define me_iud ud.iud
#define me_dud ud.dud
#define me_ud  ud.ud
#define me_cud ud.cud

    // parameters set by menu module
    Tegs_widget  me_widEGS; 
    union { /* the menu that handles this entry */
        Twidm_any*      menuAN; 
        Twidm_popup*    menuPP;
        Twidm_ddl*      menuDD;
#define me_menu         u.menuAN
#define me_menuPP       u.menuPP
#define me_menuDD       u.menuDD 
    } u;
    struct _Twidm_guiVt*me_guiVtable;
#define ME_guiVtable(m) (m)->me_guiVtable
    int    me_updating;
};

Inline int widm_entry_isCheckable(Cwidm_entry*me) { return me->me_checkable; }

/**
 * The widm_entry_setSelectable function makes the e entry either selectable
 * if state is one, or no selectable otherwise.
 * When the entry is no selectable, its associated callback is never
 * called.
 * The widm_entry_setChecked function unchecks the e entry if the state
 * parameter is 0, and checks it otherwise.
**/
extern void widm_entry_setSelectable(Cwidm_entry* e, int selectable);
extern void widm_entry_setChecked   (Twidm_entry* e, int state);
extern void widm_entry_update       (Cwidm_entry* e);

/*======================================================================*/
/*= Table of menu entries                                              =*/

typedef struct _Twidm_entrySet {
    int          eleMemNb; // element number of the memory block eles.
    int          eleNb;    // element number of set (eleNb<eleMemNb)
    Twidm_entry**eles;     // the set elements
} Twidm_entrySet;

typedef const Twidm_entrySet Cwidm_entrySet;

extern Twidm_entrySet* widm_es_new         (int n);
extern void            widm_es_free        (Twidm_entrySet* s);
extern void            widm_es_addClone    (Twidm_entrySet* s, Cwidm_entry* e);
extern void            widm_es_supLast     (Twidm_entrySet* s);

/*======================================================================*/
/*= base menu                                                          =*/

typedef struct _Twidm_any Twidm_any;
typedef const   Twidm_any Cwidm_any;

struct _Twidm_any {
    Tvgui*          ma_gui;
    Tegs_widget     ma_widEGS;
    Tegs_widget     ma_btnEGS; // the menu is attached to this widget
    void*           ma_vgData;  // vgui extra data
    TstrEaten       ma_help;    // help message (tip/balloon)
    Twidm_entrySet* ma_entries; // the menu entries
    Twidm_entry*    ma_pentry;  // parent entry for sub-menu.
    Twidm_cb        ma_guiCB;   // callback gui must call
    Twidm_cb        ma_appCB;   // client callback function
    void*           ma_appCBud; // client callback user data
    void*           ma_udEGS; 

    struct _Twidm_guiVt*ma_guiVtable;
#define MA_guiVtable(m) (m)->ma_guiVtable
} ;
#define m_gui        any.ma_gui
#define m_widEGS     any.ma_widEGS
#define m_btnEGS     any.ma_btnEGS
#define m_vgData     any.ma_vgData
#define m_help       any.ma_help
#define m_entries    any.ma_entries
#define m_guiCB      any.ma_guiCB
#define m_appCB      any.ma_appCB
#define m_appCBud    any.ma_appCBud
#define M_guiVtable(m) MA_guiVtable(&m->any)

extern Twidm_any* widm_any_newByStrT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               Tstr* es, Twidm_cb cb);
extern Twidm_any* widm_any_newByPxmT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               TpixmapEGS* es, int en,
                               int dx, int dy, Twidm_cb cb);
extern Twidm_any* widm_any_newByItemT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               Cwidm_item* es, int en);
extern void       widm_any_delete(Twidm_any*self);

extern void       widm_any_addEntryEaten    (Twidm_any*self, Twidm_entry*me);
extern void       widm_any_addEntryByStrCbIv(Twidm_any*self, Tstr str, Twidm_cb cb, int iv);
extern void       widm_any_supEntryLast     (Twidm_any*self);

extern void       widm_any_setEntrySelectable(
                                Twidm_any*self, int en, int selectable);

/*======================================================================*/
/*= popup menu                                                         =*/

typedef struct _Twidm_popup Twidm_popup;
typedef const   Twidm_popup Cwidm_popup;

struct _Twidm_popup {
    Twidm_any      any;
    TstrEaten      m_label; // the label for m_btnEGS 
} ;

extern Twidm_popup* widm_popup_newByItemT(
                               Tvgui* gui, void*vgdt, CstrBrief help,
                               int n, Cwidm_item* es, int en,
                               CstrBrief label);
extern void    widm_popup_free  (Twidm_popup*menu);

/*======================================================================*/
/*= drop down list                                                     =*/

struct _Twidm_ddl {
    Twidm_any  any;
    int        m_selectedEntry; // the # th entry of vgm_entries
} ;

extern Twidm_ddl* widm_ddl_newStr(
                            Tvgui* gui, void*vgdt, CstrBrief help,
                            Tstr* es, Twidm_cb cb, void *ud );
extern Twidm_ddl* widm_ddl_newPxm(
                            Tvgui* gui, void*vgdt, CstrBrief help,
                            TpixmapEGS* es,  int  en, int pdx, int pdy,
                            Twidm_cb cb, void*ud );
extern Twidm_ddl* widm_ddl_newByItemT(
                            Tvgui* gui, void*vgdt, CstrBrief help,
                            Twidm_item* es, int en,
                            Twidm_cb cb, void *ud);
extern void widm_ddl_delete(Twidm_ddl* self);

extern void widmM_ddl_dbf(Twidm_ddl* self, Tegs_widget parent);
extern void widmD_ddl_dbf(Twidm_ddl* self, Tegs_widget parent);

extern void widm_ddl_setEntryHelp (Twidm_ddl* self, int index, TstrStatic help); // FIXME: not working
extern void widm_ddl_setEntryIdentAny(Twidm_ddl* self, int index, Tany id);
Inline void widm_ddl_setEntryIdentInt(Twidm_ddl* self, int index, int  id) { widm_ddl_setEntryIdentAny(self,index,any_initBySInt(id)); }

extern void widm_ddl_show   (Twidm_ddl* self);
extern void widm_ddl_unshow (Twidm_ddl* self);
extern void widm_ddl_enable (Twidm_ddl* self);
extern void widm_ddl_disable(Twidm_ddl* self);

extern int  widm_ddl_getCurrentEntryIndex(Twidm_ddl* self);
extern void widm_ddl_setCurrentEntryIndex(Twidm_ddl* self, int index);
extern Tany widm_ddl_getCurrentEntryIdent(Twidm_ddl* self);
extern void widm_ddl_setCurrentEntryIdent(Twidm_ddl* self, Tany ident);

/*======================================================================*/
/*= menu bar                                                           =*/

#define WIDM_BarMaxItems 12

struct _Twidm_bar {
    Tvgui*       mb_gui;
    Tegs_widget  mb_widEGS;
    CstrStatic   mb_menuRes;
    CstrStatic   mb_btnRes;
    Twidm_popup* mb_menus[WIDM_BarMaxItems];
    int          mb_menuNb;
    struct _Twidm_guiVt*mb_guiVtable;
#define MB_guiVtable(m) (m)->mb_guiVtable
};

extern Twidm_bar* widmM_tmb_nbx   (Tvgui_mw*vmw);
extern Twidm_bar* widmD_tmb_nbx   (Tvgui_dw*vdw);
extern void       widm_tmb_delete (Twidm_bar* self);
extern void       widm_tmb_addMenuByItemT(
                           Twidm_bar* self, CstrBrief help,
                           Cwidm_item* es, int en,
                           const char* label, Twidm_cb cb, void*ud);
extern void       widm_tmb_finish(Twidm_bar* self);

extern void   widm_tmb_enable(Twidm_bar* self);
extern void   widm_tmb_disable(Twidm_bar* self);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_MENU_H
/*======================================================================*/
