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
 *      $Id: outdevTex-conv.c 381 2018-12-24 04:44:15Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/outdev/latex/outdevTex-conv.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The vtable for TodTex output device.                               =*/
/*=                                                                    =*/
/*= This file is only included by the outdevPdf.c so all definitions   =*/
/*= are static.                                                        =*/
/*======================================================================*/

#include "outdevTex-priv.h"

#include "tools/charset.h"

/*======================================================================*/

#define WRITE(b,n) fwrite(b,1,n,stream)

extern void odtex_str_print_std(Tmsgdrv*md, FILE* stream, CstrBrief text)
{
    while ( *text!=0 ) {
        int code = text[0]&0xff;
        if ( 0x00<=code && code< 0x20 ) {
            WRITE("?",1);
            text += 1;
            continue;
        }
        if ( 0x20<=code && code<=0x7f ) {
            switch ( code ) {
                case '$'  : WRITE("\\$",2); break;
                case '&'  : WRITE("\\&",2); break;
                case '|'  : WRITE("\\|",2); break;
                case '\\' : WRITE("$\\backslash$",12); break;
                default   : WRITE(text,1); break;
            }
            text += 1;
            continue;
        }
        if ( (code&0xE0)==0xC0 ) {
            text += 1;
            if ( *text==0 ) break;
            int code2 = text[0]&0xff;
            text += 1;
            if ( (code2&0xC0)!=0x80 ) continue;
            code = ((code&0x0F)<<6) | ((code2&0x3F)<<0) ;
            WRITE(text-2,2);
            continue;
        }
        if ( (code&0xF0)==0xE0 ) {
            text += 1;
            if ( *text==0 ) break;
            int code2 = text[0]&0xff;
            text += 1;
            if ( (code2&0xC0)!=0x80 ) continue;
            int code3 = text[0]&0xff;
            text += 1;
            if ( (code3&0xC0)!=0x80 ) continue;
            code = ((code&0x0F)<<12) | ((code2&0x3F)<<6) | ((code3&0x3F)<<0);
            switch ( code ) {
/* euro : € */  case 0x20AC : WRITE("\\euro",5); break;
                default     : WRITE(text-3,3); break;
            }
            continue;
        }
        tool_errerr_printf(md,"unexpected or unsupported character in UTF8 sequence (0x%02x)",*text&0xff);
        text += 1;
    }
}

extern void odtex_str_print_ding(Tmsgdrv*md, FILE* stream, CstrBrief text)
{
    int   i;
    char* dingtext = cs_conv_Utf8toZdb8(text);
    for (i=0 ; dingtext[i]!=0 ; i+=1) {
        fprintf(stream,"\\ding{%d}", dingtext[i]&0xff);
    }
    tool_free(dingtext);
}

/*======================================================================*/
