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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: figure.h 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/figure.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= figure definition.                                                 =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_FIG_FIGURE_H
#define FILE_FIG_FIGURE_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "fig-types.h"

#include "tools/lenunit.h"
#include "tools/papersize.h"
#include "tools/color.h"

/*======================================================================*/

typedef struct _Tfig_settings {
    int                landscape;
    int                flushleft;
    TuserUnit          userUnit;
    int                grid_unit;
    Cpaperdef*         papersize;
    double             magnification;
    int                multiple;
    int                transparent;
} Tfig_settings;

typedef const Tfig_settings Cfig_settings;

/*======================================================================*/
/*= Tfig_objCounters: counts figure objects by object kind.            =*/

struct _Tfig_objCounters {
    unsigned num_arcs;       /* # arcs at this depth */
    unsigned num_lines;      /* # lines at this depth */
    unsigned num_ellipses;   /* # ellipses at this depth */
    unsigned num_splines;    /* # splines at this depth */
    unsigned num_texts;      /* # texts at this depth */
    unsigned num_others;     /* # no compound and no formers */
};

/**
 * The fig_objcnt_sum function returns the object nubers of the f figure.
 * The directory objects are not taken in account.
 *
 * The fig_objcnt_attachObj function attaches the o object to the f figure.
 * The fig_objcnt_detachObj function attaches the o object to the f figure.
**/
extern int  fig_objcnt_sum( const Tfig_objCounters* f);
extern void fig_objcnt_attachObj(Tfig_objCounters* f, Cobject* o);
extern void fig_objcnt_detachObj(Tfig_objCounters* f, Cobject* o);

/*======================================================================*/
/*= Tfig_colors: colors of a figure.                                   =*/

#define FIG_NbPredefinedColor 32

struct _Tfig_colors {
    CcolorSet* predefinedColors;  // all the figures share these colors
    TcolorSet* privateColors;     // private colors of the figure
};

/**
 * fig_colors_getByUKey selects the color the user key of which is key.
 * If key is less than FIG_NbPredefinedColor the color is searched into
 * the colors->predefinedColors set otherwise into the
 * colors->privateColors set.
 * Return value: the color if it exists otherwise the null pointer.
 *
 * fig_colors_getByUKey_G is similar to fig_color_getByUKey except that
 * it returns the gcolor field of the color or VGUI_COLOR_NONE if the color
 * does not exist.
**/
extern cTcolor*   fig_colors_getByUKey(  const Tfig_colors* colors, int key);
extern Tegs_color fig_colors_getByUKeyEGS(const Tfig_colors* colors, int key);

/**
 * The fig_colors_getByClr function searches for a color close to the c color
 * in the color set of the f figure.
 * If it found a such color, it returns it, otherwise it adds a clone of
 * the c color to the f figure colors and returns the clone color.
**/
extern Ccolor*    fig_colors_getByClr(Tfigure* f, Ccolor*c);

// return 1 if c is added, otherwise return 0 ( a close color already exists
// in colors set).
// If a  color is added, it is a private clone of c (color_dupPriv(c)).
extern int     fig_colors_add0(Tfig_colors* colors, Ccolor*c);
// return the color of colors set close to c or NULL
extern Ccolor* fig_colors_get0(Tfig_colors* colors, Ccolor*c);

/**
 * The fig_colors_newSharedColor function creates the set of shared colors.
 * It contains FIG_NbPredefinedColor, with user key in range
 * [0:FIG_NbPredefinedColor-1].
 *
 * If drv driver is null, the gcolor field of colors is set to
 * VGUI_COLOR_NONE.
**/
extern TcolorSet* fig_colors_newSharedColor(CcolorDriver*drv);

extern Ccolor*    fig_colors_getBlack  (CcolorSet* s);
extern Ccolor*    fig_colors_getBlue   (CcolorSet* s);
extern Ccolor*    fig_colors_getGreen  (CcolorSet* s);
extern Ccolor*    fig_colors_getCyan   (CcolorSet* s);
extern Ccolor*    fig_colors_getRed    (CcolorSet* s);
extern Ccolor*    fig_colors_getMagenta(CcolorSet* s);
extern Ccolor*    fig_colors_getYellow (CcolorSet* s);
extern Ccolor*    fig_colors_getWhite  (CcolorSet* s);
extern Ccolor*    fig_colors_getBlue4  (CcolorSet* s);
extern Ccolor*    fig_colors_getBlue3  (CcolorSet* s);
extern Ccolor*    fig_colors_getBlue2  (CcolorSet* s);
extern Ccolor*    fig_colors_getLtBlue (CcolorSet* s);

/*======================================================================*/

struct _Tfigure {
    char*              path;          // Path to load/save file
    int                loaded_format; // xfig format of loaded path file
    int                save_format;   // xfig format to use for saving fig
                                      // into path file. (0=uninitialized)
    int                io_ppi;        // unit in ppi of coordinates in
                                      // file.
 
    Tfig_settings      settings;
    Tfig_objCounters objCounters;
    Tfig_colors      colors;
    double           fig_ppi; // the unit of coordinates and length used
                              // in objects l=254 fig_ppi--> 254/fig_ppi inch 
                              // Usually sets to FIG_UNIT_PpiDefault.
    TobjDir          objects; // the root directory of the figures

    // An history where figure modifications can be stored.
    // By default history is null and the FIG API does not save
    // anything automatically.
    Tfig_histo*      history;
    int              refcnt;    // reference counter

    Tmsgdrv*         msgdrv;    // driver for printing messages
                                // not handled by module
                                // null redirect to gl_msgdrv_default
};

/*======================================================================*/
/*= creation / reading / saving / free figures                         =*/

/**
 * The fig_new_empty function creates an empty figure:
 *   - it sets the associated file name to the n string.
 *   - it sets the general setting to gp.
 *   - it sets the shared color-map to sc.
 *
 * The fig_new_lazy function increments the reference counter
 * of the fig figure.
 *
 * The fig_new_clone function clones the fig figure.
 * The cloned figure has no history.
 *
 * The fig_delete function decrements the reference counter.
 * If the reference counter reachs zero then it frees all the memory
 * that the fig figure allocated.
 * 
 * Return:
 *  - On succes, the fig_new_empty and fig_new_clone functions return
 *    the created figure otherwise they exit.
 *  - The fig_new_lazy function returns always the fig argument.
**/
extern Tfigure* fig_new_empty(Tmsgdrv*md, CstrBrief n, Cfig_settings* gp, CcolorSet* sc);
extern Tfigure* fig_new_lazy (Tfigure* fig);
extern Tfigure* fig_new_clone(Cfigure* fig);
extern void     fig_delete   (Tfigure* fig);
Inline void     fig_deletePt (Tfigure**fig) { if (*fig) fig_delete(*fig); *fig=NULL; } 

/**
 * The fig_new_byFile function creates a new figure and fills in it
 * with the content of the path file.
 * The predefined colors of the figure are set to the sc color set.
 * Message printing are controled by the md message driver.
 * When an error or a problem occurs, the fig_new_byFile function rises
 * error messages using the md message handler.
 *
 * The fig_new_byStream function is similar to the former function
 * except that the caller must provide an opened stream in reading mode.
 * This stream corresponds usually to the path file.
 *
 * Return:
 * The functions return the figure on success otherwise the NULL pointer.
 * The failure cases (NULL pointer) are:
 *  - path file does not exists.
 *  - path file is not readable (a read error occurred).
 *  - path file is bad formatted.
 *
 * Parameter:
 *  - md:     A message handler or NULL for the default message handler.
 *  - path:   The file name of figure file.
 *  - sc:     The predefined colors.
 *  - stream: The figure description is read from this stream.
**/
extern Tfigure* fig_new_byFile  (Tmsgdrv*md, CstrBrief path, CcolorSet* sc);
extern Tfigure* fig_new_byStream(Tmsgdrv*md, CstrBrief path, CcolorSet* sc,
                                 FILE* stream);

/**
 * The fig_save_byFile function backups the fig figure into the fig->path
 * file under the fig->save_format format.
 * If the fig->save_format field is zero, the fig->loaded format format
 * is used.
 * If the df flag is set and the output format is unspecified or not
 * supported then the default output format will be used.
 *
 * The fig_save_byStream function is similar to the former function except
 * that the backup is written into the s stream.
 *
 * Return:
 *  - The fig_save_by/File/Stream functions return 0 on success, otherwise:
 *      -1: The output format is not supported or not specified.
 *      -2: Opening the fig->path file for writing failed.
 *      -3: Write error.
 *
 * Parameters:
 *  md : The message driver that is used for printing error messages.
 *      If it is set to NULL, gl_msgdrv_default is used.
 *  fig: The figure to backup.
 *  df : Set it to 1 for using the default format when
 *        - no format is specified
 *        - or the specified format is unsupported.
**/
extern int fig_save_byFile  (Tmsgdrv*md, Cfigure*fig, int df);
extern int fig_save_byStream(Tmsgdrv*md, Cfigure*fig, int df, FILE*s);

/*======================================================================*/
/*= Miscellaneous figure services.                                     =*/

extern void fig_chgPath    (Tfigure* fig, CstrBrief path);
extern void fig_chgUnit    (Tfigure* fig, double ppi);
extern void fig_clrEGScache(Tfigure* fig, void *od);

/**
 * Figure loaded from file. Its objects contains yet raw data.
 *
 * The older versions of xfig (1.3 to 2.1) used values that ended in 4 or 9
 * for coordinates on the "grid".  When multiplied by 15 for the 3.0
 * resolution these values ended up 14 "new" pixels off the grid.

 * For 3.0 files, 1 is added to the coordinates, and in addition, the USER
 * is supposed to set the x and y offset in the file panel both to the
 * amount necessary to correct the problem.
 * For older files 1 is first added to coordinates then they are multiplied by 15.
**/
extern void fig_update_forDrawing(Tfigure* fig, CcolorDriver*drv);

/*======================================================================*/
/*= fig backup format.                                                 =*/

/**
 * The fig_filefmt_default function gives the default output format of
 * this YFIG release.
 *
 * The fig_filefmt_defaultFig function gives the default output format
 * for the fig figure.
 *
 * The fig_filefmt_get2 function checks if at least one of the first
 * and second output formats is usable for the fig figure.
 *
 * Return:
 *  - The fig_filefmt_default and fig_filefmt_defaultFig functions
 *    always return the name of an output format.
 *  - The fig_filefmt_get2 function returns first if the first output
 *    format is usable for the fig figure, otherwise second if its is
 *    usable, otherwise zero.
**/
extern CstrStatic fig_filefmt_default();
extern CstrStatic fig_filefmt_defaultFig(Cfigure* fig);
extern int        fig_filefmt_get2(Cfigure*fig, int first, int second);

/**
 * The fig_filefmt_int2str function converts the fid YFIG file format
 * identifier into an human readable name.
 *
 * The fig_filefmt_int2str function converts the fstr string
 * corresponding to an YFIG file format to an numeric identifier.
 *
 * Return:
 *   On success, the fig_filefmt_int2str function returns the file
 *   format name, otherwise the "undef" string.
 *   On success, the fig_filefmt_str2int function returns the file
 *   format identifier (positive integer), otherwise it returns 0.
**/
extern CstrStatic  fig_filefmt_int2str(int       fmt);
extern int         fig_filefmt_str2int(CstrBrief fmt);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif /* FILE_FIG_FIGURE_H */
/*======================================================================*/

