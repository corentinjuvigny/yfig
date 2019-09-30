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
 *      $Id: wid-canvas-action.c 108 2018-01-07 17:34:36Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-canvas-action.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A drawing area widget with optional scales.                        =*/
/*=                                                                    =*/
/*= Implementation of communication from EGS canvas to VGUI canvas.    =*/ 
/*=                                                                    =*/
/*= See the vgui/vgui-canvas.h header for documentation.               =*/
/*======================================================================*/

#include "vgui-config.h"
#include "wid-canvasI.h"
#include "vgui/wid-canvas-geomcb.h"

#include "vgui/vgui-efsm.h"
#include "vgui/vgui-outdev.h"
#include "vgui/wid-egs.h"

/*======================================================================*/

extern void wid_canvas_boxPixUpdated(Twid_canvas* self)
{
    int ret;
    Tvgui_outdev* vod = &self->vgcm_outdev->vv_vod;
    DBG1_CanvE("canvas=%p: pix : %fx%f -- ds : %fx%f -- scale : %f",
        self,
        Box_dx(vod->v_boxPIX),Box_dy(vod->v_boxPIX),
        Box_dx(vod->v_boxDS), Box_dy(vod->v_boxDS),
        vod->v_homothetyDS2PIX);
    ret = od_updateSrcBB(&vod->v_od);
    if ( ret==1 ) {
        wid_canvas_drawRulers(self);
        wid_canvas_redraw(self,NULL);
    }
    DBG1_CanvL(
        "canvas=%p: pix : %fx%f -- ds : %fx%f -- scale : %f --ret=%d",
        self,
        Box_dx(vod->v_boxPIX),Box_dy(vod->v_boxPIX),
        Box_dx(vod->v_boxDS), Box_dy(vod->v_boxDS),
        vod->v_homothetyDS2PIX,ret);
}

static void wid_canvas_shiftFig(Twid_canvas*self, int shx, int shy, int nb)
{
    Tvgui_outdev* vod = &self->vgcm_outdev->vv_vod;
    if ( nb!=0 ) {
        shx = 10*shx*(nb);
        shy = 10*shy*(nb);
    }
    Box_x(vod->v_boxDS) += VCONV_D_D2S(vod,shx); 
    Box_y(vod->v_boxDS) += VCONV_D_D2S(vod,shy); 
    wid_canvas_drawRulers(self);
    wid_canvas_redraw(self,NULL);
    return;
}

static void wid_canvas_zoom(Twid_canvas*self,int plus,int nb)
{
    Twidcvs_geomCB* limits = self->cvs_geomcb;
    Tvgui_outdev*   vod = &self->vgcm_outdev->vv_vod;
    int i;
    if ( limits==0 ) return;

    double zoom_old = od_getUsrZoom(&vod->v_od);
    double zoom_new = zoom_old;
    double zoom_fac = plus>0 ? limits->stepZoom : 1./limits->stepZoom;
    for (i=0 ; i<nb ; i+=1) zoom_new *= zoom_fac;
    if ( zoom_new<limits->minZoom ) 
        zoom_new = limits->minZoom;
    else if ( zoom_new>limits->maxZoom )
        zoom_new = limits->maxZoom;
    if ( zoom_new==zoom_old ) return;
    if ( wid_canvas_setZoomCE(self,zoom_new)!=0 ) {
            wid_canvas_redraw (self,NULL);
            if ( limits->cb )
                limits->cb(self);
    }
}

extern void wid_canvas_action(Twid_canvas* self, int kind,
                     int p0, int p1, int p2, int p3, int p4)
{
    int shx,shy,rulelen,nb;
    Twidcvs_ruler* ruler;
    Tvgui_efsm*   fsm  =  self->vgcm_efsm;
    Tvgui_outdev* vod  = &self->vgcm_outdev->vv_vod;
    double xfu,yfu;
    switch (kind) {
      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      case WIDCVS_DA_Enter:
        if (fsm==0) return;
        vgui_efsm_transition( fsm,EFSMIN_EnterCanvas,0,0);
        return;
      case WIDCVS_DA_Leave:
        wid_canvas_clearRulersMkr(self);
        if (fsm==0) return;
        vgui_efsm_transition( fsm,EFSMIN_LeaveCanvas,0,0);
        return;
      case WIDCVS_DA_Motion:
        if (fsm==0) return;
        wid_canvas_drawRulersMkr(self,p0,p1);
        xfu = VCONV_X_D2S(vod,p0);
        yfu = VCONV_Y_D2S(vod,p1);
        vgui_efsm_transition(fsm,EFSMIN_MouseMotion,xfu,yfu);
        return;

      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      case WIDCVS_DA_BtnDown:
        xfu = VCONV_X_D2S(vod,p0);
        yfu = VCONV_Y_D2S(vod,p1);
        switch ( p2 ) {
            case 1:
                if (fsm==0) return;
                wid_canvas_drawRulersMkr(self,p0,p1);
                vgui_efsm_transition(fsm,EFSMIN_But1Pressed,xfu,yfu);
                return;
            case 2:
                if (fsm==0) return;
                wid_canvas_drawRulersMkr(self,p0,p1);
                vgui_efsm_transition(fsm,EFSMIN_But2Pressed,xfu,yfu);
                return;
            case 3:
                if (fsm==0) return;
                wid_canvas_drawRulersMkr(self,p0,p1);
                vgui_efsm_transition(fsm,EFSMIN_But3Pressed,xfu,yfu);
                return;
            case 4:
                if ( p3==0 )
                    wid_canvas_shiftFig(self,0,1,p4);
                else
                    wid_canvas_zoom(self,1,p4);
                if ( fsm ) wid_canvas_drawRulersMkr(self,p0,p1);
                return;
            case 5:
                if ( p3==0 )
                    wid_canvas_shiftFig(self,0,-1,p4);
                else
                    wid_canvas_zoom(self,-1,p4);
                if ( fsm ) wid_canvas_drawRulersMkr(self,p0,p1);
                return;
            default:TEMP_spe_printf("unexpected button number %d",p2);
        }
        return;

      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      case WIDCVS_HR_BtnDown:
        ruler = &self->cvs_hruler;
        switch ( p2 ) {
            case 1:
            case 2:
            case 3:
                rulelen = vgui_widegs_getDX(ruler->widEGS);
                shx     = p0<(rulelen/2) ? -rulelen/3 : rulelen/3;
                wid_canvas_shiftFig(self,shx,0,0);
                return;
            case 4:
                wid_canvas_shiftFig(self,1,0,p4);
                return;
            case 5:
                wid_canvas_shiftFig(self,-1,0,p4);
                return;
            default:TEMP_spe_printf("unexpected button number %d",p2);
        }
        return;

      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      case WIDCVS_VR_BtnDown:
        ruler = &self->cvs_vruler;
        switch ( p2 ) {
            case 1:
            case 2:
            case 3:
                rulelen = vgui_widegs_getDY(ruler->widEGS);
                shy     = p1<(rulelen/2) ? -rulelen/3 : rulelen/3;
                wid_canvas_shiftFig(self,0,shy,0);
                return;
            case 4:
                wid_canvas_shiftFig(self,0,1,p4);
                return;
            case 5:
                wid_canvas_shiftFig(self,0,-1,p4);
                return;
            default:TEMP_spe_printf("unexpected button number %d",p2);
        }
        return;

      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      case WIDCVS_KeyDown:  shx =  0; shy =  -1; nb=1; goto move_shx_shy;
      case WIDCVS_KeyUp:    shx =  0; shy =  +1; nb=1; goto move_shx_shy;
      case WIDCVS_KeyLeft:  shx = -1; shy =   0; nb=1; goto move_shx_shy;
      case WIDCVS_KeyRight: shx = +1; shy =   0; nb=1; goto move_shx_shy;
move_shx_shy:
        wid_canvas_shiftFig(self,shx,shy,nb);
        if ( p0==WIDCVS_WH_DA )
            wid_canvas_drawRulersMkr(self,
                    self->cvs_hruler.currMkrPos,
                    self->cvs_vruler.currMkrPos);
        return;
      case WIDCVS_KeyPgDown:
      case WIDCVS_KeyPgUp:
        shx =  0;
        shy = vgui_widegs_getDY(self->vgcm_drawingArea.widEGS)/3;
        nb =  0;
        if ( kind==WIDCVS_KeyPgDown )
            shy = - shy;
        goto move_shx_shy;

      /*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
      default:
        TEMP_spe_printf("unexpected action: 0x%04x",kind);
    }
}

/*======================================================================*/
