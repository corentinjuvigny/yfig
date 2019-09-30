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
 *      $Id: guiqt-outdev-font.cc 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-outdev-font.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver)      =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "outdev/resources.h"

/*======================================================================*/
/*= Private utilities for un/loading fonts and get string extents.     =*/

static void gui_font_unload (Tvgui* gui, TfontEGS f)
{
    QFont* fegs = FONTegs2qt(f);
    delete fegs;
}

static QFont* gui_fonts_new(Tvgui* gui, CfontDef* fd, double pts)
{
    Cstr family = fd->family;
    Cstr att    = fd->style;

    QFont::Style style = QFont::StyleNormal;
    if ( strstr(att,"italic") !=0 ) style = QFont::StyleItalic;
    if ( strstr(att,"oblique")!=0 ) style = QFont::StyleOblique;

    QFont::Weight weight = QFont::Normal;
#ifdef QT_VERS_5
    if ( strstr(att,"medium") !=0 ) weight = QFont::Medium;
#endif
    if ( strstr(att,"light")  !=0 ) weight = QFont::Light;
    if ( strstr(att,"demi")   !=0 ) weight = QFont::DemiBold;
    if ( strstr(att,"bold")   !=0 ) weight = QFont::Bold;
    
    QFont::StyleHint hint = QFont::AnyStyle;
    if ( strstr(att,"sans")  !=0 ) hint = QFont::SansSerif;
    if ( strstr(att,"serif") !=0 ) hint = QFont::Serif;
    if ( strstr(att,"typewriter") !=0 ) hint = QFont::TypeWriter;
    if ( strstr(att,"monspace")   !=0 ) hint = QFont::Monospace;
    
    QFont* fqt = new QFont(family,pts,weight);
    fqt->setStyle(style);
    fqt->setStyleHint(hint);

#if defined(TOOL_DBG_Trace) && TOOL_DBG_Trace
    if ( fqt!=0 ) {
        QString str = fqt->toString();
        char*   s   = dup_QStr2CStr(str);
        int     fi  = VG_fontdefIndex(gui,fd);
        DBG2_Font("fi=%d pts=%f basename=%s exact=%3s : str=%s",
                fi,pts,fd->family, fqt->exactMatch() ? "Yes" : "No", s);
        tool_free( s );
    }
#endif

    return fqt;
}

static TfontEGS gui_font_load0(Tvgui* gui, CfontDef*fd, double pts, double*ratio)
{
    QFont* fqt = gui_fonts_new(gui,fd,pts);
    if ( fqt!=0 ) {
        *ratio = 1;
        return FONTqt2egs(fqt);
    } else {
        *ratio = 0;
        return FONTEGS_none;
    }
}

static TfontEGS gui_font_load(Tvgui* gui, int fi, double pts, double*ratio)
{
    CfontDef* fd = VG_fontdef(gui,fi);
    TfontEGS  fegs;
    DBG1_Font("look for fi=%d pts=%f family=%s",fi,pts,fd->family);
    fegs = gui_font_load0(gui,fd,pts,ratio);
    DBG0_Font("look for fi=%d pts=%f family=%s: %s",fi,pts,fd->family,
            FONTEGS_IsNone(fegs) ? "NOT FOUND" : "found");
    return fegs;
}

static TfontEGS  gui_font_loadCacheEGS (Tvgui*gui, int fi, double pts)
{
    TodResources* res = gui->vg_odResources;
    CfontInfo*  finfo =  fontset_searchLRU(res->fontCache,fi,pts);
    if ( finfo==0 ) {
        // finfo = od_str_fontLoadAndCache(od,fi,pts);
        double    fontratio;
        TfontEGS  fontEGS;

        // get font
        fontEGS = gui_font_load(gui,fi, pts, &fontratio);
        if ( FONTEGS_IsNone(fontEGS) ) 
            fontEGS = gui_font_load(gui,0,pts,&fontratio);
        finfo = od_fontset_addLRU(gui->vg_od,fi,pts,fontEGS,fontratio);
    }
    return finfo->fo_egs;
}

static void gui_fonts_extents_DesUnit0(Tvgui*gui, TfontEGS f,
        CstrBrief text, int*ascent, int*descent, int*dx, int*dy)
{
    QFont* fqt = FONTegs2qt(f);
    QFontMetrics fm(*fqt);
    if ( ascent  ) *ascent  = fm.ascent();
    if ( descent ) *descent = fm.descent();
    if ( dx      ) *dx      = fm.width(text);
    if ( dy      ) *dy      = fm.height();
}

/*======================================================================*/
/*= Interface for guiqt-init.c                                         =*/

extern void gui_fonts_initGC (Tvgui* gui, Tod_GCdef gcd,
                int fi, double pts, Ccolor* fg, Ccolor* bg)
{
    double ratio;
    TGUI_gc* gc = E_GC(gcd);
    gc->t_pen.setColor( COLOR(fg) );
    TfontEGS f = gui_font_load(gui,fi,pts,&ratio); 
    gc->t_fontinfo = fontinfo_new(fi,pts,f,ratio);
}

/*======================================================================*/
/*= Interface for VGUI.                                                =*/

extern void gui_fonts_extents_DesUnit(Tvgui*gui, int fontid, double fontsize,
        CstrBrief str, int*asc, int*desc, int*dx, int*dy)
{
    TfontEGS  fegs = gui_font_loadCacheEGS (gui, fontid, fontsize);
    gui_fonts_extents_DesUnit0(gui, fegs, str, asc, desc, dx, dy);
}

extern void gui_fonts_lineinfo_DesUnit(Tvgui*gui, int fontid, double fontsize,
        int*idy, int*ldy, int*ils)

{
    TfontEGS  fegs = gui_font_loadCacheEGS (gui, fontid, fontsize);
    QFont*    fqt  = FONTegs2qt(fegs);

    double pts;
    pts = fqt->pixelSize();
    if ( pts==-1 ) {
        pts = fqt->pointSize();
        pts*= VG_ppiScreen(gui)/72;
    }

    QFontMetrics fm(*fqt);
    int    dy  = fm.height();
    if ( idy!=0 ) *idy = round( pts );
    if ( ldy!=0 ) *ldy = round( dy );
    if ( ils!=0 ) *ils = round( dy-pts );
}

/*======================================================================*/
/*= Interface for output device.                                       =*/

static TfontEGS odvt_fontLoadPts (void*dev, int fi, double pts, double*ratio)
{
    TVARS_VodGui(dev);
    return gui_font_load(gui,fi,pts,ratio);
}

static void  odvt_fontUnload  (void*dev, TfontEGS f)
{
    TVARS_VodGui(dev);
    return gui_font_unload(gui,f);
}

static void odvt_extentsPix  (void*dev, TfontEGS f, CstrBrief text,
                   double* ascent, double* descent, double* dx, double* dy)
{
    TVARS_VodGui(dev);
    int vascent, vdescent, vdx, vdy;
    gui_fonts_extents_DesUnit0(gui, f, text, &vascent, &vdescent, &vdx, &vdy);
    if ( ascent  ) *ascent  =  vascent ;
    if ( descent ) *descent =  vdescent;
    if ( dx      ) *dx      =  vdx ;
    if ( dy      ) *dy      =  vdy ;
}

#define DBG_Fmt "gcd=%d f=%d/%f fg=%s"
#define DBG_Arg gcd,font->fo_index,font->fo_size,DBG_Color2str(fg)

static void odvt_setGCstr    (void*dev, Tod_GCdef gcd, CfontInfo* font, Ccolor* fg)
{
    DBG1_FontE(DBG_Fmt,DBG_Arg);
    TVARS_VodGui(dev);
    TGUI_gc* gc = E_GC(gcd);
    od_fontinfo_chgLazy((Toutdev*)dev, &gc->t_fontinfo, font);
    if (fg ) {
        QColor& qc   = COLOR(fg);
        gc->t_pen.setColor(qc);
    }
    DBG1_FontL(DBG_Fmt,DBG_Arg);
}

static void odvt_drawWL_Pix(void* dev, Tod_GCdef gcd,
                        double x, double y, CstrBrief text)
{
    if ( text==0 || *text==0 ) return;
    TVARS_VodGui(dev);
    TGUI_gc* gc = E_GC(gcd);

    QPainter* painter = gc->t_painterOpen(vod);
      painter->drawText(round(x),round(y),text);
    gc->t_painterClose(vod);
}

static void odvt_drawWLr_Pix(void* dev, Tod_GCdef gcd,
                        double x, double y, CstrBrief text, double a)
{
    if ( text==0 || *text==0 ) return;
    TVARS_VodGui(dev);
    TGUI_gc* gc = E_GC(gcd);
    TGUI_DrawingArea* area = (TGUI_DrawingArea*)vod->v_cvsWidEGS;
    gc->t_painterOpen(area->painter,area->drawnPicture);
      a = a/M_PI*180.;
      area->painter->translate(x,y);
      area->painter->rotate(-a);
      area->painter->drawText(0,0,text);
      area->painter->rotate(a);
      area->painter->translate(-x,-y);
    gc->t_painterClose(area->painter,area);
}

/*======================================================================*/
/*= vim: set textwidth=0:                                              =*/
/*======================================================================*/
