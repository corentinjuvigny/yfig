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
 *      $Id: widqt-simple-slin.cc 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple-slin.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*= Implementation of single line input widget (SLIN).                 =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "widqt-simple-slin.h"

/*======================================================================*/
/* Interface                                                           =*/

extern Tegs_widget gui_slin_dbf(Twid_slin*self,
                    Tegs_widget pw, CstrBrief wn,
                    int bd, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
                    CstrBrief text)
{
    if ( text==0 ) text="";

    QLineEdit* widget = new TGUI_LineEdit(self,text);
    //if (bw>=0)               w->setLineWidth(2); // FIXME
    if (dx>0)  widget->setMinimumWidth(dx);
    if (dy>0)  widget->setMaximumHeight(dy);
    widget_setColor(self->gui, widget, wn, fg,bg);

    return widget;
}

extern TstrEaten  gui_slin_getText   (Twid_slin* self)
{
    TstrBrief str;
    QLineEdit* widget = (QLineEdit*)self->widEGS;
    QString    qstr = widget->text();
    str = tool_strdup( conv_QStr2CStrUtf8( qstr ) );
    return str;
}

extern void       gui_slin_setText(Twid_slin* self, CstrBrief str)
{
    QLineEdit* widget = (QLineEdit*)self->widEGS;
    widget->setText(str);
}

extern void        gui_slin_disable(Twid_slin* self)
{
    QLineEdit* widget = (QLineEdit*)self->widEGS;
    widget->setReadOnly(true);
}

extern void        gui_slin_enable (Twid_slin* self)
{
    QLineEdit* widget = (QLineEdit*)self->widEGS;
    widget->setReadOnly(false);
}

/*======================================================================*/
/* class                                                               =*/

TGUI_LineEdit::TGUI_LineEdit(Twid_slin*self,CstrBrief text)
: QLineEdit(text), data(self)
{
    connect( this, SIGNAL(textChanged(const QString &)),
             this, SLOT  (textChanged_cb(const QString &)) );
    
    connect( this, SIGNAL(editingFinished()),
             this, SLOT  (editingFinished_cb()) );

    connect( this, SIGNAL(returnPressed()),
             this, SLOT  (returnPressed_cb()) );
    setFocus();
}

void TGUI_LineEdit::textChanged_cb(const QString &text) {
    //char buf[STRLEN_MAX+1];
    Twid_slin* self = data;
    DBG1_MSlinE("slin=%p ud=%p",self,self->ud);

    //const char* str=0;
    //int pos = gui_slin_getPosStr(&str,self);
    //DBG1_MSlinM("slin=%p ud=%p : str=%s",self,self->ud, str);
    if ( self->chg!=0 ) self->chg(self,self->ud);
    DBG1_MSlinL("slin=%p ud=%p",self,self->ud);
}

void TGUI_LineEdit::editingFinished_cb() {
    //printf("%s: %s\n", __PRETTY_FUNCTION__,"");
}

void TGUI_LineEdit::returnPressed_cb() {
    Twid_slin* self = data;
    DBG1_MSlinE("slin=%p ud=%p",self,self->ud);
    if ( self->val!=0 )  self->val(self,self->ud);
    DBG1_MSlinL("slin=%p ud=%p",self,self->ud);
}

/*======================================================================*/
