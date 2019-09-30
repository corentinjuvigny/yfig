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
 *      $Id: vgui-names.c 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-names.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= widget name                                                        =*/
/*=                                                                    =*/
/*= See the vgui/vgui-names.h header for the type description, the     =*/
/*= function description and the documentation.                        =*/
/*======================================================================*/

// defines global variables here
#define NoExtern_WIDGET_NAMES_H

#include "vgui-config.h"
#include "vgui/vgui-names.h"
#include "vgui/vgui.h"

#include "tools/color.h"

/*======================================================================*/
#define BIG_TEST(kind) \
        if ( strcmp(wn+prefixlen,"Label")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundLAB;      \
            *bg = gui->vg_##kind##BackgroundLAB;      \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"BtnOrn")==0 ) {     \
            *fg = gui->vg_##kind##ForegroundBtnOrn;   \
            *bg = gui->vg_##kind##BackgroundBtnOrn;   \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"BtnStd")==0 ||      \
             strcmp(wn+prefixlen,"Spin")==0   ) {     \
            *fg = gui->vg_##kind##ForegroundBtnStd;   \
            *bg = gui->vg_##kind##BackgroundBtnStd;   \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"BtnOff")==0 ) {     \
            *fg = gui->vg_##kind##ForegroundBtnOff;   \
            *bg = gui->vg_##kind##BackgroundBtnOff;   \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"BtnOn")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundBtnOn;    \
            *bg = gui->vg_##kind##BackgroundBtnOn;    \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Menu")==0 ) {       \
            *fg = gui->vg_##kind##ForegroundMenu;     \
            *bg = gui->vg_##kind##BackgroundMenu;     \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"MenuBtn")==0 ) {    \
            *fg = gui->vg_##kind##ForegroundMenuBtn;  \
            *bg = gui->vg_##kind##BackgroundMenuBtn;  \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Slout")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundSlout;    \
            *bg = gui->vg_##kind##BackgroundSlout;    \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Slin")==0 ) {       \
            *fg = gui->vg_##kind##ForegroundSlin;     \
            *bg = gui->vg_##kind##BackgroundSlin;     \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Mlout")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundMlout;    \
            *bg = gui->vg_##kind##BackgroundMlout;    \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Mlin")==0 ) {       \
            *fg = gui->vg_##kind##ForegroundMlin;     \
            *bg = gui->vg_##kind##BackgroundMlin;     \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Panel")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundPanel;    \
            *bg = gui->vg_##kind##BackgroundPanel;    \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Canvas")==0 ) {     \
            *fg = gui->vg_##kind##ForegroundCanvas;   \
            *bg = gui->vg_##kind##BackgroundCanvas;   \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"List")==0 ) {       \
            *fg = gui->vg_##kind##ForegroundList;     \
            *bg = gui->vg_##kind##BackgroundList;     \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"ListDir")==0 ) {    \
            *fg = gui->vg_##kind##ForegroundListDir;  \
            *bg = gui->vg_##kind##BackgroundListDir;  \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"ListFile")==0 ) {   \
            *fg = gui->vg_##kind##ForegroundListFile; \
            *bg = gui->vg_##kind##BackgroundListFile; \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Shell")==0 ) {      \
            *fg = gui->vg_##kind##ForegroundShell;    \
            *bg = gui->vg_##kind##BackgroundShell;    \
            return;                                   \
        }                                             \
        if ( strcmp(wn+prefixlen,"Top")==0 ) {        \
            *fg = gui->vg_##kind##ForegroundTop;      \
            *bg = gui->vg_##kind##BackgroundTop;      \
            return;                                   \
        }

/*======================================================================*/

extern void           vgui_wn_getColor   (Cvgui*gui, CstrBrief wn,
                                          Ccolor** fg, Ccolor** bg)
{
    const char* prefix;
    int   prefixlen;
    
    prefix="dial"; prefixlen=strlen(prefix);
    if ( strncmp(prefix,wn,prefixlen)==0 ) {
        BIG_TEST(dial)
        if ( strcmp(wn+prefixlen,"Sub")==0 ) {
            *fg = gui->vg_dialForegroundSub;
            *bg = gui->vg_dialBackgroundSub;
            return;
        }
        if ( strcmp(wn+prefixlen,"SubTitle")==0 ) {
            *fg = gui->vg_dialForegroundSubTitle;
            *bg = gui->vg_dialBackgroundSubTitle;
            return;
        }
        TEMP_spe_printf(
                "%s unrecognized dialog widget name, "
                "fg and bg set to vg_dialogForeground & vg_dialogBackground\n",
                wn);
        *fg = gui->vg_dialForegroundTop;
        *bg = gui->vg_dialBackgroundTop;
        return;
    }

    prefix="main"; prefixlen=strlen(prefix);
    if ( strncmp(prefix,wn,prefixlen)==0 ) {
        BIG_TEST(main)
        TEMP_spe_printf(
            "%s unrecognized main widget name, "
            "fg and bg set to vg_foreground & vg_background\n",
            wn);
        *fg = gui->vg_mainForegroundTop;
        *bg = gui->vg_mainBackgroundTop;
        return;
    }
    TEMP_spe_printf(
            "%s unrecognized widget name, "
            "fg and bg set to vg_foreground & vg_background\n",
            wn);
    *fg = gui->vg_foreground;
    *bg = gui->vg_background;
}

extern void        vgui_wn_getColorEGS(Cvgui*gui, CstrBrief wn,
                                        Tegs_color* fg,  Tegs_color* bg)
{
    Ccolor *fgColor,*bgColor;
    vgui_wn_getColor(gui,wn, &fgColor, &bgColor);
    *fg = fgColor->egs; 
    *bg = bgColor->egs; 
}

/*======================================================================*/

extern Ccolor*        vgui_wn_getFgColor   (Cvgui*gui, CstrBrief wn)
{
    Ccolor *fg,*bg;
    vgui_wn_getColor(gui,wn,&fg,&bg);
    return fg;
}

extern Ccolor*        vgui_wn_getBgColor   (Cvgui*gui, CstrBrief wn)
{
    Ccolor *fg,*bg;
    vgui_wn_getColor(gui,wn,&fg,&bg);
    return bg;
}

extern Tegs_color     vgui_wn_getFgColorEGS(Cvgui*gui, CstrBrief wn)
{
    Tegs_color fg,bg;
    vgui_wn_getColorEGS(gui,wn,&fg,&bg);
    return fg;
}

extern Tegs_color     vgui_wn_getBgColorEGS(Cvgui*gui, CstrBrief wn)
{
    Tegs_color fg,bg;
    vgui_wn_getColorEGS(gui,wn,&fg,&bg);
    return bg;
}

/*======================================================================*/
