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
 *      $Id: vdw-viewer-pts.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vdw-viewer-pts.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widgets for viewing and/or modifying Tdble2Set.             =*/
/*=                                                                    =*/
/*= See the vgui/vdw-viewer.h and vgui/wid-dialog.h headers for        =*/
/*= documentation.                                                     =*/
/*======================================================================*/

#include "vgui-config.h"

#include "vgui/vdw-viewer.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-lvu.h"
#include "vgui/wid-button.h"

#include "tools/math-macros.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= Data structure                                                     =*/

#define PTS_NB 5

struct _Tvdw_viewpts {
    Tvgui_dw           any;

    // widgets
    union {
        Twid_olvu*    o[PTS_NB];
        Twid_ilvu*    i[PTS_NB];
        Twid_lvuBase* m[PTS_NB];
    } x;
#define dw_ox x.o
#define dw_ix x.i
#define dw_x  x.m
    union {
        Twid_olvu*    o[PTS_NB];
        Twid_ilvu*    i[PTS_NB];
        Twid_lvuBase* m[PTS_NB];
    } y;
#define dw_oy y.o
#define dw_iy y.i
#define dw_y  y.m
    Twid_button*   dw_btnadd[PTS_NB];
    Twid_button*   dw_btnsup[PTS_NB];
    Twid_slider*   dw_slider;

    // proper data
    Tdble2Set*     dw_set;
    int            dw_pts_idx; // pts[idx) is printed in dw_x/y[0]
    Tvdw_view_cb   dw_cbAdd;
    Tvdw_view_cb   dw_cbSup;
    Tvdw_view_cb   dw_cbChg;
    void*          dw_cbUd;

    // configuration
    int  dw_cbEnabled;// avoid callback recursion
    int  dw_unit;     // set points are in LUNIT_ID_FU and points
                      // are displayed in dw_unit unit (LUNIT_ID_...)
    int  dw_minNbPts; // minimal number of points
    int  dw_maxNbPts; // maximal number of points
    int  dw_idxOfChangedPoint; // handled by dw_cbChg & vdw_viewpts_chgIdx
};

#define IsViewerOnly(p) ((p)->dw_cbAdd==0 && (p)->dw_cbSup==0 \
                                          && (p)->dw_cbChg==0)

#define  WIDXY_YMASK   0x40000000
#define  WIDXY_IND(ud) (((intptr_t)(ud))&(~WIDXY_YMASK))
#define  WIDXY_IsY(ud) ((((intptr_t)(ud))&WIDXY_YMASK)!=0)
#define  WIDXY_XID(i)  ((void*)((intptr_t)i))
#define  WIDXY_YID(i)  ((void*)((intptr_t)(i)|WIDXY_YMASK))

// actions
static void vdw_viewpts_update(Tvdw_viewpts* self);

// callbacks
static void vdw_viewpts_cb_add(Tvgui_dw*vdw, int btnId, void*ud);
static void vdw_viewpts_cb_sup(Tvgui_dw*vdw, int btnId, void*ud);
static void vdw_viewpts_cb_scroll(Twid_slider* slider, void *ud, double v);
static void vdw_viewpts_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v);

/*======================================================================*/
/*= Constructors & destructor                                          =*/

extern Tvdw_viewpts* vdw_viewpts_nbf(Tvgui_dw* vdw, CstrBrief title,
         Tdble2Set* pts, Tvdw_view_cb  add, Tvdw_view_cb  sup,
         Tvdw_view_cb  chg, void* ud, int options)
{
    int i;
#define       TITFMT    "%6d  "
#define       LABFMT    "x/y %4d "
    CstrBrief titfmt  = "__9999__";
    CstrBrief xyfmt   = "99999.99";
    CstrBrief labfmt  = "x/y 9999 ";

    Tvdw_viewpts* self = (Tvdw_viewpts*) vgui_dw_newSub(sizeof(*self),
                           vdw, tool_strdup(title) );
    vgui_dw_titleAddLabTxt(SelfA, NULL, titfmt);
    self->dw_set               = pts;
    self->dw_cbAdd             = add;
    self->dw_cbSup             = sup;
    self->dw_cbChg             = chg;
    self->dw_cbUd              = ud;
    self->dw_cbEnabled         = 1;
    self->dw_unit              = VDWVIEW_OPT_UNITget(options);
    self->dw_idxOfChangedPoint = -1;

    /* Create the points viewer */
    for (i=0 ; i<PTS_NB ; i++) {
        if ( IsViewerOnly(self) ) {
            self->dw_ox[i] = wid_olvu_newN10(0,xyfmt);
            self->dw_oy[i] = wid_olvu_newN10(0,xyfmt);
            wid_olvu_addLabel( self->dw_ox[i], NULL, labfmt);
        } else {
            self->dw_ix[i] = wid_ilvu_newD(0,xyfmt,
                  vdw_viewpts_cb_chg,vdw_viewpts_cb_chg,
                  WIDXY_XID(i), 0,0);
            self->dw_iy[i] = wid_ilvu_newD(0,xyfmt,
                  vdw_viewpts_cb_chg,vdw_viewpts_cb_chg,
                  WIDXY_YID(i), 0,0);
            wid_ilvu_addLabel( self->dw_ix[i], NULL, labfmt);
        }
    }

    for (i=0 ; i<PTS_NB ; i++) {
        if ( IsViewerOnly(self) ) {
            widD_olvu_dbf(self->dw_ox[i], SelfA);
            widD_olvu_dbf(self->dw_oy[i], SelfA);
        } else {
            widD_ilvu_dbf(self->dw_ix[i], SelfA);
            widD_ilvu_dbf(self->dw_iy[i], SelfA);
        }
    }

    /* Create modification button associated to the points viewer */
    if ( IsViewerOnly(self)==0 ) {
        for (i=0 ; i<PTS_NB ; i++) {
            self->dw_btnadd[i] = widD_button_std_nbfLab(SelfA,
                    "Add a point under this one",
                    vdw_viewpts_cb_add,NULL,NULL,"add",NULL);
            self->dw_btnsup[i] = widD_button_std_nbfLab(SelfA,
                    "remove this point",
                    //vdw_viewpts_cb_sup,NULL,NULL,"del",NULL);
                    vdw_viewpts_cb_sup,NULL,NULL,"del",NULL);
            self->dw_btnadd[i]->but_uid = i;
            self->dw_btnsup[i]->but_uid = i;
        }
    }

    /* create the slider that allows to move in the point set. */
    self->dw_slider = widD_slider_nbf(SelfA,
            0, 1./((double)self->dw_set->eleNb), // range
            vdw_viewpts_cb_scroll,self);
    wid_slider_setSize(self->dw_slider,15,PTS_NB*20);

    // do the layout
#   define LN(x)    VDWLF_ROW(x)
#   define LAB(c,l) c,self->dw_x[l]->lvu_labEGS
#   define WX(c,l)  c,self->dw_x[l]->lvu_valEGS
#   define WY(c,l)  c,self->dw_y[l]->lvu_valEGS
#   define ADD(c,l) c,self->dw_btnadd[l] ? self->dw_btnadd[l]->but_widEGS : NULL
#   define SUP(c,l) c,self->dw_btnsup[l] ? self->dw_btnsup[l]->but_widEGS : NULL
#   define SDR()    5,self->dw_slider->widEGS
    vgui_dw_finishMTX(&self->any
     ,LN(0) ,LAB(0,0) ,WX(1,0) ,WY(2,0) ,ADD(3,0) ,SUP(4,0) ,SDR()
     ,LN(1) ,LAB(0,1) ,WX(1,1) ,WY(2,1) ,ADD(3,1) ,SUP(4,1) ,SDR()
     ,LN(2) ,LAB(0,2) ,WX(1,2) ,WY(2,2) ,ADD(3,2) ,SUP(4,2) ,SDR()
     ,LN(3) ,LAB(0,3) ,WX(1,3) ,WY(2,3) ,ADD(3,3) ,SUP(4,3) ,SDR()
     ,LN(4) ,LAB(0,4) ,WX(1,4) ,WY(2,4) ,ADD(3,4) ,SUP(4,4) ,SDR()
     ,-1);

    self->dw_pts_idx = 0;
    vdw_viewpts_update(self);
    return self;
}

extern void         vdw_viewpts_free(Tvdw_viewpts* self)
{
    int i;

    for ( i=0 ; i<PTS_NB ; i++ ) {
        if ( IsViewerOnly(self) ) {
            wid_olvu_free( self->dw_ox[i] );
            wid_olvu_free( self->dw_oy[i] );
        } else {
            wid_ilvu_free( self->dw_ix[i] );
            wid_ilvu_free( self->dw_iy[i] );
        }
        wid_button_delete( self->dw_btnadd[i] );
        wid_button_delete( self->dw_btnsup[i] );
    }
    wid_slider_free( self->dw_slider );
    dble2set_delete( self->dw_set );
    vgui_dw_free  (SelfA);
}

/*======================================================================*/
/*= Accessors                                                          =*/

extern Tegs_widget vdw_viewpts_widEGS(Cvdw_viewpts* self) { return self->dw_widEGS; }
extern Tdble2Set*  vdw_viewpts_pts   (Cvdw_viewpts* self) { return self->dw_set; }
extern int         vdw_viewpts_chgIdx(Cvdw_viewpts* self) {
    TEMP_asf_printf(self->dw_idxOfChangedPoint<0,"%s","invalid call, no point changed");
    return self->dw_idxOfChangedPoint; }

extern Tdble2      vdw_viewpts_getDblePt (Tvdw_viewpts* self, int idx)     { return self->dw_set->eles[idx]; }
extern void        vdw_viewpts_setDblePt (Tvdw_viewpts* self, int idx, Tdble2 pt) { self->dw_set->eles[idx] = pt; }

extern void  vdw_viewpts_setUnit (Tvdw_viewpts* self, int uu)
{
    if ( self->dw_unit!=LUNIT_ID_None ) {
         self->dw_unit = uu;
        vdw_viewpts_update(self);
    } else {
        PERM_spe_printf("%s","unit change not implemented when >dw_unit is None");
    }
}

extern void  vdw_viewpts_setMinNb(Tvdw_viewpts* self, int nb)
{
    TEMP_asf_printf(nb!=0 && nb>self->dw_set->eleNb,"asked to %d min > %d set size",
        nb,self->dw_set->eleNb);
    self->dw_minNbPts = nb;
    if ( nb!=0 && self->dw_maxNbPts!=0 && self->dw_maxNbPts<self->dw_minNbPts )
        self->dw_maxNbPts = self->dw_minNbPts;
    vdw_viewpts_update(self);
}

extern void  vdw_viewpts_setMaxNb(Tvdw_viewpts* self, int nb)
{
    TEMP_asf_printf(nb!=0 && self->dw_set->eleNb>nb,"asked to %d max < %d set size",
        nb,self->dw_set->eleNb);
    self->dw_maxNbPts = nb;
    if ( nb!=0 && self->dw_minNbPts!=0 && self->dw_maxNbPts<self->dw_minNbPts )
        self->dw_minNbPts = self->dw_maxNbPts;
    vdw_viewpts_update(self);
}

extern void  vdw_viewpts_redraw  (Tvdw_viewpts* self) { vdw_viewpts_update(self); }

/*======================================================================*/
/*= Actions                                                            =*/

static void vdw_viewpts_update(Tvdw_viewpts* self)
{
    int i,ipts,nb;
    char buf[100];
    Tdble2Set* set = self->dw_set;
//printf("%s: set=%d min=%d max=%d\n",__func__,set->eleNb,self->dw_minNbPts,self->dw_maxNbPts);
    self->dw_cbEnabled -= 1;
    
    // number of points in the title bar
    Tegs_widget  mlabel  = vgui_dw_titleLab(SelfA);
    sprintf(buf,TITFMT,set->eleNb);
    wid_label_setTextSS(mlabel,buf);

    // the point list
    for (ipts=self->dw_pts_idx,nb=0 ; ipts<self->dw_set->eleNb ; ipts+=1 ) {
        if ( nb>=PTS_NB ) break;
        Tegs_widget label = self->dw_x[nb]->lvu_labEGS;
        sprintf(buf,LABFMT,ipts);
        wid_label_setTextSS(label,buf);

        Tdble2* p = self->dw_set->eles+ipts;
        double x = p->x, y = p->y;
        if ( self->dw_unit!=LUNIT_ID_None ) {
            x = lu_convFromFu(x,self->dw_unit);
            y = lu_convFromFu(y,self->dw_unit);
        }
        if ( IsViewerOnly(self) ) {
            wid_olvu_setNum10(self->dw_ox[nb],x);
            wid_olvu_setNum10(self->dw_oy[nb],y);
        } else {
            wid_ilvu_setNum10(self->dw_ix[nb],x);
            wid_ilvu_setNum10(self->dw_iy[nb],y);
            wid_ilvu_enable (self->dw_ix[nb]);
            wid_ilvu_enable (self->dw_iy[nb]);
        }
        nb += 1;
    }
    for (i=nb ; i<PTS_NB ; i+=1) {
        Tegs_widget label = self->dw_x[i]->lvu_labEGS;
        wid_label_setTextSS(label,"");
        if ( IsViewerOnly(self) ) {
            wid_olvu_clear(self->dw_ox[i]);
            wid_olvu_clear(self->dw_oy[i]);
            wid_olvu_disable(self->dw_ox[i]);
            wid_olvu_disable(self->dw_oy[i]);
        } else {
            wid_ilvu_clear(self->dw_ix[i]);
            wid_ilvu_clear(self->dw_iy[i]);
            wid_ilvu_disable(self->dw_ix[i]);
            wid_ilvu_disable(self->dw_iy[i]);
        }
    }

    // enable/disable widgets
    for ( i=0 ; i<PTS_NB ; i++) {
        Twid_button* btn;
        btn = self->dw_btnadd[i];
        if ( btn!=0 ) {
            if ( i>=nb ||
                 (self->dw_maxNbPts!=0 && self->dw_maxNbPts<=set->eleNb) )
                wid_button_disable(btn);
            else
                wid_button_enable(btn);
        }
        btn = self->dw_btnsup[i];
        if ( btn!=0 ) {
            if ( i>=nb ||
                 (self->dw_minNbPts!=0 && self->dw_minNbPts>=set->eleNb) )
                wid_button_disable(btn);
            else
                wid_button_enable(btn);
        }
    }

    self->dw_cbEnabled += 1;
}

/*======================================================================*/
/*= callback                                                           =*/

static void vdw_viewpts_cb_add(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tvdw_viewpts*  self = (Tvdw_viewpts*)vdw;
    Tdble2Set*     set  = self->dw_set;
    int            idx  = self->dw_pts_idx+btnId;
    double        x,y;
    x = set->eles[idx].x;
    y = set->eles[idx].y;
    if ( (idx+1)<set->eleNb ) {
        x = (x+set->eles[idx+1].x)/2;
        y = (y+set->eles[idx+1].y)/2;
    }
    dble2set_addAfterXY(self->dw_set,idx,x,y);
    vdw_viewpts_update( self );
    if ( self->dw_cbEnabled==1 && self->dw_cbAdd ) {
        self->dw_idxOfChangedPoint = idx+1;
        self->dw_cbAdd(SelfA,self->dw_cbUd,idx+1);
        self->dw_idxOfChangedPoint = -1;
    }
}

static void vdw_viewpts_cb_sup(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tvdw_viewpts*  self = (Tvdw_viewpts*)vdw;
    Tdble2Set*     set  = self->dw_set;
    int            idx  = self->dw_pts_idx+btnId;
    dble2set_supShift(set,idx);
    vdw_viewpts_update( self );
    if ( self->dw_cbEnabled==1 && self->dw_cbSup ) {
        self->dw_idxOfChangedPoint = idx;
        self->dw_cbSup(SelfA,self->dw_cbUd,idx);
        self->dw_idxOfChangedPoint = -1;
    }
}

static void vdw_viewpts_cb_scroll(Twid_slider* slider, void *ud, double v)
{
    Tvdw_viewpts*  self = ud;
    Tdble2Set*     set = self->dw_set;
    double idxdbl = set->eleNb; idxdbl *= v;
    int    idx = round( idxdbl );
    if (idx<0) idx=0; else if (idx>=set->eleNb) idx=set->eleNb-1;
    if (idx>=0) {
        self->dw_pts_idx = idx;
        vdw_viewpts_update(self);
    }
}

static void vdw_viewpts_cb_chg(Tvgui_dw*vdw, Twid_ilvu*iw, double v)
{
    Tvdw_viewpts* self = (Tvdw_viewpts*)vdw;
    int      idx  = WIDXY_IND(iw->lvu_ud) + self->dw_pts_idx;
    int      isy  = WIDXY_IsY(iw->lvu_ud);
    double*  p;

    if ( self->dw_cbEnabled!=1 ) return;
    if ( idx>=self->dw_set->eleNb ) {
        PERM_spe_printf("%d index >= %d array size",idx,self->dw_set->eleNb);
        return;
    }
    if ( isy==0 )
        p = &self->dw_set->eles[idx].x;
    else
        p = &self->dw_set->eles[idx].y;
    if ( self->dw_unit != LUNIT_ID_None )
        v = lu_convToFu(v, self->dw_unit);
    if ( dble_close(*p,v) )
        return;
    *p = v;
    if ( self->dw_cbEnabled==1 && self->dw_cbChg ) {
        self->dw_idxOfChangedPoint = idx;
        self->dw_cbChg(SelfA,self->dw_cbUd,idx);
        self->dw_idxOfChangedPoint = -1;
    }
}

/*======================================================================*/
