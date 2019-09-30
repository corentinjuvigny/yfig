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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: widqt-egs.cc 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-egs.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Operations on EGS widget (EGS driver).                             =*/
/*=                                                                    =*/
/*= See the vgui/wid-egs.h header for the function description and     =*/
/*= for documentation.                                                 =*/
/*======================================================================*/

#include "guiqt.h"
#include "vgui/vgui-accelkey.h"

#ifndef FILE_VGUI_WIDGET_H
#error ---- Missing #include "vgui/wid-egs.h"
#endif

/*======================================================================*/

void gui_widegs_addTip(Tegs_widget w, CstrBrief hs)
{
    if ( hs==0 ) return;

    QWidget* widget = (QWidget*)w;
    widget->setToolTip(hs);
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
    QWidget* pw = (QWidget*)w2;  // parent
    QWidget* cw = (QWidget*)w1;  // child
    int pdx = vgui_widegs_getDX(w2); 
    int dx  = vgui_widegs_getDX(w1); 
    cw->setParent(pw);
    cw->move( pdx-dx, 0 );
    //XReparentWindow(E_Display,_XtWindow(w1),_XtWindow(w2), pdx-dx, 0);
}

extern void gui_widegs_disable(Tegs_widget w)
{
    QWidget* self = (QWidget*)w;
    self->setEnabled(false);
}
extern void gui_widegs_enable (Tegs_widget w)
{
    QWidget* self = (QWidget*)w;
    self->setEnabled(true);
}

extern void gui_widegs_unshow (Tegs_widget w)
{
    QWidget* self = (QWidget*)w;
    self->setVisible(false);
}
extern void gui_widegs_show   (Tegs_widget w)
{
    QWidget* self = (QWidget*)w;
    self->setVisible(true);
}
extern int  gui_widegs_isShown(Tegs_widget w)
{
    QWidget* self = (QWidget*)w;
    return self->isVisible();
}

extern void gui_widegs_children(Tegs_widget w, Tegs_widSet* set)
{
    QWidget* self = (QWidget*)w;
    const QObjectList& ol = self->children();

    foreach( const QObject*o,ol) {
        const QFrame* qf; const QWidget* qw;
        if ( !o->isWidgetType() ) continue;
        if ( (qf=qobject_cast<const QFrame *>(o)) ) {
            // this case is for sub-dialog within a frame.
            // it hardly linked with the gui_dw_create function.
            const QObjectList& olf = qf->children();
            if ( olf.size()>1 ) {
                gui_widegs_children((Tegs_widget)qf,set);
                return;
            }
        }
        qw = (QWidget*)o;
        egswidset_add(set,(Tegs_widget)qw);
    }
}

/*======================================================================*/

extern void gui_widegs_setFgColor (Tegs_widget w, TcolorEGS c)
{
    QWidget* self = (QWidget*)w;
    QPalette pal = self->palette();
    pal.setColor( QPalette::WindowText,QtCOLOR(c) );
    pal.setColor( QPalette::Text,      QtCOLOR(c) );
    self->setPalette(pal);
    self->setAutoFillBackground(true);
}

extern void gui_widegs_setBgColor (Tegs_widget w, TcolorEGS c)
{
    QWidget* self = (QWidget*)w;
    QPalette pal = self->palette();
    pal.setColor( QPalette::Window, QtCOLOR(c) );
    pal.setColor( QPalette::Base,   QtCOLOR(c) );
    pal.setColor( QPalette::Button, QtCOLOR(c) );
    self->setPalette(pal);
    self->setAutoFillBackground(true);
}

extern void gui_widegs_setBgPxm   (Tegs_widget w, TpixmapEGS pxm)
{
    QWidget* self = (QWidget*)w;
    self->setAutoFillBackground(true);
    QPalette pal(self->palette());
    pal.setBrush(QPalette::Window,QBrush(*PXMegs2qt(pxm)));
    self->setPalette(pal);
}

extern int   gui_widegs_getGeometry(Tegs_widget w,
                int*x, int*y, int*dx, int*dy, int*bdt)
{
    QWidget* self = (QWidget*)w;
    if (x)   *x   = self->pos().x();
    if (y)   *x   = self->pos().y();
    if (dx)  *dx  = self->width();
    if (dy)  *dy  = self->height();
    if (bdt) *bdt = 0;
}

extern void gui_widegs_setDY(Tegs_widget w, int dy)
{
    QWidget* self = (QWidget*)w;
    self->setMinimumHeight( dy );
    self->setMaximumHeight( dy );
}
extern void gui_widegs_setDX(Tegs_widget w, int dx)
{
    QWidget* self = (QWidget*)w;
    self->setMinimumWidth( dx );
    self->setMaximumWidth( dx );
}
extern void gui_widegs_setSize(Tegs_widget w, int dx, int dy)
{
    QWidget* self = (QWidget*)w;
    QSize    size(dx,dy);
    self->setMinimumSize( size );
    self->setMaximumSize( size );
}

/*======================================================================*/

extern int gui_string_width(Tvgui*gui, CstrBrief wn, CstrBrief text)
{ 
#warning gui_string_width must be improved
    if ( text==0 ) return 0;
    int len = strlen(text);
    if ( len==0 ) return 0;
    return round( len*gui->g_mPts*0.75 ); // FIXME: we must use a font here
}

/*======================================================================*/
