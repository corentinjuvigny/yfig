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
 *      $Id: xfig32-load.c 369 2018-11-19 12:35:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/io/xfig32-load.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= handle the loading of figure at XFIG-3.2 format.                   =*/
/*======================================================================*/

#include "xfig32.h"
#include "tools/charset.h"
#include "tools/math-macros.h"

#if defined(HAVE_LOCALE_H) && HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(HAVE_DECL_SETLOCALE) && !HAVE_DECL_SETLOCALE
extern char *setlocale(int category, const char *locale);
#endif

#define BUF_SIZE    1024

#define  Err_incomp "Incomplete %s object"

#define FIG_FONT_DefaultIndex   0 
#define FIG_FONT_MaxIndex       FONT_NB
#define FIG_FONT_MaxLatexIndex  6
#define FIG_FONT_LastIndex(T) (psfont_text(T) ? FIG_FONT_MaxIndex \
                                              : FIG_FONT_MaxLatexIndex)

#define fig_create_pic(...)      0
#define fig_create_picture_entry(...) 0

/*======================================================================*/

extern int xfig32_check(Tloadpar*lp)
{
    char   str[BUF_SIZE];   /* input buffer */

    if ( fgets(str, BUF_SIZE, lp->stream) == 0 ) {
        return -2;
    }

    if ( strncmp(str, "#FIG", 4)!=0 )
        return -2;

    if (strlen(str) <= 6)
        return -2;   /* Short line - say corrupt */

    int intp,decp;  // interger and decimal part
    switch ( sscanf(str,"#FIG %d.%d", &intp,&decp) ) {
        default: /* assume 1.4 */
            lp->ifmt = 14;
            lp->ffmt = 1.4;
            return -1;
        case 1:
            lp->ifmt = intp*10;
            lp->ffmt = intp;
            return -1;
        case 2:
            while ( decp > 9 ) decp /= 10;
            lp->ifmt = intp*10 + decp;
            lp->ffmt = intp    + decp/10.;
            if ( lp->ifmt == 32 )
                return 0;
            else
                return -1;
    }
}

static int xfig32load_header(Tloadpar* param, Tfigure* fig);
static int xfig32load_objects(Tloadpar* param);

extern int xfig32_load (Tloadpar*lp)
{
    char   str[BUF_SIZE];   /* input buffer */

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    char * currentlocale = setlocale(LC_NUMERIC, "C");
    currentlocale=tool_strdup(currentlocale);
#endif

    // skip format line
    if ( fgets(str, BUF_SIZE, lp->stream) == 0 ) 
        goto error;

    // read header
    if ( xfig32load_header(lp,lp->fig)!=0 )
        goto error;

    // read objects and colors
    if ( xfig32load_objects(lp)!=0 )
        goto error;

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        tool_free(currentlocale);
    }
#endif
    return 0;

error:
#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        free(currentlocale);
    }
#endif
    return -1;
}

/*======================================================================*/
#define attach_comment(param,obj) do { (obj)->OA_comment=(param)->comment;\
                                       (param)->comment=0; } while(0)
#define CHECK_COLOR_RETNULL(ret,color,param) do {                 \
    int __ukey__;                                                 \
    if ( color<FIG_NbPredefinedColor )                            \
        __ukey__ = color;                                         \
    else {                                                        \
        if ( param->clrs==0 || color>=param->clrs->eleNb ) goto clr_error; \
        if ( param->clrs->eles[color]==-1 )                goto clr_error; \
        __ukey__ = param->clrs->eles[color];                      \
    }                                                             \
    if ( (ret=(Tcolor*)fig_colors_getByUKey(&param->fig->colors,__ukey__))==0 ) { \
        clr_error:                                                \
        X2Y_error(param,"Cannot locate user color %d.", color); \
        return 0;                                                 \
    } color_newLazy(ret);                                         \
  } while (0)

/**
 * Read a line from param->stream into buf.
 * The function handles comment and skips empty lines.
 * Return value: -1: EOF or IO error
 *                1: success
**/
static int xfig32load_inputLine(char*buf, Tloadpar *param)
{
    while (1) {
        if ( fgets(buf, BUF_SIZE, param->stream)==NULL) {
            return -1;
        }
        param->line_no++;
        if ( TOOL_strIsEmptyLine(buf) )
            continue; // skip empty line
        if (*buf == '#') {
            /* save any comments */
            CstrBrief com = buf+1;
            if ( param->comment==0 )
                param->comment = tool_strdup(com);
            else
                tool_strchgEaten(&param->comment,tool_aprintf("%s%s", param->comment,com));
        } else 
            return 1;
    }
}

static int backslash_count(char *cp, int start)
{
  int i, count = 0;

  for(i=start; i>=0; i--) {
    if (cp[i] == '\\')
    count++;
    else
    break;
  }
  return count;
}

/* skip to the end of the current line */
static void skip_line(FILE *fp)
{
    while (fgetc(fp) != '\n') {
    if (feof(fp))
        return;
    }
}

/* make sure angle is 0 to 2PI */

static void fix_angleD(double *angle)
{
    while (*angle < 0.0)
        *angle += TOOL_2PI;
    while (*angle >= TOOL_2PI)
        *angle -= TOOL_2PI;
}

static void fix_angleF(float *angle)
{
    while (*angle < 0.0)
        *angle += TOOL_2PI;
    while (*angle >= TOOL_2PI)
        *angle -= TOOL_2PI;
}

static void fix_depth(int *depth, Tloadpar*param)
{
    if ( *depth>FIG_MAX_DEPTH ) {
        X2Y_error(param,"Depth (%d) > Maximum allowed (%d), setting to %d.",
            *depth, FIG_MAX_DEPTH, FIG_MAX_DEPTH);
        *depth=FIG_MAX_DEPTH;
    }
    else if ( *depth<FIG_MIN_DEPTH ) {
        X2Y_error(param,"Depth (%d) < Minimum allowed (%d), setting to %d.",
            *depth, FIG_MIN_DEPTH, FIG_MIN_DEPTH);
        *depth=FIG_MIN_DEPTH;
    }
}

/* this function is to count line numbers correctly while reading
 * input files.
 * It skips all tabs and spaces and increments the global
 * variable line_no if a newline was found.
 * If any other character is read, it is put back to the input
 * stream and the function returns.
 * It should be called from within the point reading loops
 * in the read_{line,spline}object functions, where the point
 * coordinates may be given in an arbitrary number of lines.
 * Added by Andreas_Bagge@maush2.han.de (A.Bagge), 14.12.94
 */

static void count_lines_correctly(FILE *fp, Tloadpar*param)
{
    int cc;
    do{
    cc=getc(fp);
    if (cc=='\n') {
       param->line_no++;
       cc=getc(fp);
    }
    } while (cc==' '||cc=='\t');
    ungetc(cc,fp);
}

static Tdble2Set* read_dble2set(Tloadpar*param, int nb, CstrBrief oname)
{
    int n,x,y;
    Tdble2Set* set = dble2set_new(nb) ;
    for (n=0 ; n<nb ; n++) {
        count_lines_correctly(param->stream, param);
        if (fscanf(param->stream, "%d%d", &x, &y) != 2) {
            X2Y_error(param,Err_incomp, oname);
            dble2set_delete(set);
            return 0;
        }
        /* ignore identical consecutive points */
        if ( set->eleNb==0 || dble2_cmpXY(set->eles+set->eleNb-1,x,y)!=0 )
            dble2set_addXY(set,x,y);
    }
    return set;
}

/*======================================================================*/
/*= Arrows                                                             =*/

typedef struct _Txfig32_arrowParam {
    int    type;        // an ident
    int    style;       // 0:not Filled 1:filled others:?
    // caution: thickness height and length are absolute values
    double thickness;   // thickness of arrow in point (probably)
    double height;      // xfig.width   in fu
    double length;      // xfig.height  in fu
/**
 * length:  <-->
 *          \     ∆
 *           \    |
 *            \   |
 *          ---x  | height
 *            /   |
 *           /    |
 *          /     ∇
**/
} Txfig32_arrowParam;

// load raw arrows parameters into endarrow and begarrow according
// to endarrowLoad and begarrowLoad flag (1=load ; 0:skip)
// Return: 0 on success and -1 on failure
static int xfig32load_arrow(Tloadpar*param,
        int endarrowLoad, Txfig32_arrowParam* endarrow,
        int begarrowLoad, Txfig32_arrowParam* begarrow)
{
    char buf[BUF_SIZE];
    Txfig32_arrowParam* p;
    if ( endarrowLoad ) {
        p = endarrow;
        if ( xfig32load_inputLine(buf,param) == -1) {
            X2Y_error(param,"unexpected EOF");
            return -1;
        }
        if (sscanf(buf, "%d%d%lf%lf%lf", &p->type, &p->style,
                         &p->thickness, &p->height, &p->length) != 5) {
            X2Y_error(param,"incomplete arrow description");
            return -1;
        }
    }
    if ( begarrowLoad ) {
        p = begarrow;
        if ( xfig32load_inputLine(buf,param) == -1) {
            X2Y_error(param,"unexpected EOF");
            return -1;
        }
        if (sscanf(buf, "%d%d%lf%lf%lf", &p->type, &p->style,
                         &p->thickness, &p->height, &p->length) != 5) {
            X2Y_error(param,"incomplete arrow description");
            return -1;
        }
    }
    return 0;
}

static void xfig32load_addBegEndArrowToObj(Tloadpar*param,
        TobjAll obj, int atBeg, Txfig32_arrowParam* ap)
{
    double minlength = LUNIT_PtToFu( 2 ); // 2 points (2/72 Inches)
    void* vt;
    double length    = ap->length  <= minlength ? minlength : ap->length;
    double angle     = atan(ap->height/2./length);
    double thickness = 1;
    int    abs       = 1;
    Tsty_fill* pstf  = NULL;
    if ( -5<RadToDeg180(angle) && RadToDeg180(angle)<5 ) angle=0;
    // make ap absolute length relative to line width of op object.
    // note: the angle still stays an absolute value.
    length = length / obj_StyLine(obj)->stl_thickness;

    int vtidx; Tsty_fill* stf; xfig32X2Y_arrow(param,ap->type,ap->style,&vtidx,&stf);
    vt = obj_vtable[vtidx];
    obj_arrow_chgRel(obj,atBeg,vt,stf,length,angle);
}

/*======================================================================*/

// Return value: 0 on success otherwise -1.
static int xfig32load_header(Tloadpar* param, Tfigure* fig)
{
    char   str[BUF_SIZE];
    char   buf[BUF_SIZE];

    /* read Portrait/Landscape indicator now */
    if (xfig32load_inputLine(buf,param) < 0) {
        X2Y_error(param,"unexpected EOF: "
                          "No Portrait/Landscape specification");
        return -1;
    }
    fig->settings.landscape = (strncasecmp(buf,"landscape",9) == 0);
    
    /* read Centering indicator now */
    if (xfig32load_inputLine(buf,param) < 0) {
        X2Y_error(param,"unexpected EOF: "
                          "No Center/Flushleft specification");
        return -1;
    }
    if ( strncasecmp(buf,"center",6) == 0 ) {
        fig->settings.flushleft = !(strncasecmp(buf,"flush",5)!=0);
    } else if ( strncasecmp(buf,"flush",5) == 0 ) {
        fig->settings.flushleft = !(strncasecmp(buf,"flush",5)!=0);
    } else {
        X2Y_error(param,"bad format: "
                "Expected Center or Flush (found:%s)",buf);
        return -1;
    }

    /* read metric/inches indicator */
    if ( xfig32load_inputLine(buf,param) < 0) {
        X2Y_error(param,"unexpected EOF: "
                          "No Metric/Inches specification");
        return -1;
    }
    lu_init(&fig->settings.userUnit,
            strncasecmp(buf,"metric",5)==0 ? LUNIT_ID_Cm : LUNIT_ID_Inch,
            1.0, 10.);
    
    /* paper size, magnification, multiple page flag and transparent color
      (for GIF export) new in 3.2 */
    //if (param->proto >= 32) {
        /* read paper size now */
        if ( xfig32load_inputLine(buf,param) < 0) {
            X2Y_error(param,"unexpected EOF: "
                              "No Paper size specification");
            return -1;
        }
        sscanf(buf,"%s",str);
        const Tpaperdef* paper =  tool_paperdef_get(str);
        if ( paper == 0 ) {
            X2Y_warning(param,"%s paper not supported (use %s instead).",
                  buf,tool_paperSizes->sname);
            fig->settings.papersize = tool_paperSizes;
        } else
            fig->settings.papersize = paper;
    
        /* read magnification now */
        if ( xfig32load_inputLine(buf,param) < 0) {
            X2Y_error(param,"unexpected EOF: "
                              "No Magnification specification");
            return -1;
        }
        fig->settings.magnification = atoi(buf);
    
        /* read multiple page flag now */
        if ( xfig32load_inputLine(buf,param) < 0) {
            X2Y_error(param,"unexpected EOF: "
                              "No Multiple page flag specification");
            return -1;
        }
        if (strncasecmp(buf,"multiple",8) != 0 &&
            strncasecmp(buf,"single",6) != 0) {
            X2Y_error(param,"No Multiple page flag specification");
            return -1;
        }
        fig->settings.multiple = (strncasecmp(buf,"multiple",8) == 0);
    
        /* read transparent color now */
        if ( xfig32load_inputLine(buf,param) < 0) {
            X2Y_error(param,"unexpected EOF: "
                              "No Transparent color specification");
            return -1;
        }
        fig->settings.transparent = atoi(buf);
    //}

    /**
     * read the resolution (ppi) and the coordinate system.
     * The coordinate system code is
     *    - 1 for lower left at 0,0
     *    - 2 for upper left at 0,0
     * but the 1 value is not used, because it is not yet implemented.
     * So coordinate system code must be 2.
    **/
    if ( xfig32load_inputLine(buf,param) < 0) {
        X2Y_error(param,"unexpected EOF: "
                "expecting resolution and coordinate system");
        return -1;
    }
    int origin_code; // not yet used, must be 2
    if ( sscanf(buf, "%d%d\n", &param->fig->io_ppi, &origin_code) != 2) {
        X2Y_error(param,"bad format: "
                "resolution and/or the specification of the coordinate system");
        return -1;
    }
    if ( fig->io_ppi <10 || (1200*1000)<fig->io_ppi ) {
        X2Y_error(param,"bad format: "
                "%d is an invalid resolution (expected in [10:1,200,000]",fig->io_ppi);
        return -1;
    }
    fig->fig_ppi = fig->io_ppi;
    if ( origin_code==2 ) {
        X2Y_info(param,"coordinate sytem origin sets to upper left corner of page.");
    } else if ( origin_code==1 ) {
        X2Y_error(param,"not yet supported: "
                "coordinate sytem origin sets to lower left corner of page.");
        return -1;
    } else {
        X2Y_error(param,"bad value: "
                "%d is an invalid code for coordinate sytem origin "
                "(valid code: 1 for lower left, 2 for upper left corner).",
                origin_code);
        return -1;
    }

    return 0;
}

/*======================================================================*/

static int fig_read30_colordef(char*buf, Tloadpar* param)
{
    Tfigure*  fig=param->fig;
    int       i,c,r,g,b;
    TsintSet* clrs;

    if ( sscanf(buf, "%*d %d #%02x%02x%02x", &c, &r, &g, &b)!=4 ) {
        X2Y_error(param,"Invalid color definition: %s, setting to black (#00000).",buf);
        return -1;
    }
    if ( c<FIG_NbPredefinedColor ) {
        X2Y_error(param,"Invalid color definition: %s, setting to black (#00000).",buf);
        return -1;
    }
    if ( fig_colors_getByUKey(&fig->colors,c)!=0 ) {
        X2Y_error(param,"%d th color is already defined.",c);
        return -1;
    }

    // we need the c entry in  param->clrs
    if ( param->clrs==0 ) param->clrs=sintset_new(100);
    clrs = param->clrs;
    for ( i=clrs->eleNb; clrs->eleNb<=c ; i++) sintset_add(clrs,-1);
    // now clrs->eles[c] exists

    if ( fig->colors.privateColors==0 )
        fig->colors.privateColors=colorset_newDNT(0,32,7);
    Tcolor* color  = color_new(0,r,g,b,EGSCOLOR_none);
    color->ukey = c;
    Ccolor* color2 = colorset_addEated(fig->colors.privateColors,color);
    if ( color==color2 ) {
        clrs->eles[c] = c;
    } else {
        clrs->eles[c] = color2->ukey;
        color_delete( color );
        X2Y_warning(param,"%d th and %d th colors are the same.",color2->ukey,c);
    }
    return 0;
}

/*======================================================================*/

static int xfig30read_obj_arc(Tloadpar*param, char*buf)
{
    TobjArc    *a = param->currobj.arc;
    int         status,n, fa, ba;
    int         xtype,xsubtype,depth,pen_color,fill_color;
    int         ytype,ysubtype,ytags;
    int         pen_style;

    n = sscanf(buf,
      "%d%d%u%d%d%d%d%d%u%f%u%d%d%d%lf%lf%lf%lf%lf%lf%lf%lf\n",
       &xtype,&xsubtype, &a->OAR_style, &a->OAR_thickness,
       &pen_color, &fill_color, &depth,
       &pen_style, &a->OAR_fill_style,
       &a->OAR_style_val, &a->OAR_cap,
       &a->OAR_direction, &fa, &ba,
       &a->OAR_center.x,    &a->OAR_center.y,
       &a->OAR_points[0].x, &a->OAR_points[0].y,
       &a->OAR_points[1].x, &a->OAR_points[1].y,
       &a->OAR_points[2].x, &a->OAR_points[2].y);
    if ( n != 22 ) {
        X2Y_error(param,Err_incomp, "arc");
        return 0;
    }
    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    a->OAR_type = ysubtype;
    a->O_tags   = ytags;

    fix_depth(&depth,param); O_setDepth(a,depth);
    CHECK_COLOR_RETNULL(a->OAR_pen_color,pen_color,param);
    xfig32X2Y_styfill(param,a->OAR_fill_style, fill_color, &a->OAR_fill);

    // read rawly the arrow parameters if any.
    Txfig32_arrowParam endarrow, begarrow;
    if ( (status=xfig32load_arrow(param,fa,&endarrow,ba,&begarrow))<0 ) {
        return status;
    }

    // add arrows if any
    TobjAll oa; oa.arc = a;
    if (fa) xfig32load_addBegEndArrowToObj(param,oa,0,&endarrow);
    if (ba) xfig32load_addBegEndArrowToObj(param,oa,1,&begarrow);

    return 1;
}

static int xfig30read_obj_ellipse(Tloadpar*param, char* buf) 
{
    TobjEllipse*  e = param->currobj.ell;
    int         n;
    int         xtype,xsubtype,depth,pen_color,fill_color;
    int         ytype,ysubtype,ytags;
    int         pen_style;

    n = sscanf(buf, "%d%d%u%d%d%d%d%d%u%f%d%f"
                    "%lf%lf%lf%lf%lf%lf%lf%lf\n",
       &xtype,&xsubtype, &e->OEL_style, &e->OEL_thickness,
       &pen_color, &fill_color, &depth,
       &pen_style, &e->OEL_fill_style,
       &e->OEL_style_val, &e->OEL_direction, &e->OEL_angle,

       &e->OEL_center.x, &e->OEL_center.y,
       &e->OEL_radiuses.x, &e->OEL_radiuses.y,
       &e->OEL_start.x, &e->OEL_start.y,
       &e->OEL_end.x, &e->OEL_end.y);
    if ( n != 20 ) {
        X2Y_error(param,Err_incomp, "ellipse");
        return 0;
    }
    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    e->OEL_type = ysubtype;
    e->O_tags   = ytags;

    fix_depth(&depth,param); O_setDepth(e,depth);
    CHECK_COLOR_RETNULL(e->OEL_pen_color,pen_color,param);
    xfig32X2Y_styfill(param, e->OEL_fill_style, fill_color, &e->OEL_fill);
    fix_angleF(&e->OEL_angle);   /* make sure angle is 0 to 2PI */
    return 1;
}

static int xfig30read_obj_line(Tloadpar*param, char* buf)
{
    TobjLine   *l = param->currobj.line;
    int         n, fa, ba, npts;
    int         status;
    int         xtype,xsubtype,depth,pen_color,fill_color;
    int         ytype,ysubtype,ytags;
    int         pen_style; // ignored (seems to be always -1)

    /* 3.0) or later additionally has number of points parm for
       all line objects and fill color separate from border color */
    n = sscanf(buf, "%d%d%u%d%d%d%d%d%u%f%u%u%d%d%d%d",
       &xtype,&xsubtype, &l->OLI_style, &l->OLI_thickness,
       &pen_color, &fill_color, &depth,
       &pen_style, &l->OLI_fill_style, &l->OLI_style_val,
       &l->OLI_join, &l->OLI_cap, &l->OLI_radius,
       &fa, &ba, &npts);
    if ( n != 16 ) {
        X2Y_error(param,Err_incomp, "line");
        return 0;
    }
    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    l->OLI_type = ysubtype;
    l->O_tags   = ytags;

    fix_depth(&depth,param); O_setDepth(l,depth);
    xfig32X2Y_styfill(param, l->OLI_fill_style, fill_color, &l->OLI_fill);
    CHECK_COLOR_RETNULL(l->OLI_pen_color,pen_color,param);

    // read rawly the arrow parameters if any.
    Txfig32_arrowParam endarrow, begarrow;
    if ( (status=xfig32load_arrow(param,fa,&endarrow,ba,&begarrow))<0 ) {
        return status;
    }

    // read the line vertices.
    l->OLI_points = read_dble2set(param,npts,"line");
    if ( l->OLI_points==0 ) return 0;

    // check vertices numbers
    n = l->OLI_points->eleNb;
    if ( (n < 5 && (xsubtype == XFIG_T_BOX || xsubtype == XFIG_T_ARCBOX ||
                       xsubtype == XFIG_T_PICTURE)) ||
         (n < 3 && xsubtype == XFIG_T_POLYGON) ) {
        if (xsubtype == XFIG_T_POLYGON) {
            X2Y_error(param, "invalid polygon containing fewer than 3 points");
        } else {
            X2Y_error(param,"invalid %s (zero-sized)",
                xsubtype==XFIG_T_BOX    ? "box"    :
                xsubtype==XFIG_T_ARCBOX ? "arcbox" : "picture");
        }
        return 0;
    }

    /* skip to the next line */
    skip_line(param->stream);

    // add arrows if any
    TobjAll oa; oa.line = l;
    if (fa) xfig32load_addBegEndArrowToObj(param,oa,0,&endarrow);
    if (ba) xfig32load_addBegEndArrowToObj(param,oa,1,&begarrow);

    return 1;
}

static int xfig30read_obj_boxhv(Tloadpar*param, char* buf)
{
    TobjBoxhv  *obj = param->currobj.boxhv;
    int         n, fa, ba, npts;
    int         xtype,xsubtype,style,thickness,pen_color,fill_color,depth;
    int         pen_style,fill_style;
    float       style_val;
    int         join,cap,radius;
    int         ytype,ysubtype,ytags;
    int         flip;

    /* 3.0) or later additionally has number of points parm for
       all line objects and fill color separate from border color */
    n = sscanf(buf, "%d%d%u%d%d%d%d%d%u%f%u%u%d%d%d%d",
       &xtype,&xsubtype, &style, &thickness,
       &pen_color, &fill_color, &depth,
       &pen_style, &fill_style, &style_val,
       &join, &cap, &radius,
       &fa, &ba, &npts);
    if ( n != 16 ) {
        X2Y_error(param,Err_incomp, "line picture");
        return 0;
    }

    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    obj->OBHV_type = ysubtype;
    obj->O_tags    = ytags;

    fix_depth(&depth,param); O_setDepth(obj,depth);

    if (ysubtype == FIGOBHV_T_Image) {
        char    picfile[PATH_MAX], s1[PATH_MAX];
        
        if ( xfig32load_inputLine(buf,param) == -1) {
            X2Y_error(param,Err_incomp, "line picture");
            return 0;
        }
        if (sscanf(buf, "%d %[^\n]", &flip, s1) != 2) {
            X2Y_error(param,Err_incomp, "line picture");
            return 0;
        }
        
        /* if path is relative convert it to absolute path */
        if (s1[0] != '/') 
            sprintf(picfile, "%s/%s", gl_currDir, s1);
        else
            strcpy(picfile, s1);
        
        obj->OBHV_img = image_new(picfile,0);
    } else {
        X2Y_error(param,"unsuported %d sub-type in boxhv object",ysubtype);
    }

    // read boxhv vertices.
    Tdble2Set* pts = read_dble2set(param,npts,"line picture");
    if ( pts==0 ) return 0;

    // check vertices numbers
    n = pts->eleNb;
    if ( n!=4 && n!=5 ) { // 5 is allowed supposing a xfig closed line
        X2Y_error(param, "invalid rectangle definition (fewer than 4 points");
        dble2set_delete(pts);
        return 0;
    }

    Tdble2 min,max; dble2set_minmaxXY(pts,&min,&max);
    obj_boxhv_setRectXYWH(param->currobj, min.x,min.y, max.x-min.x,max.y-min.y);

#if 1
    int i,j,p[2];
    for (i=0; i<2 ; i++)
        for (j=0; j<4 ; j++)
            if ( dble2_cmpPT(pts->eles+i,obj->OBHV_points+j)==0 ) {
                p[i]=j;
            }
#else
    int i,j,p[4];
printf("-- flip=%d",flip);
    for (i=0; i<4 ; i++)
        for (j=0; j<4 ; j++)
            if ( dble2_cmpPT(pts->eles+i,obj->OBHV_points+j)==0 ) {
printf(" %d",j);
                p[i]=j;
            }
printf("%s\n",obj->OBHV_img->img_path);
#endif
    obj->OBHV_img->img_ope = xfig32X2Y_fliprot(param,flip,p[0],p[1]);

    /* skip to the next line */
    skip_line(param->stream);
    
    dble2set_delete(pts);
    return 1;
}

static int xfig30read_obj_spline(Tloadpar*param, char * buf) 
{
    FILE* fp = param->stream;
    TobjSpline *s = param->currobj.spline;
    int         c, n, x, y, fa, ba, npts;
    int         status;
    float       thickness, wd, ht;
    double      s_param;
    float       lx, ly, rx, ry;
    int         xtype,xsubtype,depth,pen_color,fill_color;
    int         ytype,ysubtype,ytags;
    int         pen_style; // ignored (seems to be always -1)

    /* 3.0(experimental 2.2) or later has number of points parm for all spline
    objects and fill color separate from border color */
    n = sscanf(buf, "%d%d%u%d%d%d%d%d%u%f%u%d%d%d",
        &xtype,&xsubtype, &s->OSP_style, &s->OSP_thickness,
        &pen_color, &fill_color,
        &depth, &pen_style, &s->OSP_fill_style,
        &s->OSP_style_val, &s->OSP_cap, &fa, &ba, &npts);
    if ( n!=14 ) {
        X2Y_error(param,Err_incomp, "spline");
        return 0;
    }
    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    s->OSP_type = ysubtype;
    s->O_tags   = ytags;

    fix_depth(&depth,param); O_setDepth(s,depth);
    xfig32X2Y_styfill(param, s->OSP_fill_style, fill_color, &s->OSP_fill);
    CHECK_COLOR_RETNULL(s->OSP_pen_color, pen_color, param);

    // read rawly the arrow parameters if any.
    Txfig32_arrowParam endarrow, begarrow;
    if ( (status=xfig32load_arrow(param,fa,&endarrow,ba,&begarrow))<0 ) {
        return status;
    }

    // read the control points
    s->OSP_points = read_dble2set(param,npts,"spline");
    if ( s->OSP_points==0 ) return 0;

    // read the nodes
    s->OSP_nodes = dbleset_new(npts) ;
    for (n=0 ; n<npts ; n++) {
        double x;
        count_lines_correctly(param->stream, param);
        if (fscanf(param->stream, "%lf", &x) != 1) {
            X2Y_error(param,Err_incomp " (nodes)", "spline");
            return 0;
        }
        dbleset_add(s->OSP_nodes,x);
    }
    
    n = s->OSP_points-> eleNb;
    if (closed_spline(s) && n<3) {
        X2Y_error(param, "invalid closed spline containing fewer than 3 points");
        return 3;
    } else if ( n<2 ) {
        X2Y_error(param, "invalid open spline containing fewer than 2 points");
        return 0;
    }

    /* skip to the end of the line */
    skip_line(fp);

    // add arrows if any
    TobjAll oa; oa.spline = s;
    if (fa) xfig32load_addBegEndArrowToObj(param,oa,0,&endarrow);
    if (ba) xfig32load_addBegEndArrowToObj(param,oa,1,&begarrow);

    return 1;
}

static int xfig30read_obj_text(Tloadpar*param, char*buf)
{
    TobjText   *t = param->currobj.text;
    int         l,n,len;
    int         ignore = 0;
    char        s[BUF_SIZE], s_temp[BUF_SIZE], junk[2];
    float       tx_size;
    float       length, height;
    int         more;
    int         xtype,xsubtype,depth,pen_color;
    int         ytype,ysubtype,ytags;
    int         pen_style;

    /*
     * The text object is terminated by a CONTROL-A, so we read everything up
     * to the CONTROL-A and then read that character. If we do not find the
     * CONTROL-A on this line then this must be a multi-line text object and
     * we will have to read more.
     *
     * We read text size, height and length as floats because TransFig uses
     * floats for these, but they are rounded to ints internally to xfig.
     */
    /* read the leading blanks for the string, but delete the first one later */

    /*
     * NOTE: The height, length and descent will be recalculated from the
     *       actual font structure in fig_scale_text().
     */

   /* order of parms is more like other objects now;
      string is now terminated with the literal '\001',
      and 8-bit characters are represented as \xxx */
   n = sscanf(buf, "%d%d%d%d%d%d%f%lf%d%f%f%lf%lf%[^\n]",
       &xtype,&xsubtype, &pen_color, &depth, &pen_style,
       &t->OTE_font, &tx_size, &t->OTE_angle, &t->OTE_flag,
       &height, &length, &t->OTE_base.x, &t->OTE_base.y, s);
    if ( n != 14 ) {
        X2Y_error(param,Err_incomp, "text");
        return 0;
    }
    xfig32X2Y_type(param,xtype,xsubtype,&ytype,&ysubtype,&ytags);
    if (ytype==-1 || ysubtype==-1) return 0;
    t->OTE_type = ysubtype;
    t->O_tags   = ytags;

    fix_depth(&depth,param); O_setDepth(t,depth);

    /* remove newline */
    buf[strlen(buf)-1] = '\0';
    if (buf[strlen(buf)-1] == '\r')
        buf[strlen(buf)-1] = '\0';
    /* remove any trailing carriage returns (^M, possibly from a PC) */
    if (s[strlen(s)-1] == '\r')
        s[strlen(s)-1] = '\0';
    /* use length and height, but recalculate them later when fonts
     * will be accesible. */
    t->OTE_length = length;
    t->OTE_height = height;

#if 0
    /* now round size to int and check validity */
    /* set some limits */
    if ( tx_size<FONT_MinSize )
        t->OTE_pts=FONT_MinSize;
    else if (tx_size > FONT_MaxSize)
        t->OTE_pts= FONT_MaxSize;
    else 
#endif
    t->OTE_pts = tx_size;

    /* make sure angle is 0 to 2PI */
    fix_angleD(&t->OTE_angle);

    /* check for valid font number */
    if ( t->OTE_font>=FONT_NB ) {
        X2Y_error(param,"Invalid text font (%d), setting to DEFAULT.", t->OTE_font);
        t->OTE_font = FIG_FONT_DefaultIndex;
    }

    fix_depth(&t->OTE_depth,param);
    CHECK_COLOR_RETNULL(t->OTE_color,pen_color,param);
    more = 0;
    /* in 3.0(2.2) there is more if \001 wasn't found */
    len = strlen(s);
    if ((strcmp(&s[len-4],"\\001") == 0) && /* if we find '\000' */
        !(backslash_count(s, len-5) % 2)) { /* and not '\\000' */
        more = 0;           /* then there are no more lines */
        s[len-4]='\0';          /* and get rid of the '\001' */
    } else {
        more = 1;
        s[len++]='\n';          /* put back the end of line char */
        s[len] = '\0';          /* and terminate it */
    }
    if (more) {
        /* Read in the subsequent lines of the text object
         * if there is more than one. */
        do {
            param->line_no++;       /* As is done in xfig32load_inputLine */
            if (fgets(buf, BUF_SIZE, param->stream) == NULL)
                break;
            /* remove newline */
            buf[strlen(buf)-1] = '\0';
            if (buf[strlen(buf)-1] == '\r')
                buf[strlen(buf)-1] = '\0';
            strcpy(s_temp,buf);
            len = strlen(s_temp);
            if ((strncmp(&s_temp[len-4],"\\001",4) == 0) &&
                !(backslash_count(s, len-5) % 2)) {
                n=0;            /* found the '\001', set n to stop */
                s_temp[len-4]='\0'; /* and get rid of the '\001' */
            } else {
                n=1;            /* keep going (more lines) */
            }
            /* Safety check */
            if (strlen(s) + 1 + strlen(s_temp) + 1 > BUF_SIZE) {
                /* Too many characters.  Ignore the rest. */
                if (!ignore)
                    X2Y_error(param,"Truncating TEXT object to %d chars.", BUF_SIZE);
                ignore = 1;
            }
            if (!ignore)
                strcat(s, s_temp);
        } while (n == 1);
    }

    /* now convert any \xxx to ascii characters */
    if (strchr(s,'\\')) {
        int num;
        len = strlen(s);
        for (l=0,n=0; l < len; l++) {
            if (s[l]!='\\') {
                buf[n++] = s[l];        /* ordinary character */
                continue;
            }
            /* a backslash, see if a digit follows */
            if (l < len && isdigit(s[l+1])) {
                /* yes, allow exactly 3 digits following the \ for the octal value */
                if (sscanf(&s[l+1],"%3o",&num)!=1) {
                    X2Y_error(param,"%s","Error in parsing text string.");
                    return 0;
                }
                buf[n++]= (unsigned char) num;  /* put char in */
                l += 3;         /* skip over digits */
            } else {
                buf[n++] = s[++l];      /* some other escaped character */
            }
        }
        buf[n]='\0';        /* terminate */
        strcpy(s,buf);      /* copy back to s */
    }

    if (strlen(s) <= 1) {
        s[0]=' ';s[1]=0;
    }

    /* skip first blank from input file by starting at s[1] */
    /* and convert to unicode in UTF-8                      */
    if ( t->OTE_font==34 )
        t->OTE_str=cs_conv_Zdb8toUtf8(s+1);
    else
        t->OTE_str=cs_conv_La15toUtf8(s+1);
//  printf("%s --> %s\n",s+1,t->OTE_str);

    obj_text_updateGeom(t,t->OTE_length,t->OTE_height,-1);
    return 1;
}

static int xfig32load_parseObj(Tloadpar*param, TobjCompound*dir, int xoid,
    int xsubtype, char* buf)
{
    int status;
    int (*objparser)(Tloadpar*param,char*buf);

    param->currobj = xfig32X2Y_typenew(param,xoid,xsubtype);
    if ( OBJ_IsNone(param->currobj) ) {
        X2Y_error(param,"Incorrect object code %d (skip object)",xoid);
        return 0;
    }
    param->currobj.any->OA_ppi = param->fig->fig_ppi;

    switch ( obj_Kind(param->currobj) ) {
        case FIG_KD_Arc     : objparser = xfig30read_obj_arc;     break;
        case FIG_KD_Boxhv   : objparser = xfig30read_obj_boxhv;   break;
        case FIG_KD_Ellipse : objparser = xfig30read_obj_ellipse; break;
        case FIG_KD_Line    : objparser = xfig30read_obj_line;    break;
        case FIG_KD_Spline  : objparser = xfig30read_obj_spline;  break;
        case FIG_KD_Text    : objparser = xfig30read_obj_text;    break;
        default:
            X2Y_error(param,"Unimplemented object code, skip object (yfig=%d xfig=%d/%d)",
                    obj_Kind(param->currobj),xoid,xsubtype);
            obj_delete(param->currobj);
            return 0;
    }
    if ( (status=objparser(param,buf))<=0 ) {
        obj_delete(param->currobj);
        tool_strfreeS0(&param->comment);
        return status;
    }
    attach_comment(param,param->currobj.any);
    obj_dir_addObj( obj_ObjAll(dir), param->currobj);
    param->num_object++;
    return 1;
}

static TobjCompound * xfig30read_obj_compound(char*buf, Tloadpar*param)
{
    TobjAll       obj;
    TobjCompound* dir;
    TobjAny  *a;
    int      n, xoid, xsid;

    obj=obj_dir_newF(NULL);
    obj.any->OA_ppi = param->fig->fig_ppi;
    dir=obj.dir;

    attach_comment(param,O_2A(dir));

    /* read bounding info of compound, not used, we calculate it later */
    n = sscanf(buf, "%*d%*f%*f%*f%*f\n");

    while ( xfig32load_inputLine(buf,param) > 0) {
        switch ( sscanf(buf, "%d%d", &xoid,&xsid) ) {
          case 1: xsid=-1; break;
          case 2:          break;
          default:
            X2Y_error(param,Err_incomp, "compound", param->line_no);
            obj_delete( obj );
            return NULL;
        }
        if ( xoid==XFIG_KD_Compound ) {
            if ( (a=(TobjAny*)xfig30read_obj_compound(buf,param))==NULL ) continue;
            obj_dir_addObj( obj, obj_ObjAll(a) );
            param->num_object++;
            continue;
        } else if ( xoid==XFIG_KD_CompoundEnd ) {
            return dir;
        } else {
            xfig32load_parseObj(param,dir,xoid,xsid,buf);
            continue;
        }
    } /* while ( xfig32load_inputLine(buf,param) > 0) */

    if (feof(param->stream)) {
        return dir;
    } else {
        return NULL;
    }
}

/*======================================================================*/

static int xfig32load_objects(Tloadpar* param)
{
    Tfigure*      fig = param->fig;
    TobjCompound* obj = (TobjCompound*)fig->objects;
    TobjAny  *a;
    int      xoid,xsid ;
    char     buf[BUF_SIZE];

    /* attach any comments found thus far to the whole figure */
    attach_comment(param,O_2A(obj));

    while ( xfig32load_inputLine(buf,param) > 0) {
        switch ( sscanf(buf, "%d%d", &xoid,&xsid) ) {
          case 1: xsid=-1; break;
          case 2:          break;
          default:
            X2Y_error(param,Err_incomp, "top compound", param->line_no);
            return (param->num_object != 0? 0: -1); /* ok if any objects have been read */
        }
        if ( xoid==XFIG_KD_ColorDef ) {
            if ( fig_read30_colordef(buf,param)<0 )
                return -1;
            if (param->num_object) {
                X2Y_error(param,"%s","Color definitions must come before other objects.");
            }
            continue;
        } else if ( xoid==XFIG_KD_Compound ) {
            if ( (a=(TobjAny*)xfig30read_obj_compound(buf,param))==NULL ) continue;
            obj_dir_addObj( obj_ObjAll(obj), obj_ObjAll(a));
            continue;
        } else {
            xfig32load_parseObj(param,obj,xoid,xsid,buf);
            continue;
        }
    } /* while */

    if (feof(param->stream))
        return 0;
    else
        return -1;
}

/*======================================================================*/
