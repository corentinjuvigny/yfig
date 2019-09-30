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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-main.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-main.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Functions that create initialize and free the VGUI (Tvgui type)    =*/
/*=                                                                    =*/
/*= See the vgui.h header for the type and function documentation.     =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vgui-resources.h"
#include "vgui/vgui-outdev.h"

#include "outdev/style.h"
#include "outdev/resources.h"
#include "outdev/ldaf-hl.h"
#include "outdev/string-hl.h"

#include "tools/font.h"
#include "tools/color.h"
#include "tools/messagedriver.h"
#include "tools/globalvars.h"

/*======================================================================*/
/*= Color creation and freeing                                         =*/

static void main_colors_init_ByNameF(Tcolor**clr, TcolorDriver*drv,
        CstrBrief clr_name)
{
    if ( (*clr=color_newByStr(drv,clr_name))==0 ) {
        tool_ftlerr_printf(NULL,"can not get the %s color.",clr_name);
        exit(1);
    }
}

static void main_colors_init_ByNameD(Tcolor**clr, TcolorDriver*drv,
        CstrBrief clr_name, Tcolor* clrdef)
{
    if ( (*clr=color_newByStr(drv,clr_name))==0 ) {
        tool_wrnerr_printf(NULL,"can not get the %s color (set to %s)",clr_name,clrdef->name);
        *clr = color_newLazy(clrdef);
    }
}

static void main_colors_init_ByResD(Tcolor**clr, Tvgui*gui,
        CstrBrief res_name, Tcolor* clrdef)
{
    Tcolor*     color;
    const char* clr_name;
    TEMP_asf_printf( res_getStr(&gui_appResources,res_name,&clr_name)!=0,
        "%s resource not found (ret=%d)",
        res_name,res_getStr(&gui_appResources,res_name,&clr_name));

    res_getStr(&gui_appResources,res_name,&clr_name);
    if ( (*clr=color_newByStr(gui->colorDriver,clr_name))==0 ) {
        tool_wrnerr_printf(NULL,"can not get the %s color (set to %s)",clr_name,clrdef->name);
        *clr = color_newLazy(clrdef);
    }
}

static void main_colors_init(Tvgui*gui)
{
    // initialization of the color driver
    TcolorDriver *drv = tool_malloc( sizeof(*drv) );
    drv->egs         = gui;
    drv->free        = (void     (*)(void*egs, TcolorEGS c))gui_color_delete;
    drv->dup         = (TcolorEGS(*)(void*egs, TcolorEGS c))gui_color_dup;
    drv->newByName   = (TcolorEGS(*)(void*egs, CstrBrief n))gui_color_newByName;
    drv->newByRGB08  = (TcolorEGS(*)(void*egs, int r, int g, int b))gui_color_newByRGB08;
    drv->newByRGB16  = (TcolorEGS(*)(void*egs, int r, int g, int b))gui_color_newByRGB16;
    drv->getRGB08    = (void     (*)(void*egs, TcolorEGS c,
                                     int*r, int*g, int*b))gui_color_getRGB08;
    drv->getRGB16    = (void     (*)(void*egs, TcolorEGS c, int*r,
                                     int*g, int*b))gui_color_getRGB16;
    gui->colorDriver = drv;

    // initialization of the colors
#   undef   VGUI_DO_COLOR_ByNameF
#   undef   VGUI_DO_COLOR_ByNameD
#   undef   VGUI_DO_COLOR_ByResD
#   define  VGUI_DO_COLOR_ByNameF(f,cn)    main_colors_init_ByNameF(&gui->f,drv,cn);
#   define  VGUI_DO_COLOR_ByNameD(f,cn,df) main_colors_init_ByNameD(&gui->f,drv,cn,gui->df);
#   define  VGUI_DO_COLOR_ByResD(f,rn,df)  main_colors_init_ByResD (&gui->f,gui,rn,gui->df);
#   include "vgui/vgui-colors.h"
}

static void main_colors_free(Tvgui*gui)
{
    // free the colors
#undef VGUI_DO_COLOR_ByNameF
#undef VGUI_DO_COLOR_ByNameD
#undef VGUI_DO_COLOR_ByResD
#if defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr
#define VGUI_DO_COLOR_ByNameF(f,cn)    if (gui->f->cnt!=1) color_delete(gui->f);\
                                       else color_deleteS0(&gui->f);
#else
#define VGUI_DO_COLOR_ByNameF(f,cn)    color_delete(gui->f);
#endif
#define VGUI_DO_COLOR_ByNameD(f,cn,df) VGUI_DO_COLOR_ByNameF(f,0)
#define VGUI_DO_COLOR_ByResD(f,rn,df)  VGUI_DO_COLOR_ByNameF(f,0)
#include "vgui/vgui-colors.h"

    // free the color driver
    free( gui->colorDriver );

#if defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr
    // verify if colors are really freed (they must be the NULL pointer).
#   undef  VGUI_DO_COLOR_ByNameF
#   undef  VGUI_DO_COLOR_ByNameD
#   undef  VGUI_DO_COLOR_ByResD
#   define VGUI_DO_COLOR_ByNameF(f,cn)    PERM_ase_printf( gui->f!=0,\
            "gui->" #f " not freed (refcnt=%d)",gui->f->cnt);
#   define VGUI_DO_COLOR_ByNameD(f,cn,df) VGUI_DO_COLOR_ByNameF(f,0)
#   define VGUI_DO_COLOR_ByResD(f,rn,df)  VGUI_DO_COLOR_ByNameF(f,0)
#   include "vgui/vgui-colors.h"
#endif
}

/*======================================================================*/
/*= GC creation and freeing                                            =*/

static void main_gc_init_lineClr(Toutdev*od, Tod_GCdef gcd, Tcolor* clr)
{
    Tsty_line stl =  odsty_copyLine( od_style_topLine(od,gcd) );
    color_chgLazy(&stl.stl_color,clr);
    od_ldaf_setGCsoP_byStyle(od,gcd,&stl);
    odsty_freeLine(&stl);
}
static void main_gc_init_lineClrXor(Toutdev*od, Tod_GCdef gcd,
        Tcolor* clr1, Tcolor* clr2)
{
    Tcolor*   clr = color_newByXor(clr1, clr2);
    main_gc_init_lineClr(od,gcd,clr);
    color_delete(clr);
}
static void main_gc_init_lineThickness(Toutdev*od, Tod_GCdef gcd, int th)
{
    Tsty_line stl =  odsty_copyLine( od_style_topLine(od,gcd) );
    stl.stl_thickness = th;
    od_ldaf_setGCsoP_byStyle(od,gcd,&stl);
    odsty_freeLine(&stl);
}
static void main_gc_init(Tvgui* gui)
{
    // create the vg_odPxm
    TodResources* odres = odres_newWithBtm(gui->vg_black);
    odres_chgFontUsedEated( odres, font_defaultDefSet() );
    gui->vg_odPxm = vgui_odpixmap_newRes(gui,gui->vg_white,100,50,NULL,odres);
    gui->vg_od    = &gui->vg_odPxm->vv_od;
    odres_delete( odres, 0, 0); // just decrement refcnt
    gui_od_init(gui);

    // create the EGS GC corresponding to the GC of output devices
    // caution these lines must be copy/past to main_gc_free
#define VGUI_STYLE(gcd) odres_style_top(gui->vg_odResources,gcd)
    Tod_GCdef gcd;
    gcd=OD_GC_UsrDraw;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 0);
    gcd=OD_GC_UsrFill;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 0);
    gcd=OD_GC_MissDraw; gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 0);
    gcd=OD_GC_SelArea;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 1);
    //gcd=OD_GC_Squeleton;gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 1); same as OD_GC_SelArea
    gcd=OD_GC_Preview;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 1);
    gcd=OD_GC_Marker;   gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 1);
    gcd=OD_GC_MarkerHL; gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 1);
    gcd=OD_GC_SysDraw;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 0);
    gcd=OD_GC_SysFill;  gui_gc_create(gui, gcd, VGUI_STYLE(gcd), 0);

    // configure the GC (line draw color)
    Tcolor *clr0;
    if ( gui->g_unsprtXor ) {
        main_gc_init_lineClr(gui->vg_od,OD_GC_Squeleton,gui->vg_daSqueColor);
        main_gc_init_lineClr(gui->vg_od,OD_GC_Preview,gui->vg_daPrevColor);
        main_gc_init_lineClr(gui->vg_od,OD_GC_Marker,gui->vg_markerColor);
        main_gc_init_lineClr(gui->vg_od,OD_GC_MarkerHL,gui->vg_markerHlColor);
    }
    else {

        Tcolor *clr1;
        clr0 = color_newByXor(gui->vg_daBackground, gui->vg_markerColor);
        clr1 = color_newByXor(gui->vg_markerHlColorFill,clr0);
        //clr1 = color_newByXor(gui->vg_markerHlColorFill, gui->vg_markerColor);
        main_gc_init_lineClrXor(gui->vg_od,OD_GC_Squeleton, gui->vg_daBackground, gui->vg_daSqueColor);
        main_gc_init_lineClrXor(gui->vg_od,OD_GC_Preview,   gui->vg_daBackground, gui->vg_daPrevColor);
        main_gc_init_lineClrXor(gui->vg_od,OD_GC_Marker,    gui->vg_daBackground, gui->vg_markerColor);
        main_gc_init_lineClrXor(gui->vg_od,OD_GC_MarkerHL,  clr1                , gui->vg_markerHlColor);
        color_delete(clr0); color_delete(clr1);
    }

    // configure the GC (line thickness)
    main_gc_init_lineThickness(gui->vg_od,OD_GC_Marker,    2);
    main_gc_init_lineThickness(gui->vg_od,OD_GC_MarkerHL,  2);

    // configure the GC (fill style)
    clr0 = gui->g_unsprtXor ? color_newLazy(gui->vg_markerHlColorFill) :
                              color_newByXor(gui->vg_daBackground, gui->vg_markerHlColorFill);
    od_ldaf_setGCll(gui->vg_od,OD_GC_MarkerHL,0, DA_FS_Solid,   clr0                   ,0, 0,0);
    od_ldaf_setGCll(gui->vg_od,OD_GC_MissFill,0, DA_FS_LighTile,gui->vg_daMissColorFill,0,10,0);
    color_delete(clr0);

    // configure the GC (text style)
    Tsty_text* stt;
    od_str_setGC(gui->vg_od,OD_GC_MissDraw, 0, 14, gui->vg_black);
    stt = od_style_topText(gui->vg_od,OD_GC_Preview);
    clr0 = gui->g_unsprtXor ? color_newLazy(gui->vg_daPrevColor) :
                              color_newByXor(gui->vg_daBackground, gui->vg_daPrevColor);
    od_str_setGC(gui->vg_od,OD_GC_Preview ,stt->stt_font, stt->stt_pts, clr0);
    color_delete(clr0);
}

static void main_gc_free(Tvgui* gui)
{
    Tod_GCdef gcd;
    // free the EGS GC corresponding to the GC of output devices
    // caution these lines must be copy/past from main_gc_init
    gcd=OD_GC_UsrDraw;  gui_gc_free(gui, gcd);
    gcd=OD_GC_UsrFill;  gui_gc_free(gui, gcd);
    gcd=OD_GC_MissDraw; gui_gc_free(gui, gcd);
    gcd=OD_GC_MissFill; gui_gc_free(gui, gcd);
    gcd=OD_GC_SelArea;  gui_gc_free(gui, gcd);
    //gcd=OD_GC_Squeleton;gui_gc_free(gui, gcd);
    gcd=OD_GC_Preview;  gui_gc_free(gui, gcd);
    gcd=OD_GC_Marker;   gui_gc_free(gui, gcd);
    gcd=OD_GC_MarkerHL; gui_gc_free(gui, gcd);
    gcd=OD_GC_SysDraw;  gui_gc_free(gui, gcd);
    gcd=OD_GC_SysFill;  gui_gc_free(gui, gcd);

    vgui_odpixmap_delete(gui->vg_odPxm);
    gui_od_free(gui);
}

/*======================================================================*/

extern Tvgui* vgui_newPass1(TastrSet*argv,Tvgui_openParam*op)
{
    /* open the gui */
    Tvgui* gui = tool_malloc_0(sizeof(*gui));
    gui->g_argv_org = astrset_newClone(argv); astrset_nullTerminated(gui->g_argv_org);
    gui->g_argc     = gui->g_argv_org->eleNb;
    gui->g_argv     = tool_malloc_0((gui->g_argc+1)*sizeof(char*));
                      memcpy(gui->g_argv,gui->g_argv_org->eles,(gui->g_argc)*sizeof(char*));
    gui_open(gui,op);

    /* print resolution */
    vgui_inferr_printf(gui,"Display pix resolution: %dx%d",
            gui->g_pixScreenWidth,gui->g_pixScreenHeight);
    vgui_inferr_printf(gui,"Display ppi resolution: %fx%f",
            gui->g_ppiScreenWidth,gui->g_ppiScreenHeight);

    /* miscellaneous vgui initialisation */
    gui->g_msgdrv       = tool_malloc(sizeof(*gui->g_msgdrv));
   *gui->g_msgdrv       = gl_msgdrv_default;
    gui->vg_odVtableCvs = gui_od_getVtableCvs();
    gui->vg_odVtablePxm = gui_od_getVtablePxm();

    /* initialize g_ds to a dummy Tvgui_ds */
    gui->g_vmw = vgui_mw_newDummy(gui);

    /* retrieve ressources from EGS DB */
    vgui_res_initEGS(gui);

    return gui;
}

extern void vgui_newPass2(Tvgui*gui)
{
    // color initialization 
    main_colors_init(gui);  // colors for widget


    // VGUI font initialization 
    switch ( res_getDbl(&gui_appResources,
                VGUI_ResN_FontSize,&gui->g_devPtsStd) ) {
        case -1:
            gui->g_devPtsStd = 12.0;
            vgui_errerr_printf(gui,"%s resource is undefined (set to %f)", 
                VGUI_ResN_FontSize,gui->g_devPtsStd);
            break;
        case  1: {
            const char* str;
            gui->g_devPtsStd = 12.0;
            res_getStr(&gui_appResources, VGUI_ResN_FontSize, &str);
            vgui_errerr_printf(gui,
                "%s value is bad formated for %s resource (set to %f)",
                str,VGUI_ResN_FontSize,gui->g_devPtsStd);
            break;
         }
    }
    gui->g_devPtsSmall = gui->g_devPtsStd * (1-0.15);
    gui->g_devPtsBig   = gui->g_devPtsStd * (1+0.15);
    DBG0_Font("Font Size: %f %f %f",gui->g_devPtsSmall, gui->g_devPtsStd, gui->g_devPtsBig);
    gui_fonts_init(gui);

    // gc initialization 
    main_gc_init(gui);

    // others 
    gui_open_finish(gui);
}

extern void   vgui_free(Tvgui*gui)
{
    vgui_msgdrv_delConsole(gui);
    tool_free( gui->g_msgdrv ); gui->g_msgdrv = 0;
    gui_close_start(gui);
    main_gc_free(gui);
    gui_fonts_free(gui);
    main_colors_free(gui);
    vgui_res_freeEGS(gui);
    vgui_mw_delDummy( gui->g_vmw );
    free         ( gui->g_argv );
    astrset_delete( gui->g_argv_org );
    gui_close(gui);
    free( gui );
}
