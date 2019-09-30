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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: outdev-vt.c 214 2018-06-13 11:34:56Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdev-vt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The vtable for Toutdev output device.                              =*/
/*=                                                                    =*/
/*= This file is only included by the outdev.c so all definitions      =*/
/*= are static.                                                        =*/
/*======================================================================*/

#include "outdev/resources.h"
#include "outdev/outdev-vt.h"
#include "tools/color.h"

/*======================================================================*/
/*= Constructors & destructor                                          =*/

static void vt_destructor(void* dev)
{
    Toutdev* od = dev;
    Coutdev_vt* vt = od->vtable;
    TfontEGSunload unload=0;
    while ( vt!=0 && (unload=vt->strVT.fontUnload)==0 )
        vt = vt->topVT.parentVT;
    if ( od->resources!=0 ) odres_delete( od->resources , unload, od );
}

static void vt_clear(const void* dev)
{
    Coutdev* od = dev;
    Ccolor* clr = od_clearColor(od);
    od_ldaf_setGCll((Toutdev*)od, OD_GC_SysDraw, 0.0, DA_FS_Solid, clr, 0,0,0);
    od_fill_rectBB( od, OD_GC_SysDraw, od->boxSrc);
}

static Ccolor* vt_clearColor(const void* dev)
{
    Coutdev* od  = dev;
    Ccolor*  clr = od->vtable->colorVT.white(dev);
    color_delete( (Tcolor*)clr ); // while "white(dev) allocated the color 
    return clr;
}

/*======================================================================*/
/*= Color handler                                                      =*/

/*======================================================================*/
/*= pixmap handler                                                     =*/

/*======================================================================*/
/*= GC settings                                                        =*/

/*======================================================================*/
/*= point, single lines and multi-lines                                =*/

/*======================================================================*/
/*= rectangle  and polygon                                             =*/

/*======================================================================*/
/*= The virtual table of output device.                                =*/

Coutdev_vt outdev_vtable = {
    .topVT  = {
        .ident      = 0,
        .parentVT   = NULL,
        .byteSize   = sizeof( Toutdev ),
        .destructor = vt_destructor,
        .clear      = vt_clear,
        .clearColor = vt_clearColor,
    },
};

/*======================================================================*/
