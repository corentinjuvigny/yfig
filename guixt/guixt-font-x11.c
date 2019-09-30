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
 *      $Id: guixt-font-x11.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-font-x11.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for handling fonts and drawing strings (EGS driver       =*/
/*= based on X11.                                                      =*/
/*=                                                                    =*/
/*= See the vgui/vgui-font.h header for the type description and       =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-fonts.h"
#include "vgui/vgui-device.h"

CfontDef fontTable[FONT_NB] = {
// 0
    {"times"                   , "medium regular"        , "-*-times-medium-r-normal--%d-%s-*-*-*-*-%s-*",          },
    {"times"                   , "medium italic"         , "-*-times-medium-i-normal--%d-%s-*-*-*-*-%s-*",          },
    {"times"                   , "regular bold"          , "-*-times-bold-r-normal--%d-%s-*-*-*-*-%s-*",            },
    {"times"                   , "italic bold"           , "-*-times-bold-i-normal--%d-%s-*-*-*-*-%s-*",            },
    {"avantgarde"              , "book regular"          , "-*-avantgarde-book-r-normal--%d-%s-*-*-*-*-%s-*",       },
    {"avantgarde"              , "book oblique"          , "-*-avantgarde-book-o-normal--%d-%s-*-*-*-*-%s-*",       },
    {"avantgarde"              , "demi regular"          , "-*-avantgarde-demi-r-normal--%d-%s-*-*-*-*-%s-*",       },
    {"avantgarde"              , "demi oblique"          , "-*-avantgarde-demi-o-normal--%d-%s-*-*-*-*-%s-*",       },
    {"bookman"                 , "light regular"         , "-*-bookman-light-r-normal--%d-%s-*-*-*-*-%s-*",         },
    {"bookman"                 , "light italic"          , "-*-bookman-light-i-normal--%d-%s-*-*-*-*-%s-*",         },
// 10                                                   
    {"bookman"                 , "demi regular"          , "-*-bookman-demi-r-normal--%d-%s-*-*-*-*-%s-*",          },
    {"bookman"                 , "demi italic"           , "-*-bookman-demi-i-normal--%d-%s-*-*-*-*-%s-*",          },
    {"courier"                 , "medium regular"        , "-*-courier-medium-r-normal--%d-%s-*-*-*-*-%s-*",        },
    {"courier"                 , "medium oblique"        , "-*-courier-medium-o-normal--%d-%s-*-*-*-*-%s-*",        },
    {"courier"                 , "bold regular"          , "-*-courier-bold-r-normal--%d-%s-*-*-*-*-%s-*",          },
    {"courier"                 , "bold oblique"          , "-*-courier-bold-o-normal--%d-%s-*-*-*-*-%s-*",          },
    {"helvetica"               , "medium regular"        , "-*-helvetica-medium-r-normal--%d-%s-*-*-*-*-%s-*",      },
    {"helvetica"               , "medium oblique"        , "-*-helvetica-medium-o-normal--%d-%s-*-*-*-*-%s-*",      },
    {"helvetica"               , "bold regular"          , "-*-helvetica-bold-r-normal--%d-%s-*-*-*-*-%s-*",        },
    {"helvetica"               , "bold oblique"          , "-*-helvetica-bold-o-normal--%d-%s-*-*-*-*-%s-*",        },
// 20                          
    {"helvetica"               , "medium regular narrow" , "-*-helvetica-medium-r-narrow--%d-%s-*-*-*-*-%s-*",      },
    {"helvetica"               , "medium oblique narrow" , "-*-helvetica-medium-o-narrow--%d-%s-*-*-*-*-%s-*",      },
    {"helvetica"               , "bold regular narrow"   , "-*-helvetica-bold-r-narrow--%d-%s-*-*-*-*-%s-*",        },
    {"helvetica"               , "bold oblique narrow"   , "-*-helvetica-bold-o-narrow--%d-%s-*-*-*-*-%s-*",        },
    {"new century schoolbook"  , "medium regular"        , "-*-new century schoolbook-medium-r-normal--%d-%s-*-*-*-*-%s-*", },
    {"new century schoolbook"  , "medium italic"         , "-*-new century schoolbook-medium-i-normal--%d-%s-*-*-*-*-%s-*", },
    {"new century schoolbook"  , "bold regular"          , "-*-new century schoolbook-bold-r-normal--%d-%s-*-*-*-*-%s-*",   },
    {"new century schoolbook"  , "bold italic"           , "-*-new century schoolbook-bold-i-normal--%d-%s-*-*-*-*-%s-*",   },
    {"palatino"                , "medium regular"        , "-*-palatino-medium-r-normal--%d-%s-*-*-*-*-%s-*",               },
    {"palatino"                , "medium italic"         , "-*-palatino-medium-i-normal--%d-%s-*-*-*-*-%s-*",               },
// 30
    {"palatino"                , "bold regular"          , "-*-palatino-bold-r-normal--%d-%s-*-*-*-*-%s-*",                 },
    {"palatino"                , "bold italic"           , "-*-palatino-bold-i-normal--%d-%s-*-*-*-*-%s-*",                 },
    {"symbol (greek)"          , "medium regular"        , "-*-symbol-medium-r-normal--%d-%s-*-*-*-*-*-*",                  },
    {"itc zapf chancery"       , "medium italic"         , "-*-itc zapf chancery-medium-i-normal--%d-%s-*-*-*-*-*-*",       },
    {"itc zapf dingbats"       , ""                      , "-*-itc zapf dingbats-*-*-*--%d-%s-*-*-*-*-*-*",                 },
};

/*======================================================================*/
/*= font handler                                                       =*/

extern Tegs_font    gui_font_load   (Tvgui* gui, int fi, double pts)
{
    double ppiDev  = VG_ppiScreen(gui);
    CfontDef* fd = fontTable+fi;
    DBG1_Font("look for fi=%d pts=%f (%f) basename=%s",fi,
            pts, pts*ppiDev/72., fd->family);

    pts =  pts*ppiDev/72.; // adapt pts to screen ppi.

    char fname[300];
    int ipts = round(pts);
    sprintf(fname,fd->format,ipts,"*","ISO8859");
    Font fegs = XLoadFont(E_Display,fname);
    if ( fegs!=0 ) {
        DBG0_Font("FOUND: %s",fname);
        return (Tegs_font)fegs;
    } else {
        DBG0_Font("NOT found: %s",fname);
        return (Tegs_font)0;
    }
}

extern void gui_font_unload (Tvgui* gui, Tegs_font f)
{
    Font fegs = (Font)f;
    XUnloadFont(E_Display,fegs);
}

extern Tegs_font    gui_font_default(Tvgui* gui, double pts)
{
    PERM_spf_NYI(0);
}

/*======================================================================*/
/*= Drawing horizontal strings                                         =*/

extern void gui_font_setGC (Tvgui* gui, Tvgui_GCdef gcd,
                        Tegs_font f, Ccolor* bg, Ccolor* fg)
{
#define EGSFONT_NONE 0
    XGCValues       gcv;
    unsigned long   mask=0;
    if ( f!=EGSFONT_NONE ) {
        gcv.font = (Font)f;
        mask |= GCFont;
    }
    if ( fg!=0 ) {
        gcv.foreground = fg->gcolor;
        mask |= GCForeground;
    }
    if ( bg!=0 ) {
        gcv.background = bg->gcolor;
        mask |= GCBackground;
    }
    DBG2_Font("changing font=%s fg=%s bg=%s",
           mask&GCFont ? "yes" : "No",
           mask&GCForeground ? "yes" : "No",
           mask&GCBackground ? "yes" : "No");
    if ( mask!=0 )
      XChangeGC(E_Display,E_GCcache(gcd),mask,&gcv);
}

extern void gui_font_pix_extents(Tvgui* gui, Tvgui_GCdef gcd,
                        CstrBrief text,
                        double* _ascent, double* _descent,
                        double* _dx,     double* _dy)
{
    int dir,ascent,descent;
    XCharStruct overall_return;
    XQueryTextExtents(E_Display, XGContextFromGC(E_GCcache(gcd)),
            text, strlen(text),
            &dir, &ascent, &descent, &overall_return);
    if (_ascent)  *_ascent  = overall_return.ascent;
    if (_descent) *_descent = overall_return.descent;
    if (_dx)      *_dx      = overall_return.width;
    if (_dy)      *_dy      = overall_return.ascent+overall_return.descent;
}

extern void gui_font_src_extents(Tvgui_device* dev, Tvgui_GCdef gcd,
                        CstrBrief text,
                        double* _ascent, double* _descent,
                        double* _dx,     double* _dy)
{
    Tvgui* gui = dev->v_gui;
    int dir,ascent,descent;
    XCharStruct overall_return;
    XQueryTextExtents(E_Display, XGContextFromGC(E_GCcache(gcd)),
            text, strlen(text),
            &dir, &ascent, &descent, &overall_return);
    if (_ascent)  *_ascent  = VCONV_D_Win2Ds(dev,overall_return.ascent);
    if (_descent) *_descent = VCONV_D_Win2Ds(dev,overall_return.descent);
    if (_dx)      *_dx      = VCONV_D_Win2Ds(dev,overall_return.width);
    if (_dy)      *_dy      = VCONV_D_Win2Ds(dev,overall_return.ascent+overall_return.descent);
}

extern void gui_font_pix_drawNW(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double _x, double _y, CstrBrief text)
{
    int ascent;

    if ( text==0 || *text==0 ) return;
    Tvgui* gui    = dev->v_gui;
    gui_font_pix_extents(gui,gcd,text,&ascent,0,0,0);
    int x = round(_x); //VCONV_X_Ds2Win(dev,_x);
    int y = round(_y); //VCONV_Y_Ds2Win(dev,_y);
    XDrawString(E_Display,_XtWindow(dev->v_widEGS),
                E_GCcache(gcd),x,y+ascent,text,strlen(text));
}

extern void gui_font_pix_drawBL(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double _x, double _y, CstrBrief text)
{
    if ( text==0 || *text==0 ) return;
    Tvgui* gui    = dev->v_gui;
    Tgui_main* main = dev->v_gui->g_main;
    int x = round(_x); //VCONV_X_Ds2Win(dev,_x);
    int y = round(_y); //VCONV_Y_Ds2Win(dev,_y);
    XDrawString(E_Display,_XtWindow(dev->v_widEGS),
                main->m_gccache[gcd],x,y,text,strlen(text));
}

#if 0
extern void gui_font_src_drawNW(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double _x, double _y, CstrBrief text)
{
    double ascent;

    if ( text==0 || *text==0 ) return;
    Tvgui* gui    = dev->v_gui;
    gui_font_src_extents(dev,gcd,text,&ascent,0,0,0);
    int x = VCONV_X_Ds2Win(dev,_x);
    int y = VCONV_Y_Ds2Win(dev,_y-ascent);
    XDrawString(E_Display,_XtWindow(dev->v_widEGS),
                E_GCcache(gcd),x,y,text,strlen(text));
}

extern void gui_font_src_drawBL(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double _x, double _y, CstrBrief text)
{
    if ( text==0 || *text==0 ) return;
    Tvgui* gui    = dev->v_gui;
    Tgui_main* main = dev->v_gui->g_main;
    int x = VCONV_X_Ds2Win(dev,_x);
    int y = VCONV_Y_Ds2Win(dev,_y);
    XDrawString(E_Display,_XtWindow(dev->v_widEGS),
                main->m_gccache[gcd],x,y,text,strlen(text));
}
#endif

/*======================================================================*/
/*= Drawing rotated strings                                            =*/

#if 0
static void Gui_font_drawRect(Tvgui_device* dev,  Tvgui_GCdef gcd,
                              CboxDble* b, double a, double cx, double cy)
{
    int i;
    TpointDble pts[5];  // nw ne se sw nw
    pts[0].x = Box_x(*b)            ; pts[0].y = Box_y(*b);
    pts[1].x = Box_x(*b)+Box_dx(*b) ; pts[1].y = Box_y(*b);
    pts[2].x = Box_x(*b)+Box_dx(*b) ; pts[2].y = Box_y(*b)+Box_dy(*b);
    pts[3].x = Box_x(*b)            ; pts[3].y = Box_y(*b)+Box_dy(*b);

    for (i=0 ; i<4 ; i++) {
        ROTCC_PAXY(pts[i],a,cx,cy);
    }
    pts[4]=pts[0];
    gui_draw_rectangleXY(dev,gcd,pts[0].x,pts[0].y,Box_dx(*b),Box_dy(*b));
    gui_draw_lineNT(dev,gcd,pts,5);
}
#endif

extern void gui_font_pix_drawNWr(Tvgui_device* dev, Tvgui_GCdef gcd,
                        int x, int y, CstrBrief text, double a);
extern void gui_font_pix_drawBLr(Tvgui_device* dev, Tvgui_GCdef gcd,
                        int x, int y, CstrBrief text, double a);
#if 0
extern void gui_font_src_drawNWr(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double x, double y, CstrBrief text, double a)
{
    TboxDble box;
    double dx,dy;
    gui_font_src_extents(dev,gcd,text,0,0,&dx,&dy);
    Box_x (box) =  x;
    Box_y (box) =  y;
    Box_dx(box) = dx;
    Box_dy(box) = dy;
    Gui_font_drawRect(dev,gcd,&box,a,x,y);
}

extern void gui_font_src_drawBLr(Tvgui_device* dev, Tvgui_GCdef gcd,
                        double x, double y, CstrBrief text, double a)
{
    TboxDble box;
    double ascent,dx,dy;
    gui_font_src_extents(dev,gcd,text,&ascent,0,&dx,&dy);
    Box_x (box) =  x;
    Box_y (box) =  y-ascent;
    Box_dx(box) = dx;
    Box_dy(box) = dy;
    Gui_font_drawRect(dev,gcd,&box,a,x,y);
}
#endif

/*======================================================================*/
/*= initialization and free of module                                  =*/

extern void Gui_fonts_init2(Tvgui*gui)
{
    Tgui_main* main = gui->g_main;

    // check for font availability
    int nbfont=0,nbfontfound=0;
    int count,i,j;
    for ( i=0 ; i<FONT_NB ; i++ ) { 
        char fname[300];
        char** fontlist;
        CfontDef* f = fontTable+i;
        int pts[] = { 5, 6, 7, 8, 11, 15, 20, 100, 200, 300, 500};
        for (j=0 ; j<TOOL_TABLE_NbEle(pts) ; j+=1,nbfont+=1 ) {
            int pt=pts[j];
            sprintf(fname,f->format,pt,"0","ISO8859");
            fontlist = XListFonts(main->m_display, fname, 1, &count);
            if ( fontlist )
                nbfontfound += 1;
            else
                printf("%2d: pt=%d NOT found %s\n",i,pt,fname);
            if (fontlist) XFreeFontNames( fontlist );
        }
    }
    tool_info_printf("%d/%d fonts found.",nbfontfound,nbfont);
}

extern void Gui_fonts_free2(Tvgui*gui)
{
    PERM_spf_NYI(0);
}

/*======================================================================*/
/*= vim: set textwidth=0:                                              =*/
/*======================================================================*/
