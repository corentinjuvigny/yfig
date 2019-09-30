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
 *      $Id: vdw-title.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vdw-title.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog defining the title of a dialog.                         =*/
/*=                                                                    =*/
/*= Of course this dialog must not have a title.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VDW_TITLE_H
#define FILE_VDW_TITLE_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-dw.h"
#include "wid-simple.h"

/*======================================================================*/

typedef struct _Tvdw_title Tvdw_title;
typedef const   Tvdw_title Cvdw_title;

struct _Tvdw_title {
    Tvgui_dw     any;
    TstrEaten    title;     // the dialog title
    Twid_button* leftFUF;
    Tegs_widget  label;
    Tegs_widget  extra;
    Twid_button* rightFUF;
};

/*======================================================================*/

extern Tvgui_dw* vdw_title_nbx   (Tvgui_dw* vdw, CstrStatic cwn, TstrEaten str);
extern void      vdw_title_finish(Tvgui_dw* vdw);
extern void      vdw_title_delete(Tvgui_dw* vdw);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VDW_TITLE_H
/*======================================================================*/
