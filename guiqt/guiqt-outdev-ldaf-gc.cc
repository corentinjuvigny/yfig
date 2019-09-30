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
 *      $Id: guiqt-outdev-ldaf-gc.cc 113 2018-01-09 18:35:22Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-ldaf-gc.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Graphic Context (GC) EGS driver.                                   =*/
/*=                                                                    =*/
/*= See the vgui/vgui-ldaf.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guiqt/guiqt-util.h"

/*======================================================================*/

static void odvt_setGCso(void*od, Tod_GCdef gcd,
    double th, TlineStyle ls, TjoinStyle js, TcapStyle  cs, TcolorEGS fg)
{
    TVARS_VodGui(od);
    TGUI_gc* gc  = E_GC(gcd);
    QColor&  qc  = QtCOLOR(fg);

/*printf("%s: gcd=%d gc=%p w=%d:%d ls=%d:%d js=%d:%d cs=%d:%d fg=%d:%ld\n",__func__,
        gcd, gc,
        (th>=0 && th!=gc->lineWidth)  , th,
        (ls!=VGUI_GC_LS_None)               , ls,
        (js!=VGUI_GC_JS_None)               , js,
        (cs!=VGUI_GC_CS_None)               , cs,
        (fg!=EGSCOLOR_NONE && gc->drawColor != qc), fg);*/
    if ( th>=0 && th!=gc->lineWidth) {
        gc->lineWidth = th;
        gc->pen.setWidth(th);
    }
    if ( ls!=DA_LS_None ) {
        Qt::PenStyle qls =  conv_lineStyle_od2qt(ls);
        if ( qls!=Qt::NoPen ) {
            gc->lineStyle = qls;
            gc->pen.setStyle(qls);
        }
    }
    if ( js!=DA_JS_None ) {
        Qt::PenJoinStyle qjs =  conv_joinStyle_od2qt(js);
        gc->joinStyle = qjs;
        gc->pen.setJoinStyle(qjs);
    }
    if ( cs!=DA_CS_None ) {
        Qt::PenCapStyle qcs =  conv_capStyle_od2qt(cs);
        gc->capStyle = qcs;
        gc->pen.setCapStyle(qcs);
    }
    if (fg.ulong!=EGSCOLOR_NONE && gc->drawColor != qc ) {
        gc->drawColor  = qc;
        gc->pen.setColor(qc);
    }
}

static void odvt_setGCdd(void*od, Tod_GCdef gcd,
                         double th, TlineStyle ls, TcolorEGS ddh)
{
    TVARS_VodGui(od);
    TGUI_gc* gc  = E_GC(gcd);
    if ( ls!=DA_LS_None ) {
        switch (ls) {
          case DA_LS_Dot :
          case DA_LS_Dash1Dot :
          case DA_LS_Dash2Dot :
          case DA_LS_Dash3Dot :
            gc->capStyle = Qt::RoundCap;
            gc->pen.setCapStyle(gc->capStyle);
        }
        Qt::PenStyle qls =  conv_lineStyle_od2qt(ls);
        if ( qls!=Qt::NoPen ) {
            gc->lineStyle = qls;
            gc->pen.setStyle(qls);
        }
    }
#if 0
    if ( ddn>0 && ls==DA_LS_Dash3Dot ) {
        int i,ii=0;
        QVector<qreal> dashes(ddn);
        for ( i=0 ; i<ddn ; i+=2 ) dashes[ii++] = ddt[i];
        for ( i=1 ; i<ddn ; i+=2 ) dashes[ii++] = ddt[i];
        gc->pen.setDashPattern(dashes);
    }
#endif
}

static void odvt_setGCfillColor(void*od, Tod_GCdef gcd, TcolorEGS c)
{
    TVARS_VodGui(od);
    TGUI_gc* gc  = E_GC(gcd);
    gc->fillBrush.setColor( QtCOLOR(c) );
    gc->fillBrush.setStyle( Qt::SolidPattern );
}

//static void odvt_setGCfillColorBrightness(
//                                void*dev, Tod_GCdef gcd, TcolorEGS c,
//                                double coef);

static void odvt_setGCfillTile (void*od, Tod_GCdef gcd, TpixmapEGS pxm)
{
    TVARS_VodGui(od);
    TGUI_gc* gc  = E_GC(gcd);
    gc->fillBrush.setColor( COLOR(gui->vg_white) );
    gc->fillBrush.setTexture( *PXMegs2qt(pxm) );
    gc->fillBrush.setStyle( Qt::TexturePattern );
}

/*======================================================================*/
