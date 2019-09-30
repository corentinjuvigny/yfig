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
 *      $Id: vgui-resources-table.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-resources-table.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource table of the Virtual GUI.                             =*/
/*=     TappResources gui_appResources = { ... }                       =*/
/*= It must be included ONLY ONCE by the GUI driver system.            =*/
/*=                                                                    =*/
/*= Resources are defined in the "vgui-resources-macros.h" header.     =*/
/*= Resources API is defined the "vgui-resources.h" header.            =*/
/*======================================================================*/

#include "vgui-resources.h"

/*======================================================================*/

#ifndef FIG_COLOR_RGB 
#define FIG_COLOR_RGB(r,g,b) "rgb:"#r"/"#g"/"#b
#endif

/**
 * ADD_DEF formats the resource according to external graphic system
**/
#if defined(USE_ResourceX11) && USE_ResourceX11 == 1
#   define ADD_DEFr(r,v) { VGUI_ResN_##r, VGUI_ResC_##r, #v, 0 }
#   define ADD_DEFs(r,v) { VGUI_ResN_##r, VGUI_ResC_##r,  v, 0 }
#elif defined(USE_ResourceQT) && USE_ResourceQT==1
#   warning Qt based resource mannager not yet implemented.
#   define ADD_DEFr(r,v) { VGUI_ResN_##r, 0, #v, 0 }
#   define ADD_DEFs(r,v) { VGUI_ResN_##r, VGUI_ResC_##r,  v, 0 }
#else
#   warning No resource mannager defined.
#   define ADD_DEFr(r,v) { VGUI_ResN_##r, 0, #v, 0 }
#   define ADD_DEFs(r,v) { VGUI_ResN_##r, VGUI_ResC_##r,  v, 0 }
#endif

/*======================================================================*/

static Tresource resource_table[] = { 
  ADD_DEFr( SmallIcons           , no                      ),
  ADD_DEFs( Foreground           , FIG_COLOR_RGB(20,20,20) ),
  ADD_DEFs( Background           , FIG_COLOR_RGB(CC,CC,CC) ),

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
  ADD_DEFs( MainForegroundShell   , "black"  ),
  ADD_DEFs( MainBackgroundShell   , "gray80" ),
  ADD_DEFs( MainForegroundTop     , "black"  ),
  ADD_DEFs( MainBackgroundTop     , "gray80" ),
  ADD_DEFs( MainForegroundLAB     , "black"  ),
  ADD_DEFs( MainBackgroundLAB     , "gray80" ),
  ADD_DEFs( MainForegroundBtnOrn  , "black"  ),
  ADD_DEFs( MainBackgroundBtnOrn  , "gray80" ),
  ADD_DEFs( MainForegroundBtnStd  , "black"  ),
  ADD_DEFs( MainBackgroundBtnStd  , "gray83" ),
  ADD_DEFs( MainForegroundBtnOff  , "black"  ),
  ADD_DEFs( MainBackgroundBtnOff  , "gray83" ),
  ADD_DEFs( MainForegroundBtnOn   , "black"  ),
  ADD_DEFs( MainBackgroundBtnOn   , "cyan"   ),
  ADD_DEFs( MainForegroundMenu    , "black"  ),
  ADD_DEFs( MainBackgroundMenu    , "gray80" ),
  ADD_DEFs( MainForegroundMenuBtn , "black"  ),
  ADD_DEFs( MainBackgroundMenuBtn , "gray88" ),
  ADD_DEFs( MainForegroundSlin    , "black"  ),
  ADD_DEFs( MainBackgroundSlin    , "linen"  ),
  ADD_DEFs( MainForegroundSlout   , "black"  ),
  ADD_DEFs( MainBackgroundSlout   , FIG_COLOR_RGB(FF,F8,EE) ),
  ADD_DEFs( MainForegroundMlin    , "black"  ),
  ADD_DEFs( MainBackgroundMlin    , "linen"  ),
  ADD_DEFs( MainForegroundMlout   , "black"  ),
  ADD_DEFs( MainBackgroundMlout   , FIG_COLOR_RGB(FF,F8,EE) ),
  ADD_DEFs( MainForegroundPanel   , "black"  ),
  ADD_DEFs( MainBackgroundPanel   , "gray70" ),
  ADD_DEFs( MainForegroundCanvas  , "black"  ),
  ADD_DEFs( MainBackgroundCanvas  , "gray70" ),
  ADD_DEFs( MainForegroundList    , "black"  ),
  ADD_DEFs( MainBackgroundList    , "gray90" ),
  ADD_DEFs( MainForegroundListDir , "black"  ),
  ADD_DEFs( MainBackgroundListDir , "gray90" ),
  ADD_DEFs( MainForegroundListFile, "black"  ),
  ADD_DEFs( MainBackgroundListFile, "gray95" ),

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
  ADD_DEFs( DialForegroundShell   , "black"  ),
  ADD_DEFs( DialBackgroundShell   , "gray85" ),
  ADD_DEFs( DialForegroundTop     , "black"  ),
  ADD_DEFs( DialBackgroundTop     , "gray85" ),
  ADD_DEFs( DialForegroundSub     , "black"  ),
  ADD_DEFs( DialBackgroundSub     , "gray85" ),
  ADD_DEFs( DialForegroundSubTitle, "black"  ),
  ADD_DEFs( DialBackgroundSubTitle, "gray60" ),
  ADD_DEFs( DialForegroundLAB     , "black"  ),
  ADD_DEFs( DialBackgroundLAB     , "gray80" ),
  ADD_DEFs( DialForegroundBtnOrn  , "black"  ),
  ADD_DEFs( DialBackgroundBtnOrn  , "gray80" ),
  ADD_DEFs( DialForegroundBtnStd  , "black"  ),
  ADD_DEFs( DialBackgroundBtnStd  , "gray80" ),
  ADD_DEFs( DialForegroundBtnOff  , "black"  ),
  ADD_DEFs( DialBackgroundBtnOff  , "gray80" ),
  ADD_DEFs( DialForegroundBtnOn   , "black"  ),
  ADD_DEFs( DialBackgroundBtnOn   , "cyan"   ),
  ADD_DEFs( DialForegroundMenu    , "black"  ),
  ADD_DEFs( DialBackgroundMenu    , "gray90" ),
  ADD_DEFs( DialForegroundMenuBtn , "black"  ),
  ADD_DEFs( DialBackgroundMenuBtn , "gray80" ),
  ADD_DEFs( DialForegroundSlin    , "black"  ),
  ADD_DEFs( DialBackgroundSlin    , "linen"  ),
  ADD_DEFs( DialForegroundSlout   , "black"  ),
  ADD_DEFs( DialBackgroundSlout   , FIG_COLOR_RGB(FF,F8,EE) ),
  ADD_DEFs( DialForegroundMlin    , "black"  ),
  ADD_DEFs( DialBackgroundMlin    , "linen"  ),
  ADD_DEFs( DialForegroundMlout   , "black"  ),
  ADD_DEFs( DialBackgroundMlout   , FIG_COLOR_RGB(FF,F8,EE) ),
  ADD_DEFs( DialForegroundPanel   , "black"  ),
  ADD_DEFs( DialBackgroundPanel   , "gray70" ),
  ADD_DEFs( DialForegroundCanvas  , "black"  ),
  ADD_DEFs( DialBackgroundCanvas  , "gray70" ),
  ADD_DEFs( DialForegroundList    , "black"  ),
  ADD_DEFs( DialBackgroundList    , "gray90" ),
  ADD_DEFs( DialForegroundListDir , "black"  ),
  ADD_DEFs( DialBackgroundListDir , "gray90" ),
  ADD_DEFs( DialForegroundListFile, "black"  ),
  ADD_DEFs( DialBackgroundListFile, "gray95" ),

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
  ADD_DEFs( DADrawColor          , FIG_COLOR_RGB(10,10,10) ),
  ADD_DEFs( DABackground         , FIG_COLOR_RGB(F0,F0,F0) ),
  ADD_DEFs( DAMissColor          , "gray90" ),
  ADD_DEFs( DAMissColorFill      , "lightcyan" ),
  ADD_DEFs( DASqueColor          , FIG_COLOR_RGB(80,80,80) ),
  ADD_DEFr( DAPrevColor          , cyan                    ),
  ADD_DEFs( DAShowColor          , FIG_COLOR_RGB(80,80,80) ),
  ADD_DEFr( MarkerColor          , green                   ),
  ADD_DEFr( MarkerHlColor        , red                     ),
  ADD_DEFr( MarkerHlColorFill    , cyan                    ),

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
  ADD_DEFs( GuiFont              , "times:11.0:75" ),
  ADD_DEFr( FontSize             , 12.0 ),

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
  ADD_DEFr( PageBorderForeground , lightblue               ),
  ADD_DEFr( AxisLineForeground   , hotpink                 ),
  ADD_DEFr( GridLineForeground   , lightpink               ),
} ; 

/*======================================================================*/
/*= The global resource set                                            =*/

TappResources gui_appResources = {
    sizeof( resource_table)/sizeof(resource_table[0]),
    resource_table
};

/*======================================================================*/
