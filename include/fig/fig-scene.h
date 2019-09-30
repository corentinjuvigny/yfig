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
 * $Software: YFIG-FDS (Figure Data Structure)
 *      $Id: fig-scene.h 387 2019-01-26 22:53:16Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/fig-scene.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A figure scene defines:                                            =*/
/*=   - the figure objects that must been draw.                        =*/
/*=   - the output device into which drawings are done.                =*/
/*=   - the output geometry.                                           =*/
/*=                                                                    =*/
/*= The interface provides the utilities:                              =*/
/*=   - for creating a scene from a figure.                            =*/
/*=   - for setting the object to print.                               =*/
/*=   - for fixing output device and output geometry.                  =*/
/*=   - for drawing the scene or single object.                        =*/
/*======================================================================*/

#include "fig/fig-types.h"

#include "outdev/types.h"
#include "tools/boxes.h"


/*======================================================================*/
#ifndef FILE_FIG_SCENE_H
#define FILE_FIG_SCENE_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

/*======================================================================*/
/* Data structure                                                      =*/

/* depth Selection */
typedef struct _Tfig_sceneDepth Tfig_sceneDepth;
typedef const   Tfig_sceneDepth Cfig_sceneDepth;

struct _Tfig_sceneDepth {
    Tscdepth table[FIG_MAX_DEPTH]; // <0:not active
                                   //  0:not used 
                                   // >0:active
    int unactiveMode;   // 0: the no active objects must be ignored
                        // 1: the no active objects must be drawn
                        //    lightly.
};

/* Run time data */
typedef struct _Tfig_sceneRTP Tfig_sceneRTP;
typedef const   Tfig_sceneRTP Cgui_sceneRTP;

struct _Tfig_sceneRTP {
    int currDepth;  // the depth that is currently printed
};

/*======================================================================*/

/* The scene itself */
struct _Tfig_scene {
    /* information about depth */
      Tfig_sceneDepth sc_depths; // they are stored here.
      // defines if is the i th depth is active.
#define sc_isActive(sc,i)  (sc->sc_depths.table[i]>0)
      // defines how no active depth must be drawn.
      //   Igno: must not be shown
      //   Gray: must be shown lightly in background
#define sc_unActIgno(sc)  (sc->sc_depths.unactiveMode==0)
#define sc_unActGray(sc)  (sc->sc_depths.unactiveMode!=0)

    /* information about active markers */
      unsigned int sc_selectableObjectMask; // see YSC_MM_Xxx

    /* information about mode: compound open */
      void*        sc_compoundData;

    /* and finally the figure */
      Tfigure*     sc_dessin;

    /* the device for drawwing */
      Toutdev*     sc_device;

    /* run time parameters (used by fig module when drawing) */
      TobjAll       sc_currObj;
      Tfig_sceneRTP*sc_rtp;
};

/*======================================================================*/
/*= Interface for creating/deleting scene                              =*/
/*=                                                                    =*/
/*= The fig_scene_newByFig function creates a new scene that contains  =*/
/*= the fig figure. The scene is initialized:                          =*/
/*=   - for printing all the depth,                                    =*/
/*=   - for printing all the object of the fig figure.                 =*/
/*= The fuction eats the fig figure (the later is deleted by the       =*/
/*= fig_scene_delete function.                                         =*/
/*=                                                                    =*/
/*= The fig_scene_delete function frees all the memory (figure         =*/
/*= included) used by the sc scene.                                    =*/
/*=                                                                    =*/
/*= The fig_scene_attachDev function attaches the sc scene to the od   =*/
/*= output device. Once performed, od_draw(od) draws the sc scene into =*/
/*= the od output device.                                              =*/
/*= If realSize is false (0) the od output device is configured to     =*/
/*= show the whole scene.                                              =*/
/*= If realSize is true (1) the od output device is configured to show =*/
/*= the scene at real size, the (0,0) point of the device being the    =*/
/*= scene origin (point (0,0)).                                        =*/
/*=                                                                    =*/
/*= The fig_scene_attachDev function attaches the sc scene to the od   =*/
/*= output device. Once performed, od_draw(od) draws the sc scene into =*/
/*= the od output device.                                              =*/
/*= The od output device is configured:                                =*/
/*=  - to have the figure entirely drawn.                              =*/
/*=  - to have the maximal ratio,                                      =*/
/*=  - to center the figure in output device.                          =*/
/*=  - to put the figure at least at x cm (resp; y) of the left and    =*/
/*=    right (resp: top and bottom) edges of the device                =*/
/*=                                                                    =*/
/*= For detaching a scene of an output device, use the od_detach       =*/
/*= function.                                                          =*/

extern Tfig_scene* fig_scene_newByFig (Tfigure*fig); // eat fig
extern void        fig_scene_delete   (Tfig_scene*sc);
extern void        fig_scene_attachDev(Tfig_scene*sc, Toutdev*od,
                                       int realSize);
extern void        fig_scene_attachDevXY(Tfig_scene*sc, Toutdev*od,
                                         double x, double y);

/*======================================================================*/
/*= Interface for configuring scene                                     =*/

extern void fig_scene_enableAllDepths (Tfig_scene*sc);
extern void fig_scene_disableAllDepths(Tfig_scene*sc);
extern void fig_scene_enableDepth     (Tfig_scene*sc, int d);
extern void fig_scene_disableDepth    (Tfig_scene*sc, int d);
Inline void fig_scene_initDepthByT    (Tfig_scene*sc, Cscdepth*depths) { memcpy(sc->sc_depths.table, depths, sizeof( sc->sc_depths.table ) ); }
Inline void fig_scene_initDepthByS    (Tfig_scene*sc, Cfig_scene*src)  { fig_scene_initDepthByT(sc,src->sc_depths.table); }
Inline void fig_scene_copyDepthToT    (Cfig_scene*sc, Tscdepth*depths) { memcpy(depths, sc->sc_depths.table, sizeof( sc->sc_depths.table ) ); }

extern TobjDir fig_scene_getTopObj (Cfig_scene*sc);
extern TobjAll fig_scene_getCurrObj(Cfig_scene*sc);
extern void    fig_scene_setCurrObj(Cfig_scene*sc, TobjAll o);

/*======================================================================*/
/*= Interface for getting bounding boxes.                              =*/

/**
 *  - The fig_scene_BBox function is similar to fig_scene_BBoxTopAll
 *    except that it returns a pointer to a box instead of a copy.
 *  - The fig_scene_BBoxTopAll function returns the bounding box of
 *    the figure of the sc scene using all the layers.
 *  - The fig_scene_BBoxTopSel function returns the bounding box of
 *    the figure of the sc scene using only the active layers.
 *  - The fig_scene_BBoxCurAll function returns the bounding box of
 *    the current object of the sc scene (fig_scene_getCurrObj(sc))
 *    using all the layers.
 *  - The fig_scene_BBoxCurSel function returns the bounding box of
 *    the current object of the sc scene (fig_scene_getCurrObj(sc))
 *    using only the active layers.
 * The unit of the bounding box returned by the former functions is
 * the unit of the figure that is embedded into the sc scene.
 *
 * The fig_scene_BBox...U functions are similar to the fig_scene_BBox...
 * function but the unit of the returned bounding box is given by the
 * uu parameter (see LUNIT_ID_... macros).
**/
extern CboxDble* fig_scene_BBox       (Tfig_scene*sc); // same as fig_scene_BBoxTopAll
extern TboxDble  fig_scene_BBoxTopAll (Tfig_scene*sc);
extern TboxDble  fig_scene_BBoxTopSel (Tfig_scene*sc);
extern TboxDble  fig_scene_BBoxCurAll (Tfig_scene*sc);
extern TboxDble  fig_scene_BBoxCurSel (Tfig_scene*sc);
extern TboxDble  fig_scene_BBoxTopAllU(Tfig_scene*sc, int uu);
extern TboxDble  fig_scene_BBoxTopSelU(Tfig_scene*sc, int uu);
extern TboxDble  fig_scene_BBoxCurAllU(Tfig_scene*sc, int uu);
extern TboxDble  fig_scene_BBoxCurSelU(Tfig_scene*sc, int uu);

/*======================================================================*/
/*= Interface for printing scene                                       =*/

/**
 * Return:
 *  - The fig_scene_draw function returns 0 on success otherwise -1.
**/
extern void      fig_scene_zoom     (Tfig_scene*sc, double zoom);
extern void      fig_scene_translate(Tfig_scene*sc, double tx, double ty);
extern int       fig_scene_draw     (Tfig_scene*sc);
extern void      fig_scene_draw_obj (Tfig_scene*sc, CobjAll o, Tod_GCdef gcd);
extern void      obj_draw           (Tfig_scene*sc, CobjAll o, Tod_GCdef gcd);

/*======================================================================*/
/*= Miscellaneous scene services.                                      =*/
/*=                                                                    =*/
/*= The fig_scene_clrEGScache function resets/clearis the data of the  =*/
/*= figure of the sc scene that the attached output device stored.     =*/
/*= Calling forces the attached output device to recompute them when   =*/
/*= the figure is drawn again.                                         =*/
/*= There is no reason to call it except when you change the output    =*/
/*= device of a scene.                                                 =*/
/*=                                                                    =*/
/*= The fig_scene_pdf function writes into the "stream" stream, the    =*/
/*= pdf description of the srcBoxCm box of the sc scene according to   =*/
/*= the parameters given by the cfg argument.                          =*/
/*=                                                                    =*/
/*= Return:                                                            =*/
/*=  On success, the fig_scene_pdf function returns 1.                 =*/
/*=                                                                    =*/

extern void        fig_scene_clrEGScache(Tfig_scene* sc);

extern int         fig_scene_pdf(Tfig_scene* sc, Tmsgdrv* msgdrv,
                         FILE* stream, TodPdfCfg*cfg, CboxDble srcBoxCm);

extern int         fig_scene_tex(Tfig_scene* sc, Tmsgdrv* msgdrv,
                                 TodPdfCfg*cfg, CboxDble srcBoxCm);

extern int         fig_scene_svg(Tfig_scene* sc, Tmsgdrv* msgdrv,
                         FILE* stream, TodPdfCfg*cfg, CboxDble srcBoxCm);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
#endif // FILE_FIG_SCENE_H
/*======================================================================*/
