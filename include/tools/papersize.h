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
 *      $Id: papersize.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/papersize.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= paper sizes                                                        =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_PAPERSIZE_H
#define FILE_TOOLS_PAPERSIZE_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "base-types.h"

/*======================================================================*/

typedef struct _Tpaperdef  Tpaperdef;
typedef const   Tpaperdef  Cpaperdef;

/*======================================================================*/

#define LETTER_WIDTH_FU  10200
#define LETTER_HEIGHT_FU 13200
#define A4_WIDTH_FU      9921
#define A4_HEIGHT_FU     14031

struct _Tpaperdef {
    char   *sname;       /* short name e.g. 'A' */
    char   *fname;       /* full name e.g. 'A     (8.5" x 11")' */
    double wcm,hcm;      /* width and height in cm */
    int    wfu,hfu;      /* width and height in fu (1/LUNIT_FuDefault inch) */
};

/*======================================================================*/

#define TOOL_NUMPAPERSIZES    29
extern Cpaperdef  tool_paperSizes[TOOL_NUMPAPERSIZES];

extern Cpaperdef* tool_paperdef_get(CstrBrief name);

extern Tpaperdef* tool_paperdef_newClone   (Cpaperdef* src);
extern Tpaperdef* tool_paperdef_newByIndex (int idx);
extern Tpaperdef* tool_paperdef_newByName  (CstrBrief name);
extern Tpaperdef* tool_paperdef_newCustomCm(double width,double height);
extern Tpaperdef* tool_paperdef_newCustomPt(double width,double height);
extern Tpaperdef* tool_paperdef_newCustomFu(double width,double height);

extern void       tool_paperdef_chgClone   (Tpaperdef**p,Cpaperdef* src);
extern void       tool_paperdef_chgByIndex (Tpaperdef**p,int idx);
extern void       tool_paperdef_chgByName  (Tpaperdef**p,CstrBrief name);
extern void       tool_paperdef_chgCustomCm(Tpaperdef**p,double width,double height);
extern void       tool_paperdef_chgCustomPt(Tpaperdef**p,double width,double height);
extern void       tool_paperdef_chgCustomFu(Tpaperdef**p,double width,double height);

extern void       tool_paperdef_delete  (Tpaperdef* ppd);
extern void       tool_paperdef_deletePt(Tpaperdef**ppd);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_TOOLS_PAPERSIZE_H
/*======================================================================*/
