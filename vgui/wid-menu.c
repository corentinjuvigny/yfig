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
 *      $Id: wid-menu.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-menu.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets.                                   =*/
/*=                                                                    =*/
/*= See the vgui/vgui-menu.h header for the type description,          =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-menu.h"
#include "vgui/vgui-names.h"

/*======================================================================*/

#define ME_SetUpdating(me)  (me)->me_updating+=1
#define ME_ClrUpdating(me)  (me)->me_updating-=1
#define ME_IsUpdating(me)   (me)->me_updating 

/*======================================================================*/

//static void widm_entry_guiCB(Twidm_any*m, Twidm_entry* me);

static Twidm_entry  widm_entry_init     (int target, int kind)
{
    Twidm_entry e;
    memset(&e,0,sizeof(e));
    e.me_target     = target;
    e.me_kind       = kind;
    e.me_pixmap     = PXMEGS_none;
    return e;
}

static Twidm_entry  widm_entry_initByItem(Cwidm_item*e)
{
    Twidm_entry self=widm_entry_init(e->me_target,e->me_kind);
    self.me_checkable   = e->me_checkable;
    self.me_checked     = e->me_checked;
    self.me_label       = e->me_label;
    self.me_accelerator = e->me_accelerator;
    self.me_ident       = e->me_ident;
    self.me_appCB       = e->me_CB;
    return self;
}

Inline Twidm_entry* widm_entry_new       (int target, int kind)   { Twidm_entry* self=(Twidm_entry*)tool_malloc(sizeof(*self)); *self=widm_entry_init(target,kind); return self; }
Inline Twidm_entry* widm_entry_newClone  (Cwidm_entry*e)          { Twidm_entry* self=(Twidm_entry*)tool_malloc(sizeof(*self)); *self=*e; return self; }
Inline void         widm_entry_delete    (Twidm_entry*e)          { tool_free(e); }

extern void widm_entry_setSelectable(Cwidm_entry* me, int selectable) { me->me_guiVtable->entry_setEnablement(me,selectable); }

extern void widm_entry_setChecked(Twidm_entry* me, int state)
{
    ME_SetUpdating(me);
    TEMP_asf_printf( ! widm_entry_isCheckable(me),"%s","invalid call (entry is uncheckable)");
    me->me_checked = state ? 1 : 0;
    ME_guiVtable(me)->entry_update(me);
    ME_ClrUpdating(me);
}

extern void widm_entry_update (Cwidm_entry* me) { ME_guiVtable(me)->entry_update(me); }

/*======================================================================*/

extern Twidm_entrySet* widm_es_new  (int n)
{
    Twidm_entrySet* ret = tool_malloc(sizeof(*ret));
    ret->eleMemNb  = n>=0 ? n : 0; 
    ret->eleNb     = 0;
    ret->eles      = n>0  ? tool_malloc(sizeof(*ret->eles)*n) : 0;
    return ret;
}

extern void  widm_es_delete (Twidm_entrySet* s)
{
    int i;
    for (i=0 ; i<s->eleNb ; i+=1) widm_entry_delete( s->eles[i] );
    tool_free( s->eles );
    free     ( s );
}

extern void  widm_es_addClone(Twidm_entrySet* s, Cwidm_entry* e)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(s->eles[0])*s->eleMemNb);
    }
    s->eles[s->eleNb] = widm_entry_newClone(e);
    s->eleNb += 1;
}

extern void  widm_es_addEaten(Twidm_entrySet* s, Twidm_entry* me)
{
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(s->eles[0])*s->eleMemNb);
    }
    s->eles[s->eleNb] = me;
    s->eleNb += 1;
}

extern Twidm_entry* widm_es_addByStrCB (Twidm_entrySet*s, char* str, Twidm_cb cb)
{
    Twidm_entry* me;
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(s->eles[0])*s->eleMemNb);
    }
    me = s->eles[s->eleNb] = widm_entry_new(GUI_ExtSys_ALL,GUI_ME_ItemStr);
    me->me_label = str;
    me->me_appCB = cb;
    s->eleNb += 1;
    return me;
}

extern Twidm_entry* widm_es_addByPxmCB (Twidm_entrySet*s, TpixmapEGS pxm, int dx, int dy, Twidm_cb cb)
{
    Twidm_entry* me;
    if ( s->eleNb == s->eleMemNb ) {
        s->eleMemNb += 4;
        s->eles =realloc(s->eles,sizeof(s->eles[0])*s->eleMemNb);
    }
    me = s->eles[s->eleNb] = widm_entry_new(GUI_ExtSys_ALL,GUI_ME_ItemStr);
    me->me_pixmap   = pxm;
    me->me_pixmapDx = dx;
    me->me_pixmapDy = dy;
    me->me_appCB = cb;
    s->eleNb += 1;
    return me;
}

extern void widm_es_supLast(Twidm_entrySet* s) { s->eleNb -= 1; widm_entry_delete(s->eles[s->eleNb]); }

/*======================================================================*/

static void widm_any_cb(Twidm_any*m, Twidm_entry*me)
{
    if ( ME_IsUpdating(me) ) return;
    // toggle me_checked for checkable entry
    if ( widm_entry_isCheckable(me)!=0 ) me->me_checked = me->me_checked==0;
    // call user callbacks
    if ( m->ma_appCB ) m->ma_appCB(m,me);
    if ( me->me_appCB ) me->me_appCB(m,me);
}

static Twidm_any* widm_any_new(int bs, Tvgui* gui, void*vgdt, CstrBrief help, int n)
{
    Twidm_any* submenu;
    Twidm_any* self     = tool_malloc_0( bs );
    self->ma_gui        = gui;
    self->ma_vgData     = vgdt;
    self->ma_entries    = widm_es_new( n );;
    self->ma_help       = tool_strdup(help);
    self->ma_guiCB      = widm_any_cb;
    self->ma_guiVtable  = gui->vg_guiVtMenu;
    return self;
}

extern Twidm_any* widm_any_newByStrT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               Tstr* es, Twidm_cb cb)
{
    char** p;
    Twidm_any* self = widm_any_new(bs,gui,vgdt,help,3);
    for ( p=es; *p!=0 ; p+=1) {
        Twidm_entry* me = widm_es_addByStrCB(self->ma_entries,*p,cb);
        me->me_ident = any_initBySInt(self->ma_entries->eleNb-1);
        me->me_menu  = self;
    }
    return self;
}

extern Twidm_any* widm_any_newByPxmT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               TpixmapEGS* es, int en,
                               int dx, int dy, Twidm_cb cb)
{
    int i;
    Twidm_any* self = widm_any_new(bs,gui,vgdt,help,en);
    for ( i=0 ; i<en ; i+=1) {
        Twidm_entry* me = widm_es_addByPxmCB(self->ma_entries,es[i],dx,dy,cb);
        me->me_ident = any_initBySInt(self->ma_entries->eleNb-1);
    }
    return self;
}

extern Twidm_any* widm_any_newByItemT(int bs, Tvgui* gui,
                               void*vgdt, CstrBrief help,
                               Cwidm_item* es, int en)
{
    Twidm_any* submenu;
    Twidm_any* self     = widm_any_new(bs,gui,vgdt,help,en);

    // update entries
    int i;
    for (i=0 ; i<en ; i+=1) {
        Cwidm_item*  it = es+i;
        Twidm_entry  e  = widm_entry_initByItem(it);
        widm_es_addClone( self->ma_entries, &e);
        Twidm_entry* me  = self->ma_entries->eles[i];
        me->me_menu      = self;
        me->me_guiVtable = gui->vg_guiVtMenu;
        if ( it->me_smstrs!=0 ) {
            submenu = widm_any_newByStrT(sizeof(*submenu),gui,vgdt,NULL,it->me_smstrs,me->me_appCB);
            me->me_kind=GUI_ME_ItemSubmenu;
            me->me_submenu = submenu;
            submenu->ma_pentry = me;
        }
    }
    return self;
}

extern void       widm_any_delete(Twidm_any*self)
{
    if ( self==NULL ) return;
    widm_es_delete( self->ma_entries );
    tool_free     ( self->ma_help );
    free          ( self );
}

extern void       widm_any_addEntryEaten(Twidm_any*self, Twidm_entry*me)
{
    widm_es_addEaten(self->ma_entries,me);
    me->me_menu      = self;
    me->me_guiVtable = MA_guiVtable(self);
    MA_guiVtable(self)->any_lastEntryAdd(self);
}

extern void       widm_any_addEntryByStrCbIv(Twidm_any*self, Tstr str, Twidm_cb cb, int iv)
{
    Twidm_entry* me  = widm_entry_new(GUI_ExtSys_ALL,GUI_ME_ItemStr);
    me->me_label     = str;
    me->me_appCB     = cb;
    me->me_ident     = any_initBySInt(iv);
    widm_any_addEntryEaten(self,me);
}

extern void       widm_any_supEntryLast(Twidm_any*self)
{
    MA_guiVtable(self)->any_lastEntryDel(self);
    widm_es_supLast(self->ma_entries);
}

extern void widm_any_setEntrySelectable(Twidm_any*self, int en, int selectable)
{
    Twidm_entry* me = self->ma_entries->eles[en];
    widm_entry_setSelectable(me,selectable);
}

/*======================================================================*/

extern Twidm_popup* widm_popup_newByItemT(Tvgui* gui,
           void*vgdt, CstrBrief help,
           int n, Cwidm_item* es, int en, CstrBrief label)
{
    Twidm_popup* self = (Twidm_popup*) widm_any_newByItemT(
                                sizeof(*self), gui, vgdt,help, es,en);
    self->m_label = tool_strdup(label);
    return self;
}

extern void    widm_popup_free  (Twidm_popup*self)
{
    tool_free    ( self->m_label );
    widm_any_delete( &self->any );
}

/*======================================================================*/

static void widm_ddl_cb(Twidm_any*m, Twidm_entry*e)
{
    Twidm_ddl* self = (Twidm_ddl*)m;
    for (self->m_selectedEntry=0 ; ; self->m_selectedEntry+=1) {
      if ( self->m_entries->eles[self->m_selectedEntry] == e ) break;
    }
    M_guiVtable(self)->ddl_setCurrentEntry(self);
    widm_any_cb(m,e);
}

extern Twidm_ddl*
            widm_ddl_newStr(Tvgui* gui, void*vgdt, CstrBrief help,
                            Tstr* es, Twidm_cb cb, void *ud )
{
    Twidm_ddl* self = (Twidm_ddl*) widm_any_newByStrT( sizeof(*self),
                            gui, vgdt, help, es, NULL);
    self->m_appCB   = cb;
    self->m_appCBud = ud;
    return self;
}

extern Twidm_ddl*
            widm_ddl_newPxm(Tvgui* gui, void*vgdt, CstrBrief help,
                            TpixmapEGS* es,  int  en, int dx, int dy,
                            Twidm_cb cb, void*ud )
{
    Twidm_ddl* self = (Twidm_ddl*) widm_any_newByPxmT( sizeof(*self),
                            gui, vgdt, help, es, en, dx,dy, NULL);
    self->m_appCB   = cb;
    self->m_appCBud = ud;
    return self;
}

extern Twidm_ddl* widm_ddl_newByItemT(
                            Tvgui* gui, void*vgdt, CstrBrief help,
                            Twidm_item* es, int en,
                            Twidm_cb cb, void *ud)
{
    Twidm_ddl* self = (Twidm_ddl*) widm_any_newByItemT( sizeof(*self),
                                        gui, vgdt, help, es, en);
    self->m_appCB   = cb;
    self->m_appCBud = ud;
    return self;
}

extern void widm_ddl_delete(Twidm_ddl* self) { widm_any_delete( &self->any ); }
extern void widmM_ddl_dbf  (Twidm_ddl* self, Tegs_widget parent) { M_guiVtable(self)->ddl_dbf(self,parent,wn_mainMenu,wn_mainMenuBtn); }
extern void widmD_ddl_dbf  (Twidm_ddl* self, Tegs_widget parent) { M_guiVtable(self)->ddl_dbf(self,parent,wn_dialMenu,wn_dialMenuBtn); }

extern void widm_ddl_setEntryIdentAny(Twidm_ddl* self, int index, Tany id)
{
    Twidm_entry* me = self->m_entries->eles[index];
    me->me_ident    = id;
}

// FIXME: not working
extern void widm_ddl_setEntryHelp (Twidm_ddl* self, int index, TstrStatic help)
{
    Twidm_entry* me = self->m_entries->eles[index];
    if ( me->me_widEGS==NULL ) return;
    gui_widegs_addTip( me->me_widEGS,help);
}

extern void widm_ddl_show   (Twidm_ddl* self) { gui_widegs_show   ( self->any.ma_btnEGS ); }
extern void widm_ddl_unshow (Twidm_ddl* self) { gui_widegs_unshow ( self->any.ma_btnEGS ); }
extern void widm_ddl_enable (Twidm_ddl* self) { gui_widegs_enable ( self->any.ma_btnEGS ); }
extern void widm_ddl_disable(Twidm_ddl* self) { gui_widegs_disable( self->any.ma_btnEGS ); }

extern int  widm_ddl_getCurrentEntryIndex(Twidm_ddl* self) { return self->m_selectedEntry; }
extern void widm_ddl_setCurrentEntryIndex(Twidm_ddl* self, int index)
{
    if ( index<0 || self->m_entries->eleNb<=index ) return;
    self->m_selectedEntry=index;
    M_guiVtable(self)->ddl_setCurrentEntry(self);
}

extern Tany widm_ddl_getCurrentEntryIdent(Twidm_ddl* self) {
    Twidm_entry* me = self->m_entries->eles[ widm_ddl_getCurrentEntryIndex(self) ];
    return me->me_ident;
}
extern void widm_ddl_setCurrentEntryIdent(Twidm_ddl* self, Tany ident)
{
    int index;
    for (index=0 ; index<self->m_entries->eleNb ; index+=1) {
        Twidm_entry* me = self->m_entries->eles[index];
        if ( any_cmp(ident,me->me_ident)==0 ) {
            widm_ddl_setCurrentEntryIndex(self,index);
            return;
}   }   }

/*======================================================================*/

extern Twidm_bar* widmM_tmb_nbx   (Tvgui_mw*vmw)
{
    Twidm_bar* self    = tool_malloc_0(sizeof(*self));
    self->mb_gui       = vgui_mw_getVGui(vmw);
    self->mb_menuRes   = wn_mainMenu;
    self->mb_btnRes    = wn_mainMenuBtn;
    self->mb_guiVtable = self->mb_gui->vg_guiVtMenu;
    self->mb_widEGS    = MB_guiVtable(self)->tmb_build(self,vgui_mw_getWidEGS(vmw));
    return self;
}
extern Twidm_bar* widmD_tmb_nbx   (Tvgui_dw*vdw);
extern void       widm_tmb_delete (Twidm_bar* self)
{
  int i;
  for (i=0 ; i<self->mb_menuNb ; i+=1) {
    Twidm_popup* menu = self->mb_menus[i];
    widm_popup_free( menu );
  }
  free( self );
}

extern void       widm_tmb_addMenuByItemT(
                           Twidm_bar* self, CstrBrief help,
                           Cwidm_item* es, int en,
                           const char* label, Twidm_cb cb, void*ud)
{
  Tvgui*       gui  = self->mb_gui;
  Twidm_popup* menu = widm_popup_newByItemT(gui, 0, help, 0,es,en, label);
  menu->m_appCB     = cb;
  menu->m_appCBud   = ud;
  self->mb_menus[self->mb_menuNb] = menu;
  self->mb_menuNb  += 1;
}

extern void       widm_tmb_finish(Twidm_bar* self)
{
  int i;
  for (i=0 ; i<self->mb_menuNb ; i+=1) {
    Twidm_popup* menu = self->mb_menus[i];
    menu->m_btnEGS = MB_guiVtable(self)->tmb_sm_dbf(self,menu);
  }
  if ( MB_guiVtable(self)->tmb_finish ) MB_guiVtable(self)->tmb_finish(self);
}

extern void   widm_tmb_enable (Twidm_bar* self) { gui_widegs_enable(self->mb_widEGS); }
extern void   widm_tmb_disable(Twidm_bar* self) { gui_widegs_disable(self->mb_widEGS); }

/*======================================================================*/
