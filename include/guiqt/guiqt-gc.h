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
 *      $Id: guiqt-gc.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/guiqt/guiqt-gc.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * Utilities for handling graphic context.
 *
 * The TGUI_gc class groups different contexts:
 *   - One for drawing lines and curves
 *   - One for drawing strings.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_GUIQT_GC_H
#define FILE_GUIQT_GC_H
/*======================================================================*/

#include "guiqt/widqt-canvas.h"
#include "outdev/types.h"
#include "outdev/outdev-vt.h"

/*======================================================================*/
/*= some converter from VGUI enum to QT enum                           =*/

extern Qt::PenStyle     conv_lineStyle_Fig2Qt(TlineStyle fig);
extern Qt::PenJoinStyle conv_joinStyle_Fig2Qt(TjoinStyle fig);
extern Qt::PenCapStyle  conv_capStyle_Fig2Qt (TcapStyle fig);

/*======================================================================*/

struct TGUI_gc {
    void init(Tvgui* gui);

  /* part for drawing points lines and curves */
    void l_init();     // initialize the next fields
    int                lineWidth; // thickness of curves, lines, ...
    Qt::PenStyle       lineStyle;
    Qt::PenJoinStyle   joinStyle;
    Qt::PenCapStyle    capStyle;
    QColor             drawColor; // color for drawing curves, lines, ...
    QPainter::CompositionMode     // how shapes are filled
                       fillStyle; // how lines are drawn
                                  // used both for line and text GC
    QPen               pen;       // the QT graphic context.

    QPainter           painter;
    inline void painterOpen(QPainter*painter,QPixmap*pixmap) {
        painter->begin(pixmap);
        painter->setPen(pen);
        painter->setCompositionMode(fillStyle);
        painter->setRenderHint(QPainter::Antialiasing);
    }
    inline void painterClose(QPainter*painter, TGUI_DrawingArea* area) {
        painter->end();
        area->update();
    }
    inline void painterClose(QPainter*painter) {
        painter->end();
    }
    inline QPainter* painterOpen(Cvgui_outdev*dev) {
        if ( dev->v_ident==VGUI_ODID_Canvas ) {
            TGUI_DrawingArea* area = (TGUI_DrawingArea*)dev->v_cvsWidEGS;
            painterOpen(area->painter,area->drawnPicture);
            area->painter->setRenderHint(QPainter::Antialiasing);
            return area->painter;
        } else {
            QPixmap* pixmap = PXMegs2qt(dev->v_pixmap);
            painterOpen(&painter, pixmap);
            return &painter;
        }
    }
    inline void painterClose(Cvgui_outdev*dev) {
        if ( dev->v_ident==VGUI_ODID_Canvas ) {
            TGUI_DrawingArea* area = (TGUI_DrawingArea*)dev->v_cvsWidEGS;
            painterClose(area->painter,area);
        } else {
            painterClose(&painter);
        }
    }

  /* part for filling shapes */
    QBrush      fillBrush;   // color for filling shapes

  /* part for drawing */
    QPen        t_pen;       // the QT graphic context,
                             // for fore/background colors
    TfontInfo*  t_fontinfo;  // the font used for drawing
                             //   - allocation is handled outside the class
    QFont&      t_font() { return *(QFont*)FONTegs2qt(t_fontinfo->fo_egs); }

    double      t_angle;     // rotation angle for drawing string
    inline void t_painterOpen(QPainter*painter,QPixmap*pixmap) {
        painter->begin(pixmap);
        painter->setPen(t_pen);
        painter->setFont(t_font());
        painter->setCompositionMode(fillStyle);
    }
    inline void t_painterClose(QPainter*painter, TGUI_DrawingArea* area) {
        painter->end();
        area->update();
    }
    inline void t_painterClose(QPainter*painter) {
        painter->end();
    }
    inline QPainter* t_painterOpen(Tvgui_outdev*dev) {
        if ( dev->v_ident==VGUI_ODID_Canvas ) {
            TGUI_DrawingArea* area = (TGUI_DrawingArea*)dev->v_cvsWidEGS;
            t_painterOpen(area->painter,area->drawnPicture);
            return area->painter;
        } else {
            QPixmap* pixmap = PXMegs2qt(dev->v_pixmap);
            t_painterOpen(&painter, pixmap);
            return &painter;
        }
    }
    inline void t_painterClose(Tvgui_outdev*dev) {
        if ( dev->v_ident==VGUI_ODID_Canvas ) {
            TGUI_DrawingArea* area = (TGUI_DrawingArea*)dev->v_cvsWidEGS;
            t_painterClose(area->painter,area);
        } else {
            t_painterClose(&painter);
        }
    }
};

/*======================================================================*/
#endif // FILE_GUIQT_GC_H
/*======================================================================*/
