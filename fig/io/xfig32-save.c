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
 *      $Id: xfig32-save.c 280 2018-07-05 23:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/io/xfig32-save.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= handle back-up of figure to original XFIG format 32                =*/
/*======================================================================*/

#include "xfig32.h"
#include "tools/charset.h"

/*======================================================================*/
static int f32write_header  (Tsavepar*sp, Cfigure* fig);
static int f32write_colors  (Tsavepar*sp, Cfigure* fig);
static int f32write_arc     (Tsavepar*sp, TobjArc *a);
static int f32write_compound(Tsavepar*sp, TobjCompound *com);
static int f32write_ellipse (Tsavepar*sp, TobjEllipse *e);
static int f32write_line    (Tsavepar*sp, TobjLine *l);
static int f32write_boxhv   (Tsavepar*sp, TobjAll  obj);
static int f32write_spline  (Tsavepar*sp, TobjSpline *t);
static int f32write_text    (Tsavepar*sp, TobjText *t);
static int f32write_arrowsFL(Tsavepar*sp, TobjAll obj);
static int f32write_arrows  (Tsavepar*sp, TobjAll obj);

#define SRf32write_arrowsFL(sp,obj) do { if (f32write_arrowsFL(sp,obj)<0) return -3; } while (0)
#define SRf32write_arrows(sp,obj)   do { if (f32write_arrows  (sp,obj)<0) return -3; } while (0)

static int xfig32_dirloop(Tsavepar*sp, TobjAll dir)
{
    int ret;
    TobjCompound* objects= dir.compound;
    TobjAny*      a;
    for (a = objects->OCO_arcs; a != NULL; a = a->oa_next)
        if ( (ret=f32write_arc(sp, (TobjArc*)a))<0 ) return ret;
    for (a = objects->OCO_ellipses; a != NULL; a = a->oa_next)
        if ( (ret=f32write_ellipse(sp, (TobjEllipse*)a))<0 ) return ret;
    for (a = objects->OCO_lines; a != NULL; a = a->oa_next)
        if ( (ret=f32write_line(sp, (TobjLine*)a))<0 ) return ret;
    for (a = objects->OCO_splines; a != NULL; a = a->oa_next)
        if ( (ret=f32write_spline(sp, (TobjSpline*)a))<0 ) return ret;
    for (a = objects->OCO_texts; a != NULL; a = a->oa_next)
        if ( (ret=f32write_text(sp, (TobjText*)a))<0 ) return ret;
    for (a = objects->OCO_compounds; a != NULL; a = a->oa_next)
        if ( (ret=f32write_compound(sp, (TobjCompound*)a))<0 ) return ret;
    for (a = objects->OCO_others; a != NULL; a = a->oa_next)
        if ( (ret=f32write_boxhv(sp, obj_ObjAll(a)))<0 ) return ret;
}

extern int xfig32_save(Tsavepar*sp, Cfigure*fig)
{
    int ret;

    if ( sp->fmt!=32 ) return -1;

    if ( (ret=f32write_header(sp,fig))!=0 ) goto end;
    if ( (ret=f32write_colors(sp,fig))!=0 ) goto end;

    TobjAll  dir = obj_ObjAll(fig->objects);
    ret=xfig32_dirloop(sp,dir);

end:

    return 0;
}

/*======================================================================*/

#define SRprintf(...) do { if ( fprintf(sp->stream,__VA_ARGS__)<0 ) return -3; } while(0)
#define SRf32write_dble2T(p,t,n) do { int ret=f32write_dble2T(p,t,n); \
                                      if (ret<0) return 0; } while (0)
#define SRf32write_dble2S(p,s) do { int ret=f32write_dble2T(p,s->eles,s->eleNb); \
                                    if (ret<0) return 0; } while (0)

static int f32write_dble2T(Tsavepar*sp, Tdble2* pts, int nb)
{
    int n;
    SRprintf( "\t");
    for (n=0 ; n<nb ; n+=1) {
        if (n!=0 && (n%6)==0) SRprintf("\n\t");
        SRprintf( " %d %d", (int)round(pts[n].x), (int)round(pts[n].y));
    }
    SRprintf( "\n");
    return 0;
}

static int f32write_comments(Tsavepar*sp, CstrBrief com)
{
    char last;

    if (!com || !*com) return 0;
    SRprintf("# ");
    while (*com) {
        last = *com;
        if ( fputc(*com,sp->stream)<0 ) return -3;
        if (*com == '\n' && *(com+1) != '\0')
            SRprintf("# ");
        com++;
    }
    /* add newline if last line of comment didn't have one */
    if (last != '\n')
        SRprintf("\n");
    return 0;
}

/*======================================================================*/

static int f32write_header(Tsavepar*sp, Cfigure* fig)
{
    int ret;
    const Tfig_settings* settings = &fig->settings;
    TobjCompound* self = (TobjCompound*) fig->objects;

    SRprintf( "#FIG %s Produced by yfig version %s%s\n",
            sp->fmtstr, YFIG_VERS, YFIG_VERS_PATCH);
    SRprintf( "%s\n",   settings->landscape ? "Landscape" : "Portrait");
    SRprintf( "%s\n",   settings->flushleft ? "Flush left": "Center");
    SRprintf( "%s\n",   settings->userUnit.uu_id==LUNIT_ID_Cm
                                            ? "Metric"    : "Inches");
    SRprintf( "%s\n",   settings->papersize->sname);
    SRprintf( "%.2f\n", settings->magnification);
    SRprintf( "%s\n",   settings->multiple  ? "Multiple"  : "Single");
    SRprintf( "%d\n",   settings->transparent);
    if ( (ret=f32write_comments(sp, self->O_comment))!=0 )
        return ret;
    int ppi = fig->io_ppi!=0 ? fig->io_ppi : fig->fig_ppi;
    SRprintf( "%d %d\n", ppi , 2); // resolution

    return 0;
}

static int f32write_colors(Tsavepar*sp, Cfigure*fig)
{
    TcolorSet* colors = fig->colors.privateColors;

    if ( colors==0 ) return 0;
//printf("%s: #%04x:%04x:%04x\n", __func__,c->rgb.red,c->rgb.green,c->rgb.blue);
    int i;
    for ( i=0i ; i<colors->eleNb ; i+=1 ) {
        Tcolor* c = colors->eles[i];
        SRprintf( "0 %d #%02x%02x%02x\n",
                i+FIG_NbPredefinedColor,
                c->rgb.red,
                c->rgb.green,
                c->rgb.blue);
    }
    return 0;
}

static int f32write_arc(Tsavepar*sp, TobjArc *a)
{
    TobjAll obj = obj_ObjAll(a);
    int ret;
    if ( (ret=f32write_comments(sp, a->O_comment))<0 ) return ret;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    int fill_style,fill_color;
    xfig32Y2X_styfill(sp,obj_StyFill(obj),&fill_style,&fill_color);

    /* externally, type 1=open arc, 2=pie wedge */
    SRprintf( "%d %d %d %d %d %d %d",
            xoid, xsid, a->OAR_style, a->OAR_thickness,
            a->OAR_pen_color->ukey, fill_color, a->OAR_depth);
            
    SRprintf( " -1 %d %.3f %d %d ",
            fill_style, a->OAR_style_val,
            a->OAR_cap, a->OAR_direction);
    SRf32write_arrowsFL(sp,obj);        
    SRprintf( " %.3f %.3f %.0f %.0f %.0f %.0f %.0f %.0f\n",
            a->OAR_center.x, a->OAR_center.y,
            a->OAR_points[0].x, a->OAR_points[0].y,
            a->OAR_points[1].x, a->OAR_points[1].y,
            a->OAR_points[2].x, a->OAR_points[2].y);

    /* write any arrowheads */
    SRf32write_arrows(sp,obj);
    return 0;
}

/*======================================================================*/

static int f32write_compound(Tsavepar*sp, TobjCompound *com)
{
    int      ret;

    /* any comments first */
    if ( (ret=f32write_comments(sp, com->O_comment))<0 ) return ret;

    CboxDble* bb= obj_BBox( obj_ObjAll(com) );
    SRprintf( "%d %.0f %.0f %.0f %.0f\n", XFIG_KD_Compound,
        Box_x(*bb),Box_y(*bb),                          // North-West corner
        Box_x(*bb)+Box_dx(*bb),Box_y(*bb)+Box_dy(*bb)); // Opposite corner

    TobjAll dir = obj_ObjAll(com);
    if ( (ret=xfig32_dirloop(sp,dir))<0 ) return ret;

    /* close off the compound */
    SRprintf( "%d\n", XFIG_KD_CompoundEnd);

    return 0;
}

static int f32write_ellipse(Tsavepar*sp, TobjEllipse *e)
{
    TobjAll obj = obj_ObjAll(e);
    int ret;

    /* get rid of any evil ellipses which have either radius = 0 */
    if (e->OEL_radiuses.x == 0 || e->OEL_radiuses.y == 0)
        return 0;

    /* any comments first */
    if ( (ret=f32write_comments(sp, e->O_comment))<0 ) return ret;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    int fill_style,fill_color;
    xfig32Y2X_styfill(sp,obj_StyFill(obj),&fill_style,&fill_color);

    SRprintf("%d %d %d %d %d %d %d %d %d %.3f %d %.4f",
        xoid, xsid, e->OEL_style, e->OEL_thickness,
        e->OEL_pen_color->ukey, fill_color,
        e->OEL_depth, -1, fill_style,
        e->OEL_style_val, e->OEL_direction, e->OEL_angle);

    SRprintf(" %d %d %d %d %d %d %d %d\n",
        (int)round(e->OEL_center.x  ) , (int)round(e->OEL_center.y),
        (int)round(e->OEL_radiuses.x) , (int)round(e->OEL_radiuses.y),
        (int)round(e->OEL_start.x   ) , (int)round(e->OEL_start.y),
        (int)round(e->OEL_end.x     ) , (int)round(e->OEL_end.y));
    return 0;
}

static int f32write_line(Tsavepar*sp, TobjLine *l)
{
    TobjAll obj = obj_ObjAll(l);
    int ret,n;

    if (l->OLI_points == NULL) return 0;

    /* any comments first */
    if ( (ret=f32write_comments(sp, l->O_comment))<0 ) return ret;;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    int fill_style,fill_color;
    xfig32Y2X_styfill(sp,obj_StyFill(obj),&fill_style,&fill_color);

    int radius = l->OLI_radius;
    if ( radius==0 && l->OLI_type==FIGOLI_T_RoundBox) radius=15;

    SRprintf( "%d %d %d %d %d %d %d %d %d %.3f %d %d %d ",
        xoid, xsid, l->OLI_style, l->OLI_thickness,
        l->OLI_pen_color->ukey, fill_color, l->OLI_depth, -1, 
        fill_style, l->OLI_style_val, l->OLI_join, l->OLI_cap, 
        radius);
    SRf32write_arrowsFL(sp,obj);        
    SRprintf( " %d\n",l->OLI_points->eleNb);

    /* write any arrowheads */
    SRf32write_arrows(sp,obj);        

    // writes vertices
    SRf32write_dble2S(sp,l->OLI_points);

    return 0;
}

static int f32write_boxhv(Tsavepar*sp, TobjAll obj)
{
    TobjBoxhv* self = obj.boxhv;
    int ret;

    /* any comments first */
    if ( (ret=f32write_comments(sp, self->O_comment))<0 ) return ret;;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    if ( self->OBHV_type==FIGOBHV_T_Image ) {
        Timage* img = self->OBHV_img;
        int flip,p0,p1,p2,p3;
        xfig32Y2X_fliprot(sp,img->img_ope,&flip,&p0,&p1,&p2,&p3);
        Tdble2Set* pts = dble2set_new(5);
        dble2set_addPT(pts,self->OBHV_points[p0]);
        dble2set_addPT(pts,self->OBHV_points[p1]);
        dble2set_addPT(pts,self->OBHV_points[p2]);
        dble2set_addPT(pts,self->OBHV_points[p3]);
        dble2set_addPT(pts,self->OBHV_points[p0]);

        SRprintf( "%d %d 0 1 0 -1 %d -1 -1 0.000 0 0 -1 0 0 5\n",
            xoid, xsid, self->O_depth);
        SRprintf( "\t%d %s\n", flip,img->img_path);
        SRf32write_dble2S(sp,pts);

        dble2set_delete(pts);
        return 0;
    }
    Y2X_error(sp,"%d type of boxhv object not implemented"
                 " (objcet is skipped)", self->OBHV_type);
    return -1;
}

static int f32write_spline(Tsavepar*sp, TobjSpline *s)
{
    TobjAll obj = obj_ObjAll(s);
    int        ret,n;

    if (s->OSP_points == NULL) return 0;

    /* any comments first */
    if ( (ret=f32write_comments(sp, s->O_comment))<0 ) return ret;;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    int fill_style,fill_color;
    xfig32Y2X_styfill(sp,obj_StyFill(obj),&fill_style,&fill_color);

    SRprintf( "%d %d %d %d %d %d %d %d %d %.3f %d ",
            xoid, xsid, s->OSP_style, s->OSP_thickness,
            s->OSP_pen_color->ukey, fill_color, s->OSP_depth, -1,
            fill_style, s->OSP_style_val, s->OSP_cap);
    SRf32write_arrowsFL(sp,obj);        
    SRprintf( " %d\n",s->OSP_points->eleNb);

    /* write any arrowheads */
    SRf32write_arrows(sp,obj);        

    // writes conttrol points
    SRf32write_dble2S(sp,s->OSP_points);

    /* save nodes */
    SRprintf( "\t");
    for (n=0 ; n<s->OSP_nodes->eleNb ; n+=1) {
        double v = s->OSP_nodes->eles[n];
        if (n!=0 && (n%6)==0) SRprintf("\n\t");
        SRprintf( " %f", v);
    }
    SRprintf("\n");

    return 0;
}

static int f32write_text(Tsavepar*sp, TobjText *t)
{
    TobjAll obj = obj_ObjAll(t);
    int ret;
    int l, len;
    unsigned char   c;

    if ( t->OTE_str==0 || *t->OTE_str==0 ) return 0;

    /* any comments first */
    if ( (ret=f32write_comments(sp, t->O_comment))<0 ) return ret;;

    int xoid,xsid;
    xfig32Y2X_type(sp,obj,&xoid,&xsid);

    SRprintf( "%d %d %d %d -1",
        xoid, xsid, t->OTE_color->ukey, t->O_depth);
    SRprintf(" %d %.0f %.4f %d %d %d %.0f %.0f ",
        t->OTE_font, t->OTE_pts, t->OTE_angle,
        t->OTE_flag, (int)t->OTE_height, (int)t->OTE_length,
        t->OTE_base.x, t->OTE_base.y);
    char* str_latin;
    if ( t->OTE_font==34 )
        str_latin=cs_conv_Utf8toZdb8(t->OTE_str);
    else
        str_latin=cs_conv_Utf8toLa15(t->OTE_str);
    len = strlen(str_latin);
    for (l=0; l<len; l++) {
        c = str_latin[l];
        if (c == '\\')
            SRprintf("\\\\");         /* escape a '\' with another one */
        else if (c < 0x80) {
            if ( putc(c,sp->stream)<0 ) return -3;  /* normal 7-bit ASCII */
        } else
            SRprintf( "\\%3o", c);  /* 8-bit, make \xxx (octal) */
    }
    SRprintf("\\001\n");              /* finish off with '\001' string */
    free( str_latin );
    return 0;
}

/*======================================================================*/
/*= write any arrow heads                                              =*/

static int f32write_arrowsFL(Tsavepar*sp, TobjAll obj)
{
    int end = obj_arrow_EndPtr(obj)==0 ? 0 : OBJ_IsDefined( *obj_arrow_EndPtr(obj) ); 
    int beg = obj_arrow_BegPtr(obj)==0 ? 0 : OBJ_IsDefined( *obj_arrow_BegPtr(obj) ); 
    SRprintf("%d %d",end,beg);
    return 0;
}

extern void xfig32Y2X_arrow(Tsavepar* param, TobjAll arrow, int*type, int*style);

static int f32write_arrow (Tsavepar*sp, int addspace, TobjAll obj)
{
    TobjArrow* oaw = obj.arrow;
    int    type,style;  xfig32Y2X_arrow(sp,obj,&type,&style);
    double thickness = oaw->OAW_thickness;
    double width     = oaw->OAW_currlength;
    double height    = 2.0 * tan( oaw->OAW_baseangle) * width;
#define OAW_COEF 1.

    if (addspace) SRprintf("%s"," ");
    SRprintf( "\t%d %d %.2f %.2f %.2f\n", type, style, thickness, height*OAW_COEF, width*OAW_COEF);
    return 0;
}

static int f32write_arrows(Tsavepar*sp, TobjAll obj)
{
    TobjAll* end = obj_arrow_EndPtr(obj);
    TobjAll* beg = obj_arrow_BegPtr(obj);
    int ret=0,addspace=0;
    if ( end && OBJ_IsDefined(*end) ) {
        ret = f32write_arrow(sp,addspace,*end);
        if ( ret<0 ) return ret;
        addspace=1;
    }
    if ( beg && OBJ_IsDefined(*beg) )
        ret = f32write_arrow(sp,addspace,*beg);
    return ret;
}

/*======================================================================*/
