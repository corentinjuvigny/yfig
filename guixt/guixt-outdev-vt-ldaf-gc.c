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
 *$Software: YFIG-XT (VGUI driver to Xt and Xawd or Xaw3d)
 *      $Id: guixt-outdev-vt-ldaf-gc.c 111 2018-01-07 18:20:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-outdev-vt-ldaf-gc.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= output device implementation (GC setting for ldaf)                 =*/
/*=                                                                    =*/
/*= This file is included by guixt-outdevice-vt.c.                     =*/
/*======================================================================*/

/*======================================================================*/

static void odvt_setGCso(void*dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TjoinStyle js,
                TcapStyle  cs, TcolorEGS fg)
{
    STD_VARS;
    XGCValues       gcv;
    unsigned long   mask=0;

    if ( th>=0 ) {
        gcv.line_width = round( th );
        mask |= GCLineWidth;
    }
    if ( ls!=DA_LS_None ) {
        mask |= GCLineStyle;
        gcv.line_style = conv_lineStyle_Od2X11(ls);
    }
    if ( js!=DA_JS_None ) {
        mask |= GCJoinStyle;
        gcv.join_style = conv_joinStyle_Od2X11(js);
    }
    if ( cs!=DA_CS_None ) {
        mask |= GCCapStyle;
        gcv.cap_style = conv_capStyle_Od2X11(cs);
    }
    if ( fg.ulong!=EGSCOLOR_NONE ) {
        gcv.foreground = fg.ulong;
        mask |= GCForeground;
    }
#if 0
printf("%s: gcd=%d gc=%ld mask=%ld w=%d:%d ls=%d:%d js=%d:%d cs=%d:%d fg=%d:%ld\n",__func__,
        gcd, E_GCcache(gcd), mask,
        (GCLineWidth  &mask)!=0, gcv.line_width,
        (GCLineStyle  &mask)!=0, gcv.line_style,
        (GCJoinStyle  &mask)!=0, gcv.join_style,
        (GCCapStyle   &mask)!=0, gcv.cap_style,
        (GCForeground &mask)!=0, gcv.foreground);
#endif

    if ( mask!=0 )
      XChangeGC(E_Display,E_GCcache(gcd),mask,&gcv);
}

static void odvt_setGCdd(void*dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TcolorEGS ddh)
{
    STD_VARS;
    unsigned long   mask=0;
    XGCValues       gcv;
    char            ddt[10];
    int             ddn=0;
    int i;

    if ( ls!=DA_LS_None ) {
        mask |= GCLineStyle;
        gcv.line_style = conv_lineStyle_Od2X11(ls);
        switch (ls) {
          case DA_LS_Dot :
          case DA_LS_Dash1Dot :
          case DA_LS_Dash2Dot :
          case DA_LS_Dash3Dot :
            //mask |= GCJoinStyle; gcv.join_style = JoinRound;
            mask |= GCCapStyle; gcv.cap_style = CapRound;
        }

static double dot[2]      = { 0., 1. };
static double dash[2]     = { 1., 1. };
static double dash1dot[4] = { 1., 0.5, 0., 0.5 };
static double dash2dot[6] = { 1., 0.45, 0., 0.333, 0., 0.45 };
static double dash3dot[8] = { 1., 0.4, 0., 0.3, 0., 0.3, 0., 0.4 };

#define INIT_DDF(t) ddf = t; ddn = TOOL_TABLE_NbEle(t)
#define DDLength(v,l0) round( v==0 ? l0 : v<1 ? 1 : v>255 ? 255 : v )
        double lenf  = 4 * (th + 1.) / 2.; // get from xfig
        double lenf0 = 1;
        double *ddf;
        switch (ls) {
          case DA_LS_Solid    : ddf=0; ddn=0; break;
          case DA_LS_Dot      : INIT_DDF(dot);   break;
          case DA_LS_Dash     : INIT_DDF(dash);  break;
          case DA_LS_Dash1Dot : INIT_DDF(dash1dot); break;
          case DA_LS_Dash2Dot : INIT_DDF(dash2dot); break;
          case DA_LS_Dash3Dot : INIT_DDF(dash3dot); break;
          default :
             PERM_spe_printf(
                 "%d (%s) is an unexpected style (changed to Solid)",
                 ls,DBG_LS2str(ls));
             ls=DA_LS_Solid;
             ddf=0; ddn=0;
        }
        for ( i=0 ; i<ddn ; i++ ) {
            double v = lenf*ddf[i];
            if (i&1) v += th*1.2;
            ddt[i] = DDLength(v,lenf0);
        }
    }

    if ( mask!=0 )
        XChangeGC(E_Display,E_GCcache(gcd),mask,&gcv);
    if ( ddn>0 ) 
        XSetDashes(E_Display,E_GCcache(gcd),0,ddt,ddn);
}

static void odvt_setGCfillColor(void*dev, Tod_GCdef gcd, TcolorEGS c)
{
    STD_VARS;
    XGCValues       gcv;
    unsigned long   mask=0;

    gcv.fill_style = FillSolid;
    mask |= GCFillStyle;
    
    if ( c.ulong!=EGSCOLOR_NONE ) {
        gcv.foreground = c.ulong;
        mask |= GCForeground;
    }
    if ( mask!=0 )
      XChangeGC(E_Display,E_GCcache(gcd),mask,&gcv);
}

//static void odvt_setGCfillColorBrightness(
//                                void*dev, Tod_GCdef gcd, TcolorEGS c,
//                                double coef);

static void odvt_setGCfillTile (void*dev, Tod_GCdef gcd, TpixmapEGS pxm)
{
    STD_VARS;
    XGCValues       gcv;
    unsigned long   mask=0;

    gcv.fill_style = FillTiled;
    mask |= GCFillStyle;
    
    gcv.tile = pxm.ulong;
    mask |= GCTile;

    XChangeGC(E_Display,E_GCcache(gcd),mask,&gcv);
}

//static void odvt_setGCfillTileZ(void*dev, Tod_GCdef gcd, TpixmapEGS _pxm,
//                                double zf);

/*======================================================================*/
