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
 *      $Id: wid-canvas.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-canvas.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * A drawing area widget with optional scales.
 *
 * The main function of this widget is to create a Tvgui_odcanvas device
 * that allows to use all the VGUI drawing function.
 *
 * The widget supports 3 flavors given by the vgcm_mode field:
 *   - 0:  * the device is sensitive to the mouse.
 *         * at layout level, the widget grows and fills its parent as
 *           much at possible.
 *   - 1:  * the device is insensitive to the mouse.
 *         * at layout level, its widget size is fixed (given at creation).
 *   - 2:  * the device is insensitive to the mouse.
 *         * at layout level, the widget extends its size as much as
 *           possible in the SE direction.
**/
#if 0 // FIXME REMOVE UPDATE
 * The widget only creates the EGS window of the device but not the
 * device. The creation of this later is left to the upper module.
 * For that the upper module must provide a Tvgui_mw main widget with 
 * a dev_new function (a permanent fatal watchdog verifies the dev_new
 * function is defined). The device creation follows the next steps:
 *   - The canvas calls the dev_new function providing the clear function.
 *   - The dev_new function creates a sub class of Tvgui_odcanvas class
 *     and then returns it.
 *   - The canvas finishes the initialization of the proper fields of
 *     the Tvgui_devcanvas devices.
 * The example below illustrates these steps for the upper modules assuming
 * that vmw->new_dev is set to mydev_new.
 *
 *    // sub class of Tvgui_devcanvas of upper module
 *    struct Tmydev {
 *      Tvgui_devcanvas any;
 *      int field1; // Tvgui_devcanvas extension 1
 *      int field2; // Tvgui_devcanvas extension 2
 *      ...
 *    };
 *
 *    extern void mydev_draw(Tdevice* dev, void* ud) {
 *      Tmydev * mydev = (Tmydev*) dev;
 *      ... 
 *    }
 *
 *    extern Tvgui_devcanvas* mydev_new(Tvgui_mw* vmv,
 *         void (*clear)(Cdevice*,void*), Twid_canvas* canvas) {
 *      Tmydev* mydev = tool_malloc_0( sizeof(*mydev) );
 *      dev_init( (Tdevice*)mydev, 1.0, 0, 0, clear, mydraw, vmw);
 *      // initialization of the proper field of Tmydev
 *      ... 
 *      return &mydev->any;
 *   }
 *
 * For canvas widgets that are sensitive to mouse, the mouse event are
 * transmitted to upper module through the EFSM interface.
 * To use it, you must create a Tvgui_efsm object and make the
 * vgcm_efsm canvas pointer points to the efsm object.
 * Then the transition function of the efsm object will be called every
 * time that a mouse event occurs.
 * Notice that vgcm_efsm canvas pointer can be set to null, in this
 * case the mouse events are ignored.
**======================================================================*/
#endif

/*======================================================================*/
#ifndef FILE_VGUI_WID_CANVAS_H
#define FILE_VGUI_WID_CANVAS_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "vgui-mw.h"

#include "tools/boxes.h"
#include "tools/bitmap-types.h"
#include "tools/grid.h"

/*======================================================================*/

typedef struct _Twidcvs_geomCB Twidcvs_geomCB;
typedef const   Twidcvs_geomCB Cwidcvs_geomCB;

typedef struct _Twidcvs_ruler {
    Tegs_widget      widEGS;
    Twid_canvas*     canvas;      /* parent canvas */
    Tvgui_odpixmap*  pdev;
    TpixmapEGS       pxm;         /* base pixmap */
    int              currMkrPos;  // in pixel
    int              fixdim;      // fixed height (hruler) or
                                  // width (vruler) in pixel
} Twidcvs_ruler;
typedef struct _Twidcvs_ruler Twidcvs_ruler;
typedef const   Twidcvs_ruler Cwidcvs_ruler;
typedef Twidcvs_ruler Twid_canvas_scale; // obsolete

typedef struct _Twid_canvas_area {
    Tegs_widget      widEGS;
    Twid_canvas*     canvas;        /* parent canvas */
    Tvgui_odcanvas*  outdev;
    Tvgui_efsm*      automata;
} Twid_canvas_area;

struct _Twid_canvas {
    Tvgui_mw*  cvs_vmw;
#define        cvs_gui cvs_vmw->MW_gui
#define vgcm_mw cvs_vmw
#define        vgcm_gui vgcm_mw->MW_gui
    CstrStatic vgcm_wn; // widget resource name 

    int   vgcm_mode;   // 0:enable input event and growing/filling size;
                       // 1:no input event and fixed size
                       // 2:no input event and SE growing size
    int   vgcm_scale;  // 1:with scales ; 0:without scales

    // vgcm_daBW/Width/Height/HVSep are only used when vgcm_mode is 0.
    int   vgcm_daBW;     // Border Width of the drawing area in pixel
    int   vgcm_daWidth;  // width  of drawing area in pixel
    int   vgcm_daHeight; // height of drawing area in pixel
    int   vgcm_daHVSep ; // top, bottom, left, right separators in pixel

    Twidcvs_ruler cvs_hruler;
    Twidcvs_ruler cvs_vruler;
#define vgcm_horizontalScale cvs_hruler
#define vgcm_verticalScale   cvs_vruler
#define                vgcm_outdev   vgcm_drawingArea.outdev
#define                vgcm_automata vgcm_drawingArea.automata
#define                vgcm_efsm     vgcm_drawingArea.automata
    Twid_canvas_area   cvs_drawingArea;
    Tegs_widget        cvs_widEGS;
#define vgcm_drawingArea cvs_drawingArea
#define vgcm_widEGS      cvs_widEGS
    Tgrid*             cvs_grid;
    Twidcvs_geomCB*    cvs_geomcb;
    int                cvs_drawAxis;
};

/*======================================================================*/
/*= VGUI interface                                                     =*/

extern Twid_canvas* widM_canvas_new(Tvgui_mw*vmw,
                        int mode, int withscales,
                        int daBW, int daWidth, int daHeight, int daHVSep);

extern Twid_canvas* widD_canvas_new(Tvgui_dw*d,
                        int mode, int withscales,
                        int daBW, int daWidth, int daHeight, int daHVSep);

extern void         wid_canvas_dbf(Twid_canvas*self, Tegs_widget pw);

extern void         wid_canvas_free(Twid_canvas* wid);

/**
 * The wid_canvas_clear function clears the drawing area of the self
 * canvas. It is a shortcut to
 *     dev_clear((Tdevice*)self->vgcm_device);
 *
 * The wid_canvas_draw function draws the picture into the drawing area
 * of the self canvas. It is a shortcut to
 *    dev_draw((Tdevice*)self->vgcm_device,ud);
 * 
 * The wid_canvas_redraw function clears the the drawing area of the self
 * canvas and then draws draws the picture into the drawing area. It is
 * a shortcut to
 *    dev_clear((Tdevice*)self->vgcm_device);
 *    dev_draw((Tdevice*)self->vgcm_device,ud);
 *    
 * The wid_canvas_drawFsm function invokes the transition function associated
 * to the self canvas:
 *   vgui_efsm_transition(self->vgcm_efsm,AUTOMATA_IN_REDRAW,0,0);
 * This usually draws transient objects such as preview or markers that the
 * FSM handle.
 * If the self canvas does not run a FSM, the function does nothing.
**/
extern void wid_canvas_clear    (Twid_canvas* self);
extern void wid_canvas_draw     (Twid_canvas* self, void* ud);
extern void wid_canvas_redraw   (Twid_canvas* self, void* ud);
extern void wid_canvas_drawFsm  (Twid_canvas* self);
extern void wid_canvas_drawGrid (Cwid_canvas* self);
extern void wid_canvas_drawAxis (Cwid_canvas* self);

/*======================================================================*/
/**
 * The widM_canvas_nbf0 function creates a canvas widget with scales
 * as a child of the pw widget.
 * The canvas widget is mouse sensitive and at layout level, it fills
 * the room as much as possible.
 *
 * Return:
 *   The function returns a pointer to the canvas widget.
 *
 * Parameters:
 *  daBW: the border thickness of the drawing area window (in pixel).
 *  daWidth/heigh: the minimal size of drawing area window (in pixel).
 *  daHVSep: the amount of space in pixel left between the borders of
 *           the top window and its sub-windows (scales and drawing
 *           area windows).
**/

extern Twid_canvas* widM_canvas_nbf0(Tvgui_mw*vmw, Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep);
extern Twid_canvas* widM_canvas_nbf1(Tvgui_mw*vmw, Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep);

extern Twid_canvas* widD_canvas_nbf1(Tvgui_dw*d, //Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep);
extern Twid_canvas* widD_canvas_nbf2(Tvgui_dw*d, //Tegs_widget pw,
                        int daBW, int daWidth, int daHeight, int daHVSep);

/*======================================================================*/
/* Scale, axis, grid, magnetic grid                                    =*/

extern void wid_canvas_axisEnable   (Twid_canvas*self, int on);

extern void wid_canvas_unitSet      (Twid_canvas* self, CuserUnit* uu);
extern void wid_canvas_unitSetGrid  (Cwid_canvas*self, double stn);
extern void wid_canvas_unitSetMagnet(Cwid_canvas*self, double stn);

extern void wid_canvas_unitEnableGrid   (Cwid_canvas*self, int on);
extern void wid_canvas_unitEnableMagnet (Cwid_canvas*self, int on);

extern void wid_canvas_unitDevGeomChanged(Cwid_canvas*self);

// return 1 if zoom changed
extern int wid_canvas_setZoomNW (Cwid_canvas*self, double zf);
extern int wid_canvas_setZoomCE (Cwid_canvas*self, double zf);

/*======================================================================*/
/*= EGS interface                                                      =*/

extern Tegs_widget  gui_canvas_buildTop     (Twid_canvas* canvas,
                                             Tegs_widget parent);
extern void         gui_canvas_finishTop    (Twid_canvas* canvas);

extern Tegs_widget  gui_canvas_buildScale   (Twid_canvas_scale* scale,
                                             int horizontal);
extern Tegs_widget  gui_canvas_buildArea    (Twid_canvas_area*  area);

extern void         gui_canvas_clear        (Twid_canvas_area*  area);
//extern void         gui_canvas_clearRect    (Twid_canvas_area*  area,
//                        // x, y, dx, dy in screen pixel
//                        int x, int y, int dx, int dy);

/*======================================================================*/
/*= VGUI utilities for EGS                                             =*/

/**
 * The wid_canvas_boxPixUpdated updates the boxSrc size from the boxDest
 * size and the homothety ratio. If the boxSrc size changed, the picture
 * is updated in the drawing area.
 * 
 * This function is dedicated to EGS that can call it when the size of
 * the drawing area changed.
**/
extern void wid_canvas_boxPixUpdated(Twid_canvas* self);

#define WIDCVS_WH_DA       0      // drawing area
#define WIDCVS_WH_HR       1      // horizontal ruler
#define WIDCVS_WH_VR       2      // vertical ruler

#define WIDCVS_DA_Enter    0x0001 // 0   , 0 , 0   , 0   , 0
#define WIDCVS_DA_Leave    0x0002 // 0   , 0 , 0   , 0   , 0
#define WIDCVS_DA_Motion   0x0003 // x   , y , 0   , 0   , 0
// len:  1:1 step ; 2:2 step ; 3:3 step ... 
#define WIDCVS_DA_BtnDown  0x0004 // x   , y , btn , kmod, len
#define WIDCVS_HR_BtnDown  0x0005 // x   , y , btn , kmod, len
#define WIDCVS_VR_BtnDown  0x0006 // x   , y , btn , kmod, len
#define WIDCVS_DA_BtnUp    0x0007 // x   , y , btn , kmod, 0

// w (where) see WIDCVS_WH  
#define WIDCVS_KeyDown     0x0010 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyUp       0x0020 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyLeft     0x0030 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyRight    0x0040 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyPgDown   0x0050 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyPgUp     0x0060 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyZoomP    0x0070 // w   , kmod , 0 , 0 , 0
#define WIDCVS_KeyZoomM    0x0080 // w   , kmod , 0 , 0 , 0

extern void wid_canvas_action(Twid_canvas* self, int kind,
                     int p0, int p1, int p2, int p3, int p4);

/*======================================================================*/
/*= macros for debuging                                                =*/

#define DBG0_Canv(fmt,...)  DBG0_printf("canvas",fmt,__VA_ARGS__)
#define DBG1_Canv(fmt,...)  DBG1_printf("canvas",fmt,__VA_ARGS__)
#define DBG2_Canv(fmt,...)  DBG2_printf("canvas",fmt,__VA_ARGS__)

#define DBG0_CanvE(fmt,...) DBG0_printf("canvas","Enter : "fmt,__VA_ARGS__)
#define DBG0_CanvM(fmt,...) DBG0_printf("canvas","Middle: "fmt,__VA_ARGS__)
#define DBG0_CanvL(fmt,...) DBG0_printf("canvas","Leave : "fmt,__VA_ARGS__)
#define DBG1_CanvE(fmt,...) DBG1_printf("canvas","Enter : "fmt,__VA_ARGS__)
#define DBG1_CanvM(fmt,...) DBG1_printf("canvas","Middle: "fmt,__VA_ARGS__)
#define DBG1_CanvL(fmt,...) DBG1_printf("canvas","Leave : "fmt,__VA_ARGS__)
#define DBG2_CanvE(fmt,...) DBG2_printf("canvas","Enter : "fmt,__VA_ARGS__)
#define DBG2_CanvM(fmt,...) DBG2_printf("canvas","Middle: "fmt,__VA_ARGS__)
#define DBG2_CanvL(fmt,...) DBG2_printf("canvas","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif //FILE_VGUI_WID_CANVAS_H
/*======================================================================*/
