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
 *      $Id: vdw-viewer.h 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vdw-viewer.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widgets for viewing and/or modifying entities:              =*/
/*=                                                                    =*/
/*=  - Tdble2Set                                                       =*/
/*=                                                                    =*/
/*= See the wid-dialog.h header for function documentation.            =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_VDW_VIEWER_H
#define FILE_VGUI_VDW_VIEWER_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-types.h"

/*======================================================================*/

#define VDWVIEW_OPT_FUF     0x0001
#define VDWVIEW_OPT_FUFLABL 0x0003
#define VDWVIEW_OPT_FUFLABR 0x0005
#define VDWVIEW_OPT_FUFPXML 0x0009
#define VDWVIEW_OPT_FUFPXMR 0x0011

#define VDWVIEW_OPT_UNIT(uu) ((uu)<<8 & 0x00ff00)
#define VDWVIEW_OPT_UNITnone VDWVIEW_OPT_UNIT(LENUNIT_ID_None)
#define VDWVIEW_OPT_UNITget(opt) ((char)(((opt)>>8)&0xFF))

#define VDWVIEW_OPT_UnitDdl 0x0020

typedef void (*Tvdw_view_cb)(Tvgui_dw*self, void* ud, ...);

/*======================================================================*/
/*= Tdble2Set viewver                                                  =*/

typedef struct _Tvdw_viewpts Tvdw_viewpts;
typedef const   Tvdw_viewpts Cvdw_viewpts;

/**
 * The vdw_viewpts_nbf function creates a sub dialog that allows
 * a user to view and/or to modify a point set.
 * The view only mode is enabled by setting the 3 callback (add,
 * sup and chg) to NULL.
 * When a callback is defined and the user does the corresponding
 * action, the widget performs it:
 *   cb(self,u-d,idx);
 * Where self is the dialog itself, u-d the ud parameter and idx the
 * index in the pts point set of the point that the user modified.
 *
 * The option parameter allows to set options:
 *   - VDWVIEW_OPT_FUF...  : to enable a show/unshow button in title bar.
 *   - VDWVIEW_OPT_UNIT(uu): to set the unit of displayed points to uu,
 *                           and the set point unit to LENUNIT_ID_FU.
 *   - VDWVIEW_OPT_UNITnone: must be set if the previous option is
 *                           not present.
 *   - VDWVIEW_OPT_UnitDdl : add a unit DDL to control the unit.
**/

extern Tvdw_viewpts* vdw_viewpts_nbf(Tvgui_dw* vdw, CstrBrief title,
                                     Tdble2Set*     pts,
                                     Tvdw_view_cb   add,
                                     Tvdw_view_cb   sup,
                                     Tvdw_view_cb   chg,
                                     void*          ud,
                                     int            opt);
extern void        vdw_viewpts_free(Tvdw_viewpts* self);

extern Tegs_widget vdw_viewpts_widEGS(Cvdw_viewpts* self);
extern Tdble2Set*  vdw_viewpts_pts   (Cvdw_viewpts* self);
extern int         vdw_viewpts_chgIdx(Cvdw_viewpts* self); // index of changed point

extern Tdble2      vdw_viewpts_getDblePt (Tvdw_viewpts* self, int idx);
extern void        vdw_viewpts_setDblePt (Tvdw_viewpts* self, int idx, Tdble2 pt);

extern void        vdw_viewpts_setUnit   (Tvdw_viewpts* self, int uu);
extern void        vdw_viewpts_setMinNb  (Tvdw_viewpts* self, int nb);
extern void        vdw_viewpts_setMaxNb  (Tvdw_viewpts* self, int nb);
extern void        vdw_viewpts_redraw    (Tvdw_viewpts* self);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_VGUI_VDW_VIEWER_H
/*======================================================================*/
