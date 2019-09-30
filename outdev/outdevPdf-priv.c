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
 *$Software: YFIG-ODEV (a virtual Output DEVice)
 *      $Id: outdevPdf-priv.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/outdevPdf-priv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities.                                           =*/
/*=                                                                    =*/
/*= See outdevPdf-priv.h header for documentation.                     =*/
/*======================================================================*/

#include "outdevPdf-priv.h"

#include "tools/string-builder.h"

/*======================================================================*/

extern void od_pdf_addPatternToPage(TodPdf*od, int oid)
{
    strfmt_printfItem(od->pagepats," /P%03d %3d 0 R",oid,oid);
}

extern void od_pdf_addObjToPage(TodPdf*od, int*oid)
{
    if ( *oid<=0 ) return;

    strfmt_printfItem(od->pageobjs," %4d 0 R",*oid);
    *oid = 0;
}

// with oid=0 add a dummy object 0
// with oid>0 overwrite the first objct with oid 
extern void od_pdf_addStartObjToPage(TodPdf*od, int oid)
{
    char buf[100];
    sprintf(buf," %4d",oid);

    if ( oid<1 ) {
        strfmt_printfItem(od->pageobjs," %4d 0 R",0);
    } else {
        strncpy(od->pageobjs->str+od->pageobjs->shift,buf,strlen(buf));
    }
}

/*======================================================================*/
