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
 *$Software: YFIG-MP (Mode Panel)
 *      $Id: text.c 316 2018-07-19 15:29:19Z td $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel/text.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Mode panel FSM for creation and modification of text object.       =*/
/*======================================================================*/

#include "modepanel.h"
#include "yfig-efsmSelObj.h"
#include "vgui/vgui.h"
#include "vgui/wid-simple.h"

#include "fig/fig-scene.h"
#include "fig/obj-text.h"

#include "outdev/string-hl.h"
#include "tools/math-macros.h"

/*======================================================================*/
/*= Interface                                                          =*/

// This function is performed when a new text object must be created
// at the pos position.
static void mp_text_newtext(TefsmSel1Obj*fsm, int click, Tdble2 pos);

// This function is performed when the text of the selected obj object
// must be changed.
// Furthermore if the text is changed to the empty string, the obj
// object is deleted.
static void mp_text_chgtext(TefsmSel1Obj*fsm, int click,
                                  TobjAll obj, Tdble2 marker);

static Twid_mousefun3S mp_text_mess[] = {
    { "New or Edit" , "New or Edit"  , 0 },       // BEGIN
    { 0             , 0              , 0 },       // END
    { 0             , 0              , 0 },       // OBJSEL
    { 0             , 0              , 0 },       // OBJNEW
};

extern void mp_text_cb(Tvgui_mw* vmw, int uid, void* ud)
{
    Tyfig_mw*        ymw  = (Tyfig_mw*)vmw;
    Tvgui_efsm**     ret  = (Tvgui_efsm**)ud;
    Twid_canvas*     cvs  = ymw->ymw_canvas;

    TefsmSel1Obj*  fsm = yfig_efsmSel1Obj_new(cvs,
            FSC_MM_TEX,mp_text_chgtext,ymw);
    fsm->any.magnet = 1;
    fsm->fsm_posfunc   = mp_text_newtext;

    vgui_efsm_mousefun_enable(&fsm->any,
            ymw->ymw_mouseFun, mp_text_mess);

    yfig_dpp_showonly(ymw,DPP_SHOW_Std|DPP_SHOW_Text);
    *ret = &fsm->any;
}

/*======================================================================*/
/*= data structure used to transmit information to dialog callback     =*/

typedef struct {
    TefsmSel1Obj* fsm;      // The current FSM
    Tyfig_device* ydev;     // The device to paint
    Tdble2        pos;      // The string object must be placed
                            // at this position. 
    TobjAll       obj;      // The current string object
                            // or NULL if it is not defined.
} TparamTextCB;

/*======================================================================*/
/*= Main private utilities                                             =*/

/*= The mptext_getNextPos function returns the position of the next    =*/
/*= line of the current line at the pt position.                       =*/
/*= The pt position as the returned position are given in the source   =*/
/*= unit of the ydev device.                                           =*/
/*= This function takes into account the font identifier, the font     =*/
/*= size, the line spacing factor (lsf) and the angle given in the     =*/
/*= dpt structure.                                                     =*/
static Tdble2 mptext_getNextPos(
    Cyfig_device* ydev, double lsf, Tdble2 pt, Csty_text* dpt);

// dialog callback when a user changed text
static void mptext_newtext_gsls_cb(void* ud, CstrBrief str);
static void mptext_chgtext_gsls_cb(void* ud, CstrBrief str);

static void mp_text_newtext(TefsmSel1Obj*fsm, int click, Tdble2 pos)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tvgui*         gui  = ymw->ymw_gui;
    Tegs_widget    pw   = wid_buttonPanel_getWidEGS(ymw->ymw_dpPanel);
    double         lsf  = yfig_mw_getLSF(ymw);


    DBG0_EfsmE("fsm=%p click=%d marker=(%f %f)\n",
            fsm,click,pos.x, pos.y);

    TparamTextCB data = { fsm, ydev, pos, 0 };

    while (1) {
        Twid_dialStatus status =
            vdwM_gsls_start_CancelContOk(&ymw->any,0,
                0,".......................", // FIXME
                mptext_newtext_gsls_cb,0,&data,
                pw,1
            );
    
#define Str data.obj.text->OTE_str
        if ( OBJ_IsDefined(data.obj) && status==WID_DIALSTATUS_Ok ) {
            // user defined a new string object 
            DBG1_EfsmM("fsm=%p click=%d marker=(%f %f) : Ok : str=%s\n",
                fsm,click,pos.x, pos.y, Str);
            ydev_draw_obj(ydev,data.obj.any,OD_GC_Preview);
            yfig_cursce_addObjWHis(ymw,data.obj);
            yfig_cursce_redraw_objAdded(ymw,data.obj);
            od_ds_xorDrawStart_0(fsm->fsm_outdevice);
            od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
            break;
        } else if ( OBJ_IsDefined(data.obj) && status==WID_DIALSTATUS_Cont ) {
            DBG1_EfsmM("fsm=%p click=%d marker=(%f %f) : Cont.: str=%s\n",
                fsm,click,pos.x, pos.y, Str);
            ydev_draw_obj(ydev,data.obj.any,OD_GC_Preview);
            yfig_cursce_addObjWHis(ymw,data.obj);
            yfig_cursce_redraw_objAdded(ymw,data.obj);
            data.pos = mptext_getNextPos(ydev, lsf, data.pos,
                    &ymw->ymw_currDP.fdp_text);
            data.obj=OBJ_none;
            od_ds_xorDrawStart_0(fsm->fsm_outdevice);
            od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
        } else if ( OBJ_IsDefined(data.obj) ) {
            // user defined a new string object but canceled its creation
            DBG1_EfsmM("fsm=%p click=%d marker=(%f %f) : Cancel : str=%s\n",
                fsm,click,pos.x, pos.y, Str);
            ydev_draw_obj(ydev,data.obj.any,OD_GC_Preview);
            obj_dir_delObjQ ( data.obj );
            od_ds_xorEraseStart_0(fsm->fsm_outdevice);
            od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
            break;
        } else { //data.obj==0
            if ( status==WID_DIALSTATUS_Ok ) { // ignore it
                od_ds_xorEraseStart_0(fsm->fsm_outdevice);
                od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
                continue;
            }
            else if ( status==WID_DIALSTATUS_Cancel ) // quit                
                break;
            else { // line feed
                data.pos = mptext_getNextPos(ydev, lsf, data.pos,
                    &ymw->ymw_currDP.fdp_text);
                continue;
            }
        }
    }

    DBG2_EfsmL("fsm=%p click=%d marker=(%f %f)\n",
            fsm,click,pos.x, pos.y);
}

static void mp_text_chgtext(TefsmSel1Obj*fsm, int click,
                                  TobjAll obj, Tdble2 marker)
{
    Tyfig_device*  ydev = (Tyfig_device*) fsm->fsm_outdevice;
    Tyfig_mw*      ymw  = (Tyfig_mw*)     fsm->fsm_userdata;
    Tegs_widget    pw   = wid_buttonPanel_getWidEGS(ymw->ymw_dpPanel);
    DBG0_EfsmE("fsm=%p obj=%p click=%d marker=(%f %f)\n",
            fsm,click,obj,marker.x, marker.y);

    // obj2: used for preview and it is the object to insert in the
    // current figure if change is accepted.
    Tobject obj2 = obj_newClone( obj ).any;
    obj_dir_remObjQ( obj );
    yfig_cursce_redraw(ymw);
    od_ds_xorDrawStart_0(fsm->fsm_outdevice);
    od_ds_xorDrawCommit_0(fsm->fsm_outdevice);
    

    // preview the obj2 object
    TobjText* objt = (TobjText*)obj2;
    od_str_setGC( fsod_getOD(ydev), OD_GC_Preview, objt->OTE_font, objt->OTE_pts, NULL);
    ydev_draw_obj(ydev,obj2,OD_GC_Preview);

    TparamTextCB data = { fsm, ydev, marker, .obj={ .any=obj2 } };
    Twid_dialStatus status =
            vdwM_gsls_start_CancelOk(&ymw->any,0,
                ((TobjText*)obj2)->OTE_str,".......................",
                mptext_chgtext_gsls_cb,0,&data,
                pw,1
            );
    // erase the last preview
            od_ds_xorEraseStart_0(fsm->fsm_outdevice);
            od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
    ydev_draw_obj(ydev,data.obj.any,OD_GC_Preview);
    if ( status==WID_DIALSTATUS_Ok ) {
        yfig_cursce_chgObjWHis(ymw,obj,obj_ObjAll(obj2));
    } else /* status==WID_DIALSTATUS_Cancel */ {
        yfig_cursce_addObjNHis(ymw,obj);
        obj_dir_delObjQ( obj_ObjAll(obj2) );
    }

    // draw either the original string or the new one.
    yfig_cursce_redraw(ymw);

    DBG0_EfsmL("fsm=%p obj=%p click=%d marker=(%f %f)\n",
            fsm,click,obj,marker.x, marker.y);
}

/*======================================================================*/
/*= Private utilities                                                  =*/

static Tdble2 mptext_getNextPos(
    Cyfig_device* ydev, double lsf, Tdble2 pt, Csty_text* dpt)
{
    Tvgui* gui    = fsod_getGUI(ydev);
    double angle  = dpt->dpt_angle;

    int idy,ldy,ils;
    gui_fonts_lineinfo_DesUnit(gui, dpt->dpt_font,dpt->dpt_pts, &idy,&ldy,&ils);
    double shift = idy + ils*lsf;

    Tdble2 ret;
    ret.x = pt.x;
    ret.y = pt.y + CONV_D_D2S( fsod_getOD(ydev),shift);;
    if ( angle!=0 ) {
        double rangle = angle * (TOOL_2PI / 360.);
        ROTCC_PAXY(ret,-rangle,pt.x,pt.y);
    }
    
    return ret;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

// This function is performed every time that an user writes a char
// in the text input widget.
static void mptext_chgtext_gsls_cb(void* ud, CstrBrief str)
{
    TparamTextCB*  data = ud;
    TefsmSel1Obj*  fsm  = data->fsm;
    Tyfig_device*  ydev = data->ydev;
    Tyfig_mw*      yds = (Tyfig_mw*) fsm->fsm_userdata;
    //Tsty_text*     dpt = &yds->ymw_currDP.fdp_text;

    DBG0_EfsmE("fsm=%p dev=%p str=%02x,%02x,%02x:%s",fsm,ydev,
            str[0]&0xff,str[1]&0xff,str[2]&0xff,str);
    
    // erase the old string
    ydev_draw_obj(ydev,data->obj.any,OD_GC_Preview);

    // update the text of obj text object
    double a=0,l=0,h=0;
    od_str_extents_SrcUnit((Toutdev*)ydev,
            data->obj.text->OTE_font,data->obj.text->OTE_pts,str, &a,NULL,&l,&h);
    obj_text_setTextOU(data->obj.text,str,l,h);

    // preview the new string
    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
    ydev_draw_obj(ydev,data->obj.any,OD_GC_Preview);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

// This function is performed every time that an user writes a char
// in the text input widget. It does:
//   - clear the preview of current string object by redrawing it.
//   - delete the current string object.
//   - create a new text object (with the new string) and makes
//     it the current object.
//   - draw the preview of the current text object.
static void mptext_newtext_gsls_cb(void* ud, CstrBrief str)
{
    TparamTextCB*  data = ud;
    TefsmSel1Obj*  fsm  = data->fsm;
    Tyfig_device*  ydev = data->ydev;
    Tyfig_mw*      yds = (Tyfig_mw*) fsm->fsm_userdata;
    Tsty_text*     dpt = &yds->ymw_currDP.fdp_text;
    DBG0_EfsmE("fsm=%p dev=%p str=%02x,%02x,%02x:%s",fsm,ydev, str[0]&0xff,
            str[0] ? str[1]&0xff : 0 ,str[0]&&str[1] ? str[2]&0xff : 0,str);
    if ( OBJ_IsDefined(data->obj) ) {
        ydev_draw_obj(ydev,data->obj.any,OD_GC_Preview);
        obj_dir_delObjQ( data->obj );
    } else {
        od_str_setGC( fsod_getOD(ydev), OD_GC_Preview, dpt->stt_font, dpt->stt_pts, NULL);
    }
    // get string geometry
    Tsty_text* stt = &yds->ymw_currDP.fdp_style.sty_text;
    double a=0,l=0,h=0;
    od_str_extents_SrcUnit((Toutdev*)ydev,
            stt->stt_font,stt->stt_pts,str, &a,NULL,&l,&h);
    double angle = dpt->dpt_angle;       // save angle in degree
    dpt->dpt_angle *= (TOOL_2PI / 360.); // chg to radian
    int    dir   = yds->ymw_currDP.fdp_dir;
    data->obj = obj_text_newA(&yds->ymw_currDP,
            dir,data->pos,l,h,
            str,FIGOTE_FL_DEFAULT);
    dpt->dpt_angle = angle;             // restore angle in degree

    od_ds_xorEraseStart_0(fsm->fsm_outdevice);
    od_ds_xorEraseCommit_0(fsm->fsm_outdevice);
    ydev_draw_obj(ydev,data->obj.any,OD_GC_Preview);
    DBG1_EfsmL("fsm=%p dev=%p str=%s",fsm,ydev,str);
}

/*======================================================================*/
/*= Icons                                                              =*/

CUstr mp_text_big[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xff,0xff,
    0x1f,0x00,0x80,0x83,0x1f,0x1c,0x00,0x80,0x01,0x0f,0x18,
    0x00,0x80,0x00,0x0f,0x10,0x00,0x00,0x00,0x0f,0x00,0x00,
    0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
    0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,
    0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,
    0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,
    0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,
    0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
    0x80,0x1f,0x00,0x00,0x00,0xe0,0x7f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00};

CUstr mp_text_small[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xff,0x07,
    0xfc,0xff,0x07,0x0c,0x1f,0x06,0x04,0x0e,0x04,0x00,0x0e,0x00,
    0x00,0x0e,0x00,0x00,0x0e,0x00,0x00,0x0e,0x00,0x00,0x0e,0x00,
    0x00,0x0e,0x00,0x00,0x0e,0x00,0x00,0x0e,0x00,0x00,0x0e,0x00,
    0x00,0x0e,0x00,0x00,0x3f,0x00,0xe0,0xff,0x01,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00};

/*======================================================================*/
