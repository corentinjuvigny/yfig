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
 *      $Id: widxt-egs.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-egs.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Operations on EGS widget (EGS driver).                             =*/
/*=                                                                    =*/
/*= See the vgui/wid-egs.h header for the function description and     =*/
/*= for documentation.                                                 =*/
/*======================================================================*/

#include "guixt.h"
#include "vgui/vgui-accelkey.h"
#include "vgui/wid-egs.h"

/*======================================================================*/

void gui_widegs_addTip(Tegs_widget w, CstrBrief hs)
{
    if ( hs==0 ) return;

    Widget widget = w;
#if USE_XAW==1
    XtVaSetValues(widget,
            XtNtip,      hs,
          //XtNtimeout,  (int)0,  // default: 500
            NULL);
#elif USE_XAW3D==1
    XawTipEnable(widget,(char*)hs);
#endif
}
void gui_widegs_addTipAA(Tegs_widget w, CstrBrief hs,
                         TaccelDef a1, TaccelDef a2)
{
    if ( hs==0 ) return;

    char* helpText = accel_appendToString(hs,a1,a2);
    gui_widegs_addTip(w,helpText);
    tool_free(helpText);
}

void  gui_widegs_putW1inNEofW2(Tvgui*gui,Tegs_widget w1,Tegs_widget w2)
{
    Tgui_main*   main = gui->g_main;
    int pdx = vgui_widegs_getDX(w2); 
    int dx  = vgui_widegs_getDX(w1); 
    XReparentWindow(E_Display,_XtWindow(w1),_XtWindow(w2), pdx-dx, 0);
}

extern void gui_widegs_disable(Tegs_widget w) { XtSetSensitive(w,False); }
extern void gui_widegs_enable (Tegs_widget w) { XtSetSensitive(w,True); }

extern void gui_widegs_unshow (Tegs_widget w) { XtUnmanageChild(((Widget)w));}
extern void gui_widegs_show   (Tegs_widget w) { XtManageChild(((Widget)w));}
extern int  gui_widegs_isShown(Tegs_widget w)
{
    Display* display = XtDisplay( (Widget)w );
    Window xwin = XtWindow( (Widget)w );
    if ( xwin==0 ) return 0;
    XWindowAttributes att;
    if ( XGetWindowAttributes( display, xwin, &att)==0 ) {
        return 0;
    }
    return att.map_state!=IsUnmapped;
}

//extern void gui_widegs_unmap (Tegs_widget w) { XtUnmapWidget(((Widget)w));}
//extern void gui_widegs_map   (Tegs_widget w) { XtMapWidget(((Widget)w));}

extern void gui_widegs_children(Tegs_widget w, Tegs_widSet* set)
{
    int i;
    Cardinal   numChildren;
    WidgetList children;
    XtVaGetValues(w,
            XtNchildren,    &children,
            XtNnumChildren, &numChildren,
            NULL);
    for ( i=0 ; i<numChildren ; i+=1 ) {
        Tegs_widget w = children[i];
        egswidset_add(set,w);
    }
}

/*======================================================================*/

extern void gui_widegs_setFgColor (Tegs_widget w, TcolorEGS c)
{ XtVaSetValues(w, XtNforeground,c, NULL); }
extern void gui_widegs_setBgColor (Tegs_widget w, TcolorEGS c)
{ XtVaSetValues(w, XtNbackgroundPixmap,XtUnspecifiedPixmap,
                   XtNbackground,c, NULL); }
extern void gui_widegs_setBgPxm   (Tegs_widget w, TpixmapEGS pxm)
{
    Pixmap xpxm = ! PXMEGS_IsNone(pxm) ? PXMEGS2X11(pxm) : (Pixmap)XtUnspecifiedPixmap ;
    XtVaSetValues(w, XtNbackgroundPixmap,(Pixmap)0, NULL); // FIXME: without that, xpxm is not drawn
    XtVaSetValues(w, XtNbackgroundPixmap,xpxm, NULL);
}

extern int   gui_widegs_getGeometry(Tegs_widget w,
                int*x, int*y, int*dx, int*dy, int*bdt)
{
    Position _x,_y;
    Dimension _dx,_dy,_bdt;

    XtVaGetValues(w
            ,XtNx,      &_x
            ,XtNy,      &_y
            ,XtNwidth,  &_dx
            ,XtNheight, &_dy
            ,XtNborderWidth, &_bdt
            ,NULL);
    if (x)   *x=_x;
    if (y)   *x=_y;
    if (dx)  *dx=_dx;
    if (dy)  *dy=_dy;
    if (bdt) *bdt=_bdt;
}

extern void gui_widegs_setDY(Tegs_widget w, int dy)
{
    Dimension _dy=dy;
    XtVaSetValues(w, XtNheight,_dy, XtNresize,(Boolean)False, NULL);
}
extern void gui_widegs_setDX(Tegs_widget w, int dx)
{
    Dimension _dx=dx;
    XtVaSetValues(w, XtNwidth,_dx, NULL);
}
extern void gui_widegs_setSize(Tegs_widget w, int dx, int dy)
{
    Dimension _dx=dx, _dy=dy;
    XtVaSetValues(w, XtNwidth,_dx, XtNheight,_dy, NULL);
}

/*======================================================================*/

extern int gui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text)
{ 
#warning gui_string_width must be improved
    if ( text==0 ) return 0;
    double len = strlen(text);
    if ( len==0 ) return 0;
    return round( len*gui->g_mPts*0.75 ); // FIXME: we must use a font here
}

/*======================================================================*/
