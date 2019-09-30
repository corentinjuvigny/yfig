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
 *      $Id: ydw-objedit-header.c 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/ydw/ydw-objedit-header.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Sub-dialog header. All the dialogues that edit figure objects must =*/
/*= place it at their top.                                             =*/
/*=                                                                    =*/
/*= See the "ydw/ydw.h" header for documentation.                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "ydw/ydw-objedit.h"

#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Data structure                                                     =*/

struct _Toed_header {
    Tvgui_dw      any;
    Tegs_widget   dw_titleLab; // label
    Tegs_widget   dw_titlePxm; // label
    TpixmapEGS    dw_titlePxmDat; // the pixmap of dw_titlePxm
    Twid_mlout*   dw_comment;  // FIXME: must be changed in mlin when
                               // mlin will exist
} ;

typedef struct _Toed_header Toed_header;
typedef const   Toed_header Coed_header;

/*======================================================================*/
/*= Interface                                                          =*/

static CstrStatic  obj_kind_str(int kind, int type);
static TpixmapEGS  obj_kind_pxm(int kind, int type, Tvgui*gui);

extern Tvgui_dw* yoed_any_header_nbf    (Tyoed_any* oed)
//extern Tvgui_dw* ydw_objedit_header_nbf   (Tvgui_dw* vdw, Cobject obj, int opt)
{
    Tvgui_dw*   vdw   = (Tvgui_dw*)oed;
    Tvgui*      gui   = vgui_dw_getVGui(vdw);

    // vidget creation
    Toed_header* self = (Toed_header*) vgui_dw_newSub(sizeof(*self), vdw, NULL );
    self->dw_titleLab = widD_label_nbf(SelfA, obj_kind_str(oed->dw_kind,oed->dw_type), NULL );
    self->dw_titlePxmDat  = obj_kind_pxm(oed->dw_kind,oed->dw_type,gui);
    if ( PXMEGS_IsNone(self->dw_titlePxmDat) ) {
        self->dw_titlePxm = widD_label_nbf(SelfA, " ??? ", NULL );
    } else {
        self->dw_titlePxm = widD_label_nbf(SelfA, NULL, NULL );
        wid_label_setPixmap(self->dw_titlePxm, self->dw_titlePxmDat);
    }
    self->dw_comment = widD_mlout_nbf(SelfA,0,3,1);

    // vidget layout
#   define LN(x)   VDWLF_ROW(x)
#   define EH0     VDWLF_EH
#   define EH2     VDWLF_EH|VDWLF_SEP2
#   define EH3     VDWLF_EH|VDWLF_SEP3
#   define SEP0    0
#   define SEP2    VDWLF_SEP2
#   define SEP3    VDWLF_SEP3
#   define OPXM    self->dw_titlePxm
#   define ODES    self->dw_titleLab
#   define OCOM    self->dw_comment->widEGS
    vgui_dw_finishMTX(&self->any
     ,LN(0)       ,0    ,OPXM  ,1|VDWLF_EH,ODES
     ,LN(1)|SEP0  ,0|EH0,OCOM  ,1|VDWLF_EH,OCOM
     ,-1);

    return SelfA;
}

extern void      yoed_any_header_delete(Tvgui_dw*vdw)
{
    Tvgui*       gui  = vgui_dw_getVGui(vdw);
    Toed_header* self = (Toed_header*)vdw;
    if ( ! PXMEGS_IsNone(self->dw_titlePxmDat) ) 
        gui_pixmap_free(gui,self->dw_titlePxmDat);
    wid_mlout_free(self->dw_comment);
    vgui_dw_free  (vdw);
}

extern void      yoed_any_header_update(Tvgui_dw*vdw);

/*======================================================================*/
/*= Private utilities                                                  =*/

#include "../modepanel/modepanel.h"
#include "vgui/vgui.h"
#include "fig/object.h"

static TpixmapEGS  obj_kind_pxm(int kind, int type, Tvgui*gui)
{
    CUstr*data;
#define bits(x) data = mp_##x##_big
#define mp_undef_big NULL

    switch (kind) {
        case FIG_KD_Arc:
            switch (type) {
                case FIGOAR_T_Open:      bits(arcopen); break;
                case FIGOAR_T_PiePiece:  bits(arcpop); break;
                default:
                    TEMP_spe_printf("unrecognized arc type %d",type);
                    bits(undef);
            }
            break;
        case FIG_KD_Boxhv:
            switch (type) {
                case FIGOBHV_T_Image:   bits(image); break;
                default:
                    TEMP_spe_printf("unrecognized bowhv type %d",type);
                    bits(undef);
            }
            break;
        case FIG_KD_Ellipse:
            switch (type) {
                case FIGOELL_T_EllByRad: bits(ellrad); break;
                case FIGOELL_T_EllByDia: bits(elldia); break;
                case FIGOELL_T_CirByRad: bits(ellrad); break;
                case FIGOELL_T_CirByDia: bits(elldia); break;
                default:
                    TEMP_spe_printf("unrecognized ellipse type %d",type);
                    bits(undef);
            }
            break;
        case FIG_KD_Line:
            switch (type) {
                case FIGOLI_T_MLine:    bits(lineopen);   break;
                case FIGOLI_T_Box:      bits(undef);      break;
                case FIGOLI_T_Polygon:  bits(lineclosed); break;
                case FIGOLI_T_RoundBox: bits(undef);      break;
                default:
                    TEMP_spe_printf("unrecognized line type %d",type);
                    bits(undef);
            }
            break;
        case FIG_KD_Spline:
            switch (type) {
                case FIGOSP_T_OpenApprox:   bits(splineopen);      break;
                case FIGOSP_T_ClosedApprox: bits(splineclosed);    break;
                case FIGOSP_T_OpenInterp:   bits(splineopenInt);   break;
                case FIGOSP_T_ClosedInterp: bits(splineclosedInt); break;
                default:
                    TEMP_spe_printf("unrecognized spline type %d",type);
                    bits(undef);
            }
            break;
        case FIG_KD_Text:
            bits(text);
            break;
        case FIG_KD_Compound:
            bits(cpdDo);
            break;
        default:
            TEMP_spe_printf("unrecognized object kind %d",kind);
            bits(undef);
    }
    if ( data==0 )
        return PXMEGS_none;

    Tvgui_icon*  icon;
    TpixmapEGS  pxm;
    icon = vgui_icon_newBitmap(MP_BM_BIG_DX,MP_BM_BIG_DY,data);
    pxm = vgui_pixmap_create(gui,
        gui->vg_dialForegroundBtnOff, gui->vg_dialBackgroundBtnOff,icon);
    vgui_icon_free( icon );
    return pxm;
}

static CstrStatic  obj_kind_str(int kind, int type)
{
    switch (kind) {
        case FIG_KD_Arc:
            switch (type) {
                case FIGOAR_T_Open:
                    return "ARC specified by 3 points"; 
                case FIGOAR_T_PiePiece:
                    return "PIECE OF PIE specified by 3 points"; 
                default:
                    TEMP_spe_printf("unrecognized arc type %d",type);
                    return "ARC specified by ????"; 
            }
        case FIG_KD_Boxhv:
            switch (type) {
                case FIGOBHV_T_Image:
                    return "Image"; 
                default:
                    TEMP_spe_printf("unrecognized bowhv type %d",type);
                    return "box ????"; 
            }
        case FIG_KD_Ellipse:
            switch (type) {
                case FIGOELL_T_EllByRad:
                    return "ELLIPSE specified by radius"; 
                case FIGOELL_T_EllByDia:
                    return "ELLIPSE specified by bounding rectangle"; 
                case FIGOELL_T_CirByRad:
                    return "CIRCLE specified by radius"; 
                case FIGOELL_T_CirByDia:
                    return "CIRCLE specified by bounding square"; 
                default:
                    TEMP_spe_printf("unrecognized ellipse type %d",type);
                    return "CIRCLE OR ELLIPSE specified by ????"; 
            }
        case FIG_KD_Line:
            switch (type) {
                case FIGOLI_T_MLine:
                    return "MLINE: Open multi-line"; 
                case FIGOLI_T_Box:
                    return "MLINE: Rectangular box"; 
                case FIGOLI_T_Polygon:
                    return "MLINE: Polygon"; 
                case FIGOLI_T_RoundBox:
                    return "MLINE: Rectangular box with rounded corners"; 
                default:
                    TEMP_spe_printf("unrecognized line type %d",type);
                    return "MLINE:  ????"; 
            }
        case FIG_KD_Spline:
            switch (type) {
                case FIGOSP_T_OpenApprox:
                    return "SPLINE: Open approximated spline";
                case FIGOSP_T_ClosedApprox:
                    return "SPLINE: Closed approximated spline";
                case FIGOSP_T_OpenInterp:
                    return "SPLINE: Open interpolated spline";
                case FIGOSP_T_ClosedInterp:
                    return "SPLINE: Closed interpolated spline";
                default:
                    TEMP_spe_printf("unrecognized spline type %d",type);
                    return "SPLINE:  ????"; 
            }
        case FIG_KD_Text:
            return "TEXT";
        case FIG_KD_Compound:
            return "COMPOUND";
        default:
            TEMP_spe_printf("unrecognized object kind %d",kind);
            return "????";
    }
}

/*======================================================================*/
