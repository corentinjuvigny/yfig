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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: guiqt-init.cc 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-init.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This files groups the functions that initializes and frees         =*/
/*= the a Tvgui structure.                                             =*/
/*=                                                                    =*/
/*= See the "vgui/vgui.h" header for types and function documentation. =*/
/*======================================================================*/

#include "guiqt.h"

#include "vgui/vgui-resources.h"
#include "tools/strings.h"

#include <QDesktopWidget>

/*======================================================================*/

#if defined(USE_X11) && USE_X11==1
static int parseGeometry(const char*str, int*x,int*y,unsigned int*dx,unsigned int*dy)
{
    int status = sscanf(str,"%dx%d%d%d",dx,dy,x,y);
    return status;
}
#endif

#if 0 // FIXME:PRINTER
#include <QtPrintSupport>
static void gui_printer(Tvgui* gui)
{
//     	QPageSetupDialog sd; //((QWidget*)NULL);
//        sd.exec();
        QPrinter printer;
        QPrintDialog* pd = new QPrintDialog(&printer); //&printer,(QWidget*)NULL);
        printf("AAAAAAAAA:%d\n",pd->exec());
}
#endif

/*======================================================================*/
/*= The gui initialization.                                            =*/

extern void gui_open(Tvgui*gui, Tvgui_openParam*op)
{
    Tgui_main* main = new Tgui_main();
    gui->g_main = main;

    //Q_INIT_RESOURCE(qt_ex4);

    main->m_application = new QApplication(gui->g_argc, gui->g_argv);

#ifdef QT_VERS_4
    QDesktopWidget *desktop = main->m_application->desktop();
    gui->g_externalSystem = GUI_ExtSys_QT4;
    gui->g_pixScreenWidth  =  desktop->screenGeometry().width();
    gui->g_pixScreenHeight =  desktop->screenGeometry().height();
    gui->g_ppiScreenWidth  =  desktop->logicalDpiX();
    gui->g_ppiScreenHeight =  desktop->logicalDpiY();
#endif
#ifdef QT_VERS_5
    //QCoreApplication::processEvents();
    gui->g_externalSystem  = GUI_ExtSys_QT5;
    int cnt = QApplication::desktop()->screenCount();
    tool_inferr_printf(NULL,"count  Screen: %d ; primaryScreen: %d",
            cnt, QApplication::desktop()->primaryScreen());
    //rect = QApplication::desktop()->availableGeometry();
    //printf("availableGeometry: %dx%d\n", rect.width(), rect.height());
    int nbpixel=0;
    int screenNumSelected = -1;
    for ( int i=-1 ; i<cnt ; i+=1) {
        QRect rect;
        int   tmp;
        rect = QApplication::desktop()->screenGeometry(i);
        tool_inferr_printf(NULL,"screenGeometry %2d: %dx%d", i, rect.width(), rect.height());
        tool_inferr_printf(NULL," logical ppi   %2d: %fx%f", i,
            (double)QApplication::desktop()->screen(i)->logicalDpiX(),
            (double)QApplication::desktop()->screen(i)->logicalDpiY());
        tool_inferr_printf(NULL," physical ppi  %2d: %fx%f", i,
            (double)QApplication::desktop()->screen(i)->physicalDpiX(),
            (double)QApplication::desktop()->screen(i)->physicalDpiY());
        // we select the screen that has more pixels
        tmp=rect.width() * rect.height();
        if ( tmp>nbpixel ) {
            nbpixel = tmp;
            screenNumSelected = i;
        }
    }

    //QScreen* screen        = main->m_application->primaryScreen();
    QWidget* screen        = QApplication::desktop()->
                                screen(screenNumSelected);
    gui->g_pixScreenWidth  =  screen->geometry().width();
    gui->g_pixScreenHeight =  screen->geometry().height();
    //gui->g_ppiScreenWidth  =  screen->physicalDotsPerInchX ();
    //gui->g_ppiScreenHeight =  screen->physicalDotsPerInchY ();
    gui->g_ppiScreenWidth  =  screen->physicalDpiX ();
    gui->g_ppiScreenHeight =  screen->physicalDpiY ();

    tool_wrnerr_printf(NULL,"screen selected: numero     = %d",screenNumSelected);
    tool_wrnerr_printf(NULL,"screen selected: geometry   = %dx%d  pix",
             gui->g_pixScreenWidth, gui->g_pixScreenHeight);
    tool_wrnerr_printf(NULL,"screen selected: resolution = %fx%f  ppi",
             gui->g_ppiScreenWidth, gui->g_ppiScreenHeight);
#endif

// set geometry of top form
#if defined(TOS_MINGW) && TOS_MINGW
    unsigned int dx=0,dy=0;
    dx=gui->g_pixScreenWidth;  dx -= dx/4; if (dx>1000) dx=1000;
    dy=gui->g_pixScreenHeight; dy -= dy/4; if (dy>800)  dy=800;
    main->m_topForm = new WtopForm(dx,dy);
#else
    int x=0,y=0;
    unsigned int dx=0,dy=0;
    if ( op->op_geometry==0 ) {
        dx=gui->g_pixScreenWidth;  dx -= dx/4; if (dx>1000) dx=1000;
        dy=gui->g_pixScreenHeight; dy -= dy/4; if (dy>800)  dy=800;
        main->m_topForm = new WtopForm(dx,dy);
    } else {
        int status=parseGeometry((char*)op->op_geometry,&x,&y,&dx,&dy);
        if ( status==2 ) {
            main->m_topForm = new WtopForm(dx,dy);
        } else if ( status==4 ) {
            main->m_topForm = new WtopForm(x,y,dx,dy);
        } else {
            tool_ftlerr_printf(NULL,"bad formated geometry option: %s", op->op_geometry);
        } 
    }
#endif

#if 0 // FIXME:PRINTER
    gui->vg_printer = gui_printer;
#endif
#if defined(TOS_MINGW) && TOS_MINGW
    gui->g_unsprtEmbeddedFb = 1;
#elif defined(TOS_LINUX) && TOS_LINUX
    gui->g_unsprtEmbeddedFb = 0;
#else
#error TOS_... must be specified with ... in { LINUX MINGW }
#endif

    // init. of vtable: VGUI --> GUI
    extern struct _Twidm_guiVt guiqt_VtMenu; gui->vg_guiVtMenu = &guiqt_VtMenu;
}

extern void gui_close(Tvgui* gui)
{
    Tgui_main* main = gui->g_main;
    delete main->m_topForm;
    delete main->m_application;
}

/*=  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  =*/

extern void gui_open_finish(Tvgui* gui)
{
}

extern void gui_close_start(Tvgui* gui)
{
}

/*======================================================================*/
/*= font initialization                                                =*/

extern void   gui_fonts_init(Tvgui*gui)
{
    char   buffam[100];
    Cstr   resfont;
    double guipts;
    int    guidty;
    int    changed=0;

    // check if VGUI_ResN_GuiFont defines the GUI font
    if ( res_getStr(&gui_appResources,VGUI_ResN_GuiFont,&resfont)==-1 ) {
        // undefined: we keep the default QT font 
keep_default_font:
        QFont font   = E_Application->font();
        gui->g_mPts  = font.pointSizeF();
        gui->g_mFont = (Tegs_font) new QFont( font );
    } else {
        // decode the resfont string
        if ( tool_strtofont(buffam,&guipts,&guidty,resfont)<0 ) {
            tool_errerr_printf(NULL,"GUI font definition is bad formated (I switch to  default): %s",resfont);
            goto keep_default_font;
        }
        QFont font = E_Application->font();
#if 0
QString qstr = font.family();        char* fam = dup_QStr2CStr( qstr );
QString qstr2= font.defaultFamily(); char* fam2= dup_QStr2CStr( qstr2);
printf("fam=%s:%s sz=%d %.1f w=%d\n", fam, fam2, font.pixelSize(), font.pointSizeF(), font.weight());
#endif
        if ( buffam[0]!=0 ) { changed=1; font.setFamily(buffam);     }
        if (  guipts>5 )    { changed=1; font.setPointSizeF(guipts); }
        if (  guidty>0 )    { changed=1; font.setWeight(guidty);     }
        if ( changed ) {
            E_Application->setFont( font );
        }
        gui->g_mPts  = font.pointSizeF();
        gui->g_mFont = (Tegs_font) new QFont( font );
    }
}

extern void   gui_fonts_free(Tvgui*gui) { delete (QFont*)gui->g_mFont; }

/*======================================================================*/
/*= show, the main loop, breaking the main loop                        =*/

extern void gui_show(Tvgui*gui)
{
    Tgui_main* main = gui->g_main;

    QPalette pal = main->m_topForm->palette();
    pal.setColor(QPalette::Window, COLOR(gui->vg_background)); 
    main->m_topForm->setPalette(pal);

#ifdef QT_VERS_4
main->m_topForm->show();
#endif
#ifdef QT_VERS_5
    // with QT5 "main->m_topForm->show()" bugs randomly if
    // it is done before the main loop is started.
    // The next line delays "show()". Indeed the main loop will
    // runs it.
//    QTimer::singleShot( 0, main->m_topForm, SLOT(show()) );
main->m_topForm->show();
#endif
}

extern void gui_loop(Tvgui*gui)
{
    gui->g_main->m_topForm->show();
    gui->g_main->m_application->exec();
}

extern void gui_loop_break (Tvgui*gui)
{
    QApplication::exit(0);    
}

/*======================================================================*/

#include "outdev/style.h"

extern void gui_fonts_initGC(Tvgui* gui, Tod_GCdef gcd,
                int fi, double pts, Ccolor* fg, Ccolor* bg);

extern void gui_gc_create(Tvgui*gui,  Tod_GCdef gcd, Cstyle*sty, int _xor)
{
    TGUI_gc* gc   = E_GC(gcd);

    Csty_text* stt = &sty->sty_text;
    Csty_line* stl = &sty->sty_line;
    Csty_fill* stf = &sty->sty_fill;

    // line style
    gc->lineWidth = stl->stl_thickness<=0 ? 1 : stl->stl_thickness;
        gc->pen.setWidth(gc->lineWidth);
    gc->lineStyle =  conv_lineStyle_od2qt(stl->stl_style);
        gc->pen.setStyle(gc->lineStyle);
    gc->joinStyle = conv_joinStyle_od2qt(stl->stl_join);
        gc->pen.setJoinStyle(gc->joinStyle);
    gc->capStyle = conv_capStyle_od2qt(stl->stl_cap);
        gc->pen.setCapStyle(gc->capStyle);
    gc->drawColor = COLOR( stl->stl_color );
        gc->pen.setColor(gc->drawColor);

    // Fill style FIXME always set to DA_FS_Solid
    if ( stf->stf_color!=0 )
        gc->fillBrush.setColor( COLOR( stf->stf_color ) );
    else
        gc->fillBrush.setColor( COLOR( gui->vg_black ) );
    gc->fillStyle    =  _xor==0 ? QPainter::CompositionMode_Source
                                : QPainter::RasterOp_SourceXorDestination;

    // Text style
    gui_fonts_initGC(gui,gcd,stt->stt_font,stt->stt_pts,stt->stt_color,NULL);
}

extern void gui_gc_free(Tvgui*gui,  Tod_GCdef gcd)
{
    TGUI_gc* gc   = E_GC(gcd);
    if ( gc->t_fontinfo && FONTEGS_IsDefined(gc->t_fontinfo->fo_egs) )
            od_fontinfo_delete(gui->vg_od,gc->t_fontinfo);
}

/*======================================================================*/
