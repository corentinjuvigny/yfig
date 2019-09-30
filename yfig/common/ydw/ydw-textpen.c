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
 *      $Id: ydw-textpen.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-textpen.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that handles pen parameters.                            =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-grp.h"
#include "vgui/wid-lvu.h"
#include "vgui/wid-button.h"

#include "tools/color.h"

/*======================================================================*/
/*= data structure                                                     =*/

struct _Tydw_tpen {
    Tvgui_dw    any;
    Twid_olvu*  dw_color;
    Tvgui_dw*   dw_color_d;
    TastrSet*   dw_fontStr;
    Twid_ilvu*  dw_fontWid;
    Twid_ilvu*  dw_dirWid;
    Twid_ilvu*  dw_pts;
    Twid_grp*   dw_angle;

    int          dw_dirSTY[3];  // direction order stored in dw_dirWid
    Tsty_text    dw_pen;        // current text pen
    Tydw_tpen_cb dw_cb;
    void*        dw_ud;
} ;

Inline int DirSTY2Idx(Cydw_tpen*s,int dir) { return dir==s->dw_dirSTY[0] ? 0 : dir==s->dw_dirSTY[1] ? 1 : 2; }
Inline int CurrDirSTY(Cydw_tpen*s)         { return s->dw_dirSTY[ (int)wid_ilvu_getNum10(s->dw_dirWid) ]; }

/*======================================================================*/
/*= Interface                                                          =*/

// build the pen widget
static void ydw_tpen_nbf_I(Tydw_tpen*self, 
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_text*dpt, int dir, Tydw_tpen_cb cb, void*ud);

// redraw the widget
static void tpen_act_update(Tydw_tpen* self);

static void tpen_cb_color (Tvdw_colorSel*cs, Ccolor*c, void*ud);
static void tpen_cb_font  (void* ud, ...);
static void tpen_cb_dir  (void* ud, ...);
static void tpen_cb_pts   (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void tpen_cb_angle (Twid_grp*grp, void* ud);

extern Tvgui_dw* ydw_tpen_nbfTop(
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_text*dpt, int dir, Tydw_tpen_cb cb, void*ud)
{
    char*      title = "Text pen configuration";
    Tydw_tpen* self  = (Tydw_tpen*) vgui_dw_newTop(sizeof(*self), (Tvgui_mw*)ymw,
                            tool_strdup(title),
                            WID_DIALSTATUS_CancelX|WID_DIALSTATUS_Ok);

    ydw_tpen_nbf_I(self,vdw,ymw,dpt,dir,cb,ud);
    ydw_tpen_hideClr(self);
    return SelfA;
}

extern Tydw_tpen* ydw_tpen_nbfSub(
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_text*dpt, int dir, Tydw_tpen_cb cb, void*ud)
{
    char*      title = "Text pen configuration";
    Tydw_tpen* self  = (Tydw_tpen*) vgui_dw_newSub(sizeof(*self), vdw,
                            tool_strdup(title));
    vgui_dw_titleAddLabTxt(SelfA, "--------------------", "--------------------");

    ydw_tpen_nbf_I(self,vdw,ymw,dpt,dir,cb,ud);

    ydw_tpen_hideClr(self);
    tpen_act_update(self);
    return self;
}

extern void       ydw_tpen_free(Tydw_tpen* self)
{
    wid_olvu_free  ( self->dw_color );
    ydw_colors_free( self->dw_color_d );
    wid_ilvu_free  ( self->dw_fontWid );
    astrset_delete ( self->dw_fontStr );
    wid_ilvu_free  ( self->dw_pts );
    wid_grp_delete ( self->dw_angle      );
    vgui_dw_free(SelfA);
}

extern Tegs_widget ydw_tpen_widEGS    (Tydw_tpen* self) { return self->dw_widEGS; }
extern int         ydw_tpen_dirSTY    (Tydw_tpen* self) { return CurrDirSTY(self); }
extern Csty_text*  ydw_tpen_dpt       (Tydw_tpen* self) { return &self->dw_pen; }

extern void        ydw_tpen_showClr(Tydw_tpen* self)
{
    Twid_button* btn = wid_olvu_getUniWid(self->dw_color);
    widD_button_fuf_setUnFolded(btn);
}

extern void       ydw_tpen_hideClr(Tydw_tpen* self)
{
    Twid_button* btn = wid_olvu_getUniWid(self->dw_color);
    widD_button_fuf_setFolded(btn);
}

/*======================================================================*/
/*= Private utilities                                                  =*/

#include "vgui/vgui.h"
#include "outdev/outdev.h"
#include "outdev/resources.h"
#include "tools/font-types.h"
#include "tools/font.h"
extern char* ymw_cursce_getFontUsrName(Tyfig_mw*ymw, char* buf, int fid)
{
    Tvgui*     gui      = vgui_mw_getVGui((Tvgui_mw*)ymw);
    CfontDef*  fontDef  = VG_fontdef(gui,fid);
    CstrStatic fontName = TFD_GetLongName(*fontDef);
    sprintf(buf,"%s",fontName);
    return buf;
}

static TpixmapEGS tpen_genDirPxm(Tvgui* gui, int dir);

static void ydw_tpen_nbf_I(Tydw_tpen*self, 
        Tvgui_dw* vdw, Tyfig_mw* ymw,
        Csty_text*dpt, int dir, Tydw_tpen_cb cb, void*ud)
{
    int          i;
    char         buf[1024];
    Twid_button* btn;
    Tvgui*       gui        = vdw!=0 ? vgui_dw_getVGui(vdw)
                                     : yfig_mw_getVGui(ymw);

    self->dw_pen = *dpt;
    self->dw_cb  = cb;
    self->dw_ud  = ud;

    // widgets for changing color 
    self->dw_color_d = ydw_colors_nbf(SelfA,ymw, NULL, tpen_cb_color,self);
    self->dw_color = wid_olvu_newC(NULL,YDW_VAL_WITH, dpt->dpt_color,0,0);
    wid_olvu_addLabel  (self->dw_color, "color", YDW_LAB_WITH);
    btn = widD_button_fuf_nbflab(SelfA,NULL,NULL);
    wid_olvu_addButton (self->dw_color, btn);
    widD_olvu_dbf      (self->dw_color, SelfA);
    widD_button_fuf_addWidToIgn(btn,self->dw_color_d->D_widEGS,NULL);

    // fonts
    int fontNb = VG_fontdefNb(gui);
    self->dw_fontStr = astrset_new(fontNb);
    for (i=0 ; i<fontNb ; i+=1)
        astrset_add(self->dw_fontStr,ymw_cursce_getFontUsrName(ymw, buf,i));
    astrset_nullTerminated(self->dw_fontStr);
    self->dw_fontWid = wid_ilvu_newDdlStr(dpt->stt_font,
        self->dw_fontStr->eles, tpen_cb_font,self);
    wid_ilvu_addLabel (self->dw_fontWid, "font", YDW_LAB_WITH);
    widD_ilvu_dbf     (self->dw_fontWid, SelfA);

    // pts
    self->dw_pts = wid_ilvu_newD(dpt->dpt_pts, "99.9 ",
                           NULL,tpen_cb_pts,self,
                           FIG_THICKNESS_MIN, FIG_THICKNESS_MAX);
    wid_ilvu_addLabel  (self->dw_pts, "font size", YDW_LAB_WITH);
    wid_ilvu_addSpinInc(self->dw_pts,1);
    wid_ilvu_addUnitLab(self->dw_pts, "pt", NULL);
    widD_ilvu_dbf      (self->dw_pts, SelfA);

    // direction
#define DirPxm_DX 50
#define DirPxm_DY 30
    TpixmapEGS*pxms = tool_malloc(sizeof(*pxms)*3); // eaten by dw_dirWid.
    i =0; self->dw_dirSTY[i]=STY_T_RIGHT;  pxms[i]=tpen_genDirPxm(gui,self->dw_dirSTY[i]);
    i+=1; self->dw_dirSTY[i]=STY_T_CENTER; pxms[i]=tpen_genDirPxm(gui,self->dw_dirSTY[i]);
    i+=1; self->dw_dirSTY[i]=STY_T_LEFT;   pxms[i]=tpen_genDirPxm(gui,self->dw_dirSTY[i]);
    self->dw_dirWid = wid_ilvu_newDdlPxm( DirSTY2Idx(self,dir), pxms,3, DirPxm_DX,DirPxm_DY, tpen_cb_dir,self);
    wid_ilvu_addLabel (self->dw_dirWid, "direction", YDW_LAB_WITH);
    widD_ilvu_dbf     (self->dw_dirWid, SelfA);

    // angle
    self->dw_angle = wid_ioa180_nbf(SelfA,"angle", NULL,tpen_cb_angle,self, dpt->stt_angle);

#define FontL     self->dw_fontWid->lvu_labEGS
#define FontV     self->dw_fontWid->lvu_valEGS
#define DirL      self->dw_dirWid->lvu_labEGS
#define DirV      self->dw_dirWid->lvu_valEGS
#define PtsL      self->dw_pts->lvu_labEGS
#define PtsV      self->dw_pts->lvu_valEGS
#define PtsU      self->dw_pts->lvu_uniEGS
#define ColorL    self->dw_color->lvu_labEGS
#define ColorV    self->dw_color->lvu_valEGS
#define ColorU    self->dw_color->lvu_uniEGS
#define ColorD    self->dw_color_d->D_widEGS
#define LN(x)     VDWLF_ROW(x)
    vgui_dw_finishMTX(SelfA
     ,LN(0)   ,0,FontL            ,1,FontV      ,2,FontV
     ,LN(1)   ,0,PtsL             ,1,PtsV       ,2,PtsU
     ,LN(2)   ,0,DirL             ,1,DirV     //,2,DirU
     ,LN(3)   ,MTX_IOAD_VAU (0,self->dw_angle)
     ,LN(4)   ,0,ColorL           ,1,ColorV     ,2,ColorU
     ,LN(5)   ,0|VDWLF_EH,ColorD  ,1,ColorD     ,2|VDWLF_EH,ColorD  ,3,ColorD ,4,ColorD
     ,-1);
}

static void tpen_act_update(Tydw_tpen* self)
{
    char         temp[1024];
//  Tvgui*       gui     = vgui_dw_getVGui((Tvgui_dw*)self);
    Tsty_text*   pen     = &self->dw_pen;
    Twid_olvu*   cviewer = self->dw_color;
    Tvgui_mw*    vmw     = vgui_dw_getMw( &self->any );
    Tyfig_mw*    ymw     = (Tyfig_mw*)vmw;

    wid_olvu_setBgClr    (cviewer,pen->stt_color);
    if ( self->dw_title ) {
        Tegs_widget  mlabel  = vgui_dw_titleLab(SelfA);
        //wid_label_setBgColor(mlabel,pen->stt_color);
        wid_label_setText   (mlabel,ymw_cursce_getFontUsrName(ymw,temp,pen->stt_font));
    }
}

/*======================================================================*/
/*= Callback                                                           =*/

#define CALL_CB() if ( self->dw_cb ) self->dw_cb(self,self->dw_ud)

static void tpen_cb_font  (void* ud, ...)
{
    Tydw_tpen* self = ud;
    int        fid  = wid_ilvu_getNum10(self->dw_fontWid);
    if ( self->dw_pen.dpt_font != fid ) {
        self->dw_pen.dpt_font = fid;
        CALL_CB();
    }
}

static void tpen_cb_dir  (void* ud, ...)
{
    Tydw_tpen* self = ud;
    int        dir  = CurrDirSTY( self );
    CALL_CB();
}

static void tpen_cb_pts(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_tpen* self = (Tydw_tpen*)dw;
    if ( self->dw_pen.dpt_pts != v ) {
        self->dw_pen.dpt_pts = v;
        CALL_CB();
    }
}

static void tpen_cb_color(Tvdw_colorSel*cs, Ccolor*c, void*ud)
{
    Tydw_tpen* self = ud;
    if ( color_cmp(self->dw_pen.stt_color,c)!=0 ) {
        color_chgLazy(&self->dw_pen.stt_color,c);
        tpen_act_update(self);
        CALL_CB();
    }
}

static void tpen_cb_angle (Twid_grp*grp, void* ud)
{
    Tydw_tpen* self = ud;
    self->dw_pen.stt_angle = wid_grp_getDble10(grp);
    CALL_CB();
}

/*======================================================================*/
/*= Creation of direction pixmap.                                      =*/ 

static int linelengths[] = {
    DirPxm_DX,
    DirPxm_DX - (2*DirPxm_DX)/3,
    DirPxm_DX - (1*DirPxm_DX)/3,
    DirPxm_DX - (2*DirPxm_DX)/3,
};
#define linelengthsNB TOOL_TABLE_NbEle(linelengths)

#include "vgui/vgui-names.h"
#include "vgui/vgui-outdev.h"
#include "outdev/ldaf-hl.h"

static TpixmapEGS tpen_genDirPxm(Tvgui* gui, int dir)
{
    Ccolor         *fg,*bg; vgui_wn_getColor(gui,wn_dialSub,&fg,&bg);
    Tvgui_odpixmap* odpxm = vgui_odpixmap_new(gui,bg, DirPxm_DX,DirPxm_DY,NULL);
    Toutdev*        od    = &odpxm->vv_od;
    Tod_GCdef       gcd   = OD_GC_SysDraw;

    od_style_pushTop(od,gcd);
      int l,x,y,x0=2,y0=2;
      for ( l=0,y=y0+2 ; y<(y0+DirPxm_DY) ; y+=4,l+=1 ) {
          int dx = linelengths[l%linelengthsNB];
          x = x0; // set for STY_T_LEFT
          if ( dir==STY_T_RIGHT )
              x = x0 + DirPxm_DX - dx;
          else if ( dir==STY_T_CENTER )
              x = x0 + (DirPxm_DX - dx)/2;
          od_draw_lineXY(od,gcd,x,y,x+dx,y);
      }
    od_style_pop(od,gcd);
    return vgui_odpixmap_delete2(odpxm);
}

/*======================================================================*/
