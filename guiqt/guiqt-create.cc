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
 *      $Id: guiqt-create.cc 63 2017-04-22 08:03:26Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-create.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for creating widgets.                                    =*/
/*======================================================================*/

#include "guiqt.h"
#include "guiqt/guiqt-create.h"

/*======================================================================*/
/*= container widgets                                                  =*/

extern Tegs_widget create_form(
        Tvgui* gui, Tegs_widget parent, const char* wn,
        int hvsep, int bw, int dx, int dy, TcolorEGS bg, TcolorEGS fg)
{
    QGridLayout* form = new QGridLayout((QWidget*)parent);
    //widget_setColors(gui,form,wn,bg,fg);  // form is not a widget
    form->setSpacing(hvsep>=0 ? hvsep : 2);
    return form;
}

/*======================================================================*/
