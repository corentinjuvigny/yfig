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
 *      $Id: yfig-pixmap.h 247 2018-06-24 22:26:58Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-pixmap.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the pixmaps that the YFIG GUI requires.                =*/
/*=                                                                    =*/
/*= Those are either generated from static bitmap data or drawn in a   =*/
/*= pixmap device.                                                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_PIXMAP_H
#define FILE_YFIG_PIXMAP_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-types.h"
#include "vgui/vgui-pixmap.h"
#include "outdev/style.h"

/*======================================================================*/
/*= pixmaps for line attributes (only used in GUI).                    =*/

// for line style: solid, dot, dash ...
extern int          pxm_lineStyle_dx ();
extern int          pxm_lineStyle_dy ();
extern int          pxm_lineStyle_nbe(Tvgui*gui);
extern TlineStyle*  pxm_lineStyle_ref();
extern TpixmapEGS*  pxm_lineStyle_pxm(Tvgui* gui, int dx, int dy, Ccolor* bg);

// for join style: Miter, Round, ...
extern int          pxm_joinStyle_dx ();
extern int          pxm_joinStyle_dy ();
extern int          pxm_joinStyle_nbe();
extern TjoinStyle*  pxm_joinStyle_ref();
extern TpixmapEGS*  pxm_joinStyle_pxm(Tvgui* gui);

// for cap style: Butt, Round, ...
extern int          pxm_capStyle_dx ();
extern int          pxm_capStyle_dy ();
extern int          pxm_capStyle_nbe();
extern TcapStyle*   pxm_capStyle_ref();
extern TpixmapEGS*  pxm_capStyle_pxm(Tvgui* gui);

// for begin and end arrows.
extern int          pxm_arrow_dx ();
extern int          pxm_arrow_dy ();
extern int          pxm_arrow_nbe();
extern TarrowDesc*  pxm_arrow_ref();
extern TpixmapEGS*  pxm_arrow_pxm(Tvgui* gui, int atBeg);

// for arrow mode.
extern int          pxm_arrowmode_dx ();
extern int          pxm_arrowmode_dy ();
#define             pxm_arrowmode_nbe()  4
extern TpixmapEGS*  pxm_arrowmode_pxm(Tvgui* gui, int idx);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YFIG_PIXMAP_H
/*======================================================================*/
