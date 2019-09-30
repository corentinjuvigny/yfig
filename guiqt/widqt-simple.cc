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
 *      $Id: widqt-simple.cc 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guiqt.h"
#include "widqt-simple.h"

#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Label widget                                                       =*/ 

static QLabel* widget_label_create(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    if ( text==0 ) text="";

    QLabel* ret = new QLabel(text);
    if (bdw>=0)     { ret->setLineWidth(bdw);
                      ret->setFrameStyle(QFrame::Box|QFrame::Plain); }

    if (dx>0)         ret->setMinimumWidth (dx);
    if (dy>0)         ret->setMinimumHeight(dy);
    widget_setColor(gui, ret, wn, bg,fg);
    return ret;
}

extern Tegs_widget gui_label_nbf( // FIXME:OLD widget_label_create
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{ return widget_label_create(gui,pw,wn,bdw,dx,dy,bg,fg,text); }

extern void gui_label_setTextAS(Tegs_widget self,  CstrBrief text)
{ ((QLabel*)self)->setText(text); }

extern void gui_label_setTextSS(Tegs_widget self,  CstrBrief text)
{ ((QLabel*)self)->setText(text); }

extern void gui_label_setPixmap(Tegs_widget self, TpixmapEGS pm)
{
    QLabel* qself = (QLabel*)self;
//    if ( !qself->autoFillBackground() ) {
//        qself->setBackgroundRole(QPalette::Base);
//        qself->setAutoFillBackground(true);
//    }
    qself->setPixmap(*PXMegs2qt(pm));
}

/*======================================================================*/
/*= Text input widgets (Single Line)                                   =*/
/*= (done in guiqt/guiqt-simple-slin.c)                                =*/

/*======================================================================*/
/*= Text output widgets (Single Line)                                  =*/

extern Tegs_widget gui_slout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    if ( text==0 ) text="";

    QLabel* self = new QLabel(text);
    //if (bdw>=0)               ret->setLineWidth(bdw); // FIXME
    if (dx>0)                self->setMinimumWidth(dx);
    if (dy>0)                self->setMinimumWidth(dy);
    widget_setColor(gui, self, wn, bg,fg);
    return self;
}

extern TstrEaten gui_slout_getText (Tegs_widget self)
{
    TEMP_asf_printf( dynamic_cast<QLabel*>((QObject*)self)==0,
                     "Not a QLabel",0);
    QLabel*    label = (QLabel*)self;
    QString    qstr  = label->text();
    return conv_QStr2CStrUtf8(qstr);
}
extern void gui_slout_setText(Tegs_widget self, CstrBrief str)
{
    TEMP_asf_printf( dynamic_cast<QLabel*>((QObject*)self)==0,
                     "Not a QLabel",0);
    ((QLabel*)self)->setText(str);
}
extern void gui_slout_setTextWrn(Tegs_widget self, CstrBrief str)
{ gui_slout_setText(self,str); }
extern void gui_slout_setTextErr(Tegs_widget self, CstrBrief str)
{ gui_slout_setText(self,str); }

/*======================================================================*/
/*= Text output widgets (Multiple Line with scroll bar)                =*/

extern Tegs_widget gui_mlout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        int sb)
{
    QTextEdit* self = new QTextEdit();
    self->setReadOnly(1);
    //if (bdw>=0)               w->setLineWidth(bdw); // FIXME
    if (dx>0)  self->setMinimumWidth(dx);
    if (dy>0)  self->setMaximumHeight(dy);
    widget_setColor(gui, self, wn, bg,fg);
    return self;
}

extern void gui_mlout_setText(Twid_mlout* self, CstrBrief str)
{
    QTextEdit* qte = (QTextEdit*)self->widEGS;
    TEMP_asf_printf( dynamic_cast<QTextEdit*>((QObject*)qte)==0,
                     "Not a QTextEdit",0);
    qte->setText(str);
}
extern void gui_mlout_setTextWrn(Twid_mlout* self, CstrBrief str)
{ gui_mlout_setText(self,str); }
extern void gui_mlout_setTextErr(Twid_mlout* self, CstrBrief str)
{ gui_mlout_setText(self,str); }

static void gui_mlout_appTextWC(Twid_mlout* self, CstrBrief str,
                                TcolorEGS*c)
{
    QTextEdit* qte = (QTextEdit*)self->widEGS;
    qte->insertPlainText(str);
    qte->verticalScrollBar()->setValue(qte->verticalScrollBar()->maximum());
}

extern void gui_mlout_appText(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextWrn(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextErr(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }

/*======================================================================*/
/*= Spinner widget.                                                    =*/
/*= (done in guiqt/guiqt-simple-spinner.c)                             =*/

/*======================================================================*/
/*= Slider widget.                                                    =*/

TGUI_slider::TGUI_slider(Twid_slider* dt, QWidget*pw)
: QSlider(Qt::Vertical,pw), data(dt), lock(0)
{ 
    connect( this, SIGNAL(valueChanged(int)),
    //connect( this, SIGNAL(sliderMoved(int)),
             this, SLOT  (valueChanged_cb(int)) );
    setTracking(false);
}

void TGUI_slider::valueChanged_cb(int v)
{
    double newval = convEgs2Vgui(v);

    if ( newval==data->val) return;

    data->val = newval;

    if ( lock==1 ) return;
    if ( data->chg && isVisible() ) {
        lock=1;
        data->chg(data,data->ud,data->val);
        lock=0;
    }
}

extern Tegs_widget  gui_slider_create(Twid_slider*self,
                                      Tegs_widget pw, CstrBrief wn,
                                      int bdt, int dx, int dy)
{
    QSlider* ret = new TGUI_slider(self,(QWidget*)pw);
    //if (bww>=0)               ret->setLineWidth(bdw); // FIXME
    if (dx>0)                ret->setMinimumWidth (dx);
    if (dy>0)                ret->setMinimumHeight(dy);
    widget_setColor(self->gui, ret, wn);
    ret->setMinimum(0);
    ret->setMaximum(round(1./self->inc));
    ret->setSingleStep(1);
    return ret;
}

extern double       gui_slider_get (Twid_slider*self)
{
    TGUI_slider* qw = (TGUI_slider*)self->widEGS;
    double vguival = qw->convEgs2Vgui(qw->value());
    return vguival;
}

extern void         gui_slider_set (Twid_slider*self, double v)
{
    TGUI_slider* qw = (TGUI_slider*)self->widEGS;
    int egsval = qw->convVgui2Egs(v);
    qw->lock += 1;
    qw->setValue( egsval );
    qw->lock -= 1;
}

/*======================================================================*/
/*= Slider widget.                                                     =*/

/*======================================================================*/
