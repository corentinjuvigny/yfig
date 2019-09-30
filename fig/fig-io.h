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
 *      $Id: fig-io.h 127 2018-04-14 10:30:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-io.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Interface between                                                  =*/
/*=   - the general IO drivers provides by the fig_save.c and          =*/
/*=     fig-load.c files,                                              =*/
/*=   - the specialized IO drivers that stay in the io directory.      =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_FIG_IO_H
#define FILE_FIG_IO_H
/*======================================================================*/

#include "fig-config.h"
#include "yfig-version.h"
#include "fig/figure.h"
#include "fig/object-priv.h"
#include "fig/obj-arc.h"
#include "fig/obj-ellipse.h"
#include "fig/obj-line.h"
#include "fig/obj-spline.h"
#include "fig/obj-text.h"
#include "fig/obj-boxhv.h"
#include "fig/obj-arrow.h"
#include "fig/obj-compound.h"

#include "tools/globalvars.h"
#include "tools/font.h"
#include "tools/bitmap.h"
#include "tools/lenunit.h"

/*======================================================================*/

#define SAVEHEAD "fig-save"

/*======================================================================*/
/*= feature for saving figure                                          =*/

typedef struct _Tsavepar {
    Tmsgdrv*    msgdrv;
    int         fmt;      // numeric value of format
    CstrStatic  fmtstr;   // human name for fmt
    FILE*       stream;   // where data are written 
} Tsavepar;

/*======================================================================*/
/*= feature for loading figure                                         =*/

typedef struct _Tloadpar {
    Tmsgdrv*   msgdrv;
    int        num_object;
    TstrEaten  comment;    // the current comment or NULL
    int        ifmt;       /* file format */
    float      ffmt;       /* floating values of ifmt. */
    Tsty_fill  stf_white;  /* fill style Solid with white color */
    TsintSet*  clrs;       /* indirection table for colors */

    CstrStatic file_name;
    int        line_no;
    FILE*      stream;
    Tfigure*   fig;     /* the current figure */
    TobjAll    currobj; /* the figure object that is currently read */
} Tloadpar;

/* add the file name and the line number to message */
extern void figio_parse_printf(void(*func)(Tmsgdrv*,CstrBrief,va_list),
                         Tloadpar* param, CstrBrief fmt, ...);
#define parse_info(...)  figio_parse_printf(tool_inferr_vprintf, __VA_ARGS__);
#define parse_warn(...)  figio_parse_printf(tool_wrnerr_vprintf, __VA_ARGS__);
#define parse_error(...) figio_parse_printf(tool_errerr_vprintf, __VA_ARGS__);
#define parse_fatal(...) figio_parse_printf(tool_errftl_vprintf, __VA_ARGS__);

/*======================================================================*/
/*= specialized drivers                                                =*/

/**
 * The xfig32_check function checks if the format of the lp->stream
 * is an original XFIG 3.2.
 *
 * The xfig32_load function initializes the lp->fig from the lp->stream
 * stream.
 *
 * The xfig32_save function writes into the sp->stream the description
 * at original XFIG 32 format of the fig figure.
 * 
 * Return:
 *  The xfig32_check function returns:
 *    0: XFIG 3.2 format is recognized, and lp->ifmt/ffmt are set to 3.2.
 *       The xfig32_load function is able to read the lp->stream.
 *   -1: An unsupported old XFIG format is recognized, lp->ifmt/ffmt
 *       are set to this format.
 *   -2: not a known XFIG format.
 *  The xfig32_load function returns:
 *    0: success
 *   -1: error
 *  The xfig32_save function returns:
 *    0: success
**/
extern int xfig32_check(Tloadpar*lp);
extern int xfig32_load (Tloadpar*lp);
extern int xfig32_save (Tsavepar*sp, Cfigure*fig);

/*======================================================================*/
#endif // FILE_FIG_IO_H
/*======================================================================*/
