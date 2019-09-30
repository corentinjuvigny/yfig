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
 *$Software: YFIG  (YFIG driver to Xt and Xawd or Xaw3d)
 *      $Id: yfigxt-mw.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/xt/yfigxt-mw.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/* EGS driver for building and handling the top YFIG widget.           =*/
/*======================================================================*/

#include "yfig-config.h"

#include "yfig-mw.h"
#include "vgui/wid-menu.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"

#include "guixt/guixt-top.h"
#include "guixt/guixt-event.h"
#include "guixt/guixt-util.h"
#include "guixt/guixt-create.h"

/*======================================================================*/

extern void gui_buttonPanel_resize(Twid_buttonPanel*panel, int width, int height);

typedef struct { int x,y,dx,dy,bd; } Tgeom;
#define PRINT(k)  printf(#k":%2d: %3d %3d %3d %3d --> %3d %3d %3d %3d\n",cnt,\
            e##k.x,e##k.y,e##k.dx,e##k.dy, t##k.x,t##k.y,t##k.dx,t##k.dy);
#define PRINT2(k) printf(#k":%2d: %3d %3d %3d %3d --> %3d %3d %3d %3d\n",cnt,\
            k.x,k.y,k.dx,k.dy, e##k.x,e##k.y,e##k.dx,e##k.dy);


static void geom_init(Tgeom*geom, Tegs_widget w)
{
    widget_geometry(w,&geom->x,&geom->y,&geom->dx,&geom->dy);
    geom->bd  = widget_bdw(w);
    geom->dx += 2*geom->bd;
    geom->dy += 2*geom->bd;
}

static int geom_cmp(const Tgeom*l, const Tgeom*r)
{
    int ret;
    if ( (ret=l->x-r->x)!=0 ) return ret;
    if ( (ret=l->y-r->y)!=0 ) return ret;
    if ( (ret=l->dx-r->dx)!=0 ) return ret;
    if ( (ret=l->dy-r->dy)!=0 ) return ret;
    if ( (ret=l->bd-r->bd)!=0 ) return ret;
    return 0;
}

static void ygui_CBresize(Tyfig_mw* ymw, XEvent *event)
{
    int dx,dy,colNb,rowNb;

    Tvgui* gui       = ymw->ymw_gui;
    Widget form      = ymw->any.widEGS;
    Widget menuBar   = ymw->ymw_menuBar->mb_widEGS;
    Widget message   = ymw->ymw_message;
    Widget mousefun  = ymw->ymw_mouseFun->mf_widEGS;
    Widget modePanel = ymw->ymw_modePanel->bp_widEGSext;
    Widget canvas    = ymw->ymw_canvas->vgcm_widEGS;
    Widget update    = vgui_dw_getWidEGS(ymw->ymw_update);
    Widget dpPanel   = ymw->ymw_dpPanel->bp_widEGSext;
    Widget layers    = vgui_dw_getWidEGS(ymw->ymw_layers);

#define DEF_GEOM(k,w) Tgeom k,e##k,t##k; geom_init(&k,w); e##k=k; t##k=k;
    DEF_GEOM(top,form);
    DEF_GEOM(mbr,menuBar);
    DEF_GEOM(mes,message);
    DEF_GEOM(mfn,mousefun);
    DEF_GEOM(mpl,modePanel);
    DEF_GEOM(cvs,canvas);
    DEF_GEOM(ude,update);
    DEF_GEOM(dpp,dpPanel);
    DEF_GEOM(lay,layers);

    if ( top.dx<650 ) {
        etop.dx=ttop.dx = 650;
    }

    /* get minimal width of mouse fun widget */
    gui_fonts_extents_DesUnit(gui,gui->g_devFid,gui->g_devPtsStd,MOUSEFUN_WD_STR,0,0,&dx,0);
    int mfn_dx_min = 3*dx + 2*mfn.bd + 2*ymw->ymw_mouseFun->mf_canvas->vgcm_daHVSep;

    int found=0;    // 1:expected layout is stable
    int cnt;
    tmpl.dy = etop.dy - ( embr.dy + emes.dy + eude.dy );
    for (cnt=0 ; !found && cnt<5 ; cnt++) {
        // mouse fun
        tmfn.dx =  mfn_dx_min; //emfn.dx<mfn_dx_min ? mfn_dx_min : emfn.dx;
        tmfn.x  =  etop.dx - tmfn.dx;
        tmfn.dy =  embr.dy + emes.dy;
        // top bar (mbr) and message (mes)
        tmbr.dx = etop.dx - emfn.dx - 2*etop.bd ;
        tmes.dx = etop.dx - emfn.dx - 2*etop.bd ;
        // mode panel (mpl)
        for ( colNb=2; colNb<10; colNb+=1 ) {
            int dy;
            rowNb = wid_buttonPanel_getLayout(ymw->ymw_modePanel,colNb,0,0,0);
            dy = wid_buttonPanel_getHeight(ymw->ymw_modePanel,rowNb);
            //dy += rowNb*3;
            if ( dy<=tmpl.dy )
                break;
        }
        rowNb  = wid_buttonPanel_getLayout(ymw->ymw_modePanel,colNb,0,0,0);
        tmpl.dx = wid_buttonPanel_getWidth (ymw->ymw_modePanel,colNb);
        tmpl.dy = wid_buttonPanel_getHeight(ymw->ymw_modePanel,rowNb);
        if ( tmpl.dx < eude.dx )
            tmpl.dx = eude.dx;
        tmpl.y = emes.y  + emes.dy;

        // canvas (cvs)
        tcvs.x  = empl.x  + empl.dx ;
        tcvs.y  = empl.y;
        tcvs.dx = etop.dx - ( empl.x + empl.dx ) - elay.dx ;
        tcvs.dy = etop.dy - ( emfn.y + emfn.dy ) 
                          - max2(eude.dy,edpp.dy) ;
        // update (ude)
        tude.y  = ecvs.y + ecvs.dy ;

        // dpPanel (dpp)
        tdpp.x   = eude.x + eude.dx;
        tdpp.y   = tude.y;
        tdpp.dx  = etop.dx - (eude.x + eude.dx) ;

        // layers (lay)
        tlay.x  = ecvs.x + ecvs.dx ;
        tlay.y  = ecvs.y ;
        tlay.dy = ecvs.dy ;
        tlay.bd = 0;

        // top form ( we do'nt change it)
//PRINT(top); PRINT(mbr); PRINT(mes); PRINT(mfn);
//PRINT(mpl); PRINT(cvs); PRINT(ude); PRINT(dpp);
        found=1;
        if ( geom_cmp( &ttop, &etop )!=0 ) { found=0; etop=ttop; }
        if ( geom_cmp( &tmbr, &embr )!=0 ) { found=0; embr=tmbr; }
        if ( geom_cmp( &tmes, &emes )!=0 ) { found=0; emes=tmes; }
        if ( geom_cmp( &tmfn, &emfn )!=0 ) { found=0; emfn=tmfn; }
        if ( geom_cmp( &tmpl, &empl )!=0 ) { found=0; empl=tmpl; }
        if ( geom_cmp( &tcvs, &ecvs )!=0 ) { found=0; ecvs=tcvs; }
        if ( geom_cmp( &tude, &eude )!=0 ) { found=0; eude=tude; }
        if ( geom_cmp( &tdpp, &edpp )!=0 ) { found=0; edpp=tdpp; }
        if ( geom_cmp( &tlay, &elay )!=0 ) { found=0; elay=tlay; }
    }
//printf("%s: stable %d in %d/10 ite\n",__func__,found,cnt);
if ( found==0 ) exit(1);
//PRINT2(top); PRINT2(mbr); PRINT2(mes); PRINT2(mfn);
//PRINT2(mpl); PRINT2(cvs); PRINT2(ude); PRINT2(dpp);
#define VG_XtResizeWidget(w,dx,dy,bd) do { \
    int __dx__ = dx-2*bd; if ( __dx__<=0 ) __dx__=1; \
    int __dy__ = dy-2*bd; if ( __dy__<=0 ) __dy__=1; \
    XtResizeWidget(w,__dx__,__dy__,bd); \
} while (0)

    if ( geom_cmp( &mbr, &embr )!=0 )
        VG_XtResizeWidget(menuBar,embr.dx,embr.dy,embr.bd );

    if ( geom_cmp( &mes, &emes )!=0 )
        VG_XtResizeWidget(message,emes.dx,emes.dy,emes.bd );

    if ( geom_cmp( &mfn, &emfn )!=0 )
        gui_canvas_resize(gui,ymw->ymw_mouseFun->mf_canvas,
                emfn.x,emfn.y,emfn.dx,emfn.dy);

    if ( geom_cmp( &mpl, &empl )!=0 ) {
        XtMoveWidget(modePanel,empl.x,empl.y);
        gui_buttonPanel_resize(ymw->ymw_modePanel,empl.dx,empl.dy);
    }

    if ( geom_cmp( &cvs, &ecvs )!=0 )
        gui_canvas_resize(gui,ymw->ymw_canvas,
                ecvs.x,ecvs.y,ecvs.dx,ecvs.dy);

    if ( geom_cmp( &ude, &eude )!=0 )
        XtMoveWidget  (update,eude.x,eude.y);

    if ( geom_cmp( &dpp, &edpp )!=0 ) {
        XtMoveWidget  (dpPanel,edpp.x,edpp.y);
        gui_buttonPanel_resize(ymw->ymw_dpPanel,edpp.dx,edpp.dy);
    }

    if ( geom_cmp( &lay, &elay )!=0 ) {
        XtMoveWidget  (layers,elay.x,elay.y);
        VG_XtResizeWidget(layers,elay.dx,elay.dy,elay.bd );
    }

    if ( geom_cmp( &top, &etop )!=0 ) {
        XtResizeWidget(E_TopWidget,etop.dx,etop.dy,etop.bd );
        XtResizeWidget(form,etop.dx,etop.dy,etop.bd );
    }
}

/*======================================================================*/

extern Tegs_widget gui_yfig_create   (Tvgui*    gui)
{
    Widget form;
    Tgui_main* main = gui->g_main;
    form = create_form(gui, main->m_topWidget, wn_mainTop,
            0, 0,-1,-1, EGSCOLOR_default,EGSCOLOR_default); 
    XtVaSetValues(form, XtNresizable,True, NULL);
    //XtVaSetValues(form, XtNresize,True, NULL);
    XtVaSetValues(form, XtNinput,True, NULL);
    return form;
}

extern void        gui_yfig_createend(Tyfig_mw* ymw)
{
    Widget form      = ymw->any.widEGS;
    Widget menuBar   = ymw->ymw_menuBar->mb_widEGS;
    Widget message   = ymw->ymw_message;
    Widget mousefun  = ymw->ymw_mouseFun->mf_widEGS;
    Widget modePanel = ymw->ymw_modePanel->bp_widEGSext;
    Widget canvas    = ymw->ymw_canvas->vgcm_widEGS;
    Widget update    = vgui_dw_getWidEGS(ymw->ymw_update);
    Widget dpPanel   = ymw->ymw_dpPanel->bp_widEGSext;
    Widget layers    = vgui_dw_getWidEGS(ymw->ymw_layers);

    widget_place_FixedSize   (menuBar   , 0,0         ,  0,0       );
    widget_place_GrowToEast  (message   , 0,0         ,  0,menuBar );
    widget_place_FixedSize   (mousefun  , 0,message   ,  0,0       );
    widget_place_FixedSize   (modePanel , 0,0         ,  0,message );
    widget_place_GrowToSE    (canvas    , 0,modePanel ,  0,message );
    widget_place_FixedSize   (update    , 0,NULL      ,  0,canvas  );
    widget_place_FixedSize   (dpPanel   , 0,update    ,  0,canvas  );
    widget_place_GrowToSouth (layers    , 0,canvas    ,  0,mousefun);

    widget_cb_addU(form, ymw,
        YFIG_EVENT_ResizeEvent,     ygui_CBresize, 
        YFIG_EVENT_VisibilityEvent, ygui_CBresize, 
        NULL
    );
}

extern void gui_yfig_delete     (Tyfig_mw* ymw)
{
    // XtDestroy widget does not free title
    char *t;
    XtVaGetValues(XtParent((Widget)ymw->any.widEGS), XtNtitle,&t, NULL);
    free( t );

    XtDestroyWidget( ymw->any.widEGS );
}

extern void gui_yfig_setTitle(Cyfig_mw* ymw, TstrBrief t)
{ XtVaSetValues(XtParent((Widget)ymw->any.widEGS), XtNtitle,t, NULL); }

/*======================================================================*/
