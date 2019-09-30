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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-arrow.h 368 2018-11-19 00:23:18Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/ydw/ydw-arrow.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialogue that edit arrow parameters.                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YDW_YDW_ARROWPARAM_H
#define FILE_YDW_YDW_ARROWPARAM_H
/*======================================================================*/

#include "yfig-config.h"

#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-lvu.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Tydw_arrow1 Tydw_arrow1;
typedef const   Tydw_arrow1 Cydw_arrow1;

struct _Tydw_arrow1 {
    Tvgui_dw    any;
    TarrowDesc* dw_ad;
    Twid_ilvu*  dw_length;
    Twid_ilvu*  dw_angle;
    Twid_ilvu*  dw_thickness;
    Twid_ilvu*  dw_mode;
    void      (*dw_CB)(void*);
    void*       dw_CBud;
};

typedef struct _Tydw_arrow2 Tydw_arrow2;
typedef const   Tydw_arrow2 Cydw_arrow2;

struct _Tydw_arrow2 {
    Tvgui_dw    any;
    Tydw_arrow1* dw_arrowbeg;
    Tydw_arrow1* dw_arrowend;
};

/*======================================================================*/
/*= Interface                                                          =*/

extern Tydw_arrow1*
            ydw_arrow1_nbfSub(Tvgui_dw* vdw, Tyfig_mw* ymw,
                              int atBeg, TarrowDesc*ead,
                              void* cb, void*ud);
extern void ydw_arrow1_delete(Tydw_arrow1*seld);

extern Tydw_arrow2*
            ydw_arrow2_nbfSub(Tvgui_dw* vdw, Tyfig_mw* ymw,
                              TarrowDesc* bad, TarrowDesc*ead,
                              void* cb, void*ud);
extern void ydw_arrow2_delete(Tydw_arrow2*seld);

/*======================================================================*/
#endif  // FILE_YDW_YDW_ARROWPARAM_H
/*======================================================================*/
