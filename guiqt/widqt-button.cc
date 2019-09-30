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
 *      $Id: widqt-button.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-button.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous buttons (EGS driver).                                =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for the type and function         =*/
/*= documentation.                                                     =*/
/*= See the guiqt/widqt-button.h header for the button QT classes.     =*/
/*======================================================================*/

#include "guiqt.h"
#include "guiqt/widqt-button.h"

#include "vgui/vgui-accelkey.h"

/*======================================================================*/
/*= The ornament button                                                =*/

extern Tegs_widget gui_button_orn_dbf(Twid_button*  b, Tegs_widget pw)
{
    QLabel*  widget= new QLabel(b->but_orn_text);
    widget->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    return widget;
}

/*======================================================================*/
/*= The standard button                                                =*/

extern Tegs_widget gui_button_std_dbf(Twid_button*  b, Tegs_widget pw)
{ return new TGUI_ButtonStd(b,pw); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_ButtonStd::TGUI_ButtonStd(Twid_button* b, Tegs_widget pw)
//: TGUI_Button(b,pw)
: QToolButton((QWidget*)pw), TGUI_Button(b,pw)
{
    init(this);
    redraw();
    //setCheckable(true);
    //setDown(false);
}

void TGUI_ButtonStd::mousePressEvent(QMouseEvent *e)
{
    DBG0_ButE("butt=%p",button);
    int num;
    if ( e->button()==Qt::LeftButton ) {
        num=1;
    } else if ( e->button()==Qt::MiddleButton ) {
        num=2;
    } else if ( e->button()==Qt::RightButton ) {
        num=3;
    } else {
        DBG0_ButL("butt=%p ; not a button (send to upper class)",button);
        //TGUI_Button::mousePressEvent(e);
        QToolButton::mousePressEvent(e);
        return;
    }
    DBG2_ButM("butt=%p ; button=%d (%s)",button,num,
            num==1?"left":num==2?"middle":num==3?"right":"????");
    animateClick();
    wid_button_std_cb(button,num);
    DBG1_ButL("butt=%p",button);
}

void TGUI_ButtonStd::wheelEvent(QWheelEvent *event)
{
    int i;
    DBG0_ButE("butt=%p",button);
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    if ( numSteps>0 ) {
        for (i=0 ; i<numSteps ; i++)
            wid_button_std_cb(button,4);
    } else {
        for (i=0 ; i<-numSteps ; i++)
            wid_button_std_cb(button,5);
    }
    DBG1_ButL("butt=%p %d %d\n",button,numDegrees,numSteps);
}

void TGUI_ButtonStd::redraw()
{
    if ( PXMEGS_IsDefined(button->but_std_pixmap) ) {
        int pixDX = button->but_dx;
        int pixDY = button->but_dy;
#if 0 // FIXME
        int border = p->vgbpm_buttonBorder;
        setMinimumSize (pixDX+border,pixDY+border);
        setMaximumSize (pixDX+border,pixDY+border);
#endif
        QSize sz(pixDX,pixDY);
        setIconSize(sz);
        setIcon( QIcon( PXMegs2qt(button->but_std_pixmap)->copy(0,0,pixDX,pixDY)));
    } else {
        setText( button->but_std_text );
    }
}

/*======================================================================*/
/*= The On/Off button                                                  =*/

extern Tegs_widget gui_button_onoff_dbf(Twid_button*  b, Tegs_widget pw)
{
    Tegs_widget w;
    TEMP_ase_printf( dynamic_cast<QWidget*>((QObject*)pw)==NULL,
            "pw=%p not a QWidget",pw);
    if ( dynamic_cast<QWidget*>((QObject*)pw)==NULL ) pw=0;
    if ( b->but_onoff_checkBtn!=0 )
        w = new TGUI_ButtonCB(b,pw);
    else
        w = new TGUI_ButtonOnOff(b,pw);
    return w;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_ButtonOnOff::TGUI_ButtonOnOff(Twid_button* b, Tegs_widget pw)
: QToolButton((QWidget*)pw), TGUI_Button(b,pw)
{
    init(this);
    setOff();
    setCheckable(true);
    connect(this, SIGNAL(clicked(bool)),
            this, SLOT(selected(bool)) );
}

void TGUI_ButtonOnOff::selected(bool checked)
{
    //printf("%s(checked=%d):0: button\n",__func__,checked,this);
    wid_button_onoff_cb(button,0);
}

void TGUI_ButtonOnOff::selectedByShortcut()
{
    if ( isChecked() ) return;
    wid_button_onoff_cb(button,0);
}

void TGUI_ButtonOnOff::setOff()
{
    int pixDX = button->but_dx;
    int pixDY = button->but_dy;
    if (button->but_onoff_usePixmap) {
        if ( PXMEGS_IsDefined(button->but_onoff_pixmapOff) ) {
            QSize sz(pixDX,pixDY);
            setIconSize(sz);
            setIcon( QIcon( *PXMegs2qt(button->but_onoff_pixmapOff)) );
        }
    } else {
        Tvgui* gui = wid_button_getVGui(button);
//printf("BTN-OFF: %s : %s\n",button->but_onoff_textOff, button->but_onoff_wnOff);
        widget_setColor(gui,this,button->but_onoff_wnOff);
        if (button->but_onoff_textOff)
            setText( button->but_onoff_textOff );
    }
    setDown(false);
}

void TGUI_ButtonOnOff::setOn()
{
    int pixDX = button->but_dx;
    int pixDY = button->but_dy;
    if (button->but_onoff_usePixmap) {
        if ( PXMEGS_IsDefined(button->but_onoff_pixmapOn) ) {
            QSize sz(pixDX,pixDY);
            setIconSize(sz);
            setIcon( QIcon( *PXMegs2qt(button->but_onoff_pixmapOn)) );
        }
    } else {
//printf("BTN-ON : %s : %s\n",button->but_onoff_textOff, button->but_onoff_wnOn);
        Tvgui* gui = wid_button_getVGui(button);
        widget_setColor(gui,this,button->but_onoff_wnOn);
        if (button->but_onoff_textOn)
            setText( button->but_onoff_textOn );
        else if (button->but_onoff_textOff)
            setText( button->but_onoff_textOff );

    }
    setDown(true);
}

void TGUI_ButtonOnOff::showEvent(QShowEvent* event)
{ wid_button_map_cb(button); }

/*======================================================================*/
/*= The Check Box button                                               =*/

TGUI_ButtonCB::TGUI_ButtonCB(Twid_button* b, Tegs_widget pw)
: QCheckBox((QWidget*)pw), TGUI_Button(b,pw), lock(0)
{
    init(this);
    setOff();
    connect(this, SIGNAL(stateChanged(int)),
            this, SLOT(stateChanged(int)) );
    redraw();
}

void TGUI_ButtonCB::stateChanged(int state)
{
    if (lock>0) return;
    lock +=1;
    wid_button_onoff_cb(button,0);
    lock -=1;
}

void TGUI_ButtonCB::redraw()
{
    int pixDX = button->but_dx;
    int pixDY = button->but_dy;
    if (button->but_onoff_usePixmap) {
        if ( PXMEGS_IsDefined(button->but_onoff_pixmapOff) ) {
            QSize sz(pixDX,pixDY);
            setIconSize(sz);
            setIcon( QIcon( *PXMegs2qt(button->but_onoff_pixmapOff)) );
        }
    } else if (button->but_onoff_textOff) {
            setText( button->but_onoff_textOff );
    }
}

void TGUI_ButtonCB::setOff()
{
    lock +=1;
    setCheckState(Qt::Unchecked);
    lock -=1;
}

void TGUI_ButtonCB::setOn()
{
    lock +=1;
    setCheckState(Qt::Checked);
    lock -=1;
}

/*======================================================================*/
/*= Button operation                                                   =*/

extern void  gui_button_setOn (Twid_button* button)
{
#if 0 // This line is the right code but MOC makes it to fail
    TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
    qb->setOn();
#else // So we implement badly the virtual feature !!!
    if ( button->but_kind==WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn!=0 ) {
            TGUI_ButtonCB* qb = (TGUI_ButtonCB*)button->but_widEGS;
            qb->TGUI_ButtonCB::setOn();
        } else {
            TGUI_ButtonOnOff* qb = (TGUI_ButtonOnOff*)button->but_widEGS;
            qb->TGUI_ButtonOnOff::setOn();
        }
    } else {
        TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
        qb->TGUI_Button::setOn();
    }
#endif
}

extern void  gui_button_setOff(Twid_button* button)
{
#if 0 // This line is the right code but MOC makes it to fail
    TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
    qb->setOff();
#else // So we implement badly the virtual feature !!!
    if ( button->but_kind==WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn!=0 ) {
            TGUI_ButtonCB* qb = (TGUI_ButtonCB*)button->but_widEGS;
            qb->TGUI_ButtonCB::setOff();
        } else {
            TGUI_ButtonOnOff* qb = (TGUI_ButtonOnOff*)button->but_widEGS;
            qb->TGUI_ButtonOnOff::setOff();
        }
    } else {
        TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
        qb->TGUI_Button::setOff();
    }
#endif
}

extern void  gui_button_redraw(Twid_button* button)
{
#if 0 // This line is the right code but MOC makes it to fail
    TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
    qb->redraw();
#else // So we implement badly the virtual feature !!!
    if ( button->but_kind==WID_BUT_KD_Standard ) {
        TGUI_ButtonStd* qb = (TGUI_ButtonStd*)button->but_widEGS;
        qb->TGUI_ButtonStd::redraw();
    } else if ( button->but_kind==WID_BUT_KD_OnOff ) {
        if ( button->but_onoff_checkBtn!=0 ) {
            TGUI_ButtonCB* qb = (TGUI_ButtonCB*)button->but_widEGS;
            qb->TGUI_ButtonCB::redraw();
        } else {
            //TGUI_ButtonOnOff* qb = (TGUI_ButtonOnOff*)button->but_widEGS;
            //qb->TGUI_ButtonOnOff::setOff();
            TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
            qb->TGUI_Button::redraw();
        }
    } else {
        TGUI_Button* qb = (TGUI_Button*)button->but_widEGS;
        qb->TGUI_Button::redraw();
    }
#endif
}

/*======================================================================*/
/*= The button classes                                                 =*/

TGUI_Button::TGUI_Button(Twid_button* b, Tegs_widget pw)
: button(b) { }
void TGUI_Button::init(QAbstractButton* qbtn)
{
    Tvgui*       gui  = wid_button_getVGui(button);
    Twid_button* b    = button;

    // configuration
    int  dx   = b->but_dx;
    int  dy   = b->but_dy;
    Cstr wn   = b->but_wn;
    //if (bww>=0)               ret->setLineWidth(bdw); // FIXME
    if (dx>0)                qbtn->setMinimumWidth (dx);
    if (dy>0)                qbtn->setMinimumHeight(dy);
    if (dx>0)                qbtn->setMaximumWidth (dx);
    if (dy>0)                qbtn->setMaximumHeight(dy);
    widget_setColor(gui, qbtn, wn); //, EGSCOLOR_DEFAULT,fg);
 
    // help balloon
    gui_widegs_addTipAA(qbtn, b->but_helpText,
                        b->but_accelKey0,b->but_accelKey1);

    // key accelerators
    Tgui_main* main = gui->g_main;
    QShortcut* shortcut;
    char accelbuf[100];

    if ( b->but_accelKey0!=0 ) {
        conv_accelDef2str(accelbuf,b->but_accelKey0);
        shortcut = new QShortcut(QKeySequence(accelbuf), main->m_topForm);
        shortcut->setWhatsThis(button->but_helpText);
        QObject::connect(shortcut, SIGNAL(activated()),
                 qbtn, SLOT(selectedByShortcut()));
    }
    if ( button->but_accelKey1!=0 ) {
        conv_accelDef2str(accelbuf,b->but_accelKey1);
        shortcut = new QShortcut(QKeySequence(accelbuf), main->m_topForm);
        shortcut->setWhatsThis(button->but_helpText);
        QObject::connect(shortcut, SIGNAL(activated()),
                qbtn, SLOT(selectedByShortcut()));
    }
}

void TGUI_Button::selectedByShortcut() { }

#define INVALID_KIND() PERM_spe_printf(\
            "Invalid call this button kind (%d)", button->but_kind);

void TGUI_Button::redraw() { INVALID_KIND(); }
void TGUI_Button::setOn()  { INVALID_KIND(); }
void TGUI_Button::setOff() { INVALID_KIND(); }
void TGUI_Button::showEvent(QShowEvent *event)
{
    if ( (button->btn_option&WID_BTNOPT_CCBWM)!=0 )
        wid_button_map_cb(button);
}

/*======================================================================*/
