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
 *      $Id: widqt-simple-spinner.cc 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-simple-spinner.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Spinner widgets (EGS driver).                                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h and widqt-simple-spinner.h headers       =*/
/*= for the type/class description and for the function description    =*/
/*= and for documentation.                                             =*/
/*======================================================================*/

#include "guiqt.h"

#include "widqt-simple-spinner.h"
#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Spinner widget (EGS Interface)                                     =*/

extern void gui_spinner_dbf(Twid_spinner*spin, Tvgui*gui, Tegs_widget p)
{
    Trange* range = &spin->range;
    if ( range->nAdt!=0 ) {
        TGUI_spinBoxDle* qspin = new TGUI_spinBoxDle(spin,gui);
        widget_setColor(gui, qspin, wn_dialSlin);
    } else {
        TGUI_spinBoxInt* qspin = new TGUI_spinBoxInt(spin,gui);
        widget_setColor(gui, qspin, wn_dialSlin);
    }
    spin->enabled=1;
}

extern void          gui_spinner_free   (Twid_spinner* spin)
{  }

extern void          gui_spinner_update (Twid_spinner* spin)
{
    Trange* range = &spin->range;
    if ( range->nAdt!=0 ) {
        TGUI_spinBoxDle* qspin = (TGUI_spinBoxDle*)spin->self;
        if ( (range->flag&RANGE_FL_LowBound)!=0 ) qspin->setMinimum(range->min);
        if ( (range->flag&RANGE_FL_UppBound)!=0 ) qspin->setMaximum(range->max);
    } else {
        TGUI_spinBoxInt* qspin = (TGUI_spinBoxInt*)spin->self;
        if ( (range->flag&RANGE_FL_LowBound)!=0 ) qspin->setMinimum(range->min);
        if ( (range->flag&RANGE_FL_UppBound)!=0 ) qspin->setMaximum(range->max);
    }
}

extern TstrEaten     gui_spinner_setStr (Twid_spinner* spin);
extern void          gui_spinner_setNum (Twid_spinner* spin, double v)
{
    Trange* range = &spin->range;
    if ( range->nAdt!=0 ) {
        TGUI_spinBoxDle* qspin = (TGUI_spinBoxDle*)spin->self;
        qspin->setValue(v);
    } else {
        TGUI_spinBoxInt* qspin = (TGUI_spinBoxInt*)spin->self;
        qspin->setValue(v);
    }
}

extern void          gui_spinner_clear  (Twid_spinner* spin)
{
    QSpinBox* qspin = (QSpinBox*)spin->self;
    qspin->clear();
}

extern void          gui_spinner_enable (Twid_spinner* spin)
{
    QWidget* qspin = (QWidget*)spin->self;
    qspin->setEnabled(true);
}

extern void          gui_spinner_disable(Twid_spinner* spin)
{
    QWidget* qspin = (QWidget*)spin->self;
    qspin->setEnabled(false);
}

/*======================================================================*/
/*= TGUI_spinner class implementation                                  =*/

TGUI_spinBoxDle::TGUI_spinBoxDle(Twid_spinner*s, Tvgui* gui, QWidget* p)
  : QDoubleSpinBox(p), spin(s)
{
    Trange* range = &spin->range;
    spin->self = this;
    setDecimals(range->nAdt);
    if ( (range->flag&RANGE_FL_LowBound)!=0 )
        setMinimum(range->min);
    if ( (range->flag&RANGE_FL_UppBound)!=0 )
        setMaximum(range->max);
    else {
        setMinimum(-pow( 10, range->nBdt) );
        setMaximum( pow( 10, range->nBdt) );
    }
    setAlignment(Qt::AlignRight);
    if ( spin->cbVal ) {
        connect( this, SIGNAL(editingFinished()),
                 this, SLOT(  valueValidated()) );
    }
    if ( spin->cbChg ) {
        connect( this, SIGNAL(valueChanged(double)),
                 this, SLOT(valueChanged(double)) );
    }
}

void TGUI_spinBoxDle::stepBy(int steps)
{
    Trange* range = &spin->range;
    if ( steps<0 ) range_dec(range);
    if ( steps>0 ) range_inc(range);
    setValue(range->val);
}

void TGUI_spinBoxDle::valueValidated() {
    Trange* range = &spin->range;
    if (  spin->cbVal ) {
        range_setVal(range,value());
        if (spin->enabled) spin->cbVal(spin,spin->cbUd,value());
    }
}
void TGUI_spinBoxDle::valueChanged(double v) {
    Trange* range = &spin->range;
    if (  spin->cbChg ) {
        range_setVal(range,value());
        if (spin->enabled) spin->cbChg(spin,spin->cbUd,value());
    }
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

TGUI_spinBoxInt::TGUI_spinBoxInt(Twid_spinner*s, Tvgui* gui, QWidget* p)
  : QSpinBox(p), spin(s)
{
    Trange* range = &spin->range;
    spin->self = this;
    if ( (range->flag&RANGE_FL_LowBound)!=0 ) setMinimum(range->min);
    if ( (range->flag&RANGE_FL_UppBound)!=0 ) setMaximum(range->max);
    if ( spin->cbVal ) {
        connect( this, SIGNAL(editingFinished()),
                 this, SLOT(  valueValidated()) );
    }
    if ( spin->cbChg ) {
        connect( this, SIGNAL(valueChanged(int)),
                 this, SLOT(  valueChanged(int)) );
    }
}

void TGUI_spinBoxInt::stepBy(int steps)
{
    Trange* range = &spin->range;
    if ( steps<0 ) range_dec(range);
    if ( steps>0 ) range_inc(range);
    setValue(range->val);
}

void TGUI_spinBoxInt::valueValidated() {
    Trange* range = &spin->range;
    if (  spin->cbVal ) {
        range_setVal(range,value());
        if (spin->enabled) spin->cbVal(spin,spin->cbUd,value());
    }
}
void TGUI_spinBoxInt::valueChanged(int) {
    Trange* range = &spin->range;
    if (  spin->cbChg ) {
        range_setVal(range,value());
        if (spin->enabled) spin->cbChg(spin,spin->cbUd,value());
    }
}

/*======================================================================*/
