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
 *      $Id: tb-file-OMSF.h 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/include/ydw/tb-file-OMSF.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Dialog widget for open/merge/save-as YFIG files.                   =*/
/*=                                                                    =*/
/*= Provide:                                                           =*/
/*=  - Tdial_OMSF:     top OMSF popup widget                           =*/
/*=  - Tdial_OMSFprev: sub out-dialog that preview an YFIG file;       =*/
/*=  - Tdial_OMSFpara: sub in-dialog for entering parameters that      =*/
/*=                    specifies how the YFIG file must be loaded.     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_YDW_TB_FILE_OMSF_H
#define FILE_YDW_TB_FILE_OMSF_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "yfig-config.h"
#include "vgui/vgui-dw.h"

/*======================================================================*/
/*= Prefered geomotry for file dialog                                  =*/

// file dialog
//#define DFGEO_WIDTH  650
//#define DFGEO_HEIGHT  -1  // let external drawing engine to compute them
//#define DFGEO_HVSEP    5  // space between widget and dialog border

// preview area
#define DFGEO_PV_CANVAS_WIDTH  80
#define DFGEO_PV_CANVAS_HEIGHT 80
#define DFGEO_PV_CANVAS_HVSEP   0

/*======================================================================*/

typedef struct _Tdial_OMSFprev Tdial_OMSFprev;
struct _Tdial_OMSFprev {
    Tvgui_dw     any;
    Tfig_scene*  Fscene;

    // 1st line
    Tegs_widget   df_titleLAB;
    Tegs_widget   df_fileNameLAB;

    // 2nd line column 1
    Twid_olvu*  df_objnb;
    Twid_olvu*  df_size;
    Tegs_widget   df_commTitleLAB;
    Twid_mlout*   df_commValueTOU;

    // 2nd line column 2
    Twid_canvas* df_canvas;
};

extern Tdial_OMSFprev* dial_OMSFprev_new(Tvgui_dw*       pd);
extern void             gui_OMSFprev_dbf(Tdial_OMSFprev* dp);
extern void            dial_OMSFprev_free     (Tdial_OMSFprev* dp);

/*======================================================================*/

typedef struct _Tdial_OMSFpara Tdial_OMSFpara;
struct _Tdial_OMSFpara {
    Tvgui_dw any;

    Twid_ilvu   *df_xINP,   *df_yINP;
    Twid_olvu   *df_dx, *df_dy;
    TstrStatic   df_scaleText;
    Twid_ilvu   *df_scaleDIN;
};

extern Tdial_OMSFpara* dial_OMSFpara_new(Tvgui_dw*    pd);
extern void             gui_OMSFpara_dbf(Tdial_OMSFpara* dp);
extern void            dial_OMSFpara_free     (Tdial_OMSFpara* dp);

/*======================================================================*/

typedef enum _Tdial_OMSFkind {
    OMSF_KD_Open, OMSF_KD_Merge, OMSF_KD_SaveAs
} Tdial_OMSFkind;

typedef struct _Tdial_OMSF Tdial_OMSF;

struct _Tdial_OMSF {
    Tvgui_dw any;

    /* dialog parameters */
    Tdial_OMSFkind df_kind;  // Kind of file dialog (Open, Merge, ...)
    Tfigure*       df_fig;   // The selected figure

    // callback when a file is selected
    void (*df_fileIsSelected)(Tdial_OMSF*df,const char*path);

    /* sub dialogs */
    Tdial_OMSFprev*   df_preview; // shows a preview of the selected file.
    Tdial_OMSFpara*   df_param;   // parameters for loading the selected file.
    Twid_filebrowser* df_browser; //  the file browser itself.
};

/**
 * dial_OMSF_new allocates a dialog file structure and then
 * initializes it using the kd flag.
 *
 * Return: dial_OMSF_new return the initialized structure.
 * Parameters:
 *   - dir:    The filebrowser starts by browsing this directory.
 *   - savebn: The filebrowser shows savebn and/or dir/filebn
 *             in its file dialog.
 *             This field is only used by SaveAs filebrowsers.
 *             It can be the null pointer.
**/
extern Tdial_OMSF* dial_OMSF_new(Tyfig_mw*yds, Tdial_OMSFkind kd,
                                               CstrBrief dir, CstrBrief savebn);
extern void         gui_OMSF_newFinish(Tdial_OMSF* df);
extern void        dial_OMSF_free     (Tdial_OMSF* df);

/**
 * The dial_OMSF_getParam function retrieves the parameters
 * of the d file dialog.
 * It returns the absolute path of the selected file,
 * (x,y) the position of the NW corner of the figure after scaling,
 * s the scale factor.
 * 
 * (x,y) and s are set to (0,0) and 1 if no file is selected,
 * or if (x, y) and scale are not supported by the kind of d file dialog.
 *
 * Furthermore, you can set x, y and s to the NULL pointer if you
 * do not need their value.
**/
extern TstrEaten dial_OMSF_getParam(Tdial_OMSF*df,
                                double *x, double *y, double *s);

/**
 * The dial_OMSF_getDir function retrieves the last directory that
 * the user tryed to entered during the file browsing.
 * 
 * Return:
 *  The function returns either the path of the directory,
 *  or NULL if the directory is not readable.
**/ 
extern TstrEaten dial_OMSF_getDir(Tdial_OMSF*df);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif  // FILE_YDW_TB_FILE_OMSF_H
/*======================================================================*/

