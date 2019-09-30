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
 *      $Id: outdev.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/outdev/outdev.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Abstract type for drawing something into something.                =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OUTDEV_OUTDEV_H
#define FILE_OUTDEV_OUTDEV_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "types.h"
#include "tools/boxes.h"
#include "tools/color-types.h"
#include "tools/font-types.h"

/*======================================================================*/

struct _Toutdev {
  /* destination part */
    Coutdev_vt*  vtable; // device drawing functions.
    double       ppiDes; // device has desPPI pixel by Inch
    TboxDble     boxDes; // bounding box of the destination area in
                         // destination unit.  x and y are always null.
    void*        parDes; // Parameters of the destination (not handled).
                         // Usually data for implementation ov vtable functions
    TodResources*resources;  // resources handled by destination device

  /* source part */
    Coutdev_src_vt*
                 vtSrc;  // virtual table for source operations.
    TboxDble     boxSrc; // bounding box of the source area in source unit.
    void*        parSrc; // Parameters of the source area (not handled).

  /* other */
    void*        userdata; // not handled by output device

  /* these fields are fully handled by the od_... functions */
    double homothetySrc2Des; // from boxSrc to boxDes it is always equal to
                             // boxDes.dx/boxSrc.dx and boxDes.dy/boxSrc.dy
    double desPerSrc;        // there is destPerSrc destination units (pixel)
                             // in 1 source unit.
#define OD_ppiSrc(od) ((od)->ppiDes/(od)->desPerSrc)

    Tmsgdrv* msgdrv;    // driver for printing messages (not handled by module),
                        // null redirect to gl_msgdrv_default.

    char error_imgLoad_NoEgs; // 1 when error was printed

#define CONV_D_S2D(dev,d) ( (d) * (dev)->homothetySrc2Des )
#define CONV_X_S2D(dev,x) ( ((x)-Box_x((dev)->boxSrc)) * (dev)->homothetySrc2Des )
#define CONV_Y_S2D(dev,y) ( ((y)-Box_y((dev)->boxSrc)) * (dev)->homothetySrc2Des )
#define CONV_D_D2S(dev,d) ( (d) / (dev)->homothetySrc2Des )
#define CONV_X_D2S(dev,x) ( ((x)/(dev)->homothetySrc2Des) + Box_x((dev)->boxSrc) )
#define CONV_Y_D2S(dev,y) ( ((y)/(dev)->homothetySrc2Des) + Box_y((dev)->boxSrc) )

// obsolete
#define CONV_D_Ds2Win(dev,d) ( (d) * (dev)->homothetySrc2Des )
#define CONV_X_Ds2Win(dev,x) ( ((x)-Box_x((dev)->boxSrc)) * (dev)->homothetySrc2Des )
#define CONV_Y_Ds2Win(dev,y) ( ((y)-Box_y((dev)->boxSrc)) * (dev)->homothetySrc2Des )
#define CONV_D_Win2Ds(dev,d) ( (d) / (dev)->homothetySrc2Des )
#define CONV_X_Win2Ds(dev,x) ( ((x)/(dev)->homothetySrc2Des) + Box_x((dev)->boxSrc) )
#define CONV_Y_Win2Ds(dev,y) ( ((y)/(dev)->homothetySrc2Des) + Box_y((dev)->boxSrc) )
};

/*======================================================================*/
/*= Constructors & Destructor                                          =*/

/**
 * The od_new0 function creates an output device leaving the
 * resources pointer of the output device to NULL.
 *
 * The od_new0r function creates an output device setting the
 * resources pointer of the output device to r.
 *
 * The od_delete function invokes the destructors in the class tree
 * from bottom to top and then frees the od pointer itself.
 *
 * Return:
 *  The od_new0 and the od_new0r functions return the created output device.
 *
 * Parameters:
 *  vt: the virtual table for this output device kind (mandatory).
 *  ppiDes: the output device resolution in pixel by inch.
 *  boxDes: dx and dy size in pixel of the output device.
 *          x and y are not used.
 *          if boxDes is null, the size is set to 10x100 pixels.
**/
extern Toutdev* od_new0  (Toutdev_vt*vt, double ppiDes, CboxDble*boxDes);
extern Toutdev* od_new0r (Toutdev_vt*vt, double ppiDes, CboxDble*boxDes,
                          TodResources* r);
extern void     od_delete(Toutdev*   od);

/**
 * The od_attach function attaches a source device to the od output device.
 * The zoom factor is set to 1 (real size) and the (0.0) source pixel is mapped
 * on the (0,0) output device pixel.
 *
 * The od_detach function resets to zero or NULL, all the fields the od_attach
 * function initialized.
 * No destructor is called.
 **/
extern void od_attach(Toutdev*od, Coutdev_src_vt* vt, double ppiSrc, void* dataSrc);
extern void od_detach(Toutdev*od);

/** The default device operations **/
extern Coutdev_vt outdev_vtable;

/*======================================================================*/
/*= Wrappers to operations of topVT and vtSrc.                         =*/

/**
 * The od_clear function reinitializes the output device without any drawing.
 * The function is searched in the class tree.
 *
 * The od_draw function chains in order the next functions that are firsly
 * defined in the class tree.
 *   - topVT->preDraw(od)
 *   - vtSrc->draw(od)
 *   - topVT->postDraw(od)
 * If the class tree does not define a such function, it is skipped.
 * 
 * The od_redraw function clears the output device and then draws (od_draw)
 * the picture.
 *
 * DEFAULT:
 *  - od_clear:  it fills output device with the od_clearColor.
 *  - od_draw:   it does nothing.
 *  - od_redraw: it has the default of od_clear and od_draw.
**/
extern void od_clear (Coutdev*dev);
extern void od_draw  (Coutdev*dev);
extern void od_redraw(Coutdev*dev);

/**
 * The od_clearColor function returns the color to use for clearing
 * the output device.
**/
extern Ccolor* od_clearColor(Coutdev*dev);

// default is dev->msgdrv
extern Tmsgdrv* od_msgdrv(Coutdev*dev);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Drawing sequence.                                                  =*/

/**
 * The od_ds_stdStart function initializes a new sequence of standard
 * drawings that stores all the subsequent drawing operations on the dev
 * device.
 *
 * The od_ds_stdCommit functions sends to the dev output device all
 * the drawing operations that are stored since the previous ds_stdStart
 * invocation.
**/
extern void od_ds_stdStart (Coutdev*dev);
extern void od_ds_stdCommit(Coutdev*dev);

#define OD_DS_NBXOR 4 //number of xorDraw/Erase sequences
extern void od_ds_xorDrawStart(Coutdev*od, int num);
extern void od_ds_xorDrawCommit(Coutdev*od, int num);
extern void od_ds_xorEraseStart(Coutdev*od, int num);
extern void od_ds_xorEraseCommit(Coutdev*od, int num);

Inline void od_ds_xorDrawStart_0   (Coutdev *dev) { od_ds_xorDrawStart  (dev,0); }
Inline void od_ds_xorDrawStart_1   (Coutdev *dev) { od_ds_xorDrawStart  (dev,1); }
Inline void od_ds_xorDrawStart_2   (Coutdev *dev) { od_ds_xorDrawStart  (dev,2); }
Inline void od_ds_xorDrawStart_3   (Coutdev *dev) { od_ds_xorDrawStart  (dev,3); }
Inline void od_ds_xorDrawCommit_0  (Coutdev *dev) { od_ds_xorDrawCommit (dev,0); }
Inline void od_ds_xorDrawCommit_1  (Coutdev *dev) { od_ds_xorDrawCommit (dev,1); }
Inline void od_ds_xorDrawCommit_2  (Coutdev *dev) { od_ds_xorDrawCommit (dev,2); }
Inline void od_ds_xorDrawCommit_3  (Coutdev *dev) { od_ds_xorDrawCommit (dev,3); }
Inline void od_ds_xorEraseStart_0  (Coutdev *dev) { od_ds_xorEraseStart (dev,0); }
Inline void od_ds_xorEraseStart_1  (Coutdev *dev) { od_ds_xorEraseStart (dev,1); }
Inline void od_ds_xorEraseStart_2  (Coutdev *dev) { od_ds_xorEraseStart (dev,2); }
Inline void od_ds_xorEraseStart_3  (Coutdev *dev) { od_ds_xorEraseStart (dev,3); }
Inline void od_ds_xorEraseCommit_0 (Coutdev *dev) { od_ds_xorEraseCommit(dev,0); }
Inline void od_ds_xorEraseCommit_1 (Coutdev *dev) { od_ds_xorEraseCommit(dev,1); }
Inline void od_ds_xorEraseCommit_2 (Coutdev *dev) { od_ds_xorEraseCommit(dev,2); }
Inline void od_ds_xorEraseCommit_3 (Coutdev *dev) { od_ds_xorEraseCommit(dev,3); }

/*======================================================================*/
/*= Wrappers to operations of colorVT.                                 =*/

/**
 * The black function returns the black color.
 * The white function returns the white color.
 *
 * The caller must delete the returned color after use.
**/
extern Tcolor* od_whiteColor(Coutdev*dev);
extern Tcolor* od_blackColor(Coutdev*dev);

/*======================================================================*/
/*= Wrappers to operations of srcVT and font cache of resources.       =*/

extern void       od_fontinfo_chgLazy (Toutdev*od, TfontInfo**fdes, CfontInfo* fsrc);
extern void       od_fontinfo_delete  (Toutdev*od, TfontInfo* fi);

extern CfontInfo* od_fontset_searchLRU(Toutdev*od, int fi, double pts);
extern CfontInfo* od_fontset_addLRU   (Toutdev*od, int fi, double pts,
                                       TfontEGS f, double ratio);

/*======================================================================*/
/*= Changing the drawing point of view and scale.                      =*/

/**
 * The od_updateSrcBB_scale function does:
 *   - It sets dev->homothetySrc2Des ratio to r.
 *   - It sets the size of the dev->boxSrc bounding box to the size
 *     of the boxDes bounding box scaled by the r ratio.
 *   - The positions of the boxSrc and boxDes bounding boxes
 *     are not modified.
 * 
 * The od_updateSrcBB function updates the srcBox from the desBox box
 * and the homothetySrc2Des factor.
 *   - the desBox box and the homothetySrc2Des factor are left unchanged.
 *
 * The od_updateSrcBB_toBox functions updates the ratio and boxSrc of
 * the dev device in order
 *   - to have the maximal ratio,
 *   - to have the sbox fully included into boxSrc.
 *   - to center sbox into boxSrc.
 *   - to put sbox at least at 0.5 cm of the boxSrc edges.
 *
 * The od_updateSrcBB_toBoxXY functions updates the ratio and boxSrc of
 * the dev device in order
 *   - to have the maximal ratio,
 *   - to have the sbox fully included into boxSrc.
 *   - to center sbox into boxSrc.
 *   - to put sbox at least at x cm of the left and right boxSrc edges.
 *   - to put sbox at least at y cm of the top and bottom boxSrc edges.
 *
 * Return:
 *   The od_updateSrcBB... functions return 0 if the dev device is
 *   left unchanged, otherwise they return 1.
**/
extern int od_updateSrcBB_scale  (Toutdev*dev, double r);
extern int od_updateSrcBB        (Toutdev*dev);
extern int od_updateSrcBB_toBox  (Toutdev*dev, CboxDble*sbox);
extern int od_updateSrcBB_toBoxXY(Toutdev*dev, CboxDble*sbox,
                                  double x, double y);

/**
 * The dev_getUsrZoom function returns the actual zoom factor of
 * the src scene to the dest device.  This f factor verifies:
 *    <n cm in src device>  = <n*f cm on the dest device>
 * So f=2  --> the src scene is painted 2 times bigger on the dest device
 * So f=.5 --> the src scene is painted 2 times smaller on the dest device
**/
extern double od_getUsrZoom(Coutdev*dev);

/**
 * The od_setUsrZoom_NW function sets the zoom factor of the od output
 * device to zf. The NW point {eg: (0,0)} of the destination device is
 * left unchanged. It corresponds to the same point of the source scene.
 *
 * The od_setUsrZoom_CENTER function sets the zoom factor of the od
 * output device to zf.
 * The center point {eg: (box_dx(boxDes)/2,box_dy(boxDes)/2)}
 * of the destination device is left unchanged.
 *
 * These functions change the homothetySrc2Des and boxSrc fields of
 * the dev device.
 * After the call, dev_getUsrZoom(dev) returns the zf factor.
 *
 * The zoom factor verifies:
 *    <n cm in src device>  = <n*f cm on the dest device>
 * So f=2  --> the src scene is painted 2 times bigger on the dest device
 * So f=.5 --> the src scene is painted 2 times smaller on the dest device
 * 
 * Return:
 * These functions return 1 if the zoom factor is changed otherwise they
 * return 0.
 *
 * Exemple:
 * The standard usage of these functions is:
 *   if ( od_setUsrZoom_CENTER(od,zf) 
 *      od_redraw(od);
**/
extern int od_setUsrZoom_NW(    Toutdev*od, double zf);
extern int od_setUsrZoom_CENTER(Toutdev*od, double zf);

/*======================================================================*/
/*= Miscelaneous getters and setters                                   =*/

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Handler of GC stack                                                =*/

extern Tsty_text* od_style_topText(Coutdev*od, Tod_GCdef gcd);
extern Tsty_line* od_style_topLine(Coutdev*od, Tod_GCdef gcd);
extern Tsty_fill* od_style_topFill(Coutdev*od, Tod_GCdef gcd);
extern Tstyle*    od_style_top    (Coutdev*od, Tod_GCdef gcd);
extern void       od_style_pushTop(Toutdev*od, Tod_GCdef gcd);
extern void       od_style_pop    (Toutdev*od, Tod_GCdef gcd);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OUTDEV_OUTDEV_H
/*======================================================================*/
