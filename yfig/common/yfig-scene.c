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
 *$Software: YFIG
 *      $Id: yfig-scene.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-scene.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling scenes that are drawn in the Tyfig_mw       =*/
/*= widget.                                                            =*/
/*=                                                                    =*/
/*= See the "fig-scene.h" and "yfig-mw.h" headers for documentation.   =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-device.h"
#include "yfig-dsds.h"

#include "vgui/vgui.h"
#include "vgui/wid-canvas.h"
#include "fig/fig-histo.h"

#include "tools/globalvars.h"
#include "tools/file.h"

/*======================================================================*/
/*= Getters of the current scene.                                      =*/

extern Tfig_scene* yfig_cursce_scene (Cyfig_mw*ymw) { return fsod_getScene  ( YMW_ydev(ymw) ); }
extern Tfigure*    yfig_cursce_fig   (Cyfig_mw*ymw) { return fsod_getFigure ( YMW_ydev(ymw) ); }
extern Tfig_histo* yfig_cursce_histo (Cyfig_mw*ymw) { return fsod_getFigure ( YMW_ydev(ymw) )->history; }
extern TobjDir     yfig_cursce_topDir(Cyfig_mw*ymw) { return fsod_getTopObj ( YMW_ydev(ymw) ); }
extern TobjAll     yfig_cursce_curDir(Cyfig_mw*ymw) { return fsod_getCurrObj( YMW_ydev(ymw) ); }

/*======================================================================*/
/*= History managment.                                                 =*/

static void yfig_histo_pushMovObj (Tyfig_mw* ymw, TobjAll obj, Tdble2 vt)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushMovObj(histo, obj.any, vt);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

static void yfig_histo_pushAddObj (Tyfig_mw* ymw, Tobject obj)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushAddObj(histo, obj);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

static void yfig_histo_pushDelObj (Tyfig_mw* ymw, Tobject obj)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushDelObj(histo, obj);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

static void yfig_histo_pushDelObjSet(Tyfig_mw* ymw,
                                     TobjAll*objs, int objnb, TobjDir dir)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushDelObjsT(histo, objs, objnb, dir);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

static void        yfig_histo_pushCpdDo   (Tyfig_mw* ymw, TobjDir dir)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushCpdDo(histo, dir);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

static void        yfig_histo_pushCpdFlat(Tyfig_mw* ymw, TobjDir dir)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushCpdFlat(histo, dir);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

// FIXME: used once in modepannel/edit.c
extern void yfig_histo_pushChgObj  (Tyfig_mw* ymw, Tobject older,
                                                   Tobject latest)
{
    Tfig_histo* histo = yfig_cursce_histo( ymw );
    if ( histo ) {
        fig_histo_commit( histo );
        fig_histo_pushChgObj(histo, older, latest);
        yfig_topbar_update( ymw, YFIG_TB_EDIT );
    }
    ymw->ymw_currHistCurr += 1;
}

/*======================================================================*/
/*= changing/handling current scene.                                   =*/

extern void yfig_cursce_chgFigPath(Tyfig_mw*ymw, CstrBrief path)
{
    Tfigure* fig = yfig_cursce_fig(ymw);
    fig_chgPath(fig,path);
    yfig_mw_titleUpdate( ymw );
}

extern int yfig_cursce_isModified(Tyfig_mw*ymw)
{
    Tfig_histo*  histo = yfig_cursce_histo(ymw);
    return ymw->ymw_currHistSave!=ymw->ymw_currHistCurr;
}

// FIXME static void yfig_cursce_newUpdate(Tyfig_mw*ymw)
void yfig_cursce_newUpdate(Tyfig_mw*ymw)
{
    Tfigure* fig = YMW_figure(ymw);
    fig->history = fig_histo_new(fig,10);
    ymw->ymw_currHistSave = 0;
    ymw->ymw_currHistCurr = 0;
    if ( ymw->ymw_do_useFigUnit )
        ymw->ymw_userUnit = fig->settings.userUnit;

    // update the other widgets to the current scene
    yfig_canvas_updateUnit(ymw,NULL);
    yfig_mw_titleUpdate( ymw );
    yfig_topbar_update(ymw,YFIG_TB_ALL);
    yfig_dppanel_update( ymw );
    yfig_layers_updateFull( ymw );
}

extern void yfig_cursce_new(Tyfig_mw*ymw)
{
    Tvgui* gui       = ymw->ymw_gui;
    Tyfig_dsds* dsds = gui->g_ud;
    Tfigure*    fig  = fig_new_empty(ymw->ymw_msgdrv,"unamed.fig",0,dsds->ds_baseColors);
    // setting default user unit
    fig->settings.userUnit = dsds->ds_defaults.dsp_userUnit;
    Tfig_scene* scene = fig_scene_newByFig(fig);
    fsod_detachAttach( ymw_fsod(ymw), scene, 1 ); // 1: real size
    yfig_cursce_newUpdate( ymw );
}

extern int yfig_cursce_newByPath(Tyfig_mw*ymw, CstrBrief path)
{ return yfig_cursce_newByPathScaleAndTrans(ymw,path,0.0,0.0,0.0); }

extern int   yfig_cursce_newByPathScaleAndTrans(Tyfig_mw*ymw,
                CstrBrief path, double sf, double tx, double ty)
{
    Tvgui*      gui  = ymw->ymw_gui;
    Tyfig_dsds* dsds = gui->g_ud;
    Tfigure*    fig  = fig_new_byFile(ymw->ymw_msgdrv,path,dsds->ds_baseColors);
    if ( fig==0 ) {
        yfig_cursce_new(ymw);
        return -1;
    } else {
        fig_update_forDrawing(fig,gui->colorDriver);
        if (sf!=0) 
            obj_zoom( obj_ObjAll(fig->objects), sf);
        if ( tx!=0 || ty!=0 )
            obj_translation( obj_ObjAll(fig->objects), tx, ty);
    }

    Tfig_scene* scene = fig_scene_newByFig(fig);
    fsod_detachAttach( ymw_fsod(ymw), scene, 2 ); // 2: preserve zoom
    tool_rf_addClone(dsds->ds_recentFiles,path);
    yfig_cursce_newUpdate( ymw );
    yfig_cursce_redraw( ymw );
    return 0;
}

#include "vgui/wid-simple.h"

extern int  yfig_cursce_delete   (Tyfig_mw*ymw)
{
    if ( yfig_cursce_isModified(ymw) ) {
        Tfigure* fig = yfig_cursce_fig(ymw);
        int status = wid_pum_criticalWBL(&ymw->any,
            WID_DIALSTATUS_CancelX,    NULL,
            WID_DIALSTATUS_Ignore,     "Quit discarding\nchanges",
            WID_DIALSTATUS_Save,       "Save figure\n& Quit",
            WID_DIALSTATUS_None,
            "%s figure is modified",
            fig->path
        );
        switch ( status ) {
            case WID_DIALSTATUS_Ignore: break;
            case WID_DIALSTATUS_Save: yfig_cursce_save(ymw,0); break;
            default: return 0;
        }
    }
    Tyfig_device* fsod = ymw_fsod(ymw);
    fsod_detach(fsod);
    // fsod is deleted by wid_canvas_delete
    return 1;
}

extern Cstr yfig_cursce_saveFilename( Cyfig_mw*yds )
{
    Tfigure* fig;
    if ( (fig=yfig_cursce_fig(yds))==0 )
        return 0;
    if ( TOOL_strIsEmpty(fig->path) )
        return 0;
    return fig->path;
}

extern int yfig_cursce_save(Tyfig_mw*ymw, int silent)
{
    int status;
    if ( silent )
        status = fig_save_byFile(&gl_msgdrv_silence,yfig_cursce_fig(ymw),1);
    else
        status = fig_save_byFile(vgui_mw_getMsgDrv(&ymw->any),yfig_cursce_fig(ymw),1);
    if ( status==0 ) {
        Tfig_histo* histo = yfig_cursce_histo(ymw);
        ymw->ymw_currHistSave = fig_histo_undoNb( histo );
        ymw->ymw_currHistCurr = fig_histo_undoNb( histo );
        yfig_mw_inferr_printf(ymw,"Current figure saved into file: %s\n",
                yfig_cursce_saveFilename(ymw));
        return 0;
    } else if ( !silent ) {
        yfig_mw_errerr_printf(ymw,"Saving current figure failed: %s\n",
                yfig_cursce_saveFilename(ymw));
    }
    return -1;
}

/*======================================================================*/
/*= drawing the current scene.                                         =*/

static void yfig_cursce_clear (Tyfig_mw*ymw)
{ 
    Tyfig_device* dev = YMW_ydev(ymw);
    od_clear(fsod_getOD(dev) );
}

static void yfig_cursce_draw  (Tyfig_mw*ymw)
{
    Tyfig_device* dev = YMW_ydev(ymw);
    od_draw( fsod_getOD(dev) );
}

static void yfig_cursce_setBgGrid (Tyfig_mw*ymw)
{
    double gr;
    Twid_canvas* canvas = ymw->ymw_canvas;
    if ( (gr=yfig_mw_getGridShow(ymw))!=0 ) {
        wid_canvas_unitSetGrid(canvas,1.0/gr); 
        wid_canvas_unitEnableGrid(canvas,1); 
    } else {
        wid_canvas_unitEnableGrid(canvas,0); 
    }
}

extern void yfig_cursce_redraw(Tyfig_mw*ymw)
{
    yfig_cursce_setBgGrid(ymw);
    yfig_cursce_clear(ymw);
    yfig_cursce_draw (ymw);
}

extern void yfig_cursce_redraw_objAdded  (Tyfig_mw*ymw, TobjAll obj)
{
    yfig_layers_updateAddObj(ymw,obj.any);
    yfig_cursce_redraw(ymw);
}
extern void yfig_cursce_redraw_objDeleted(Tyfig_mw*ymw, TobjAll obj)
{
    yfig_layers_updateSupObj(ymw,obj.any);
    yfig_cursce_redraw(ymw);
}
extern void yfig_cursce_redraw_update    (Tyfig_mw*ymw)
{
    yfig_layers_update(ymw);
    yfig_cursce_redraw(ymw);
}

extern void yfig_cursce_setMagnetGrid (Tyfig_mw*ymw)
{
    double gr;
    Twid_canvas*     canvas = ymw->ymw_canvas;
    if ( (gr=yfig_mw_getGridMag(ymw))!=0 ) {
        wid_canvas_unitSetMagnet(canvas,1.0/gr);
        wid_canvas_unitEnableMagnet(canvas,1);
    } else {
        //wid_canvas_unitSetMagnet(canvas,0.0);
        wid_canvas_unitEnableMagnet(canvas,0);
    }
}

/*======================================================================*/
/*= Modifying picture of the current scene.                            =*/

extern void    yfig_cursce_movObjNHis(Tyfig_mw* ymw, TobjAll obj, Tdble2 tv) { obj_translation(obj,tv.x,tv.y); }
extern void    yfig_cursce_movObjWHis(Tyfig_mw* ymw, TobjAll obj, Tdble2 tv) { obj_translation(obj,tv.x,tv.y); yfig_histo_pushMovObj(ymw,obj,tv);  }
extern void    yfig_cursce_supObjNHis(Tyfig_mw* ymw, TobjAll obj) { obj_dir_delObjQ(obj); }
extern void    yfig_cursce_supObjWHis(Tyfig_mw* ymw, TobjAll obj)
{
    yfig_histo_pushDelObj(ymw,obj.any);
    yfig_cursce_supObjNHis(ymw,obj);
}

extern void    yfig_cursce_supObjSetNHis(Tyfig_mw* ymw, TobjAll*objs, int objnb)
{
    int i;
    if (objnb==0) return;
    if (objnb==1) { yfig_cursce_supObjNHis(ymw,*objs); return; }
    for ( i=0 ; i<objnb ; i+=1 )
        obj_dir_delObjQ(objs[i]);

}
extern void    yfig_cursce_supObjSetWHis(Tyfig_mw* ymw, TobjAll*objs, int objnb)
{
    if (objnb==0) return;
    if (objnb==1) { yfig_cursce_supObjWHis(ymw,*objs); return; }
    yfig_histo_pushDelObjSet(ymw,objs,objnb,yfig_cursce_curDir(ymw).dir);
    yfig_cursce_supObjSetNHis(ymw,objs,objnb);
}

extern void yfig_cursce_addObjNHis(Tyfig_mw* ymw, TobjAll obj) {
    Tyfig_dsds* dsds = ymw->ymw_dsds;
    TobjAll     dir  = yfig_cursce_curDir(ymw);

    obj_setPpi(obj,obj_Ppi(dir));
    TarrowDesc* ad; Tsty_fill*  pstf;
    if ( obj_arrow_IsSupported(obj) ) {
        double length;
        double PtsToFu = 1200./72.;
        ad = &ymw->ymw_arrowbeg;
        if ( ad->enabled ) {
            length = ad->length * PtsToFu;
            void* vt = obj_vtable[ad->vtidx];
            pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
            if ( ad->abslength )
                obj_arrow_chgAbs(obj,1,vt,pstf,length,ad->angle,ad->thickness);
            else
                obj_arrow_chgRel(obj,1,vt,pstf,length,ad->angle);
        }
        ad = &ymw->ymw_arrowend;
        if ( ad->enabled ) {
            length = ad->length * PtsToFu;
            void* vt = obj_vtable[ad->vtidx];
            pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
            if ( ad->abslength )
                obj_arrow_chgAbs(obj,0,vt,pstf,length,ad->angle,ad->thickness);
            else
                obj_arrow_chgRel(obj,0,vt,pstf,length,ad->angle);
        }
    }
    obj_dir_addObj(dir,obj);
}
extern void yfig_cursce_addObjWHis(Tyfig_mw* ymw, TobjAll obj)
{
    yfig_cursce_addObjNHis(ymw,obj);
    yfig_histo_pushAddObj(ymw,obj.any);
}

extern void yfig_cursce_chgObjNHis(Tyfig_mw* ymw, TobjAll obj, TobjAll newobj)
{
    TobjAll dir  = yfig_cursce_curDir(ymw);
    obj_dir_addObj(dir, newobj );
    obj_dir_delObjQ( obj );
}

extern void yfig_cursce_chgObjWHis(Tyfig_mw* ymw, TobjAll obj, TobjAll newobj)
{
    TobjAll dir  = yfig_cursce_curDir(ymw);
    obj_dir_addObj(dir, newobj );
    yfig_histo_pushChgObj(ymw,obj.any,newobj.any);
    obj_dir_delObjQ( obj );
}

extern void    yfig_cursce_fltObjNHis(Tyfig_mw* ymw, TobjAll obj) { obj_dir_flat( obj ); }
extern void    yfig_cursce_fltObjWHis(Tyfig_mw* ymw, TobjAll obj)
{
    yfig_histo_pushCpdFlat(ymw,obj.dir);
    obj_dir_flat( obj );
}

extern TobjAll yfig_cursce_mkdObjNHis(Tyfig_mw* ymw, TobjAll*objs, int objnb)
{
    int i;
    // create compound
    TobjAll dir  = obj_dir_mkdir( yfig_cursce_curDir(ymw) );
    for ( i=0 ; i<objnb ; i+=1 ) {
        obj_dir_addObj(dir, objs[i]);
    }
    return dir;
}
extern TobjAll yfig_cursce_mkdObjWHis(Tyfig_mw* ymw, TobjAll*objs, int objnb)
{
    TobjAll dir = yfig_cursce_mkdObjNHis(ymw,objs,objnb);
    yfig_histo_pushCpdDo(ymw, dir.dir);
    return dir;
}

/*======================================================================*/
/*= Setting arrows of an object of the current scene.                  =*/

extern void yfig_cursce_chgObjArrowBeg   (Tyfig_mw* ymw, TobjAll obj)
{ 
    Tyfig_dsds* dsds    = ymw->ymw_dsds;
    TarrowDesc* ad      = &ymw->ymw_arrowbeg;
    double      PtsToFu = 1200./72.;
    double length; Tsty_fill*  pstf;
    if ( ad->enabled ) {
        length = ad->length * PtsToFu;
        void* vt = obj_vtable[ad->vtidx];
        pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
        if ( ad->abslength )
            obj_arrow_chgAbs(obj,1,vt,pstf,length,ad->angle,ad->thickness);
        else
            obj_arrow_chgRel(obj,1,vt,pstf,length,ad->angle);
    } else {
        obj_arrow_delete(obj,1);
    }
}

extern void yfig_cursce_chgObjArrowEnd   (Tyfig_mw* ymw, TobjAll obj)
{ 
    Tyfig_dsds* dsds    = ymw->ymw_dsds;
    TarrowDesc* ad      = &ymw->ymw_arrowend;
    double      PtsToFu = 1200./72.;
    double length; Tsty_fill*  pstf;
    if ( ad->enabled ) {
        length = ad->length * PtsToFu;
        void* vt = obj_vtable[ad->vtidx];
        pstf = ad->flag==FARROW_FillWhite ? &dsds->ds_fillWhite : NULL;
        if ( ad->abslength )
            obj_arrow_chgAbs(obj,0,vt,pstf,length,ad->angle,ad->thickness);
        else
            obj_arrow_chgRel(obj,0,vt,pstf,length,ad->angle);
    } else {
        obj_arrow_delete(obj,0);
    }
}

extern void yfig_cursce_chgObjArrowBegEnd(Tyfig_mw* ymw, TobjAll obj)
{
    yfig_cursce_chgObjArrowBeg(ymw,obj);
    yfig_cursce_chgObjArrowEnd(ymw,obj);
}

extern void yfig_cursce_draw_obj(Tyfig_mw*ymw, Cobject o, Tod_GCdef gcd)
{
    Tvgui_odcanvas* dev = ymw->ymw_canvas->vgcm_outdev;
    ydev_draw_obj((Tyfig_device*)dev,o,gcd);
}

/*======================================================================*/
