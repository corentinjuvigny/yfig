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
 *$Software: YFIG
 *      $Id: yfig-drawparam.h 368 2018-11-19 00:23:18Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/yfig-drawparam.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The widget collection that allow the user to change parameters     =*/
/*=   - for modifying how the scene is displayed (zoom factor)         =*/
/*=   - for creating or modifying user object (line width, font, ...). =*/
/*=     Those are defined in the Tfdp structure (see fig-drawparam.h). =*/
/*=   - for positioning or rotating user object (line spacing, ...).   =*/
/*=     Those are defined to yfig and used in the mode panel controler.=*/ 
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YFIG_DRAWPARAM_H
#define FILE_YFIG_DRAWPARAM_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-types.h"

// update flags: they are really defined in fig-types.h.
//#define FDP_A_mode  FDP_U_... // update arrows
//#define FDP_A_beg   FDP_U_... // can have a begin arrow
//#define FDP_A_end   FDP_U_... // can have an end arrow
//#define FDP_A_all   (FDP_A_mode|FDP_A_beg|FDP_A_end)

// update flags masking all the parameters that are not pure style
#define FDP_O_all  FDP_U_all

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Tyfig_drawparam Tyfig_drawparam;
typedef const   Tyfig_drawparam Cyfig_drawparam;

struct _Tyfig_drawparam {
  /* general parameters */
#define DPP_SHOW_Update (1<<0)
#define DPP_SHOW_Zoom   (1<<1)
#define DPP_SHOW_Depth  (1<<2)
#define DPP_SHOW_Angle  (1<<3)
     Tvgui_dw*    dp_update;
     Twid_button* dp_gridP;
     Twid_button* dp_gridM;
     Twid_button* dp_zoom;
     Twid_button* dp_depth;
     Twid_button* dp_angle;
#define DPP_SHOW_Std  (DPP_SHOW_Zoom|DPP_SHOW_Depth)

  /* align parameters */
#define DPP_SHOW_Align (1<<9)
     Twid_buttonPanel* dp_alignpanel;
     Twid_button*      dp_alignH;
     Twid_button*      dp_alignV;

  /* line parameters */
#define DPP_SHOW_Curve  (1<<4)
#define DPP_SHOW_Line  ((1<<5)|(1<<4))
#define DPP_SHOW_Arrow  (1<<6)
     Twid_buttonPanel* dp_linepanel;
     Twid_button*      dp_linewidth;
     Twid_button*      dp_linecolor;
     Twid_button*      dp_linestyle;
   //Twid_button*      dp_lineothers;
     Twid_button*      dp_linearrow;
#define DPP_SHOW_CurveFillNA    (DPP_SHOW_Curve         | DPP_SHOW_Fill)
#define DPP_SHOW_LineFillNA     (DPP_SHOW_Line          | DPP_SHOW_Fill)
#define DPP_SHOW_StdCurveFillNA (DPP_SHOW_CurveFillNA   | DPP_SHOW_Std)
#define DPP_SHOW_StdLineFillNA  (DPP_SHOW_LineFillNA    | DPP_SHOW_Std)

#define DPP_SHOW_CurveFillWA    (DPP_SHOW_CurveFillNA   | DPP_SHOW_Arrow)
#define DPP_SHOW_LineFillWA     (DPP_SHOW_LineFillNA    | DPP_SHOW_Arrow)
#define DPP_SHOW_StdCurveFillWA (DPP_SHOW_StdCurveFillNA| DPP_SHOW_Arrow)
#define DPP_SHOW_StdLineFillWA  (DPP_SHOW_StdLineFillNA | DPP_SHOW_Arrow)

  /* fill parameters */
#define DPP_SHOW_Fill  (1<<7)
     Twid_buttonPanel* dp_fillpanel;
     Twid_button*      dp_fillcolor;
     Twid_button*      dp_filltile;

  /* text parameters */
#define DPP_SHOW_Text  (1<<8)
     Twid_buttonPanel* dp_textpanel;
     Twid_button*      dp_textdir;
     Twid_button*      dp_textcolor;
     Twid_button*      dp_textsize;
     Twid_button*      dp_textfont;
     Twid_button*      dp_textangle;
     Twid_button*      dp_textLSF;    // yfig param (Line Spacing Factor)
};

/*======================================================================*/
/*= Interface to select the parameter buttons that are shown           =*/
/*= in the panel.                                                      =*/

/**
 * The yfig_dpp_show function makes visible in the ddp button panel
 * of the ymw widget the buttons that the mask parameter defines.
 *
 * The yfig_dpp_show function makes invisible in the ddp button panel
 * of the ymw widget the buttons that the mask parameter defines.
 *
 * The yfig_dpp_show function makes visible in the ddp button panel
 * of the ymw widget the buttons that the mask parameter defines.
 * The function makes also invisible the buttons that the mask parameter
 * does not define.
 *
 * The mask parameter is composed of DPP_SHOW_Xxx macros that must be
 * bitwise-or'd.
**/
extern void yfig_dpp_show     (Cyfig_mw* ymw, int mask);
extern void yfig_dpp_unshow   (Cyfig_mw* ymw, int mask);
extern void yfig_dpp_showonly (Cyfig_mw* ymw, int mask);

/*======================================================================*/
/*= Interface to get/change parameters                                 =*/

extern double yfig_mw_getZoom (Cyfig_mw* ymw);
extern void   yfig_mw_setZoom (Cyfig_mw* ymw, double val);
extern double yfig_mw_getGridShow(Cyfig_mw* ymw);
extern double yfig_mw_getGridMag (Cyfig_mw* ymw);
extern int    yfig_mw_getAlignOpe(Cyfig_mw* ymw);

extern void   yfig_mw_setDepth   (Cyfig_mw* ymw, int val);
extern void   yfig_mw_setDepth   (Cyfig_mw* ymw, int val);
extern double yfig_mw_getAngleDeg(Cyfig_mw* ymw);
extern double yfig_mw_getAngleRad(Cyfig_mw* ymw);
extern void   yfig_mw_setAngle   (Cyfig_mw* ymw, double val);
extern double yfig_mw_getLSF     (Cyfig_mw* ymw);
extern double yfig_mw_setLSF     (Cyfig_mw* ymw);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_YFIG_DRAWPARAM_H
/*======================================================================*/
