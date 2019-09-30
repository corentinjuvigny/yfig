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
 *      $Id: guiqt-top.h 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-top.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This headers groups the EGS resources and basic utilities that are =*/
/*= required everywhere.                                               =*/
/*=                                                                    =*/
/*= The EGS resources are grouped in the Tgui_main structure.          =*/
/*=                                                                    =*/
/*= The basic utilities are mainly:                                    =*/
/*=    - macros thay cast VGUI egs type to the true EGS types          =*/
/*=      (eg: Tcolor to QtColor).                                      =*/
/*=    - some initialization and free functions.                       =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_TOP_H
#define FILE_GUIQT_TOP_H
/*======================================================================*/

#include "guiqt-config.h"
#include "tools/font.h"

inline QPixmap* PXMegs2qt(TpixmapEGS egs) {
    if ( PXMEGS_IsNone(egs) ) return 0;
    else                      return (QPixmap*)egs.ptr; }
inline TpixmapEGS PXMqt2egs(QPixmap*qp) {
    if ( qp==0 ) return PXMEGS_none;
    TpixmapEGS ep; ep.ptr=qp; return ep; }

inline QFont* FONTegs2qt(TfontEGS egs) {
    if ( FONTEGS_IsNone(egs) ) return 0;
    else                       return (QFont*)egs.ptr; }
inline TfontEGS FONTqt2egs(QFont*qf) {
    if ( qf==0 ) return FONTEGS_none;
    TfontEGS ef; ef.ptr=qf; return ef; }

#include "guiqt-topform.h"
#include "guiqt-gc.h"

/*======================================================================*/
/*= EGS resources and shortcut                                         =*/

struct _Tgui_main {
    // initialized in gui_open
    QApplication* m_application;
    WtopForm*     m_topForm;         // the top container
    
    // Graphic Context
    TGUI_gc m_gcs[OD_GC_NB];
} ;

#define E_Application   (gui->g_main->m_application)
#define E_TopForm       (gui->g_main->m_topForm)
#define E_GC(gcd)       (gui->g_main->m_gcs+(gcd))

/*======================================================================*/
/*= some macros for colors                                             =*/

#undef  COLOR
#undef  QtCOLOR

#define GUI_RGB(r,g,b) "#" #r #g #b

#define QtCOLOR(x) (*((QColor*)((x).ptr)))
#define COLOR(x) QtCOLOR((x)->egs)

/*======================================================================*/
/*= some converters from VGUI enum to QT enum                          =*/

extern void conv_accelDef2str(char* buf, TaccelDef accelDef);

/*======================================================================*/
/*= Some utilities internal to QT                                      =*/

inline char* get_strBuffer(int sz)
{
    static int   current=0;
    static char* buffers[10];
    static int   bufferNb = sizeof(buffers)/sizeof(buffers[0]);

    current = (current+1) % bufferNb;
    buffers[current] = (char*)realloc(buffers[current],sz);
    return buffers[current];
}

inline char* conv_QStr2CStrLa15(QString& qstr)
{
    QByteArray ba = qstr.toLatin1();
    char *ret = get_strBuffer(ba.size()+1);
    strcpy(ret,ba.constData());
    return ret;
}

inline char* conv_QStr2CStrUtf8(QString& qstr)
{
    QByteArray ba = qstr.toUtf8();
    char *ret = get_strBuffer(ba.size()+1);
    strcpy(ret,ba.constData());
    return ret;
}


inline char* dup_QStr2CStr(QString& qstr)
{
    QByteArray ba = qstr.toLatin1();
    return strdup(ba.constData());
}

/*======================================================================*/
#endif // FILE_GUIQT_TOP_H
/*======================================================================*/
