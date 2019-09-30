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
 *$Software: YFIG-WIN32 (Interface to WIN32 EGS under MinGw)
 *      $Id: guimingw-widget.h 337 2018-11-05 22:44:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guimingw/guimingw-widget.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups:                                               =*/
/*=   - the EGS widget initialization                                  =*/
/*=   - the widget features that are common to all the widgets.        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIMINGW_WIDGET_H
#define FILE_GUIMINGW_WIDGET_H
/*======================================================================*/

/*======================================================================*/
/*= Widget class and regitration.                                      =*/

#define MINGW_WC_Spinner "ClassSpinner"
extern ATOM mingw_spinner_register(Tvgui* gui);

/*======================================================================*/
/*= Widget procdure.                                                   =*/

#define MINGW_WidProc_Symbol(bn) mingw_WidProc_##bn
#define MINGW_WidProc_Declaration(bn) \
    static LRESULT CALLBACK MINGW_WidProc_Symbol(bn) \
    (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

/*======================================================================*/
#endif // FILE_GUIMINGW_TOP_H
/*======================================================================*/
