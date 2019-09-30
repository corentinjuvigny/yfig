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
 *      $Id: wid-canvasI.h 113 2018-01-09 18:35:22Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-canvasI.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Function definitions shared between the wid-canvas...c files       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_CANVASI_H
#define FILE_WID_CANVASI_H
/*======================================================================*/

#include "vgui/wid-canvas.h"

/*======================================================================*/
/*= interface to wid-canvas-unit.c                                     =*/

/**
 * Return in Dy the height of horizontal scale.
 * Return in Dx the width  of vertical scale.
**/
extern void wid_canvas_rulerDxDy (Cwid_canvas*self, int*dx, int*dy);

extern void wid_canvas_drawRulers    (Cwid_canvas* self);
extern void wid_canvas_drawRulersMkr (Cwid_canvas* self, int x, int y);
extern void wid_canvas_clearRulersMkr(Cwid_canvas* self);

/*======================================================================*/
#endif //FILE_WID_CANVASI_H
/*======================================================================*/
