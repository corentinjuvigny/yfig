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
 *$Software: YFIG-DIAL (popup DIALog for YFIG gui)
 *      $Id: ydw-export.c 387 2019-01-26 22:53:16Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-export.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Top-dialog that export the current figure under usual image format.=*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation                       =*/
/*======================================================================*/

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
#include "vgui/wid-egs.h"
#include "vgui/wid-filebrowser.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"

#include "outdev/outdevPdf.h"
#include "tools/globalvars.h"
#include "tools/file.h"

/*======================================================================*/
/*= Configuration                                                      =*/

#define WITH_AutoGenMode 0

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Tydw_export {
    Tvgui_dw          any;

    // widgets
    Twid_ilvu*        dw_expFormat;  // export format
    Twid_olvu*        dw_prtFormat;  // print format (PDF)
    Twid_olvu*        dw_figNWxy;
    Twid_olvu*        dw_figDxDy;
    Twid_olvu*        dw_paperBd;
    Tegs_widget       dw_paperOrient;
    Twid_olvu*        dw_paperSize;
    Tegs_widget       dw_paperDef;
    Twid_olvu*        dw_devSize;      // export: pixel size ; print: device
    Tegs_widget       dw_devUnit;      // export: ppi        ; print: # pages
    Tegs_widget       dw_devFilled;    // # % filled
    Twid_olvu*        dw_devFigSize;   // export: pixel size ; print: device
    Tegs_widget       dw_devFigZoom;   // at zoom #
    Tegs_widget       dw_devFigDrawn;  // # % drawn
    Twid_filebrowser* dw_browser;

    // private data
    int               dw_mode;  // 0:export ; 1:print
    TimgFmtKd         dw_expKind;
    TexportOption     dw_expopt;
    Tscdepth          dw_depths[FIG_MAX_DEPTH]; // org depths of current figure
    Tfig_scene*       dw_scene;

    TstrEaten         dw_tmpPdfPath;
#if WITH_AutoGenMode
    int               dw_autoGenMode; // 1: generate output file at each option change.
#endif
};

// used formats
static int outFormatIds[] = { 
    IMGFMT_PDF, IMGFMT_JPG, IMGFMT_PNG, IMGFMT_TIF, IMGFMT_SVG
};
#define outFormatIdNb TOOL_TABLE_NbEle(outFormatIds)

/*======================================================================*/
/*= Declaration of private utilities.                                  =*/

static void     export_getPaperSizeCm(Tydw_export*self, double*dx, double*dy);
static void     export_getPaperSizeFu(Tydw_export*self, double*dx, double*dy);
static TboxDble export_getSrcBBFu    (Tydw_export*self);
static TboxDble export_getSrcBBCm    (Tydw_export*self);

// actions
static void export_act_update_paper(Tydw_export* self);   // redraw dw_paperSize widget & dw_paperDef
static void export_act_update_devUnit(Tydw_export* self); // redraw the dw_devUnit label
static void export_act_update_devOthers(Tydw_export* self); // redraw dw_devFilled, dw_devFigSize, dw_devFigDrawn
static void export_act_update (Tydw_export* self);        // redraw the widget
static void export_act_preview(Tvgui_dw* vdw, void* ud);  // preview the export

// callbacks 
static void export_cb_outFmt     (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void export_cb_btnOptions (Tvgui_dw*vdw, int btnId, void*ud);
#if WITH_AutoGenMode
static void export_cb_btnOptions2(Tvgui_dw*vdw, int btnId, void*ud);
#endif

/*======================================================================*/
/*= Interface                                                          =*/

extern Tydw_export* ydw_export_nbf(Tyfig_mw* ymw, int flag)
{
    Tvgui*      gui  = ymw->ymw_gui;
    Tyfig_dsds* dsds = ymw->ymw_dsds;

    int         uu    = ymw->ymw_userUnit.uu_id;
    char*       title = flag==0 ? "Export the figure to usual image formats"
                                : "Print the current figure";
    char*       btnOkLab = flag==0 ? "Export" : "Print";
    Twid_ilvu*  ilvu;
    Twid_olvu*  olvu;
    int         i;
    double      dx,dy;
    TboxDble    box;
    Cstr        msg1,msg2;

    Tydw_export* self = (Tydw_export*) vgui_dw_newTop(sizeof(*self),
            &ymw->any, tool_strdup(title),
            0
    );

    self->dw_mode    = flag!=0;
    self->dw_expKind = self->dw_mode ? IMGFMT_PDF : dsds->ds_expFmt->kind;
    self->dw_expopt  = yfig_expopt_initClone(self->dw_mode ? &dsds->ds_prtOpt : &dsds->ds_expOpt);
    fig_scene_copyDepthToT( yfig_cursce_scene(ymw), self->dw_depths);
    self->dw_scene   = fig_scene_newByFig( fig_new_clone(yfig_cursce_fig(ymw)) );
    fig_scene_initDepthByT(self->dw_scene,self->dw_depths);
//obj_dump( fig_scene_getTopObj(self->dw_scene), 0);
//printf("wid: 50:%d 51:%d 52:%d\n", self->dw_scene->sc_depths.table[50], self->dw_scene->sc_depths.table[51], self->dw_scene->sc_depths.table[52]);
//printf("cur: 50:%d 51:%d 52:%d\n", yfig_cursce_scene(ymw)->sc_depths.table[50], yfig_cursce_scene(ymw)->sc_depths.table[51], yfig_cursce_scene(ymw)->sc_depths.table[52]);

  /* selection of output image format */
    msg1 = "Output format";
    if ( self->dw_mode==0 ) {
        char* outFormatNames [outFormatIdNb+1];
        for (i=0;i<outFormatIdNb;i+=1)
            outFormatNames[i]=tool_imgfmt_getByKd( outFormatIds[i] )->shortname;
        outFormatNames[i]=NULL;
        ilvu = wid_ilvu_newDdlStr(0,outFormatNames,(Twid_cb_Uap)export_cb_outFmt,self);
        wid_ilvu_addLabel(ilvu,msg1,NULL);
        widD_ilvu_dbf(ilvu,SelfA);
        Twidm_ddl* ddl = wid_ilvu_getValDdl(ilvu);
        for (i=0 ; i<outFormatIdNb ; i++) {
            widm_ddl_setEntryIdentInt(ddl,i,outFormatIds[i]);
            widm_ddl_setEntryHelp (ddl,i,tool_imgfmt_getByKd( outFormatIds[i])->fullname);
        }
        wid_ilvu_setValDdl_CurrIdInt(ilvu,self->dw_expKind); 
        self->dw_expFormat = ilvu;
    } else {
        msg2 = tool_imgfmt_getByKd( IMGFMT_PDF )->shortname;
        olvu = wid_olvu_newT(msg2,NULL);
        wid_olvu_addLabel(olvu,msg1,NULL);
        widD_olvu_dbf(olvu,SelfA);
        self->dw_prtFormat = olvu;
    }

  /* figure bounding box (dw_figNWxy & dw_figDxDy) */
    dx = 0; dy = 0;
    self->dw_figNWxy = widD_olvu_nbfN11TU(SelfA, dx, "9999.99", dy, "Figure origin", NULL );
    wid_olvu_chgUnitStd( self->dw_figNWxy, uu);
    self->dw_figDxDy = widD_olvu_nbfN11TU(SelfA, dx, "9999.99", dy, "Figure size"  , NULL );
    wid_olvu_chgUnitStd( self->dw_figDxDy, uu);

  /* paper border, size, def & orientation */
    olvu = widD_olvu_nbfN11TU(SelfA,  0, "9999.99",  0, "border depth" , NULL );
    wid_olvu_chgUnitStd( olvu, uu);
    self->dw_paperBd = olvu;
    olvu = widD_olvu_nbfN11TU(SelfA,  0, "9999.99",  0, "paper size" , NULL );
    wid_olvu_chgUnitStd( olvu, uu);
    self->dw_paperSize   = olvu;
    self->dw_paperDef    = widD_label_nbf(SelfA," Custom "   ,NULL);
    self->dw_paperOrient = widD_label_nbf(SelfA," Landscape ",NULL);

  /* output device size and unit */
    if ( self->dw_mode==0 ) {
        olvu = wid_olvu_newN11(0, "9999"   ,  0);
        wid_olvu_addLabel(olvu,"Document size",NULL);
        wid_olvu_addUnitLab(olvu,"pixel",NULL);
        widD_olvu_dbf(olvu,SelfA);
        self->dw_devSize = olvu;
    } else {
        dx = 0; dy = 0;
        olvu = widD_olvu_nbfN11TU(SelfA, dx, "9999.99", dy, "Document size", NULL );
        wid_olvu_chgUnitStd( olvu, uu);
        self->dw_devSize = olvu;
    }
    self->dw_devUnit = widD_label_nbf(SelfA,NULL,NULL);
    export_act_update_devUnit(self);

  /* output figure size with its information */
    if ( self->dw_mode==0 ) {
        olvu = wid_olvu_newN11(0, "9999"   ,  0);
        wid_olvu_addLabel(olvu,"Image size",NULL);
        wid_olvu_addUnitLab(olvu,"pixel",NULL);
        widD_olvu_dbf(olvu,SelfA);
        self->dw_devFigSize = olvu;
    } else {
        dx = 0; dy = 0;
        olvu = widD_olvu_nbfN11TU(SelfA, dx, "9999.99", dy, "Drawn figure", NULL );
        wid_olvu_chgUnitStd( olvu, uu);
        self->dw_devFigSize = olvu;
    }
    self->dw_devFilled   = widD_label_nbf(SelfA,NULL,NULL);
    self->dw_devFigZoom  = widD_label_nbf(SelfA,NULL,NULL);
    self->dw_devFigDrawn = widD_label_nbf(SelfA,NULL,NULL);
    export_act_update_devOthers(self);

  /* file browser widget */
    TstrEaten dir,bn;
    dir  = tool_basenameNoExt( self->dw_scene->sc_dessin->path );
    bn   = tool_aprintf("%s.%s",dir,tool_imgfmt_getExtendByKd(self->dw_expKind));
    tool_free(dir);
    dir = tool_strdup( ymw->ymw_expDir );
    if ( dir==0 ) dir = tool_dirname( self->dw_scene->sc_dessin->path );
    self->dw_browser = wid_filebrowser_new(SelfA,0,0,dir);
    vgui_filebrowser_initFileMaskT(self->dw_browser,TOOL_TABLE_ARG(outFormatIds));
    self->dw_browser->okText = tool_strdup(btnOkLab);
    self->dw_browser->addBtns = 0;
    wid_filebrowser_dbf( self->dw_browser );
    self->dw_browser->addBtns = 1;
    vgui_filebrowser_setBasName( self->dw_browser, bn);
    tool_free( dir );
    tool_free( bn );

  /* add buttons to the dialog footer */
    Twid_button* btn;
    vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_CancelX,NULL);
    btn = vgui_dw_addBtnStdLab(SelfA,
            "Change options and parameters",
            export_cb_btnOptions, "Options",NULL);
#if WITH_AutoGenMode
    btn = vgui_dw_addBtnStdLab(SelfA,
            "Generate the PDF file of figure,\n"
            "change options and parameters and\n"
            "regenerate the PDF file at option change.",
            export_cb_btnOptions2, "Options & Preview",NULL);
#endif
    vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_Ok,btnOkLab);
    
    // layout
#define OutFmtL    (self->dw_mode==0 ? self->dw_expFormat->lvu_labEGS : self->dw_prtFormat->lvu_labEGS)
#define OutFmtV    (self->dw_mode==0 ? self->dw_expFormat->lvu_valEGS : self->dw_prtFormat->lvu_valEGS)
#define FigNWxyL    self->dw_figNWxy->lvu_labEGS
#define FigNWxyV    self->dw_figNWxy->lvu_valEGS
#define FigNWxyU    self->dw_figNWxy->lvu_uniEGS
#define FigDxDyL    self->dw_figDxDy->lvu_labEGS
#define FigDxDyV    self->dw_figDxDy->lvu_valEGS
#define FigDxDyU    self->dw_figDxDy->lvu_uniEGS
#define PapBdL      self->dw_paperBd->lvu_labEGS
#define PapBdV      self->dw_paperBd->lvu_valEGS
#define PapBdU      self->dw_paperBd->lvu_uniEGS
#define PapOriL     self->dw_paperOrient
#define PapSzL      self->dw_paperSize->lvu_labEGS
#define PapSzV      self->dw_paperSize->lvu_valEGS
#define PapSzU      self->dw_paperSize->lvu_uniEGS
#define PapDefL     self->dw_paperDef
#define DevSizeL    self->dw_devSize->lvu_labEGS
#define DevSizeV    self->dw_devSize->lvu_valEGS
#define DevSizeU    self->dw_devSize->lvu_uniEGS
#define DevUnitL    self->dw_devUnit
#define DevFigSzL   self->dw_devFigSize->lvu_labEGS
#define DevFigSzV   self->dw_devFigSize->lvu_valEGS
#define DevFigSzU   self->dw_devFigSize->lvu_uniEGS
#define DevFillL    self->dw_devFilled
#define DevFigZoL   self->dw_devFigZoom
#define DevFigDrL   self->dw_devFigDrawn
#define Browser     self->dw_browser->widEGS

#define LN(x) VDWLF_ROW(x)
#define COL(x) x
#define CEH(x) x|VDWLF_EH
    vgui_dw_finishMTX(SelfA
    ,LN(0) ,COL(0),OutFmtL   ,COL(1),OutFmtV
    ,LN(1) ,COL(0),FigNWxyL  ,COL(1),FigNWxyV  ,COL(2),FigNWxyU
    ,LN(2) ,COL(0),FigDxDyL  ,COL(1),FigDxDyV  ,COL(2),FigDxDyU
    ,LN(3) ,COL(0),PapSzL    ,COL(1),PapSzV    ,COL(2),PapSzU    ,COL(3),PapDefL
    ,LN(4) ,COL(0),PapBdL    ,COL(1),PapBdV    ,COL(2),PapBdU    ,COL(3),PapOriL
    ,LN(5) ,COL(0),DevSizeL  ,COL(1),DevSizeV  ,COL(2),DevSizeU  ,COL(3),DevUnitL  ,COL(4),DevFillL
    ,LN(6) ,COL(0),DevFigSzL ,COL(1),DevFigSzV ,COL(2),DevFigSzU ,COL(3),DevFigZoL ,COL(4),DevFigDrL
    ,LN(7) ,CEH(0),Browser   ,CEH(1),Browser   ,CEH(2),Browser   ,CEH(3),Browser   ,CEH(4),Browser
    ,-1);

    export_act_update(self);
    return self;
}

extern void       ydw_export_delete(Tydw_export* self)
{
    wid_ilvu_free          ( self->dw_expFormat );
    wid_olvu_free          ( self->dw_prtFormat );
    wid_olvu_free          ( self->dw_figNWxy );
    wid_olvu_free          ( self->dw_figDxDy );
    wid_olvu_free          ( self->dw_paperBd );
  //label                  ( self->dw_paperOrient  );
    wid_olvu_free          ( self->dw_paperSize );
  //label                  ( self->dw_paperDef  );
                          
    wid_olvu_free          ( self->dw_devSize );
  // label                 ( self->dw_devUnit );
  // label                 ( self->dw_devFilled );
    wid_olvu_free          ( self->dw_devFigSize );
  // label                 ( self->dw_devFigZoom );
  // label                 ( self->dw_devFigDrawn);

    wid_filebrowser_delete ( self->dw_browser );

    yfig_expopt_free       (&self->dw_expopt  );
    fig_scene_delete       ( self->dw_scene   );

    if ( self->dw_tmpPdfPath ) {
        unlink             ( self->dw_tmpPdfPath );
        tool_free          ( self->dw_tmpPdfPath );
    }
// FIXME if ( self->dw_evincePid ) kill( self->dw_evincePid, SIGTERM );
    vgui_dw_free           ( SelfA );
}

extern Twid_dialStatus ydw_export_run(Tydw_export* self) { return vgui_dw_run(SelfA); }

extern void            ydw_export_updateOption(Tydw_export* self)
{
    Tyfig_mw*   ymw  = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tyfig_dsds* dsds = ymw->ymw_dsds;
    if ( self->dw_mode==0 )
        yfig_expopt_cpy(&dsds->ds_expOpt, &self->dw_expopt);
    else
        yfig_expopt_cpy(&dsds->ds_prtOpt, &self->dw_expopt);
    TstrEaten dir = vgui_filebrowser_getDirPath(self->dw_browser);
    if ( ! TOOL_strIsEmpty(dir) ) tool_strchgEaten(&ymw->ymw_expDir,dir);
}

static TodPdfCfg ydw_export_PdfCfg(Tydw_export* self, CboxDble* srcBoxCm);

extern int ydw_export_do (Tydw_export* self)
{
  /* Sanity check */
    TstrEaten  outPath = vgui_filebrowser_getFilPath( self->dw_browser );
    if ( outPath==0 )
        return 1;

  /* update scene depths */
    if ( self->dw_expopt.srcAllDepths!=0 )
        fig_scene_enableAllDepths(self->dw_scene);
    else
        fig_scene_initDepthByT   (self->dw_scene,self->dw_depths);
  /* creation of figure into a temporary file */
    int status;
    FILE* stream;
    if ( self->dw_tmpPdfPath==0 )
        stream = tool_tempfile(&self->dw_tmpPdfPath);
    else
        stream = fopen(self->dw_tmpPdfPath,"w+");
    if ( stream==NULL ) {
        vgui_dw_errerr_printf(SelfA,
            "export aborted, unable to create or open a temporary file: %s", strerror(errno));
        tool_free( outPath );
        return 1;
    }
    TboxDble  srcBoxCm = export_getSrcBBCm(self);
    TodPdfCfg cfg      = ydw_export_PdfCfg(self,&srcBoxCm);
    
    //Handling of svg case
    status = self->dw_expKind==IMGFMT_SVG 
             ? fig_scene_svg(self->dw_scene,vgui_dw_getMsgDrv(SelfA),stream,&cfg,srcBoxCm)
             : fig_scene_pdf(self->dw_scene,vgui_dw_getMsgDrv(SelfA),stream,&cfg,srcBoxCm);
    fclose(stream);
    if ( status!=1 ) {
        vgui_dw_errerr_printf(SelfA,"export aborted: PDF generation of figure failed");
        tool_free( outPath );
        return 1;
    }

  /* convert the PDF temporary file into the expected format */
    CstrStatic pdf = tool_imgfmt_getShNameByKd( IMGFMT_PDF );
    CstrStatic fmt = tool_imgfmt_getShNameByKd( self->dw_expKind );
    if ( self->dw_expKind==IMGFMT_PDF || self->dw_expKind==IMGFMT_SVG ) {
        // just rename file
        if ( tool_fileCopy(vgui_dw_getMsgDrv(SelfA),self->dw_tmpPdfPath,outPath)!=0 ) {
            vgui_dw_errerr_printf(SelfA,
                "export aborted: failed to create %s file: %s",
                 outPath,strerror(errno));
        } else {
            goto export_done;
        }
    } else {
        if ( tool_imgfmt_convIM(vgui_dw_getMsgDrv(SelfA),
                    self->dw_tmpPdfPath,IMGFMT_PDF,
                    outPath,            self->dw_expKind,
                    self->dw_expopt.ppi,self->dw_expopt.quality)!=0 )
            vgui_dw_errerr_printf(SelfA,
                "export aborted: failed to convert %s format into %s format",
                 pdf,fmt);
        else
            goto export_done;
    }
    tool_free( outPath );
    return 1;

export_done:
    vgui_dw_inferr_printf(SelfA,
        "export done: to %s format into file: %s",fmt,outPath);
    tool_free( outPath );
    return 0;
}

/*======================================================================*/
/*= Actions                                                            =*/

static void export_act_update_devUnit(Tydw_export* self) // redraw the dw_devUnit label
{
    TexportOption* eo    = &self->dw_expopt;
    char tmp[100];
    if ( self->dw_mode==0 )
        sprintf(tmp,"at %d ppi",eo->ppi);
    else {
        TboxDble  srcBoxCm = export_getSrcBBCm(self);
        TodPdfCfg cfg      = ydw_export_PdfCfg(self,&srcBoxCm);
        int pgnb = (cfg.cfg_mode&OD_PDFCFG_1Page)!=0 ? 1 : cfg.cfg_npLine*cfg.cfg_npColumn;
        sprintf(tmp,"on %d page%s",pgnb,pgnb>1 ? "s" : "");
    }
    wid_label_setTextAS(self->dw_devUnit,tmp);
}

static void export_act_update_devOthers(Tydw_export* self)
{
    TexportOption* eo  = &self->dw_expopt;
    TboxDble  srcBoxCm = export_getSrcBBCm(self);
    TodPdfCfg cfg      = ydw_export_PdfCfg(self,&srcBoxCm);

    // get daDx/Dy the drawing area of document taking in account the orientation
    // get zoom factor and picDx/Dy the size of picture in the document 
    double daDx  = odpdf_cfg_getDocDa(&cfg).x;
    double daDy  = odpdf_cfg_getDocDa(&cfg).y;
    double zoom  = odpdf_cfg_getZomm(&cfg,Box_dx(srcBoxCm),Box_dy(srcBoxCm));
    double picDx = zoom* Box_dx(srcBoxCm);
    double picDy = zoom* Box_dy(srcBoxCm);
    double trunc,unfill;
    double picArea = picDx*picDy;
    double daArea  =  daDx* daDy;
    if ( picDx<=daDx && picDy<=daDy ) {
        // xxx++++++++++++
        // xxx           +
        // +             +
        // +++++++++++++++
        trunc  = 0;
        unfill = (daArea-picArea)/daArea;
    } else if ( picDx<=daDx && picDy>daDy ) {
        // xxx++++++++++++
        // xxx           +
        // xxx           +
        // xxx++++++++++++
        // xxx
        trunc  = (picDx * (picDy-daDy)) / picArea;
        unfill = (daArea - picDx*daDy ) / daArea;
    } else if ( picDx>daDx && picDy<=daDy) {
        // xxxxxxxxxxxxxxxxxx
        // xxxxxxxxxxxxxxxxxx
        // +             +
        // +++++++++++++++
        trunc  = (picDy  * (picDx-daDx)) / picArea;
        unfill = (daArea - picDy*daDx  ) / daArea;
    } else /* if ( picDx>daDx && picDy>daDy) */ {
        // xxx++++++++++++xxx
        // xxxxxxxxxxxxxx+xxx
        // xxxxxxxxxxxxxx+xxx
        // xxx++++++++++++xxx
        // xxxxxxxxxxxxxxxxxx
        trunc = (picArea-daArea)/picArea;
        unfill  = 0;
    }

    CimgFmtDt*fmtdt = tool_imgfmt_getByKd( self->dw_expKind );
    if ( self->dw_mode==0 && fmtdt->raster==1 ) {
        double dx = round( LUNIT_CmToIn(picDx)*eo->ppi );
        double dy = round( LUNIT_CmToIn(picDy)*eo->ppi );
        wid_olvu_setNum11(self->dw_devFigSize,dx,dy);
    } else
        wid_olvu_setNum11(self->dw_devFigSize,LUNIT_CmToFu(picDx),LUNIT_CmToFu(picDy));

    char tmp[100];
    sprintf(tmp,"(%3.1f %% drawn)"  ,(1.-trunc) *100.); wid_label_setTextAS(self->dw_devFigDrawn,tmp);
    sprintf(tmp,"(%3.1f %% filled)" ,(1.-unfill)*100.); wid_label_setTextAS(self->dw_devFilled  ,tmp);
    if ( zoom<0.006 ) sprintf(tmp,"at zoom %1.4f", zoom);
    else              sprintf(tmp,"at zoom %5.2f", zoom);
    wid_label_setTextAS(self->dw_devFigZoom,tmp);
}

// redraw dw_paperBd, dw_paperSize, dw_paperDef, dw_paperOrient
static void export_act_update_paper(Tydw_export* self)
{
    TexportOption* eo    = &self->dw_expopt;
    double dx,dy;

    export_getPaperSizeFu(self,&dx,&dy);
    wid_olvu_setNum10(self->dw_paperBd,  LUNIT_CmToFu(eo->paperBdCm));
    wid_olvu_setNum11(self->dw_paperSize,dx,dy);

    if ( eo->paperKind==ExpOptPaKd_SetByUser ) {
        Cstr def = eo->paperDef->sname;
        if ( tool_paperdef_get( def )!=0 ) {
            char tmp[100];
            sprintf(tmp,"(%s)",def);
            wid_label_setTextAS(self->dw_paperDef,tmp);
            gui_widegs_show( self->dw_paperDef );
        } else
            gui_widegs_unshow( self->dw_paperDef );
    } else {
        gui_widegs_unshow( self->dw_paperDef );
    }

    TexportOptionOrient paperOrient = eo->paperOrient;
    if ( paperOrient==ExpOptOrient_Auto ) {
        TboxDble  srcBoxCm = export_getSrcBBCm(self);
        TodPdfCfg cfg      = ydw_export_PdfCfg(self,&srcBoxCm);
        paperOrient = (cfg.cfg_mode&OD_PDFCFG_Land)==0 ? ExpOptOrient_Portrait : ExpOptOrient_Landscape;
    }
    wid_label_setTextAS(self->dw_paperOrient,paperOrient==ExpOptOrient_Landscape ? " landscape " : " portrait ");
}

static void export_act_update(Tydw_export* self)
{
    Tyfig_mw*      ymw   = (Tyfig_mw*)vgui_dw_getMw(SelfA);
    Tyfig_dsds*    dsds  = ymw->ymw_dsds;
    Tfig_scene*    sc    = self->dw_scene;
    TexportOption* eo    = &self->dw_expopt;
    TimgFmtKd      fmtkd = self->dw_expKind;
    CimgFmtDt*     fmtdt = tool_imgfmt_getByKd( fmtkd );
    TboxDble box; double dx,dy;

  /* update dw_figNWxy and dw_figDxDy */
    box = export_getSrcBBFu(self);
    wid_olvu_setNum11(self->dw_figNWxy,Box_x (box),Box_y (box));
    wid_olvu_setNum11(self->dw_figDxDy,Box_dx(box),Box_dy(box));

  /* update dw_paperBd, dw_paperSize, dw_paperDef */
    export_act_update_paper(self);

  /* update dw_devSize */
    if ( self->dw_mode==0 && fmtdt->raster==1 ) {
        export_getPaperSizeFu(self,&dx,&dy);
        dx = round( lu_convFromFu(dx,LUNIT_ID_Inch)*eo->ppi );
        dy = round( lu_convFromFu(dy,LUNIT_ID_Inch)*eo->ppi );
        wid_olvu_setNum11(self->dw_devSize,dx,dy);
        export_act_update_devUnit(self);
        wid_olvu_show  (self->dw_devSize);
        gui_widegs_show(self->dw_devUnit);
    } else {
        TboxDble  srcBoxCm = export_getSrcBBCm(self);
        TodPdfCfg cfg      = ydw_export_PdfCfg(self,&srcBoxCm);
        // daDx/Dy: the drawing area of document taking into account the orientation
        double daDx  = odpdf_cfg_getDocDa(&cfg).x;
        double daDy  = odpdf_cfg_getDocDa(&cfg).y;
        if ( (cfg.cfg_mode&OD_PDFCFG_Land)!=0 ) {
            dx=daDx; daDx=daDy; daDy=dx;
        }
        wid_olvu_setNum11(self->dw_devSize,LUNIT_CmToFu(daDx),LUNIT_CmToFu(daDy));
        export_act_update_devUnit(self);
    }

  /* update dw_devFilled, dw_devFigSize, dw_devFigDrawn */
    export_act_update_devOthers(self);
}

static void export_act_preview(Tvgui_dw* vdw, void* ud)
{
    Tydw_expopt* dialog = (Tydw_expopt*)vdw;
    Tydw_export* self = ud;

    yfig_expopt_cpy(&self->dw_expopt, ydw_expopt_get(dialog));
    export_act_update(self);

#if WITH_AutoGenMode
    if ( self->dw_autoGenMode ) ydw_export_do(self);
#endif
}

/*======================================================================*/
/*= Callbacks                                                          =*/

static void export_cb_outFmt(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_export* self = (Tydw_export*)dw;
    TimgFmtKd expkd = wid_ilvu_getValDdl_CurrIdAny(iw).uint;
    if ( expkd==self->dw_expKind ) return;

    // get extension of current basename
    Twid_filebrowser* fbw = self->dw_browser;
    TstrEaten oldbsn = vgui_filebrowser_getBasName( fbw );
    TstrEaten oldext = tool_basenameExt( oldbsn );
    if ( TOOL_strIsEmpty(oldext) ) {
free_1:
        tool_free( oldbsn );
        tool_free( oldext );
        self->dw_expKind = expkd;
        export_act_update(self);
        return;
    }
    // if the extension does not correspond to old file format,
    // user changed it, so we keep it.
    if ( ! tool_imgfmt_isExtOfKd(self->dw_expKind,oldext) ) goto free_1;

    // update file bowser
    TstrEaten oldbsnNoExt = tool_basenameNoExt(oldbsn);
    TstrEaten newbsn = tool_aprintf("%s.%s",oldbsnNoExt,tool_imgfmt_getExtendByKd(expkd));
    vgui_filebrowser_setBasName( fbw, newbsn);
    tool_free(oldbsnNoExt);
    tool_free(newbsn);
    goto free_1;
}

static void export_cb_btnOptions (Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_export*    self = (Tydw_export*)vdw;
    Tyfig_mw*       ymw  = (Tyfig_mw*)vgui_dw_getMw(vdw);
    TexportOption*  eo   = &self->dw_expopt;
    TexportOption   save = yfig_expopt_initClone(eo);
    Tydw_expopt*    dialog = ydw_expopt_nbf(ymw,eo,self->dw_mode,export_act_preview,self);
    Twid_dialStatus status = ydw_expopt_run(dialog);

    if ( status==WID_DIALSTATUS_Ok ) {
        yfig_expopt_free(&save);
        yfig_expopt_cpy(eo,ydw_expopt_get(dialog));
    } else {
        yfig_expopt_free(&self->dw_expopt);
        self->dw_expopt = save;
    }
    export_act_update(self);
    ydw_expopt_delete(dialog);
}

#if WITH_AutoGenMode
static void export_cb_btnOptions2 (Tvgui_dw*vdw, int btnId, void*ud)
{
    Tydw_export* self = (Tydw_export*)vdw;
    self->dw_autoGenMode = 1;
    ydw_export_do(self);
    export_cb_btnOptions(vdw,btnId,ud);
    self->dw_autoGenMode = 0;
}
#endif

/*======================================================================*/
/*======================================================================*/
/*= Private utilities.                                                 =*/

static void     export_getPaperSizeCm(Tydw_export*self, double*dx, double*dy)
{
    TboxDble       box;
    Tfig_scene*    sc   = self->dw_scene;
    TexportOption* eo   = &self->dw_expopt;

    switch ( eo->paperKind ) {
        case ExpOptPaKd_SrcBB:
alldepth:
            eo->placeZoom = 1;
        case ExpOptPaKd_SrcBBZ:
            box = export_getSrcBBCm(self);
            *dx = eo->placeZoom*Box_dx(box) + 2*eo->paperBdCm;
            *dy = eo->placeZoom*Box_dy(box) + 2*eo->paperBdCm;
            break;
        case ExpOptPaKd_SetByUser:
            *dx  = eo->paperDef->wcm;
            *dy  = eo->paperDef->hcm;
            break;
        default:
            PERM_spf_printf("invalid paper kind",eo->paperKind);
            goto alldepth;
    }
}
static void     export_getPaperSizeFu(Tydw_export*self, double*dx, double*dy)
{
    export_getPaperSizeCm(self,dx,dy);
    *dx = LUNIT_CmToFu(*dx);
    *dy = LUNIT_CmToFu(*dy);
}

static TboxDble export_getSrcBB    (Tydw_export*self, int uu)
{
    Tfig_scene*    sc   = self->dw_scene;
    TboxDble       box;

    if ( self->dw_expopt.srcBBKind==ExpOptSrcBB_UsedDepth )
        box = fig_scene_BBoxTopSelU(sc,uu);
    else {
        box = fig_scene_BBoxTopAllU(sc,uu);
        if ( self->dw_expopt.srcBBKind==ExpOptSrcBB_AllDepth00 ) {
            if ( Box_x(box) > 0 ) { Box_dx(box) += Box_x(box); Box_x(box)=0; }
            if ( Box_y(box) > 0 ) { Box_dy(box) += Box_y(box); Box_y(box)=0; }
        }
    }
    return box;
}
static TboxDble export_getSrcBBFu  (Tydw_export*self) { return export_getSrcBB(self,LUNIT_ID_FU); }
static TboxDble export_getSrcBBCm  (Tydw_export*self) { return export_getSrcBB(self,LUNIT_ID_Cm); }

/*======================================================================*/
/*= PDF driver                                                         =*/

// Initializes a PDF configuration using self->dw_option and srcBBKind and
// returns it.
// It sets *zoom to the zoom factor that is set explicitly or
// implicitly in the PDF configuration.
// The function assumes that the paper orientation of self->dw_option
// is either portrait or landscape (NOT AUTOMATIC).
static TodPdfCfg ydw_export_PdfCfg_priv(Tydw_export* self, CboxDble* srcBoxCm, double*_zoom)
{
    TexportOption* eo = &self->dw_expopt;

    // get paper size
    double paperDx,paperDy;
    export_getPaperSizeCm(self,&paperDx,&paperDy);

    // get zoom & gravity & page matrix size
    double zoom        = 1;
    int    gravity     = DirWR_NW;
    int    nbline      = 1;
    int    nbcolumn    = 1;
    if ( eo->paperKind==ExpOptPaKd_SetByUser ) {
        if ( eo->placeKind==ExpOptPlace_ZoomUser ) {
            zoom    = eo->placeZoom;
            gravity = eo->placeWhere;
            nbline   = eo->nbline;
            nbcolumn = eo->nbcolumn;
        } else if ( eo->placeKind==ExpOptPlace_ZoomUserAPM ) {
            zoom    = eo->placeZoom;
            gravity = eo->placeWhere;
            // we search for nb: nb*(paper-2*bd) >= zoom*img
            //                 : nb >= (zoom*img) / (paper-2*bd)
            if ( eo->paperOrient!=ExpOptOrient_Portrait ) {
                // landscape (see documentation)
                nbline   = ceil( zoom * Box_dy(*srcBoxCm) / (paperDx-2*eo->paperBdCm) );
                nbcolumn = ceil( zoom * Box_dx(*srcBoxCm) / (paperDy-2*eo->paperBdCm) );
            } else {
                // portrait
                nbline   = ceil( zoom * Box_dy(*srcBoxCm) / (paperDy-2*eo->paperBdCm) );
                nbcolumn = ceil( zoom * Box_dx(*srcBoxCm) / (paperDx-2*eo->paperBdCm) );
            } 
        } else /* if ( eo->placeKind==ExpOptPlace_ZoomToFit ) */ {
            gravity = eo->placeWhere;
            if ( eo->nbline>0 && eo->nbcolumn>0 ) {
                nbline   = eo->nbline;
                nbcolumn = eo->nbcolumn;
            }
            if ( eo->paperOrient!=ExpOptOrient_Portrait ) {
                // landscape (see documentation)
                double devDx = nbcolumn * (paperDy - 2*eo->paperBdCm);
                double devDy = nbline   * (paperDx - 2*eo->paperBdCm);
                zoom = min2( devDx/Box_dx(*srcBoxCm) , devDy/Box_dy(*srcBoxCm) );
            } else {
                // portrait
                double devDx = nbcolumn * (paperDx - 2*eo->paperBdCm);
                double devDy = nbline   * (paperDy - 2*eo->paperBdCm);
                zoom = min2( devDx/Box_dx(*srcBoxCm) , devDy/Box_dy(*srcBoxCm) );
            }
        }
    } else if ( eo->paperKind==ExpOptPaKd_SrcBBZ ) {
            zoom    = eo->placeZoom;
    } // else  last case ( eo->paperKind==ExpOptPaKd_SrcBB ) defaults are good 
    *_zoom = zoom;
    // init cfg
    TodPdfCfg cfg; memset(&cfg,0,sizeof(cfg));
    if ( nbline>1 || nbcolumn>1 ) {
        cfg.cfg_npLine   = nbline;
        cfg.cfg_npColumn = nbcolumn;
    } else
        cfg.cfg_mode  |= OD_PDFCFG_1Page;
    if ( eo->placeKind!=ExpOptPlace_ZoomToFit ) {
        cfg.cfg_mode |= OD_PDFCFG_ScaleG;
        cfg.cfg_scale = zoom;
    }
    cfg.cfg_paperDx = paperDx;
    cfg.cfg_paperDy = paperDy;
    cfg.cfg_gravity = gravity;
    if ( eo->paperBdCm!=0 ) {
      cfg.cfg_mode |= OD_PDFCFG_Bd1;
      cfg.cfg_bd1   = eo->paperBdCm;
    }
    if ( eo->paperOrient!=ExpOptOrient_Portrait ) {
        // landscape (see documentation)
        cfg.cfg_mode |= OD_PDFCFG_Land;
    }
    return cfg;
}

static TodPdfCfg ydw_export_PdfCfg(Tydw_export* self, CboxDble* srcBoxCm)
{
    TexportOption* eo   = &self->dw_expopt;

    if ( eo->paperOrient==ExpOptOrient_Auto ) {
        double zoomPort,zoomLand;
        eo->paperOrient   = ExpOptOrient_Portrait;
        TodPdfCfg cfgPort = ydw_export_PdfCfg_priv(self,srcBoxCm,&zoomPort);
        eo->paperOrient   = ExpOptOrient_Landscape;
        TodPdfCfg cfgLand = ydw_export_PdfCfg_priv(self,srcBoxCm,&zoomLand);
        eo->paperOrient   = ExpOptOrient_Auto;
#if 0
printf("%s:1: port=%f %dx%d ; land=%f %dx%d\n",__func__,
  zoomPort,cfgPort.cfg_npLine,cfgPort.cfg_npLine,
  zoomLand,cfgLand.cfg_npLine,cfgLand.cfg_npLine);
#endif
        if ( eo->placeKind==ExpOptPlace_ZoomToFit )
            return zoomPort>=zoomLand ? cfgPort : cfgLand;
        return (cfgPort.cfg_npLine  *cfgPort.cfg_npLine) <=
               (cfgLand.cfg_npColumn*cfgLand.cfg_npColumn) 
               ? cfgPort : cfgLand;
    }

    double zoom;
    return ydw_export_PdfCfg_priv(self,srcBoxCm,&zoom);
}

