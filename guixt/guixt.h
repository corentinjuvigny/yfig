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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt.h 142 2018-05-28 16:35:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Header that is included at top of all source files                 =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_H
#define FILE_GUIXT_H
/*======================================================================*/

#include "../config.h"
#include "guixt/guixt-top.h"
#include "guixt/guixt-event.h"
#include "guixt/guixt-util.h"
#include "guixt/guixt-create.h"

#include "tools/bitmap-types.h"

/*======================================================================*/

#define VOD_Drawable(dev) \
        ((dev)->v_vtable->topVT.ident==VGUI_ODID_Canvas \
                ? (Drawable)_XtWindow(((Widget)(dev)->v_cvsWidEGS)) \
                : (Drawable)PXMEGS2X11((dev)->v_pixmap))
#define VDEV_Drawable(dev) VOD_Drawable(dev)
#define PXMEGS2X11(pxm) (pxm).ulong

Inline  TpixmapEGS X112PXMEGS(Pixmap pxm)
  { TpixmapEGS pxmEGS; pxmEGS.ulong = pxm; return pxmEGS; }

/*======================================================================*/
#endif // FILE_GUIXT_H
/*======================================================================*/
