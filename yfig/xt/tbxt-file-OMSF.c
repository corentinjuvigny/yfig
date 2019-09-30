/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 */

#include "yfig-config.h"

#include "ydw/tb-file-OMSF.h"
#include "vgui/vgui.h"
#include "vgui/wid-lvu.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-filebrowser.h"

#include "guixt/guixt-top.h"
#include "guixt/guixt-util.h"

/*======================================================================*/

static void gui_fileprev_resize(Tdial_OMSFprev* dp)
{
    Tvgui*    gui = dp->wd_gui;
    Widget    cw,pw;
    Dimension pdx,pdy;      // parent widget size (pw)
    Dimension cdx,cdy,cbw;  // current widget (cw) size and border width
    Position  cx,cy;        // and position in parent widget
    Dimension nsz;

    if ( dp==0) return;

    // resize preview horizontally
    pw=dp->wd_pWidget; cw=dp->wd_widEGS;
    XtVaGetValues(pw, XtNwidth,&pdx, XtNheight,&pdy, NULL);
    XtVaGetValues(cw, XtNwidth,&cdx, XtNheight,&cdy,
                      XtNborderWidth,&cbw, XtNx,&cx, XtNy,&cy, NULL);
    nsz = pdx-cx-5;
    if ( nsz > cdx )
        XtResizeWidget(cw,nsz,cdy,cbw);

    // resize df_fileNameLAB horizontally
    pw=dp->wd_widEGS; cw=dp->df_fileNameLAB;
    XtVaGetValues(pw, XtNwidth,&pdx, XtNheight,&pdy, NULL);
    XtVaGetValues(cw, XtNwidth,&cdx, XtNheight,&cdy,
                      XtNborderWidth,&cbw, XtNx,&cx, XtNy,&cy, NULL);
    nsz = pdx-cx-5;
    if ( nsz > cdx )
        XtResizeWidget(cw,nsz,cdy,cbw);

    // resize df_commValueTOU horizontally and vertically
    pw=dp->wd_widEGS; cw=dp->df_commValueTOU->widEGS;
    XtVaGetValues(pw, XtNwidth,&pdx, XtNheight,&pdy, NULL);
    XtVaGetValues(cw, XtNwidth,&cdx, XtNheight,&cdy,
                      XtNborderWidth,&cbw, XtNx,&cx, XtNy,&cy, NULL);
#if 1
    int x_canvas,y_canvas, dx_canvas,dy_canvas;
    widget_geometry(dp->df_canvas->vgcm_widEGS,
            &x_canvas,&y_canvas,&dx_canvas,&dy_canvas);
#else
    Position x_canvas;
    XtVaGetValues(dp->df_canvas->vgcm_widEGS, XtNx,&x_canvas, NULL);
#endif
    nsz = x_canvas-cx-5;
    Dimension nszY = pdy-cy-5;
    if ( nsz>cdx && nszY>cdy ) 
        XtResizeWidget(cw,nsz,nszY,cbw);
    else if ( nsz>cdx ) 
        XtResizeWidget(cw,nsz,cdy,cbw);
    else if ( nszY>cdy ) 
        XtResizeWidget(cw,cdx,nszY,cbw);

    // canvas
    gui_canvas_resize(gui,dp->df_canvas,
            x_canvas,y_canvas,dx_canvas,dy_canvas);
}

extern void gui_OMSFprev_dbf(Tdial_OMSFprev* dp)
{
    Tvgui* gui = dp->wd_gui;

//#define df_objnbTitleLAB df_objnb->any.titEGS
#define   df_objnbTitleLAB df_objnb->lvu_labEGS
//#define df_objnbValueLAB df_objnb->any.valEGS
#define   df_objnbValueLAB df_objnb->lvu_valEGS
//#define df_sizeTitleLAB  df_size ->any.titEGS
#define   df_sizeTitleLAB  df_size ->lvu_labEGS
//#define df_sizeValueLAB  df_size ->any.valEGS
#define   df_sizeValueLAB  df_size ->lvu_valEGS
//#define df_sizeUnitDDL   df_size ->any.uniEGS
#define   df_sizeUnitDDL   df_size ->lvu_uniEGS



    widget_setSameDx(gui,
        dp->df_titleLAB,      dp->df_objnbTitleLAB,
        dp->df_sizeTitleLAB,  dp->df_commTitleLAB,
        NULL);

    //XawFormDoLayout(dp->df_self,0);

    // line 1, all the columns
    widget_place(dp->df_titleLAB       , WPW_FixedSize  , 5 , 0                     , 5 , 0);
    widget_place(dp->df_fileNameLAB    , WPW_GrowRight  , 5 , dp->df_titleLAB       , 5 , 0);
    // line 2, column 1a/1b/1c
    widget_place(dp->df_objnbTitleLAB  , WPW_FixedSize  , 5 , 0                     , 5 , dp->df_titleLAB);
    widget_place(dp->df_objnbValueLAB  , WPW_FixedSize  , 5 , dp->df_objnbTitleLAB  , 5 , dp->df_fileNameLAB);
    widget_place(dp->df_sizeTitleLAB   , WPW_FixedSize  , 5 , 0                     , 5 , dp->df_objnbTitleLAB);
// IVG #define df_sizeValueLAB df_sizeValueLWU->lwu_westWidget
// IVG #define df_sizeUnitDDL  df_sizeValueLWU->lwu_eastWidget
    widget_place(dp->df_sizeValueLAB   , WPW_FixedSize  , 5 , dp->df_sizeTitleLAB   , 5 , dp->df_objnbValueLAB);
    widget_place(dp->df_sizeUnitDDL    , WPW_FixedSize  , 5 , dp->df_sizeValueLAB   , 5 , dp->df_objnbValueLAB);
    widget_place(dp->df_commTitleLAB   , WPW_FixedSize  , 5 , 0                     , 5 , dp->df_sizeTitleLAB);
    widget_place(dp->df_commValueTOU->widEGS , WPW_GrowBot    , 5 , dp->df_commTitleLAB   , 5 , dp->df_sizeValueLAB);
    // line 2, column 2
    widget_place(dp->df_canvas->vgcm_widEGS
                                             , WPW_GrowRB      , 5 , dp->df_sizeUnitDDL   , 5 , dp->df_titleLAB);
    //XawFormDoLayout(dp->df_self,1);
}

/*======================================================================*/

extern void        gui_OMSFpara_dbf(Tdial_OMSFpara* dp)
{
#define df_xLAB      df_xINP ->lvu_labEGS
#define df_xWestWid  df_xINP ->lvu_valEGS
#define df_xEastWid  df_xINP ->lvu_uniEGS
#define df_yLAB      df_yINP ->lvu_labEGS
#define df_yWestWid  df_yINP ->lvu_valEGS
#define df_yEastWid  df_yINP ->lvu_uniEGS
#define df_dxLAB     df_dx->lvu_labEGS
#define df_dxWestWid df_dx->lvu_valEGS
#define df_dxEastWid df_dx->lvu_uniEGS
#define df_dyLAB     df_dy->lvu_labEGS
#define df_dyWestWid df_dy->lvu_valEGS
#define df_dyEastWid df_dy->lvu_uniEGS
#define df_scaleLAB  df_scaleDIN->lvu_labEGS
#define df_scaleWid  df_scaleDIN->lvu_valEGS
    Tvgui* gui = dp->wd_gui;
    widget_setSameDx(gui,
        dp->df_xLAB  , dp->df_yLAB,
        dp->df_dxLAB , dp->df_dyLAB,
        //dp->df_scaleLAB,
        NULL);
    widget_setSameDx(gui,
        dp->df_xWestWid  , dp->df_yWestWid,
        dp->df_dxWestWid , dp->df_dyWestWid,
        NULL);

    widget_place_widget(dp->df_xLAB      , WPW_FixedSize,  5  , 0                ,  5, 0);
    widget_place_widget(dp->df_xWestWid  , WPW_FixedSize,  5  , dp->df_xLAB      ,  5, 0);
    widget_place_widget(dp->df_xEastWid  , WPW_FixedSize,  5  , dp->df_xWestWid  ,  5, 0);
    widget_place_widget(dp->df_yLAB      , WPW_FixedSize,  5  , 0                ,  2, dp->df_xWestWid);
    widget_place_widget(dp->df_yWestWid  , WPW_FixedSize,  5  , dp->df_yLAB      ,  2, dp->df_xWestWid);
    widget_place_widget(dp->df_yEastWid  , WPW_FixedSize,  5  , dp->df_yWestWid  ,  2, dp->df_xWestWid);

    widget_place_widget(dp->df_dxLAB     , WPW_FixedSize,  5  , 0                ,  5, dp->df_yWestWid);
    widget_place_widget(dp->df_dxWestWid , WPW_FixedSize,  5  , dp->df_dxLAB     ,  5, dp->df_yWestWid);
    widget_place_widget(dp->df_dxEastWid , WPW_FixedSize,  5  , dp->df_dxWestWid ,  5, dp->df_yWestWid);
    widget_place_widget(dp->df_dyLAB     , WPW_FixedSize,  5  , 0                ,  2, dp->df_dxEastWid);
    widget_place_widget(dp->df_dyWestWid , WPW_FixedSize,  5  , dp->df_dyLAB     ,  2, dp->df_dxEastWid);
    widget_place_widget(dp->df_dyEastWid , WPW_FixedSize,  5  , dp->df_dyWestWid ,  2, dp->df_dxEastWid);

    widget_place_widget(dp->df_scaleLAB  , WPW_FixedSize, 20  , dp->df_dxEastWid ,  6, dp->df_yEastWid);
    widget_place_widget(dp->df_scaleWid  , WPW_FixedSize,  0  , dp->df_scaleLAB ,  10, dp->df_yEastWid);
}

/*======================================================================*/

static int gui_dialfile_resize(Tvgui_dw* d)
{
    Tdial_OMSF *df = (Tdial_OMSF*)d;
    if ( df->df_preview ) gui_fileprev_resize(df->df_preview);
    if ( df->df_param )   widget_resize_toEast(df->df_param->dw_widEGS, NULL);
    xt_filebrowser_resize(df->df_browser);
}

extern void gui_OMSF_newFinish(Tdial_OMSF*df)
{ xt_dw_addCBresize(&df->any,gui_dialfile_resize); }

/*======================================================================*/
// vim: set textwidth=0:
