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
 *      $Id: tb-edit.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/tb-edit.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the edit menu.                                         =*/
/*======================================================================*/

#include "topbar.h"
#include "yfig-device.h"
#include "vgui/wid-canvas.h"
#include "vgui/wid-canvas-geomcb.h"  // for CVSGEO_Zstep macro
#include "fig/figure.h"
#include "fig/fig-histo.h"
#include "fig/object.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void action_redraw     (Twidm_any*m, Twidm_entry* e);
static void action_undo       (Twidm_any*m, Twidm_entry* e);
static void action_redo       (Twidm_any*m, Twidm_entry* e);
//static void action_pasteObject (Twidm_any*m, Twidm_entry* e);
//static void action_pasteText  (Twidm_any*m, Twidm_entry* e);
static void action_transTo00  (Twidm_any*m, Twidm_entry* e);
static void action_moveTo00   (Twidm_any*m, Twidm_entry* e);
static void action_setGravity (Twidm_any*m, Twidm_entry* e);
static void action_zoomTo1CTR (Twidm_any*m, Twidm_entry* e);
static void action_zoomTo1NW  (Twidm_any*m, Twidm_entry* e);
static void action_zoomIn     (Twidm_any*m, Twidm_entry* e);
static void action_zoomOut    (Twidm_any*m, Twidm_entry* e);
static void action_zoomToFit  (Twidm_any*m, Twidm_entry* e);
static void action_zoomToFit00(Twidm_any*m, Twidm_entry* e);
static void action_deleteAll  (Twidm_any*m, Twidm_entry* e);
static void action_unitFig    (Twidm_any*m, Twidm_entry* e);
static void action_unitCm     (Twidm_any*m, Twidm_entry* e);
static void action_unitInch   (Twidm_any*m, Twidm_entry* e);
static void action_drawAxis   (Twidm_any*m, Twidm_entry* e);
static void action_settings   (Twidm_any*m, Twidm_entry* e);

static char* gravity_table[] = {
    "Center",
    "North-West", "North", "North-East", "East",
    "South-East", "South", "South-West", "West",
    NULL
};
static Twidm_item table[] = {
//               label                checkmark   gme_action
  WIDM_ITEM_STR   ( "Undo"                   , 0,0   , action_undo        ),
  WIDM_ITEM_STR   ( "Redo"                   , 0,0   , action_redo        ),
  WIDM_ITEM_STR   ( "Redraw"                 , 0,0   , action_redraw      ),
//WIDM_ITEM_STR   ( "Paste Object"           , 0,0   , action_pasteObject ),
//WIDM_ITEM_STR   ( "Paste Text"             , 0,0   , action_pasteText   ),
//WIDM_ITEM_STR   ( "Search"                 , 0,0   , action_searchPanel ),
//WIDM_ITEM_STR   ( "Spell"                  , 0,0   , action_spelling    ),
  WIDM_ITEM_STR   ( "Translate to origin"    , 0,0   , action_transTo00   ),
  WIDM_ITEM_STR   ( "Move to origin"         , 0,0   , action_moveTo00    ),
  WIDM_ITEM_STR_SM( "Move to "               , 0,0   , action_setGravity   , gravity_table ),
  WIDM_ITEM_STR   ( "Set Zoom to 1 (NW)"     , 0,0   , action_zoomTo1NW   ),
  WIDM_ITEM_STR   ( "Set Zoom to 1 (center)" , 0,0   , action_zoomTo1CTR  ),
  WIDM_ITEM_STR   ( "Zoom in"                , 0,0   , action_zoomIn      ),
  WIDM_ITEM_STR   ( "Zoom out"               , 0,0   , action_zoomOut     ),
  WIDM_ITEM_STR   ( "Zoom to fit"            , 0,0   , action_zoomToFit   ),
  WIDM_ITEM_STR   ( "Pan  to origin"         , 0,0   , action_zoomToFit00 ),
  WIDM_ITEM_STR   ( "Delete All"             , 0,0   , action_deleteAll   ),
  WIDM_ITEM_SEP   ( /* separator */                                     ),
#define FigureUnitIndex 14 // table[..Index]="Figure Unit" item
  WIDM_ITEM_STR   ( "Figure Unit"            , 1,1   , action_unitFig     ),
  WIDM_ITEM_STR   ( "Centimeter Unit"        , 1,0   , action_unitCm      ),
  WIDM_ITEM_STR   ( "Inch (decimal) Unit"    , 1,0   , action_unitInch    ),
  WIDM_ITEM_STR   ( "Draw axis"              , 1,0   , action_drawAxis    ),
//WIDM_ITEM_STR   ( "Settings"               , 0,0   , action_settings    ),
};

extern Twidm_item*  tb_edit_t() { return table; }
extern int          tb_edit_n() { return TOOL_TABLE_NbEle(table); }
extern void         tb_edit_update(Cyfig_mw*ymw, Twidm_any* m)
{
    Tfig_histo*  histo = yfig_cursce_histo(ymw);
    if ( histo==0 ) {
        widm_any_setEntrySelectable(m,0,0); // undo
        widm_any_setEntrySelectable(m,1,0); // redo
    } else {
        widm_any_setEntrySelectable(m,0,fig_histo_undoNb(histo)>0);
        widm_any_setEntrySelectable(m,1,fig_histo_redoNb(histo)>0);
    }

    int uuIsInch = ymw->ymw_userUnit.uu_id==LUNIT_ID_Inch;
    int i=FigureUnitIndex;
    Twidm_entry* unitFig  = m->ma_entries->eles[i++];
    Twidm_entry* unitCm   = m->ma_entries->eles[i++];
    Twidm_entry* unitInch = m->ma_entries->eles[i++];
    TEMP_asf_printf( unitFig->me_label && strstr(unitFig->me_label,"Figure")==0,"%s","unit items not found");
    if ( ymw->ymw_do_useFigUnit==1 ) {
        widm_entry_setChecked   (unitFig   ,1);
        widm_entry_setSelectable(unitCm    ,1);
        widm_entry_setSelectable(unitInch  ,1);
        widm_entry_setChecked   (unitCm    ,!uuIsInch);
        widm_entry_setChecked   (unitInch  , uuIsInch);
        widm_entry_setSelectable(unitCm    ,0);
        widm_entry_setSelectable(unitInch  ,0);
    } else {
        widm_entry_setChecked   (unitFig   ,0);
        widm_entry_setSelectable(unitCm    ,1);
        widm_entry_setSelectable(unitInch  ,1);
        widm_entry_setChecked   (unitCm    , !uuIsInch);
        widm_entry_setChecked   (unitInch  ,  uuIsInch);
    }
}

/*======================================================================*/
/*= Messages                                                           =*/

/*======================================================================*/
/*= Callbacks                                                          =*/

static void action_redraw(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    DBG0_MenuSelectedMAE(m,e);
    yfig_cursce_redraw_update( ymw );
    DBG1_MenuSelectedMAL(m,e);
}

static void action_undo(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    Tfig_histo*  histo = yfig_cursce_histo(ymw);

    TEMP_asf_printf(histo==NULL, "histo==NULL or empty and %s->%s entry not disabled", DBG_MenuParMAE(m,e));

    DBG0_MenuSelectedMAE(m,e);
    if ( fig_histo_undoNb(histo)<=0 ) {
        TEMP_spe_printf(
            "histo is empty (%d undo) and %s->%s entry not disabled",
            fig_histo_undoNb(histo),DBG_MenuParMAE(m,e));
    } else {
        fig_histo_undo( histo );
        ymw->ymw_currHistCurr -= 1;
        yfig_cursce_redraw_update( ymw );
        if ( fig_histo_undoNb(histo)==0 )
            widm_entry_setSelectable(e,0);
        widm_any_setEntrySelectable(m,1,1); // 1==redo
            
    }
    DBG1_MenuSelectedMAL(m,e);
}

static void action_redo(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    Tfig_histo*  histo = yfig_cursce_histo(ymw);

    TEMP_asf_printf(histo==NULL, "histo==NULL or empty and %s->%s is not disabled", DBG_MenuParMAE(m,e));
    DBG0_MenuSelectedMAE(m,e);

    if ( fig_histo_redoNb(histo)<=0 ) {
        TEMP_spe_printf("no redo in histo (%d redo) and %s->%s is not disabled",fig_histo_redoNb(histo),DBG_MenuParMAE(m,e));
    } else {
        fig_histo_redo( histo );
        ymw->ymw_currHistCurr += 1;
        yfig_cursce_redraw_update( ymw );
        if ( fig_histo_redoNb(histo)==0 )
            widm_entry_setSelectable(e,0);
        widm_any_setEntrySelectable(m,0,1); // 1==redo
    }
    DBG1_MenuSelectedMAL(m,e);
}

static void action_deleteAll(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    Tfigure*     fig   = YMW_figure(ymw);
    CobjAllSet*  objs  = obj_getChildren(obj_ObjAll(fig->objects));
    yfig_cursce_supObjSetWHis(ymw,objs->eles,objs->eleNb);
    yfig_cursce_redraw_update(ymw);
}

/*======================================================================*/
#define    borderPt         6.0
#define    borderCm         LUNIT_PtToCm(borderPt)
#define    borderSrcOd(od) (LUNIT_PtToIn(borderPt)*OD_ppiSrc(od))

static void action_transTo00 (Twidm_any*m, Twidm_entry*e) {
    ACTMM_V_TOBJ(m,e);
    Tdble2        objxy  = tool_boxdble_pt1(boxObj);

    objxy = dble2_initVab(objxy,dble2_origin);
    yfig_cursce_movObjWHis(ymw,obj,objxy);
    action_moveTo00(m,e);
}

static void action_moveTo00  (Twidm_any*m, Twidm_entry*e)
{
    ACTMM_V_FSOD(m,e);

    Box_x(od->boxSrc) = -borderSrcOd(od);
    Box_y(od->boxSrc) = -borderSrcOd(od);
    yfig_canvas_updateUnit(ymw,NULL);
    yfig_cursce_redraw(ymw);
}

static void action_setGravity(Twidm_any*m, Twidm_entry*e)
{
    ACTSM_V_COBJ(m,e);

    CboxDble*     boxSrc    = fsod_getBoxSrc( fsod );
    Tdble2        ctrObj    = tool_boxdble_ctr(boxObj);
    Tdble2        pt1Obj    = tool_boxdble_pt1(boxObj);
    Tdble2        pt3Obj    = tool_boxdble_pt3(boxObj);

#define TEMP_ASE(s1,s2,me) TEMP_ase_printf(strstr(me->me_label,#s1)==0 || strstr(me->me_label,#s2)==0,\
        "found %s for %d entry, expected %s and %s.",me->me_label,me->me_ident.sint,#s1,#s2)
    switch ( e->me_ident.sint ) {
        case 0: TEMP_ASE(ent,ent,e);
            Box_x(od->boxSrc) = ctrObj.x - Box_dx(*boxSrc)/2 ;
            Box_y(od->boxSrc) = ctrObj.y - Box_dy(*boxSrc)/2 ;
            break;
        case 1: TEMP_ASE(ort,est,e);
            Box_x(od->boxSrc) = pt1Obj.x - 0.0*Box_dx(*boxSrc) - 1.0*borderSrcOd(od);
            Box_y(od->boxSrc) = pt1Obj.y - 0.0*Box_dx(*boxSrc) - 1.0*borderSrcOd(od);
            break;
        case 2: TEMP_ASE(ort,ort,e);
            Box_x(od->boxSrc) = ctrObj.x - 0.5*Box_dx(*boxSrc) + 0.0*borderSrcOd(od) ;
            Box_y(od->boxSrc) = pt1Obj.y - 0.0*Box_dx(*boxSrc) + 1.0*borderSrcOd(od);
            break;
        case 3: TEMP_ASE(ort,ast,e);
            Box_x(od->boxSrc) = pt3Obj.x - 1.0*Box_dx(*boxSrc) + 1.0*borderSrcOd(od) ;
            Box_y(od->boxSrc) = pt1Obj.y - 0.0*Box_dy(*boxSrc) - 1.0*borderSrcOd(od) ;
            break;
        case 4: TEMP_ASE(ast,ast,e);
            Box_x(od->boxSrc) = pt3Obj.x - 1.0*Box_dx(*boxSrc) + 1.0*borderSrcOd(od) ;
            Box_y(od->boxSrc) = ctrObj.y - 0.5*Box_dy(*boxSrc) - 0.0*borderSrcOd(od) ;
            break;
        case 5: TEMP_ASE(out,ast,e);
            Box_x(od->boxSrc) = pt3Obj.x - 1.0*Box_dx(*boxSrc) + 1.0*borderSrcOd(od) ;
            Box_y(od->boxSrc) = pt3Obj.y - 1.0*Box_dy(*boxSrc) + 1.0*borderSrcOd(od) ;
            break;
        case 6: TEMP_ASE(out,out,e);
            Box_x(od->boxSrc) = ctrObj.x - 0.5*Box_dx(*boxSrc) + 0.0*borderSrcOd(od) ;
            Box_y(od->boxSrc) = pt3Obj.y - 1.0*Box_dy(*boxSrc) + 1.0*borderSrcOd(od) ;
            break;
        case 7: TEMP_ASE(out,est,e);
            Box_x(od->boxSrc) = pt1Obj.x - 0.0*Box_dx(*boxSrc) - 1.0*borderSrcOd(od);
            Box_y(od->boxSrc) = pt3Obj.y - 1.0*Box_dy(*boxSrc) + 1.0*borderSrcOd(od) ;
            break;
        case 8: TEMP_ASE(est,est,e);
            Box_x(od->boxSrc) = pt1Obj.x - 0.0*Box_dx(*boxSrc) - 1.0*borderSrcOd(od);
            Box_y(od->boxSrc) = ctrObj.y - 0.5*Box_dy(*boxSrc) + 0.0*borderSrcOd(od) ;
            break;
    }
    yfig_canvas_updateUnit(ymw,NULL);
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/

static void action_zoom_do (Twidm_any*m, Twidm_entry* e, int op)
{
    ACTMM_V_COBJ(m,e);

    switch ( op ) {
        // change zoom to 1 (NW),the obj point at the DA NW stays at the DA NW.
        case 0: od_setUsrZoom_NW(od,1); break;
        // change zoom to 1 (CENTER), the obj point at the DA center stays at the DA center.
        case 1: od_setUsrZoom_CENTER(od,1); break;
        // zoom in (the figure becomes greater), the obj point at the DA center stays at the DA center.
        case 2: od_setUsrZoom_CENTER(od,od_getUsrZoom(od)*CVSGEO_Zstep); break;
        // zoom out (the figure becomes smaller), the obj point at the DA center stays at the DA center.
        case 3: od_setUsrZoom_CENTER(od,od_getUsrZoom(od)/CVSGEO_Zstep); break;
        // zoom (in or out) to fit the figure in the DA.
        case 4: od_updateSrcBB_toBoxXY(od,boxObj,borderCm,borderCm); break;
        // zoom (in or out) to fit the origin (0,0) and the figure in the DA.
        case 5: {
            TboxDble box;
            Tdble2 pt1 = tool_boxdble_pt1(boxObj);
            Tdble2 pt2 = tool_boxdble_pt2(boxObj);
            Tdble2 pt3 = tool_boxdble_pt3(boxObj);
            Tdble2 pt4 = tool_boxdble_pt4(boxObj);
            if ( pt1.x>=0 && pt1.y>=0 ) {
                box = tool_boxdble_initPT(dble2_origin,pt3);
            } else if ( pt2.x<=0 && pt2.y>=0 ) {
                box = tool_boxdble_initPT(dble2_origin,pt4);
            } else if ( pt3.x<=0 && pt3.y<=0 ) {
                box = tool_boxdble_initPT(dble2_origin,pt1);
            } else if ( pt4.x>=0 && pt4.y<=0 ) {
                box = tool_boxdble_initPT(dble2_origin,pt2);
            } else {
                double d,maxd = VECTOR_NormePT(pt1);
                box = tool_boxdble_initPT(dble2_origin,pt1);
                if ( (d=VECTOR_NormePT(pt2))>maxd) { maxd=d; box = tool_boxdble_initPT(dble2_origin,pt2); }
                if ( (d=VECTOR_NormePT(pt3))>maxd) { maxd=d; box = tool_boxdble_initPT(dble2_origin,pt3); }
                if ( (d=VECTOR_NormePT(pt4))>maxd) { maxd=d; box = tool_boxdble_initPT(dble2_origin,pt4); }
            }
            od_updateSrcBB_toBoxXY(od,&box,borderCm,borderCm);
            break;
            }
        default:
            TEMP_spe_printf("%d op is invalid",op);
            break;
    }
    yfig_dppanel_update(ymw);
    yfig_canvas_updateUnit(ymw,NULL);
    yfig_cursce_redraw_update(ymw);
}

static void action_zoomTo1NW  (Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,0); }
static void action_zoomTo1CTR (Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,1); }
static void action_zoomIn     (Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,2); }
static void action_zoomOut    (Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,3); }
static void action_zoomToFit  (Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,4); }
static void action_zoomToFit00(Twidm_any*m, Twidm_entry*e) { action_zoom_do(m,e,5); }

/*======================================================================*/

static void action_unitChg(Twidm_any*m, Twidm_entry* e, int uu)
{
    ACTMM_V_YMW(m,e);
    DBG0_MenuE("top->%s->%s : checked=%d\n",((Twidm_popup*)m)->m_label,e->me_label,e->me_checked);
    if ( uu!=LUNIT_ID_None ) {
        if ( e->me_checked==0 )
            uu = uu==LUNIT_ID_Cm ? LUNIT_ID_Inch : LUNIT_ID_Cm;
        TuserUnit uu2; lu_init(&uu2,uu,1.0,10);
        yfig_canvas_updateUnit(ymw,&uu2);
    } else {
        if ( e->me_checked ) {
            Tfigure* fig  = YMW_figure(ymw);
            uu = fig->settings.userUnit.uu_id;
            ymw->ymw_do_useFigUnit = 1;
            if ( uu!=ymw->ymw_userUnit.uu_id ) {
                TuserUnit uu2; lu_init(&uu2,uu,1.0,10);
                yfig_canvas_updateUnit(ymw,&uu2);
            }
        } else {
            ymw->ymw_do_useFigUnit = 0;
            tb_edit_update(ymw,m);
        }
    }
    DBG1_MenuL("top->%s->%s : checked=%d\n",((Twidm_popup*)m)->m_label,e->me_label,e->me_checked);
}

static void action_unitFig (Twidm_any*m, Twidm_entry* e) { action_unitChg(m,e,LUNIT_ID_None); }
static void action_unitCm  (Twidm_any*m, Twidm_entry* e) { action_unitChg(m,e,LUNIT_ID_Cm  ); }
static void action_unitInch(Twidm_any*m, Twidm_entry* e) { action_unitChg(m,e,LUNIT_ID_Inch); }

/*======================================================================*/

static void action_drawAxis(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    yfig_canvas_enableAxis(ymw,e->me_checked);
    yfig_cursce_redraw(ymw);
}

/*======================================================================*/

static void action_settings(Twidm_any*m, Twidm_entry* e)
{
    ACTMM_V_YMW(m,e);
    PERM_error_NYI();
}

/*======================================================================*/
