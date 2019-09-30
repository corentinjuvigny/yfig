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
 *$Software: YFIG-TMB (Top Menu Bar)
 *      $Id: ydw-export-option.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-export-option.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

#include "yfig-config.h"
#include "vgui/vgui.h"
#include "yfig-mw.h"
#include "yfig-dsds.h"
#include "vgui/vgui-dw.h"
#include "yfig-export-option.h"
#include "ydw/ydw.h"

#include "vgui/wid-simple.h"
#include "vgui/wid-button.h"
#include "vgui/wid-menu.h"
#include "vgui/wid-lvu.h"

#if 0
#define widm_ddl_unshow(w)   while(0) //widm_ddl_disable(w);
#define wid_ilvu_unshow(w)   while(0) //wid_ilvu_disable(w);
#define gui_widegs_unshow(w) while(0) //gui_widget_disable(w)
#endif

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tydw_expopt {
    Tvgui_dw          any;

    // widgets
    Twid_button*      dw_btn_allDepth;
    Twid_button*      dw_btn_selDepth;
    Twid_ilvu*        dw_srcBBKind;

    TastrSet*         dw_paperDefTable;
    Twidm_ddl*        dw_paperKind;
    Twidm_ddl*        dw_paperDef;
    Twid_ilvu*        dw_paperBd;
    Twid_ilvu*        dw_paperOrient;
    Twid_ilvu        *dw_paperDx,*dw_paperDy;
    Twid_ilvu        *dw_paperLi,*dw_paperCo;

    Twidm_ddl*        dw_placeKind;
    Twidm_ddl*        dw_placeWR;
    Twid_ilvu*        dw_placeZoom;

    Twid_ilvu*        dw_rasPpi;  // ppi for raster image format
    Twid_ilvu*        dw_rasQly;  // quality for lossy raster image format

    // private data
    int               dw_mode;  // 0:export ; 1:print
    TexportOption     dw_option;
    int               dw_updating; // -2: under construction 
                                   // -1: dialog is built, first update can be run.
                                   //  0: dialog is fully initialized
                                   //  1: dialog is fully initialized and the
                                   //     expopt_act_update function is running
                                   //     (avoid recursive call)
    Twid_cb_DU        dw_cb;
    void*             dw_cbUD;
};

/*======================================================================*/
/*= Declaration of private utilities.                                  =*/

// actions
static void expopt_act_update(Tydw_expopt* self);
Inline void expopt_act_callCB(Tydw_expopt* self) { if ( self->dw_cb!=0 && self->dw_updating==0 ) self->dw_cb(SelfA,self->dw_cbUD);}

// callbacks 
#define EXPOPT_ilvuNum(field)     self->dw_option.field = wid_ilvu_getNum10(iw); expopt_act_update(self)
#define EXPOPT_ilvuDdl(field)     self->dw_option.field = wid_ilvu_getValDdl_CurrIdAny(iw).uint; expopt_act_update(self)
static void expopt_cb_btnDepths   (Tvgui_dw*vdw, int btnId,     void*ud);
static void expopt_cb_srcBBKind   (Tvgui_dw*vdw, Twid_ilvu* iw, double v) { Tydw_expopt* self = (Tydw_expopt*)vdw; EXPOPT_ilvuDdl(srcBBKind); }
static void expopt_cb_paperKind   (Twidm_any*m,  Twidm_entry*e);
static void expopt_cb_paperDef    (Twidm_any*m,  Twidm_entry*e);
static void expopt_cb_paperOrient (Tvgui_dw*vdw, Twid_ilvu* iw, double v) { Tydw_expopt* self = (Tydw_expopt*)vdw; EXPOPT_ilvuDdl(paperOrient); }
static void expopt_cb_paperBdChg  (Tvgui_dw*vdw, Twid_ilvu* iw, double v) { Tydw_expopt* self = (Tydw_expopt*)vdw; expopt_act_callCB(self); }
static void expopt_cb_paperLiChg  (Tvgui_dw*vdw, Twid_ilvu* iw, int    v) { Tydw_expopt* self = (Tydw_expopt*)vdw; expopt_act_callCB(self); }
static void expopt_cb_paperCoChg  (Tvgui_dw*vdw, Twid_ilvu* iw, int    v) { Tydw_expopt* self = (Tydw_expopt*)vdw; expopt_act_callCB(self); }
static void expopt_cb_paperDxChg  (Tvgui_dw*vdw, Twid_ilvu* iw, double v);
static void expopt_cb_paperDyChg  (Tvgui_dw*vdw, Twid_ilvu* iw, double v);
static void expopt_cb_placeKind   (Twidm_any*m,  Twidm_entry*e);
static void expopt_cb_placeWR     (Twidm_any*m,  Twidm_entry*e)           { Tydw_expopt* self = m->ma_appCBud;     expopt_act_callCB(self); }
static void expopt_cb_placeZoomChg(Tvgui_dw*vdw, Twid_ilvu* iw, double v) { Tydw_expopt* self = (Tydw_expopt*)vdw; EXPOPT_ilvuNum(placeZoom); }
static void expopt_cb_rasPpi      (Tvgui_dw*vdw, Twid_ilvu* iw, int    v) { Tydw_expopt* self = (Tydw_expopt*)vdw; expopt_act_callCB(self); }
static void expopt_cb_rasQlt      (Tvgui_dw*vdw, Twid_ilvu* iw, int    v) { Tydw_expopt* self = (Tydw_expopt*)vdw; expopt_act_callCB(self); }

/*======================================================================*/
/*= Interface                                                          =*/

extern Tydw_expopt* ydw_expopt_nbf(Tyfig_mw* ymw, CexportOption* _eo,
                                   int flag, // 0:export ; 1:print ; ?:print
                                   Twid_cb_DU cb, void* ud)
{
    int         i;
    Tvgui*      gui  = ymw->ymw_gui;
    int         uu   = ymw->ymw_userUnit.uu_id;
    int         ubd  = LUNIT_ID_Pt;
    char*       title      = flag==0 ? "Set export options & parameters"
                                     : "Set print options & parameters";
    CstrStatic     msg1,msg2,msg3;
    Twid_ilvu*     ilvu;
    Twidm_ddl*     ddl;
    TexportOption* eo;

    Tydw_expopt* self = (Tydw_expopt*) vgui_dw_newTop(sizeof(*self),
            &ymw->any, tool_strdup(title),
            WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok
    );
    self->dw_mode     = flag;
    self->dw_option   = yfig_expopt_initClone(_eo);
    self->dw_updating = -2;
    self->dw_cb       = cb;
    self->dw_cbUD     = ud;

    eo = &self->dw_option;
    if ( self->dw_mode==1 ) eo->paperKind=ExpOptPaKd_SetByUser;

  /* which layers to export */
    msg1 = "Which layers\nto export";
    msg2 = "all the depths";
    msg3 = "the selected ones";
    Tegs_widget depthsLabel = widD_label_nbf(SelfA,msg1,NULL);
    self->dw_btn_allDepth = widD_button_check_newLab(SelfA,
            NULL, // help text
            expopt_cb_btnDepths, SelfA, NULL, // cb, ud, free
            msg2, msg3); // label and width
    wid_button_dbf( self->dw_btn_allDepth, NULL);
    self->dw_btn_selDepth = widD_button_check_newLab(SelfA,
            NULL, // help text
            expopt_cb_btnDepths, SelfA, NULL, // cb, ud, free
            msg3, msg3); // label and width
    wid_button_dbf( self->dw_btn_selDepth, NULL);
    self->dw_btn_selDepth->but_uid = 1;

  /* source Bounding Box */
    msg1 = "Bounding box\nto export";
    static char* srcBBKind_table[] = {
        "figure with all the depths",
        "figure with selected depths",
        "figure with all the depths starting at origin",
        NULL
    };
    ilvu = wid_ilvu_newDdlStr(0,srcBBKind_table,(Twid_cb_Uap)expopt_cb_srcBBKind,NULL);
    wid_ilvu_addLabel(ilvu,msg1,NULL);
    widD_ilvu_dbf(ilvu,SelfA);
    ddl = wid_ilvu_getValDdl(ilvu);
    widm_ddl_setEntryIdentInt(ddl,0,ExpOptSrcBB_AllDepth);
    widm_ddl_setEntryIdentInt(ddl,1,ExpOptSrcBB_UsedDepth);
    widm_ddl_setEntryIdentInt(ddl,2,ExpOptSrcBB_AllDepth00);
    wid_ilvu_setValDdl_CurrIdInt(ilvu,eo->srcBBKind); 
    self->dw_srcBBKind = ilvu;

  /* paper size */
    msg1 = "Size of output\ndocument";
    Tegs_widget paperLabel = widD_label_nbf(SelfA,msg1,NULL);
    // paper kind
    if ( self->dw_mode==0 ) {
    static char* paperKind_table[] = {
        "figure bounding box (real size)",
        "figure bounding box + zoom",
        "defined by user",
        NULL
    };
    self->dw_paperKind = widm_ddl_newStr(gui,NULL,
            "Select the kind of ouput document",
            paperKind_table, expopt_cb_paperKind, self);
    widmD_ddl_dbf(self->dw_paperKind,self->dw_widEGS);
    if ( eo->paperKind==ExpOptPaKd_SrcBBZ )    widm_ddl_setCurrentEntryIndex(self->dw_paperKind, 1);
    if ( eo->paperKind==ExpOptPaKd_SetByUser ) widm_ddl_setCurrentEntryIndex(self->dw_paperKind, 2);
    }
    // widgets for orientation
    static char* paperOrient_table[] = { "Portrait", "Landscape", "Automatic", NULL };
    msg1 = "orientation" ;
    ilvu = wid_ilvu_newDdlStr(0,paperOrient_table,(Twid_cb_Uap)expopt_cb_paperOrient,NULL);
    wid_ilvu_addLabel(ilvu,msg1,NULL);
    widD_ilvu_dbf(ilvu,SelfA);
    ddl = wid_ilvu_getValDdl(ilvu);
    widm_ddl_setEntryIdentInt(ddl,0,ExpOptOrient_Portrait);
    widm_ddl_setEntryIdentInt(ddl,1,ExpOptOrient_Landscape);
    widm_ddl_setEntryIdentInt(ddl,2,ExpOptOrient_Auto);
    wid_ilvu_setValDdl_CurrIdInt(ilvu,eo->paperOrient); 
    self->dw_paperOrient = ilvu;
    // widgets for border
    double minBD = lu_convToFu(0,ubd);
    double maxBD = lu_convToFu(1000,ubd);
    double defBD = lu_convToFu(eo->paperBdCm,LUNIT_ID_Cm);
    double incBD = lu_convToFu(1,ubd);
    msg1 = "borders" ; //"Borders arround\nthe figure" ;
    self->dw_paperBd = wid_ilvu_newD( defBD, "99999.99", NULL,expopt_cb_paperBdChg,NULL, minBD,maxBD);
    wid_ilvu_addLabel  ( self->dw_paperBd, msg1, 0);
    wid_ilvu_addSpinInc( self->dw_paperBd, incBD );
    wid_ilvu_addUnitStd( self->dw_paperBd );
    widD_ilvu_dbf(self->dw_paperBd,SelfA);
    wid_ilvu_chgUnitStd( self->dw_paperBd, ubd);
    // widgets for paper definition
    self->dw_paperDefTable = astrset_new(10);
    astrset_add(self->dw_paperDefTable,"Custom");
    astrset_add(self->dw_paperDefTable,"Letter");
    astrset_add(self->dw_paperDefTable,"Legal");
    astrset_add(self->dw_paperDefTable,"A6");
    astrset_add(self->dw_paperDefTable,"A5");
    astrset_add(self->dw_paperDefTable,"A4");
    astrset_add(self->dw_paperDefTable,"A3");
    astrset_nullTerminated(self->dw_paperDefTable);
#if 0
{
    int i;
    for (i=1; i<self->dw_paperDefTable->eleNb ; i+=1) {
        char* n = self->dw_paperDefTable->eles[i];
        PERM_asf_printf( tool_paperdef_get(n)==0, "Invalid paper name (n=%s i=%d)",n,i);
    }
}
#endif
    self->dw_paperDef = widm_ddl_newStr(gui,NULL,
            "Select the paper size",
            self->dw_paperDefTable->eles, expopt_cb_paperDef, self);
    widmD_ddl_dbf(self->dw_paperDef,self->dw_widEGS);
    if ( eo->paperDef!=0 ) {
        for (i=0; i<self->dw_paperDefTable->eleNb ; i+=1) {
            if ( strcasecmp(self->dw_paperDefTable->eles[i],eo->paperDef->sname)==0 )
                break;
        }
        PERM_ase_printf(i==self->dw_paperDefTable->eleNb,
            "%s is an unsupported paper type, (switching to custom)",eo->paperDef->sname);
        if ( i==self->dw_paperDefTable->eleNb ) {
            // unsupported paper type, we switch to a custom one
            i = 0;
            tool_paperdef_chgCustomCm(&eo->paperDef, eo->paperDef->wcm, eo->paperDef->hcm);
        }
        widm_ddl_setCurrentEntryIndex(self->dw_paperDef,i);
    }
    // widgets for page matrix
    if ( self->dw_mode==1 ) {
    double minMX = 1;
    double maxMX = 20;
    double incMX = 1;
    double defMXli = eo->nbline<1   ? 1 : eo->nbline;
    double defMXco = eo->nbcolumn<1 ? 1 : eo->nbcolumn;
    ilvu = wid_ilvu_newI( defMXli, "99", NULL,expopt_cb_paperLiChg,NULL, minMX,maxMX);
    wid_ilvu_addLabel  ( ilvu, "page number vertically", 0);
    wid_ilvu_addSpinInc( ilvu, incMX );
    widD_ilvu_dbf(ilvu,SelfA);
    self->dw_paperLi = ilvu;
    ilvu = wid_ilvu_newI( defMXco, "99", NULL,expopt_cb_paperCoChg,NULL, minMX,maxMX);
    wid_ilvu_addLabel  ( ilvu, "page number horizontally", 0);
    wid_ilvu_addSpinInc( ilvu, incMX );
    widD_ilvu_dbf(ilvu,SelfA);
    self->dw_paperCo = ilvu;
    }
    // widgets for custom paper size
    double minP = lu_convToFu(1    , uu);    //  1 cm
    double maxP = lu_convToFu(1000 , uu);    // 10 m
    double incP = lu_convToFu(1    , uu);    //  1 cm
    double dxP  = eo->paperDef==0 ? minP : eo->paperDef->wfu ;
    double dyP  = eo->paperDef==0 ? minP : eo->paperDef->hfu ;
    ilvu = wid_ilvu_newD( dxP, "99999.99", NULL,expopt_cb_paperDxChg,NULL, minP,maxP);
    wid_ilvu_addLabel  ( ilvu, "width", 0);
    wid_ilvu_addSpinInc( ilvu, incP );
#if 0 // FIXME: waiting for wid_ilvu_addUnitStdUU( ilvu, uu );
    wid_ilvu_addUnitStdUU( ilvu, uu );
    widD_ilvu_dbf(ilvu,SelfA);
#else
    wid_ilvu_addUnitStd( ilvu );
    widD_ilvu_dbf(ilvu,SelfA);
    wid_ilvu_disable   ( ilvu);
    wid_ilvu_chgUnitStd( ilvu, uu);
    wid_ilvu_enable    ( ilvu);
#endif
    self->dw_paperDx = ilvu;
    ilvu = wid_ilvu_newD( dyP, "99999.99", NULL,expopt_cb_paperDyChg,NULL, minP,maxP);
    wid_ilvu_addLabel  ( ilvu, "height", 0);
    wid_ilvu_addSpinInc( ilvu, incP );
#if 0 // FIXME: waiting for wid_ilvu_addUnitStdUU( ilvu, uu );
    wid_ilvu_addUnitStdUU( ilvu, uu );
    widD_ilvu_dbf(ilvu,SelfA);
#else
    wid_ilvu_addUnitStd( ilvu );
    widD_ilvu_dbf(ilvu,SelfA);
    wid_ilvu_disable   ( ilvu);
    wid_ilvu_chgUnitStd( ilvu, uu);
    wid_ilvu_enable    ( ilvu);
#endif
    self->dw_paperDy = ilvu;

  /* how to place the figure into the paper */
    msg1 = "Place the figure\ninto the\noutput document";
    Tegs_widget placeLabel = widD_label_nbf(SelfA,msg1,NULL);
    // place kind
    static char* placeKind_table0[] = {
        "zoom to fit",
        "set zoom",
        NULL
    };
    static char* placeKind_table1[] = {
        "zoom to fit",
        "set zoom",
        "set zoom & adjust page matrix",
        NULL
    };
    self->dw_placeKind = widm_ddl_newStr(gui,NULL,
            "Chose how the figure must be drawn on the ouput document",
            self->dw_mode==0 ? placeKind_table0 : placeKind_table1,
            expopt_cb_placeKind, self);
    widmD_ddl_dbf(self->dw_placeKind,self->dw_widEGS);
    widm_ddl_setEntryIdentInt(self->dw_placeKind,0,ExpOptPlace_ZoomToFit);
    widm_ddl_setEntryIdentInt(self->dw_placeKind,1,ExpOptPlace_ZoomUser);
    if ( self->dw_mode==1 )
    widm_ddl_setEntryIdentInt(self->dw_placeKind,2,ExpOptPlace_ZoomUserAPM);
    widm_ddl_setCurrentEntryIdent(self->dw_placeKind,any_initBySInt(eo->placeKind));
    // place Wind Rose
    const char** placeWR_table = tool_dirWR_toStrTable();
    self->dw_placeWR = widm_ddl_newStr(gui,NULL,
            "Chose how the figure must be placed on the ouput document",
            (char**) placeWR_table, expopt_cb_placeWR, self);
    widmD_ddl_dbf(self->dw_placeWR,self->dw_widEGS);
    tool_free( placeWR_table );
    for (i=0 ; i<TOOL_DirWR_NB ;i+=1)
        widm_ddl_setEntryIdentInt(self->dw_placeWR,i,tool_dirWR_table()[i]);
    widm_ddl_setCurrentEntryIdent(self->dw_placeWR,any_initBySInt(eo->placeWhere));
    // place zoom factor
    double minZ =   0.05;
    double maxZ = 100.  ;
    double mulZ =   1.25;
    ilvu = wid_ilvu_newD( 1.0, "9999.99", NULL,expopt_cb_placeZoomChg,NULL, minZ,maxZ);
    wid_ilvu_addLabel  ( ilvu, "zoom factor", 0);
    wid_ilvu_addSpinMul( ilvu, mulZ );
    widD_ilvu_dbf(ilvu,SelfA);
    self->dw_placeZoom = ilvu;

  /* raster image format parameters */
    Tegs_widget rasterLabel = NULL;
    if ( self->dw_mode==0 ) {
    msg1 = "For raster image\nformat";
    rasterLabel = widD_label_nbf(SelfA,msg1,NULL);
    // ppi
    msg1 = "Pixel Per Inch" ;
    int minPPI = 72;
    int maxPPI = 72*1000;
    int defPPI = 72*2;
    int incPPI = 72;
    ilvu = wid_ilvu_newI( defPPI, "99999", NULL,expopt_cb_rasPpi,NULL, minPPI,maxPPI);
    wid_ilvu_addLabel  ( ilvu, msg1, 0);
    wid_ilvu_addSpinInc( ilvu, incPPI );
    wid_ilvu_addUnitLab( ilvu, "ppi", NULL );
    widD_ilvu_dbf      (ilvu,SelfA);
    wid_ilvu_setNum10(ilvu, eo->ppi );
    self->dw_rasPpi = ilvu;
    // quality
    msg1 = "Quality" ;
    int minQLY = 10;
    int maxQLY = 100;
    int defQLY = 100;
    int incQLY = 10;
    ilvu = wid_ilvu_newI( defQLY, "99999", NULL,expopt_cb_rasQlt,NULL, minQLY,maxQLY);
    wid_ilvu_addLabel  ( ilvu, msg1, 0);
    wid_ilvu_addSpinInc( ilvu, incQLY );
    wid_ilvu_addUnitLab( ilvu, "%", NULL );
    widD_ilvu_dbf      (ilvu,SelfA);
    wid_ilvu_setNum10(ilvu, round(100*eo->quality) );
    self->dw_rasQly = ilvu;
    }

    // layout
#define DepthsL    depthsLabel
#define DepthsA    self->dw_btn_allDepth->but_widEGS
#define DepthsS    self->dw_btn_selDepth->but_widEGS
#define SrcBbL     self->dw_srcBBKind->lvu_labEGS
#define SrcBbV     self->dw_srcBBKind->lvu_valEGS
#define PaperL     paperLabel
#define PaperK    (self->dw_mode==0 ? self->dw_paperKind->any.ma_btnEGS : self->dw_paperDef->any.ma_btnEGS)
#define PaperD    (self->dw_mode==0 ? self->dw_paperDef->any.ma_btnEGS  : NULL)
#define BorderL    self->dw_paperBd->lvu_labEGS
#define BorderV    self->dw_paperBd->lvu_valEGS
#define BorderU    self->dw_paperBd->lvu_uniEGS
#define OrientL    self->dw_paperOrient->lvu_labEGS
#define OrientV    self->dw_paperOrient->lvu_valEGS
#define PapNbLiL  (self->dw_mode==0 ? NULL : self->dw_paperLi->lvu_labEGS)
#define PapNbLiV  (self->dw_mode==0 ? NULL : self->dw_paperLi->lvu_valEGS)
#define PapNbCoL  (self->dw_mode==0 ? NULL : self->dw_paperCo->lvu_labEGS)
#define PapNbCoV  (self->dw_mode==0 ? NULL : self->dw_paperCo->lvu_valEGS)
#define WidthL     self->dw_paperDx->lvu_labEGS
#define WidthV     self->dw_paperDx->lvu_valEGS
#define WidthU     self->dw_paperDx->lvu_uniEGS
#define HeightL    self->dw_paperDy->lvu_labEGS
#define HeightV    self->dw_paperDy->lvu_valEGS
#define HeightU    self->dw_paperDy->lvu_uniEGS

#define PlaceL     placeLabel
#define PlaceK     self->dw_placeKind->any.ma_btnEGS
#define PlaceWR    self->dw_placeWR->any.ma_btnEGS
#define PlaceZL    self->dw_placeZoom->lvu_labEGS
#define PlaceZV    self->dw_placeZoom->lvu_valEGS

#define RasterL    rasterLabel
#define RasPpiL   (self->dw_mode==1 ? NULL : self->dw_rasPpi->lvu_labEGS)
#define RasPpiV   (self->dw_mode==1 ? NULL : self->dw_rasPpi->lvu_valEGS)
#define RasPpiU   (self->dw_mode==1 ? NULL : self->dw_rasPpi->lvu_uniEGS)
#define RasQlyL   (self->dw_mode==1 ? NULL : self->dw_rasQly->lvu_labEGS)
#define RasQlyV   (self->dw_mode==1 ? NULL : self->dw_rasQly->lvu_valEGS)
#define RasQlyU   (self->dw_mode==1 ? NULL : self->dw_rasQly->lvu_uniEGS)

#define LN(x) VDWLF_ROW(x)
#define COL(x) x
#define CEH(x) x|VDWLF_EH
#define CEB(x) x|VDWLF_EB
    vgui_dw_finishMTX(SelfA
      ,LN( 0) ,CEB(0),DepthsL  ,COL(1),DepthsA  ,2,DepthsA    ,3,DepthsA
      ,LN( 1) ,CEB(0),DepthsL  ,COL(1),DepthsS  ,2,DepthsS    ,3,DepthsS
      ,LN( 2) ,CEB(0),SrcBbL   ,COL(1),SrcBbV   ,2,SrcBbV     ,3,SrcBbV
      ,LN( 3) ,CEB(0),PaperL   ,COL(1),PaperK   ,2,PaperK     ,3,PaperD
      ,LN( 4) ,CEB(0),PaperL   ,CEH(1),BorderL  ,2,BorderV    ,3,BorderU
      ,LN( 5) ,CEB(0),PaperL   ,CEH(1),OrientL  ,2,OrientV  //,3,OrientV
      ,LN( 6) ,CEB(0),PaperL   ,CEH(1),PapNbLiL ,2,PapNbLiV //,3,PapNbLiU
      ,LN( 7) ,CEB(0),PaperL   ,CEH(1),PapNbCoL ,2,PapNbCoV //,3,PapNbCoU
      ,LN( 8) ,CEB(0),PaperL   ,CEH(1),WidthL   ,2,WidthV     ,3,WidthU
      ,LN( 9) ,CEB(0),PaperL   ,CEH(1),HeightL  ,2,HeightV    ,3,HeightU
      ,LN(10) ,CEB(0),PlaceL   ,COL(1),PlaceK   ,2,PlaceK     ,3,PlaceWR
      ,LN(11) ,CEB(0),PlaceL   ,CEH(1),PlaceZL  ,2,PlaceZV    ,3,NULL
      ,LN(12) ,CEB(0),RasterL  ,CEH(1),RasPpiL  ,2,RasPpiV    ,3,RasPpiU
      ,LN(13) ,CEB(0),RasterL  ,CEH(1),RasQlyL  ,2,RasQlyV    ,3,RasQlyU
      ,-1);
    
    self->dw_updating = -1;
    expopt_act_update(self);

    return self;
}

extern void       ydw_expopt_delete(Tydw_expopt* self)
{
    wid_button_delete   ( self->dw_btn_allDepth  );
    wid_button_delete   ( self->dw_btn_selDepth  );
    wid_ilvu_free       ( self->dw_srcBBKind     );
    widm_ddl_delete     ( self->dw_paperKind     );
    astrset_delete      ( self->dw_paperDefTable );
    widm_ddl_delete     ( self->dw_paperDef      );
    wid_ilvu_free       ( self->dw_paperBd       );
    wid_ilvu_free       ( self->dw_paperOrient   );
    wid_ilvu_free       ( self->dw_paperDx       );
    wid_ilvu_free       ( self->dw_paperDy       );
    wid_ilvu_free       ( self->dw_paperLi       );
    wid_ilvu_free       ( self->dw_paperCo       );
    widm_ddl_delete     ( self->dw_placeKind     );
    widm_ddl_delete     ( self->dw_placeWR       );
    wid_ilvu_free       ( self->dw_placeZoom     );
    wid_ilvu_free       ( self->dw_rasPpi        );
    wid_ilvu_free       ( self->dw_rasQly        );
    yfig_expopt_free    (&self->dw_option        );
    vgui_dw_free        ( SelfA );
}

extern Twid_dialStatus ydw_expopt_run(Tydw_expopt* self) { return vgui_dw_run(SelfA); }

extern CexportOption*  ydw_expopt_get(Tydw_expopt* self) {
    TexportOption* eo = &self->dw_option;

    // srcAllDepths & srcBBKind (uptodate)
    // paperKind (uptodate)
    // paperDef (uptodate)
    // paperOrient (uptodate)
    eo->paperBdCm = LUNIT_FuToCm( wid_ilvu_getNum10(self->dw_paperBd) );
    // placeKind (uptodate)
    eo->placeWhere = widm_ddl_getCurrentEntryIdent(self->dw_placeWR).sint;
    // placeZoom (uptodate)

    // ppi & quality
    if ( self->dw_mode==0 ) {
        eo->ppi     = wid_ilvu_getNum10(self->dw_rasPpi);
        eo->quality = wid_ilvu_getNum10(self->dw_rasQly)/100.;
    } else {
        eo->ppi     = 0;
        eo->quality = 0;
    }
    
    // nbline & nbcolumn
    if ( self->dw_mode==1 ) {
        if ( eo->placeKind==ExpOptPlace_ZoomUserAPM ) {
            eo->nbline=eo->nbcolumn=0;
        } else {
            eo->nbline   = wid_ilvu_getNum10(self->dw_paperLi);
            eo->nbcolumn = wid_ilvu_getNum10(self->dw_paperCo);
        }
    } else {
        eo->nbline=eo->nbcolumn=0;
    }

    return &self->dw_option;
}

/*======================================================================*/
/*= Actions                                                            =*/

static void expopt_act_update0(Tydw_expopt* self)
{
    TexportOption* eo = &self->dw_option;

    // dw_btn_allDepth & dw_btn_selDepth widget
    if ( eo->srcAllDepths ) {
        wid_button_setOn (self->dw_btn_allDepth);
        wid_button_setOff(self->dw_btn_selDepth);
    } else {
        wid_button_setOff(self->dw_btn_allDepth);
        wid_button_setOn (self->dw_btn_selDepth);
    }

    // dw_srcBBKind: always present

    // dw_paperKind & dw_paperBd: always present
    // dw_paperDef & dw_paperOrient & dw_paperDx/Dy
    if ( eo->paperKind==ExpOptPaKd_SetByUser ) {
        widm_ddl_show(self->dw_paperDef);
        wid_ilvu_show(self->dw_paperOrient);
        if ( self->dw_paperDef->m_selectedEntry==0 ) { 
            // custom paper size is set within dw_paperDx/Dy widget
            wid_ilvu_show(self->dw_paperDx);
            wid_ilvu_show(self->dw_paperDy);
        } else {
            wid_ilvu_unshow(self->dw_paperDx);
            wid_ilvu_unshow(self->dw_paperDy);
        }
        // update BD such as 3*bd is not larger than paper size
        double maxBd =  min2(eo->paperDef->wfu,eo->paperDef->hfu)/3.;
        if ( maxBd>LUNIT_PtToFu(1000) ) maxBd=LUNIT_PtToFu(1000);
        wid_ilvu_chgValMax( self->dw_paperBd, maxBd);
    } else {
        widm_ddl_unshow(self->dw_paperDef);
        wid_ilvu_unshow(self->dw_paperOrient);
        wid_ilvu_unshow(self->dw_paperDx);
        wid_ilvu_unshow(self->dw_paperDy);
        // reset BD max to default (in this case the paper size is built by
        // adding borders to the figure.
        wid_ilvu_chgValMax( self->dw_paperBd, LUNIT_PtToFu(1000) );
    }

    // dw_placeKind & dw_placeWR & dw_placeZoom
    if ( eo->paperKind==ExpOptPaKd_SrcBB ) {
        widm_ddl_unshow(self->dw_placeKind);
        widm_ddl_unshow(self->dw_placeWR);
        wid_ilvu_unshow(self->dw_placeZoom);
    } else if ( eo->paperKind==ExpOptPaKd_SrcBBZ ) {
        widm_ddl_unshow(self->dw_placeKind);
        widm_ddl_unshow(self->dw_placeWR);
        wid_ilvu_show  (self->dw_placeZoom);
    } else /* if ( eo->paperKind==ExpOptPaKd_SetByUser ) */ {
        widm_ddl_show(self->dw_placeKind);
        widm_ddl_show(self->dw_placeWR);
        if ( eo->placeKind==ExpOptPlace_ZoomToFit ) {
            wid_ilvu_unshow(self->dw_placeZoom);
            if ( self->dw_mode==1 ) {
                wid_ilvu_show  (self->dw_paperLi);
                wid_ilvu_show  (self->dw_paperCo);
            }
        } else if ( eo->placeKind==ExpOptPlace_ZoomUser ) {
            wid_ilvu_show(self->dw_placeZoom);
            if ( self->dw_mode==1 ) {
                wid_ilvu_show  (self->dw_paperLi);
                wid_ilvu_show  (self->dw_paperCo);
            }
        } else if ( eo->placeKind==ExpOptPlace_ZoomUserAPM ) {
            wid_ilvu_show(self->dw_placeZoom);
            wid_ilvu_unshow(self->dw_paperLi); wid_ilvu_reset(self->dw_paperLi);
            wid_ilvu_unshow(self->dw_paperCo); wid_ilvu_reset(self->dw_paperCo);
        }
    }
}

static void expopt_act_update(Tydw_expopt* self)
{
    if (self->dw_updating==-2) {
        TEMP_spe_printf("%s","unexpected call during creation (return)");
    } else if (self->dw_updating==-1) {
        self->dw_updating = 1;
        expopt_act_update0(self);
        self->dw_updating = 0;
    } else if (self->dw_updating==0) {
        self->dw_updating = 1;
        expopt_act_update0(self);
        self->dw_updating = 0;
        expopt_act_callCB(self);
    } else if (self->dw_updating==1) {
        TEMP_spe_printf("%s","recursive call detected and suppressed");
    }
}

/*======================================================================*/
/*= Callbacks                                                          =*/

static void expopt_cb_btnDepths(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_expopt* self = ud;
    self->dw_option.srcAllDepths = ! self->dw_option.srcAllDepths;
    expopt_act_update(self);
}

static void expopt_cb_paperKind(Twidm_any*m,  Twidm_entry*e)
{
    Tydw_expopt*   self = m->ma_appCBud;
    TexportOption* eo   =&self->dw_option;
    Tyfig_mw*      ymw  =(Tyfig_mw*)vgui_dw_getMw(SelfA);
    int            en = e->me_ident.sint;
TEMP_spe_printf("---> en=%d id=%d",en,e->me_ident.sint);
    switch ( en ) {
      case 0:
        self->dw_option.paperKind = ExpOptPaKd_SrcBB;
        tool_paperdef_deletePt(&eo->paperDef);
        eo->placeZoom = 1;
        break;
      case 1:
        self->dw_option.paperKind = ExpOptPaKd_SrcBBZ;
        tool_paperdef_deletePt(&eo->paperDef);
        eo->placeZoom = 1;
        break;
      default: {
        self->dw_option.paperKind = ExpOptPaKd_SetByUser;
        int idx;
        Tyfig_mw*      ymw  =(Tyfig_mw*)vgui_dw_getMw(SelfA);
        Tyfig_dsds*    dsds = ymw->ymw_dsds;
        Cstr paperDefaultName = dsds->ds_paperSize->sname;
        for ( idx=0 ; idx<self->dw_paperDefTable->eleNb ; idx+=1) {
            if ( strcmp(self->dw_paperDefTable->eles[idx],paperDefaultName)==0 )
                    break;
        }
        if ( idx==self->dw_paperDefTable->eleNb ) {
            TEMP_spe_printf("%s default paper does not exist in ddl, switch to A4", paperDefaultName);
            paperDefaultName= "A4";
            for ( idx=0 ; idx<self->dw_paperDefTable->eleNb ; idx+=1)
                if ( strcmp(self->dw_paperDefTable->eles[idx],paperDefaultName)==0 )
                    break;
            tool_paperdef_chgByName(&eo->paperDef,paperDefaultName);
        } else {
            tool_paperdef_chgClone(&eo->paperDef,dsds->ds_paperSize);
        }
        widm_ddl_setCurrentEntryIndex( self->dw_paperDef, idx);
        break;
        }
    }
    expopt_act_update(self);
}

static void expopt_cb_paperDef(Twidm_any*m,  Twidm_entry*e)
{
    Tydw_expopt*   self = m->ma_appCBud;
    TexportOption* eo   =&self->dw_option;
    int  idx   = e->me_ident.sint;
    Cstr paper = e->me_label;
TEMP_spe_printf("---> en=%d id=%d",idx,e->me_ident.sint);
    if ( idx==0 ) {
        tool_paperdef_chgCustomCm(&eo->paperDef,10,10);
        self->dw_updating = 1;
        wid_ilvu_setNum10(self->dw_paperDx,eo->paperDef->wfu);
        wid_ilvu_setNum10(self->dw_paperDy,eo->paperDef->hfu);
        self->dw_updating = 0;
    } else {
        tool_paperdef_chgByName(&eo->paperDef,paper);
    }
    expopt_act_update(self);
}

// for dw_paperDx/Dy, we do not check if paperKind is ExpOptPaKd_SetByUser
// and paperDef is custom because these widgets are shown and active only
// in this case.
static void expopt_cb_paperDxChg  (Tvgui_dw*vdw, Twid_ilvu* iw, double v)
{
    Tydw_expopt*  self = (Tydw_expopt*)vdw;
    TexportOption*eo   = &self->dw_option;
    if ( self->dw_updating==0 ) {
        tool_paperdef_chgCustomFu(&eo->paperDef,v,eo->paperDef->hfu);
        expopt_act_update(self);
    }
}

static void expopt_cb_paperDyChg  (Tvgui_dw*vdw, Twid_ilvu* iw, double v)
{
    Tydw_expopt*  self = (Tydw_expopt*)vdw;
    TexportOption*eo   = &self->dw_option;
    if ( self->dw_updating==0 ) {
        tool_paperdef_chgCustomFu(&eo->paperDef,eo->paperDef->wfu,v);
        expopt_act_update(self);
    }
}

static void expopt_cb_placeKind(Twidm_any*m,  Twidm_entry*e)
{
    Tydw_expopt*   self = m->ma_appCBud;
    TexportOption* eo   =&self->dw_option;
    eo->placeKind = widm_ddl_getCurrentEntryIdent((Twidm_ddl*)m).sint;
    expopt_act_update(self);
}

/*======================================================================*/
/*= Interface of the TexportOption type.                               =*/
/*= (not really at the right place)                                    =*/

TexportOption  yfig_expopt_initDefault()
{
    TexportOption eo;
    eo.srcAllDepths = 0;
    eo.srcBBKind    = ExpOptSrcBB_AllDepth;
    eo.paperKind    = ExpOptPaKd_SrcBB;
    eo.paperDef     = 0;
    eo.paperOrient  = ExpOptOrient_Portrait;
    eo.paperBdCm    = LUNIT_PtToCm(5);
    eo.placeKind    = ExpOptPlace_ZoomUser;
    eo.placeWhere   = DirWR_CC;
    eo.placeZoom    = 1;
    eo.ppi          = 144;
    eo.quality      = 0.9;
    eo.nbline       = 0;
    eo.nbcolumn     = 0;
    return eo;
}

TexportOption  yfig_expopt_initClone(CexportOption* src)
{
    TexportOption eo;
    eo = *src;
    if ( eo.paperDef!=0 ) eo.paperDef = tool_paperdef_newClone(eo.paperDef);
    return eo;
}

void           yfig_expopt_free       (TexportOption* eo)
{ tool_paperdef_deletePt(&eo->paperDef); }

TexportOption* yfig_expopt_newDefault ()
{
    TexportOption* eo = tool_malloc( sizeof(*eo) );
    yfig_expopt_initDefault(eo);
    return eo;
}
TexportOption* yfig_expopt_newClone   (CexportOption* src)
{
    TexportOption* eo = tool_malloc( sizeof(*eo) );
    *eo = yfig_expopt_initClone(src);
    if ( eo->paperDef!=0 ) eo->paperDef = tool_paperdef_newClone(eo->paperDef);
    return eo;
}
void           yfig_expopt_delete     (TexportOption* eo)
{
    yfig_expopt_free(eo);
    tool_free( eo );
}

void           yfig_expopt_cpy         (TexportOption* eo, CexportOption* src)
{
    if ( eo->paperDef!=0 && eo->paperDef!=src->paperDef) tool_paperdef_delete(eo->paperDef);
    *eo = *src;
    if ( eo->paperDef!=0 ) eo->paperDef = tool_paperdef_newClone(eo->paperDef);
}

/*======================================================================*/
