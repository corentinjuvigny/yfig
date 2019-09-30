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
 *$Software: YFIG-QT (Interface to Qt)
 *      $Id: widqt-buttonPanel.cc 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-buttonPanel.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Lines, rows and matrix of buttons (EGS driver).                    =*/
/*=                                                                    =*/
/*= See the vgui/wid-buttonPanel.h header for the type and function    =*/
/*= documentation.                                                     =*/
/*= See the guiqt/widqt-buttonPanel.h header for the panel QT classes. =*/
/*======================================================================*/

#include "guiqt.h"
#include "guiqt/widqt-buttonPanel.h"

/*======================================================================*/
/*= The panel classes                                                  =*/

TGUI_ButtonPanel::TGUI_ButtonPanel(Twid_buttonPanel*_panel)
: QScrollArea(), panel(_panel), container(NULL)
{
    Tvgui* gui = _panel->vgbpm_gui;
    widget_setColor(panel->vgbpm_gui,this,panel->vgbpm_wn);
    setFrameStyle(QFrame::Box|QFrame::Plain);
    setLineWidth(0);
    setContentsMargins(0,0,0,0);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //setSizeAdjustPolicy(AdjustToContents);
    enableScrollBar();
    padding = new QLabel;
    padding->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#if 0
{
        QPalette pal = palette();
            pal.setColor(QPalette::Window, Qt::blue);
            pal.setColor(QPalette::Base,   Qt::blue);
            pal.setColor(QPalette::Button, Qt::blue);
            pal.setColor(QPalette::Foreground, Qt::cyan);
        setPalette(pal);
        setAutoFillBackground(true);
        setLineWidth(4);
}
#endif
}

TGUI_ButtonPanel::~TGUI_ButtonPanel() { }

void TGUI_ButtonPanel::enableScrollBar()
{
    if ( panel->bp_scroll==0 ) {
        container = this;
        return;
    }
    panel->bp_widEGSext = this;
    container = new QWidget(this);
    setWidgetResizable(true);
    container->setContentsMargins(0,0,0,0);
    //container->setMinimumSize(200,50);
    //container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if ( panel->bp_type=='H' ) {
        if ( (panel->bp_scroll&2)==0 )
            setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        else
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#ifdef QT_VERS_5
        setSizeAdjustPolicy(AdjustToContents);
#endif
    }
    if ( panel->bp_type=='V' ) {
        if ( (panel->bp_scroll&2)==0 )
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        else
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
#ifdef QT_VERS_5
        setSizeAdjustPolicy(AdjustToContents);
#endif
    }
#if 0
{
        QPalette pal = container->palette();
            pal.setColor(QPalette::Window, Qt::red);
            pal.setColor(QPalette::Base,   Qt::red);
            pal.setColor(QPalette::Button, Qt::red);
            pal.setColor(QPalette::Foreground, Qt::yellow);
        container->setPalette(pal);
        container->setAutoFillBackground(true);
}
#endif
}

void TGUI_ButtonPanel::update()
{ PERM_spe_NYI(0); }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_ButtonPanelRaCa::TGUI_ButtonPanelRaCa(Twid_buttonPanel*_panel)
: TGUI_ButtonPanel(_panel), grid(NULL)
{
    grid = new QGridLayout(this);
    grid->setSpacing(panel->bp_buttonHVsep);
    grid->setContentsMargins(0,0,0,0);
    grid->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void TGUI_ButtonPanelRaCa::finish()
{
    if WID_BP_RaCa_IsFlc(panel)
        finishFlc();
    else {
        TEMP_ase_printf(
            panel->bp_buttonWidth==0 || panel->bp_buttonHeight==0,
           "bp_buttonWidth=%d bp_buttonHVsep=%d : must be both !=0",
            panel->bp_buttonWidth, panel->bp_buttonHeight);
        finishAlc();
    }
}

void TGUI_ButtonPanelRaCa::update()
{
    int i;
    for (i=0 ; i<panel->bp_btnNb ; i+=1 ) {
        grid->removeWidget((QWidget*)panel->bp_btns[i]->but_widEGS);
    }
    finish();
}

void TGUI_ButtonPanelRaCa::finishAlc()
{
    doMappingAlc(3);
}

void TGUI_ButtonPanelRaCa::doMappingAlc(int colnb)
{
    int i;
    int rows   [VGUI_BP_BoutonNbMax];
    int cols   [VGUI_BP_BoutonNbMax];
    int colspan[VGUI_BP_BoutonNbMax];

    colNb=colnb;
    rowNb = wid_buttonPanel_getLayout(panel,colNb,rows,cols,colspan);
    for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1 ) {
        Twid_button* b = panel->vgbpm_buttons[i];
        QWidget* w = (QWidget*) b->but_widEGS;
        grid->addWidget(w,rows[i],cols[i],1,colspan[i]);
    }
    grid->addWidget(padding,rowNb,0,1,colNb);
}

void TGUI_ButtonPanelRaCa::finishFlc()
{
    grid->setSizeConstraint(QLayout::SetFixedSize);
    int colnb,rownb;
    if ( panel->bp_colnb!=0 && panel->bp_rownb==0 ) {
        colnb = panel->bp_colnb;
        rownb = (panel->bp_btnNb+colnb-1)/colnb;
    } else if (  panel->bp_colnb==0 && panel->bp_rownb!=0 ) {
        rownb = panel->bp_rownb;
        colnb = (panel->bp_btnNb+rownb-1)/rownb;
    } else if ( panel->bp_colnb!=0 && panel->bp_rownb!=0 ) {
        colnb = panel->bp_colnb;
        rownb = panel->bp_rownb;
    } else {
        PERM_spf_printf("either col or row number must be specified",0);
    }
    int l,c,idx; Tegs_widget w;
    for (l=0,idx=0 ; l<rownb ; l++) {
        for (c=0 ; c<colnb ; c++) {
            if ( idx<panel->bp_btnNb ) {
                w = panel->bp_btns[idx++]->but_widEGS;
                grid->addWidget((QWidget*)w,l,c);
            }
        }
    }
}

void TGUI_ButtonPanelRaCa::resizeEvent(QResizeEvent * event)
{
    if WID_BP_RaCa_IsFlc(panel)  return;

    int i,col,row;
    int rows    [VGUI_BP_BoutonNbMax];
    int cols    [VGUI_BP_BoutonNbMax];
    int colspan [VGUI_BP_BoutonNbMax];
    int odx = event->oldSize().width();
    int ody = event->oldSize().height();
    int dx  = event->size().width();
    int dy  = event->size().height();
    int eDx,eDy;

    for (col=1 ; col< maxColAlc() ; col+=1) {
        row = wid_buttonPanel_getLayout(panel,col,rows,cols,colspan);
        eDx = wid_buttonPanel_getWidth(panel,col)+30;
        eDy = wid_buttonPanel_getHeight(panel,row);
        if ( eDy<dy ) {
            if ( row!=rowNb || col!=colNb ) {
                for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1 ) {
                    grid->removeWidget((QWidget*)panel->vgbpm_buttons[i]->but_widEGS);
                }
                grid->removeWidget(padding);
                doMappingAlc(col);
            }
            if ( eDx!=dx ) {
                setMinimumWidth(eDx);
                setMaximumWidth(eDx);
                return;
            }
            return;
        } 
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_ButtonPanelR1Cn::TGUI_ButtonPanelR1Cn(Twid_buttonPanel*_panel)
: TGUI_ButtonPanel(_panel)
{
    grid = new QHBoxLayout(container);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    //grid->setSizeConstraint(QLayout::SetMinimumSize);
    //grid->setSizeConstraint(QLayout::SetNoConstraint);
    grid->setSpacing(panel->bp_buttonHVsep);
    grid->setContentsMargins(0,0,0,0);
    grid->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
}

void TGUI_ButtonPanelR1Cn::finish()
{
    int i;
    for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1 ) {
        Twid_button* b = panel->vgbpm_buttons[i];
        QWidget* w = (QWidget*) b->but_widEGS;
        grid->addWidget(w);
    }
    if ( container!=this)
        setWidget(container);
}

#if 1 // FIXME
void TGUI_ButtonPanelR1Cn::showEvent(QShowEvent *event)
{
    int sa_dx  = width();
    int sa_dy  = height();
    if ( panel->bp_widEGSext!=panel->bp_widEGSint ) {
        QWidget* wi = widget();
        int wi_dx  = wi->width();
        int wi_dy  = wi->height();
        //printf("sa_dx/dy=%d/%d : wi_dx/dy=%d/%d\n",sa_dx,sa_dy,wi_dx,wi_dy);
        setMaximumHeight(sa_dy-20);
    } else {
        QRect r = grid->geometry();
        int wi_dx  = r.width();
        int wi_dy  = r.height();
        //printf("sa_dx/dy=%d/%d : la_dx/dy=%d/%d\n",sa_dx,sa_dy,wi_dx,wi_dy);
    }
}
#endif

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_ButtonPanelRnC1::TGUI_ButtonPanelRnC1(Twid_buttonPanel*_panel)
: TGUI_ButtonPanel(_panel)
{
    grid = new QVBoxLayout(container);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    //grid->setSizeConstraint(QLayout::SetMinimumSize);
    //grid->setSizeConstraint(QLayout::SetNoConstraint);
    grid->setSpacing(panel->bp_buttonHVsep);
    grid->setContentsMargins(0,0,0,0);
    grid->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
}

void TGUI_ButtonPanelRnC1::finish()
{
    int i;
    for (i=0 ; i<panel->vgbpm_buttonNb ; i+=1 ) {
        Twid_button* b = panel->vgbpm_buttons[i];
        QWidget* w = (QWidget*) b->but_widEGS;
        grid->addWidget(w);
    }
    //grid->addWidget(padding);
    if ( container!=this)
        setWidget(container);
}

/*======================================================================*/
/*= GUI interface to button panels                                     =*/

extern Tegs_widget gui_buttonPanel_build (Twid_buttonPanel* panel,
                                          Tegs_widget parent)
{
    TGUI_ButtonPanel* widget=0;

    if ( panel->vgbpm_type=='P' ) 
        widget = new TGUI_ButtonPanelRaCa(panel);
    else if ( panel->vgbpm_type=='H' ) 
        widget = new TGUI_ButtonPanelR1Cn(panel);
    else if ( panel->vgbpm_type=='V' ) 
        widget = new TGUI_ButtonPanelRnC1(panel);
    else
        TEMP_spf_printf( "0x%02x panel type not recognized.",
                panel->vgbpm_type);
    return (Tegs_widget*) widget->container;
}

extern void gui_buttonPanel_finishRaCa(Twid_buttonPanel* panel)
{
    TGUI_ButtonPanel* widget = (TGUI_ButtonPanel*) panel->bp_widEGSext;
    widget->finish();
}

extern void gui_buttonPanel_finishR1Cn(Twid_buttonPanel* panel)
{
    TGUI_ButtonPanel* widget = (TGUI_ButtonPanel*) panel->bp_widEGSext;
    widget->finish();
}

extern void gui_buttonPanel_finishRnC1(Twid_buttonPanel* panel)
{
    TGUI_ButtonPanel* widget = (TGUI_ButtonPanel*) panel->bp_widEGSext;
    widget->finish();
}

extern void gui_buttonPanel_update(Twid_buttonPanel* panel)
{
    TGUI_ButtonPanel* widget = (TGUI_ButtonPanel*) panel->bp_widEGSext;
    widget->update();
}
/*======================================================================*/
