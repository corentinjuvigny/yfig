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
 *      $Id: string-builder.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/string-builder.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= see the string-builder.h header for documentation.                 =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/string-builder.h"

/*======================================================================*/

static void strfmt_rawwrite(Tstrfmt*str, const void* data, int datalen)
{
    int newlen = str->bytesize + datalen ;
    str->str = tool_realloc(str->str,newlen);
    memcpy(str->str+str->bytesize,data,datalen);
    str->bytesize += datalen;
}

static void strfmt_appendFooter(Tstrfmt*str)
{
    if ( str->footlen<0 ) return;
    if ( str->footlen==0 ) {
        if ( str->bytesize>0 && str->str[str->bytesize-1]!='\n') {
            strfmt_rawwrite(str,"\n",2);
            str->bytesize -= 2;
        }
    } else {
        strfmt_rawwrite(str,str->foot,str->footlen+1);
        str->bytesize -= str->footlen+1;
    }
}

/*======================================================================*/
/*= Initialization and freeing.                                        =*/

extern void     strfmt_iniLSILF(Tstrfmt*str,
                              int initlen, int shift, int nbItemByLine)
{
    memset(str,0,sizeof(*str));
    str->shift = shift;
    str->nbItemByLine = nbItemByLine;
    //str->foot[0] = '\n';
    //strfmt_appendFooter(str);
    //str->footlen = 1;
    strfmt_restart(str);
}

extern Tstrfmt* strfmt_newLSILF(int initlen, int shift, int nbItemByLine)
{
    Tstrfmt* str = tool_malloc(sizeof(*str));
    strfmt_iniLSILF(str,initlen,shift,nbItemByLine);
    return str;
}

extern void     strfmt_free  (Tstrfmt*str)
{ if ( str!=0 && str->str!=0 ) free(str->str); }

extern void     strfmt_delete(Tstrfmt*str)
{ if (str==0) return; strfmt_free(str); free(str); }

extern void     strfmt_restart(Tstrfmt*str)
{
    if (str->str!=0 ) { free( str->str ); str->str=0; }
    str->bytesize = 0;
    str->footlen = strlen(str->foot);
    str->nbItem  = 0;
    strfmt_rawwrite(str,"",1);
    str->bytesize = 0;
    //strfmt_appendFooter(str);
}

/*======================================================================*/
/*= building the string.                                               =*/

extern void strfmt_write    (Tstrfmt*str, const void* data, int datalen)
{
    strfmt_rawwrite(str,data,datalen);
    strfmt_appendFooter(str);
}

extern void strfmt_writeLF     (Tstrfmt*str); // ????

extern void strfmt_writeItem(Tstrfmt*str, const void* data, int datalen)
{
    int i;
    if ( str->nbItemByLine<=0 ) {
        strfmt_write(str,data,datalen);
        return;
    }
    if ( str->nbItem==str->nbItemByLine ) {
        strfmt_rawwrite(str,"\n",2);
        str->bytesize -= 1;
        str->nbItem = 0;
    }
    if ( str->nbItem==0 ) {
        for (i=0 ; i<str->shift ; i+=1) 
            strfmt_rawwrite(str," ",1);
    }
    str->nbItem += 1;
    strfmt_rawwrite(str,data,datalen);
    strfmt_appendFooter(str);
}

extern void strfmt_writeStr    (Tstrfmt*str, CstrBrief data)
{ strfmt_write(str,data,strlen(data)); }

extern void strfmt_writeStrItem(Tstrfmt*str, CstrBrief data)
{ strfmt_writeItem(str,data,strlen(data)); }

extern void strfmt_printf     (Tstrfmt*str, CstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    strfmt_vprintf(str,fmt,ap);
    va_end(ap);
}

extern void strfmt_printfItem (Tstrfmt*str, CstrBrief fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    strfmt_vprintfItem(str,fmt,ap);
    va_end(ap);
}

extern void strfmt_vprintf    (Tstrfmt*str, CstrBrief fmt, va_list ap)
{
    char *data = tool_vaprintf(fmt,ap);
    strfmt_writeStr(str,data);
    free( data );
}

extern void strfmt_vprintfItem(Tstrfmt*str, CstrBrief fmt, va_list ap)
{
    char *data = tool_vaprintf(fmt,ap);
    strfmt_writeStrItem(str,data);
    free( data );
}


/*======================================================================*/
