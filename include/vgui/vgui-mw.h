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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-mw.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-mw.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data structure & interface for main widgets.                       =*/
/*=                                                                    =*/
/*= The Twid_mw type contains the information that the vgui system     =*/
/*= requires for:                                                      =*/
/*=   - getting GUI and EGS parameters.                                =*/
/*=   - handling a canvas widget                                       =*/
/*=   - handling a mouse fun widget                                    =*/
/*=   - handling a message widget for printing notice, warning or      =*/
/*=     error messages.                                                =*/
/*=                                                                    =*/
/*= There are neither constructor nor destructor, the sub class        =*/
/*= must initialized it.                                               =*/
/*=   struct _Tygui_mw {                                               =*/
/*=      Tvgui_mw any;  // the common part of main widget              =*/
/*=      ...      ...;  // specialized fields od dub class             =*/
/*=   };                                                               =*/
/*=                                                                    =*/
/*= Notes:                                                             =*/
/*=   - all the not required fields must be set to null.               =*/
/*=   - the only mandatory fields are: gui and widEGS.                 =*/
/*=   - the modal field must be initialized to zero and only the vgui  =*/
/*=     module must change it.                                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_MW_H
#define FILE_VGUI_MW_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================*/

struct _Tvgui_mw {
    Tvgui*       gui;      // It belongs to this gui
    Tegs_widget  widEGS;   // The container widgets

    // The next parameters are for handling modal dialog.
    // setInputEnable(vmw,false) is called and modal is set to 1 
    // before staring a modal dialog and then
    // setInputEnable(vmw,true) is called and modal is reset to 0
    // when the dialog finished.
    // The role of setInputEnable(vmw,false) is to disabled the keyboard
    // and mouse of the vmw input widgets so there callbacks are not called
    // during the modal time.
    // The modal boolean is for the input widget that can not be disabled
    // by setInputEnable. In this case, their callbacks must test the modal
    // boolean and do nothing if it is true.
    void    (*setInputEnable)(Tvgui_mw* vmw, int on); // 1/0:en/disable
    int      modal; // 1:modal dialog is running; 0:otherwise

    // The message widget (type slout) of the main widget if any or null
    Tegs_widget     message;
    Tmsgdrv*        msgdrv;
#define MW_message  message
#define vmw_message any.MW_message
#define MW_msgdrv   msgdrv
#define vmw_msgdrv  any.MW_msgdrv
};

// macros to use in vgui
#define MW_gui            gui
#define MW_widEGS         widEGS
#define MW_setInputEnable setInputEnable
#define MW_modal          modal

// macros to use in Tvgui_mw subclass
#define MW_2MW(mw)        &mw->any
#define mw_gui            any.MW_gui
#define mw_widEGS         any.MW_widEGS
#define mw_setInputEnable any.MW_setInputEnable
#define mw_modal          any.MW_modal

/*======================================================================*/
/*= Interface for printing messages                                    =*/     

extern void vgui_mw_msgdrv_initWidLvl(Tvgui_mw* vmw,
                                      Tegs_widget msgwid, int lvl);
extern void vgui_mw_msgdrv_free      (Tvgui_mw* vmw);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_MW_H
/*======================================================================*/
