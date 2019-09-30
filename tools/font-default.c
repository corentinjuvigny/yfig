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
 * $Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: font-default.c 362 2018-11-18 22:55:21Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/font-default.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Provides the font_defaultDefSet() function.                        =*/
/*=                                                                    =*/
/*= See the font.h header for documentation.                           =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/font.h"

/*======================================================================*/

static TfontDef fontTable[FONT_NB] = {
// 0
    {.family="times"                   , .style ="medium regular"
                                       , .geom = { 0.416, 1.000, 1.058, 0.343 } // FontGeom00!
                                       , .stdfmt="times medium regular %f"
                                       , .pdffmt="Times-Roman"
                                       , .x11fmt="-*-times-medium-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="times"
    },
    {.family="times"                   , .style ="medium italic"
                                       , .geom = { 0.409, 1.000, 1.073, 0.343 } // FontGeom01!
                                       , .stdfmt="times medium italic %f"
                                       , .pdffmt="Times-Italic"
                                       , .x11fmt="-*-times-medium-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="times italic"
    },
    {.family="times"                   , .style ="regular bold"
                                       , .geom = { 0.439, 1.000, 1.118, 0.463 } // FontGeom02!
                                       , .stdfmt="times bold regular %f"
                                       , .pdffmt="Times-Bold"
                                       , .x11fmt="-*-times-bold-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="times bold"
    },
    {.family="times"                   , .style ="italic bold"
                                       , .geom = { 0.423, 1.000, 1.088, 0.418 } // FontGeom03!
                                       , .stdfmt="times bold italic %f"
                                       , .pdffmt="Times-BoldItalic"
                                       , .x11fmt="-*-times-bold-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="times bold italic"
    },
    {.family="avantgarde"              , .style ="book regular"
                                       , .geom = { 0.493, 1.000, 1.163, 0.463 } // FontGeom04!
                                       , .stdfmt="avantgarde book regular %f"
                                       , .x11fmt="-*-avantgarde-book-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="avantgarde"
    },
    {.family="avantgarde"              , .style ="book oblique"
                                       , .geom = { 0.492, 1.000, 1.163, 0.463 } // FontGeom05!
                                       , .stdfmt="avantgarde book oblique %f"
                                       , .x11fmt="-*-avantgarde-book-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="avantgarde italic"
    },
    {.family="avantgarde"              , .style ="demi regular"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom06!
                                       , .stdfmt="avantgarde demi regular %f"
                                       , .x11fmt="-*-avantgarde-demi-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="avantgarde demi"
    },
    {.family="avantgarde"              , .style ="demi oblique"
                                       , .geom = { 0.518, 1.000, 0.923, 0.156 } // FontGeom07!
                                       , .stdfmt="avantgarde demi oblique %f"
                                       , .x11fmt="-*-avantgarde-demi-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="avantgarde demi italic"
    },
    {.family="bookman"                 , .style ="light regular"
                                       , .geom = { 0.502, 1.000, 1.073, 0.343 } // FontGeom08!
                                       , .stdfmt="bookman light regular %f"
                                       , .x11fmt="-*-bookman-light-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="bookman light"
    },
    {.family="bookman"                 , .style ="light italic"
                                       , .geom = { 0.488, 1.000, 1.073, 0.358 } // FontGeom09!
                                       , .stdfmt="bookman light italic %f"
                                       , .x11fmt="-*-bookman-light-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="bookman light italic"
    },
// 10
    {.family="bookman"                 , .style ="demi regular"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom10!
                                       , .stdfmt="bookman demi regular %f"
                                       , .x11fmt="-*-bookman-demi-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="bookman demi"
    },
    {.family="bookman"                 , .style ="demi italic"
                                       , .geom = { 0.518, 1.000, 0.923, 0.156 } // FontGeom11!
                                       , .stdfmt="bookman demi italic %f"
                                       , .x11fmt="-*-bookman-demi-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="bookman demi italic"
    },
    {.family="courier"                 , .style ="medium regular"
                                       , .geom = { 0.600, 1.000, 0.848, 0.140 } // FontGeom12!
                                       , .stdfmt="courier medium regular %f"
                                       , .pdffmt="Courier"
                                       , .x11fmt="-*-courier-medium-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="courier"
    },
    {.family="courier"                 , .style ="medium oblique"
                                       , .geom = { 0.600, 1.000, 0.848, 0.140 } // FontGeom13!
                                       , .stdfmt="courier medium oblique %f"
                                       , .pdffmt="Courier-Oblique"
                                       , .x11fmt="-*-courier-medium-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="courier italic"
    },
    {.family="courier"                 , .style ="bold regular"
                                       , .geom = { 0.600, 1.000, 0.848, 0.140 } // FontGeom14!
                                       , .stdfmt="courier bold regular %f"
                                       , .pdffmt="Courier-Bold"
                                       , .x11fmt="-*-courier-bold-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="courier bold"
    },
    {.family="courier"                 , .style ="bold oblique"
                                       , .geom = { 0.600, 1.000, 0.840, 0.133 } // FontGeom15!
                                       , .stdfmt="courier bold oblique %f"
                                       , .pdffmt="Courier-BoldOblique"
                                       , .x11fmt="-*-courier-bold-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="courier bold italic"
    },
    {.family="helvetica"               , .style ="medium regular"
                                       , .geom = { 0.454, 1.000, 1.148, 0.433 } // FontGeom16!
                                       , .stdfmt="helvetica medium regular %f"
                                       , .pdffmt="Helvetica"
                                       , .x11fmt="-*-helvetica-medium-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica"
    },
    {.family="helvetica"               , .style ="medium oblique"
                                       , .geom = { 0.452, 1.000, 1.125, 0.411 } // FontGeom17!
                                       , .stdfmt="helvetica medium oblique %f"
                                       , .pdffmt="Helvetica-Oblique"
                                       , .x11fmt="-*-helvetica-medium-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica italic"
    },
    {.family="helvetica"               , .style ="bold regular"
                                       , .geom = { 0.490, 1.000, 1.125, 0.433 } // FontGeom18!
                                       , .stdfmt="helvetica bold regular %f"
                                       , .pdffmt="Helvetica-Bold"
                                       , .x11fmt="-*-helvetica-bold-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold"
    },
    {.family="helvetica"               , .style ="bold oblique"
                                       , .geom = { 0.488, 1.000, 1.125, 0.433 } // FontGeom19!
                                       , .stdfmt="helvetica bold oblique %f"
                                       , .pdffmt="Helvetica-BoldOblique"
                                       , .x11fmt="-*-helvetica-bold-o-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold italic"
    },
// 20
    {.family="helvetica"               , .style="medium regular narrow"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom20!
                                       , .stdfmt="helvetica medium regular narrow %f"
                                       , .x11fmt= "-*-helvetica-medium-r-narrow--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold italic"
    },
    {.family="helvetica"               , .style ="medium oblique narrow"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom21!
                                       , .stdfmt="helvetica medium oblique narrow %f"
                                       , .x11fmt="-*-helvetica-medium-o-narrow--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold italic"
    },
    {.family="helvetica"               , .style ="bold regular narrow"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom22!
                                       , .stdfmt="helvetica bold regular narrow %f"
                                       , .x11fmt="-*-helvetica-bold-r-narrow--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold italic"
    },
    {.family="helvetica"               , .style ="bold oblique narrow"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom23!
                                       , .stdfmt="helvetica bold oblique narrow %f"
                                       , .x11fmt="-*-helvetica-bold-o-narrow--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="helvetica bold italic"
    },
    {.family="new century schoolbook"  , .style ="medium regular"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom24!
                                       , .stdfmt="new\\ century\\ schoolbook medium regular %f"
                                       , .x11fmt="-*-new century schoolbook-medium-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .shname="new century"
                                       , .lgname="new-century"
    },
    {.family="new century schoolbook"  , .style="medium italic"
                                       , .geom = { 0.518, 1.000, 0.923, 0.156 } // FontGeom25!
                                       , .stdfmt="new\\ century\\ schoolbook medium italic %f"
                                       , .x11fmt="-*-new century schoolbook-medium-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .shname="new century"
                                       , .lgname="new-century italic"
    },
    {.family="new century schoolbook"  , .style="bold regular"
                                       , .geom = { 0.583, 1.000, 0.923, 0.156 } // FontGeom26!
                                       , .stdfmt="new\\ century\\ schoolbook bold regular %f"
                                       , .x11fmt="-*-new century schoolbook-bold-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .shname="new century"
                                       , .lgname="new-century bold"
    },
    {.family="new century schoolbook"  , .style ="bold italic"
                                       , .geom = { 0.583, 1.000, 0.923, 0.156 } // FontGeom27!
                                       , .stdfmt="new\\ century\\ schoolbook bold italic %f"
                                       , .x11fmt="-*-new century schoolbook-bold-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .shname="new century"
                                       , .lgname="new-century bold italic"
    },
    {.family="palatino"                , .style ="medium regular"
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom28!
                                       , .stdfmt="palatino-medium regular %f"
                                       , .x11fmt="-*-palatino-medium-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="palatino"
    },
    {.family="palatino"                , .style ="medium italic"
                                       , .geom = { 0.518, 1.000, 0.923, 0.156 } // FontGeom29!
                                       , .stdfmt="palatino-medium italic %f"
                                       , .x11fmt="-*-palatino-medium-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="palatino italic"
    },
// 30
    {.family="palatino"                , .style ="bold regular"
                                       , .geom = { 0.474, 1.000, 1.103, 0.373 } // FontGeom30!
                                       , .stdfmt="palatino bold regular %f"
                                       , .x11fmt="-*-palatino-bold-r-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="palatino bold"
                                       , .lgname="palatino bold"
    },
    {.family="palatino"                , .style ="bold italic"
                                       , .geom = { 0.458, 1.000, 1.110, 0.388 } // FontGeom31!
                                       , .stdfmt="palatino bold italic %f"
                                       , .x11fmt="-*-palatino-bold-i-normal--%d-%s-*-*-*-*-%s-*"
                                       , .lgname="palatino bold italic"
                                       , .lgname="palatino bold italic"
    },
    {.family="symbol (greek)"          , .style ="medium regular"
                                       , .geom = { 0.515, 1.000, 1.013, 0.313 } // FontGeom32!
                                       , .stdfmt="symbol medium regular %f"
                                       , .pdffmt="Symbol"
                                       , .x11fmt="-*-symbol-medium-r-normal--%d-%s-*-*-*-*-*-*"
                                       , .shname="greek symbol"
    },
    {.family="itc zapf chancery"       , .style ="medium italic"
                                       , .geom = { 0.518, 1.000, 0.923, 0.156 } // FontGeom33!
                                       , .stdfmt="itc\\ zapf\\ chancery medium italic %f"
                                       , .x11fmt="-*-itc zapf chancery-medium-i-normal--%d-%s-*-*-*-*-*-*"
                                       , .shname="chancery"
                                       , .lgname="itc-zapf-chancery"
    },
    {.family="itc zapf dingbats"       , .style =""
                                       , .geom = { 0.517, 1.000, 0.923, 0.156 } // FontGeom34!
                                       , .stdfmt="itc\\ zapf\\ dingbats %f"
                                       , .pdffmt="ZapfDingbats"
                                       , .x11fmt="-*-itc zapf dingbats-*-*-*--%d-%s-*-*-*-*-*-*"
                                       , .shname="dingbats"
                                       , .lgname="itc-zapf-dingbats"
    },
};

/*====================================================================*/

extern TfontDefSet* font_defaultDefSet()
{
    TfontDefSet* s = fontdefset_new( TOOL_TABLE_NbEle(fontTable) );
    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(fontTable) ; i+=1)
            fontdefset_add(s,fontTable+i);
    return s;
}

/*====================================================================*/
// vim: set textwidth=0
