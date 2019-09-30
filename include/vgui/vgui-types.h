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
 *      $Id: vgui-types.h 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-types.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= type pre-definitions and main macros.                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_TYPES_H
#define FILE_VGUI_TYPES_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"
#include "tools/color-types.h"
#include "tools/bitmap-types.h"
#include "outdev/types.h"

/*======================================================================*/

#define GUI_ExtSys_XT     1
#define GUI_ExtSys_QT4    2
#define GUI_ExtSys_QT5    4
#define GUI_ExtSys_QT     (GUI_ExtSys_QT4|GUI_ExtSys_QT5)
#define GUI_ExtSys_GTK    8
#define GUI_ExtSys_MinGw 16

#define GUI_ExtSys_NONE   0 
#define GUI_ExtSys_ALL   -1

/*======================================================================*/

typedef void*       Tegs_widget;
typedef void*       Tegs_font;

typedef unsigned short   TaccelDef;

typedef struct _Tvgui    Tvgui;
typedef const   Tvgui    Cvgui;
extern Tmsgdrv* vgui_getMsgDrv(Cvgui*gui);
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_usridt_printf,vgui_usridt_vprintf,tool_usridt_vprintf,vgui_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_usrmsg_printf,vgui_usrmsg_vprintf,tool_usrmsg_vprintf,vgui_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_inferr_printf,vgui_inferr_vprintf,tool_inferr_vprintf,vgui_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_wrnerr_printf,vgui_wrnerr_vprintf,tool_wrnerr_vprintf,vgui_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_errerr_printf,vgui_errerr_vprintf,tool_errerr_vprintf,vgui_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui,vgui_ftlerr_printf,vgui_ftlerr_vprintf,tool_ftlerr_vprintf,vgui_getMsgDrv(o));

typedef struct _Tvgui_font    Tvgui_font;
typedef const   Tvgui_font    Cvgui_font;
typedef struct _Tvgui_fontSet Tvgui_fontSet;
typedef const   Tvgui_fontSet Cvgui_fontSet;

typedef struct _Tvgui_icon    Tvgui_icon;
typedef const   Tvgui_icon    Cvgui_icon;
typedef struct _Tvgui_iconSet Tvgui_iconSet;
typedef const   Tvgui_iconSet Cvgui_iconSet;

typedef struct _Tvgui_efsm   Tvgui_efsm;
typedef const   Tvgui_efsm   Cvgui_efsm;

typedef struct _Tvgui_outdev Tvgui_outdev;
typedef const   Tvgui_outdev Cvgui_outdev;

typedef struct _Tvgui_odcanvas Tvgui_odcanvas;
typedef const   Tvgui_odcanvas Cvgui_odcanvas;

typedef struct _Tvgui_odpixmap Tvgui_odpixmap;
typedef const   Tvgui_odpixmap Cvgui_odpixmap;

typedef struct _Tvgui_mw     Tvgui_mw;
typedef const   Tvgui_mw     Cvgui_mw;
extern Tvgui*      vgui_mw_getVGui  (Cvgui_mw*vmw);
extern Tegs_widget vgui_mw_getWidEGS(Cvgui_mw*vmw);
extern Tmsgdrv*    vgui_mw_getMsgDrv(Cvgui_mw*vmw);
extern Tvgui_mw*   vgui_mw_newDummy (Tvgui*   gui);
extern void        vgui_mw_delDummy (Tvgui_mw* mv);
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_usridt_printf,vgui_mw_usridt_vprintf,tool_usridt_vprintf,vgui_mw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_usrmsg_printf,vgui_mw_usrmsg_vprintf,tool_usrmsg_vprintf,vgui_mw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_inferr_printf,vgui_mw_inferr_vprintf,tool_inferr_vprintf,vgui_mw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_wrnerr_printf,vgui_mw_wrnerr_vprintf,tool_wrnerr_vprintf,vgui_mw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_errerr_printf,vgui_mw_errerr_vprintf,tool_errerr_vprintf,vgui_mw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_mw,vgui_mw_ftlerr_printf,vgui_mw_ftlerr_vprintf,tool_ftlerr_vprintf,vgui_mw_getMsgDrv(o));
 
typedef struct _Tvgui_dw     Tvgui_dw;
typedef const   Tvgui_dw     Cvgui_dw;
extern Tvgui*      vgui_dw_getVGui  (Cvgui_dw*vdw);
extern Tvgui_mw*   vgui_dw_getMw    (Cvgui_dw*vdw);
extern Tegs_widget vgui_dw_getWidEGS(Cvgui_dw*vdw);
extern Tmsgdrv*    vgui_dw_getMsgDrv(Cvgui_dw*vdw);
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_usridt_printf,vgui_dw_usridt_vprintf,tool_usridt_vprintf,vgui_dw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_usrmsg_printf,vgui_dw_usrmsg_vprintf,tool_usrmsg_vprintf,vgui_dw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_inferr_printf,vgui_dw_inferr_vprintf,tool_inferr_vprintf,vgui_dw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_wrnerr_printf,vgui_dw_wrnerr_vprintf,tool_wrnerr_vprintf,vgui_dw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_errerr_printf,vgui_dw_errerr_vprintf,tool_errerr_vprintf,vgui_dw_getMsgDrv(o));
TOOL_MSGDRV_FandVF_redirect(Cvgui_dw,vgui_dw_ftlerr_printf,vgui_dw_ftlerr_vprintf,tool_ftlerr_vprintf,vgui_dw_getMsgDrv(o));
 
typedef struct _Twid_canvas  Twid_canvas;
typedef const   Twid_canvas  Cwid_canvas;
extern Tegs_widget   wid_canvas_getWidEGS(Cwid_canvas*self);
extern Tvgui_efsm*   wid_canvas_getEFsm  (Cwid_canvas*self);
extern Tvgui_outdev* wid_canvas_getVOD   (Cwid_canvas*self);
extern Toutdev*      wid_canvas_getOD    (Cwid_canvas*self);
#define wid_canvas_getVDev  wid_canvas_getVOD

typedef struct _Twid_button Twid_button;
typedef const   Twid_button Cwid_button;
typedef void  (*Twid_button_cbDw)  (Tvgui_dw*vdw, int btnId, void*ud);
typedef void  (*Twid_button_cbMw)  (Tvgui_mw*vmw, int btnId, void*ud);
typedef void  (*Twid_button_free)  (Tvgui*gui,    void*ud);
extern Tvgui*      wid_button_getVGui  (Cwid_button*self);
extern Tegs_widget wid_button_getWidEGS(Cwid_button*self);
extern Tvgui_mw*   wid_button_getMw    (Cwid_button*self);
extern Tvgui_dw*   wid_button_getDw    (Cwid_button*self);
typedef struct _Twid_buttonPanel Twid_buttonPanel;
typedef const   Twid_buttonPanel Cwid_buttonPanel;
extern Tvgui*      wid_buttonPanel_getVGui   (Cwid_buttonPanel*self);
extern Tegs_widget wid_buttonPanel_getWidEGS (Cwid_buttonPanel*self);
extern Tvgui_mw*   wid_buttonPanel_getMw     (Cwid_buttonPanel*self);
//extern Tegs_widget wid_buttonPanel_getPWidEGS(Cwid_buttonPanel*self);

typedef struct _Twid_olvu  Twid_olvu;
typedef const   Twid_olvu  Cwid_olvu;
typedef struct _Twid_ilvu  Twid_ilvu;
typedef const   Twid_ilvu  Cwid_ilvu;

typedef struct _Twidm_entry  Twidm_entry;
typedef const   Twidm_entry  Cwidm_entry;
typedef struct _Twidm_any    Twidm_any;
typedef const   Twidm_any    Cwidm_any;
typedef struct _Twidm_popup  Twidm_popup;
typedef const   Twidm_popup  Cwidm_popup;
typedef struct _Twidm_ddl    Twidm_ddl;
typedef const   Twidm_ddl    Cwidm_ddl;
typedef struct _Twidm_bar    Twidm_bar;
typedef const   Twidm_bar    Cwidm_bar;

typedef struct _Twid_filebrowser Twid_filebrowser;
typedef const   Twid_filebrowser Cwid_filebrowser;

typedef struct _Tvdw_console Tvdw_console;
typedef const   Tvdw_console Cvdw_console;

typedef struct _Twid_mousefun   Twid_mousefun;
typedef const   Twid_mousefun   Cwid_mousefun;
typedef struct _Twid_mousefunDt Twid_mousefunDt;
typedef const   Twid_mousefunDt Cwid_mousefunDt;
typedef struct _Twid_mousefun3S Twid_mousefun3S;
typedef const   Twid_mousefun3S Cwid_mousefun3S;
struct _Twid_mousefun3S { const char* str[3]; }; // left, middle right labels

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_TYPES_H
/*======================================================================*/

