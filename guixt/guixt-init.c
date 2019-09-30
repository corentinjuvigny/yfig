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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-init.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-init.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This files groups the functions that initializes and frees         =*/
/*= the XT Tvgui.                                                      =*/
/*=                                                                    =*/
/*= See the "vgui/vgui.h" header for types and function documentation. =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt/guixt-event.h"

#include "tools/font.h"   // for DBG0/1_Font
#include "tools/strings.h"
#include "tools/lenunit.h"
#include "vgui/vgui-resources.h"

#include "outdev/style.h" // for gui_gc_create

/*======================================================================*/
/*= Redefinition of X11 error handler.                                 =*/
/*= This is done mainly for debuging purpose. Indeed, one that places  =*/
/*= a GDB break point in this handler, can known where in the source   =*/
/*= the X11 error occurs.                                              =*/

static int X_error_handler(Display *d, XErrorEvent *e)
{
        char msg[80];

        XGetErrorText(d, e->error_code, msg, sizeof(msg));

        fprintf(stderr, "YFIG: Error %d (%s): request %d.%d\n",
                        e->error_code, msg, e->request_code,
                        e->minor_code);
        sleep(20);
}

/*======================================================================*/
/*= This function is called indirectly by the WM when it destroys     =*/
/*= a windows.                                                         =*/

static void gui_quit(Tvgui* gui, Atom a)
{
    Tgui_main* main = gui->g_main;
    if ( a==main->m_wm_delete_window ) {
        printf("%s: Delete window asked\n",__func__);
        gui_loop_break(gui);
    } else if ( a==main->m_wm_save_yourself ) {
        printf("%s: Save yourself is asked (not implemented)\n",__func__);
        printf("%s: defult action is to exit\n",__func__);
        gui_loop_break(gui);
    } else {
        printf("%s: Unknowm WM message, (ignored)\n",__func__);
    }
}

/*======================================================================*/
/*= misc utilities                                                     =*/

static void gui_global_init(Tvgui*gui)
{
    Tgui_main* main  = gui->g_main;

    main->m_wm_delete_window =  XInternAtom(main->m_display, "WM_DELETE_WINDOW", False);
    main->m_wm_save_yourself =  XInternAtom(main->m_display, "WM_SAVE_YOURSELF", False);

    widget_cb_init(gui);
}

void process_pending(Tvgui* gui)
{
    Tgui_main* main = gui->g_main;
    while (XtAppPending(main->m_appContext))
        XtAppProcessEvent(main->m_appContext, XtIMAll);
    XSync(main->m_display, False);
}

/*======================================================================*/
/*= The gui initialization.                                            =*/

extern void gui_open(Tvgui*gui, Tvgui_openParam*op)
{
    //XtSetLanguageProc(NULL,NULL,NULL);
    XT_DeclareArgs(10);
    gui->g_main = malloc(sizeof(*gui->g_main));
    Tgui_main* main = gui->g_main;  // shortcut

    int           i, n;
    int           count;        /* number of matches (only 1?) */

    ArgCount=0;
    if (op->op_geometry) {
        XT_NextArg(XtNgeometry,   op->op_geometry );
    }
    // changed later in gui_show when the window sizes are computed.
    //XT_NextArg(XtNmappedWhenManaged, False);
    //XT_NextArg(XtNinitialResourcesPersistent, True);
    gui->g_main->m_topWidget = XtOpenApplication(
            &gui->g_main->m_appContext, "Yfig",
            //options, XtNumber (options),
            0, 0, // options
            &gui->g_argc, gui->g_argv,
            (String *) NULL,   // fall-back_resources
            applicationShellWidgetClass,
            Args, ArgCount               // args num_args
    );

    /* save X11 main settings */
    E_Display       = XtDisplay(gui->g_main->m_topWidget);
    E_Screen        = XtScreen (gui->g_main->m_topWidget);
    E_ScreenNum     = DefaultScreen(  main->m_display );
    E_Visual        = DefaultVisual(gui->g_main->m_display,gui->g_main->m_screenNum);
    E_ColorMap      = DefaultColormapOfScreen(gui->g_main->m_screen);
    E_Depth         = DefaultDepthOfScreen(gui->g_main->m_screen);

    ArgCount = 0;
    XT_NextArg (XtNvisual,   E_Visual);
    XT_NextArg (XtNcolormap, E_ColorMap);
    XT_NextArg (XtNdepth,    E_Depth);
    XT_SetValues(main->m_topWidget);

    /* store screen geometry and pixel density in the gui structure. */
    gui->g_pixScreenWidth  = WidthOfScreen(  main->m_screen );
    gui->g_pixScreenHeight = HeightOfScreen( main->m_screen);
#if 0
    gui->g_ppiScreenWidth  = 72.0;
    gui->g_ppiScreenHeight  = 72.0;
#else
    gui->g_ppiScreenWidth  = ((double)gui->g_pixScreenWidth) /
                             ((double)WidthMMOfScreen(main->m_screen))*25.4;
    gui->g_ppiScreenHeight = ((double)gui->g_pixScreenHeight) /
                             ((double)HeightMMOfScreen(main->m_screen))*25.4;
#endif

//    XtGetApplicationResources (gui->g_main->m_topWidget, gui->g_main, resources, XtNumber (resources), NULL, 0);

#if 0
XSynchronize(main->m_display,True);
XSetErrorHandler(X_error_handler);
#endif

    // others initialisation
    gui_global_init(gui);

    // init. of vtable: VGUI --> GUI
    extern struct _Twidm_guiVt guixt_VtMenu; gui->vg_guiVtMenu = &guixt_VtMenu;
}

extern void gui_open_finish(Tvgui* gui) { gui_pixmaps_init(gui); }

/*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/

extern void gui_close_start(Tvgui* gui) { gui_pixmaps_free(gui); }
extern void gui_close      (Tvgui* gui)
{
    while ( XtAppPending( gui->g_main->m_appContext ) ) {
        XEvent	    event;
        XtAppNextEvent(gui->g_main->m_appContext,&event);
	    XtDispatchEvent(&event);
    }
    XtDestroyApplicationContext(gui->g_main->m_appContext);
    free( gui->g_main );
}

/*======================================================================*/
/*= VGUI font initialization                                           =*/

typedef struct _Tguifont Tguifont;
typedef const   Tguifont Cguifont;
struct _Tguifont {
    char* family;
    char* fmt;
    char* dty_normal;
    char* dty_bold;
};

static void guifont_getName(Tvgui*gui, char*buf, Cguifont*gf, double pts, int dty)
{
    int pix =  round( LUNIT_PtToIn(pts)*VG_ppiScreen(gui) );
    sprintf(buf,gf->fmt,dty<70 ? gf->dty_normal : gf->dty_bold,pix);
}

static Font guifont_load(Tvgui*gui, Cguifont*gf, double pts, int dty)
{
    Font         font;
    XFontStruct* fontinfo;
    char         fontname[500]; guifont_getName(gui,fontname,gf,pts,dty);
    
    // XLoadFont does not really load the font, it just parses the name.
    // If the font does not exist, that generates errors when it is used.
    if ( (fontinfo=XLoadQueryFont(E_Display,fontname))==0 )
        return 0;
    XFreeFont(E_Display,fontinfo);

    // the job, the font exists
    return XLoadFont(E_Display,fontname);
}

Tguifont fonts[] = {
    { "avantgarde"        ,       "-*-avantgarde-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "demi" },
    { "courier"           ,          "-*-courier-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "courier 10 pitch"  , "-*-courier 10 pitch-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "fixed"             ,            "-*-fixed-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "helvetica"         ,        "-*-helvetica-%s-r-normal-*-%d-*-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "lucida"            ,           "-*-lucida-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "palatino"          ,           "-*-lucida-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "terminal"          ,         "-*-terminal-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "times"             ,            "-*-times-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "medium" , "bold" },
    { "urw bookman l"     ,    "-*-urw bookman l-%s-r-normal-*-%d-0-*-*-*-*-ISO8859-*" , "light"  , "bold" },
    { 0                   , 0                                                       , 0        , 0      }
};

static Font gui_fonts_init_search(Tvgui*gui, CstrBrief fam, double pts, int dty)
{
    int i;
    Font font=0;

    // search the entry in fonts
    for (i=0 ; fonts[i].family!=0 ; i+=1) {
        if ( strcasecmp(fam,fonts[i].family)==0 )
            break;
    }
    if ( fonts[i].family==0 ) {
        tool_errerr_printf(NULL,"unsupported font family: %s",fam);
        return 0;
    }

    // try to load it
    if ( (font=guifont_load(gui,fonts+i,pts,dty))==0 ) {
        char   fontname[500];
        guifont_getName(gui,fontname,fonts+i,pts,dty);
        tool_errerr_printf(NULL,"can not load the font: %s",fontname);
        return 0;
    }
    return font;
}

#if 0 // for debug
static Font gui_font_init_getDefaultFontInResDB(Tvgui*gui)
{
    Font font=0;
	XrmName xrm_name[2];
	XrmClass xrm_class[2];
	XrmRepresentation rep_type;
	XrmValue value;
	xrm_name[0] = XrmPermStringToQuark ("xtDefaultFont");
	xrm_name[1] = 0;
   	xrm_class[0] = XrmPermStringToQuark ("XtDefaultFont");
	xrm_class[1] = 0;
	if (XrmQGetResource(XtDatabase(E_Display), xrm_name, xrm_class, &rep_type, &value)) {
        XrmQuark   XtQFont       = XrmPermStringToQuark(XtRFont);
        XrmQuark   XtQString     = XrmPermStringToQuark(XtRString);
        XrmQuark   XtQFontStruct = XrmPermStringToQuark(XtRFontStruct);
	    if (rep_type == XtQString) {
		    font = XLoadFont(E_Display, (char *)value.addr);
	    } else if (rep_type == XtQFont) {
		    font = *(Font*)value.addr;
	    } else if (rep_type == XtQFontStruct) {
		    font = ((XFontStruct*)value.addr)->fid;
	    }
	}
printf("-------------Font=%p\n",(void*)font);
    return font;
}
#endif

extern void   gui_fonts_init(Tvgui*gui)
{
#define DEFAULT_FAMILY  "fixed" // "times"
#define DEFAULT_PTS     9       // 11
#define DEFAULT_DENSITY 50      // 75
    int    i;
    Font   font;

    char   buffam[100];
    Cstr   resfont;
    Cstr   guifam;
    double guipts;
    int    guidty;

    // check if VGUI_ResN_GuiFont defines the GUI font
    if ( res_getStr(&gui_appResources,VGUI_ResN_GuiFont,&resfont)==-1 ) {
        // undefined
        goto try_default_font;
    } else {
        // decode the resfont string
        if ( tool_strtofont(buffam,&guipts,&guidty,resfont)<0 ) {
            tool_errerr_printf(NULL,"GUI font definition is bad formated (I switch to  default): %s",resfont);
            goto try_default_font;
        }
        guifam = buffam[0]==0 ? DEFAULT_FAMILY : buffam;
        if ( guipts<=5 ) guipts = DEFAULT_PTS;
        if ( guidty< 0 ) guidty = DEFAULT_DENSITY;

        // try to load the font
        if ( (font=gui_fonts_init_search(gui,guifam,guipts,guidty))==0 )
            goto try_default_font;
        goto found;
    }

    // Try the default font
try_default_font:
    guifam = DEFAULT_FAMILY;
    guipts = DEFAULT_PTS;
    guidty = DEFAULT_DENSITY;
    if ( (font=gui_fonts_init_search(gui,guifam,guipts,guidty))==0 )
        goto try_all_fonts;
    goto found;

    // Try the fonts of the fonts table keeping default pts & dty.
    // Stop at the first that is loaded successfully.
try_all_fonts:
    guipts = DEFAULT_PTS;
    guidty = DEFAULT_DENSITY;
    for (i=0 ; fonts[i].family!=0 ; i+=1) {
        if ( (font=guifont_load(gui,fonts+i,guipts,guidty))!=0 )
            goto found;
    }
    tool_ftlerr_printf(NULL,"%s","no font found for the GUI.");
    exit(1);

found:
    gui->g_mPts  = guipts;
    gui->g_mFont = (Tegs_font) font;


    // set font as default XT font
    XrmValue any; any.size=sizeof(XPointer); any.addr=(void*)&gui->g_mFont;
//printf("-------------F=%p any=(%d%p)\n",gui->g_mFont,any.size,any.addr);
    XrmDatabase db = XtDatabase(E_Display);

    XrmQuark   XtQFont = XrmPermStringToQuark(XtRFont);
    XrmBinding xrm_bind[2];
    XrmQuark   xrm_quark[3];
    xrm_bind[0]  = XrmBindLoosely;
    xrm_bind[0]  = 0;
    xrm_quark[0] =  XrmPermStringToQuark ("xtDefaultFont"); 
  //xrm_quark[1] =  XrmPermStringToQuark ("XtDefaultFont"); 
    xrm_quark[1] =  0;
//gui_font_init_getDefaultFontInResDB(gui);
    XrmQPutResource(&db,xrm_bind,xrm_quark,XtQFont,&any);
//gui_font_init_getDefaultFontInResDB(gui);
}

// gui->g_mFont is deleted by XtDestroyApplicationContext when it
// deletes the resourece set.
// --->  no XUnloadFont(E_Display,(Font)gui->g_mFont ) here
extern void gui_fonts_free(Tvgui*gui) { }

/*======================================================================*/
/*= show, the main loop, breaking the main loop                        =*/

extern void gui_show(Tvgui*gui)
{
    Tgui_main* main = gui->g_main;
    Widget      children[100];
    int         childNb=0;

    XtRealizeWidget(gui->g_main->m_topWidget);
    XtMapWidget(gui->g_main->m_topWidget);
    // catch VM cancel and redirect them to main_exit routine
    XSetWMProtocols(main->m_display, XtWindow(main->m_topWidget), &main->m_wm_delete_window, 1);
    widget_cb_addU(main->m_topWidget,gui,YFIG_EVENT_CancelSaveDots, gui_quit,  NULL);

#if 0
    Widget console = (Widget)vgui_dw_getWidEGS( (Tvgui_dw*)gui->g_console );
    XSetWMProtocols(main->m_display, XtWindow(console), &main->m_wm_delete_window, 1);
    widget_cb_addU(console, gui,YFIG_EVENT_CancelSaveDots, gui_quit,  NULL);
#endif

    XWMHints* wmhints;
    wmhints = XGetWMHints(main->m_display, XtWindow(main->m_topWidget));
    wmhints->flags |= InputHint;/* add in input hint */
    wmhints->input = True;
    XSetWMHints(main->m_display, XtWindow(main->m_topWidget), wmhints);
    XFree((char *) wmhints);
    //XtInstallAllAccelerators(gui->vg_modePannel->vgbpm_widgetSelf, main->m_topWidget);
    //XtInstallAllAccelerators(gui->vg_canvas->vgcm_widgetSelf, main->m_topWidget);
    //XtInstallAllAccelerators(main->m_topWidget,gui->vg_modePannel->vgbpm_widgetSelf);
    //XtInstallAllAccelerators( gui->vg_canvas->vgcm_widgetSelf, gui->vg_modePannel->vgbpm_widgetSelf);
    //XtAugmentTranslations(main->m_topWidget, main->modePanelTable);
// FIXME:IVG    XtAugmentTranslations(vgui_buttonPanel_widget(gui->vg_modePannel), main->modePanelTable);
// FIXME:IVG    XtAugmentTranslations(gui->vg_canvas->vgcm_widgetSelf,      main->modePanelTable); 
}

static int xt_main_loop_break;

extern void gui_loop(Tvgui*gui)
{
    xt_main_loop_break=0;
    while ( xt_main_loop_break==0 )
	    XtAppProcessEvent(gui->g_main->m_appContext, XtIMAll);
}

extern void gui_loop_break(Tvgui*gui) { xt_main_loop_break = 1; }

/*======================================================================*/

#define EGS_COLOR(c) ( (c)!=0 ? (c)->egs : EGSCOLOR_none )

extern void gui_gc_create(Tvgui*gui,  Tod_GCdef gcd, Cstyle*sty, int xor)
{
    GC             ngc;
    XGCValues      gcv;
    unsigned long  gcmask=0;

    Csty_text* stt = &sty->sty_text;
    Csty_line* stl = &sty->sty_line;
    Csty_fill* stf = &sty->sty_fill;

    gcmask |= GCFont       ; gcv.font       = (Font)gui->g_mFont;
    gcmask |= GCLineWidth  ; gcv.line_width = stl->stl_thickness<=0
                                            ? 1
                                            : stl->stl_thickness;
    gcmask |= GCLineStyle  ; gcv.line_style = stl->stl_style==DA_LS_None 
                                            ? LineSolid
                                            : conv_lineStyle_Od2X11(stl->stl_style);
    gcmask |= GCJoinStyle  ; gcv.join_style = stl->stl_join==DA_JS_None
                                            ? JoinMiter
                                            : conv_joinStyle_Od2X11(stl->stl_join);
    gcmask |= GCCapStyle   ; gcv.cap_style  = stl->stl_cap==DA_CS_None
                                            ? CapRound
                                            : conv_capStyle_Od2X11(stl->stl_cap);
    gcmask |= GCForeground ; gcv.foreground = CLRegs2xt(EGS_COLOR(stl->stl_color));
    gcmask |= GCFillStyle  ; gcv.fill_style = FillSolid ; // FIXME always set to DA_FS_Solid
    if ( stf->stf_color!=0 ) {
        gcmask |= GCBackground ; gcv.background = CLRegs2xt(EGS_COLOR(stf->stf_color));
    } else {
        gcmask |= GCBackground ; gcv.background = CLRegs2xt(EGS_COLOR(gui->vg_black));
    }
    gcmask |= GCFunction   ; gcv.function   = xor==0 ? GXcopy : GXxor;
    gcmask |= GCFillRule   ; gcv.fill_rule  = EvenOddRule;
    ngc = XCreateGC(E_Display, RootWindow(E_Display,E_ScreenNum), gcmask, &gcv);

    E_GCcache(gcd) = ngc;

    gui_gc_create_font(gui,gcd,stt->stt_font,stt->stt_pts,stt->stt_color,NULL);
}

extern void gui_gc_free(Tvgui*gui,  Tod_GCdef gcd)
{
    gui_gc_free_font(gui,gcd);
    XFreeGC(E_Display,E_GCcache(gcd));
}

/*======================================================================*/
