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
 *      $Id: guixt-config.h.in 220 2018-06-14 07:32:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guixt/guixt-config.h.in $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIXT_CONFIG_H
#define FILE_GUIXT_CONFIG_H
/*======================================================================*/

// X11 headers

#if defined(HAVE_SYS_TYPES_H) && HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/resource.h>
#include <X11/X.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

// Xaw7 headers
#define USE_XAW 0


// Xaw3d headers 
#define USE_XAW3D 1

#include <X11/Xaw3d/Command.h>
#include <X11/Xaw3d/Label.h>
#include <X11/Xaw3d/Dialog.h>
#include <X11/Xaw3d/Box.h>
#include <X11/Xaw3d/Form.h>
#include <X11/Xaw3d/Cardinals.h>
#include <X11/Xaw3d/Text.h>
#include <X11/Xaw3d/AsciiText.h>
#include <X11/Xaw3d/MenuButton.h>
#include <X11/Xaw3d/Scrollbar.h>
#include <X11/Xaw3d/Simple.h>
#include <X11/Xaw3d/SimpleMenu.h>
#include <X11/Xaw3d/Sme.h>
#include <X11/Xaw3d/SmeLine.h>
#include <X11/Xaw3d/Tip.h>
#include <X11/Xaw3d/Toggle.h>
#include <X11/Xaw3d/Paned.h>
#include <X11/Xaw3d/Viewport.h>
#include <X11/Xaw3d/List.h>
#include <X11/Xaw3d/SmeBSB.h>

// macros for switching between Xaw and Xaw3d 
#if USE_XAW==1
#   define _XtWindow(w) XtWindow((Widget)w)
#elif USE_XAW3D==1
#   define _XtWindow(w) XtWindow((Widget)w)
#else
#   error Either USE_XAW or USE_XAW3D must be defined to 1
#endif

// if pango, xft and fontconfig are usable
#define USE_PANGOXFT @pangoxft_enabled@

/*======================================================================*/
#endif // FILE_GUIXT_CONFIG_H
/*======================================================================*/

