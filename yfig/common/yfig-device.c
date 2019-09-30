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
 *      $Id: yfig-device.c 390 2019-01-31 08:49:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-device.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= YFIG device (spezialization of Tvgui_odcanvas device)             =*/
/*=                                                                    =*/
/*= See the "yfig-device.h" header for documentation.                  =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-dsds.h"
#include "yfig-device.h"

#include "vgui/vgui.h"
#include "fig/object.h"
#include "outdev/ldaf-hl.h"

/*======================================================================*/
/*= wConfiguration                                                     =*/

// returns the tolerance for "close to" in pixel of output device
Inline int tolerance(Tyfig_device*fsod)
{
    double sizeInInch  = LUNIT_PtToIn(5); // in points (1/72 Inch)
    int    sizeInPixel = round( sizeInInch*fsod_getPpiDes(fsod) );
    return (sizeInPixel&(~1)) + 1; // round to upper odd integer
}

// returns the size of a marker in pixel of output device
Inline int marker_sizeInPixel(Tyfig_device*fsod)
{
    double sizeInInch  = LUNIT_PtToIn(5); // in points (1/72 Inch)
    int    sizeInPixel = round( sizeInInch*fsod_getPpiDes(fsod) );
    return (sizeInPixel&(~1)) + 1; // round to upper odd integer
}
Inline int marker_sizeInPixelFill(Tyfig_device*fsod)
{
    double sizeInInch  = LUNIT_PtToIn(5+2); // in points (1/72 Inch)
    int    sizeInPixel = round( sizeInInch*fsod_getPpiDes(fsod) );
    return (sizeInPixel&(~1)) + 1; // round to upper odd integer
}

/*======================================================================*/
/*= Abstract interface                                                 =*/

#define FSOD ((Tvgui_outdev*)fsod)

extern Tvgui*       fsod_getGUI    (Cyfig_device* fsod) { return  FSOD->v_gui; }
extern void*        fsod_getUD     (Cyfig_device* fsod) { return  FSOD->v_userdata; }
extern Tvgui_outdev fsod_cpyVOD    (Cyfig_device* fsod) { return *FSOD; }
extern Tvgui_outdev*fsod_getVOD    (Cyfig_device* fsod) { return  FSOD; }
extern Toutdev      fsod_cpyOD     (Cyfig_device* fsod) { return  FSOD->v_od; }
extern Toutdev*     fsod_getOD     (Cyfig_device* fsod) { return &FSOD->v_od; }
extern double       fsod_getPpiSrc (Cyfig_device* fsod) { return OD_ppiSrc(&FSOD->v_od); }
extern double       fsod_getPpiDes (Cyfig_device* fsod) { return FSOD->v_od.ppiDes; }
extern CboxDble*    fsod_getBoxSrc (Cyfig_device* fsod) { return &FSOD->v_od.boxSrc; }
extern CboxDble*    fsod_getBoxDes (Cyfig_device* fsod) { return &FSOD->v_od.boxDes; }
extern Tfig_scene*  fsod_getScene  (Cyfig_device* fsod) { return  FSOD->v_od.parSrc; }
extern Tfigure*     fsod_getFigure (Cyfig_device* fsod) { return fsod_getScene(fsod)->sc_dessin; }

extern TobjDir      fsod_getTopObj (Cyfig_device* fsod) { return fig_scene_getTopObj(fsod_getScene(fsod)); }
extern TobjAll      fsod_getCurrObj(Cyfig_device* fsod) { return fig_scene_getCurrObj(fsod_getScene(fsod)); }
extern void         fsod_setCurrObj(Cyfig_device* fsod, TobjAll o) { fig_scene_setCurrObj(fsod_getScene(fsod),o); }

/*======================================================================*/
/*= A quick interface for handling object in a yfig_device.            =*/
/*= It is mainly used by the GUI for creating pixmap corresponding     =*/
/*= exactly to the figure objects.                                     =*/

extern Tyfig_device* fsod_attach(Tvgui_outdev* vod)
{
    Tyfig_dsds* dsds = vod->v_gui->g_ud;
    Tmsgdrv*    md   = vod->v_gui->g_msgdrv;
    Tfigure* fig = fig_new_empty(md,"dummy.fig",0,dsds->ds_baseColors);
    fig_chgUnit(fig,vod->v_ppiDes);
    Tfig_scene* scene = fig_scene_newByFig(fig);
    fig_scene_attachDev(scene,&vod->v_od,1);
    return (Tyfig_device*)vod;
}

extern Tyfig_device* fsod_attachPt(Tvgui_outdev* vod)
{
    Tyfig_dsds* dsds = vod->v_gui->g_ud;
    Tmsgdrv*    md   = vod->v_gui->g_msgdrv;
    Tfigure* fig = fig_new_empty(md,"dummy.fig",0,dsds->ds_baseColors);
    fig_chgUnit(fig,LUNIT_PtPerInch);
    Tfig_scene* scene = fig_scene_newByFig(fig);
    fig_scene_attachDev(scene,&vod->v_od,1);
    return (Tyfig_device*)vod;
}

extern void fsod_detach(Tyfig_device* fsod)
{
    Tfig_scene* sc = fsod_getScene(fsod);
    if ( sc==0 ) return;
    fig_scene_clrEGScache( sc );
    od_detach( fsod_getOD(fsod) );
    fig_scene_delete(sc);
}

extern void fsod_detachAttach(Tyfig_device* fsod, Tfig_scene* sc, int w)
{
    Toutdev* od = fsod_getOD( fsod );
    double zoom=0; // 0: means w=0 or w=1
    if ( w==2 ) {
        w=0;
        if ( fsod_getScene( fsod )!=0 ) {
            zoom  = od_getUsrZoom( od );
        }
    }
    fsod_detach( fsod );
    fig_scene_attachDev( sc, od, w);
    if ( zoom!=0 ) {
        // w=0 so the figure is already centered in the device.
        od_setUsrZoom_CENTER( od , zoom );
    }
}

extern TobjAll fsod_addLineXY(Cyfig_device* fsod, double thickness,
                    double x0, double y0, double x1, double y1)
{
    TobjAll    obj;
    Toutdev*   od     = fsod_getOD( fsod );
    TobjDir    dir    = fsod_getTopObj( fsod );
    double     srcppi = fsod_getPpiSrc(fsod);
    Tdble2Set* pts    = dble2set_new(2);
    dble2set_addXY(pts,lu_convToFuPpi(x0,srcppi),lu_convToFuPpi(y0,srcppi));
    dble2set_addXY(pts,lu_convToFuPpi(x1,srcppi),lu_convToFuPpi(y1,srcppi));
    Tsty_line stl = odsty_copyLine(od_style_topLine(od,OD_GC_SysDraw));
    Tsty_fill stf = odsty_copyFill(od_style_topFill(od,OD_GC_SysFill));
    stl.stl_thickness = thickness;
    obj = obj_line_newF(NULL,100,&stl,&stf,FIGOLI_T_MLine,pts);
    obj_dir_addObj( obj_ObjAll(dir), obj);

    odsty_freeLine(&stl);
    odsty_freeFill(&stf);
    // dble2set_delete(pts); eated by line

    return obj;
}

extern TobjAll fsod_addLineH(Cyfig_device* fsod, double thickness, double bd)
{
    CboxDble* box = fsod_getBoxSrc( fsod );
    double x  = Box_x (*box);
    double y  = Box_y (*box);
    double dx = Box_dx(*box);
    double dy = Box_dy(*box);
    return fsod_addLineXY(fsod,thickness, x+bd, y+dy/2, x+dx-bd, y+dy/2);
}

extern void fsod_chgArrowAbs(Cyfig_device* fsod, TobjAll obj, int atBeg,
                             TarrowDesc* ad, double thickness)
{
    Tyfig_dsds* dsds   = fsod_getGUI(fsod)->g_ud;
    void*       vt     = obj_vtable[ad->vtidx];
    double      length = LUNIT_PtToFu(ad->length);
    Tsty_fill*  pstf = NULL;
    if (ad->flag==FARROW_FillWhite)
        pstf = &dsds->ds_fillWhite;
    obj_arrow_chgAbs(obj,atBeg,vt,pstf,length,ad->angle,thickness);
}

extern void fsod_chgArrowRel(Cyfig_device* fsod, TobjAll obj, int atBeg,
                             TarrowDesc* ad)
{
    Tyfig_dsds* dsds   = fsod_getGUI(fsod)->g_ud;
    void*       vt     = obj_vtable[ad->vtidx];
    double      length = LUNIT_PtToFu(ad->length);
    Tsty_fill*  pstf = NULL;
    if (ad->flag==FARROW_FillWhite)
        pstf = &dsds->ds_fillWhite;
    obj_arrow_chgRel(obj,atBeg,vt,pstf,length,ad->angle);
}

extern double     fsod_scaArrowAbsDU(Tyfig_device* fsod, CarrowDesc*ad,
                                     double dx, double dy)
{
    Toutdev* od = fsod_getOD(fsod);
    //         ..x
    //      ...  | height/2
    //   ...     | 
    //  x--------|-----    (height/2)/length = tan(alpha)
    //    length

    double lenDes;
    lenDes = LUNIT_PtToIn( ad->length ); // in inch
    lenDes = lenDes * od->ppiDes ; // in des unit same as dx & dy

    // dx = lenDes * scaleDx
    // dy = height * scaleDY
    double scaleDX = dx/lenDes;
    double scaleDY = dy/(2.*tan(ad->angle)*lenDes);
    double scale   = min2(scaleDX,scaleDY);
//printf("s=%f sx=%f sy=%f  w=%f h=%f\n",scale,scaleDX,scaleDY, scale*ad->length,
//        scale*2*ad->length*tan(ad->angle));
    return scale;
}

extern double     fsod_scaArrowRelDU(Tyfig_device* fsod, CarrowDesc*ad,
                                     double th, double dx, double dy)
{
    TarrowDesc tmp = *ad;
    tmp.length *= th; 
    return fsod_scaArrowAbsDU(fsod,&tmp,dx,dy);
}

/*======================================================================*/

extern CobjAllSet*fsod_getChildrenAll(Tyfig_device*fsod)
{ return obj_getChildren( fsod_getCurrObj(fsod) ); }
extern TobjAllSet*fsod_getChildren(Tyfig_device*fsod)
{ return obj_getChildrenB( fsod_getCurrObj(fsod), fsod_getBoxSrc(fsod) ); }
extern TobjAllSet*fsod_getChildrenB(Tyfig_device*fsod,CboxDble* bb)
{ return obj_getChildrenB( fsod_getCurrObj(fsod), bb ); }

extern TobjCloseToPt fsod_searchFirstPT(Tyfig_device*fsod,Tdble2 pt)
{
    int         pix    = tolerance(fsod);
    double      d      = CONV_D_D2S(fsod_getOD(fsod),pix);
    TobjAll     dir    = fsod_getCurrObj(fsod);
    Tscdepth*   depths = fsod_getScene(fsod)->sc_depths.table;
    CboxDble*   bb     = fsod_getBoxSrc(fsod);
    TobjAllSet* objs   = obj_getChildrenBD(dir,bb,depths);
    TobjCloseToPt ret  = obj_getObjCloseToPtFirst(objs,d,pt);
    objAllSet_delete( objs );
    return ret;
}

extern TobjCloseToPt fsod_searchFirstXY(Tyfig_device*fsod,double x, double y)
{ Tdble2 pt = { x, y }; return fsod_searchFirstPT(fsod,pt); }

extern TobjCloseToPt fsod_searchFirstMPT(Tyfig_device*fsod,Tdble2 pt, int msk)
{
    int         pix    = tolerance(fsod);
    double      d      = CONV_D_D2S(fsod_getOD(fsod),pix);
    TobjAll     dir    = fsod_getCurrObj(fsod);
    Tscdepth*   depths = fsod_getScene(fsod)->sc_depths.table;
    CboxDble*   bb     = fsod_getBoxSrc(fsod);
    TobjAllSet* objs   = obj_getChildrenBDM(dir,bb,depths,msk);
    TobjCloseToPt ret  = obj_getObjCloseToPtFirst(objs,d,pt);
    objAllSet_delete( objs );
    return ret;
}

extern TobjCloseToPt fsod_searchFirstMXY(Tyfig_device*fsod,double x, double y, int msk)
{ Tdble2 pt = { x, y }; return fsod_searchFirstMPT(fsod,pt,msk); }

extern TobjCloseToPt fsod_searchFirstM2PT(Tyfig_device*fsod,Tdble2 pt,          int objMsk, int mkrOpeMsk)
{
    int         pix    = tolerance(fsod);
    double      d      = CONV_D_D2S(fsod_getOD(fsod),pix);
    TobjAll     dir    = fsod_getCurrObj(fsod);
    Tscdepth*   depths = fsod_getScene(fsod)->sc_depths.table;
    CboxDble*   bb     = fsod_getBoxSrc(fsod);
    TobjAllSet* objs0  = obj_getChildrenBDM(dir,bb,depths,objMsk);
    TobjAllSet* objs   = objAllSet_new(objs0->eleNb);
    int         i;
    for (i=0 ; i<objs0->eleNb ; i++) {
        if ( (obj_markerOpe(objs0->eles[i])&mkrOpeMsk)==mkrOpeMsk ) 
            objAllSet_add(objs,objs0->eles[i]);
    }
    TobjCloseToPt ret  = obj_getObjCloseToPtFirst(objs,d,pt);
    objAllSet_delete( objs );
    objAllSet_delete( objs0 );
    return ret;
}

extern TobjCloseToPt fsod_searchFirstM2XY(Tyfig_device*fsod,double x, double y, int objMsk, int mkrOpeMsk)
{ Tdble2 pt = { x, y }; return fsod_searchFirstM2PT(fsod,pt,objMsk,mkrOpeMsk); }

/*======================================================================*/
/*= figure object drawing                                              =*/

extern void ydev_draw_obj(Tyfig_device* ydev, Cobject obj, Tod_GCdef gcd)
{
    TobjAll      oa;  oa.any = obj;
    Tfig_scene*  scene = fsod_getScene(ydev);
    //fig_scene_draw_obj(scene, oa, gcd);
    obj_draw(scene, oa, gcd);
}

extern void fsod_objmarker_toggleCur(Tyfig_device*fsod, int mask, int noCtrMkr)
{
    int i;
    TobjAll     dir    = fsod_getCurrObj(fsod);
    CboxDble*   bb     = fsod_getBoxSrc(fsod);
    Tscdepth*   depths = fsod_getScene(fsod)->sc_depths.table;
    TobjAllSet* objs   = obj_getChildrenBDM(dir,bb,depths,mask);
    for (i=0 ; i<objs->eleNb ; i+=1)
        fsod_objmarker_toggle(fsod,objs->eles[i],noCtrMkr);
    objAllSet_delete( objs );
}

extern void fsod_objmarker_toggleCur2(Tyfig_device*fsod, int mask, int noCtrMkr, int mkrOpeMask)
{
    if ( mkrOpeMask==0 ) {
        fsod_objmarker_toggleCur(fsod,mask,noCtrMkr);
        return;
    }
    int i;
    TobjAll     dir    = fsod_getCurrObj(fsod);
    CboxDble*   bb     = fsod_getBoxSrc(fsod);
    Tscdepth*   depths = fsod_getScene(fsod)->sc_depths.table;
    TobjAllSet* objs   = obj_getChildrenBDM(dir,bb,depths,mask);
    for (i=0 ; i<objs->eleNb ; i+=1) {
        if ( (obj_markerOpe(objs->eles[i])&mkrOpeMask)!=mkrOpeMask ) continue;
        fsod_objmarker_toggle(fsod,objs->eles[i],noCtrMkr);
    }
    objAllSet_delete( objs );
}


extern void fsod_objmarker_toggle   (Tyfig_device*fsod, TobjAll obj, int noCtrMkr)
{
    int i;
    Toutdev* od = fsod_getOD(fsod);
    double pix = marker_sizeInPixel(fsod);
    int ctrMkrIdx;
    Tdble2Set* markers;
    if ( noCtrMkr ) {
        markers = obj_markersAndCtr(obj,&ctrMkrIdx);
    } else {
        markers = obj_markers(obj);
        ctrMkrIdx = -1;
    }
    for ( i=0 ; i<markers->eleNb ; i+=1 ) {
        if (i==ctrMkrIdx) continue;
        double x=markers->eles[i].x;
        double y=markers->eles[i].y;
        double w = CONV_D_D2S(od,pix);
        od_draw_rectXYWH(od,OD_GC_Marker,x-w/2,y-w/2,w,w); 
    }
    dble2set_delete(markers);
}

extern void fsod_objmarker_toggleHL (Tyfig_device*fsod, TobjAll obj, int noCtrMkr)
{
    int i;
    Toutdev* od = fsod_getOD(fsod);
    double pix = marker_sizeInPixel(fsod);
    double pixf= marker_sizeInPixelFill(fsod);
    Tdble2Set* markers = obj_markers(obj);
    for ( i=0 ; i<markers->eleNb ; i+=1 ) {
        double x=markers->eles[i].x;
        double y=markers->eles[i].y;
        double w = CONV_D_D2S(od,pix);
        double wf= CONV_D_D2S(od,pixf);
        //od_draw_pointXY (od,OD_GC_MarkerHL, x,y);
        od_fill_rectXYWH(od,OD_GC_MarkerHL,x-wf/2,y-wf/2,wf,wf); 
        od_draw_rectXYWH(od,OD_GC_MarkerHL,x-w/2,y-w/2,w,w); 
    }
    dble2set_delete(markers);
}

/*======================================================================*/
