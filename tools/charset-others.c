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
 *      $Id: charset-others.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset-others.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Implement the charset conversion that the iconv API can not do     =*/
/*= using the conversion table of the charset-table-unicode.c file.    =*/
/*= Those that iconv can do are implemented in the charset.c file.     =*/
/*=                                                                    =*/
/*= See charset.h header for documentation.                            =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/charset.h"

#include "charset-table.h"

/*======================================================================*/

extern TstrEaten cs_conv_Zdb8toUtf8(CstrBrief str)
{
    CcodeTable* t = gl_zapfDingBats;
    TEMP_asf_printf( t->mincode!=0 || t->maxcode!=256,
            "the code assumes that zapfDingBats covers all codes [0:256[,"
            " found [%d:%d[\n",t->mincode,t->maxcode);
    TcodeEle* e; int i,zdb;
    int      len   = strlen(str);
    Tuint32* uni   = tool_malloc(sizeof(*uni)*len+1);
    for ( i=0 ; i<len ; i+=1 ) {
        zdb = str[i]&0xFF;
        e = t->eles[zdb];
        if ( e==0 )
            uni[i] = 0;
        else
            uni[i] = CS_UNICODE(e->ucode) ;
    }
    TstrEaten ret = cs_conv_Utf32toUtf8(uni,len);
    tool_free(uni);
    return ret;
}

extern TstrEaten cs_conv_Utf8toZdb8(CstrBrief str)
{
    CcodeTable* t = gl_unicodes;

    TcodeEle* e; int i,idx;
    int      len  = strlen(str);
    Tuint32* unis = cs_conv_Utf8toUtf32(str, &len);
    char*    ret  = tool_malloc(sizeof(*ret)*len+1);
    for ( idx=0,i=0 ; i<len ; i+=1 ) {
        Tuint32 uni = CS_UNICODE(unis[i]);
        if ( uni==0xFEFF ) continue;    // little endian marker
        if ( uni<t->mincode || uni>=t->maxcode )
            e = 0;
        else 
            e = t->eles[uni-t->mincode];
        if ( e==0 ) {
            // not convertible in ZapfDingBats, we skip it.
            continue;
        }
        if (  ! CS_FL_ZapfDingbatsIs(e->ucode) ) {
            // not a ZapfDingBats glyph, we skip it.
            continue;
        }
        ret[idx++] = e->zcode;
    }
    if ( ret[idx-1]!=0 ) ret[idx]=0;
    tool_free( unis );

    return ret;
}

/*======================================================================*/
