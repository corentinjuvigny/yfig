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
 *$Software: YFIG2DEV
 *      $Id: main.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/main.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The YFIG2DEV main function.                                        =*/
/*======================================================================*/

#include "yfig2dev-config.h"

#include "tools/globalvars.h"
#include "tools/strings.h"
#include "tools/messagedriver.h"
#include "tools/imgfmt.h"
#include "tools/file.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"

#if (defined(TOOL_DBG_MemClr) && TOOL_DBG_MemClr) ||\
    (defined(TOOL_DBG_MemFnt) && TOOL_DBG_MemFnt) ||\
    (defined(TOOL_DBG_MemObj) && TOOL_DBG_MemObj)
#define DBG_MEM_ALLOCATION_ON
#include "tools/font-types.h"
#endif

static void main_pdf_to_target(Tds*ds);
static void main_cleanup(Tds*ds);

/*======================================================================*/

extern int main(int argc, char*argv[])
{
    Tds ds; bzero(&ds,sizeof(ds));
    char tmp[1000];
    char *str;
    int i,nbe;

    /* initializes the tools global variables */
    tool_glvars_init( (const char * const *)argv, NULL);

    Tcmdline* cmdline = cmdline_newSwSet(&userParams, sizeof(Tsw), sizeof(userParams)/sizeof(Tsw), "", "infile", "infile outfile");
    char*help_header[]={
        "The %s command converts the figure of the \"infile\" file under fig format to ",
        "standard graphic image formats like PNG, PDF that it writes into the \"outfile\"",
        "file. When the %s command is invoked with the second format (a single file), it",
        "reads the given fig file and writes the conversion into the standard output",
        "stream.  In the third format, it reads the figure description from the standard",
        "input stream and writes the conversion into the standard output stream.",
        NULL,
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

    // check for main arguments: input file, output file and output file format
    if      ( argv_notrecognized->eleNb <= 1 ) ds = ds_init0();
    else if ( argv_notrecognized->eleNb == 2 ) ds = ds_init1(argv_notrecognized->eles[1]);
    else if ( argv_notrecognized->eleNb == 3 ) ds = ds_init2(argv_notrecognized->eles[1],argv_notrecognized->eles[2]);
    else tool_ftlerr_printf(NULL,"too many arguments: %s %s %s ...",argv_notrecognized->eles[1],argv_notrecognized->eles[2],argv_notrecognized->eles[3]);

    // load the figure and build the scene
    Tfigure* fig;
    void*    colors      = fig_colors_newSharedColor(NULL);
    if ( ds.instream==0 ) {
        fig = fig_new_byFile  (ds.wMsgdrv,ds.inname,colors);
    } else {
        fig = fig_new_byStream(ds.wMsgdrv,ds.inname,colors,ds.instream);
        fclose( ds.instream );
    }
    if ( fig==0 ) tool_ftlerr_printf(NULL,"loading \"%s\" figure failed.",ds.inname);
    fig_update_forDrawing(fig,NULL);
    ds.scene = fig_scene_newByFig(fig);
    if ( ds.scene==0 ) tool_ftlerr_printf(NULL,"loading \"%s\" figure failed.",ds.inname);
    tool_inferr_printf(NULL,"\"%s\" figure is loaded.",ds.inname);
    DBG0_printf("yfig2dev","input=%s %p ; output=%s %p %s ; fig=%p\n", ds.inname, ds.instream, ds.outname, ds.outstream, ds.outformat,fig);
#if 0 // for fig2dev comparison
    fig_scene_zoom(ds.scene,1.05);
#endif
    
    // update depths of scene (-D option) if needed
    int depths[FIG_MAX_DEPTH];
    fig_scene_enableAllDepths(ds.scene);
    if ( userParam_getDepth(depths,TOOL_TABLE_NbEle(depths)) ) {
        int i;
        for (i=0 ; i<TOOL_TABLE_NbEle(depths) ; i+=1 ) {
            if ( depths[i]==0 )
            fig_scene_disableDepth(ds.scene,i);
        }
    }

    // generate the TEX, SVG or PDF for other formats
    ds_updateOption(&ds);
    if ( ds.opt_imgfmt->kind==IMGFMT_TEX ) {
        generate_tex(&ds);
    } else if ( ds.opt_imgfmt->kind==IMGFMT_SVG ) {
        generate_svg(&ds);
    } else {
        generate_pdf(&ds);
        tool_inferr_printf(NULL,"\"%s\" figure written into PDF file: %s",ds.inname, ds.pdfname);
        // convert PDF to target
        main_pdf_to_target(&ds);
    }

    // clean up
    TEMP_spw_printf("%s","Start cleanup");
    main_cleanup(&ds);
    cmdline_delete( cmdline );
    colorset_free( colors );
    ds_free( &ds );
    astrset_delete( argv_notrecognized );
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

/*======================================================================*/

static void main_cleanup(Tds*ds)
{
    if ( ds->pdfname ) {
        unlink(ds->pdfname);
        tool_strfreeS0(&ds->pdfname);
    }
}

/* convert the PDF temporary (ds->pdfname) file into the expected format */
static void main_pdf_to_target(Tds*ds)
{
    TimgFmtKd outFmt =  ds->opt_imgfmt->kind;
    if ( outFmt==IMGFMT_PDF ) {
        if ( ds->outstream!=0 ) {
            // probably stdout
            if ( tool_fileCopyStream(&gl_msgdrv_silence,ds->pdfname,ds->outstream)!=0 )
                tool_ftlerr_printf(NULL, "failed to write %s file: %s", ds->outname,strerror(errno));
        } else {
            // just rename file
            if ( tool_fileMove(&gl_msgdrv_silence,ds->pdfname,ds->outname)!=0 )
                tool_ftlerr_printf(NULL, "failed to create %s file: %s", ds->outname,strerror(errno));
        }
    } else {
        Tstr       outname = ds->outstream==0 ? ds->outname : "-";
        CstrStatic pdf     = tool_imgfmt_getShNameByKd( IMGFMT_PDF );
        CstrStatic fmt     = tool_imgfmt_getShNameByKd( outFmt );
        double     zoom    = ds_getZoomFactor(ds);
        double     ppi     = 72; // 80 for fig2dev comparison;
        double     newppi  = ppi*zoom;
        double     quality = 0; // default
        // check ppi
        if ( newppi<0.1 ) {
            tool_errerr_printf(NULL,"too small %.2f PPI set to 0.1.",zoom*ppi);
            newppi=0.1;
        } else {
            TboxDble box = ds_getSrcBBCm(ds);
            double bdCm = userParam_getBorderThicknessCm();
            double dxIn = LUNIT_CmToIn(Box_dx(box)+2*bdCm);
            double dyIn = LUNIT_CmToIn(Box_dy(box)+2*bdCm);
            double pixels = dxIn*newppi*dyIn*newppi;
            double pixels_max = 1e7;
            if ( pixels > pixels_max ) {
                tool_wrnerr_printf(NULL,"zoom=%.2f + ppi=%.2f --> %e pixels.",zoom,ppi,pixels);
                newppi = sqrt(pixels_max/(dxIn*dyIn));
                tool_errerr_printf(NULL,"changed zoom from %.2f to %.2f to get %e pixels.",zoom,newppi/ppi,dxIn*newppi*dyIn*newppi);
            }
            ppi = newppi;
        }
        // Do the job
        if ( tool_imgfmt_convIM(NULL,//&gl_msgdrv_silence,
                ds->pdfname,IMGFMT_PDF, outname,outFmt, ppi,quality)!=0 )
            tool_ftlerr_printf(NULL, "failed to convert %s format into %s format", pdf,fmt);
    }
}

/*======================================================================*/
