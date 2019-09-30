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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-event.h 62 2017-04-15 15:05:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guixt/guixt-event.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities to add event callback to a widget.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_EVENT_H
#define FILE_GUIXT_EVENT_H
/*======================================================================*/

/*======================================================================*/
/* Event definition.                                                   =*/

#define YFIG_EVENT_CancelSaveDots   "0"            // cb( std_param , Atom a )
#define YFIG_EVENT_ResizeEvent      "1"            // cb( std_param , XEvent* e )
#define YFIG_EVENT_VisibilityEvent  "2"            // cb( std_param , XEvent* e )

#define YFIG_EVENT_KeyReturnPressed "<Key>Return"
#define YFIG_EVENT_Button123Pressed "<Btn1Down>,<Btn2Down>,<Btn3Down>"
#define YFIG_EVENT_Button1Pressed   "<Btn1Down>"
#define YFIG_EVENT_Button2Pressed   "<Btn2Down>"
#define YFIG_EVENT_Button3Pressed   "<Btn3Down>"
#define YFIG_EVENT_Map              "<MapNotify>"
#define YFIG_EVENT_Expose           "<Expose>"

/*======================================================================*/
/* Interface.                                                          =*/

/**
 * The function widget_cb_init must be called once before using
 * the other functions widget_cb_xxx.
**/
extern void widget_cb_init (Tvgui*gui);

/**
 * The format of the ... parameter list is:
 *   e0,cb0, e1,cb1, ..., NULL
 *
 * The widget_cb_add/U/UU function adds the callbacks cbi for the
 * event ei to the widget w.
 * So after this setting, when the event ei occurs the function cbi(ud)
 * or cbi(ud1,ud2) is performed.
 *
 * The events are standard Xt events like "<ConfigureNotify>". Most of
 * them are given by a YFIG_EVENT_xxx macros.
 *
 * Caution: The ... list must not exceed 20 entries.
**/
extern void widget_cb_addU (Widget w, void* ud, ...);
extern void widget_cb_addUU(Widget w, void* ud1, void* ud2, ...);

/*======================================================================*/
#endif // FILE_GUIXT_EVENT_H
/*======================================================================*/
