/*======================================================================
 *  Copyright 2018 Corentin Juvigny
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
 *$Software: YFIG-GTK (VGUI driver to Xt and Xawd or Xaw3d)
 *      $Id: guigtk-outdev-vt-ldaf-gc.c 111 2018-01-07 18:20:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guigtk/guigtk-outdev-vt-ldaf-gc.c $
 *  $Author: Corentin Juvigny (Email: corentin.juvigny@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= output device implementation (GC setting for ldaf)                 =*/
/*=                                                                    =*/
/*= This file is included by guigtk-outdevice-vt.c.                     =*/
/*======================================================================*/

/*======================================================================*/

static void odvt_setGCso(void* dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TjoinStyle js,
                TcapStyle  cs, TcolorEGS fg)
{
#if 0
    STD_VARS;

    if ( th >= 0 ) cairo_set_line_width(gc, th);
    if ( ls != DA_LS_None ) cairo_set_dash(gc, NULL, 0, 0); //TODO à faire
    if ( js != DA_JS_None ) cairo_set_line_join(gc, conv_joinStyle_Od2Gtk(js));
    if ( cs != DA_CS_None ) cairo_set_line_cap(gc, conv_capStyle_OdGtk(cs));
    if ( !COLOREGS_IsNone(fg)  ) {
    GdkRGBA* clr = GtkCOLOR(fg); 
    cairo_set_source_rgb(gc, clr->red, clr->green, clr->blue); 
    }
#endif
}

static void odvt_setGCdd(void* dev, Tod_GCdef gcd,
                double th, TlineStyle ls, TcolorEGS ddh)
{
#if 0
    STD_VARS;
    double dash[100];
    Toutdev* od = dev;


    if ( ls == DA_LS_None )
        return; 
    if ( ls == DA_LS_Solid ) {
    cairo_set_dash(gc, NULL, 0, 0);
    return;
    }
    switch (ls) {
      case DA_LS_Dot :
          dash[0] = od->resources->dot[0];
          dash[1] = od->resources->dot[1];
          cairo_set_dash(gc, dash, 2, 0);
          break;
      case DA_LS_Dash :
          dash[0] = od->resources->dash[0];
          dash[1] = od->resources->dash[1];
          cairo_set_dash(gc, dash, 2, 0);
          break;
      case DA_LS_Dash1Dot :
dash1_dot1:
          dash[0] = od->resources->dash1dot1[0];
          dash[1] = od->resources->dash1dot1[1];
          dash[2] = od->resources->dash1dot1[2];
          dash[3] = od->resources->dash1dot1[3]; 
          cairo_set_dash(gc, dash, 4, 0);
          break;
      case DA_LS_Dash2Dot :
        tool_errerr_printf(od->msgdrv, "%s", "Style 1 Dashs 2 Dots not implemented, use 1 dash 1 dot instead");
        goto dash1_dot1;
      case DA_LS_Dash3Dot :
        tool_errerr_printf(od->msgdrv, "%s", "Style 1 Dashs 3 Dots not implemented, use 1 dash 1 dot instead");
        goto dash1_dot1;    
      default :
        PERM_spe_printf("unexpected line style %d",ls);
        goto dash1_dot1;
    }
#endif
}

static void odvt_setGCfillColor(void*dev, Tod_GCdef gcd, TcolorEGS c)
{
#if 0
    STD_VARS;

    
    if ( !COLOREGS_IsNone(c) ) {
        GdkRGBA* clr = GtkCOLOR(c); 
        cairo_set_source_rgb(gc, clr->red, clr->green, clr->blue); 
    }
#endif
}

//static void odvt_setGCfillColorBrightness(
//                                void*dev, Tod_GCdef gcd, TcolorEGS c,
//                                double coef);

static void odvt_setGCfillTile (void*dev, Tod_GCdef gcd, TpixmapEGS pxm)
{
    STD_VARS;
    if (PXMegs2gtk(E_PXM(gcd)) != NULL)  g_object_unref((PXMegs2gtk(E_PXM(gcd))));
    E_PXM(gcd) = PXMgtk2egs(gdk_pixbuf_copy(PXMegs2gtk(pxm)));
    //gdk_cairo_set_source_pixbuf(gc, pxb, 0, 0);
}

//static void odvt_setGCfillTileZ(void*dev, Tod_GCdef gcd, TpixmapEGS _pxm,
//                                double zf);

/*======================================================================*/
