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
 *      $Id: yfig-main-argv-parser.c 342 2018-11-08 00:14:55Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-main-argv-parser.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Define the switches of the command line ( main(int,char**argv) ).  =*/
/*=                                                                    =*/
/*= This file is included in the main file --> all is defined static   =*/
/*======================================================================*/

/*======================================================================*/

static void main_argv_setDefault(Tcmdline*cl)
{
    int s;
    for (s=0; s<cl->cl_switches->eleNb ; s+=1 ) {
        Tsw* sw = cl->cl_switches->eles[s];
        if ( sw->set==1 ) continue;
        if ( sw->resname==0 ) continue;
        switch ( sw->argfmt ) {
          case SWAF_Int:
            res_getInt(&gui_appResources,sw->resname,&sw->value.sint);
            break;
          case SWAF_Str: {
            const char* str;
            res_getStr(&gui_appResources,sw->resname,&str);
            sw->value.str = tool_strdup(str);
            break;
            }
          default:
            break;
        }
    }
}

static void main_argv_setResource(Tcmdline*cl)
{
    int s;
    for (s=0; s<cl->cl_switches->eleNb ; s+=1 ) {
        Tsw* sw = cl->cl_switches->eles[s];
        if ( sw->resname==0 ) continue;
        switch ( sw->argfmt ) {
          case SWAF_Int: {
            char tmp[50];
            sprintf(tmp,"%d",sw->value.sint);
            res_set(&gui_appResources,sw->resname,tmp,1);
            break;
            }
          case SWAF_Str:
            res_set(&gui_appResources,sw->resname,sw->value.str,1);
            break;
          default:
            break;
        }
    }
}

/*======================================================================*/
/*= the option table                                                   =*/

static TuserParam userParams = {
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
  .display={
    .shswitch="-display", .lgswitch="--display", .arswitch="h:s", .argfmt=SWAF_Str,
    .doc={ "Choose/force the output format when it can not be detected." },
    .doc={ "Set the X11 display to the s screen of h host. The default",
           "is the DISPLAY environment variable)." },
    .man= "The yfig program will draw its GUI on the \"h:s\" physical screen"
          " given by this option. Thus is the s th screen handled by th server"
          " running on the h host. The default value is the value of the"
          " DISPLAY environment variable"
  },
  .geometry={
    .shswitch="-geometry", .lgswitch="--geometry", .arswitch="geo", .argfmt=SWAF_Str,
    .doc={ "Set the the position and/or the geometry of the initial",
           "main window. geo is something like 100x200 or 100x200-10+20", },
    .man= "This option specifies the position of the initial yfig main window"
          " and its size. The \"geo\" parameter follows the standard X11"
          " geometry format."
          " \"WxH\" gives the window size of W (Width) by H (height) pixels."
          " \"-X+Y\" places the top-left corner of window at the point (X,Y) pixel."
          " A \"geo\" like \"WxH-X+Y\" places and sizes the initial window."
          " By default yfig sizes the initial window using the screen size"
          " and let the window manager to places it."
  },
  .iconsize={
    .resname=VGUI_ResN_SmallIcons,
    .shswitch="-is", .lgswitch="--icon-size", .arswitch="0|1", .argfmt=SWAF_Int,
    .value={ .sint=0 }, .valmin={.sint=0}, .valmax={.sint=1},
    .doc={ "The size of mode-panel icons (0:larg, 1:small)", },
    .man= "This option sets the size of the icons of the mode panel."
          " Set it to 0 for large ones and to 1 for small ones."
          " If this option is omitted, its default value is given by"
          " the SmallIcons resource."
  },
  .guifont={
    .resname=VGUI_ResN_GuiFont,
    .shswitch="-gf", .lgswitch="--gui-font", .arswitch="f:s:d", .argfmt=SWAF_Str,
    .doc={ "Set the font used in the GUI (not the picture) to f the family,",
           "to s pt the size and to d  the density (0:light, 100:bold)." },
    .man=  "This option allows to set the font used in the YFIG GUI (It does not"
           " alter the fonts used for drawing picture). It set the font family to"
           " \"f\", the font size to s pt and the  density to d (0:light, 100:bold)."
           " The \"f\", \"s\", \"d\" fields are optional, when one is omitted, its"
           " default value is preserved. For instance"
           "   - \"times\" changes the gui font family to \"times\"."
           "   - \":14\"   changes the font size to 14 pt."
           "   - \"::100\" changes the font density to very bold."
           "   - \"times:14:100\" does the 3 previous settings."
    },
};

/*======================================================================*/
