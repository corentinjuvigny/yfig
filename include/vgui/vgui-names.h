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
 *      $Id: vgui-names.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-names.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * widgets names.
 *
 * Provides:
 *   - a string global variable for every widget kind.
 *   - this string called "[widget] name" is used to get default widget
 *     properties such as foreground and background colors.
 *   - functions to get default properties from the widget name.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_NAMES_H
#define FILE_VGUI_NAMES_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-types.h"

#ifdef NoExtern_WIDGET_NAMES_H
#   define YFIG_WN_EXTERN   
#   define YFIG_WN_DEF(val) const char* wn_##val = #val
#else
#   define YFIG_WN_EXTERN   extern
#   define YFIG_WN_DEF(val) extern const char* wn_##val
#endif

/*======================================================================*/
/*= widget names                                                       =*/

#ifdef NoExtern_WIDGET_NAMES_H
#   define YFIG_WN_DEF(val) const char* wn_##val = #val
#else
#   define YFIG_WN_DEF(val) extern const char* wn_##val
#endif

YFIG_WN_DEF(mainShell);
YFIG_WN_DEF(mainTop);
YFIG_WN_DEF(mainLabel);
YFIG_WN_DEF(mainBtnOrn);
YFIG_WN_DEF(mainBtnStd);
YFIG_WN_DEF(mainBtnOff);
YFIG_WN_DEF(mainBtnOn);
YFIG_WN_DEF(mainMenu);
YFIG_WN_DEF(mainMenuBtn);
YFIG_WN_DEF(mainSlout);
YFIG_WN_DEF(mainSlin);
YFIG_WN_DEF(mainMlout);
YFIG_WN_DEF(mainMlin);
YFIG_WN_DEF(mainSpin);
YFIG_WN_DEF(mainPanel);
YFIG_WN_DEF(mainCanvas);
YFIG_WN_DEF(mainList);
YFIG_WN_DEF(mainListDir);
YFIG_WN_DEF(mainListFile);

YFIG_WN_DEF(dialShell);
YFIG_WN_DEF(dialTop);
YFIG_WN_DEF(dialSub);
YFIG_WN_DEF(dialSubTitle);
YFIG_WN_DEF(dialLabel);
YFIG_WN_DEF(dialBtnOrn);
YFIG_WN_DEF(dialBtnStd);
YFIG_WN_DEF(dialBtnOff);
YFIG_WN_DEF(dialBtnOn);
YFIG_WN_DEF(dialMenu);
YFIG_WN_DEF(dialMenuBtn);
YFIG_WN_DEF(dialSlout);
YFIG_WN_DEF(dialSlin);
YFIG_WN_DEF(dialMlout);
YFIG_WN_DEF(dialMlin);
YFIG_WN_DEF(dialSpin);
YFIG_WN_DEF(dialPanel);
YFIG_WN_DEF(dialCanvas);
YFIG_WN_DEF(dialList);
YFIG_WN_DEF(dialListDir);
YFIG_WN_DEF(dialListFile);

/*======================================================================*/
/*= get default properties                                             =*/

extern void           vgui_wn_getColor   (Cvgui*gui, CstrBrief wn,
                                          Ccolor** fg, Ccolor** bg);
extern void           vgui_wn_getColorEGS(Cvgui*gui, CstrBrief wn,
                                        Tegs_color* fg,  Tegs_color* bg);

extern Ccolor*        vgui_wn_getFgColor   (Cvgui*gui, CstrBrief wn);
extern Ccolor*        vgui_wn_getBgColor   (Cvgui*gui, CstrBrief wn);
extern Tegs_color     vgui_wn_getFgColorEGS(Cvgui*gui, CstrBrief wn);
extern Tegs_color     vgui_wn_getBgColorEGS(Cvgui*gui, CstrBrief wn);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif //  FILE_VGUI_NAMES_H
/*======================================================================*/


