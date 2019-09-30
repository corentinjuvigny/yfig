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
 *$Software: YFIG
 *      $Id: yfig-main.c 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-main.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The YFIG main function: YFIG_main(int,char**)                      =*/
/*======================================================================*/

#include "yfig-config.h"
#include "yfig-mw.h"
#include "yfig-dsds.h"

#include "vgui/vgui.h"
#include "vgui/vgui-resources.h"
#include "tools/globalvars.h"
#include "tools/strings.h"
#include "tools/messagedriver.h"
#include "tools/file.h"
#include "tools/cmdline.h"

/*======================================================================*/

struct _Tsw {
    SW_FIELD();
    char*  resname;   // nom de la ressource
};

typedef struct _TuserParam {
    Tsw help;
    Tsw dbglevel;
    Tsw verbosity;
    Tsw display;
    Tsw geometry;
    Tsw iconsize;
    Tsw guifont;
} TuserParam;

#include "yfig-main-argv-parser.c"

#if (defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr) ||\
    (defined(TOOL_DBG_MemFnt) && TOOL_DBG_MemFnt) ||\
    (defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj)
#define DBG_MEM_ALLOCATION_ON
#include "tools/font-types.h"
#endif

/*======================================================================*/
//#include <signal.h>

#if GSRLIMIT_AVAILABLE && defined(YFIG_MemLimitMb)

#if defined(HAVE_SYS_TYPES_H) && HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_RESOURCE_H) && HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

static void main_setMemLimit()
{
    //int res=RLIMIT_DATA;
    int res=RLIMIT_AS;
    struct rlimit old,new;
    int    expected_mo = 1*1024;//YFIG_MemLimitMb;   // in Mb
    rlim_t expected = (1L<<20)*expected_mo; // in bytes

    if ( getrlimit(res,&old)<0 ) {
        tool_errerr_printf(NULL,"can not get default memory limit: %s",strerror(errno));
        return;
    }
    new.rlim_cur = expected;
    new.rlim_max = old.rlim_max;
    if ( old.rlim_cur == RLIM_INFINITY ) {
        if ( setrlimit(res,&new)<0 ) {
            tool_errerr_printf(NULL,"failed to change memory limit from infinity to %d mo: %s",
                expected_mo,strerror(errno));
        } else
            tool_wrnerr_printf(NULL,"memory limit changed from infinity to %d mo.",expected_mo);
    } else {
        int curr_mo = old.rlim_cur/(1<<20);
        if ( expected<old.rlim_cur ) {
          if ( setrlimit(res,&new)<0 ) {
            tool_errerr_printf(NULL,"failed to change memory limit from %d to %d mo: %s",
                curr_mo,expected_mo,strerror(errno));
          } else
            tool_wrnerr_printf(NULL,"memory limit changed from %d to %d mo.",curr_mo,expected_mo);
        } else
            tool_wrnerr_printf(NULL,"memory limit is %d mo.",curr_mo);
    }
}
#endif

int YFIG_main(int argc, char*argv[])
{
    char path[PATH_MAX];
    char *str;
    int i,nbe;

    //signal(SIGPIPE,SIG_IGN);

    /* initializes the tools global variables */
    tool_glvars_init( (const char * const *)argv, "." PACKAGE );

    /* pares the command line */
    Tcmdline* cmdline = cmdline_newSwSet(&userParams, sizeof(Tsw), sizeof(userParams)/sizeof(Tsw), "", "infile", NULL);
    char*help_header[]={
        "The %s command starts a GUI that allows to draw pictures in a vectoriel",
        "mode to save it into vaious image and document formats.",
        "When the %s command is invoked with the first format (no file), the GUI",
        "starts with an empty figure.",
        "When the %s command is invoked with the second format, it reads the given",
        "figure file and starts the GUI with this figure. The input file must be",
        "a fig file.",
        NULL
    };
    cmdline->cl_hhelp = (void*)help_header;
    TastrSet*  argv_notrecognized; // argv[0] + not recognized arguments
    argv_notrecognized = cmdline_parse(cmdline,argv);
    cmdline_finish(cmdline,argv_notrecognized,"+-",0);

    // set verbosity and debug level
    gl_msgdrv_default.errlvl = userParams.verbosity.value.sint;
    tool_dbg_level           = userParams.dbglevel .value.sint;

    // print tool library status
    tool_glvars_dump();
#if GSRLIMIT_AVAILABLE && defined(YFIG_MemLimitMb)
    main_setMemLimit();
#endif

    // Create the necessary files/directories
    tool_fileMkdirR( &gl_msgdrv_default, gl_tempDir  );
    tool_fileMkdirR( &gl_msgdrv_default, gl_appRcDir );

    /* opens the gui, and initializes it.
     * vgui_newPass1 suppresses from argv_notrecognized the arguments
     * that it recognizes. */
#if TOS_MINGW == 1
    PERM_spe_printf("%s","MINWG not yet supported.");
    Tvgui* gui = vgui_newPass1(argv_notrecognized,NULL);
#elif defined(USE_X11) && USE_X11==1
    Tvgui_openParam op;
    op.op_display  = cmdline_getStr(&userParams.display);
    op.op_geometry = cmdline_getStr(&userParams.geometry);
    Tvgui* gui;
    gui = vgui_newPass1(argv_notrecognized,&op);
#else
    tool_spe_printf("%s","unknown TOS, setting EGS params to NULL ( vgui_newPass1(argv,NULL) )");
    Tvgui* gui;
    gui = vgui_newPass1(argv_notrecognized,NULL);
#endif

    /* export the options to the system and finish gui initialisation */
    main_argv_setDefault (cmdline);
    main_argv_setResource(cmdline);
    vgui_newPass2(gui);

    /* the Fig filename is the only option now */
    int    argc_bis = argv_notrecognized->eleNb;
    char** argv_bis = argv_notrecognized->eles;
    char *arg_filename=NULL; // the file name to draw;
	if (argc_bis > 2) {
	    fprintf(stderr, "%s: too many parameters: %s %s %s %s\n",
            gl_progname, argv_bis[1], argv_bis[2],
            argc_bis>3 ? argv_bis[2] : "",
            argc_bis>4 ? "..." : ""
        );
	    exit(1);
	}
    if (argc_bis == 2) {
	  arg_filename = strdup(argv_bis[1]);
    }

    vgui_msgdrv_addConsoleEL(gui, gl_msgdrv_default.errlvl<2 ? 2 : gl_msgdrv_default.errlvl );

    /* initialization of shared structure data */
    gui->g_ud = yfig_dsds_new(gui);

    // create the main widget
    Tyfig_mw* ymw = yfig_mw_nbf(gui);
    gui_show(gui);

    if ( arg_filename ) {
        yfig_cursce_newByPath(ymw,arg_filename);
    } else {
        yfig_cursce_new(ymw);
        yfig_cursce_redraw(ymw);
        yfig_dpp_showonly(ymw,DPP_SHOW_Zoom);
    }

    // start the GUI
quit_canceled:
    gui_loop(gui);

    // clean up
    TEMP_spw_printf("%s","Start cleanup");
      if ( yfig_mw_delete( ymw )==0 ) goto quit_canceled;
      astrset_delete(argv_notrecognized);
      cmdline_delete(cmdline);
      yfig_dsds_free( gui->g_ud );
      vgui_free(gui);
      tool_glvars_free();
#ifdef DBG_MEM_ALLOCATION_ON
    // some checks
    TEMP_spw_printf("%s","Check for unfreed colors");
    dbg_color_printUnfreed();
    TEMP_spw_printf("%s","Check for unfreed objects");
    dbg_obj_printUnfreed();
    TEMP_spw_printf("%s","Check for unfreed fontinfo");
    dbg_fontinfo_printUnfreed();
#endif
    TEMP_spw_printf("%s","Leave");
    return 0;
}
