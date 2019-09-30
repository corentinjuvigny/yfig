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
 *      $Id: top-vt.h 269 2018-07-04 15:11:18Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/top-vt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top utlities of the output device (constructor, destructor, ...).  =*/
/*=                                                                    =*/
/*= See the outdev.h header for the documentation.                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_CDOR_VT_H
#define FILE_OUTDEV_CDOR_VT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "outdev/types.h"
#include "outdev/ldaf-hl.h"
#include "tools/color-types.h"

/*======================================================================*/
/*= Interface                                                          =*/
struct _Toutdev_top_vt {
/*======================================================================*/

    int          ident;      // 0 for the default vtable
    Coutdev_vt*  parentVT;
    int          byteSize;   // byte size of the outdev structure.

    void       (*destructor) (void*dev); // MUST NOT DO free(dev)
    void       (*clear)      (const void*dev);
    Ccolor*    (*clearColor) (const void*dev);
    void       (*preDraw)    (const void*dev);
    void       (*postDraw)   (const void*dev);
    Tmsgdrv*   (*msgdrv)     (const void*dev);

  /*= =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  = =*/
  /*= Drawing sequence.                                                =*/
  /**
   * The ds_stdStart function starts a sequence of standard drawings.
   * All the subsequent drawing operations are stored.
   *
   * The ds_stdCommit functions sends to the dev output device all
   * the drawing operations that are stored since the previous ds_stdStart
   * invocation.
   *
   * The ds_xorDrawStart function starts a sequence of drawings that 
   * SONT DESTINE to be erased later without changing the current picture
   * of the output device.
   *
   * The ds_xorDrawCommit functions sends to the dev output device all
   * the drawing operations that are stored since the previous ds_xorDrawStart
   * invocation.
   *
   * The ds_xorEraseStart/ds_xorEraseCommit function starts a sequence of drawings
   * that erase the drawings done during the previous ds_xorDrawStart/ds_xorDrawCommit
   * sequence.
   *
   * The drawings operations done in the corresponding sequences
   *    ds_xorDrawStart/ds_xorDrawCommit and ds_xorEraseStart/ds_xorEraseCommit
   * must be the same (the opration order does not matter).
   *
   * For EGS that supports the XOR drawing operation theses functions may do nothing.
  **/
    void       (*ds_stdStart)      (const void*dev);
    void       (*ds_stdCommit)     (const void*dev);
    void       (*ds_xorDrawStart)  (const void*dev, int sn);
    void       (*ds_xorDrawCommit) (const void*dev, int sn);
    void       (*ds_xorEraseStart) (const void*dev, int sn);
    void       (*ds_xorEraseCommit)(const void*dev, int sn);

/*======================================================================*/
};
/*======================================================================*/

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_CDOR_VT_H
/*======================================================================*/
