/*======================================================================
 *  Copyright 2018 Corentin Juvigny
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
 *$Software: YFIG-GTK (VGUI driver to Gtk)
 *      $Id: guixt-font-pango.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-canvas.h $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups the features for handling drawing and erase    =*/
/*= sections into pixmap and canvas output devices.                    =*/
/*=                                                                    =*/
/*= Pixmap output device (Tvgui_odpixmap):                             =*/
/*=   The Tvgui_odpixmap contains                                      =*/
/*=     - vv_target: a cairo_surface_t where drawing are done.         =*/
/*=     - vv_pixmap: a GdkPixbuf the picture of which is synchronizes  =*/
/*=                  with the vv_target surfaces.                      =*/
/*=   For this output device only the standard section is implemented  =*/
/*=   (od_ds_stdStart ... od_ds_stdCommit).                            =*/
/*=   Inside the section vv_target and vv_pixmap are not synchronized, =*/
/*=   od_ds_stdCommit synchronizes them again.                         =*/
/*=                                                                    =*/
/*= Canvas output device (Tvgui_odcanvas):                             =*/
//FIXME: refaire les commentaires et faire une section.
// widget_chgSurface destroys the surface associated to the gtkwid widget.
// widget_getSurface returns a surface that has the same pixel size than
// the gtkwid widget.
// 2 consecutive calls return the same surface.
//   s1 = widget_getSurface(w); // if it doesn't exist, it is created.
//   s2 = widget_getSurface(w); // s2 and s1 point to the same surface.
//   widget_chgSurface(w);      // s1 and s2 are dead pointers.
//   s3 = widget_getSurface(w); // s3 is a new surface different than
//                              // the previous s1 and s2.
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDGTK_CANVAS_H
#define FILE_WIDGTK_CANVAS_H
/*======================================================================*/

#include "vgui/vgui-outdev.h"
#include "outdev/outdev.h"

/*======================================================================*/
/*= Features for Tvgui_odpixmap.                                       =*/

// odpxm_stdSeq_isStarted returns
//  - 1 : we are inside  the standard drawing sequence.
//  - 0 : we are outside the standard drawing sequence.
Inline void odpxm_stdSeq_start    (GdkPixbuf *pxm) {        g_object_set_data(G_OBJECT(pxm),"ds",(void*)0x1); }
Inline void odpxm_stdSeq_commit   (GdkPixbuf *pxm) {        g_object_set_data(G_OBJECT(pxm),"ds",(void*)0x0); }
Inline int  odpxm_stdSeq_isStarted(GdkPixbuf *pxm) { return g_object_get_data(G_OBJECT(pxm),"ds")!=0 ? 1 : 0; }

/*======================================================================*/
/*= Features for Tvgui_odcanvas.                                       =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Data structure.                                                    =*/

typedef struct _Tgui_daData Tgui_daData;
typedef const   Tgui_daData Cgui_daData;

struct _Tgui_daData { // Drawing Area Data
    cairo_t *main_gc; //associated gc to device (written when in_ds is 0)
    cairo_surface_t* main_gc_cpy[OD_DS_NBXOR]; //a array of gc surfaces' copies
    cairo_t *xor_gc; //gc where we write when in_ds is 1
    int in_ms;  // 1 : between stdDrawStart  and stdDrawCommit
    int in_xds; // 1 : between xorDrawStart  and xorDrawCommit
    int in_xes; // 1 : between xorEraseStart and xorEraseCommit
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Interface: constructor & destructor.                               =*/
/*=                                                                    =*/
/*= The odcvs_getOdData function returns the Tgui_daData structure     =*/
/*= that is associated to the w widget.                                =*/
/*= If it does not exist, it is created.                               =*/
/*= Two consecutive calls return the same pointer.                     =*/
/*=                                                                    =*/
/*= The odcvs_chgOdData function frees all the memory amount of the    =*/
/*= Tgui_daData structure that is associated to the w widget.          =*/
/*= If the associated Tgui_daData structure does not exist, the        =*/
/*= function returns immediately.                                      =*/

extern Tgui_daData* odcvs_getOdData(GtkWidget* w);
extern void         odcvs_chgOdData(GtkWidget* w);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Interface: enter and leave sections.                               =*/

extern void odvt_ds_stdStartCvs      (const void*dev);
extern void odvt_ds_stdCommitCvs     (const void*dev);

extern void odvt_ds_xorDrawStartCvs  (const void*dev, int sn);
extern void odvt_ds_xorDrawCommitCvs (const void*dev, int sn);

extern void odvt_ds_xorEraseStartCvs (const void*dev, int sn);
extern void odvt_ds_xorEraseCommitCvs(const void*dev, int sn);

/*======================================================================*/
#endif // FILE_WIDGTK_CANVAS_H
/*======================================================================*/
