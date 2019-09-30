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
 *      $Id: user-options.h 337 2018-11-05 22:44:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/user-options.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Definition of the user options.                                   =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_USER_OPTIONS_H
#define FILE_USER_OPTIONS_H
/*======================================================================*/

#include "tools/cmdline.h"

/*======================================================================*/
/*= The option table                                                   =*/

struct _Tsw { SW_FIELD(); };

typedef struct _TuserParam {
    Tsw help;
    Tsw verbosity;
    Tsw dbglevel;
    Tsw outformat;
    Tsw depths;
    Tsw adjustBB;
    Tsw magnification;
    Tsw maxdimension;
    Tsw borderwidth;
    Tsw gravity;
    Tsw gravityCC;
    Tsw gravityEE;
    Tsw pageOrientLandscape;
    Tsw pageOrientPortrait;
    Tsw pageOrientFigFile;
    Tsw multipage;
    Tsw multipageZTF;
    Tsw paperdefinition;
} TuserParam;

extern TuserParam userParams;

/*======================================================================*/
/*= The option getters                                                 =*/

// retrieves the format of the output image from user options
//  - if mandatory is 1, it returns a valid format or exits.
//  - if mandatory is 0, it returns a valid format or null.
extern char* userParam_getOutFormat(int mandatory);

// retrieves the zoom factor: Caution the unit of dx, dy must be inch or cm
//                            see manual of maxdimension switch in option.c.
// retrieves the thickness of border around the figure.
// retrieves the paper definition
extern double     userParam_getZoomFactor(double dx, double dy);
extern double     userParam_getBorderThicknessCm();
extern Cpaperdef* userParam_getPaperDef();

// return:
//  0: no user data about depths 
//  1: The depth array is filled within the user data (0:unactive; 1:active)
extern int userParam_getDepth(int*depths, int depthn);

// return:
//  0: no multipageZTF option
//  1: multipageZTF option is given and the numbers of rows and
//     columns are returned into nrow and ncol.
extern int userParam_getRowColZTF(int*nrow, int*ncol);

/*======================================================================*/
/*= Private to users-options.c                                         =*/

extern int yyparse(int*depths,int depthn,char**inputptr);

/*======================================================================*/
#endif //  FILE_USER_OPTIONS_H
/*======================================================================*/
