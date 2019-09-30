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
 *      $Id: papersize.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/papersize.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= paper sizes                                                        =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/papersize.h"
#include "tools/lenunit.h"

/*======================================================================*/

const Tpaperdef tool_paperSizes[TOOL_NUMPAPERSIZES] = {
    {"Letter"  , "Letter  (8.5\" x 11\" / 216 x 279 mm)" , 21.6 ,  27.9 , 10200, 13200},
    {"Legal"   , "Legal   (8.5\" x 14\" / 216 x 356 mm)" , 21.6 ,  35.6 , 10200, 16800}, 
    {"Tabloid" , "Tabloid ( 11\" x 17\" / 279 x 432 mm)" , 27.9 ,  43.2 , 13200, 20400}, 
    {"A"       , "ANSI A  (8.5\" x 11\" / 216 x 279 mm)" , 21.6 ,  27.9 , 10200, 13200}, 
    {"B"       , "ANSI B  ( 11\" x 17\" / 279 x 432 mm)" , 27.9 ,  43.2 , 13200, 20400}, 
    {"C"       , "ANSI C  ( 17\" x 22\" / 432 x 559 mm)" , 43.2 ,  55.9 , 20400, 26400}, 
    {"D"       , "ANSI D  ( 22\" x 34\" / 559 x 864 mm)" , 55.9 ,  86.4 , 26400, 40800}, 
    {"E"       , "ANSI E  ( 34\" x 44\" / 864 x 1118 mm)", 86.4 , 111.8 , 40800, 52800}, 
    {"A9"      , "ISO A9  (  37mm x   52mm)",   3.7 ,   5.2 ,  1748,  2467},
    {"A8"      , "ISO A8  (  52mm x   74mm)",   5.2 ,   7.4 ,  2457,  3500},
    {"A7"      , "ISO A7  (  74mm x  105mm)",   7.4 ,  10.5 ,  3496,  4960},
    {"A6"      , "ISO A6  ( 105mm x  148mm)",  10.5 ,  14.8 ,  4960,  6992}, 
    {"A5"      , "ISO A5  ( 148mm x  210mm)",  14.8 ,  21.0 ,  6992,  9921},
    {"A4"      , "ISO A4  ( 210mm x  297mm)",  21.0 ,  29.7 ,  9921, 14031}, 
    {"A3"      , "ISO A3  ( 297mm x  420mm)",  29.7 ,  42.0 , 14031, 19843}, 
    {"A2"      , "ISO A2  ( 420mm x  594mm)",  42.0 ,  59.4 , 19843, 28063}, 
    {"A1"      , "ISO A1  ( 594mm x  841mm)",  59.4 ,  84.1 , 28063, 39732}, 
    {"A0"      , "ISO A0  ( 841mm x 1189mm)",  84.1 , 118.9 , 39732, 56173}, 
    {"B10"     , "JIS B10 (  32mm x   45mm)",   3.2 ,   4.5 ,  1516,  2117},
    {"B9"      , "JIS B9  (  45mm x   64mm)",   4.5 ,   6.4 ,  2117,  3017},
    {"B8"      , "JIS B8  (  64mm x   91mm)",   6.4 ,   9.1 ,  3017,  4300},
    {"B7"      , "JIS B7  (  91mm x  128mm)",   9.1 ,  12.8 ,  4300,  6050},
    {"B6"      , "JIS B6  ( 128mm x  182mm)",  12.8 ,  18.2 ,  6050,  8598},
    {"B5"      , "JIS B5  ( 182mm x  257mm)",  18.2 ,  25.7 ,  8598, 12150},
    {"B4"      , "JIS B4  ( 257mm x  364mm)",  25.7 ,  36.4 , 12150, 17200},
    {"B3"      , "JIS B3  ( 364mm x  515mm)",  36.4 ,  51.5 , 17200, 24333},
    {"B2"      , "JIS B2  ( 515mm x  728mm)",  51.5 ,  72.8 , 24333, 34400},
    {"B1"      , "JIS B1  ( 728mm x 1030mm)",  72.8 , 103.0 , 34400, 48666},
    {"B0"      , "JIS B0  (1030mm x 1456mm)", 103.0 , 145.6 , 48666, 68783},
};

extern Cpaperdef* tool_paperdef_get(CstrBrief name)
{
    int i,len = strlen(name);
    for (i=0; i<TOOL_NUMPAPERSIZES; i++) {
        if (strncasecmp(name,tool_paperSizes[i].sname,len) == 0)
                return tool_paperSizes+i;
    }
    return 0;
}

extern Tpaperdef* tool_paperdef_newClone   (Cpaperdef* src)
{
    Tpaperdef* ppd = tool_malloc( sizeof(*ppd) );
    *ppd = *src;
    ppd->fname = tool_strdup( ppd->fname );
    return ppd;
}

extern Tpaperdef* tool_paperdef_newByIndex (int idx)
{ return tool_paperdef_newClone( tool_paperSizes+idx ); }

extern Tpaperdef* tool_paperdef_newByName  (CstrBrief name)
{ return tool_paperdef_newClone( tool_paperdef_get(name) ); }

extern Tpaperdef* tool_paperdef_newCustomCm(double width,double height)
{
    Tpaperdef* ppd = tool_malloc_0( sizeof(*ppd) );
    ppd->sname = "Custom";
    ppd->fname = tool_aprintf("Custom (%dmm x %dmm)",
            (int)(10.*width),(int)(10.*height));
    ppd->wcm = width;
    ppd->hcm = height;
    ppd->wfu = round( LUNIT_CmToFu(width)  );
    ppd->hfu = round( LUNIT_CmToFu(height) );
    return ppd;
}

extern Tpaperdef* tool_paperdef_newCustomPt(double width,double height)
{
    Tpaperdef* ppd = tool_malloc_0( sizeof(*ppd) );
    ppd->sname = "Custom";
    ppd->fname = tool_aprintf("Custom (%dmm x %dmm)",
            (int)LUNIT_PtToCm(10.*width),(int)LUNIT_PtToCm(10.*height));
    ppd->wcm = LUNIT_PtToCm(width);
    ppd->hcm = LUNIT_PtToCm(height);
    ppd->wfu = LUNIT_PtToFu(width);
    ppd->hfu = LUNIT_PtToFu(height);
    return ppd;
}

extern Tpaperdef* tool_paperdef_newCustomFu(double width,double height)
{
    Tpaperdef* ppd = tool_malloc_0( sizeof(*ppd) );
    ppd->sname = "Custom";
    ppd->fname = tool_aprintf("Custom (%dmm x %dmm)",
            (int)LUNIT_FuToCm(10.*width),(int)LUNIT_FuToCm(10.*height));
    ppd->wcm = LUNIT_FuToCm(width);
    ppd->hcm = LUNIT_FuToCm(height);
    ppd->wfu  = width;
    ppd->hfu = height;
    return ppd;
}

extern void       tool_paperdef_chgClone   (Tpaperdef**p,Cpaperdef* src)
{
    if ( *p == src ) return;
    if (*p!=0) tool_paperdef_delete(*p);
    *p = src==0 ? NULL : tool_paperdef_newClone( src );
}
extern void       tool_paperdef_chgByIndex (Tpaperdef**p,int idx)
{
    if (*p!=0) tool_paperdef_delete(*p);
    *p = tool_paperdef_newByIndex( idx );
}

extern void       tool_paperdef_chgByName  (Tpaperdef**p,CstrBrief name)
{
    if (*p!=0) tool_paperdef_delete(*p);
    *p = tool_paperdef_newByName( name );
}

extern void tool_paperdef_chgCustomCm(Tpaperdef**p,double width,double height) { if (*p!=0) tool_paperdef_delete(*p); *p = tool_paperdef_newCustomCm( width, height ); }
extern void tool_paperdef_chgCustomPt(Tpaperdef**p,double width,double height) { if (*p!=0) tool_paperdef_delete(*p); *p = tool_paperdef_newCustomPt( width, height ); }
extern void tool_paperdef_chgCustomFu(Tpaperdef**p,double width,double height) { if (*p!=0) tool_paperdef_delete(*p); *p = tool_paperdef_newCustomFu( width, height ); }

extern void       tool_paperdef_delete(Tpaperdef* ppd)
{
    free ( ppd->fname );
    free ( ppd );
}

extern void       tool_paperdef_deletePt(Tpaperdef**ppd)
{
    if ( *ppd==0 ) return;
    tool_paperdef_delete( *ppd );
    *ppd = NULL;
}

/*======================================================================*/
