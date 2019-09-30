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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-dw-layout.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-dw-layout.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= Implementation of dialog layouts.                                  =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for the type and function            =*/
/*= documentation.                                                     =*/
/*= See the guixt/guixt-util.h header for the documentation about      =*/
/*= the xt_dw_... functions.                                           =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt/guixt-top.h"

#include "vgui/vgui-dw.h"
#include "guixt-dw.h"

/*======================================================================*/

#define WITH_AddColMarker 1

#define SEP_H     VDW_SEP_H
#define SEP_V     VDW_SEP_V
#define SEP_BP_V 10

/*======================================================================*/

typedef struct _Twidlen {
    Tegs_widget* widEGS;    // larger widget of row or column
    int          len;       // len of row or column 
    int          lenall;    // cumulative len from this row or column from the first one
} Twidlen;

static void gui_dw_finishMtxColLen(Twidlen* dest,
                            Tvgui_dwMTX mtx, int nl, int nc);
static void gui_dw_finishMtxRowLen(Twidlen* dest,
                            Tvgui_dwMTX mtx, int nl, int nc);

extern void gui_dw_resize_setBellow(Tvgui_dw* self, Tegs_widget w, Tegs_widget b)
{
    int i;
    TGUI_dialog* xtd = self->guiUserdata;

    for ( i=0 ; i<xtd->widgetNb ; i++ ) {
      if ( xtd->widgets[i]==w) xtd->widgetsBellow[i] = b;
    }
}

static void  gui_dw_cb_map(Tvgui_dw*self)
{
    Dimension len; int sep;
    Widget w,wbelow;
    int i;
    TGUI_dialog* xtd = self->guiUserdata;

    for ( i=0 ; i<xtd->widgetNb ; i++ ) {
      w      = xtd->widgets[i];
      XawEdgeType top,bot,left,right;
      Widget hwidget,vwidget;
      XtVaGetValues(w,
        XtNtop,      &top,     XtNbottom,  &bot,
        XtNleft,     &left,    XtNright,   &right,
        XtNfromHoriz,&hwidget, XtNfromVert,&vwidget, NULL);
      if ( top ==XtChainBottom && bot  ==XtChainBottom &&
           left==XtChainLeft   && right==XtChainRight  &&
           hwidget==0  && vwidget==0) {
        Position  cx,cy;
        Dimension cdx,cdy,cbw;
        int       hsep,vsep;
        Dimension pdx,pdy;
        XtVaGetValues(w, XtNhorizDistance,&hsep, XtNvertDistance,&vsep,
            XtNwidth,&cdx, XtNheight,&cdy, XtNborderWidth,&cbw,
            XtNx,&cx, XtNy,&cy, NULL);
        Widget pw = XtParent(w);
        XtVaGetValues(pw, XtNwidth,&pdx, XtNheight,&pdy, NULL);
        Position x = cx;
        Position y = pdy-cdy + vsep;
//printf("%d:..: %p fl=%04x : growToEastSW move to bottom\n", i, w, xtd->flags[i]);
        XtMoveWidget(w,x,y);
      }
    }

    for ( i=0 ; i<xtd->widgetNb ; i++ ) {
      w      = xtd->widgets[i];
      wbelow = xtd->widgetsBellow[i];
//printf("%d:..: %p fl=%04x\n", i, w, xtd->flags[i]);
      switch ( xtd->flags[i] ) {
        case VDM_V2E_BGrow:
//printf("%d:BG: vdw=%p w=%p \n",i,self,w);
          widget_resize_toSE( w, wbelow );
          break;
        case VDM_V2E_HGrow:
//printf("%d:HG: vdw=%p w=%p \n",i,self,w);
          widget_resize_toEast( w, NULL);
          break;
        case VDM_V2E_VGrow:
//printf("%d:VG: %p \n", i, w);
          widget_resize_toSouth( w, wbelow );
          break;
        case VDM_V2E_HCenter:
          XtVaGetValues(w, XtNwidth,&len, XtNhorizDistance,&sep, NULL);
//printf("%d:HC: %p sep=%d len[i]=%d len=%d\n",
//        i, w, sep, xtd->lengths[i], len);
          sep += (xtd->lengths[i] - len) / 2;
          XtVaSetValues(w,  XtNhorizDistance,sep, NULL);
          break;
#if 0
        default:
printf("%d:DE: %p fl=%04x HC=%04d\n", i, w, xtd->flags[i], VDM_V2E_HCenter);
#endif
      }
    }
}

/*======================================================================*/

extern void gui_dw_finishMic(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    Dimension len;
    int l,c;
    Tegs_widget w, currbottom;
    Dimension   wDY, currbottomDY,bottomDy=0;
    Tegs_widget above,left,bottom=0;
    int leftSep=vdw->D_hmargin, aboveSep=vdw->D_vspace;

    TGUI_dialog* xtd = vdw->guiUserdata;
    Twidlen wleft [VGUI_DW_MTX_SZ];
    gui_dw_finishMtxColLen(wleft,mtx,nl,nc);

    /* place the title */
    if ( title!=0 ) {
        widget_place_GrowToEast(title, leftSep,NULL, aboveSep,NULL);
        xtd->widgets[xtd->widgetNb] = title;
        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
        xtd->widgetNb += 1;
    }

    /* place the widgets */
    for ( left=0,c=0 ; c<nc ; c+=1 ) {
        above=title;
        currbottom=0; currbottomDY=0;
        for ( l=0 ; l<nl ; l+=1 ) {
            Tvgui_dwMtxCell* cell = &mtx[l][c];
            w = cell->widEGS;
            if ( w==0 ) continue;
            int sepH = leftSep;
            int sepV = aboveSep + cell->sepHV;
            if ( (cell->flag&VDM_V2E_BCenter)!=0 ) {
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_HCenter;
                xtd->lengths[xtd->widgetNb] = wleft[c].len;
                xtd->widgetNb += 1;
            }
            if ( (cell->flag&VDM_V2E_BGrow)==0 ) {
                widget_place_FixedSize(w, sepH,left, sepV,above);
            } else if ( (cell->flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                widget_place_GrowToSE(w, sepH,left, sepV,above);
                xtd->widgets      [xtd->widgetNb] = w;
                xtd->widgetsBellow[xtd->widgetNb] = (l+cell->ys)>=nl ? buttons : NULL;
                xtd->flags        [xtd->widgetNb] = VDM_V2E_BGrow;
                xtd->widgetNb += 1;
            } else if ( (cell->flag&VDM_V2E_HGrow)!=0 ) {
                widget_place_GrowToEast(w, sepH,left, sepV,above);
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
                xtd->widgetNb += 1;
            } else if ( (cell->flag&VDM_V2E_VGrow)!=0 ) {
                widget_place_GrowToSouth(w, sepH,left, sepV,above);
                xtd->widgets      [xtd->widgetNb] = w;
                xtd->widgetsBellow[xtd->widgetNb] = (l+cell->ys)>=nl ? buttons : NULL;
                xtd->flags        [xtd->widgetNb] = VDM_V2E_VGrow;
                xtd->widgetNb += 1;
            }
            above = w;
            XtVaGetValues(w, XtNheight,&wDY, NULL);
            currbottom    = w;
            currbottomDY += wDY;
        }
        left = wleft[c].widEGS;
        if ( currbottom!=0 && (bottom==0 || bottomDy<currbottomDY) ) {
            bottomDy = currbottomDY;
            bottom   = currbottom;
        }
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        if ( bottom==0 ) bottom=title;
        widget_place_GrowToEastSW(buttons,leftSep,NULL,SEP_BP_V,bottom);
        xtd->widgets[xtd->widgetNb] = buttons;
        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
        xtd->widgetNb += 1;
    }

    if ( xtd->widgetNb ) {
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_ResizeEvent, gui_dw_cb_map, NULL);
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_Map,         gui_dw_cb_map, NULL);
    }
}

/*======================================================================*/

extern void gui_dw_finishMil(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget button,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int l,c;
    Tegs_widget w,above,left;
    int leftSep=SEP_H, aboveSep=SEP_V;

    TGUI_dialog* xtd = vdw->guiUserdata;
    Twidlen wabove[VGUI_DW_MTX_SZ];
    gui_dw_finishMtxRowLen(wabove,mtx,nl,nc);

    /* place the title */
    if ( title!=0 ) {
        //widget_place_FixedSize(title,leftSep,NULL,0,NULL);
        widget_place_GrowToEast(title, leftSep,NULL, aboveSep,NULL);
        xtd->widgets[xtd->widgetNb] = title;
        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
        xtd->widgetNb += 1;
    }

    /* place the widgets */
    above=title;
    for ( l=0 ; l<nl ; l+=1 ) {
        for ( left=0,c=0 ; c<nc ; c+=1 ) {
            Tvgui_dwMtxCell* cell = &mtx[l][c];
            w = cell->widEGS;
            if ( w==0 ) continue;
            int sepH = leftSep + cell->sepHV;
            int sepV = aboveSep ;
            if ( (cell->flag&VDM_V2E_BCenter)!=0 ) {
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_VCenter;
                xtd->lengths[xtd->widgetNb] = wabove[l].len;
                xtd->widgetNb += 1;
            }
            if ( (cell->flag&VDM_V2E_BGrow)==0 ) {
                widget_place_FixedSize(w, sepH,left, sepV,above);
            } else if ( (cell->flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                widget_place_GrowToSE(w, sepH,left, sepV,above);
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_BGrow;
                xtd->widgetNb += 1;
            } else if ( (cell->flag&VDM_V2E_HGrow)!=0 ) {
                widget_place_GrowToEast(w, sepH,left, sepV,above);
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
                xtd->widgetNb += 1;
            } else if ( (cell->flag&VDM_V2E_VGrow)!=0 ) {
                widget_place_GrowToSouth(w, sepH,left, sepV,above);
                xtd->widgets[xtd->widgetNb] = w;
                xtd->flags  [xtd->widgetNb] = VDM_V2E_VGrow;
                xtd->widgetNb += 1;
            }
            left = w;
        }
        above = wabove[l].widEGS;
    }

    /* place the buttons */
    if ( button!=0 ) {
        Cvgui_dwTop* top = vdw->D_top;
        if ( top==0 )
            widget_place_FixedSize(button,leftSep,NULL,SEP_BP_V,above);
        else switch ( (top->dt_btnLayout>1)&3 ) {
            case 1: // east
              widget_place_FixedSize(button,leftSep,left,SEP_BP_V,title);
              break;
            default:// bottom
              widget_place_FixedSize(button,leftSep,NULL,SEP_BP_V,above);
              break;
        }
    }

    if ( xtd->widgetNb ) {
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_ResizeEvent, gui_dw_cb_map, NULL);
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_Map,         gui_dw_cb_map, NULL);
    }
}

/*======================================================================*/

extern void gui_dw_finishMfa(Tvgui_dw*vdw,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int i,l,c;
    Dimension   wDX,wDY; //,nextaboveDY;
    Tegs_widget w,above,left; //,nextabove;
    int leftSep=SEP_H, aboveSep=SEP_V;

    TGUI_dialog* xtd = vdw->guiUserdata;
    Twidlen wleft [VGUI_DW_MTX_SZ];
    Twidlen wabove[VGUI_DW_MTX_SZ];
    gui_dw_finishMtxColLen(wleft,mtx,nl,nc);
    gui_dw_finishMtxRowLen(wabove,mtx,nl,nc);

    /* place the title */
    if ( title!=0 ) {
        widget_place_GrowToEast(title, leftSep,NULL, aboveSep,NULL);
        xtd->widgets[xtd->widgetNb] = title;
        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
        xtd->widgetNb += 1;
    }

#if WITH_AddColMarker
    /* add invisble labels at top of the dialog to fixe the column starts */
    Tvgui*      gui = vgui_dw_getVGui(vdw);
    for (c=0 ; c<(nc-1) ; c++) {
#if 1  // 0 for debug
        wleft[c].widEGS= create_label(gui,vdw->D_widEGS,vdw->D_wcn,
            0,wleft[c].lenall,1, EGSCOLOR_default,EGSCOLOR_default, NULL);
        widget_place_FixedSize(wleft[c].widEGS, 0,NULL, 0,NULL);
#else
        wleft[c].widEGS= create_label(gui,vdw->D_widEGS,vdw->D_wcn,
            0,wleft[c].lenall,4,gui->vg_black->egs ,EGSCOLOR_default, NULL);
        widget_place_FixedSize(wleft[c].widEGS, 0,NULL, 5,title);
        title=wleft[c].widEGS;
#endif
    }
#endif

    /* place the widget */
    for ( above=title,l=0 ; l<nl ; l+=1 ) {
        for ( left=0,c=0 ; c<nc ; left=wleft[c].widEGS,c=c+=1 ) {
            Tvgui_dwMtxCell* cell = &mtx[l][c];
            w = cell->widEGS;
            if ( w==0 ) continue;
            if ( cell->xs==0 || cell->ys==0 ) continue;
            int sepH = leftSep + cell->sepHV;
            int sepV = aboveSep ;
            if ( cell->xs==1 && cell->ys==1 ) {
place_widget:
                if ( (cell->flag&VDM_V2E_BGrow)==0 ) {
                    widget_place_FixedSize(w, sepH,left, sepV,above);
                } else if ( (cell->flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                    if ( (c+cell->xs)>=nc ) {
                        widget_place_GrowToSE(w, sepH,left, sepV,above);
                        xtd->widgets      [xtd->widgetNb] = w;
                        xtd->widgetsBellow[xtd->widgetNb] = (l+cell->ys)>=nl ? buttons : NULL;
                        xtd->flags        [xtd->widgetNb] = VDM_V2E_BGrow;
                        xtd->widgetNb += 1;
                    } else {
                        wDX = leftSep*(cell->xs-1);
                        for (i=0 ; i<cell->xs ; i++) wDX+=wleft[c+i].len;
                        XtVaSetValues(w, XtNwidth,wDX, NULL);
                        widget_place_FixedSize(w, sepH,left, sepV,above);
                    }
                } else if ( (cell->flag&VDM_V2E_HGrow)!=0 ) {
                    if ( (c+cell->xs)>=nc ) {
                        widget_place_GrowToEast(w, sepH,left, sepV,above);
                        xtd->widgets[xtd->widgetNb] = w;
                        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
                        xtd->widgetNb += 1;
                    } else {
                        wDX = leftSep*(cell->xs-1);
                        for (i=0 ; i<cell->xs ; i++) wDX+=wleft[c+i].len;
                        XtVaSetValues(w, XtNwidth,wDX, NULL);
                        widget_place_FixedSize(w, sepH,left, sepV,above);
                    }
                } else if ( (cell->flag&VDM_V2E_VGrow)!=0 ) {
                    widget_place_GrowToSouth(w, sepH,left, sepV,above);
                    xtd->widgets      [xtd->widgetNb] = w;
                    xtd->widgetsBellow[xtd->widgetNb] = (l+cell->ys)>=nl ? buttons : NULL;
                    xtd->flags        [xtd->widgetNb] = VDM_V2E_VGrow;
                    xtd->widgetNb += 1;
                }
            } else {
                wDX = leftSep*(cell->xs-1);
                for (i=0 ; i<cell->xs ; i++) wDX+=wleft[c+i].len;
                wDY = aboveSep*(cell->ys-1);
                for (i=0 ; i<cell->ys ; i++) wDY+=wabove[l+i].len;
                XtVaSetValues(w, XtNwidth,wDX, XtNheight,wDY, NULL);
                goto place_widget;
            }
        }
        if ( wabove[l].widEGS!=0 )
            above = wabove[l].widEGS;
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        widget_place_GrowToEastSW(buttons,leftSep,NULL,SEP_BP_V,above);
        xtd->widgets[xtd->widgetNb] = buttons;
        xtd->flags  [xtd->widgetNb] = VDM_V2E_HGrow;
        xtd->widgetNb += 1;
    }

    if ( xtd->widgetNb ) {
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_ResizeEvent, gui_dw_cb_map, NULL);
        widget_cb_addU(vdw->D_widEGS, vdw, YFIG_EVENT_Map,         gui_dw_cb_map, NULL);
    }
}

/*======================================================================*/

static void gui_dw_finishMtxColLen(Twidlen* dest,
                            Tvgui_dwMTX mtx, int nl, int nc)
{
    int l,c,c2;
    Dimension len;
    Tegs_widget      w;
    Tvgui_dwMtxCell* cell;

    for (c=0 ; c<nc ; c++) {
        dest[c].widEGS = NULL;
        dest[c].len    = 0;
        dest[c].lenall = 0;
#if WITH_AddColMarker
        for (l=0 ; l<nl ; l++) for (c2=0 ; c2<nc ; c2++) {
                // skipp all the cells that are empty or
                // that do not end to the c column
                cell = &mtx[l][c2];
                w = cell->widEGS;
                if ( w==0 ) continue;
                if ( cell->xs==0 ) continue;
                if ( cell->xs-1+c2!=c ) continue;
                Dimension wlen; // widget width
                Dimension elen; // length from the dialog left border to the w right border
                XtVaGetValues(w, XtNwidth,&wlen, NULL);
                // Very bad hook:
                //   here spinner width is just the width of its slin widget, because the
                //   spinner is not yet realized. So we had the left arrow width.
                     if ( strcmp(XtName(w),wn_dialSpin)==0 ) wlen+=11;
                elen = c2==0 ? 0 : dest[c2-1].lenall;
                elen += wlen + SEP_H * cell->xs;
                if (elen>dest[c].lenall) {
                    dest[c].widEGS=w;
                    dest[c].lenall=elen;
                }
        }
        // we have the column right border (except if column is empty)
        if (c==0) {
            dest[c].len    = dest[c].lenall - SEP_H;
        } else if ( dest[c].lenall==0 ) {
            // empty column
            dest[c].widEGS = dest[c-1].widEGS;
            dest[c].len    = 0;
            dest[c].lenall = dest[c-1].lenall;
        } else {
            dest[c].len    = dest[c].lenall - dest[c-1].lenall - SEP_H;
        }
    }
#else
        for (l=0 ; l<nl && dest[c].widEGS==0 ; l++) {
            for (c2=0 ; c2<nc && dest[c].widEGS==0 ; c2++) {
                cell = &mtx[l][c2];
                w = cell->widEGS;
                if ( w==0 ) continue;
                if ( cell->xs==0 ) continue;
                if ( cell->xs-1+c2==c )
                    dest[c].widEGS = cell->widEGS;
            }
        }
        for (l=0 ; l<nl ; l++) {
            cell = &mtx[l][c];
            w = cell->widEGS;
            if ( w==0 ) continue;
            if ( cell->xs!=1 ) continue;
            XtVaGetValues(w, XtNwidth,&len, NULL);
            if ( len>dest[c].len ) {
                dest[c].len = len;
            }
        }
    }
#endif
}

static void gui_dw_finishMtxRowLen(Twidlen* dest,
                            Tvgui_dwMTX mtx, int nl, int nc)
{
    int l,c;
    Dimension len;
    Tegs_widget      w;
    Tvgui_dwMtxCell* cell;

    for (l=0 ; l<nl ; l++) {
        dest[l].widEGS = NULL;
        dest[l].len    = 0;
        for (c=0 ; c<nc ; c++) {
            cell = &mtx[l][c];
            w = cell->widEGS;
            if ( w==0 ) continue;
            if ( cell->ys!=1 ) continue;
            XtVaGetValues(w, XtNheight,&len, NULL);
            if ( len>dest[l].len ) {
                dest[l].widEGS = w;
                dest[l].len = len;
            }
        }
    }
}

/*======================================================================*/
