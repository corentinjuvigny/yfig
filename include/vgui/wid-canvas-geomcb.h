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
 *      $Id: wid-canvas-geomcb.h 88 2017-05-30 09:31:57Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-canvas-geomcb.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_WID_CANVAS_GEOMCB_H
#define FILE_VGUI_WID_CANVAS_GEOMCB_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================
 * A drawing area widget with optional scales.
 *
 * Definition of the canvas geometric limits 
 *
 * These constraints are only used when the user changes via the GUI
 * the canvas geometry by shifting or scaling the shown scene.
 *
 * The structure contains also a callback that is run when the canvas
 * geometry is changed via the GUI.
 * The new geometry can be retrieved from the canvas device.
**======================================================================*/

#include "tools/range.h"

/*======================================================================*/
/*= Default values                                                     =*/

#define CVSGEO_Xmin LUNIT_CmToFu(-100)   // -1 meter
#define CVSGEO_Xmax LUNIT_CmToFu(+100)   //  1 meter
#define CVSGEO_Ymin LUNIT_CmToFu(-100)   // -1 meter
#define CVSGEO_Ymax LUNIT_CmToFu(+100)   //  1 meter

#define CVSGEO_Zmin  0.01                 //  1/100 times
#define CVSGEO_Zmax  100                  //  100 times
#define CVSGEO_Zstep RANGE_DF_FSTEP       //  9/8

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Twidcvs_geomCB {
    void  (*cb)(Twid_canvas*);
    double minXfu,maxXfu;           // not yet used
    double minYfu,maxYfu;           // not yet used
    double minZoom,maxZoom,stepZoom;
};

/*======================================================================*/
/*= Interface                                                          =*/

/**
 * The wid_canvas_geomCB_new function creates a default geometry
 * constraint structure and attaches it to the self canvas
 *
 * The structure is initialized using the CVSGEO_... macros
**/
Twidcvs_geomCB* wid_canvas_geomCB_new(Twid_canvas*self,
                    void  (*cb)(Twid_canvas*));

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif //FILE_VGUI_WID_CANVAS_GEOMCB_H
/*======================================================================*/
