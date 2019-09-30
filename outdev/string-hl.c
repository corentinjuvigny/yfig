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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: string-hl.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/string-hl.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= See the string-hl.h header for documentation.                      =*/
/*======================================================================*/

#include "outdev-config.h"
#include "outdev/style.h"
#include "outdev/resources.h"
#include "outdev/outdev.h"
#include "outdev/outdev-vt.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"
#include "tools/charset.h"
#include "tools/font.h"
#include "tools/color.h"

#define LUNIT_PtPerInch 72.

/*======================================================================*/

// load (fi,pts) font at EGS level, and cache it
// MUST BE USED only if (fi,pts) is not cached
static  CfontInfo* od_str_fontLoadAndCache(Toutdev* od, int fi, double pts)
{
    double    fontratio;
    TfontEGS  fontEGS;
    CfontInfo*fontinfo;

    // get font
    fontEGS = od_str_fontLoad(od, fi, pts, &fontratio);
    if ( FONTEGS_IsNone(fontEGS) ) 
        fontEGS = od_str_fontLoad(od,0,pts,&fontratio);
    fontinfo = od_fontset_addLRU(od,fi,pts,fontEGS,fontratio);

    return fontinfo;
}

// load (fi,pts) font at EGS level.
//   - Firstly, it searches the font in LRU cache
//   - Secondly, it loads the font and adds it to LRU cache
static TfontEGS   od_str_fontCacheEGS (Toutdev* od, int fi, double pts)
{
    CfontInfo* finfo =  fontset_searchLRU(od->resources->fontCache,fi,pts);
    if ( finfo==0 ) 
        finfo = od_str_fontLoadAndCache(od,fi,pts);
    return finfo->fo_egs;
}

static CfontInfo* od_str_fontCacheInfo(Toutdev* od, int fi, double pts)
{
    CfontInfo* finfo =  fontset_searchLRU(od->resources->fontCache,fi,pts);
    if ( finfo==0 ) 
        finfo = od_str_fontLoadAndCache(od,fi,pts);
    return finfo;
}

/*======================================================================*/
/*= Drawing strings (font handler)                                     =*/

TfontEGS od_str_fontLoad(Toutdev* od, int fi, double pts, double* ratio)
{
    TfontEGS ret;
    Coutdev_string_vt* vtable = &od->vtable->strVT;
    if ( vtable->fontLoadPts!=0 ) {
        ret = vtable->fontLoadPts(od,fi,pts,ratio);
    } else if ( vtable->fontLoadPts!=0 ) {
        int pix = round( (pts/LUNIT_PtPerInch) * od->ppiDes );
        ret = vtable->fontLoadPix(od,fi,pix,ratio);
    } else {
        PERM_spf_printf("No driver for loading font",0);
    }
    if ( FONTEGS_IsNone(ret) && fi==0 ) {
        // unable to load the default font (fi=0)
        PERM_spf_printf("No default font found",0);
    }
    return ret;
}

extern void od_str_fontUnload(Toutdev* od, TfontEGS f)
{
    Coutdev_string_vt* vtable = &od->vtable->strVT;

    if ( vtable->fontUnload!=0 )
        vtable->fontUnload(od,f);
}

/*======================================================================*/
/*= Drawing strings (GC settings)                                      =*/

extern void od_str_setGC_byStyle(Toutdev* od, Tod_GCdef gcd, Csty_text*stt)
{
    Tsty_text wstt = *stt;
    int    fi      = stt->dpt_font;
    double pts     = stt->dpt_pts;
    Ccolor*fg      = stt->dpt_color;

    double     fontratio;
    CfontInfo* fontinfo = 0;
    TfontEGS   fontEGS;
    double     zpts = pts * od_getUsrZoom(od);

    DBG1_FontE("gcd=%d fi=%d pts=%f color=%p",gcd,fi,zpts,fg);

    wstt.stt_pts = zpts;
    if ( wstt.stt_color==0 )
        wstt.stt_color = od_style_topText(od,gcd)->stt_color;

    fontinfo = od_str_fontCacheInfo(od,fi,zpts);

    if ( fontinfo==0 )
        fontinfo = od_str_fontLoadAndCache(od,fi,zpts);
    fontEGS = fontinfo->fo_egs;

    // now we have the font we update the EGS GC
    od->vtable->strVT.setGC(od,gcd,fontinfo,wstt.stt_color);

    // now we update the style stack 
    odsty_upteText( od_style_topText(od,gcd),&wstt);

    DBG1_FontL("gcd=%d fi=%d pts=%f",gcd,fi,pts);
}

extern void od_str_setGC (Toutdev* od, Tod_GCdef gcd,
                          int fid, double pts, Ccolor*c)
{
    Tsty_text style = { 0.0, fid, pts, (Tcolor*)c };
    od_str_setGC_byStyle( od, gcd, &style);
}

extern void od_str_setGCP_byStyle(Toutdev* od, Tod_GCdef gcd, Csty_text*_stt)
{
    Tsty_text* stt = (Tsty_text*)_stt;
    int    fi  = stt->dpt_font;
    double pts = stt->dpt_pts;
    if (stt->stt_color == NULL)
        stt->stt_color = od_style_topText(od, gcd)->stt_color;
    Ccolor*fg  = stt->dpt_color;

    double     fontratio;
    CfontInfo* fontinfo = 0;
    TfontEGS   fontEGS;
    double     zpts = pts;

    DBG1_FontE("gcd=%d fi=%d pts=%f color=%p",gcd,fi,zpts,fg);

    fontinfo = od_str_fontCacheInfo(od,fi,zpts);

    if ( fontinfo==0 )
        fontinfo = od_str_fontLoadAndCache(od,fi,zpts);
    fontEGS = fontinfo->fo_egs;

    // now we have the font we update the EGS GC
    od->vtable->strVT.setGC(od,gcd,fontinfo,fg);

    // now we update the style stack 
    odsty_upteText( od_style_topText(od,gcd), stt);

    DBG1_FontL("gcd=%d fi=%d pts=%f",gcd,fi,pts);
}
extern void od_str_setGCP (Toutdev* od, Tod_GCdef gcd,
                           int fid, double pts, Ccolor*c)
{
    Tsty_text style = { 0.0, fid, pts, (Tcolor*)c };
    od_str_setGCP_byStyle( od, gcd, &style);
}

/*======================================================================*/
/*= Drawing strings (extents)                                          =*/

extern int od_str_extents_DesUnit(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy)
{
    int ret;
    Coutdev_string_vt* vtable  = &od->vtable->strVT;
    TfontEGS           fegs    = od_str_fontCacheEGS(od,fi,pts);
    double             pts2pix = od->ppiDes / LUNIT_PtPerInch;
    if ( vtable->extentsPix!=0 ) {
        vtable->extentsPix(od,fegs,text,ascent,descent,dx,dy);
        ret = 0;
    } else if ( vtable->extentsPts!=0 ) {
        double  vascent, vdescent, vdx, vdy;
        double* pascent  = ascent==0  ? 0 : &vascent;
        double* pdescent = descent==0 ? 0 : &vdescent;
        double* pdx      = dx==0      ? 0 : &vdx;
        double* pdy      = dy==0      ? 0 : &vdy;
        vtable->extentsPts(od,fegs,text,pascent,pdescent,pdx,pdy);
        if (ascent ) *ascent  = vascent  * pts2pix ;
        if (descent) *descent = vdescent * pts2pix ;
        if (dx     ) *dx      = vdx      * pts2pix ;
        if (dy     ) *dy      = vdy      * pts2pix ;
        ret = 0;
    } else {
        TfontDef* fd = odres_getFontUsed(od->resources,fi);
        if (ascent ) *ascent  = pts * pts2pix * fd->geom.asc;
        if (descent) *descent = pts * pts2pix * (1+fd->geom.ils-fd->geom.asc); //0.25;
        if (dx     ) *dx      = pts * pts2pix * fd->geom.adx * strlen(text);
        if (dy     ) *dy      = pts * pts2pix ;
        ret = 1;
    }
    return ret;
}

extern int od_str_extents_DesUnitI(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        int* ascent, int* descent,
                        int* dx,     int* dy)
{
    double  vascent, vdescent, vdx, vdy;

    int ret = od_str_extents_DesUnit(od,fi,pts,text,&vascent,&vdescent,&vdx,&vdy);
    if (ascent ) *ascent  = round( vascent );
    if (descent) *descent = round( vdescent );
    if (dx     ) *dx      = round( vdx );
    if (dy     ) *dy      = round( vdy );
    return ret;
}

extern int od_str_extents_SrcUnit(Toutdev* od, int fi, double pts,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy)
{
    int ret;
    Coutdev_string_vt* vtable = &od->vtable->strVT;
    TfontEGS           fegs   = od_str_fontCacheEGS(od,fi,pts);

    if ( vtable->extentsPts!=0 ) {
        vtable->extentsPts(od,fegs,text,ascent,descent,dx,dy);
        if (ascent ) *ascent  *= OD_ppiSrc(od) / LUNIT_PtPerInch;
        if (descent) *descent *= OD_ppiSrc(od) / LUNIT_PtPerInch;
        if (dx     ) *dx      *= OD_ppiSrc(od) / LUNIT_PtPerInch;
        if (dy     ) *dy      *= OD_ppiSrc(od) / LUNIT_PtPerInch;
        ret = 0;
    } else {
        double  vascent, vdescent, vdx, vdy;
        double* pascent  = ascent==0  ? 0 : &vascent;
        double* pdescent = descent==0 ? 0 : &vdescent;
        double* pdx      = dx==0      ? 0 : &vdx;
        double* pdy      = dy==0      ? 0 : &vdy;
        
        ret = od_str_extents_DesUnit(od,fi, pts, text,pascent,pdescent,pdx,pdy);
        double pix2src = 1.0/od->desPerSrc;
        if (ascent ) *ascent  = vascent  * pix2src;
        if (descent) *descent = vdescent * pix2src;
        if (dx     ) *dx      = vdx      * pix2src;
        if (dy     ) *dy      = vdy      * pix2src;
    }
        
    return ret;
}

extern int od_str_extents_DesUnit_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy)
{
    Tsty_text* s = od_style_topText(od,gcd);
    return od_str_extents_DesUnit(od,s->stt_font,s->stt_pts, text,
            ascent,descent,dx,dy);
}
extern int od_str_extents_SrcUnit_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text,
                        double* ascent, double* descent,
                        double* dx,     double* dy)
{
    Tsty_text* s = od_style_topText(od,gcd);
    return od_str_extents_SrcUnit(od,s->stt_font,s->stt_pts, text,
            ascent,descent,dx,dy);
}

extern Todstrgeo od_str_extents_gc(Toutdev* od, Tod_GCdef gcd,
                        CstrBrief text, int sens, double x, double y)
{
    Tsty_text* sty = od_style_topText(od,gcd); 
    int        font = sty->stt_font;
    double     pts  = sty->stt_pts;
    double     zoom = od_getUsrZoom(od);
    Todstrgeo ret;
    ret.pos.x = x;
    ret.pos.y = y;
    od_str_extents_SrcUnit(od, font, pts, text, &ret.ascent, NULL, &ret.dx, &ret.dy);
    ret.ascent /= zoom; ret.dx /= zoom; ret.dy /= zoom;
    ret.nl = 0;
    ret.of = sens;
    ret.font = font;
    ret.pts  = pts/od_getUsrZoom(od);
    return ret;
}

/*======================================================================*/
/*= Drawing strings                                                    =*/

#define DRAW_FMT "gcd=%d x=%e y=%e sens=%d a=%e : str=%s"
#define DRAW_ARG gcd,g->pos.x,g->pos.y,g->of,a,text

extern void od_str_drawXXX(Toutdev*  od,   Tod_GCdef  gcd, Codstrgeo*g,
                           CstrBrief text, double a, int flag)
{
    DBG1_FontE(DRAW_FMT,DRAW_ARG);
    CfontInfo* fontinfo = od_str_fontCacheInfo(od,g->font,g->pts);
    //double zoom    = od_getUsrZoom(od);
    double dx      = g->dx;
    double dy      = g->dy;
    double ascent  = g->ascent;
    double descent = dy-ascent;
    double ratio   = fontinfo->fo_ratio;
    //double dy_pix  = CONV_D_S2D( od, dy );

    // compute the flag
    if ( flag==0 ) {
        flag = OD_STRDRAW_Xxx;
    }

    // get the NW (p1) and SE (p2) points of the unrotated
    // text bouding box.
    // get the rotation center (pc).
    Tdble2 p1,p2; // NW and SE point of the text box.
    Tdble2 pc;    // rotation center (used if a!=0)
    switch (g->of) {
        default:
            PERM_spe_printf("invalid %d of flag, assuming LEFT (%d)",
                            g->of,OD_SORG_LEFT);
        case OD_SORG_LEFT: // x,y is the West hand of base line
            p1.x = g->pos.x;
            p1.y = g->pos.y - ascent;
            p2.x = g->pos.x + dx;
            p2.y = g->pos.y - ascent + dy;
            pc.x = g->pos.x; pc.y = g->pos.y;
            break;

        case OD_SORG_CENTER:  // x,y is the middle of base line
            p1.x = g->pos.x - dx/2.;
            p1.y = g->pos.y - ascent;
            p2.x = g->pos.x + dx/2.;
            p2.y = g->pos.y - ascent + dy;
            pc.x = g->pos.x - dx/2; pc.y = g->pos.y;
            break;

        case OD_SORG_RIGHT:  // x,y is the east hand of base line
            p1.x = g->pos.x - dx;
            p1.y = g->pos.y - ascent;
            p2.x = g->pos.x ;
            p2.y = g->pos.y - ascent + dy;
            pc.x = g->pos.x - dx; pc.y = g->pos.y;
            break;
    }

#define INKCOEF_H 0.8  // horizontal ink factor
#define INKCOEF_V 0.8  // vertical ink factor

    // init tab with the points of the bounding box
    //    tab[0]--------------tab[1]
    //      |                   |
    //    tab[2]--------------tab[3]
    // init tabi with the points of the inked bounding box
    Tdble2 tab[5],tabi[5];
    tab[0].x = p1.x;  tab[0].y = p1.y;
    tab[1].x = p2.x;  tab[1].y = p1.y;  
    tab[2].x = p2.x;  tab[2].y = p2.y;
    tab[3].x = p1.x;  tab[3].y = p2.y;  
    tab[4]   = tab[0];
    tabi[0] = tab[0]; tabi[0].y += (1-INKCOEF_V) * tabi[0].y;
    tabi[1] = tab[1]; tabi[1].y += (1-INKCOEF_V) * tabi[1].y;
    tabi[2] = tab[2]; tabi[2].y += (1-INKCOEF_V) * tabi[2].y;
    tabi[2] = tab[3]; tabi[3].y += (1-INKCOEF_V) * tabi[3].y;
    tabi[4] = tabi[0];

    // do the drawing specified in flag
    if ( (a==0 && (flag&OD_STRDRAW_Rect)!=0) || (flag&OD_STRDRAW_Rect0)!= 0 ) {
        od_draw_mlineT(od,gcd,tab,5);
        //od_draw_lineXY(od,gcd,pc.x-500,pc.y,pc.x+dx+100,pc.y);
    }

    if ( (a==0 && (flag&OD_STRDRAW_RectInk)!=0) || (flag&OD_STRDRAW_RectInk0)!= 0 ) {
        od_draw_mlineT(od,gcd,tabi,5);
    }

    // get the / and \ lines of the X if needed into xs set
    int i;
    int    n   = cs_char_length(text);
    Tdble2Set* xs = dble2set_new(n*2);
    if ( (flag&(OD_STRDRAW_Xxx|OD_STRDRAW_Xxx0))!=0 ) {
        double cdx = dx     / n;
        double idx = cdx    * INKCOEF_H; // inked width  ( the X width ) 
        double idy = ascent * INKCOEF_V; // inked height ( the X height )
        double cx  = pc.x;   // (cx,cy) is the NW point of the X 
        double cy  = pc.y - idy ;
        for (i=0 ; i<n ; i++) {
            // the \ of the X
            dble2set_addXY(xs, cx,    cy     );
            dble2set_addXY(xs, cx+idx,cy+idy );
            // the / of the X
            dble2set_addXY(xs, cx    ,cy+idy );
            dble2set_addXY(xs, cx+idx,cy     );
            cx += cdx;
        }
    }

    if ( (a==0 && (flag&OD_STRDRAW_Xxx)!=0) || (flag&OD_STRDRAW_Xxx0)!= 0 ) {
        od_draw_lineS(od,gcd,xs);
    }

    if ( a!=0 ) {
        ROTCC_PAXY(tab [0],-a,pc.x,pc.y);
        ROTCC_PAXY(tab [1],-a,pc.x,pc.y);
        ROTCC_PAXY(tab [2],-a,pc.x,pc.y);
        ROTCC_PAXY(tab [3],-a,pc.x,pc.y);
        ROTCC_PAXY(tab [4],-a,pc.x,pc.y);
        ROTCC_PAXY(tabi[0],-a,pc.x,pc.y);
        ROTCC_PAXY(tabi[1],-a,pc.x,pc.y);
        ROTCC_PAXY(tabi[2],-a,pc.x,pc.y);
        ROTCC_PAXY(tabi[3],-a,pc.x,pc.y);
        ROTCC_PAXY(tabi[4],-a,pc.x,pc.y);

        // rotated bounding box
        if ( (flag&OD_STRDRAW_Rect)!=0 ) {
            od_draw_mlineT(od,gcd,tab,5);
        }
        if ( (flag&OD_STRDRAW_RectInk)!=0 ) {
            od_draw_mlineT(od,gcd,tabi,5);
        }

        // rotate the / and \ of the X if needed
        if ( (flag&OD_STRDRAW_Xxx)!=0 ) {
            for (i=0 ; i<xs->eleNb ; i++) {
                ROTCC_PAXY(xs->eles[i],-a,pc.x,pc.y);
            }
            od_draw_lineS(od,gcd,xs);
        }
    }
    dble2set_delete( xs );

    if ( (flag&(OD_STRDRAW_BaseLine))!=0 ) {
        Tdble2 pa=p1, pb=p2;
        pa.x -= CONV_D_D2S( od,15 );
        pa.y += ascent;
        pb.x += CONV_D_D2S( od,15 );
        pb.y  = pa.y;
        if ( a!=0 ) {
            ROTCC_PAXY(pa,-a,pc.x,pc.y);
            ROTCC_PAXY(pb,-a,pc.x,pc.y);
        }
        od_draw_linePT(od,gcd,&pa,&pb);
    }

    DBG1_FontL(DRAW_FMT,DRAW_ARG);
}

static Tdble2 od_draw_getOriginNW(Toutdev*od, Codstrgeo*g)
{
    Tdble2 origin = { g->pos.x, g->pos.y - g->ascent };
    switch ( g->of ) {
        default:
            PERM_spe_printf("invalid %d of flag, assuming LEFT (%d)",
                            g->of,OD_SORG_LEFT);
        case OD_SORG_LEFT:   break;
        case OD_SORG_CENTER: origin.x -= g->dx/2.; break;
        case OD_SORG_RIGHT:  origin.x -= g->dx;    break;
    }
    origin.x = CONV_X_S2D(od,origin.x);
    origin.y = CONV_Y_S2D(od,origin.y);
    return origin;
}

static Tdble2 od_draw_getOriginNW_r(Toutdev*od, Codstrgeo*g, double a)
{
    Tdble2 center = g->pos;
    Tdble2 origin = { g->pos.x, g->pos.y - g->ascent };
    switch ( g->of ) {
        default:
            PERM_spe_printf("invalid %d of flag, assuming LEFT (%d)",
                            g->of,OD_SORG_LEFT);
        case OD_SORG_LEFT:   ROTCC_PAXY(origin,-a,center.x,center.y);
                             break;
        case OD_SORG_CENTER: origin.x -= g->dx/2.;
                             ROTCC_PAXY(origin,-a,center.x,center.y);
                             break;
        case OD_SORG_RIGHT:  origin.x -= g->dx;
                             ROTCC_PAXY(origin,-a,center.x,center.y);
                             break;
    }
    origin.x = CONV_X_S2D(od,origin.x);
    origin.y = CONV_Y_S2D(od,origin.y);
    return origin;
}

static Tdble2 od_draw_getOriginWL(Toutdev*od, Codstrgeo*g)
{
    Tdble2 origin = g->pos;
    switch ( g->of ) {
        default:
            PERM_spe_printf("invalid %d of flag, assuming LEFT (%d)",
                            g->of,OD_SORG_LEFT);
        case OD_SORG_LEFT:   break;
        case OD_SORG_CENTER: origin.x -= g->dx/2.; break;
        case OD_SORG_RIGHT:  origin.x -= g->dx;    break;
    }
    origin.x = CONV_X_S2D(od,origin.x);
    origin.y = CONV_Y_S2D(od,origin.y);
    return origin;
}

static Tdble2 od_draw_getOriginWL_r(Toutdev*od, Codstrgeo*g, double a)
{
    Tdble2 center = g->pos;
    Tdble2 origin = g->pos;
    switch ( g->of ) {
        default:
            PERM_spe_printf("invalid %d of flag, assuming LEFT (%d)",
                            g->of,OD_SORG_LEFT);
        case OD_SORG_LEFT:   break;
        case OD_SORG_CENTER: origin.x -= g->dx/2.;
                             ROTCC_PAXY(origin,-a,center.x,center.y);
                             break;
        case OD_SORG_RIGHT:  origin.x -= g->dx;
                             ROTCC_PAXY(origin,-a,center.x,center.y);
                             break;
    }
    origin.x = CONV_X_S2D(od,origin.x);
    origin.y = CONV_Y_S2D(od,origin.y);
    return origin;
}

#include "tools/lenunit.h"
extern void od_str_draw(Toutdev*  od,   Tod_GCdef gcd,
                        Codstrgeo*g,    CstrBrief text, double a)
{
    DBG1_FontE(DRAW_FMT,DRAW_ARG);

    CfontInfo* fontinfo = od_str_fontCacheInfo(od, g->font, g->pts);
    Coutdev_string_vt* vtable = &od->vtable->strVT;

    // special cases
    double ratio    = fontinfo->fo_ratio;
    double tdy_src  = g->dy / ratio;              // text height in src unit
    double tdy_pix  = CONV_D_S2D( od, tdy_src );  // text height in des unit
    double tdy_fu = lu_convToFuPpi(tdy_pix,od->ppiDes);
    if ( tdy_fu < LUNIT_PtToFu(5) ) {
        // text is unreadable --> draw a line
        DBG1_FontM(DRAW_FMT "CALL drawXXX (pix height=%f<5)",DRAW_ARG,tdy_pix);
        od_str_drawXXX(od,gcd,g,text,a,0);
        goto end;
    }
    if ( ratio<1 ) {
        // drawing will be larger than expected
        DBG1_FontM(DRAW_FMT "CALL drawXXX (ratio=%f<1)",DRAW_ARG,ratio);
        od_str_drawXXX(od,gcd,g,text,a,0);
        goto end;
    }

    // drawing
    Tdble2 origin;
    void (*draw)    (void*, Tod_GCdef, double, double, CstrBrief);
    void (*draw_rot)(void*, Tod_GCdef, double, double, CstrBrief, double a);
    if ( a==0 ) {
        if (  (draw=vtable->drawWL_Pix)!=0 ) {
            origin = od_draw_getOriginWL(od,g);
            draw(od,gcd,origin.x,origin.y,text);
        } else if ( (draw=vtable->drawNW_Pix)!=0 ) {
            origin = od_draw_getOriginNW(od,g);
            draw(od,gcd,origin.x,origin.y,text);
        } else if (  (draw_rot=vtable->drawWLr_Pix)!=0 ) {
            origin = od_draw_getOriginWL_r(od,g, 0.0);
            draw_rot(od,gcd,origin.x,origin.y,text,0.0);
        } else if (  (draw_rot=vtable->drawNWr_Pix)!=0 ) {
            origin = od_draw_getOriginNW_r(od,g, 0.0);
            draw_rot(od,gcd,origin.x,origin.y,text,a);
        } else {
            DBG1_FontM(DRAW_FMT "CALL drawXXX (a=%f no driver)",DRAW_ARG,a);
            od_str_drawXXX(od,gcd,g,text,a,0);
        }
    } else {
        if (  (draw_rot=vtable->drawWLr_Pix)!=0 ) {
            origin = od_draw_getOriginWL_r(od,g, a);
            draw_rot(od,gcd,origin.x,origin.y,text,a);
        } else if (  (draw_rot=vtable->drawNWr_Pix)!=0 ) {
            origin = od_draw_getOriginNW_r(od,g, a);
            draw_rot(od,gcd,origin.x,origin.y,text,a);
        } else {
            DBG1_FontM(DRAW_FMT "CALL drawXXX (a=%f no driver)",DRAW_ARG,a);
            od_str_drawXXX(od,gcd,g,text,a,0);
        }
    }

//  od_str_drawXXX(od,gcd,g,text,a,OD_STRDRAW_Rect|OD_STRDRAW_BaseLine); // debug
end:
    DBG1_FontL(DRAW_FMT,DRAW_ARG);
    return;
}

/*======================================================================*/
/*= drawing strings (quick)                                            =*/

extern void od_str_draw_LR(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y)
{
    if ( text==0 ) return;
    Todstrgeo g = od_str_extents_gc(od,gcd,text,OD_SORG_LEFT,x,y);
    od_str_draw(od,gcd,&g,text,0.0);
}

extern void od_str_draw_RL(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y)
{
    if ( text==0 ) return;
    Todstrgeo g = od_str_extents_gc(od,gcd,text,OD_SORG_RIGHT,x,y);
    od_str_draw(od,gcd,&g,text,0.0);
}

extern void od_str_draw_CE(Toutdev*  od, Tod_GCdef  gcd, CstrBrief text, 
                           double x, double y)
{
    if ( text==0 ) return;
    Todstrgeo g = od_str_extents_gc(od,gcd,text,OD_SORG_CENTER,x,y);
    od_str_draw(od,gcd,&g,text,0.0);
}

extern void od_str_draw_IBxy(Toutdev*  od, Tod_GCdef  gcd,
                             CstrBrief text, TdirWR w,
                             double x, double y, double dx, double dy)
{
    Todstrgeo g = od_str_extents_gc(od,gcd,text,OD_SORG_LEFT,0,0);
    od_str_draw_IBxyg(od,gcd,&g,text,w,x,y,dx,dy);
}

extern void od_str_draw_IBxyg(Toutdev*  od, Tod_GCdef gcd, Todstrgeo*g,
                              CstrBrief text, TdirWR w,
                              double x, double y, double dx, double dy)
{
    int sh = CONV_D_D2S(od,3); // shift 3 pixels from box edges
    switch ( w ) {
      case DirWR_NN: x = x + (dx-g->dx)/2      ; y = y                + sh ; break;
      case DirWR_NW: x = x               + sh  ; y = y                + sh ; break;
      case DirWR_NE: x = x +  dx-g->dx    - sh ; y = y                + sh ; break;
      case DirWR_SS: x = x + (dx-g->dx)/2      ; y = y +  dy-g->dy    - sh ; break;
      case DirWR_SW: x = x               + sh  ; y = y +  dy-g->dy    - sh ; break;
      case DirWR_SE: x = x +  dx-g->dx    - sh ; y = y +  dy-g->dy    - sh ; break;
      case DirWR_WW: x = x               + sh  ; y = y + (dy-g->dy)/2      ; break;
      case DirWR_EE: x = x +  dx-g->dx    - sh ; y = y + (dy-g->dy)/2      ; break;
      case DirWR_CC: x = x + (dx-g->dx)/2      ; y = y + (dy-g->dy)/2      ; break;
    }
    g->pos = dble2_init(x,y+g->ascent);
    g->of  = OD_SORG_LEFT;
    od_str_draw(od,gcd,g,text,0.0);
}

extern void od_str_draw_IBox (Toutdev*  od, Tod_GCdef  gcd,
                              CstrBrief text, TdirWR w, CboxDble* box);
extern void od_str_draw_IBoxg(Toutdev*  od, Tod_GCdef gcd, Todstrgeo*g,
                              CstrBrief text, TdirWR w, CboxDble* box);
/*======================================================================*/
