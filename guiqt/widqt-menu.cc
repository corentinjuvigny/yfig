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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: widqt-menu.cc 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-menu.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Different kinds of menu widgets (EGS driver).                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-menu.h header for documentation.                  =*/
/*======================================================================*/

#include "widqt-menu.h"

#include "vgui/vgui-accelkey.h"

/*======================================================================*/
/*= Private utilities                                                  =*/

static QMenu* guiXm_any_build(Twidm_any*menu, CstrBrief label,
                              CstrBrief menuRes, CstrBrief btnRes);

static void gui_menu_createBySet(Tvgui* gui, Twidm_entrySet* s,
        Tegs_widget menu, CstrBrief menuRes, CstrBrief btnRes) 
{
    QMenu* menuWdg = (QMenu*)menu;
    int i;
    for (i = 0; i<s->eleNb ; i++) {
        Twidm_entry* entry = s->eles[i];
        if ( (entry->me_target&GUI_ExtSys_QT)==0 )
            continue;
        if ( entry->me_kind == GUI_ME_ItemSep ) {
            /* put in a separator line */
            menuWdg->addSeparator();
        } else if ( entry->me_kind == GUI_ME_ItemStr ||
                    entry->me_kind == GUI_ME_ItemSubmenu ) {
            /* normal menu entry */
            char accelSequence[100];
            TaccelDef accelDef;
            int underlineIndex = accel_byAmpersand(
                    0,entry->me_label,&accelDef, COPY_IN_AMPERSAND);
            conv_accelDef2str(accelSequence,accelDef);
            QAction *act = new TGUI_menuAction(entry->me_label,menuWdg,entry);
            if ( underlineIndex>=0 ) {
                act->setShortcut(QKeySequence(accelSequence));
            }
            menuWdg->addAction(act);
            entry->me_widEGS = act;
            if ( entry->me_kind == GUI_ME_ItemSubmenu ) {
                act->setMenu( guiXm_any_build( (Twidm_any*)entry->me_submenu,
                                                NULL,menuRes,btnRes) );
            }
        } else {
            PERM_spe_printf("unknown GUI_ME_ItemXXX=%d",entry->me_kind);
        }
    }
}

static QMenu* guiXm_any_build(Twidm_any*menu, CstrBrief label,
                              CstrBrief menuRes, CstrBrief btnRes) 
{
    Tvgui* gui   = menu->ma_gui;
    QMenu* qmenu = new QMenu( label );
    widget_setColor(gui,qmenu, menuRes, EGSCOLOR_default,EGSCOLOR_default);

    /* the button menu */
    menu->ma_btnEGS =  menu->ma_widEGS = (Tegs_widget)qmenu;

    /*  add its entries to the menu */
    gui_menu_createBySet(gui, menu->ma_entries, menu->ma_btnEGS, menuRes, btnRes);

    return qmenu;
}

/*======================================================================*/
/*= Class Implementation                                               =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_menuDD::TGUI_menuDD(Twidm_ddl*m, QWidget*parent)
    : QComboBox(parent), menu(m)
{
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
    // this signal is triggered when user selects an item
    connect(this,SIGNAL(activated(int)),           this,SLOT(selected(int)));
    // this signal is triggered when the selection changes
    //connect(this,SIGNAL(currentIndexChanged(int)), this,SLOT(selected(int)));
}

void TGUI_menuDD::selected(int n)
{
    //printf("%s: item %d Selected\n",__func__,n);
    menu->m_selectedEntry = n;
    Twidm_entry* entry = menu->m_entries->eles[n];
    menu->m_guiCB(&menu->any, entry);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_menuAction::TGUI_menuAction(const char *text, QObject* parent,
                                 const Twidm_entry* gme)
      : QAction(tr(text),parent),data(gme)
{
    connect(this, SIGNAL(triggered()), this, SLOT(selected()));
    if ( PXMEGS_IsDefined(gme->me_pixmap) ) {
        int pixDX = gme->me_pixmapDx;
        int pixDY = gme->me_pixmapDy;
        QSize sz(pixDX,pixDY);
        //setIconSize(sz);
        setIcon( QIcon(PXMegs2qt(gme->me_pixmap)->copy(0,0,pixDX,pixDY)));
    }
    if ( gme->me_checkable ) {
        setCheckable(true);
        setChecked(gme->me_checked!=0);
    }
}

void TGUI_menuAction::selected()
{
    Twidm_any* menu = data->me_menu;
    menu->ma_guiCB(menu, (Twidm_entry*)data);
}

/*======================================================================*/
/*= Interface: VGUI --> GUI Interface                                  =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Entry widget                                                       =*/

static void guiqtm_vt_entry_setEnablement(Cwidm_entry*me, int enabled)
{
    QAction *act = (QAction*)me->me_widEGS;
    act->setEnabled(enabled);
}

static void guiqtm_vt_entry_update(Cwidm_entry* me)
{
    if ( (me->me_target&GUI_ExtSys_XT)==0 ) return;
    if ( me->me_kind == GUI_ME_ItemStr ) {
itemstr:
        QAction *act = (QAction*)me->me_widEGS;
        if ( me->me_label )
            act->setText( me->me_label );
        if (me->me_checkable) {
            act->setCheckable(1);
            if (me->me_checked)
                act->setChecked(1);
            else
                act->setChecked(0);
        } else
            act->setCheckable(0);
    } else if ( me->me_kind == GUI_ME_ItemSubmenu ) {
        goto itemstr;
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }
}

static void guiqtm_vt_lastEntryAdd(Twidm_any*self)
{
    QMenu* menuWdg = (QMenu*)self->ma_widEGS;
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    if ( (me->me_target&GUI_ExtSys_XT)==0 ) return;
    if ( me->me_kind == GUI_ME_ItemSep ) {
        /* put in a separator line */
        menuWdg->addSeparator();
    } else if ( me->me_kind == GUI_ME_ItemStr ) {
        /* normal menu entry */
        char accelSequence[100];
        TaccelDef accelDef;
        int underlineIndex = accel_byAmpersand(
                0,me->me_label,&accelDef, COPY_IN_AMPERSAND);
        conv_accelDef2str(accelSequence,accelDef);
        QAction *act = new TGUI_menuAction(me->me_label,menuWdg,me);
        if ( underlineIndex>=0 ) {
            act->setShortcut(QKeySequence(accelSequence));
        }
        menuWdg->addAction(act);
        me->me_widEGS = act;
    } else if ( me->me_kind == GUI_ME_ItemSubmenu ) {
        TEMP_spe_printf("Adding submenu item is not yet implemented (kind=GUI_ME_ItemSubmenu=%d)",me->me_kind);
    } else {
        TEMP_spf_printf("unknown GUI_ME_ItemXXX=%d",me->me_kind);
    }
}

static void guiqtm_vt_lastEntryDel(Twidm_any*self)
{
    Twidm_entry* me = self->ma_entries->eles[self->ma_entries->eleNb-1];
    QAction *act = (QAction*)me->me_widEGS;
    delete act;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= DDL widget                                                         =*/

static void guiqtm_vt_ddl_dbf(Twidm_ddl* menu,   Tegs_widget parent,
                              CstrBrief menuRes, CstrBrief btnRes)
{
    Tvgui* gui = menu->m_gui;
    QComboBox* self = new TGUI_menuDD(menu,(QWidget*)0); //parent);
    widget_setColor( gui, self , btnRes );
    menu->m_btnEGS = self;
    menu->m_widEGS = self;
    int i;
    for (i=0 ; i<menu->m_entries->eleNb ; i+=1) {
        Twidm_entry* entry = menu->m_entries->eles[i];
        if ( (entry->me_target&GUI_ExtSys_QT)==0 ) continue;
        if ( entry->me_kind != GUI_ME_ItemStr ) continue;
        if ( PXMEGS_IsDefined(entry->me_pixmap) ) {
            int pixDX = entry->me_pixmapDx;
            int pixDY = entry->me_pixmapDy;
            QSize sz(pixDX,pixDY);
            self->setIconSize(sz);
            self->addItem(
                    QIcon( PXMegs2qt(entry->me_pixmap)->copy(0,0,pixDX,pixDY) ),
                    entry->me_label
            );
        } else if ( entry->me_label ) {
            self->addItem(entry->me_label);
        }
    }
    self->setCurrentIndex(menu->m_selectedEntry);
}

static void guiqtm_vt_ddl_setCurrentEntry(Twidm_ddl* menu)
{
    TGUI_menuDD* self = (TGUI_menuDD*)menu->m_btnEGS;
    if ( self!=0 )
        self->setCurrentIndex(menu->m_selectedEntry);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Menu bar                                                           =*/

static Tegs_widget guiqtm_vt_tmb_build(Twidm_bar*tmb, Tegs_widget parent)
{ 
    
    QMenuBar* qtmb = new QMenuBar();
    widget_setColor(tmb->mb_gui,qtmb,tmb->mb_btnRes, EGSCOLOR_default,EGSCOLOR_default);
    return (Tegs_widget*) qtmb;
}

static Tegs_widget guiqtm_vt_tmb_sm_dbf(Twidm_bar*tmb, Twidm_popup*menu)
{
    QMenuBar* qtmb = (QMenuBar*)tmb->mb_widEGS;
    QMenu* qmenu   = guiXm_any_build(&menu->any,menu->m_label,tmb->mb_menuRes,tmb->mb_btnRes);
    qtmb->addMenu(qmenu);
    return (Tegs_widget*) qmenu;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= The GUI vtable.                                                    =*/

struct _Twidm_guiVt guiqt_VtMenu = {
    .entry_setEnablement = guiqtm_vt_entry_setEnablement,
    .entry_update        = guiqtm_vt_entry_update,
    .any_lastEntryAdd    = guiqtm_vt_lastEntryAdd,
    .any_lastEntryDel    = guiqtm_vt_lastEntryDel,
    .ddl_setCurrentEntry = guiqtm_vt_ddl_setCurrentEntry,
    .ddl_dbf             = guiqtm_vt_ddl_dbf,
    .tmb_build           = guiqtm_vt_tmb_build,
    .tmb_sm_dbf          = guiqtm_vt_tmb_sm_dbf,
//  .tmb_finish          = 0,
};

/*======================================================================*/
