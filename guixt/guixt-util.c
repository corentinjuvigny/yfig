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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-util.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-util.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling widgets.                                    =*/
/*======================================================================*/

#include "guixt.h"
#include "vgui/vgui-accelkey.h"

/*======================================================================*/
/*= miscellaneous converters                                           =*/

// return 0:unknown button; 1/2/3/4/5:left/middle/right/wheel button
extern int       conv_BtnNum_Xt2Vgui(int xnb)
{
    switch ( xnb ) {
        case Button1: return 1;
        case Button2: return 2;
        case Button3: return 3;
        case Button4: return 4;
        case Button5: return 5;
        default:
            TEMP_spw_printf("unexpected QT button identifier %04x",xnb);
            return 0;
    }
}

extern TaccelDef conv_KeyMod_Xt2Vgui(int xkmod)
{
    TaccelDef kmod=0;
    int xmeta = Mod1Mask|
                //Mod2Mask|  // Num Lock
                Mod3Mask|
                Mod4Mask|
                Mod5Mask;
    if ( (xkmod&ShiftMask)!=0 )   kmod |= GL_ACCEL_Shift;
    if ( (xkmod&ControlMask)!=0 ) kmod |= GL_ACCEL_Control;
    if ( (xkmod&xmeta)!=0 )       kmod |= GL_ACCEL_Meta;
#if 0
printf("xmod=%08x\n",xkmod);
printf("shif=%08x\n",ShiftMask);
printf("ctrl=%08x\n",ControlMask);
printf("mod1=%08x\n",Mod1Mask);
printf("mod2=%08x\n",Mod2Mask);
printf("mod3=%08x\n",Mod3Mask);
printf("mod4=%08x\n",Mod4Mask);
printf("mod5=%08x\n",Mod5Mask);
#endif
    return kmod;
}

extern int conv_lineStyle_Od2X11(TlineStyle ls)
{
    switch (ls) {
      default:
        PERM_spe_printf(
            "%d is an invalid line style (set to line-solid).",ls);
      case DA_LS_Solid: return LineSolid;

      case DA_LS_Dash:
      case DA_LS_Dot:
      case DA_LS_Dash1Dot:
      case DA_LS_Dash2Dot:
      case DA_LS_Dash3Dot:
        return LineOnOffDash;
      case DA_LS_None:
        PERM_spe_printf(
            "None (%d) line style is unexpected  (set to line-solid).",ls);
        return LineSolid;
    }
}

extern int conv_joinStyle_Od2X11(TjoinStyle js)
{
    switch (js) {
      default:
        PERM_spe_printf(
            "%d is an invalid join style (set to join-miter).",js);
      case DA_JS_Miter: return JoinMiter;
      case DA_JS_Round: return JoinRound;
      case DA_JS_Bevel: return JoinBevel;
      case DA_JS_None:
        PERM_spe_printf(
            "None (%d) join style is unexpected  (set to join-miter).",js);
        return JoinMiter;
    }
}

extern int conv_capStyle_Od2X11(TcapStyle cs)
{
    switch (cs) {
      default:
        PERM_spe_printf(
            "%d is an invalid cap style (set to cap-butt).",cs);
      case DA_CS_Butt:  return CapButt;
      case DA_CS_Round: return CapRound;
      case DA_CS_Proj:  return CapProjecting;
      case DA_CS_None:
        PERM_spe_printf(
            "None (%d) cap style is unexpected  (set to cap-butt).",cs);
        return CapButt;
    }
}

/*======================================================================*/
/*= miscellaneous utilities                                            =*/

extern void widget_getColors(Tvgui*gui, CstrBrief wn,
                             TcolorEGS*bg, TcolorEGS*fg)
{
    if ( bg->ulong==EGSCOLOR_DEFAULT || fg->ulong==EGSCOLOR_DEFAULT ) {
        Tegs_color fgd,bgd;
        vgui_wn_getColorEGS(gui,wn, &fgd, &bgd);
        if ( fg->ulong==EGSCOLOR_DEFAULT ) *fg=fgd;
        if ( bg->ulong==EGSCOLOR_DEFAULT ) *bg=bgd;
    }
}

extern CstrBrief widget_getLabel(Widget w)
{
    CstrBrief text;
    XtVaGetValues(w,XtNlabel,&text,NULL);
    return text;
}

extern CstrBrief widget_getString(Widget w)
{
    CstrBrief text;
    XtVaGetValues(w,XtNstring,&text,NULL);
    return text;
}

extern void widget_setLabel (Widget w, CstrBrief text) { XtVaSetValues(w,XtNlabel,text,NULL); }
extern void widget_setString(Widget w, CstrBrief text) { XtVaSetValues(w,XtNstring,text,NULL); }

/*======================================================================*/
/*= widget geometry                                                    =*/

extern int widget_bdw(Widget w)
{
    Dimension bd;
    XtVaGetValues(w, XtNborderWidth,&bd, NULL);
    return bd;
}

extern void widget_size(Widget w, int*_dx, int*_dy)
{
    Dimension dx,dy;
    XtVaGetValues(w, XtNwidth,&dx, XtNheight,&dy, NULL);
    if (_dx) *_dx =dx;
    if (_dy) *_dy =dy;
}

extern void widget_geometry(Widget w, int*_x, int*_y, int*_dx, int*_dy)
{
    Position   x, y;
    Dimension dx,dy;
    XtVaGetValues(w, XtNx,&x, XtNy,&y, XtNwidth,&dx, XtNheight,&dy, NULL);
    if (_x) *_x=x;
    if (_y) *_y=y;
    if (_dx) *_dx =dx;
    if (_dy) *_dy =dy;
}

void widget_setSameDx(Cvgui*gui, ...)
{
    va_list ap;
    va_start(ap,gui);

    Widget w = va_arg(ap,Widget);
    if ( w==0 ) {
        va_end(ap);
        return;
    }
 
    Dimension len,max=0,h,b;
    //XtVaGetValues(w,XtNwidth, &max, NULL);
    for ( /*w=va_arg(ap,Widget)*/ ; w!=0 ;  w=va_arg(ap,Widget) ) {
        XtVaGetValues(w,XtNwidth, &len, NULL);
        if ( len>max) max=len;
    }
    va_end(ap);
    
    va_start(ap,gui);
    for ( w=va_arg(ap,Widget) ; w!=0 ;  w=va_arg(ap,Widget) ) {
        XtVaGetValues(w,XtNwidth, &len, XtNheight, &h, XtNborderWidth, &b, NULL);
        if ( len<max ) {
            XtResizeWidget(w,max,h,b);
        }
    }
    va_end(ap);
}

void widget_setSameDy(Cvgui*gui, ...)
{
    va_list ap;
    va_start(ap,gui);

    Widget w = va_arg(ap,Widget);
    if ( w==0 ) {
        va_end(ap);
        return;
    }
 
    Dimension len,max=0,h,b;
    //XtVaGetValues(w,XtNwidth, &max, NULL);
    for ( /*w=va_arg(ap,Widget)*/ ; w!=0 ;  w=va_arg(ap,Widget) ) {
        XtVaGetValues(w,XtNheight, &len, NULL);
        if ( len>max) max=len;
    }
    va_end(ap);
    
    va_start(ap,gui);
    for ( w=va_arg(ap,Widget) ; w!=0 ;  w=va_arg(ap,Widget) ) {
        XtVaGetValues(w,XtNwidth, &len, XtNheight, &h, XtNborderWidth, &b, NULL);
        if ( h<max ) {
            XtResizeWidget(w,len,max,b);
        }
    }
    va_end(ap);
}

#if 0
#define DBG_resize_to(id) printf("%-25s:%-4s: pw=%12p cw=%12p sw=%12p : pwdx/y=%3d %3d : h/vsep=%2d %2d : cw/x/y=%3d %3d : cwdx/dy=%3d %3d --> %3d %3d (sw=%3d %3d %3d %3d)\n", __func__,id, pw,cw,sw, pdx,pdy, hsep,vsep, cx,cy,cdx,cdy,nszdx,nszdy,swx,swy,swdx,swdy)
#define DBG_ToE(id)  DBG_resize_to(id)
#define DBG_ToS(id)  DBG_resize_to(id)
#define DBG_ToSE(id) DBG_resize_to(id)
#else
#define DBG_ToE(id)  while(0)
#define DBG_ToS(id)  while(0)
#define DBG_ToSE(id) while(0)
#endif

#define CPW_INFOS(cw) \
    Position  cx,cy;           /* position of cw in parent widget */ \
    Dimension cdx,cdy,cbw;     /* size & border .. of cw */ \
    int       hsep,vsep;      \
    Dimension pdx,pdy;         /* parent widget size (pw) */ \
    Dimension nszx=0,nszy=0;   /* new expected x & y */ \
    Dimension nszdx=0,nszdy=0; /* & width & height for cw. */ \
    Position  swx=0,swy=0; \
    Dimension swdx=0,swdy=0; \
    XtVaGetValues(cw, XtNhorizDistance,&hsep, XtNvertDistance,&vsep, \
        XtNwidth,&cdx, XtNheight,&cdy, XtNborderWidth,&cbw, \
        XtNx,&cx, XtNy,&cy, NULL); \
    Widget pw = XtParent(cw); \
    XtVaGetValues(pw, XtNwidth,&pdx, XtNheight,&pdy, NULL); \
    nszdx = pdx-cx-2*hsep-2*cbw; \
    nszdy = pdy-cy-hsep-2*cbw

#define SW_INFOS(sw) do { \
        int swhsep,swvsep; \
        XtVaGetValues(sw, XtNvertDistance,&swvsep, XtNx,&swx,      XtNy,&swy,       NULL); \
        XtVaGetValues(sw, XtNhorizDistance,&swhsep, XtNwidth,&swdx, XtNheight,&swdy, NULL); \
        nszdx = pdx-cx-2*swhsep-2*cbw; \
        nszdy = swy-cy-swvsep-2*cbw; } while (0)

extern void widget_resize_toEast(Widget cw, Widget sw)
{
    CPW_INFOS(cw);
    if ( sw==0 ) {
        if ( cdy>0 && nszdx > 0 && nszdx<=0x7FFF) {
DBG_ToE("toE");
            XtResizeWidget(cw,nszdx,cdy,cbw);
            XtVaSetValues(cw, XtNwidth,nszdx, XtNheight,cdy, XtNright,XtChainRight, NULL);
        } else
DBG_ToE("toE-");
    } else {
        PERM_spe_printf("%s","Not Yet implemented when sw!=0");
    }
}

extern void widget_resize_toSouth(Widget cw, Widget sw)
{
    CPW_INFOS(cw);
    if ( sw!=0 ) SW_INFOS(sw);
    if ( cdx>0 && nszdy>0 && nszdy<=0x7FFF) {
DBG_ToS("toS");
        XtResizeWidget(cw,cdx,nszdy,cbw);
        XtVaSetValues(cw, XtNwidth,cdx, XtNheight,nszdy, XtNbottom,XtChainBottom, NULL);
    } else
DBG_ToS("toS-");
}

extern void widget_resize_toSE(Widget cw, Widget sw)
{
    CPW_INFOS(cw);

    if ( sw!=0 ) SW_INFOS(sw);
    if ( nszdx>0 && nszdy>0 && nszdx<0x7FFF && nszdy<0x7FFF ) {
DBG_ToSE("toSE");
        XtResizeWidget(cw,nszdx,nszdy,cbw);
        XtVaSetValues(cw, XtNwidth,nszdx, XtNheight,nszdy, XtNbottom,XtChainBottom, XtNright,XtChainRight, NULL);
DBG_ToSE("toSE2");
    } else if ( cdx>0 && nszdy>0 && nszdy<0x7FFF ) {
DBG_ToSE("toS");
        XtResizeWidget(cw,cdx,nszdy,cbw);
        XtVaSetValues(cw, XtNwidth,cdx, XtNheight,nszdy, XtNbottom,XtChainBottom, NULL);
    } else if ( cdy>0 && nszdx>0 && nszdx<0x7FFF ) {
DBG_ToSE("toE");
        XtResizeWidget(cw,nszdx,cdy,cbw);
        XtVaSetValues(cw, XtNwidth,nszdx, XtNheight,cdy, XtNright,XtChainRight, NULL);
    } else 
DBG_ToSE("to--");
}

/*======================================================================*/
/*= place wideget in container                                         =*/

extern void widget_place_widget(Tegs_widget w,
        int top, int   bot, int   left, int   right,
        int hspace, Tegs_widget hwidget,
        int vspace, Tegs_widget vwidget)
{
    XT_DeclareArgs(10);

    ArgCount = 0;
    if ( top>=0 )      XT_NextArg(XtNtop,    top);
    if ( bot>=0 )      XT_NextArg(XtNbottom, bot);
    if ( left>=0 )     XT_NextArg(XtNleft,   left);
    if ( right>=0 )    XT_NextArg(XtNright,  right);
    if ( hspace>=0 )   XT_NextArg(XtNhorizDistance, hspace);
 // if ( hwidget!=0 )  XT_NextArg(XtNfromHoriz,     hwidget);
                       XT_NextArg(XtNfromHoriz,     hwidget);
    if ( vspace>=0 )   XT_NextArg(XtNvertDistance,  vspace);
 // if ( vwidget!=0 )  XT_NextArg(XtNfromVert,      vwidget);
                       XT_NextArg(XtNfromVert,      vwidget);
    //XT_NextArg(XtNresize,      1); // IVG
    //XT_NextArg(XtNresizable,   1); // IVG
    XtSetValues((Widget)w,Args,ArgCount);
}

/*======================================================================*/
