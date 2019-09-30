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
 *      $Id: guiqt-dw.cc 366 2018-11-18 23:47:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-dw.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "guiqt-dw.h"
#include "guiqt/guiqt-create.h"

/*======================================================================*/
/*= Running dialog                                                     =*/

static Twid_dialStatus vt_modal_run(Tvgui_dw*d)
{
    Tvgui_dwTop* top = d->top;
    TGUI_dialog* qtd = (TGUI_dialog*) d->D_widEGS;
    qtd->exec();
    return top->dt_buttSelected;
}

static Twid_dialStatus vt_modal_run2(Tvgui_dw*d, Tegs_widget inw, int x, int y)
{
    if ( inw==0 ) return vt_modal_run(d);

    Tvgui_dwTop* top = d->top;
    TGUI_dialog* qtd = (TGUI_dialog*) d->D_widEGS;
    Tvgui_mw*    vmw = d->D_vmw;

    QWidget tmp;
    QWidget* qinw = (QWidget*)inw;
    if ( qinw->layout()!=0 )
        tmp.setLayout( qinw->layout() );
    qtd->setParent((QWidget*)inw);
    qtd->move(x,y);
    qtd->activateWindow();
    if (vmw) vmw->setInputEnable(vmw,0);
    qtd->exec();
    if (vmw) vmw->setInputEnable(vmw,1);
    if ( tmp.layout()!=0 )
        qinw->setLayout( tmp.layout() );

    return top->dt_buttSelected;
}

static void vt_modal_breakloop(Tvgui_dw*vdw, Twid_dialStatus s) { ((TGUI_dialog*)vdw->D_widEGS)->doBreakloop(s); }

static void vt_modeless_start(Tvgui_dw* d)
{
    Tvgui_dwTop* top = d->top;
    TGUI_dialog*     qtd  = (TGUI_dialog*) d->D_widEGS;

#ifdef QT_VERS_4
    qtd->show();
    qtd->raise();
    qtd->activateWindow();
#endif
#ifdef QT_VERS_5
    // with QT5 "qtd->show(); qtd->raise();" bugs randomly if
    // it is done before the main loop is started.
    // It is the case for the YFIG console that is started before
    // the main loop is entered.
    // The next line delays them. Indeed the main loop will
    // runs it.
    // Note this hack does not pose a problem if the main loop
    // is already started, the dialog will just appear a few later.
//    QTimer::singleShot( 0, qtd, SLOT(show()) );
//    QTimer::singleShot( 0, qtd, SLOT(raise()) );
//    qtd->activateWindow();
    qtd->show();
    qtd->raise();
    qtd->activateWindow();
#endif
}

static void dw_vtInit(Tvgui_dw* d)
{
    TEMP_asf_printf( d->top==0, "%s", "self->top==0 --> not a top dialog" );
    d->top->vt_modal_run       = vt_modal_run;
    d->top->vt_modal_run2      = vt_modal_run2;
    d->top->vt_modal_breakloop = vt_modal_breakloop;
    d->top->vt_modeless_start  = vt_modeless_start;
}

/*======================================================================*/
/*= Constructors and destructor                                        =*/

extern Tegs_widget gui_dw_create(Tvgui_dw* d, CstrBrief wn,
                                 int bd, int dx, int dy, int resizable)
{
    Tvgui*      gui = d->D_gui;
    QWidget*    parent;
    if ( VGUI_D_isTopDial(d) ) {
        // top dialog
        parent = new TGUI_dialog( d,bd,dx,dy,resizable );
    } else {
        // sub dialog
        parent = new TGUI_subdialog( d,bd,dx,dy,resizable );
    }
    d->D_widEGS = parent;
    widget_setColor(gui,parent,wn);

    // now we create the form
    QGridLayout* grid = (QGridLayout*)create_form(gui,parent,wn,
      d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 2 : 0, // hvsep
      bd>=0 ? bd : d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 1 : 0, // bd
      dx,dy,
      EGSCOLOR_default,EGSCOLOR_default
    );
    if  ( VGUI_D_isTopDial(d) )
        grid->setSizeConstraint(QLayout::SetFixedSize);

    if ( d->pDialog==0 )
        grid->setContentsMargins(0,0,5,5);
    else if ( d->pDialog->pDialog==0 )
        grid->setContentsMargins(5,5,2,2);
    else 
        grid->setContentsMargins(0,0,0,0);

    if ( bd>0 || d->pDialog==0 || d->pDialog->pDialog==0 ) {
        QFrame*frame = new QFrame();
        frame->setLineWidth(bd);
        frame->setFrameStyle(QFrame::Box|QFrame::Plain);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        grid->addWidget(frame,0,0);
        grid = (QGridLayout*)create_form(gui,frame,wn,
          d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 2 : 0, // hvsep
          bd>=0 ? bd : d->pDialog==0 ? 2 : d->pDialog->pDialog==0 ? 1 : 0, // bd
          dx,dy,
          EGSCOLOR_default,EGSCOLOR_default
        );
        grid->setContentsMargins(0,0,0,0);
        //grid->setSizeConstraint(QLayout::SetFixedSize);
    }

    if ( VGUI_D_isTopDial(d) ) {
        TGUI_dialog* qtd = (TGUI_dialog*)parent;
        qtd->grid = grid;
    } else {
        TGUI_subdialog* qtd = (TGUI_subdialog*)parent;
        qtd->grid = grid;
    }
    return d->D_widEGS;
}

extern Tegs_widget gui_dw_create2(Tvgui_dw* d, Tegs_widget parent, CstrBrief wn,
                                int bd, int dx, int dy, int resizable)
{ return gui_dw_create(d,wn,bd,dx,dy,resizable); }

extern void gui_dw_free(Tvgui_dw* d)
{
    if ( VGUI_D_isTopDial(d) ) {
        TGUI_dialog* qtd  = (TGUI_dialog*) d->D_widEGS;
        delete qtd;
    }
}

extern void gui_dw_finishSdx(Tvgui_dw*vdw, Tegs_widget*ws, int wn)
{
    PERM_spe_NYI(0);
#if 0
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
#endif
}

extern void gui_dw_finishSdy(Tvgui_dw*vdw, Tegs_widget*ws, int wn);

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/

extern void            gui_dw_setTitle(Cvgui_dw* d, TstrBrief t)
{
    TEMP_asf_printf( !VGUI_D_isTopDial(d),"invalid in sub dialog",0);
    TGUI_dialog* qtd = (TGUI_dialog*) d->D_widEGS;
    qtd->setWindowTitle( t?t:PACKAGE );
}

/*======================================================================*/
/*= Implementation of TGUI_dialog                                      =*/

TGUI_dialog::TGUI_dialog(Tvgui_dw*_d, int bd, int dx, int dy, int resizable)
: QDialog(), d(_d), grid(0)
{
    dw_vtInit(_d);
    //printf("%s: bd=%d dx=%d dy=%d rble=%d\n",__func__,bd,dx,dy,resizable);
    //if ( bd>0 ) setStyleSheet("border: 2px");
    if ( dx>0 ) setFixedWidth(dx);
    if ( dy>0 ) setFixedHeight(dy);
    widget_setColor(d->D_gui, this,wn_dialShell);
}

bool TGUI_dialog::close() { return QDialog::close(); }
   //void done(int r);
void TGUI_dialog::accept() { done( d->top->dt_buttSelected ); }
void TGUI_dialog::reject() { done( d->top->dt_buttSelected ); }
void TGUI_dialog::activateWindow()
{
    QDialog::activateWindow();
    QWidget* focus = (QWidget*)d->top->dt_focus;
    if ( focus!=0 ) {
        focus->setFocus();
    }
}

/*======================================================================*/
/*= Implementation of TGUI_dialog                                      =*/

TGUI_subdialog::TGUI_subdialog(Tvgui_dw*_d,
                               int bd, int dx, int dy, int resizable)
: QWidget( _d->D_pDialog ? (QWidget*)_d->D_pDialog->D_widEGS
                         : (QWidget*)NULL), d(_d), grid(0)
{
    //printf("%s: bd=%d dx=%d dy=%d rble=%d\n",__func__,bd,dx,dy,resizable);
    //if ( bd>0 ) setStyleSheet("border: 2px");
    if ( dx>0 ) setFixedWidth(dx);
    if ( dy>0 ) setFixedHeight(dy);
    widget_setColor(d->D_gui, this,wn_dialShell);
}

/*======================================================================*/
