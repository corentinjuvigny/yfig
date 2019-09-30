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
 *$Software: YFIG-DPP (Draw Parameter Panel)
 *      $Id: align.c 280 2018-07-05 23:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/dppanel/align.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Widget that show and allow user to modify the parameter for        =*/
/*= aligning object.                                                   =*/
/*=                                                                    =*/
/*= See the "dppanel.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-device.h"
#include "dppanel.h"

#include "vgui/vgui.h"
#include "vgui/vgui-outdev.h"
#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"
#include "fig/figure.h"
#include "fig/object.h"

/*======================================================================*/
/*= data structure                                                     =*/

typedef struct _Talign Talign;
typedef const   Talign Calign;

struct _Talign {
    Tdpp_l1l2base base;

    // dt_aligns gives the alignment operation that is presently shown
    TalignOpe dt_aligns[10]; // the alignement operations shown by widget
    int       dt_alignNb;    // number of alignement operations in dt_aligns
    int       dt_index;      // dt_aligns[dt_index] is the current alignement.
#define dt_alignCurr(s) (s)->dt_aligns[(s)->dt_index]
};

/*======================================================================*/
/*= Interface                                                          =*/

static void dpp_align_free(Tdpp_l1l2base* dt);
//static void dpp_align_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw);
static void dpp_align_inc (Tdpp_l1l2base* dt);
static void dpp_align_dec (Tdpp_l1l2base* dt);
static void dpp_align_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy);

static void dpp_align_sceneV(Tvgui*gui, Tyfig_device*fsod, double dx, double dy);
static void dpp_align_sceneH(Tvgui*gui, Tyfig_device*fsod, double dx, double dy);

extern Twid_button* dpp_align_new(Twid_buttonPanel* panel, int horiz)
{
    Tvgui*    gui = panel->vgbpm_gui;
    int       v3dy = dpp_l1l2base_getMaxV3Dy(gui,1);
    int       v3dx = v3dy;
    l1l2_none_adjust(gui,&v3dx,&v3dy);

    // build the button
    Twid_button* btn;
    Talign* self = tool_malloc_0( sizeof(*self) );
    self->dt_aligns[self->dt_alignNb++] = ALIGN_NONE;
    self->dt_aligns[self->dt_alignNb++] = horiz ? ALIGN_XSS : ALIGN_YSS;
    self->dt_aligns[self->dt_alignNb++] = horiz ? ALIGN_XCC : ALIGN_YCC;
    self->dt_aligns[self->dt_alignNb++] = horiz ? ALIGN_XGG : ALIGN_YGG;

    self->dt_free   = dpp_align_free;
  //self->dt_cb1    = dpp_align_cb1;
    self->dt_inc    = dpp_align_inc;
    self->dt_dec    = dpp_align_dec;
    self->dt_redraw = dpp_align_draw;
    btn = dpp_l1l2base_initBtnStdPXM3_V3VSep(&self->base, panel,
        horiz ? "Horiz." : "Vert.", "Align", v3dx,v3dy,1);

    // we attaches a scene to the dt_dev output device
    // and fill it with some geometric shapes for showing the alignment.
    Toutdev*      od     = &self->dt_dev->vv_od;
    Tyfig_device* fsod   = fsod_attachPt( &self->dt_dev->any);
    if ( horiz!=0)
        dpp_align_sceneH(gui,fsod,CONV_D_D2S(od,v3dx),CONV_D_D2S(od,v3dy));
    else
        dpp_align_sceneV(gui,fsod,CONV_D_D2S(od,v3dx),CONV_D_D2S(od,v3dy));

    return btn;
}

extern int    dpp_align_getVal(Twid_button* btn)
{ Talign* self = btn->but_ud; return dt_alignCurr(self); }

/*======================================================================*/
/*= Specialization of Tdpp_l1l2base to Talign                          =*/

static void dpp_align_free(Tdpp_l1l2base* dt)
{
    Talign*       self = (Talign*)dt;
    Tyfig_device* fsod = (Tyfig_device*)self->dt_dev;
    fsod_detach(fsod);
}

//static void dpp_align_cb1 (Tdpp_l1l2base* dt, Tyfig_mw* ymw);

static void dpp_align_inc (Tdpp_l1l2base* dt)
{ Talign* self=(Talign*)dt; self->dt_index = self->dt_index==(self->dt_alignNb-1) ?  0 : self->dt_index+1 ; }

static void dpp_align_dec (Tdpp_l1l2base* dt)
{ Talign* self=(Talign*)dt; self->dt_index = self->dt_index==0 ? self->dt_alignNb-1 : self->dt_index-1; }

static void dpp_align_draw(Tdpp_l1l2base* dt, Tvgui_outdev*vdev, double x, double y, double dx, double dy)
{
    Talign*       self = (Talign*)dt;
    Tod_GCdef     gcd  =  OD_GC_SysDraw;
    Tyfig_device* fsod = (Tyfig_device*)vdev;
    Toutdev*      od   = &vdev->v_od;
    if ( dt_alignCurr(self)==ALIGN_NONE ) {
        dpp_none_draw(vdev,x,y,dx,dy);
    } else {
        od_style_pushTop(od,gcd);
        obj_alignD(fsod_getTopObj(fsod),dt_alignCurr(self));
        obj_translation(fsod_getCurrObj(fsod),x,y);
        od_draw(od);
        obj_translation(fsod_getCurrObj(fsod),-x,-y);
        od_style_pop(od,gcd);
    }
}

/*======================================================================*/
/*= scene builder                                                      =*/

static void dpp_align_sceneH(Tvgui*gui, Tyfig_device*fsod, double dx, double dy)
{
    TobjAll       root   = fsod_getCurrObj(fsod);
    CcolorDriver* clrdrv = gui->colorDriver;
    TobjAll       obj;
    double        pix2fu = LUNIT_FuPerInch/fsod_getPpiSrc(fsod);
    Tsty_line stl = odsty_copyLine(od_style_topLine(fsod_getOD(fsod),OD_GC_SysDraw));
    Tsty_fill stf = odsty_copyFill(od_style_topFill(fsod_getOD(fsod),OD_GC_SysFill));
    double x,y,alpha;
    y = 3;
    // add a triangle
      x=dx/2;
      double trdx = ((dx-4)*50)/100;
      double trdy = (dy - y -2 - 3)/2 - 1;
      Tdble2Set* pts = dble2set_new(3); // must not be deleted cause obj_line_newF eats it.
      color_chgEaten(&stf.stf_color,color_newByStr(clrdrv,"#F07070"));
      dble2set_addXY(pts,x, y);
      y += y+trdy;
      dble2set_addXY(pts,x+trdx/2, y);
      dble2set_addXY(pts,x-trdx/2, y);
      dble2set_homothety00(pts,pix2fu);
      obj = obj_line_newF(NULL,100,&stl,&stf,FIGOLI_T_Polygon,pts);
      obj_dir_addObj(root,obj);
    // add an ellipse
      x=dx/2;
      y += 2;
      double eldx = ((dx-4)*80)/100;
      double eldy = dy - y - 3;
      color_chgEaten(&stf.stf_color,color_newByStr(clrdrv,"#70F070"));
      Tdble2 pt1   = dble2_init((x-eldx/2)*pix2fu,(y)*pix2fu);
      Tdble2 pt2   = dble2_init((x+eldx/2)*pix2fu,(y+eldy)*pix2fu);
      alpha = 0; //DegToRad(-20);
      obj = obj_ellipse_newF(NULL,100,&stl,&stf,FIGOELL_T_EllByDia,pt1,pt2,alpha);
      obj_dir_addObj(root,obj);
    // add a rectangle large enough for containing the former objects
      color_chgEaten(&stl.stl_color,color_newByStr(clrdrv,"#7070F0"));
      stl.stl_thickness=2;
      pts = dble2set_new(4); // must not be deleted cause obj_line_newF eats it.
      dble2set_addXY(pts, 1    , 1);
      dble2set_addXY(pts, dx-1 , 1);
      dble2set_addXY(pts, dx-1 , dy-1);
      dble2set_addXY(pts, 1    , dy-1);
      dble2set_homothety00(pts,pix2fu);
      stf.stf_style = DA_FS_NoFill;
      obj = obj_line_newF(NULL,100,&stl,&stf,FIGOLI_T_Box,pts);
      obj_dir_addObj(root,obj);
    odsty_freeLine(&stl);
    odsty_freeFill(&stf);
}

static void dpp_align_sceneV(Tvgui*gui, Tyfig_device*fsod, double dx, double dy)
{
    TobjAll       root   = fsod_getCurrObj(fsod);
    CcolorDriver* clrdrv = gui->colorDriver;
    TobjAll       obj;
    double        pix2fu = LUNIT_FuPerInch/fsod_getPpiSrc(fsod);
    Tsty_line stl = odsty_copyLine(od_style_topLine(fsod_getOD(fsod),OD_GC_SysDraw));
    Tsty_fill stf = odsty_copyFill(od_style_topFill(fsod_getOD(fsod),OD_GC_SysFill));
    double x,y,alpha;
    x = 3;
    // add a triangle
      y=dy/2;
      double trdx = (dx - x -2 - 3)/2 - 1;
      double trdy = ((dy-4)*50)/100;
      Tdble2Set* pts = dble2set_new(3); // must not be deleted cause obj_line_newF eats it.
      color_chgEaten(&stf.stf_color,color_newByStr(clrdrv,"#F07070"));
      dble2set_addXY(pts,x+trdx/2, y-trdy/2);
      dble2set_addXY(pts,x       , y+trdy/2);
      dble2set_addXY(pts,x+trdx  , y+trdy/2);
      dble2set_homothety00(pts,pix2fu);
      obj = obj_line_newF(NULL,100,&stl,&stf,FIGOLI_T_Polygon,pts);
      obj_dir_addObj(root,obj);
      x += x+trdx;
    // add an ellipse
      y=dy/2;
      x += 2;
      double eldy = ((dy-4)*80)/100;
      double eldx = dx - x - 5;
      color_chgEaten(&stf.stf_color,color_newByStr(clrdrv,"#70F070"));
      Tdble2 pt1   = dble2_init((x)*pix2fu,     (y-eldy/2)*pix2fu);
      Tdble2 pt2   = dble2_init((x+eldx)*pix2fu,(y+eldy/2)*pix2fu);
      alpha = 0; //DegToRad(-20);
      obj = obj_ellipse_newF(NULL,100,&stl,&stf,FIGOELL_T_EllByDia,pt1,pt2,alpha);
      obj_dir_addObj(root,obj);
    // add a rectangle large enough for containing the former objects
      color_chgEaten(&stl.stl_color,color_newByStr(clrdrv,"#7070F0"));
      stl.stl_thickness=2;
      pts = dble2set_new(4); // must not be deleted cause obj_line_newF eats it.
      dble2set_addXY(pts, 1    , 1);
      dble2set_addXY(pts, dx-1 , 1);
      dble2set_addXY(pts, dx-1 , dy-1);
      dble2set_addXY(pts, 1    , dy-1);
      dble2set_homothety00(pts,pix2fu);
      stf.stf_style = DA_FS_NoFill;
      obj = obj_line_newF(NULL,100,&stl,&stf,FIGOLI_T_Box,pts);
      obj_dir_addObj(root,obj);
    odsty_freeLine(&stl);
    odsty_freeFill(&stf);
}

/*======================================================================*/
