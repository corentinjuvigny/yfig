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
 *$Software: YFIG-TMB (Top Menu Bar)
 *      $Id: tb-file-dw-OMF.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/tb-file-dw-OMF.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Popup dialog for open/merge/save-as YFIG files                     =*/
/*=                                                                    =*/
/*= See the tb-file-dw-OMSF.h header for documentation.                =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/tb-file-OMSF.h"
#include "yfig-device.h"
#include "vgui/vgui-outdev.h"
#include "yfig-dsds.h"
#include "yfig-mw.h"

#include "vgui/vgui.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-filebrowser.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"

#include "tools/boxes.h"
#include "tools/globalvars.h"

/*======================================================================*/

#define DBG0_E(fmt,...) DBG0_printf("dialomsf","Enter : "fmt,__VA_ARGS__)
#define DBG0_M(fmt,...) DBG0_printf("dialomsf","Middle: "fmt,__VA_ARGS__)
#define DBG0_L(fmt,...) DBG0_printf("dialomsf","Leave : "fmt,__VA_ARGS__)
#define DBG1_E(fmt,...) DBG1_printf("dialomsf","Enter : "fmt,__VA_ARGS__)
#define DBG1_M(fmt,...) DBG1_printf("dialomsf","Middle: "fmt,__VA_ARGS__)
#define DBG1_L(fmt,...) DBG1_printf("dialomsf","Leave : "fmt,__VA_ARGS__)
#define DBG2_E(fmt,...) DBG2_printf("dialomsf","Enter : "fmt,__VA_ARGS__)
#define DBG2_M(fmt,...) DBG2_printf("dialomsf","Middle: "fmt,__VA_ARGS__)
#define DBG2_L(fmt,...) DBG2_printf("dialomsf","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/

static CboxDble* dial_OMSFprev_getBB(Coutdev*dev, void*ud)
{
    Cyfig_device* ydev = (Cyfig_device*)dev;
    if ( fsod_getScene(ydev) )
        return obj_BBox( fsod_getCurrObj(ydev) ) ;
    else
        return 0;
}

extern Tdial_OMSFprev* dial_OMSFprev_new(Tvgui_dw* pd)
{
    Tdial_OMSFprev* dp = (Tdial_OMSFprev*)
                          vgui_dw_newSub( sizeof(*dp),pd,0);
    Tegs_widget self = dp->wd_widEGS;

    // line 1
    dp->df_titleLAB    = widD_label_nbf(&dp->any, "Preview of ", 0);
    dp->df_fileNameLAB = widD_label_nbf(&dp->any, "", 0);

    // line 2, column 1
    dp->df_objnb = widD_olvu_nbfN10T (&dp->any,0,"999","object number",0);
    dp->df_size  = widD_olvu_nbfN11TU(&dp->any,0,"9999.9",0,"size",0);

    dp->df_commTitleLAB = widD_label_nbf(&dp->any, "comment ", 0);
    dp->df_commValueTOU = widD_mlout_nbf(&dp->any, 0, 0, 0);

    // line 2, column 2
    dp->df_canvas = widD_canvas_nbf2(&dp->any,
            0, DFGEO_PV_CANVAS_WIDTH, DFGEO_PV_CANVAS_HEIGHT,
            DFGEO_PV_CANVAS_HVSEP);

    // let gui to finish the work
    gui_OMSFprev_dbf(dp);

    return dp;
}

extern void      dial_OMSFprev_free     (Tdial_OMSFprev*dp)
{
    if ( dp->Fscene!=0 ) fig_scene_delete( dp->Fscene );
    wid_olvu_free  ( dp->df_objnb);
    wid_olvu_free  ( dp->df_size);
    wid_mlout_free ( dp->df_commValueTOU);
    wid_canvas_free( dp->df_canvas );
    vgui_dw_free   (&dp->any);
}

/*======================================================================*/

/**
 * dial_OMSFpara_cb_scaleChanged updates dx and dy widgets using the
 * new scale values.
**/
static void dial_OMSFpara_cb_scaleChanged(
        Tvgui_dw*d, Twid_ilvu*iw, double nv)
{
    Tdial_OMSFpara* dp = (Tdial_OMSFpara*)d;
    Tdial_OMSF*  df = (Tdial_OMSF*)dp->wd_pDialog;
    DBG0_E("dial=%p prev=%p para=%p iw=%p nv=%f",
            df,df->df_preview,df->df_param,iw,nv);

    Tfigure* fig = df->df_fig;

    if (fig==0) return;

    CboxDble* box = obj_BBox( obj_ObjAll(fig->objects) );
    wid_olvu_setNum10(dp->df_dx, Box_dx(*box)*nv);
    wid_olvu_setNum10(dp->df_dy, Box_dy(*box)*nv);
    DBG1_L("dial=%p prev=%p para=%p iw=%p nv=%f",
            df,df->df_preview,df->df_param,iw,nv);
}

/**
 * dial_OMSFpara_resets the widgets if df->df_fig is null,
 * otherwise it updates the widgets with the values of df->df_fig.
**/
static void dial_OMSFpara_update(Tdial_OMSF*df)
{

    Tdial_OMSFpara* dp = df->df_param;
    Tfigure* fig = df->df_fig;

    if ( dp==0 ) return;

    if ( fig==0 ) {
        wid_ilvu_reset(dp->df_xINP);
        wid_ilvu_reset(dp->df_yINP);
        wid_olvu_clear(dp->df_dx);
        wid_olvu_clear(dp->df_dy);
        wid_ilvu_reset(dp->df_scaleDIN);
        wid_ilvu_disable(dp->df_xINP);
        wid_ilvu_disable(dp->df_yINP);
        wid_ilvu_disable(dp->df_scaleDIN);
    } else {
        CboxDble* box = obj_BBox( obj_ObjAll(fig->objects) );
        wid_ilvu_setNum10(dp->df_xINP,Box_x(*box));
        wid_ilvu_setNum10(dp->df_yINP,Box_y(*box));
        wid_olvu_setNum10(dp->df_dx, Box_dx(*box));
        wid_olvu_setNum10(dp->df_dy, Box_dy(*box));
        wid_ilvu_setNum10(dp->df_scaleDIN,1);
        wid_ilvu_enable(dp->df_xINP);
        wid_ilvu_enable(dp->df_yINP);
        wid_ilvu_enable(dp->df_scaleDIN);
    }
}

extern Tdial_OMSFpara* dial_OMSFpara_new(Tvgui_dw* pd)
{
    Tdial_OMSFpara* dp = (Tdial_OMSFpara*) vgui_dw_newSub(
                                                sizeof(*dp),pd,0);
    Tegs_widget self = dp->wd_widEGS;

    // build the sub widget
    dp->df_xINP = wid_ilvu_newD( 0.0, "99999.99", NULL,NULL,NULL, 0,0);
      wid_ilvu_addLabel  ( dp->df_xINP, "x offset", 0);
      wid_ilvu_addSpinInc( dp->df_xINP, 120 );
      wid_ilvu_addUnitStd( dp->df_xINP );
      widD_ilvu_dbf(dp->df_xINP,&dp->any);
    dp->df_yINP = wid_ilvu_newD( 0.0, "99999.99", NULL,NULL,NULL, 0,0);
      wid_ilvu_addLabel  ( dp->df_yINP, "y offset", 0);
      wid_ilvu_addSpinInc( dp->df_yINP, 120 );
      wid_ilvu_addUnitStd( dp->df_yINP );
      widD_ilvu_dbf(dp->df_yINP,&dp->any);

    dp->df_scaleText = "scale\nfactor";
    dp->df_scaleDIN  = wid_ilvu_newD( 1.0, "99.99",
            0,dial_OMSFpara_cb_scaleChanged,NULL, 1e-2, 1e+2);
    wid_ilvu_addLabel( dp->df_scaleDIN, dp->df_scaleText, 0);
    wid_ilvu_addSpinMul( dp->df_scaleDIN, 1.25 );
    widD_ilvu_dbf(dp->df_scaleDIN,&dp->any);

    dp->df_dx = widD_olvu_nbfN10TU(&dp->any,0,"9999.99","width",0);
    dp->df_dy = widD_olvu_nbfN10TU(&dp->any,0,"9999.99","height",0);

    // let gui to finish the work
    gui_OMSFpara_dbf(dp);

    return dp;
}

extern void      dial_OMSFpara_free     (Tdial_OMSFpara*dp)
{
    wid_ilvu_free ( dp->df_xINP );
    wid_ilvu_free ( dp->df_yINP );
    wid_ilvu_free( dp->df_scaleDIN);
    wid_olvu_free( dp->df_dx );
    wid_olvu_free( dp->df_dy );
    vgui_dw_free(&dp->any);
}

/*======================================================================*/

/**
 * If fig pointer is not NULL the function updates the items of the
 * preview dialog of df dialog with data of the fig figure.
 * Otherwise it sets the items to their default value.
 * The path parameter is printed such as in the title item. A NULL
 * path is interpreted as the empty string ("").
**/
static void dial_OMSFprev_update( Tdial_OMSF*df, const char*path)
{
    char              buf[100];
    Tvgui*            gui = df->wd_gui;
    Tvgui_mw*         vmw = df->wd_vmw;
    Tyfig_mw*         yds = (Tyfig_mw*)vmw;
    Tdial_OMSFprev*   dp  = df->df_preview;
    Tvgui_odcanvas*   vod = dp->df_canvas->vgcm_outdev;

    if (dp==0) return;
    if ( dp->Fscene ) {
            Tfig_scene* scene = dp->Fscene;
            dp->Fscene  = 0;
            od_detach( &vod->vv_od );
            fig_scene_delete( scene );
    }

    od_clear( &vod->vv_od );

    Tfigure* fig = df->df_fig;
    if ( fig==0 ) {
        wid_label_setTextSS(dp->df_fileNameLAB,"");
        wid_olvu_clear( dp->df_objnb);
        wid_olvu_clear( dp->df_size);
        wid_mlout_setText( dp->df_commValueTOU, "");
    } else {
        wid_label_setTextSS(dp->df_fileNameLAB,path?path:"");
        wid_olvu_setNum10(dp->df_objnb,
                fig_objcnt_sum(&fig->objCounters) );
        CboxDble* box = obj_BBox( obj_ObjAll(fig->objects) );
        wid_olvu_setNum11(dp->df_size, Box_dx(*box), Box_dy(*box));
        wid_mlout_setText(dp->df_commValueTOU, ""); // FIXME
#if 1 // FIXME
        dp->Fscene = fig_scene_newByFig(fig);
        fig_scene_attachDev(dp->Fscene,&vod->vv_od, 0); // 0: fit src to des
        od_draw( &vod->vv_od );
#endif
    }
}

static void cb_OMSF_preview(Tvgui_dw*d,const char*path)
{
    Tdial_OMSF* df = (Tdial_OMSF*)d;
    Tvgui*      gui = df->wd_gui;
    Tvgui_mw*   vmw = df->wd_vmw;
    Tyfig_mw*   yds = (Tyfig_mw*)vmw;
    Tyfig_dsds* dsds  = gui->g_ud;
    DBG0_E("dial=%p prev=%p para=%p path=%s",
            df,df->df_preview,df->df_param,path?path:"null");

    if ( path==0 ) {
        if ( df->df_browser->existingPath )
            vgui_dw_disableBUTcur(d, WID_DIALSTATUS_Ok );
        df->df_fig = 0;
        dial_OMSFprev_update(df,0);
        dial_OMSFpara_update(df);
        DBG1_L("dial=%p prev=%p para=%p path=%s",
            df,df->df_preview,df->df_param,path?path:"null");
        return;
    }

    df->df_fig = fig_new_byFile(&gl_msgdrv_silence,path,dsds->ds_baseColors);
    if ( df->df_fig==0 ) {
        if ( df->df_browser->existingPath )
            vgui_dw_disableBUTcur(d, WID_DIALSTATUS_Ok );
        dial_OMSFprev_update(df,0);
        dial_OMSFpara_update(df);
        DBG1_L("dial=%p prev=%p para=%p path=%s",
            df,df->df_preview,df->df_param,path?path:"null");
        return ;
    }
    fig_update_forDrawing(df->df_fig,gui->colorDriver);
    vgui_dw_enableBUTcur(d, WID_DIALSTATUS_Ok );
    dial_OMSFprev_update(df,path);
    dial_OMSFpara_update(df);
    DBG1_L("dial=%p prev=%p para=%p path=%s",
        df,df->df_preview,df->df_param,path?path:"null");
}

/*======================================================================*/

extern Tdial_OMSF* dial_OMSF_new(Tyfig_mw* yds, Tdial_OMSFkind kd,
                                 CstrBrief dir, CstrBrief savebn)
{
    char* title = tool_aprintf("%s",
            kd == OMSF_KD_Open   ? "Open File"
          : kd == OMSF_KD_Merge  ? "Merge File"
          : kd == OMSF_KD_SaveAs ? "Save File As"
          : "???"
    );

    Tdial_OMSF* d = (Tdial_OMSF*) vgui_dw_newTop(
            sizeof(*d),&yds->any, title, WID_DIALSTATUS_CancelX);

    d->df_kind  = kd;

    if ( kd==OMSF_KD_Open ||  kd==OMSF_KD_Merge ) {
        d->df_preview = dial_OMSFprev_new(&d->any);
        d->df_param   = dial_OMSFpara_new(&d->any);
    }

    if ( kd==OMSF_KD_Merge ) {
        d->df_browser = wid_filebrowser_new(&d->any,1,cb_OMSF_preview,dir);
        tool_strchgClone(&d->df_browser->okText,"Merge");
    } else if ( kd==OMSF_KD_Open ) {
        d->df_browser = wid_filebrowser_new(&d->any,0,cb_OMSF_preview,dir);
        tool_strchgClone(&d->df_browser->okText,"Open");
        tool_strchgClone(&d->df_browser->extraText,"Cancel and\nstart a new figure");
                          d->df_browser->extraStatus = WID_DIALSTATUS_New;
    } else {
        d->df_browser = wid_filebrowser_new(&d->any,0,0,dir);
        tool_strchgClone(&d->df_browser->okText,"Save");
    }
    d->df_browser->filemaskId = tool_strdup( "XFIG"  );
    d->df_browser->filemask   = tool_strdup( "*.fig" );

    // update the filename for starting when it is needed
    if ( savebn!=0 ) {
        Tstr fn = tool_aprintf("%s/%s",d->df_browser->dir,savebn);
        vgui_filebrowser_initSaveFilName(d->df_browser,fn);
        tool_free( fn );
    }

    // let gui to finish the work
    wid_filebrowser_dbf(d->df_browser);
    vgui_dw_finishIC(&d->any,
      VDWLF_EH,d->df_preview ? d->df_preview->wd_widEGS : NULL,
      VDWLF_EH,d->df_param   ? d->df_param->  wd_widEGS : NULL,
      VDWLF_EB,d->df_browser ? d->df_browser->widEGS    : NULL,
     -1);
    gui_OMSF_newFinish(d);

    //dial_OMSFprev_update(d,0,0);

    return d;
}

extern void            dial_OMSF_free(Tdial_OMSF* d)
{
    if ( d->df_preview ) dial_OMSFprev_free    ( d->df_preview );
    if ( d->df_param )   dial_OMSFpara_free    ( d->df_param   );
    if ( d->df_browser ) wid_filebrowser_delete( d->df_browser );
    vgui_dw_free( &d->any );
}

extern TstrEaten dial_OMSF_getParam(Tdial_OMSF*df,
                                double *x, double *y, double *s)
{
    TstrEaten  path = vgui_filebrowser_getFilPath(df->df_browser);

    if ( path==0 || df->df_param==0 ) {
        if (x) *x=0;
        if (y) *y=0;
        if (s) *s=1;
        return path;
    }

    Tdial_OMSFpara* dp = df->df_param;
    if (x) *x = wid_ilvu_getNum10(dp->df_xINP);
    if (y) *y = wid_ilvu_getNum10(dp->df_yINP);
    if (s) *s = wid_ilvu_getNum10(dp->df_scaleDIN );
    return path;
}

extern TstrEaten dial_OMSF_getDir(Tdial_OMSF*df)
{
    return vgui_filebrowser_getDirPath( df->df_browser );
}

/*======================================================================*/
