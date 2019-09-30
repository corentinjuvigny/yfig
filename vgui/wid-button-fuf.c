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
 *      $Id: wid-button-fuf.c 121 2018-02-17 11:44:35Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-button-fuf.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Buttons dedicated to fold or unfold others widgets.                =*/
/*=                                                                    =*/
/*= The folding/unfolding is implemented by 2 ways                     =*/
/*=   1) The buttons contains a set of EGS widgets that it maps for    =*/
/*=      folding and that it unmaps for unfolding.                     =*/
/*=   2) The buttons contains a parent widget and a set of EGS         =*/
/*=      widgets. For unfolding, it unmaps all the child widgets of    =*/
/*=      the parent except of those specified in the set.              =*/
/*=      For folding, it maps all the child widgets of the parent.     =*/
/*=      Notice that the set must contains only child widget of the    =*/
/*=      parent.                                                       =*/
/*=                                                                    =*/
/*= The fuf button is an On/Off button with a parent widget and a      =*/
/*= widget set.                                                        =*/
/*=                                                                    =*/
/*= See the vgui/wid-button.h header for documentation.                =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/wid-egs.h"
#include "vgui/wid-button.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Twid_fuf {
    Twid_button* btn;       // to get the state
    Tegs_widget  parent;
    Tegs_widSet* wids;
} Twid_fuf;

static Twid_fuf* wid_fuf_data_new()
{
    Twid_fuf* data = tool_malloc( sizeof(*data) );
    // data->btn    = 0; initialized in ..._nbf... functions
    data->parent = 0;
    data->wids   = egswidset_new(10);
    return data;
}

static void      wid_fuf_data_free(Twid_fuf*data)
{
    egswidset_free( data->wids );
    free( data );
}

/*======================================================================*/
/*= VGUI interface                                                     =*/

static void wid_fuf_cb   (Tvgui_dw*vdw, int uid, void*ud);
static void wid_fuf_free (Tvgui*gui,    void*ud);

extern Twid_button* widD_button_fuf_nbflab(Tvgui_dw*vdw,
                        CstrBrief off, CstrBrief on)
{
    Twid_fuf* data = wid_fuf_data_new( );
    if ( off==0 ) off = "show";
    if ( on==0  ) on  = "hide";
    data->btn = widD_button_onoff_newLab(vdw,NULL,
            wid_fuf_cb, data, wid_fuf_free, off,on);
    data->btn->btn_option |= WID_BTNOPT_CCBWM;
    wid_button_dbf(data->btn,NULL);
    return data->btn;
}

extern Twid_button* widD_button_fuf_nbfPxm(Tvgui_dw*vdw,
                        int dx, int dy, TpixmapEGS off, TpixmapEGS on)
{
    Twid_fuf*     data = wid_fuf_data_new( );
    data->btn = widD_button_onoff_newPxm(vdw,NULL,
            wid_fuf_cb, data, wid_fuf_free, dx,dy,off,on);
    data->btn->btn_option |= WID_BTNOPT_CCBWM;
    wid_button_dbf(data->btn,NULL);
    return data->btn;
}

extern Twid_button* widD_button_fuf_nbfPXM(Tvgui_dw*vdw)
{
    Tvgui* gui = vgui_dw_getVGui(vdw);
    return widD_button_fuf_nbfPxm(vdw,
            gui->g_pmFoldDx,gui->g_pmFoldDy,
            gui->g_pmUnfold,gui->g_pmFold);
}

extern Twid_button* widD_button_fuf_addWidToCtl(Twid_button*self, ...)
{
    va_list ap;
    Tegs_widget  w;
    Twid_fuf*    data = self->but_ud;
    va_start(ap,self);
    for ( w=va_arg(ap,Tegs_widget) ; w!=0 ; w=va_arg(ap,Tegs_widget) ) {
        egswidset_add(data->wids,w);
    }
    va_end(ap);
}

extern Twid_button* widD_button_fuf_addWidToIgn(Twid_button*self, 
                                                Tegs_widget pw, ...)
{
    va_list ap;
    Tegs_widget  w;
    Twid_fuf*    data = self->but_ud;
    data->parent = pw;
    va_start(ap,pw);
    for ( w=va_arg(ap,Tegs_widget) ; w!=0 ; w=va_arg(ap,Tegs_widget) ) {
        egswidset_add(data->wids,w);
    }
    va_end(ap);
}

extern Twid_button* widD_button_fuf_setFolded(Twid_button*self)
{
    wid_button_setOff(self);
    wid_fuf_cb(NULL,self->but_uid,self->but_ud);
}

extern Twid_button* widD_button_fuf_setUnFolded(Twid_button*self)
{
    wid_button_setOn(self);
    wid_fuf_cb(NULL,self->but_uid,self->but_ud);
}

/*======================================================================*/
/*= Local utilities                                                    =*/

static void wid_fuf_cb (Tvgui_dw*vdw, int uid, void*ud)
{
    Twid_fuf*    data  = ud;
    Twid_button* self  = data->btn;
    int          state = self->but_onoff_state;
    int i,j;

    if ( !gui_widegs_isShown(self->but_widEGS) ) {
        return;
    }

    if ( data->parent==0 ) {
        for ( i=0; i<data->wids->eleNb ; i+=1 ) {
            if ( state == WID_BUT_STATE_On )
                gui_widegs_show( data->wids->eles[i] );
            else
                gui_widegs_unshow( data->wids->eles[i] );
        }
    } else {
        Tegs_widSet* children = egswidset_new(100);
        gui_widegs_children(data->parent,children);
        for ( i=0; i<children->eleNb ; i+=1 ) {
            Tegs_widget w = children->eles[i];
            for ( j=0; j<data->wids->eleNb ; j+=1 ) {
                if ( w==data->wids->eles[j] ) break;
            }
            if ( j!=data->wids->eleNb ) continue;
            // w child is not in data->wids, so we can map/unmap it
            if ( state == WID_BUT_STATE_On ) {
                gui_widegs_show( children->eles[i] );
            } else {
                gui_widegs_unshow( children->eles[i] );
            }
        }
        egswidset_free(children);
        if ( data->wids->eleNb==0 ) {
            if ( state == WID_BUT_STATE_On ) {
                gui_widegs_show( data->parent );
            } else {
                gui_widegs_unshow( data->parent );
            }
        }
    }
}

static void wid_fuf_free (Tvgui*gui,    void*ud)
{ wid_fuf_data_free( ud ); }

/*======================================================================*/
