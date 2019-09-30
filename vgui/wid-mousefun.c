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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: wid-mousefun.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-mousefun.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= A widget that shows an help about mouse buttons.                   =*/
/*=                                                                    =*/
/*= See the vgui/wid-mousefun.h header for the type and function       =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-outdev.h"
#include "vgui/vgui-mw.h"
#include "vgui/wid-mousefun.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-egs.h"

#include "outdev/src-vt.h"
#include "outdev/string-hl.h"
#include "outdev/ldaf-hl.h"

#define MOUSEFUN_WD_MIN   275
#define MOUSEFUN_HT_START  40

/*======================================================================*/

static void wid_mfun_doDraw(Coutdev* dev);

static Toutdev_src_vt mfun_vt = {
    .parent = &outdev_src_vt,
    .draw   = wid_mfun_doDraw
};
/*======================================================================*/
/*= The mouse fun widget (interface)                                   =*/

extern Twid_mousefun* wid_mfun_create(Tvgui_mw* vmw, Tegs_widget parent)
{
    Tvgui* gui = vgui_mw_getVGui(vmw);
    Twid_mousefun* mf = tool_malloc_0( sizeof(*mf) );

    double dx=MOUSEFUN_WD_MIN;
    double dy=MOUSEFUN_HT_START;
#if 1   // 0 for testing canvas widget
    mf->mf_canvas = widM_canvas_nbf1(vmw, parent,
            //0, MOUSEFUN_WD_MIN, MOUSEFUN_HT_START, 0);
            0, dx, dy, 0);
#else
    mf->mf_canvas = wid_canvas_nbf1(vmw, parent,
            //2, MOUSEFUN_WD_MIN, MOUSEFUN_HT_START, 5);
            2, dx, dy, 5);
#endif
    Toutdev* od = &mf->mf_outdev->vv_dev;
    od_str_extents_DesUnit_gc(od,OD_GC_SysDraw,MOUSEFUN_WD_STR,
            NULL,NULL,&dx,&dy);
    gui_widegs_setSize( mf->mf_outdev->vv_cvsWidEGS,3*dx,dy+(3*dy)/4);

    od_attach( od, &mfun_vt, od->ppiDes, mf); 

    return mf;
}

extern void           wid_mfun_delete(Twid_mousefun* mf)
{
    wid_canvas_free( mf->mf_canvas );
    tool_free(mf->mf_leftTitle);
    tool_free(mf->mf_midTitle);
    tool_free(mf->mf_rightTitle);
    free( mf );
}

extern void           wid_mfun_show (Twid_mousefun* mf,
                            CstrBrief l, CstrBrief m, CstrBrief r)
{
    tool_free( mf->mf_leftTitle ); mf->mf_leftTitle  = l==0 ? 0 : strdup(l);
    tool_free( mf->mf_midTitle  ); mf->mf_midTitle   = m==0 ? 0 : strdup(m);
    tool_free( mf->mf_rightTitle); mf->mf_rightTitle = r==0 ? 0 : strdup(r);
    od_redraw(&mf->mf_outdev->vv_dev);
}

/*======================================================================*/
/*= Quick interface to mouse fun widget.                               =*/

extern void wid_mfun_showDt(Twid_mousefunDt* dt, int n)
{
    if ( dt==0 || dt->widget==0 ) return;
    wid_mfun_show(dt->widget,
        dt->messages[n].str[0],
        dt->messages[n].str[1],
        dt->messages[n].str[2]);
}

/*======================================================================*/
/*= The drawing function                                               =*/

/**
 |<---dx-------------------------------------------------->
 |                    ↑  middle title
 | Mouse buttons      by     |
 |                    ↓      |
 |                  +---+  +---+  +---+         ↑
 |<---bx----------->|   |  |   |  |   |         |
 |                  |   |  |   |  |   |        bdy
 |               ---|   |  |   |  |   |---      |
 |                  +---+  +---+  +---+ bdx/2   ↓
 |                   bdx       bdx/2
**/
#define M_BDX    12 
#define M_BDY    ((int)(dy * 0.5))
#define M_BX     ((dx - 4 * M_BDX) / 2)
#define M_BY     ((int)(dy*0.45))

static void wid_mfun_doDraw( Coutdev*_od)
{
    Toutdev*       od  = (Toutdev*)_od;
    Twid_mousefun* mf  = od->parSrc;
    Tod_GCdef      gcd = OD_GC_SysDraw;

    int dx = Box_dx(od->boxDes);
    int dy = Box_dy(od->boxDes);
    int bdx = M_BDX;
    int bdy = M_BDY;
    int bx  = M_BX;
    int by  = M_BY;

    int  wTBL_y = by;             // baseline of widget title
    int  mTBL_y = by-(bdx/2)-2;   // baseline of middle title
    int lrTBL_y = by+(2*bdy)/3+2; // baseline of left & right title

    od_clear(od);

    // draw the left button
    int x=bx;
    od_draw_rectXYWH(od,gcd, x,by,bdx,bdy);
    od_draw_lineXY  (od,gcd,x-bdx/2,by+bdy/2,x,by+bdy/2);
    // draw the middle button
    x += bdx + bdx/2;
    od_draw_rectXYWH(od,gcd, x,by,bdx,bdy);
    od_draw_lineXY  (od,gcd,x+bdx/2,by-bdx/2,x+bdx/2,by);
    // draw the right button
    x += bdx + bdx/2;
    od_draw_rectXYWH(od,gcd, x,by,bdx,bdy);
    od_draw_lineXY  (od,gcd,x+bdx,by+bdy/2,x+bdx+bdx/2,by+bdy/2);
    int bx_l  = bx;          // left  border of left button
    int bx_r  = x+bdx+bdx/2; // right border of right button

    // now draw the texts
    od_str_draw_LR(od,gcd, MOUSEFUN_WD_STR,  2,                 wTBL_y);
    od_str_draw_CE(od,gcd, mf->mf_leftTitle, bx_l/2,           lrTBL_y);
    od_str_draw_CE(od,gcd, mf->mf_midTitle,  dx/2,              mTBL_y);
    od_str_draw_CE(od,gcd, mf->mf_rightTitle,bx_r+(dx-bx_r)/2, lrTBL_y);
}

/*======================================================================*/
