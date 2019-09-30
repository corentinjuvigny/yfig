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
 *      $Id: charset.c 143 2018-05-29 13:31:53Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Implement the charset conversion using the iconv API.              =*/
/*= Of course we find in this file only the charset converter that     =*/
/*= iconv can do.                                                      =*/
/*= Those that iconv can not do are implemented in the                 =*/
/*= charset-others.c file.                                             =*/
/*=                                                                    =*/
/*= See charset.h header for documentation.                            =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/charset.h"

#include <iconv.h>

/*======================================================================*/

#define UTF32  "UTF-32"

#define LATIN  "ISO_8859-15"
#define UTF8   "UTF-8"
#define CP1252 "CP1252"

/*======================================================================*/

static iconv_t cs_open_Utf32toUtf8()
{
    Cstr from = UTF32 "//TRANSLIT" ;
    Cstr to   = UTF8;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", UTF32,UTF8);
    return cd;
}

static iconv_t cs_open_Utf8toUtf32()
{
    Cstr from = UTF8 "//TRANSLIT" ;
    Cstr to   = UTF32;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", UTF8,UTF32);
    return cd;
}

static iconv_t cs_open_La15toUtf8()
{
    Cstr from = LATIN "//TRANSLIT" ;
    Cstr to   = UTF8;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", LATIN,UTF8);
    return cd;
}

static iconv_t cs_open_Utf8toLa15()
{
    Cstr from = UTF8 "//TRANSLIT" ;
    Cstr to   = LATIN;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", UTF8,LATIN);
    return cd;
}

static iconv_t cs_open_Cp1252toUtf8()
{
    Cstr from = CP1252 "//TRANSLIT" ;
    Cstr to   = UTF8;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", CP1252,UTF8);
    return cd;
}

static iconv_t cs_open_Utf8toCp1252()
{
    Cstr from = UTF8 "//TRANSLIT" ;
    Cstr to   = CP1252;
    iconv_t cd = iconv_open(to,from);

    TEMP_asf_printf(cd==0,"can not convert %s charset to %s charset", UTF8,CP1252);
    return cd;
}


static iconv_t cs_close(iconv_t cd)
{ iconv_close(cd); }

/*======================================================================*/

static TstrEaten cs_conv_withlen(CstrBrief str, int _ilen, int* _olen, iconv_t cd, CstrBrief from, CstrBrief to)
{
    char buf[2000];
    size_t ilen,  olen ;
    Cstr   istr;
    char  *ostr;
    int   rlen=0;
    char  *ret=0;

    ilen=_ilen;   istr=str;
    while ( ilen!=0 ) {
        int olen2;
        olen=sizeof(buf)-1; ostr=buf;
        if ( iconv(cd,(char**)&istr,&ilen,&ostr,&olen)<0 ) {
            tool_errerr_printf(NULL,"conversion from %s to %s fails",from,to);
            goto end;
        }
        *ostr=0;
        olen2 = sizeof(buf)-olen; // size within the tailing 0 
        ret = tool_realloc(ret,rlen+olen2);
        memcpy(ret+rlen,buf,olen2);
        rlen += olen2-1;
    }

end:
    cs_close(cd);
    if ( _olen!=0 ) *_olen = rlen;
    return ret;
}

static TstrEaten cs_conv(CstrBrief str, iconv_t cd, CstrBrief from, CstrBrief to)
{ return cs_conv_withlen(str, strlen(str), NULL, cd, from, to); }

extern TstrEaten cs_conv_La15toUtf8(CstrBrief str)
{
    iconv_t cd = cs_open_La15toUtf8();
    return cs_conv(str,cd,LATIN,UTF8);
}

extern TstrEaten cs_conv_Utf8toLa15(CstrBrief str)
{
    iconv_t cd = cs_open_Utf8toLa15();
    return cs_conv(str,cd,UTF8,LATIN);
}

extern TstrEaten cs_conv_Cp1252toUtf8(CstrBrief str)
{
    iconv_t cd = cs_open_Cp1252toUtf8();
    return cs_conv(str,cd,CP1252,UTF8);
}

extern TstrEaten cs_conv_Utf8toCp1252(CstrBrief str)
{
    iconv_t cd = cs_open_Utf8toCp1252();
    return cs_conv(str,cd,UTF8,CP1252);
}

extern TstrEaten cs_conv_Utf32toUtf8(Cuint32*  str, int  len)
{
    iconv_t cd = cs_open_Utf32toUtf8();
    return cs_conv_withlen((char*)str,len*4, NULL, cd,UTF32,UTF8);
}

extern Tuint32*  cs_conv_Utf8toUtf32(CstrBrief str, int* len)
{
    iconv_t cd  = cs_open_Utf8toUtf32();
    Tuint8* uni8 = cs_conv_withlen(str,strlen(str)+1,len,cd,UTF8,UTF32);
    TEMP_asf_printf( ((*len)%4)!=0, "unexpected UTF-32 len, found %d expected 4*i",*len);
    *len = *len /4; 
    Tuint32* uni32 = (Tuint32*)uni8;
    return uni32;
}

/*======================================================================*/

extern int cs_char_length(CstrBrief text)
{
    int i,nbc,ret=0;
    unsigned c;
    while ( (c=(*text)&0xff)!=0 ) {
        // ASCII
        if ( c<128 ) { ret+=1; text+=1; continue; }
        // ISO and no ASCII
        if ( 0xA0<=c && c<0x100 ) { ret+=1; text+=1; continue; }
        // UTF-8 2 3 and 4 chars (UTF-8 1 char is ascii)
        if ( (c&0xE0)==0xC0 ) { nbc=1; goto utf8_n; }
        if ( (c&0xF0)==0xE0 ) { nbc=2; goto utf8_n; }
        if ( (c&0xF1)==0xF0 ) { nbc=3; goto utf8_n; }
        // ????
        text+=1;
        continue;
utf8_n:
        for (i=0,text+=1 ; i<nbc ; i+=1) {
            c= (*text)&0xff;
            if ( (c&0xC00)==0x80) { text+=1; continue; } // 10xxxxxx
            break;
        }
        if ( i==nbc )
            ret+=1;  // former loop ended normally
        else {       // former loop was broken (not UTF-8 n char)
            if ( c!=0 )
                text+=1;
        }
    }
    return ret;
}

/*======================================================================*/

extern void cs_conv_La15toUtf8_chg(char**str);
extern void cs_conv_Utf8toLa15_chg(char**str);

/*======================================================================*/
