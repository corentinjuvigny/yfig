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
 *      $Id: xfig32-converter.c 123 2018-04-11 08:28:17Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/io/xfig32-converter.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= handle the conversion from xfig-3.2 format to yfig data structure  =*/
/*= and vice versa.                                                    =*/
/*=                                                                    =*/
/*= If a conversion fails, a default value is returned and an error    =*/
/*= message is triggered.                                              =*/
/*=                                                                    =*/
/*= Note:                                                              =*/
/*=   - These functions are absolutely not efficient but they assure   =*/
/*=     that the X2Y... is the reverse function of Y2X....             =*/
/*=   - They can be improved but they are not used very often.         =*/
/*======================================================================*/

#include "xfig32.h"

/*======================================================================*/
/*= Types                                                              =*/

typedef struct _Tconvtype {
    int y_kd,y_subkd, y_tag;
    int x_kd,x_subkd;
} Tconvtype;

static Tconvtype datatype[] = {
    { FIG_KD_Ellipse    , FIGOELL_T_EllByRad    , 0                    , XFIG_KD_Ellipse  , XFIG_T_ELLIPSE_BY_RAD },
    { FIG_KD_Ellipse    , FIGOELL_T_EllByDia    , 0                    , XFIG_KD_Ellipse  , XFIG_T_ELLIPSE_BY_DIA },
    { FIG_KD_Ellipse    , FIGOELL_T_CirByRad    , 0                    , XFIG_KD_Ellipse  , XFIG_T_CIRCLE_BY_RAD },
    { FIG_KD_Ellipse    , FIGOELL_T_CirByDia    , 0                    , XFIG_KD_Ellipse  , XFIG_T_CIRCLE_BY_DIA },
                                                                                          
    { FIG_KD_Line       , FIGOLI_T_MLine        , FIG_TAG_CanHaveArrow , XFIG_KD_Line     , XFIG_T_POLYLINE },
    { FIG_KD_Line       , FIGOLI_T_Box          , 0                    , XFIG_KD_Line     , XFIG_T_BOX },
    { FIG_KD_Line       , FIGOLI_T_Polygon      , 0                    , XFIG_KD_Line     , XFIG_T_POLYGON },
    { FIG_KD_Line       , FIGOLI_T_RoundBox     , 0                    , XFIG_KD_Line     , XFIG_T_ARCBOX },
    { FIG_KD_Boxhv      , FIGOBHV_T_Image       , 0                    , XFIG_KD_Line     , XFIG_T_PICTURE },
                                                                                          
    { FIG_KD_Spline     , FIGOSP_T_OpenApprox   , FIG_TAG_CanHaveArrow , XFIG_KD_Spline   , XFIG_T_OPEN_APPROX }, 
    { FIG_KD_Spline     , FIGOSP_T_ClosedApprox , 0                    , XFIG_KD_Spline   , XFIG_T_CLOSED_APPROX }, 
    { FIG_KD_Spline     , FIGOSP_T_OpenInterp   , FIG_TAG_CanHaveArrow , XFIG_KD_Spline   , XFIG_T_OPEN_INTERP }, 
    { FIG_KD_Spline     , FIGOSP_T_ClosedInterp , 0                    , XFIG_KD_Spline   , XFIG_T_CLOSED_INTERP }, 
                                                                                          
    { FIG_KD_Text       , FIGOTE_T_LEFT         , 0                    , XFIG_KD_Text     , XFIG_T_LEFT_JUSTIFIED },
    { FIG_KD_Text       , FIGOTE_T_CENTER       , 0                    , XFIG_KD_Text     , XFIG_T_CENTER_JUSTIFIED },
    { FIG_KD_Text       , FIGOTE_T_RIGHT        , 0                    , XFIG_KD_Text     , XFIG_T_RIGHT_JUSTIFIED },
                                                                                          
    { FIG_KD_Arc        , FIGOAR_T_Open         , FIG_TAG_CanHaveArrow , XFIG_KD_Arc      , XFIG_T_OPEN_ARC },
    { FIG_KD_Arc        , FIGOAR_T_PiePiece     , 0                    , XFIG_KD_Arc      , XFIG_T_PIE_WEDGE_ARC },
                                               
    { FIG_KD_Compound   , 0                     , 0                    , XFIG_KD_Compound , 0 },
} ;

extern TobjAll xfig32X2Y_typenew(Tloadpar* param,  int xtype, int xsubtype)
{
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(datatype) ; i++) {
        if ( datatype[i].x_kd==xtype ) {
            if ( xsubtype==-1)
                return obj_newByOid(datatype[i].y_kd,0);
            if ( xsubtype==datatype[i].x_subkd)
                return obj_newByOid(datatype[i].y_kd,0);
        }
    }
    return obj_ObjAll(0);
}

extern void xfig32X2Y_type(Tloadpar* param,  int xtype, int xsubtype, int*ykind, int*ysubtype, int*ytag)
{
    int f_idx = -1; // index of first element that matches type
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(datatype) ; i++) {
        if ( datatype[i].x_kd==xtype ) {
            if (f_idx<0) f_idx = i;
            if ( datatype[i].x_subkd==xsubtype ) { // found
                *ykind    = datatype[i].y_kd;
                *ysubtype = datatype[i].y_subkd;
                *ytag     = datatype[i].y_tag;
                return;
            }
        }
    }
    
    if ( f_idx>=0 ) {
        *ykind    = datatype[f_idx].y_kd;
        *ysubtype = datatype[f_idx].y_subkd;
        *ytag     = datatype[f_idx].y_tag;
        X2Y_error(param,"%d/%d is an unexpected object type (use %d/%d instead)",xtype,xsubtype,xtype,datatype[f_idx].y_subkd);
    } else {
        *ykind    = -1;
        *ysubtype = -1;
        *ytag     = -1;
        X2Y_error(param,"%d/%d is an unexpected object type (use -1/-1 instead)",xtype,xsubtype);
    }
}

extern void xfig32Y2X_type(Tsavepar* param,  TobjAll yobj, int*xtype, int*xsubtype)
{
    int i;
    int       f_idx = -1; // index of first element that matches arrow->kind
    int        y_kd = obj_Kind(yobj);
    int     y_subkd;

    switch ( y_kd ) {
        case FIG_KD_Ellipse : y_subkd = yobj.ell   ->OEL_type ; break;
        case FIG_KD_Line    : y_subkd = yobj.line  ->OLI_type ; break;
        case FIG_KD_Spline  : y_subkd = yobj.spline->OSP_type ; break;
        case FIG_KD_Text    : y_subkd = yobj.text  ->OTE_type ; break;
        case FIG_KD_Arc     : y_subkd = yobj.arc   ->OAR_type ; break;
        case FIG_KD_Boxhv   : y_subkd = yobj.boxhv ->OBHV_type; break;
        default             : y_subkd = -1; break; // no sub-type
    }

    for (i=0 ; i<TOOL_TABLE_NbEle(datatype) ; i++) {
        if ( datatype[i].y_kd!=y_kd ) continue;
        if ( f_idx==-1 ) f_idx=i;
        if ( datatype[i].y_subkd==y_subkd ) { // found
found:
                *xtype    = datatype[i].x_kd;
                *xsubtype = datatype[i].x_subkd;
                return;
        }
    }
    
    if ( f_idx>=0 ) {
        *xtype    = datatype[f_idx].x_kd;
        *xsubtype = datatype[f_idx].x_subkd;
        Y2X_error(param,"can not convert %d/%d object type (use %d/%d instead)",y_kd,y_subkd,*xtype,*xsubtype);
    } else {
        *xtype    = -1;
        *xsubtype = -1;
        Y2X_error(param,"can not convert %d/%d object type (use -1/-1 instead)",y_kd,y_subkd);
    }
}

/*======================================================================*/
/*= Arrows                                                             =*/

typedef struct _Tconvarrow {
    int y_vtidx,y_mask, y_fill;
    int x_type,x_style;
} Tconvarrow;

static Tconvarrow dataarrow[] = {
    { FIG_KD_ArrowT00 , -1          , 0 , 0 , 0 },
    { FIG_KD_ArrowT01 , 0           , 1 , 1 , 0 },
    { FIG_KD_ArrowT01 , FDP_F_color , 0 , 1 , 1 },
    { FIG_KD_ArrowT02 , 0           , 1 , 2 , 0 },
    { FIG_KD_ArrowT02 , FDP_F_color , 0 , 2 , 1 },
} ;

extern void xfig32X2Y_arrow(Tloadpar* param, int type, int style, int*vtindex, Tsty_fill**fill)
{
    int f_idx = -1; // index of first element that matches type
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(dataarrow) ; i++) {
        if ( dataarrow[i].x_type==type ) {
            if (f_idx<0) f_idx = i;
            if ( dataarrow[i].x_style==style ) { // found
                *vtindex = dataarrow[i].y_vtidx;
                *fill    =  dataarrow[i].y_fill==0 ? (Tsty_fill*)NULL
                         :  &param->stf_white;
                return;
            }
        }
    }
    
    if ( f_idx>=0 ) {
        X2Y_error(param,"%d is an unexpected style for %d arrow type (use default instead)",style,type);
        *vtindex = dataarrow[f_idx].y_vtidx;
        *fill    = NULL;
    } else {
        X2Y_error(param,"%d is an unexpected arrow type (use default instead)",type);
        *vtindex = dataarrow[0].y_vtidx;
        *fill    = NULL;
    }
}

extern void xfig32Y2X_arrow(Tsavepar* param, TobjAll arrow, int*type, int*style)
{
    int       f_idx = -1; // index of first element that matches arrow->kind
    int        kind = obj_Kind(arrow);
    TstyleMask mask = arrow.arrow->OAW_stymask;
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(dataarrow) ; i++) {
        if ( dataarrow[i].y_vtidx==kind ) {
            if (f_idx<0) f_idx = i;
            if ( dataarrow[i].y_mask==-1 ) { // found
found:
                *type  = dataarrow[i].x_type;
                *style = dataarrow[i].x_style;
                return;
            } else {
                if ( (mask&FDP_F_color)==dataarrow[i].y_mask )
                    goto found;
            }
        }
    }
    
    if ( f_idx>=0 ) {
        Y2X_error(param,"can not convert 0x%0x option of %d arrow type (use default instead)",mask,kind);
        *type  = dataarrow[f_idx].x_type;
        *style = dataarrow[f_idx].x_style;
    } else {
        Y2X_error(param,"can not convert %d arrow type (use default instead)",kind);
        *type  = dataarrow[0].x_type;
        *style = dataarrow[0].x_style;
    }
}

/*======================================================================*/
/*= Fill style                                                         =*/
/*=                                                                    =*/
/*= NUMSHADEPATS 21  // shade icons                                    =*/
/*= NUMTINTPATS  20  // light icons                                    =*/
/*= NUMPATTERNS  22  // pattern icons                                  =*/

typedef struct _Tconvfillsty {
    Tsty_fill  y_sty;
    int x_style,x_clr,x_clr2;
} Tconvfillsty;

#define FSno()  .stf_style=DA_FS_NoFill 
#define FSso()  .stf_style=DA_FS_Solid 
#define FSli(n) .stf_style=DA_FS_LighTile, .stf_light=n 
#define FSsh(n) .stf_style=DA_FS_ShadTile, .stf_shade=n 
#define FSpa(n) .stf_style=DA_FS_PattTile, .stf_pattern=n 
#define CLR_ANY 0x7FFFFFFF

Tconvfillsty datafillsty[] = {
    { { FSno()   },  -1, CLR_ANY, -1 },
    // x_type < NUMSHADEPATS and x_clr==black (21)
    { { FSli(19) },   0,  0, -1 },
    { { FSli(18) },   1,  0, -1 },
    { { FSli(17) },   2,  0, -1 },
    { { FSli(16) },   3,  0, -1 },
    { { FSli(15) },   4,  0, -1 },
    { { FSli(14) },   5,  0, -1 },
    { { FSli(13) },   6,  0, -1 },
    { { FSli(12) },   7,  0, -1 },
    { { FSli(11) },   8,  0, -1 },
    { { FSli(10) },   9,  0, -1 },
    { { FSli( 9) },  10,  0, -1 },
    { { FSli( 8) },  11,  0, -1 },
    { { FSli( 7) },  12,  0, -1 },
    { { FSli( 6) },  13,  0, -1 },
    { { FSli( 5) },  14,  0, -1 },
    { { FSli( 4) },  15,  0, -1 },
    { { FSli( 3) },  16,  0, -1 },
    { { FSli( 2) },  17,  0, -1 },
    { { FSli( 1) },  18,  0, -1 },
    { { FSli( 0) },  19,  0, -1 },
    { { FSso()   },  20,  0, -1 },
    // x_type < NUMSHADEPATS  (21)
    { { FSsh( 0) },   0,  CLR_ANY, -1 },
    { { FSsh( 1) },   1,  CLR_ANY, -1 },
    { { FSsh( 2) },   2,  CLR_ANY, -1 },
    { { FSsh( 3) },   3,  CLR_ANY, -1 },
    { { FSsh( 4) },   4,  CLR_ANY, -1 },
    { { FSsh( 5) },   5,  CLR_ANY, -1 },
    { { FSsh( 6) },   6,  CLR_ANY, -1 },
    { { FSsh( 7) },   7,  CLR_ANY, -1 },
    { { FSsh( 8) },   8,  CLR_ANY, -1 },
    { { FSsh( 9) },   9,  CLR_ANY, -1 },
    { { FSsh(10) },  10,  CLR_ANY, -1 },
    { { FSsh(11) },  11,  CLR_ANY, -1 },
    { { FSsh(12) },  12,  CLR_ANY, -1 },
    { { FSsh(13) },  13,  CLR_ANY, -1 },
    { { FSsh(14) },  14,  CLR_ANY, -1 },
    { { FSsh(15) },  15,  CLR_ANY, -1 },
    { { FSsh(16) },  16,  CLR_ANY, -1 },
    { { FSsh(17) },  17,  CLR_ANY, -1 },
    { { FSsh(18) },  18,  CLR_ANY, -1 },
    { { FSsh(19) },  19,  CLR_ANY, -1 },
    { { FSso()   },  20,  CLR_ANY, -1 },
    // x_type < NUMSHADEPATS+NUMTINTPATS (41)
    { { FSli( 0) },  21,  CLR_ANY, -1 },
    { { FSli( 1) },  22,  CLR_ANY, -1 },
    { { FSli( 2) },  23,  CLR_ANY, -1 },
    { { FSli( 3) },  24,  CLR_ANY, -1 },
    { { FSli( 4) },  25,  CLR_ANY, -1 },
    { { FSli( 5) },  26,  CLR_ANY, -1 },
    { { FSli( 6) },  27,  CLR_ANY, -1 },
    { { FSli( 7) },  28,  CLR_ANY, -1 },
    { { FSli( 8) },  29,  CLR_ANY, -1 },
    { { FSli( 9) },  30,  CLR_ANY, -1 },
    { { FSli(10) },  31,  CLR_ANY, -1 },
    { { FSli(11) },  32,  CLR_ANY, -1 },
    { { FSli(12) },  33,  CLR_ANY, -1 },
    { { FSli(13) },  34,  CLR_ANY, -1 },
    { { FSli(14) },  35,  CLR_ANY, -1 },
    { { FSli(15) },  36,  CLR_ANY, -1 },
    { { FSli(16) },  37,  CLR_ANY, -1 },
    { { FSli(17) },  38,  CLR_ANY, -1 },
    { { FSli(18) },  39,  CLR_ANY, -1 },
    { { FSli(19) },  40,  CLR_ANY, -1 },
    // x_type < NUMSHADEPATS+NUMTINTPATS+NUMPATTERNS (63)
    { { FSpa( 0) },  41,  CLR_ANY, -1 },
    { { FSpa( 1) },  42,  CLR_ANY, -1 },
    { { FSpa( 2) },  43,  CLR_ANY, -1 },
    { { FSpa( 3) },  44,  CLR_ANY, -1 },
    { { FSpa( 4) },  45,  CLR_ANY, -1 },
    { { FSpa( 5) },  46,  CLR_ANY, -1 },
    { { FSpa( 6) },  47,  CLR_ANY, -1 },
    { { FSpa( 7) },  48,  CLR_ANY, -1 },
    { { FSpa( 8) },  49,  CLR_ANY, -1 },
    { { FSpa( 9) },  50,  CLR_ANY, -1 },
    { { FSpa(10) },  51,  CLR_ANY, -1 },
    { { FSpa(11) },  52,  CLR_ANY, -1 },
    { { FSpa(12) },  53,  CLR_ANY, -1 },
    { { FSpa(13) },  54,  CLR_ANY, -1 },
    { { FSpa(14) },  55,  CLR_ANY, -1 },
    { { FSpa(15) },  56,  CLR_ANY, -1 },
    { { FSpa(16) },  57,  CLR_ANY, -1 },
    { { FSpa(17) },  58,  CLR_ANY, -1 },
    { { FSpa(18) },  59,  CLR_ANY, -1 },
    { { FSpa(19) },  60,  CLR_ANY, -1 },
    { { FSpa(20) },  61,  CLR_ANY, -1 },
    { { FSpa(21) },  62,  CLR_ANY, -1 },
};

extern void xfig32X2Y_styfill(Tloadpar* param, int x_style, int x_clr, Tsty_fill* y_sty)
{
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(datafillsty) ; i++) {
        if ( datafillsty[i].x_style==x_style ) {
            if ( datafillsty[i].x_clr==x_clr ||
                 datafillsty[i].x_clr==x_clr ) { // found
found:
                *y_sty = datafillsty[i].y_sty;
                y_sty->stf_color=(Tcolor*)fig_colors_getByUKey(&param->fig->colors,x_clr);
                if ( y_sty->stf_color==NULL ) { // black
                    X2Y_error(param,
                        "%d color for %d fill style is unexpected (use black instead)"
                        ,x_clr,x_style);
                    y_sty->stf_color=(Tcolor*)fig_colors_getByUKey(&param->fig->colors,0);
                }
                color_newLazy(y_sty->stf_color);
                return;
            } else if ( datafillsty[i].x_clr==CLR_ANY ) { // found
                goto found;
            }
        }
    }
    
    X2Y_error(param,"%d fill style with %d color is unexpected style (use No Fill instead)",x_style,x_clr);
    Tsty_fill nofill = { FSno() };
    *y_sty = nofill;
    y_sty->stf_color=(Tcolor*)fig_colors_getByUKey(&param->fig->colors,0);
    color_newLazy(y_sty->stf_color);
}

extern void xfig32Y2X_styfill(Tsavepar* param, Csty_fill* y_sty, int*x_style, int*x_clr)
{
    int i;
    *x_clr   = y_sty->stf_color->ukey;
    for (i=0 ; i<TOOL_TABLE_NbEle(datafillsty) ; i++) {
        if ( datafillsty[i].y_sty.stf_style   == y_sty->stf_style && 
             datafillsty[i].y_sty.stf_shade   == y_sty->stf_shade && 
             datafillsty[i].y_sty.stf_light   == y_sty->stf_light && 
             datafillsty[i].y_sty.stf_pattern == y_sty->stf_pattern ) {
            if ( datafillsty[i].x_clr==CLR_ANY ) {
found:
                *x_style = datafillsty[i].x_style;
                return;
            } else if ( datafillsty[i].x_clr ==*x_clr ||
                        datafillsty[i].x_clr2==*x_clr )
                goto found;
        }
    }
    Y2X_error(param,"can not convert fill (use No Fill instead): %s",
        DBG_STF2str(y_sty));
    *x_style = -1;
}

/*======================================================================*/
/*= HV flip + rotation +/-90 degree                                    =*/
/*=                                                                    =*/

typedef struct _Tfliprot {
    int y_ope;
    int x_flip,x_p0,x_p1,x_p2,x_p3;
} Tfliprot;

Tfliprot datafliprot[] = {
  { 0                              , 0 , 0,1,2,3 },
  { IMG_OP_FLIP_H                  , 1 , 1,0,3,2 },
  { IMG_OP_FLIP_V                  , 1 , 3,2,1,0 },
  { IMG_OP_FLIP_HV                 , 0 , 2,3,0,1 },
  { IMG_OP_ROTA_P270               , 0 , 3,0,1,2 },
  { IMG_OP_ROTA_P090               , 0 , 1,2,3,0 },
  { IMG_OP_ROTA_P090|IMG_OP_FLIP_V , 1 , 0,3,2,1 },
  { IMG_OP_ROTA_P270|IMG_OP_FLIP_V , 1 , 2,1,0,3 },

};

extern int xfig32X2Y_fliprot(Tloadpar* param, int x_flip, int x_p0, int x_p1)
{
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(datafliprot) ; i++) {
        if ( datafliprot[i].x_flip==x_flip && 
             datafliprot[i].x_p0  ==x_p0   && 
             datafliprot[i].x_p1  ==x_p1   ) { // found
                return datafliprot[i].y_ope;
        }
    }
    
    X2Y_error(param,"can not convert xfig flip-rotate operation (use No Flip/Rot instead): %d/%d/%d",x_flip,x_p0,x_p1);
    return 0;
}

extern void xfig32Y2X_fliprot(Tsavepar* param, int y_ope, int*x_flip, int*x_p0, int*x_p1, int*x_p2, int*x_p3)
{
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(datafliprot) ; i++) {
        if ( datafliprot[i].y_ope==y_ope ) goto found;
    }
    
    Y2X_error(param,"can not convert yfig flip-rotate operation (use No Flip/Rot instead): %d",y_ope);
    i=0; 
found:
    *x_flip = datafliprot[i].x_flip;
    *x_p0   = datafliprot[i].x_p0;
    *x_p1   = datafliprot[i].x_p1;
    *x_p2   = datafliprot[i].x_p2;
    *x_p3   = datafliprot[i].x_p3;
}

/*======================================================================*/
/*= vim: set textwidth=0:                                              =*/
/*======================================================================*/
