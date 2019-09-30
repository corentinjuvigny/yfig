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
 *      $Id: xfig32.h 128 2018-04-14 15:53:02Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/io/xfig32.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= resources shared by xfig32-load/save/convert.c                     =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_FIG_IO_XFIG32_H
#define FILE_FIG_IO_XFIG32_H
/*======================================================================*/

#include "fig-io.h"

/*======================================================================*/
/*= Error handler.                                                     =*/

#define Y2X_error(param,fmt,...)   tool_errerr_printf(param->msgdrv,fmt,__VA_ARGS__)

extern void xfig32_parse_printf(void(*func)(Tmsgdrv*,CstrBrief,va_list),
                         Tloadpar* param, CstrBrief fmt, ...);
#define X2Y_info(...)      parse_info(__VA_ARGS__) //xfig32_parse_printf(tool_wrnerr_vprintf ,__VA_ARGS__)
#define X2Y_warning(...)   parse_warn(__VA_ARGS__) //xfig32_parse_printf(tool_wrnerr_vprintf ,__VA_ARGS__)
#define X2Y_error(...)     parse_error(__VA_ARGS__) //xfig32_parse_printf(tool_wrnerr_vprintf ,__VA_ARGS__)
#define X2Y_fatal(...)     parse_fatal(__VA_ARGS__) //xfig32_parse_printf(tool_wrnerr_vprintf ,__VA_ARGS__)

/*======================================================================*/
/*= Conversion functions from xfig to YFIG and vice-versa.             =*/

extern TobjAll xfig32X2Y_typenew(Tloadpar* param,  int xtype, int subtype);
extern void    xfig32X2Y_type   (Tloadpar* param,  int xtype, int xsubtype, int*ykind, int*ysubtype, int*ytag);
extern void    xfig32Y2X_type   (Tsavepar* param,  TobjAll yobj, int*xtype, int*xsubtype);
extern void    xfig32X2Y_arrow  (Tloadpar* param, int type, int style, int*vtindex, Tsty_fill**fill);
extern void    xfig32Y2X_arrow  (Tsavepar* param, TobjAll arrow, int*type, int*style);
extern void    xfig32X2Y_styfill(Tloadpar* param, int x_style, int x_clr, Tsty_fill* y_sty);
extern void    xfig32Y2X_styfill(Tsavepar* param, Csty_fill* y_sty, int*x_style, int*x_clr);
extern int     xfig32X2Y_fliprot(Tloadpar* param, int x_flip, int x_p0, int x_p1);
extern void    xfig32Y2X_fliprot(Tsavepar* param, int y_ope, int*x_flip, int*x_p0, int*x_p1, int*x_p2, int*x_p3);

/*======================================================================*/
/*= xfig macros                                                        =*/

// object main type
#define XFIG_KD_ColorDef     0
#define XFIG_KD_Ellipse      1
#define XFIG_KD_Line         2
#define XFIG_KD_Spline       3
#define XFIG_KD_Text         4
#define XFIG_KD_Arc          5
#define XFIG_KD_Compound     6
#define XFIG_KD_CompoundEnd -6
 
// sub-type for XFIG_KD_Ellipse
#define XFIG_T_ELLIPSE_BY_RAD    1
#define XFIG_T_ELLIPSE_BY_DIA    2
#define XFIG_T_CIRCLE_BY_RAD     3
#define XFIG_T_CIRCLE_BY_DIA     4

// sub-type for XFIG_KD_Line
#define XFIG_T_POLYLINE  1
#define XFIG_T_BOX       2
#define XFIG_T_POLYGON   3
#define XFIG_T_ARCBOX    4
#define XFIG_T_PICTURE   5

// sub-type for XFIG_KD_Spline
#define XFIG_T_OPEN_APPROX     0
#define XFIG_T_CLOSED_APPROX   1
#define XFIG_T_OPEN_INTERP     2
#define XFIG_T_CLOSED_INTERP   3
#define XFIG_T_OPEN_XSPLINE    4       // ????
#define XFIG_T_CLOSED_XSPLINE  5       // ????

// sub-type for XFIG_KD_Text
#define XFIG_T_LEFT_JUSTIFIED    0
#define XFIG_T_CENTER_JUSTIFIED  1
#define XFIG_T_RIGHT_JUSTIFIED   2

// sub-type for XFIG_KD_Arc
#define XFIG_T_OPEN_ARC        0+1      // +1 in xfig format
#define XFIG_T_PIE_WEDGE_ARC   1+1
#define XFIG_T_ELLIPTICAL      2+1

/*======================================================================*/
#endif // FILE_FIG_IO_XFIG32_H
/*======================================================================*/
