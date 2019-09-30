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
 *      $Id: vgui-colors.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-colors.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Defines the color list used in the Virtual GUI.                    =*/
/*=                                                                    =*/
/*= Its format is a list of color definitions:                         =*/
/*=   VGUI_DO_COLOR_By...(black,black)                                 =*/
/*=   VGUI_DO_COLOR_By...(gray65,gray65,white)                         =*/
/*=   ...                                                              =*/
/*= There is 3 definition macros:                                      =*/
/*=   VGUI_DO_COLOR_ByNameF( <field-base-name>, <color-name> )         =*/
/*=   VGUI_DO_COLOR_ByNameD( <field-base-name>, <color-name>,          =*/
/*=                          <field-base-name for default> )           =*/
/*=   VGUI_DO_COLOR_ByResD(  <field-base-name>, <resource-name>,       =*/
/*=                          <field-base-name for default> )           =*/
/*=                                                                    =*/
/*= By default the 3 macros are defined for building the field list.   =*/
/*======================================================================*/

/*======================================================================*/

#ifndef VGUI_DO_COLOR_ByNameF
#define VGUI_DO_COLOR_ByNameF(f,cn) Tcolor* f;
#endif
#ifndef VGUI_DO_COLOR_ByNameD
#define VGUI_DO_COLOR_ByNameD(f,cn,fd) Tcolor* f;
#endif
#ifndef VGUI_DO_COLOR_ByResD
#define VGUI_DO_COLOR_ByResD(f,rn,fd) Tcolor* f;
#endif

/*======================================================================*/

// top color
VGUI_DO_COLOR_ByNameF(vg_white      , "white" )
VGUI_DO_COLOR_ByNameF(vg_trueblack  , "black" )
VGUI_DO_COLOR_ByNameD(vg_black      , "rgb:08/08/08" , vg_trueblack  )
VGUI_DO_COLOR_ByNameD(vg_grayDark   , "gray65"       , vg_black      )
VGUI_DO_COLOR_ByNameD(vg_grayMedian , "gray80"       , vg_white      )
VGUI_DO_COLOR_ByNameD(vg_grayLight  , "gray90"       , vg_white      )
                   
// fore/background color for the main widgets,
// they are the default for all the others.
VGUI_DO_COLOR_ByResD(vg_foreground , VGUI_ResN_Foreground       , vg_black )
VGUI_DO_COLOR_ByResD(vg_background , VGUI_ResN_Background       , vg_white )

// colors for widget in the main widgets
VGUI_DO_COLOR_ByResD(vg_mainForegroundShell   , VGUI_ResN_MainForegroundShell   , vg_foreground)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundShell   , VGUI_ResN_MainBackgroundShell   , vg_background)

VGUI_DO_COLOR_ByResD(vg_mainForegroundTop     , VGUI_ResN_MainForegroundTop     , vg_foreground)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundTop     , VGUI_ResN_MainBackgroundTop     , vg_background)

VGUI_DO_COLOR_ByResD(vg_mainForegroundLAB     , VGUI_ResN_MainForegroundLAB     , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundLAB     , VGUI_ResN_MainBackgroundLAB     , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundBtnOrn  , VGUI_ResN_MainForegroundBtnOrn  , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundBtnOrn  , VGUI_ResN_MainBackgroundBtnOrn  , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundBtnStd  , VGUI_ResN_MainForegroundBtnStd  , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundBtnStd  , VGUI_ResN_MainBackgroundBtnStd  , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundBtnOff  , VGUI_ResN_MainForegroundBtnOff  , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundBtnOff  , VGUI_ResN_MainBackgroundBtnOff  , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundBtnOn   , VGUI_ResN_MainForegroundBtnOn   , vg_mainBackgroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundBtnOn   , VGUI_ResN_MainBackgroundBtnOn   , vg_mainForegroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundMenu    , VGUI_ResN_MainForegroundMenu    , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundMenu    , VGUI_ResN_MainBackgroundMenu    , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundMenuBtn , VGUI_ResN_MainForegroundMenuBtn , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundMenuBtn , VGUI_ResN_MainBackgroundMenuBtn , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundSlin    , VGUI_ResN_MainForegroundSlin    , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundSlin    , VGUI_ResN_MainBackgroundSlin    , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundSlout   , VGUI_ResN_MainForegroundSlout   , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundSlout   , VGUI_ResN_MainBackgroundSlout   , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundMlin    , VGUI_ResN_MainForegroundMlin    , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundMlin    , VGUI_ResN_MainBackgroundMlin    , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundMlout   , VGUI_ResN_MainForegroundMlout   , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundMlout   , VGUI_ResN_MainBackgroundMlout   , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundPanel   , VGUI_ResN_MainForegroundPanel   , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundPanel   , VGUI_ResN_MainBackgroundPanel   , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundCanvas  , VGUI_ResN_MainForegroundCanvas  , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundCanvas  , VGUI_ResN_MainBackgroundCanvas  , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundList    , VGUI_ResN_MainForegroundList    , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundList    , VGUI_ResN_MainBackgroundList    , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundListDir , VGUI_ResN_MainForegroundListDir , vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundListDir , VGUI_ResN_MainBackgroundListDir , vg_mainBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_mainForegroundListFile, VGUI_ResN_MainForegroundListFile, vg_mainForegroundTop)
VGUI_DO_COLOR_ByResD(vg_mainBackgroundListFile, VGUI_ResN_MainBackgroundListFile, vg_mainBackgroundTop)

// colors for widget in the dialog widgets
VGUI_DO_COLOR_ByResD(vg_dialForegroundShell   , VGUI_ResN_DialForegroundShell   , vg_foreground)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundShell   , VGUI_ResN_DialBackgroundShell   , vg_background)

VGUI_DO_COLOR_ByResD(vg_dialForegroundTop     , VGUI_ResN_DialForegroundTop     , vg_foreground)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundTop     , VGUI_ResN_DialBackgroundTop     , vg_background)

VGUI_DO_COLOR_ByResD(vg_dialForegroundSub     , VGUI_ResN_DialForegroundSub     , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundSub     , VGUI_ResN_DialBackgroundSub     , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundSubTitle, VGUI_ResN_DialForegroundSubTitle, vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundSubTitle, VGUI_ResN_DialBackgroundSubTitle, vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundLAB     , VGUI_ResN_DialForegroundLAB     , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundLAB     , VGUI_ResN_DialBackgroundLAB     , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundBtnOrn  , VGUI_ResN_DialForegroundBtnOrn  , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundBtnOrn  , VGUI_ResN_DialBackgroundBtnOrn  , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundBtnStd  , VGUI_ResN_DialForegroundBtnStd  , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundBtnStd  , VGUI_ResN_DialBackgroundBtnStd  , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundBtnOff  , VGUI_ResN_DialForegroundBtnOff  , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundBtnOff  , VGUI_ResN_DialBackgroundBtnOff  , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundBtnOn   , VGUI_ResN_DialForegroundBtnOn   , vg_dialBackgroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundBtnOn   , VGUI_ResN_DialBackgroundBtnOn   , vg_dialForegroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundMenu    , VGUI_ResN_DialForegroundMenu    , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundMenu    , VGUI_ResN_DialBackgroundMenu    , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundMenuBtn , VGUI_ResN_DialForegroundMenuBtn , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundMenuBtn , VGUI_ResN_DialBackgroundMenuBtn , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundSlin    , VGUI_ResN_DialForegroundSlin    , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundSlin    , VGUI_ResN_DialBackgroundSlin    , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundSlout   , VGUI_ResN_DialForegroundSlout   , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundSlout   , VGUI_ResN_DialBackgroundSlout   , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundMlin    , VGUI_ResN_DialForegroundMlin    , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundMlin    , VGUI_ResN_DialBackgroundMlin    , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundMlout   , VGUI_ResN_DialForegroundMlout   , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundMlout   , VGUI_ResN_DialBackgroundMlout   , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundPanel   , VGUI_ResN_DialForegroundPanel   , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundPanel   , VGUI_ResN_DialBackgroundPanel   , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundCanvas  , VGUI_ResN_DialForegroundCanvas  , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundCanvas  , VGUI_ResN_DialBackgroundCanvas  , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundList    , VGUI_ResN_DialForegroundList    , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundList    , VGUI_ResN_DialBackgroundList    , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundListDir , VGUI_ResN_DialForegroundListDir , vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundListDir , VGUI_ResN_DialBackgroundListDir , vg_dialBackgroundTop)

VGUI_DO_COLOR_ByResD(vg_dialForegroundListFile, VGUI_ResN_DialForegroundListFile, vg_dialForegroundTop)
VGUI_DO_COLOR_ByResD(vg_dialBackgroundListFile, VGUI_ResN_DialBackgroundListFile, vg_dialBackgroundTop)

// Canvas color
VGUI_DO_COLOR_ByResD(vg_daDrawColor       , VGUI_ResN_DADrawColor      , vg_black      ) // default color for drawing
VGUI_DO_COLOR_ByResD(vg_daBackground      , VGUI_ResN_DABackground     , vg_black      ) // Drawing Area background
VGUI_DO_COLOR_ByResD(vg_daMissColor       , VGUI_ResN_DAMissColor      , vg_black      ) // for drawing in OD_GC_UsrMiss GC
VGUI_DO_COLOR_ByResD(vg_daMissColorFill   , VGUI_ResN_DAMissColorFill  , vg_black      ) // for filling in OD_GC_UsrMiss GC
VGUI_DO_COLOR_ByResD(vg_daSqueColor       , VGUI_ResN_DASqueColor      , vg_grayMedian ) // for showing object squeletons
VGUI_DO_COLOR_ByResD(vg_daPrevColor       , VGUI_ResN_DAPrevColor      , vg_grayDark   ) // for showing object previews
VGUI_DO_COLOR_ByResD(vg_daShowColor       , VGUI_ResN_DAShowColor      , vg_grayMedian ) // for showing something other than squeletons, previews and objects
VGUI_DO_COLOR_ByResD(vg_markerColor       , VGUI_ResN_MarkerColor      , vg_grayMedian ) // for drawing marker
VGUI_DO_COLOR_ByResD(vg_markerHlColor     , VGUI_ResN_MarkerHlColor    , vg_grayDark   ) // for drawing highlighted marker
VGUI_DO_COLOR_ByResD(vg_markerHlColorFill , VGUI_ResN_MarkerHlColorFill, vg_black      ) // for filling highlighted marker

// miscellaneous foreground color
VGUI_DO_COLOR_ByResD(vg_pageBorderForeground , VGUI_ResN_PageBorderForeground , vg_grayLight  ) // for showing paper size
VGUI_DO_COLOR_ByResD(vg_axisLineForeground   , VGUI_ResN_AxisLineForeground   , vg_grayMedian ) // for showing axis
VGUI_DO_COLOR_ByResD(vg_gridLineForeground   , VGUI_ResN_GridLineForeground   , vg_grayMedian ) // for showing grid

/*======================================================================*/
// vim: set textwidth=120
