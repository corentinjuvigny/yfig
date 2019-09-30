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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: imgfmt.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/imgfmt.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This header defines the image formats.                             =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_IMGFMT_H
#define FILE_TOOLS_IMGFMT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/
/*= Identify the image formats.                                        =*/

typedef enum   _TimgFmtKd TimgFmtKd;
typedef const   TimgFmtKd CimgFmtKd;

enum _TimgFmtKd {
    IMGFMT_VecFirst=0, // index of the first vector image format
    IMGFMT_VecNb=3,    // # of vector formats
      IMGFMT_PDF=IMGFMT_VecFirst+0,
      IMGFMT_SVG=IMGFMT_VecFirst+1,
      IMGFMT_TEX=IMGFMT_VecFirst+2,
    IMGFMT_RasFirst=IMGFMT_VecNb, // index of the first raster image format
    IMGFMT_RasNb=3,               // # of raster formats
      IMGFMT_PNG=IMGFMT_RasFirst+0,
      IMGFMT_TIF=IMGFMT_RasFirst+1,
      IMGFMT_JPG=IMGFMT_RasFirst+2,
};

/*======================================================================*/
/*= Miscellaneous information about the image formats.                 =*/

typedef struct _TimgFmtDt TimgFmtDt;
typedef const   TimgFmtDt CimgFmtDt;

struct _TimgFmtDt {
    TimgFmtKd  kind;
    int        raster;     // 1:raster format; 0:vector format
    TstrStatic ext;        // file name extension for this format.
    TstrStatic exts[10];   // current extensions of file names (null terminated).
    TstrStatic shortname;  // eg: JPEG
    TstrStatic fullname;   // eg: Joint Photographic Experts Group
};

// Return the NULL pointer for invalid kd image format.
extern CimgFmtDt* tool_imgfmt_getByKd      (TimgFmtKd kd);
extern CimgFmtDt* tool_imgfmt_getByExt     (CstrBrief ext);

// Return the "???" string for invalid kd image format.
Inline CstrStatic tool_imgfmt_getExtendByKd(TimgFmtKd kd) { CimgFmtDt*dt=tool_imgfmt_getByKd(kd); return dt ? dt->ext       : "???"; }
Inline CstrStatic tool_imgfmt_getShNameByKd(TimgFmtKd kd) { CimgFmtDt*dt=tool_imgfmt_getByKd(kd); return dt ? dt->shortname : "???"; }
Inline CstrStatic tool_imgfmt_getLgNameByKd(TimgFmtKd kd) { CimgFmtDt*dt=tool_imgfmt_getByKd(kd); return dt ? dt->fullname  : "???"; }

/**
 * The tool_imgfmt_isExtOfKd function checks if the ext string is
 * an usual extension of the kd file format.
 * If ext is the NULL pointer or kd is an invalid image format
 * the check is false.
 *
 * Return:
 *  On success (check is TRUE), the tool_imgfmt_isExtOfKd function
 *  returns 1 otherwise it returns 0.
**/
extern int       tool_imgfmt_isExtOfKd    (TimgFmtKd kd, CstrBrief ext);

/*======================================================================*/
/*= Image converter.                                                   =*/
/*=                                                                    =*/
/*= The tool_imgfmt_convIM function is a frontend to ImageMagick.      =*/
/*= It converts the sp file in the sk format in dk format into the     =*/
/*= dp file.                                                           =*/
/*= The ppi parameter (Pixel Per Inch) is only used for conversion     =*/
/*= from a vector format to a raster format.                           =*/
/*= The q  parameter gives the format quality of the destination file. =*/
/*= It must be in range ]0:1] where 1 indicates the best quality.      =*/
/*= It is used if the destination format is a lossy format.            =*/
/*=                                                                    =*/
/*= Return:                                                            =*/
/*=  The tool_imgfmt_convIM function returns 0 on success and          =*/
/*=  a no null value on failure.                                       =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  - md: the message driver for printing messages                    =*/
/*=        or NULL for gl_msgdrv_default.                              =*/
/*=        Set it to gl_msgdrv_silence for working in silence mode.    =*/
/*=    sp,sk: the source file name and its format.                     =*/
/*=            sk set to -1 means auto-detect.                         =*/
/*=    dp,dk: the generated file name and its format.                  =*/
/*=    ppi:   the Pixel Per Inch of generated file (>0).               =*/
/*=           0 means the default PPI.                                 =*/
/*=    q:     the image quality of generated file (in ]0:1]).          =*/
/*=           0 means the default quality.                             =*/

extern int tool_imgfmt_convIM(Tmsgdrv* md, TstrBrief sp, TimgFmtKd sk,
                                           TstrBrief dp, TimgFmtKd dk,
                                           double ppi, double q);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_IMGFMT_H
/*======================================================================*/
