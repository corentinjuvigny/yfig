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
 *      $Id: vdw-pum.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-pum.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= PUM implementation (PopUp Message dialog).                         =*/
/*= See header wid-basewidget.h for function documentation.            =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/wid-simple.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/

typedef struct _Twid_pum {
    Tvgui_dw    any;
    Tegs_widget messWidEGS;
} Twid_pum;

/*======================================================================*/

extern Twid_dialStatus wid_pum_critical(Tvgui_mw*vmw,
                            Twid_dialStatus butts, CstrBrief fmt, ...)
{
    Tvgui* gui   = vgui_mw_getVGui(vmw);
    Cstr   title = "Critical Message";
    Twid_pum* pum = (Twid_pum*)vgui_dw_newTop( sizeof(*pum),
            vmw, tool_strdup(title), butts);
    DBG1_DwE("pum=%p title=%s fmt=%s",pum,title,fmt);

    va_list ap;
    va_start(ap,fmt);
    TstrEaten mess = tool_vaprintf(fmt,ap);
    va_end(ap);
    pum->messWidEGS = widD_label_nbf(&pum->any,mess,0);
    tool_free( mess );
    vgui_dw_finishIC(&pum->any,0,pum->messWidEGS,-1);

    int status;
    status = vgui_dw_run(&pum->any);
    vgui_dw_free(&pum->any);
    DBG1_DwL("pum=%p title=%s fmt=%s : return %08x (%s)",
            pum,title,fmt,status,vgui_dw_status2buttLab(status));

    return status;
}

extern Twid_dialStatus wid_pum_criticalWBL(Tvgui_mw*vmw, ...)
{
    Tvgui* gui   = vgui_mw_getVGui(vmw);
    Cstr   title = "Critical Message";
    Twid_pum* pum = (Twid_pum*)vgui_dw_newTop( sizeof(*pum),
            vmw, tool_strdup(title), 0);

    va_list ap;
    va_start(ap,vmw);
    vgui_dw_addCtlButtsVa(&pum->any,ap);
    Cstr fmt = va_arg(ap,Cstr);
    TstrEaten mess = tool_vaprintf(fmt,ap);
    va_end(ap);
    pum->messWidEGS = widD_label_nbf(&pum->any,mess,0);
    free( mess );
    vgui_dw_finishIC(&pum->any,0,pum->messWidEGS,-1);
    
    int status;
    status = vgui_dw_run(&pum->any);

    vgui_dw_free(&pum->any);
    DBG1_DwE("pum=%p title=%s fmt=%s",pum,title,fmt);

    return status;
}

/*======================================================================*/
