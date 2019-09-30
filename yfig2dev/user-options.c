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
 *      $Id: user-options.c 340 2018-11-06 18:02:30Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/user-options.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Definition of the user options.                                   =*/
/*======================================================================*/

#include "yfig2dev-config.h"
#include "tools/imgfmt.h"
#include "tools/lenunit.h"

/*======================================================================*/
/*= The option getters                                                 =*/

extern char* userParam_getOutFormat(int mandatory)
{
    char tmp[100];
    if ( mandatory && userParams.outformat.set==0 )
        tool_ftlerr_printf(NULL,"can not detect the output format. Provide it with the %s option.",cmdline_switch2str(tmp,&userParams.outformat,'"'));
    if ( userParams.outformat.set==0 )
        return 0;
    Tstr       usrfmt = cmdline_getStr(&userParams.outformat);
    CimgFmtDt* imgfmt = tool_imgfmt_getByExt(usrfmt);
    if ( imgfmt==0 ) 
        tool_ftlerr_printf(NULL,"%s image format is not recognized.",usrfmt);
    return usrfmt;
}

extern double userParam_getZoomFactor(double dx, double dy)
{
    char tmp1[100],tmp2[100];
    double md,scale;
    Tsw* swscal = &userParams.magnification;
    Tsw* swmaxd = &userParams.maxdimension;

    if ( cmdline_isSwSet(swscal) && cmdline_isSwSet(swmaxd) )
        tool_ftlerr_printf(NULL,"%s and %s switches are exclusive",cmdline_switch2str(tmp1,swscal,'"'),cmdline_switch2str(tmp2,swmaxd,'"'));
    if (  cmdline_isSwSet(swscal) ) {
        scale = cmdline_getDle(swscal);
        return scale;
    }
    if (  cmdline_isSwSet(swmaxd) ) {
        // scale*dx < md
        // scale*dy < md
        md = cmdline_getDle(swmaxd);
        double scaledx = md/dx;
        double scaledy = md/dy;
        scale = min2(scaledx,scaledy);
        TEMP_asf_printf( (scale*dx)>md || (scale*dy)>md, "BUG: (scale*dx>md)=%d (scale*dy>md)=%d ; 0 0 is expected.",(scale*dx)>md,(scale*dy)>md);
        return scale;
    }
    return 1.0;
}

extern double userParam_getBorderThicknessCm()
{
    Tsw* sw = &userParams.borderwidth;
    if ( ! cmdline_isSwSet(sw) )
        return 0;
    double border = cmdline_getDle(sw);

    if ( border<0 ) {
        tool_errerr_printf(NULL,"negative border thickness (%e pt) is invalid (changed to 0).",border);
        border=0;
    } else if ( border>LUNIT_CmToPt(5) ) {
        tool_wrnerr_printf(NULL,"%e pt (%e cm) seems very large for border thickness.",border,LUNIT_PtToCm(border));
    }
        return LUNIT_PtToCm( border );
}

extern Cpaperdef* userParam_getPaperDef()
{
    char    tmp[100];
    Tsw* sw = &userParams.paperdefinition;

    if ( ! cmdline_isSwSet(sw) ) return NULL;

    char* paper = cmdline_getStr(sw);
    if ( TOOL_strIsEmpty(paper) ) 
        tool_ftlerr_printf(NULL,"parameter %s of %s switch is badly formated.", sw->arswitch, cmdline_switch2str(tmp,sw,'"'));

    Cpaperdef* paperdef = tool_paperdef_get(paper);
    if ( paperdef==0 ) {
        sw->arswitch = paper;
        tool_ftlerr_printf(NULL,"\"%s\" paper found in %s switch is not supported",paper,cmdline_switch2str(tmp,sw,'"'));
    }

    return paperdef;
}

extern int userParam_getDepth(int*depths, int depthn)
{
    Tsw* sw = &userParams.depths;
 
    if ( ! cmdline_isSwSet(sw) )
        return 0;

    char* inputorg = cmdline_getStr(sw);
    char* inputcur = inputorg;
    yyparse(depths,depthn,&inputcur);
#if 0 // debug
{
    int i,j;
    char t[100*5+100];
    for (i=0;i<depthn; ) {
        char* p = t + sprintf(t,"depth a 0:");
        for (j=0;i<depthn && j<100;i+=1 ) {
            if ( depths[i]==0 ) {
                p += sprintf(p," %d",i);
                j+=1;
            }
        }
        DBG2_printf("yfig2devdepths","%s",t);
    }
}
#endif

    return 1;
}

extern int userParam_getRowColZTF(int*nrow, int*ncol)
{
    Tsw* sw = &userParams.multipageZTF;

    if ( ! cmdline_isSwSet(sw) )
        return 0;

    Tstr arg = cmdline_getStr(sw);
    if ( arg==0 ) {
        // User did not set the optional argument 
        *nrow = 1; *ncol = 1;
    } else {
        if ( sscanf(arg,"%dx%d",nrow,ncol)!=2 )
            tool_ftlerr_printf(NULL,"argument of %s switch is bad formatted.",cmdline_aswitch2strDQ(sw));
        if ( *nrow<1 || *ncol<1 )
            tool_ftlerr_printf(NULL,"invalid value in argument of %s switch (expected >=1).",cmdline_aswitch2strDQ(sw));
        if ( 100<*nrow || 100<*ncol || *nrow**ncol>25 )
            tool_ftlerr_printf(NULL,"%s switch defines too many pages (expected <25).",cmdline_aswitch2strDQ(sw));
    }
    return 1;
}

/*======================================================================*/
/*= The option table                                                   =*/

#define SW(sw) &userParams.sw

TuserParam userParams = {
  .help={
    .optkind=SWOK_Help,
    .shswitch="-h", .lgswitch="--help",
    .doc={ "print this help into the standard output stream"},
  },
  .verbosity={
    .optkind=SWOK_IncDec,
    .shswitch="-v", .lgswitch="--verbosity-level", .argfmt=SWAF_Int,
    .value={ .sint=2 }, .valmin={.sint=0}, .valmax={.sint=3},
    .doc={ "set the verbosity level to n (default: 1) with",
           "0=error, 1=warning, 2=information", }
  },
  .dbglevel={
    .optkind=SWOK_IncDec,
    .shswitch="-G", .lgswitch="--debug-level", .argfmt=SWAF_Int,
    .value={ .sint=0 }, .valmin={.sint=0}, .valmax={.sint=4},
    .doc={ "set the debug level to n (default: -1)", }
  },
  .outformat={
    .shswitch="-L", .lgswitch="--out-format", .arswitch="fmt",
    .argfmt=SWAF_Str,
    .doc={ "Choose/force the output format when it can not be detected." },
    .man= "The output format is detected using the extension of the output file."
          " If this one is missing or if it does not correspond to the target output format,"
          " this switch allows to set it."
  },
  .depths={
    .shswitch="-D", .lgswitch="--depth-list", .arswitch="[+|-] depths", .argfmt=SWAF_Str,
    .doc={ "Include or exclude depths listed (ex: -D +2,3:13,5)." },
    .man= "The depths string is a comma-separated list of numbers or ranges, a range beeing 2 numbers separated by colon (:)."
          " With depths or +depths, keep only those depths. With -depths, keep all depths except those in the list."
          " For example, \"-D +10,40,55:70,80\" means to keep only layers 10, 40, 55 through 70, and 80 "
          " and \"-D -1:997,998\" means the layers 0 and 999."
  },
  .adjustBB={
    .shswitch="-K", .lgswitch="--adjust-BB", 
    .doc={ "Adjust bounding box according to selected depths", "given with '-D +/-list' option." },
    .man= "The selection of the depths with the \"-D +/-depths\" option does normally not affect the calcualtion"
          " of the bounding box. Thus the generated document might have a much larger bounding box than necessary."
          " If -K is given then the bounding box is adjusted to include only the objects in the selected depths."
  },
#define SCALE_EXCLUSIVE_SW  SW(multipageZTF),SW(magnification),SW(maxdimension)
  .magnification={
    .shswitch="-m", .lgswitch="--magnification", .arswitch="sf", .argfmt=SWAF_Dble,
    .optkind=SWOK_MinMax, .valmin={.dreal=0.01}, .valmax={.dreal=100},
    .exclusive = { SCALE_EXCLUSIVE_SW },
    .doc={ "Set magnification (scale factor), the default is 1.0.", 
           "This may not be used with the \"-Z md\" and \"-Mztf RxC\" options." },
    .man= "Set the magnification (scale factor) at which the figure is rendered to sf. The default is 1.0."
          " Notice that for raster image format, this option scales the PPI the default of which is 72."
          " This may not be used with the maximal dimension option (-Z) and the zoom-to-fit option (-Mztf)."
  },
  .maxdimension={
    .shswitch="-Z", .lgswitch="--max-dimension", .arswitch="md", .argfmt=SWAF_Dble,
    .exclusive = { SCALE_EXCLUSIVE_SW },
    .doc={ "Set magnification so that the maximum dimension (width or height) is",
           "md inches/cm.  This may not be used with the options \"-m sf\" \"-Mztf RxL\"." },
    .man= "Set magnification so that the maximum dimension (width or height) is md inches or centimeters,"
          " depending on whether the figure was saved with imperial or metric units."
          " Notice that for raster image format, this option sets the maximal dimension of figure"
          " to 72*md pixels for imperial unit and to 72*md/2.54 pixel for metric unit."
          " This may not be used with the magnification option (-m) and the zoom-to-fit option (-Mztf)"
  },
  .borderwidth={
    .shswitch="-b", .lgswitch="--border-width", .arswitch="n", .argfmt=SWAF_Dble,
    .doc={ "Specify width of blank border around figure (1/72 inch)." },
    .man= "This option adds a blank border around the figure. The border thickness is n points (1/72 in)."
  },
/** PDF options **/
  // -B 'Wx [Wy X0 Y0]'
  // -R 'Wx [Wy X0 Y0]'
  //*-b borderwidth
  // -C dummy_arg  Add a color *binary* TIFF preview for Microsoft products that need a binary  preview.   See
  //               also -T (monochrome preview).  A dummy argument must be supplied for historical reasons

  // Gravity
  // -c            option  centers  the  figure  on  the page.  The centering may not be accurate ...
  // -e            option puts the figure against the edge (not centered) of the page.
#define GRAVITY_EXCLUSIVE_SW  .exclusive = { SW(gravity), SW(gravityCC), SW(gravityEE) }
  .gravity={
    .shswitch="-g", .lgswitch="--gravity", .arswitch="wrd", .argfmt=SWAF_Str,
    GRAVITY_EXCLUSIVE_SW,
    .doc={ "Place the figure at the position given by wrd wind rose direction",
           "(eg: NN, NE, EE, SE, SS, SW, WW, NW, CC)." },
    .man= "This option places the figure at the position given by the wrd wind rose direction on the page,"
          " the page size being defined by the \"-z n\" switch."
          "The wind rose directions are:"
          " NN (horizontally=centered vertically=top)"
          ", NE (horizontally=right vertically=top)"
          ", EE (horizontally=right vertically=centered)"
          ", SE (horizontally=right vertically=bottom)"
          ", SS (horizontally=centered vertically=bottom)"
          ", SW (horizontally=left vertically=bottom)"
          ", WW (horizontally=left vertically=centered)"
          ", NW (horizontally=left vertically=top)"
          ", CC (horizontally=centered vertically=centered)."
  },
  .gravityCC={
    .shswitch="-c", .lgswitch="--gravity-center", .argfmt=SWAF_None,
    .optkind=SWOK_IsDefSWStr, .value.str="CC",
     GRAVITY_EXCLUSIVE_SW,
    .doc={ "Center the figure on the page. It's the default gravity" },
    .man= "This option centers the figure on the page,"
          " the page size being defined by the \"-z n\" switch."
          " More specifically, on the page, the center of the figure bounding box"
          " and the page center are the same point."
          " This switch is a shortcut to \"-g CC.\""
          " and it is the default gravity switch."
  },
  .gravityEE={
    .shswitch="-e", .lgswitch="--gravity-east", .argfmt=SWAF_None,
    .optkind=SWOK_HasDefValStr, .value.str="EE",
     GRAVITY_EXCLUSIVE_SW,
    .doc={ "Place the figure on the left edge of the page." },
    .man= "This option centers vertically the figure and places it horizontally"
          " on the left edge of the page,"
          " the page size being defined by the \"-z n\" switch."
          " More specifically, on the page,"
          " the PP point (the middle of page left edge) and"
          " the FP point (the middle of figure left edge) are the same point."
          " This switch is a shortcut to \"-g EE.\"."
  },
  // -F            Use correct font sizes (points) instead of the traditional  size  that  xfig/fig2dev  uses,
  //               which is 1/80 inch.  The corresponding xfig command-line option is -correct_font_size.
  // -g color      Use color for the background.

  // Orientation
  // -l dummy_arg  Generate  figure  in landscape mode.  The dummy argument is ignored, but must appear on the
  // -p dummy_arg  Generate  figure  in  portrait mode.  The dummy argument is ignored, but must appear on the
  //+-f (file default).
#define ORIENTATION_EXCLUSIVE_SW  .exclusive = { SW(pageOrientLandscape), SW(pageOrientPortrait), SW(pageOrientFigFile) }
#define ORIENTATION_EXCLUSIVE_TXT " Notice that the options \"-l\" \"-p\" and \"-f\" are mutually exclusive."
  .pageOrientLandscape={
    .shswitch="-l", .lgswitch="--orientation-landscape", .arswitch="dummy", .argfmt=SWAF_StrOpt,
    ORIENTATION_EXCLUSIVE_SW,
    .doc={ "Generate the figure in landscape mode. The argument is ignored and optional." },
    .man= "This option generates the figure in landscape mode. The dummy argument is ignored and optional."
          ORIENTATION_EXCLUSIVE_TXT
  },
  .pageOrientPortrait={
    .shswitch="-p", .lgswitch="--orientation-portrait", .arswitch="dummy", .argfmt=SWAF_StrOpt,
    ORIENTATION_EXCLUSIVE_SW,
    .doc={ "Generate the figure in portrait mode. The argument is ignored and optional." },
    .man= "This option generates the figure in portrait mode. The dummy argument is ignored and optional."
          ORIENTATION_EXCLUSIVE_TXT
  },
  .pageOrientFigFile={
    .shswitch="-f", .lgswitch="--orientation-file", .argfmt=SWAF_None,
    .doc={ "Generate  the figure in portrait or landscape mode as specified in",
           "the fig file. It's the orientation default." },
    ORIENTATION_EXCLUSIVE_SW,
    .man= "This option generates the figure in portrait or landscape mode as specified in the figure file."
          " This switch is the default orientation option."
          ORIENTATION_EXCLUSIVE_TXT
  },

  // Pages
  // -z papersize
  // -M            Generate multiple pages if figure exceeds paper size.
  // -ztf [lxc]    zoom to fit
  .paperdefinition={
    .shswitch="-z", .lgswitch="--paper-name", .arswitch="n", .argfmt=SWAF_Str,
    .doc={ "Set the target paper to n (A4, letter, A, ...).",
           "This option is ignored for raster image format." },
    .man= "This option sets the target paper to n. n must be a standard paper name such as"
          " Letter, Legal, A, B, C, D, E, A1 to A9, B0 to B10."
          " This option is ignored for raster image format."
  },
  .multipage={
    .shswitch="-M", .lgswitch="--multipage", .argfmt=SWAF_StrOpt,
    .exclusive = { SW(multipageZTF) },
    .doc={ "Generate multiple pages if figure exceeds paper size." },
    .man= "If the figure size exceeds the paper size, yfigdev truncates it by default."
          " This option builds a page matrix large enough to fit the entire figure."
          " This option is exclusive with the \"-Mztf RxL\" switch."
  },
  .multipageZTF={
    .shswitch="-P", .lgswitch="--multipage-ztf", .arswitch="RxC", .argfmt=SWAF_StrOpt,
    .exclusive = { SCALE_EXCLUSIVE_SW, SW(multipage) },
    .doc={ "Zoom the figure to fill as much as possible the page area but without",
           "truncating it. RxC defines a R times C page matrix. The default is 1x1.",
           "It is exclusive with \"-M\", \"-m sf\" and \"-Z md\"." },
    .man= "This option builds a page matrix of R rows and C columns,"
          " the page size being defined by the \"-z n\" switch."
          " The figure is zoomed to fill as much as possible the area of the page matrix but without truncating it."
          " The RxC argument is optional, its default is a single page (1x1)."
          " This option is exclusive with the switches \"-M\", \"-m sf\" and \"-Z md\"."
  },
  // -N            Convert all colors to grayscale.
  // -n name       Set  the  Title  part  of  the PostScript output to name.  This is useful when the input to
  //               fig2dev comes from standard input.
  // -O            When used with -M, overlaps the pages slightly to get around the problem of the unprintable
  //               area in most printers.
  // -T            Add a monochrome *binary* TIFF preview for Microsoft products that need a  binary  preview.
  //               See also -C (color preview).
  // -x offset     shift  the  figure in the X direction by offset units (1/72 inch).  A negative value shifts
  //               the figure to the left and a positive value to the right.  Not available in EPS.
  // -y offset     shift the figure in the Y direction by offset units (1/72 inch).  A negative  value  shifts
  //               the figure up and a positive value down.  Not available in EPS.
};

/*======================================================================*/
