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
 *      $Id: modepanel.c 372 2018-11-26 07:15:38Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/modepanel.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the mode panel.                                        =*/
/*=                                                                    =*/
/*= See the "modepanel/modepanel.h" header for documentation.          =*/
/*======================================================================*/

#include "modepanel/modepanel.h"

#include "vgui/vgui-resources.h"
#include "vgui/vgui-pixmap.h"
#include "vgui/vgui-accelkey.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/wid-canvas.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void yfig_modePanel_addMode(Tyfig_mw* ymw,
                 Twid_buttonPanel* panel, int small);
static int yfig_modePanel_callback(
        Twid_buttonPanel*self, Twid_button*next);

extern Twid_buttonPanel* yfig_modePanel_nbf(Tyfig_mw* ymw)
{
    int small;
    if ( res_getInt(&gui_appResources,VGUI_ResN_SmallIcons,&small)!=0 ) small = 0;
    int butdx = small ? MP_BM_SMALL_DX : MP_BM_BIG_DX;
    int butdy = small ? MP_BM_SMALL_DY : MP_BM_BIG_DY;

    Twid_buttonPanel* panel;
    panel                   = widM_buttonPanel_newRaCa(&ymw->any, butdx, butdy);
    panel->vgbpm_callback   = yfig_modePanel_callback;
    yfig_modePanel_addMode(ymw, panel, small);
    //panel->vgbpm_widEGS    = gui_buttonPanel_build(modePannel,NULL);
    wid_buttonPanel_build(panel,NULL);
    wid_buttonPanel_finish(panel);
    return panel;
}

static void yfig_modePanel_addMode(Tyfig_mw* ymw,
                 Twid_buttonPanel* panel, int small)
{
#define BUTTON(id)  dx,dy,small ? mp_##id##_small : mp_##id##_big
    Tvgui* gui = ymw->any.gui;
    int    dx  = small ? MP_BM_SMALL_DX  : MP_BM_BIG_DX;
    int    dy  = small ? MP_BM_SMALL_DY  : MP_BM_BIG_DY;

    Tvgui_icon*  icon;
    Twid_button* butt;

    /* DRAWING MODES */
    butt = wid_buttonPanel_addDecorativeLabel(panel,"Drawing");

    icon = vgui_icon_newBitmap(BUTTON(ellrad));
    butt = widM_button_onoff_newIco(&ymw->any,
            "CIRCLE/ELLIPSE drawing: specify RADII",
            mp_ellrad_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('c');
    butt->but_accelKey1 = GL_ACCEL_Key('e');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(elldia));
    butt = widM_button_onoff_newIco(&ymw->any,
            "CIRCLE/ELLIPSE drawing: specify DIAMETER(S)",
            mp_elldia_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('c')|GL_ACCEL_Shift;
    butt->but_accelKey1 = GL_ACCEL_Key('e')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(splineclosed));
    butt = widM_button_onoff_newIco(&ymw->any,
            "CLOSED APPROXIMATED SPLINE drawing: specify control points",
            mp_splineclosed_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('s')|GL_ACCEL_Shift; // shift-s
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(splineopen));
    butt = widM_button_onoff_newIco(&ymw->any,
            "APPROXIMATED SPLINE drawing: specify control points",
            mp_splineopen_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('s'); // s key
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(splineclosedInt));
    butt = widM_button_onoff_newIco(&ymw->any,
            "CLOSED INTERPOLATED SPLINE drawing: specify control points",
            mp_splineclosedInt_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('i')|GL_ACCEL_Shift; // shift-i
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(splineopenInt));
    butt = widM_button_onoff_newIco(&ymw->any,
            "INTERPOLATED SPLINE drawing: specify control points",
            mp_splineopenInt_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('i'); // i key
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(lineclosed));
    butt = widM_button_onoff_newIco(&ymw->any,
            "POLYGON drawing",
            mp_lineclosed_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('p');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(lineopen));
    butt = widM_button_onoff_newIco(&ymw->any,
            "MULTI-LINE drawing",
            mp_lineopen_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('l');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(sqrrect));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Rectangular BOX drawing",
            mp_sqrrect_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('b');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(arcrect));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Rectangular BOX drawing with rounded corners",
            mp_arcrect_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('b')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(arcopen));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ARC drawing: specify three points on the arc",
            mp_arcopen_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('r')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&regpoly_ic, F_REGPOLY, regpoly_drawing_selected, M_NONE,
// FIXME:        I_REGPOLY,
// FIXME:        "Regular Polygon   (Shift-p)", False},

    icon = vgui_icon_newBitmap(BUTTON(arcpop));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Pie piece drawing: specify three points on the arc",
            mp_arcpop_cb,NULL,
            NULL,
            dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('r');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(image));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Image/Picture drawing (Ctrl p)",
            mp_image_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('p')|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(text));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Text input from keyboard   (t)",
            mp_text_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('t');
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&library_ic, F_PLACE_LIB_OBJ, sel_place_lib_obj, M_NONE,
// FIXME:        I_MIN2,
// FIXME:        "PLACE a library element   (Shift-l)", True},		/* popups a panel */

    /* EDITING MODES FOLLOW */
    butt = wid_buttonPanel_addDecorativeLabel(panel,"Editing");
    
    icon = vgui_icon_newBitmap(BUTTON(cpdDo));
    butt = widM_button_onoff_newIco(&ymw->any,
            "GLUE objects into COMPOUND object",
            mp_cpdDo_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('g');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(cpdBreak));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Break COMPOUND object",
            mp_cpdBreak_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('g')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(cpdOpen));
    butt = widM_button_onoff_newIco(&ymw->any,
            "OPEN COMPOUND object",
            mp_cpdOpen_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('o');
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&join_split_ic, F_JOIN, join_split_selected, M_POLYLINE | M_SPLINE_O |M_SPLINE_C,
// FIXME:        0,
// FIXME:        "Join or Split lines/splines   (j)", False},
// FIXME:     {&chop_ic, F_CHOP, chop_selected, M_POLYLINE | M_ARC |M_ELLIPSE,  /* maybe spline someday */
// FIXME:        I_CHOP,
// FIXME:        "Chop objects   (x)", False},

    icon = vgui_icon_newBitmap(BUTTON(zoom));
    butt = widM_button_onoff_newIco(&ymw->any,
            "SCALE/ZOOM objects",
            mp_zoom_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('s');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(align));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ALIGN objects within a COMPOUND or CANVAS",
            mp_align_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('a');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(mkrmov));
    butt = widM_button_onoff_newIco(&ymw->any,
            "MOVE object point",
            mp_mkrmov_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('m')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(move));
    butt = widM_button_onoff_newIco(&ymw->any,
            "MOVE objects",
            mp_move_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('m');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(mkradd));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ADD POINTs to lines, polygons and splines",
            mp_mkradd_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('a')|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(copy));
    butt = widM_button_onoff_newIco(&ymw->any,
            "COPY objects",
            mp_copy_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('c')|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(mkrdel));
    butt = widM_button_onoff_newIco(&ymw->any,
            "DELETE POINTs from lines, polygons and splines",
            mp_mkrdel_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('d')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(delete));
    butt = widM_button_onoff_newIco(&ymw->any,
            "DELETE objects",
            mp_delete_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('d');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(update));
    butt = widM_button_onoff_newIco(&ymw->any,
            "UPDATE OBJECT <-> current settings",
            mp_update_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('u');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(edit));
    butt = widM_button_onoff_newIco(&ymw->any,
            "CHANGE OBJECT via an edition panel",
            mp_edit_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('e');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(flipv));
    butt = widM_button_onoff_newIco(&ymw->any,
            "FLIP objects up or down",
            mp_flipv_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('f');
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(fliph));
    butt = widM_button_onoff_newIco(&ymw->any,
            "FLIP objects left or right",
            mp_fliph_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('f')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(rotCW));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ROTATE objects clockwise by quarter turn",
            mp_rotCW_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('r')|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(rotCCW));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ROTATE objects counter-clockwise by quarter turn",
            mp_rotCCW_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('r')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

    icon = vgui_icon_newBitmap(BUTTON(rota));
    butt = widM_button_onoff_newIco(&ymw->any,
            "ROTATE objects by any angle",
            mp_rota_cb,NULL, NULL, dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('r')|GL_ACCEL_Shift|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&convert_ic, F_CONVERT, convert_selected, M_VARPTS_OBJECT | M_POLYLINE_BOX, 
// FIXME:        0,
// FIXME:        "CONVERSION between lines, polygons and splines   (v)", False},

    icon = vgui_icon_newBitmap(BUTTON(arrowheads));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Add/Delete arrow heads",
            mp_arrowheads_cb,NULL,NULL,dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('a')|GL_ACCEL_Shift;
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&tangent_ic, F_TANGENT, tangent_selected, M_TANGENT_OBJECT,
// FIXME:        I_TANGENT,
// FIXME:        "Add TANGENT/NORMAL to curve   (n)", False},
// FIXME:     {&anglemeas_ic, F_ANGLEMEAS, anglemeas_selected, M_ANGLEMEAS_OBJECT,
// FIXME:        I_MIN2,
// FIXME:        "MEASURE angle (specify three points or select object)   (Ctrl-g)", False},

    icon = vgui_icon_newBitmap(BUTTON(measurelength));
    butt = widM_button_onoff_newIco(&ymw->any,
            "Measure LENGTH of polylines, arcs and ellipses",
            mp_measurelength_cb,NULL,NULL,dx,dy,icon);
    vgui_icon_free(icon);
    butt->but_accelKey0 = GL_ACCEL_Key('n')|GL_ACCEL_Control;
    wid_buttonPanel_addButton(panel,butt);

// FIXME:     {&areameas_ic, F_AREAMEAS, areameas_selected, M_AREAMEAS_OBJECT,
// FIXME:        I_MIN2,
// FIXME:        "Measure AREA of polygons, arcs and ellipses   (Ctrl-m)", False},
}

/*======================================================================*/
/*= Local utilities                                                    =*/

#define Automata ymw->ymw_canvas->vgcm_automata

extern void yfig_modePanel_chgMode(
        Twid_buttonPanel*self, Twid_button*next)
{
    Tvgui*    gui = self->vgbpm_gui;
    Tyfig_mw* ymw = (Tyfig_mw*)self->vgbpm_vmw;

    if ( Automata ) {
        vgui_efsm_delete(Automata);
        Automata=0;
    }

    if (next->but_cbMw==0)
        return ;
    next->but_cbMw(next->but_vmw, next->but_uid, &ymw->ymw_canvas->vgcm_efsm);
    if ( Automata!=0 ) {
        Automata->transitionFunc(Automata,EFSMIN_Init,0,0);
    }
}

static int yfig_modePanel_callback(
        Twid_buttonPanel*self, Twid_button*next)
{
    Tvgui*    gui = self->vgbpm_gui;
    Tyfig_mw* ymw = (Tyfig_mw*)self->vgbpm_vmw;

    // if there is no active button, and an automata --> big problem.
    TEMP_asf_printf(self->vgbpm_activeButton==0 && Automata,
      "no active button and an automata is running (set it to zero).",0);

    // turn off current automata if it is possible
    if ( Automata )
        Automata->transitionFunc(Automata,EFSMIN_Quit,0,0);

    if ( Automata && Automata->state!=EFSMST_END ) {
        // current FSM does not want to quit
        // (probably waiting an other user input).
        return 0;
    }

    yfig_modePanel_chgMode(self,next);

    if (next->but_cbMw==0)
        return 0;
    else
        return 1;
}

/*======================================================================*/

