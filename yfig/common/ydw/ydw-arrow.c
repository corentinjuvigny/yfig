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
 *      $Id: ydw-arrow.c 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-arrow.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog that handles arrow parameters.                          =*/
/*=                                                                    =*/
/*= See the "ydw/ydw-arrowparam.h" header for documentation.           =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-arrow.h"

#include "vgui/wid-menu.h"

#include "tools/lenunit.h"
#include "tools/math-macros.h"

/*======================================================================*/
/*= Interface of Tydw_arrow1.                                          =*/

static void arrow1_cb_length   (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void arrow1_cb_angle    (Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void arrow1_cb_thickness(Tvgui_dw* dw, Twid_ilvu* iw, double v);
static void arrow1_cb_mode     (Tvgui_dw* dw, Twid_ilvu* iw, double v);

extern Tydw_arrow1* ydw_arrow1_nbfSub(Tvgui_dw* vdw, Tyfig_mw* ymw,
        int atBeg, TarrowDesc*ad, void* cb, void*ud)
{
    char*       title = atBeg!=0 ? "Begin arrow configuration"
                                 : "End arrow configuration";
    char*       lwidth= "thickness" ;
    Tydw_arrow1*self  = (Tydw_arrow1*) vgui_dw_newSub(sizeof(*self), vdw,
                            tool_strdup(title));
    self->dw_ad   = ad;

    // length
    self->dw_length = wid_ilvu_newD(LUNIT_PtToFu(ad->length)," 9999.9 ",
            NULL, arrow1_cb_length,NULL,0,0);
    wid_ilvu_addLabel  (self->dw_length, "length", lwidth);
    wid_ilvu_addSpinInc(self->dw_length,1);
    wid_ilvu_addUnitStd(self->dw_length);
    widD_ilvu_dbf (self->dw_length, SelfA);
    wid_ilvu_chgUnitStd( self->dw_length, LUNIT_ID_Pt);

    // angle
    self->dw_angle = wid_ilvu_newA(RadToDeg(ad->angle)," 9999.9 ",
            NULL, arrow1_cb_angle,NULL, 5.,80.,NULL);
    wid_ilvu_addLabel  (self->dw_angle, "angle",  lwidth);
    wid_ilvu_addSpinInc(self->dw_angle,5);
    widD_ilvu_dbf (self->dw_angle, SelfA);

    // thickness
    self->dw_thickness = wid_ilvu_newD(LUNIT_PtToFu(ad->thickness)," 9999.9 ",
            NULL, arrow1_cb_thickness,NULL,0,0);
    wid_ilvu_addLabel  (self->dw_thickness, "thickness", lwidth);
    wid_ilvu_addSpinInc(self->dw_thickness,1);
    widD_ilvu_dbf (self->dw_thickness, SelfA);

    // mode
    static char* mode_table[] = {
        "proportional to\nline width",
        "absolute value",
        NULL
    };
    self->dw_mode = wid_ilvu_newDdlStr(ad->abslength!=0,
            mode_table, (Twid_cb_Uap)arrow1_cb_mode, self);
    wid_ilvu_addLabel  (self->dw_mode, "mode",  lwidth);
    widD_ilvu_dbf (self->dw_mode, SelfA);

    // dialog layout
#define LengthL    self->dw_length   ->lvu_labEGS
#define LengthV    self->dw_length   ->lvu_valEGS
#define LengthU    self->dw_length   ->lvu_uniEGS
#define AngleL     self->dw_angle    ->lvu_labEGS
#define AngleV     self->dw_angle    ->lvu_valEGS
#define AngleU     self->dw_angle    ->lvu_uniEGS
#define ThicknessL self->dw_thickness->lvu_labEGS
#define ThicknessV self->dw_thickness->lvu_valEGS
#define ThicknessU self->dw_thickness->lvu_uniEGS
#define ModeL      self->dw_mode     ->lvu_labEGS
#define ModeV      self->dw_mode     ->lvu_valEGS

#define LN(x) VDWLF_ROW(x)
    vgui_dw_finishMTX(SelfA
     ,LN(0)   ,0,LengthL      ,1,LengthV      ,2,LengthU
     ,LN(1)   ,0,AngleL       ,1,AngleV       ,2,AngleU
     ,LN(2)   ,0,ThicknessL   ,1,ThicknessV   ,2,ThicknessU
     ,LN(3)   ,0,ModeL        ,1,ModeV        ,2,ModeV
     ,-1);

    // enable callback
    self->dw_CB   = cb;
    self->dw_CBud = ud;
    return self;
}

extern void ydw_arrow1_delete(Tydw_arrow1*self)
{
    if ( self==0 ) return;
    wid_ilvu_free( self->dw_length );
    wid_ilvu_free( self->dw_angle );
    wid_ilvu_free( self->dw_thickness );
    wid_ilvu_free( self->dw_mode );
    vgui_dw_free( SelfA );
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/*= Tydw_arrow1 callback.                                              =*/

#define CALL_CB() if ( self->dw_CB ) ((void(*)(void*))self->dw_CB)(self->dw_CBud)

static void arrow1_cb_length(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_arrow1*  self = (Tydw_arrow1*)dw;
    self->dw_ad->length = LUNIT_FuToPt(v);
    CALL_CB();
}    

static void arrow1_cb_angle(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_arrow1*  self = (Tydw_arrow1*)dw;
    self->dw_ad->angle = DegToRad(v);
    CALL_CB();
}    

static void arrow1_cb_thickness(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_arrow1*  self = (Tydw_arrow1*)dw;
    self->dw_ad->thickness = LUNIT_FuToPt(v);
    CALL_CB();
}    

static void arrow1_cb_mode(Tvgui_dw* dw, Twid_ilvu* iw, double v)
{
    Tydw_arrow1*  self = (Tydw_arrow1*)dw;
    int iv = v;
    self->dw_ad->abslength = iv;
    CALL_CB();
}    

/*======================================================================*/
/*= Interface of Tydw_arrow2.                                          =*/

extern Tydw_arrow2* ydw_arrow2_nbfSub(Tvgui_dw* vdw, Tyfig_mw* ymw,
        TarrowDesc* bad, TarrowDesc*ead, void* cb, void*ud)
{
    Tydw_arrow2* self = (Tydw_arrow2*) vgui_dw_newSub(sizeof(*self), vdw, NULL);
    self->dw_arrowbeg = ydw_arrow1_nbfSub(SelfA,ymw,1,bad,cb,ud);
    self->dw_arrowend = ydw_arrow1_nbfSub(SelfA,ymw,0,ead,cb,ud);

    vgui_dw_finishIL(SelfA, 0,self->dw_arrowbeg->any.D_widEGS, 0,self->dw_arrowend->any.D_widEGS,-1);

    return self;
}

extern void ydw_arrow2_delete(Tydw_arrow2*self)
{
    if ( self==0 ) return;
    ydw_arrow1_delete( self->dw_arrowend );
    ydw_arrow1_delete( self->dw_arrowbeg );
    vgui_dw_free( SelfA );
}

/*======================================================================*/
