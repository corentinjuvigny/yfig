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
 *      $Id: figure.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/figure.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= high level figure operations.                                      =*/
/*= See the fig/figure.h header for types and documentation.           =*/
/*======================================================================*/

#include "fig-config.h"
#include "fig/figure.h"
#include "fig/fig-histo.h"
#include "fig/object-priv.h"
#include "fig/obj-compound.h"
#include "tools/lenunit.h"
#include "tools/math-macros.h"

#include "yfig-version.h"

/*======================================================================*/
/*= Tfig_objCounters function                                          =*/

extern int fig_objcnt_sum( const Tfig_objCounters* p)
{
    return p->num_arcs     + p->num_lines +
           p->num_ellipses + p->num_splines +
           p->num_texts;
}

/*======================================================================*/
/*= Tfig_colors function                                               =*/

extern const Tcolor* fig_colors_getByUKey(  const Tfig_colors* colors, int key)
{
    if ( key<FIG_NbPredefinedColor )
        return colorset_getByUKey(colors->predefinedColors,key);
    else
        return colorset_getByUKey(colors->privateColors,key);
}

extern TcolorEGS fig_colors_getByUKey_G(const Tfig_colors* colors, int key)
{
    const Tcolor* c = fig_colors_getByUKey(colors,key);
    return c!=0 ? c->egs : EGSCOLOR_none;
}

extern Ccolor* fig_colors_getByClr(Tfigure* f, Ccolor*c)
{
    Tfig_colors* colors = &f->colors;
    Ccolor* fc; // fig color associated to c

    // look for c in predefined color set
    if ( (fc=colorset_getByPtr(colors->predefinedColors,c))!=0 ) return fc;
    if ( (fc=colorset_getByRGB(colors->predefinedColors,c->rgb))!=0 ) return fc;

    // look for c in private color set
    if ( colors->privateColors!=0 ) {
        if ( (fc=colorset_getByPtr(colors->privateColors,c))!=0 ) return fc;
        if ( (fc=colorset_getByRGB(colors->privateColors,c->rgb))!=0 ) return fc;
    } else {
        colors->privateColors=colorset_newDNT(
            colors->predefinedColors->driver,
            16,
            colors->predefinedColors->tolerance);
    }

    // add a new color because c color is missing
    fc = (Tcolor*) colorset_addPriv(colors->privateColors,c);
    return fc;
}

extern Ccolor* fig_colors_get0(Tfig_colors* colors, Ccolor*c)
{
    Ccolor* fc; // fig color associated to c
    if ( (fc=colorset_getByPtr(colors->predefinedColors,c))!=0 )
        return fc;
    if ( (fc=colorset_getByRGB(colors->predefinedColors,c->rgb))!=0 )
        return fc;

    // look for c in private color set
    if ( colors->privateColors!=0 ) {
        if ( (fc=colorset_getByPtr(colors->privateColors,c))!=0 )
            return fc;
        if ( (fc=colorset_getByRGB(colors->privateColors,c->rgb))!=0 )
            return fc;
    }
    return NULL;
}

extern int fig_colors_add0(Tfig_colors* colors, Ccolor*c)
{
    // look for c in predefined color set
    if ( colorset_getByPtr(colors->predefinedColors,c)!=0 )      return 0;
    if ( colorset_getByRGB(colors->predefinedColors,c->rgb)!=0 ) return 0;

    // look for c in private color set
    if ( colors->privateColors!=0 ) {
        if ( colorset_getByPtr(colors->privateColors,c)!=0 )      return 0;
        if ( colorset_getByRGB(colors->privateColors,c->rgb)!=0 ) return 0;
    }

    // add c in the private color set
    if ( colors->privateColors==0 ) {
        colors->privateColors=colorset_newDNT(
            colors->predefinedColors->driver,
            16,
            colors->predefinedColors->tolerance);
    }
    colorset_addPriv(colors->privateColors,c);
    return 1;
}

extern TcolorSet* fig_colors_newSharedColor(CcolorDriver*drv)
{
    static const char* colors[FIG_NbPredefinedColor][2] = {
    /*  0 */ {"Black",     "#000000"} , {"Blue",      "#0000FF"} ,
             {"Green",     "#00FF00"} , {"Cyan",      "#00FFFF"} ,
             {"Red",       "#FF0000"} , {"Magenta",   "#FF00FF"} ,
             {"Yellow",    "#FFFF00"} , {"White",     "#FFFFFF"} ,
             {"Blue4",     "#000090"} , {"Blue3",     "#0000b0"} ,
    /* 10 */ {"Blue2",     "#0000d0"} , {"LtBlue",    "#87ceff"} ,
             {"Green4",    "#009000"} , {"Green3",    "#00b000"} ,
             {"Green2",    "#00d000"} , {"Cyan4",     "#009090"} ,
             {"Cyan3",     "#00b0b0"} , {"Cyan2",     "#00d0d0"} ,
             {"Red4",      "#900000"} , {"Red3",      "#b00000"} ,
    /* 20 */ {"Red2",      "#d00000"} , {"Magenta4",  "#900090"} ,
             {"Magenta3",  "#b000b0"} , {"Magenta2",  "#d000d0"} ,
             {"Brown4",    "#803000"} , {"Brown3",    "#a04000"} ,
             {"Brown2",    "#c06000"} , {"Pink4",     "#ff8080"} ,
             {"Pink3",     "#ffa0a0"} , {"Pink2",     "#ffc0c0"} ,
    /* 30 */ {"Pink",      "#ffe0e0"} , {"Gold",      "#ffd700" }
    };
    int i;
    TcolorSet* sc = colorset_newDNT(0,FIG_NbPredefinedColor,7);
    for ( i=0; i<FIG_NbPredefinedColor ; i+=1) {
        Tcolor* c;
        int r,g,b;
        const char* name = colors[i][0];
        const char* rgb  = colors[i][1];
        sscanf(rgb,"#%02x%02x%02x",&r,&g,&b);
        //r <<= 8; g <<= 8; b <<= 8;
        c = color_new(name,r,g,b,EGSCOLOR_none);
        c->ukey = i;
        colorset_addEated(sc,c);
    }
    if (drv)
        colorset_update(sc,drv);
    return sc;
}

extern Ccolor*    fig_colors_getBlack  (CcolorSet* s) { return s->eles[ 0]; }
extern Ccolor*    fig_colors_getBlue   (CcolorSet* s) { return s->eles[ 1]; }
extern Ccolor*    fig_colors_getGreen  (CcolorSet* s) { return s->eles[ 2]; }
extern Ccolor*    fig_colors_getCyan   (CcolorSet* s) { return s->eles[ 3]; }
extern Ccolor*    fig_colors_getRed    (CcolorSet* s) { return s->eles[ 4]; }
extern Ccolor*    fig_colors_getMagenta(CcolorSet* s) { return s->eles[ 5]; }
extern Ccolor*    fig_colors_getYellow (CcolorSet* s) { return s->eles[ 6]; }
extern Ccolor*    fig_colors_getWhite  (CcolorSet* s) { return s->eles[ 7]; }
extern Ccolor*    fig_colors_getBlue4  (CcolorSet* s) { return s->eles[ 8]; }
extern Ccolor*    fig_colors_getBlue3  (CcolorSet* s) { return s->eles[ 9]; }
extern Ccolor*    fig_colors_getBlue2  (CcolorSet* s) { return s->eles[10]; }
extern Ccolor*    fig_colors_getLtBlue (CcolorSet* s) { return s->eles[11]; }

/*======================================================================*/
/*= Tfig function                                                      =*/

extern Tfigure* fig_new_empty(Tmsgdrv* md, CstrBrief n, Cfig_settings* gp,
                              CcolorSet* cs)
{
    Tfigure* fig = tool_malloc_0(sizeof(*fig));

    fig->path = tool_strdup(n);

    if ( gp ) 
        fig->settings=*gp;
    else {
        /* initialize settings structure with default */
        fig->settings.landscape     = 1;
        fig->settings.flushleft     = 1;
        lu_init(&fig->settings.userUnit,LUNIT_ID_Inch,1.0,10);
        fig->settings.papersize     = tool_paperdef_get("A4");
        fig->settings.magnification = 100.0;
        fig->settings.multiple      = 0; // only an A4 page
        fig->settings.transparent   = 1;
    }

    fig->colors.predefinedColors = cs;
    fig->fig_ppi = LUNIT_FuDefault;
    fig->refcnt  = 1;
    fig->msgdrv  = md;

    TobjAll root = obj_dir_newF(NULL);
    fig->objects = root.dir;
    obj_fig_attach( root, fig);
    return fig;
}

extern Tfigure* fig_new_lazy(Tfigure* fig)
{ fig->refcnt+=1; return fig; }

extern Tfigure* fig_new_clone(Cfigure* src)
{
    Tfigure* fig = tool_malloc(sizeof(*fig));

    fig->path          = tool_strdup(src->path);
    fig->loaded_format = src->loaded_format;
    fig->save_format   = src->save_format;
    fig->io_ppi        = src->io_ppi;

    fig->settings      = src->settings;
    memset(&fig->objCounters,0,sizeof(fig->objCounters));
    fig->colors.predefinedColors = src->colors.predefinedColors;
    fig->colors.privateColors    = NULL; //src->colors.privateColors;
    fig->fig_ppi       = src->fig_ppi;

    fig->history       = NULL;
    fig->refcnt        = 1;

    fig->objects       = obj_newClone(obj_ObjAll(src->objects)).dir;
    obj_fig_attach( obj_ObjAll(fig->objects), fig);
    return fig;
}

static void     fig_free(Tfigure* fig)
{
    if ( fig==0 ) return;
    if ( fig->history )
        fig_histo_delete( fig->history );
    obj_delete( obj_ObjAll(fig->objects) );
    colorset_free(fig->colors.privateColors);
    tool_free( fig->path ); 
    free(fig);
}

extern void fig_delete(Tfigure* fig)
{
  fig->refcnt-=1;
  TEMP_ase_printf(fig->refcnt<0,"unexpected fig->refcnt=%d",fig->refcnt);
  if (fig->refcnt<=0)
      fig_free( fig );
}


/*======================================================================*/
/*= Miscellaneous figure services.                                     =*/

extern void fig_chgPath    (Tfigure* fig, CstrBrief path)
{
    tool_strchgClone(&fig->path,path);
}

extern void fig_chgUnit(Tfigure* fig, double ppi)
{
    if ( dble_close(ppi,fig->fig_ppi) ) return;
    obj_chgPpi( obj_ObjAll(fig->objects), ppi);
    fig->fig_ppi = ppi;
}

extern void fig_clrEGScache(Tfigure* fig, void *od)
{ obj_clrEGScache( obj_ObjAll(fig->objects), od ); }

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
extern void fig_update_forDrawing(Tfigure* fig, CcolorDriver*drv)
{
    if ( drv!=0 )
        colorset_update(fig->colors.privateColors,drv);
    if ( fig->loaded_format==0 )
        return;
    double scale=0;
    double shift=0;
    if ( fig->loaded_format==30 ) {
        scale= fig->fig_ppi/fig->loaded_format;
        shift= 0;
    } else if ( fig->io_ppi!=fig->fig_ppi ) {
        if ( fig->loaded_format==21 && fig->io_ppi==76 ) {
            scale= fig->fig_ppi/80.;
            shift= 15;
        } else {
            scale= fig->fig_ppi/fig->loaded_format;
            shift= 15;
        }
    }
    if ( scale!=0 || shift!=0 ) {
        tool_inferr_printf(fig->msgdrv,"scaled %s figure of format %d.%d with %f factor.",
            fig->path,
            fig->loaded_format/10,
            fig->loaded_format%10,
            scale);
        tool_inferr_printf(fig->msgdrv,"shifted %s figure of format %d.%d with %f inch.",
            fig->path,
            fig->loaded_format/10,
            fig->loaded_format%10,
            shift/fig->fig_ppi);
    } else {
        tool_inferr_printf(fig->msgdrv,"%s figure of format %d.%d not scaled nor shifted.",
            fig->path,
            fig->loaded_format/10,
            fig->loaded_format%10);
    }
}

/*======================================================================*/
/*= fig backup format.                                                 =*/

extern CstrStatic fig_filefmt_default()                { return YFIG_VERS_XFigFormat_STR; }
extern CstrStatic fig_filefmt_defaultFig(Cfigure* fig) { return fig_filefmt_default(); }

extern int        fig_filefmt_get2(Cfigure*fig, int first, int second)
{
    if ( first==YFIG_VERS_XFigFormat_99 )
        return first;
    else if ( second==YFIG_VERS_XFigFormat_99 )
        return second;
    else
        return 0;
}

CstrStatic     fig_filefmt_int2str(int        fmt)
{
    if ( fmt==YFIG_VERS_XFigFormat_99 )
        return YFIG_VERS_XFigFormat_STR;
    else
        return "undef";
}

int             fig_filefmt_str2int(CstrBrief fmt)
{
    if ( strcmp(fmt,YFIG_VERS_XFigFormat_STR)==0 )
        return YFIG_VERS_XFigFormat_99;
    else
        return 0;
}

/*======================================================================*/
