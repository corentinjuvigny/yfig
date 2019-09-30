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
 *$Software: YFIG
 *      $Id: yfig-device.h 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-device.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= YFIG device (spezialization of Tvgui_odcanvas device)             =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_DEVICE_H
#define FILE_YFIG_DEVICE_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-types.h"

#include "vgui/vgui-outdev.h"
#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "fig/object.h"

/*======================================================================*/
/*= Abstract getters                                                   =*/

/**
 * The fsod_getUD getter returns the user-data field of the fsod
 * output device.
 * If fsod corresponds to the canvas of a YFIG main window (Tyfig_mw),
 * it is this window.
**/

extern Tvgui*       fsod_getGUI    (Cyfig_device* fsod);
extern Tvgui*       fsod_getMsgdrv (Cyfig_device* fsod);
extern void*        fsod_getUD     (Cyfig_device* fsod);
extern Tvgui_outdev fsod_cpyVOD    (Cyfig_device* fsod);
extern Tvgui_outdev*fsod_getVOD    (Cyfig_device* fsod);
extern Toutdev      fsod_cpyOD     (Cyfig_device* fsod);
extern Toutdev*     fsod_getOD     (Cyfig_device* fsod);
extern double       fsod_getPpiSrc (Cyfig_device* fsod);
extern double       fsod_getPpiDes (Cyfig_device* fsod);
extern CboxDble*    fsod_getBoxSrc (Cyfig_device* fsod);
extern CboxDble*    fsod_getBoxDes (Cyfig_device* fsod);
extern Tfig_scene*  fsod_getScene  (Cyfig_device* fsod);
extern Tfigure*     fsod_getFigure (Cyfig_device* fsod);
extern TobjDir      fsod_getTopObj (Cyfig_device* fsod);
extern TobjAll      fsod_getCurrObj(Cyfig_device* fsod);
extern void         fsod_setCurrObj(Cyfig_device* fsod, TobjAll o);

/*======================================================================*/
/*= A quick interface for handling object in a yfig_device.            =*/
/*= It is mainly used by the GUI for creating pixmap corresponding     =*/
/*= exactly to the figure objects.                                     =*/

/**
 * The fsod_attach function attaches an empty scene to the vod
 * output device setting the scene figure resolution to the device
 * resolution and the zoom factor to 1.
 *
 * The fsod_attachPt function attaches an empty scene to the vod
 * output device setting the scene figure resolution to a point (1/72 Inch)
 * and the zoom factor to 1.
 *
 * The fsod_detachAttach function detaches the attached scene of the fsod
 * output as the fsod_detach function does, then it attaches the sc scene
 * to the fsod device and configures the devices.
 *   - w=0 : for showing the whole scene
 *   - w=1 : for showing the scene at real size. The (0,0) point of the
 *           device corresponds to the scene origin (point (0,0)).
 *   - w=2 : for keeping the zoom factor. The center of the scene figure
 *           is placed at the device center.
 *           If there was no scene attached to the fsod device, the
 *           function configures the device for showing the whole scene
 *           (same as w=0).
 *
 * The fsod_detach function detaches the attached scene of the fsod output
 * device and then it deletes the scene within its figure.
 * If the fsod output device has no attached scene, it returns immediately.
 * The fsod_detach must be called when fsod_attach/Pt(vod) has been
 * performed before deleting vod.
**/
extern Tyfig_device* fsod_attach(Tvgui_outdev* vod);
extern Tyfig_device* fsod_attachPt(Tvgui_outdev* vod);
//extern Tyfig_device* fsod_attachPtDxDy(Tvgui_outdev* vod,double*srcDx, double*srcDy);
extern void fsod_detachAttach(Tyfig_device* fsod, Tfig_scene* sc, int w);
extern void fsod_detach(Tyfig_device* fsod);

// add a line object to the scene of the fsod device 
extern TobjAll fsod_addLineXY(Cyfig_device* fsod, double thickness,
                    double x0, double y0, double x1, double y1);

/**
 * The fsod_addLineH functions adds a line object to the scene of the
 * fsod device:
 *  - the line is centered vertically.
 *  - the line starts at bd pixels of the left border
 *  - the line ends   at bd pixels of the right border.
 *
 * The fsod_chgArrowAbs function adds an arrow header to the obj object:
 *  - atBeg==1 : the arrow head is added at the beginning of object.
 *  - atBeg==0 : the arrow head is added at the end of object.
 *  - the arrow type is given by ad->vtidx and ad->flag.
 *  - the arrow length and angle are given by ad->length and ad->angle.
 *  - the thickness parameter sets the the arrow thickness.
 *
 * The fsod_chgArrowRel function adds an arrow header to the obj object:
 *  - atBeg==1 : the arrow head is added at the beginning of object.
 *  - atBeg==0 : the arrow head is added at the end of object.
 *  - the arrow type is given by ad->vtidx and ad->flag.
 *  - the arrow length is set to ad->length times the thickness of obj line
 *    style.
 *  - the arrow angle is set to ad->angle.
 *  - the arrow thickness is set to the thickness of obj line style.
 *
 * If the obj figure object does not support arrow such as ellipse, the
 * former functions return immediately doing nothing.
**/
extern TobjAll fsod_addLineH(Cyfig_device* fsod, double thickness, double bd);
extern void fsod_chgArrowAbs(Cyfig_device* fsod, TobjAll obj, int atBeg,
                             TarrowDesc* ad, double thickness);
extern void fsod_chgArrowRel(Cyfig_device* fsod, TobjAll obj, int atBeg,
                             TarrowDesc* ad);

/**
 * The fsod_scaArrowAbsDU function returns how much the length of the
 * ad absolute arrow (ad->length) must be scaled to fit the dx by dy
 * rectangle.
 *
 * The fsod_scaArrowRelDU function returns how much the length of the
 * ad arrow relative to th th thickness must be scaled to fit the
 * dx by dy rectangle.
 *
 * In the both functions:
 *   - dx and dy are given in destination unit (pixel) of the fsod
 *     output device.
 *   - ad->length, ad->thickness and th are given in point (1/72 Inch).
**/
extern double     fsod_scaArrowAbsDU(Tyfig_device* fsod, CarrowDesc*ad,
                                     double dx, double dy);
extern double     fsod_scaArrowRelDU(Tyfig_device* fsod, CarrowDesc*ad,
                                     double th, double dx, double dy);

/*======================================================================*/
/*= object drawing.                                                    =*/

/**
 * If the gcd GC is OD_GC_UsrDraw then the ydev_draw_obj function
 * configures the OD_GC_UsrDraw GC with the drawing parameters found in
 * the obj object. Otherwise the gcd GC is left unchanged.
 * Then, the ydev_draw_obj function draws the obj object into the fsod
 * device using the parameters of the gcd GC.
 *
 * In other words, the function draws the obj object with its own 
 * drawing parameters if the gcd GC is OD_GC_UsrDraw, otherwise it
 * draws the object with the drawing parameters of the gcd GC.
**/
extern void ydev_draw_obj(Tyfig_device* fsod, Cobject obj, Tod_GCdef gcd);

// - mask is an union of FSC_MM_... flags ( which object kind).
// - noCtrMkr set to 1 for ignoring makers that are also object centers.
// - mkrOpeMask set to a combination of FIGO_MkrOpe_... for ignoring objects
//   that  do not support one of the specified maker operation.
extern void fsod_objmarker_toggleCur (Tyfig_device*fsod, int     mask, int noCtrMkr);
extern void fsod_objmarker_toggleCur2(Tyfig_device*fsod, int     mask, int noCtrMkr, int mkrOpeMask);
extern void fsod_objmarker_toggle    (Tyfig_device*fsod, TobjAll obj,  int noCtrMkr);
extern void fsod_objmarker_toggleHL  (Tyfig_device*fsod, TobjAll obj,  int noCtrMkr);

/**
 * fsod_getChildrenAll   : all the children, no filter.
 * fsod_getChildren      : the children that appear on the device.
 * fsod_getChildrenB     : the children that appear on the bb BB.
**/

extern CobjAllSet*fsod_getChildrenAll (Tyfig_device*fsod);
extern TobjAllSet*fsod_getChildren    (Tyfig_device*fsod);
extern TobjAllSet*fsod_getChildrenB   (Tyfig_device*fsod,CboxDble* bb);

/**
 * fsod_searchFirst    : among the objects that appear on the device and
 *                       that are in an active depth.
 * fsod_searchFirstM   : like fsod_searchFirst  + obj tagged by objMsk.
 * fsod_searchFirstM2  : like fsod_searchFirstM + obj marker operation tagged by mkrOpeMsk.
 *
 * objMsk    : see FSC_MM_...
 * mkrOpeMsk : see FIGO_MkrOpe_...
**/
extern TobjCloseToPt fsod_searchFirstPT (Tyfig_device*fsod,Tdble2 pt);
extern TobjCloseToPt fsod_searchFirstXY (Tyfig_device*fsod,double x, double y);
extern TobjCloseToPt fsod_searchFirstMPT(Tyfig_device*fsod,Tdble2 pt,           int objMsk);
extern TobjCloseToPt fsod_searchFirstMXY(Tyfig_device*fsod,double x, double y,  int objMsk);
extern TobjCloseToPt fsod_searchFirstM2PT(Tyfig_device*fsod,Tdble2 pt,          int objMsk, int mkrOpeMsk);
extern TobjCloseToPt fsod_searchFirstM2XY(Tyfig_device*fsod,double x, double y, int objMsk, int mkrOpeMsk);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YFIG_DEVICE_H
/*======================================================================*/
