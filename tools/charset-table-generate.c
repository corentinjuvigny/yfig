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
 *      $Id: charset-table-generate.c 102 2017-12-05 18:37:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset-table-generate.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for generating the encoding tables (TcodeTable).         =*/
/*=                                                                    =*/
/*= This file is not a part of the libtool.a library.                  =*/
/*=                                                                    =*/
/*= It is included by the programms that help to generate the encoding =*/
/*= tables (see charset-table-filter directory).                       =*/
/*======================================================================*/

#include "tools-config.h"
#include "charset-table.h"

/*======================================================================*/

#define ELE_NAME "uni_%06d"

static void cs_codtable_genHeader(FILE* stream)
{
    fprintf(stream,
"/*======================================================================\n"
" *  Copyright 2017 Ivan Augé\n"
" *\n"
" *  This file is part of the YFIG software.\n"
" * \n"
" *  YFIG is free  software;  you  can redistribute it and/or modify it\n"
" *  under the terms of the GNU  General Public License as published by\n"
" *  the  Free Software Foundation; either version 3 of the License, or\n"
" *  (at your option) any later version.\n"
" * \n"
" *  YFIG software is distributed  in the  hope that it will be useful,\n"
" *  but WITHOUT ANY WARRANTY;  without even the  implied  warranty  of\n"
" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  the GNU\n"
" *  General Public License for more details.\n"
" * \n"
" *  You should have received  a copy of the GNU General Public License\n"
" *  along with YFIG. If not, see <http://www.gnu.org/licenses/>.\n"
"**======================================================================\n"
" *$Software: YFIG-TOOL (utilities shared by the different modules)\n"
" *      $%s: charset.h 99 2017-11-23 07:07:57Z ia $\n"
" * $%sadURL: svn://lunix120.ensiie.fr/yfig/trunk/include/tools/charset.h $\n"
" *  $%sthor: generated by filters of tools/charset-table-filter directory\n"
"**======================================================================*/\n"
"\n"
"/*======================================================================*/\n"
"/*= Encoding tables (TcodeTable) for charset conversions.              =*/\n"
"/*======================================================================*/\n"
        , "Id", "He", "Au"
    );
    fprintf(stream,
        "\n"
        "#include \"tools-config.h\"\n"
        "#include \"charset-table.h\"\n"
        "\n"
        "#ifndef NULL\n"
        "#define NULL ((void*)0)\n"
        "#endif\n"
        "\n"
    );
}

static void cs_codtable_genTableEles(FILE* stream, TcodeTable* t,
                          CstrBrief c_tname, CstrBrief c_comment)
{
    TcodeEle* e;
    int i,j,ucode;

    fprintf(stream,
        "\n"
        "// %s\n"
        "\n"
        , c_comment ? c_comment : "table elements"
    );

    for ( i=t->mincode ; i<t->maxcode ; i++) {
        e = t->eles[i-t->mincode];
        if ( e==0 ) continue;
        ucode = CS_UNICODE(e->ucode);
        // psglyphs array if needed 
        if ( e->npsglyph>1 ) {
            fprintf(stream,"static char* psglyphs_u%06d[] = { ",ucode);
            for ( j=0 ; j<e->npsglyph ; j++) {
                char** glyphs = (char**)e->psglyphs;
                if ( j!=0 ) fprintf(stream,",");
                char tmp[100];
                sprintf(tmp," \"%s\"", glyphs[j]);
                fprintf(stream," %-25s", tmp);
            }
            fprintf(stream," };\n");
        }

        // prepare code element strings
        char htmlname[100];
        if ( e->htmlname ) 
            sprintf(htmlname,"\"%s\"",e->htmlname);
        else
            strcpy(htmlname,"NULL");

        char pdfglyph[100];
        if ( e->pdfglyph==0 )
            sprintf(pdfglyph,"\"u%06X\"",ucode);
        else
            sprintf(pdfglyph,"\"%s\"",e->pdfglyph);

        char psglyphs[100];
        if ( e->npsglyph==0 ) 
            strcpy(psglyphs,"NULL");
        else if ( e->npsglyph==1 ) 
            sprintf(psglyphs,"\"%s\"",e->psglyphs);
        else
            sprintf(psglyphs,"(char*)psglyphs_u%06d",ucode);

        // print the code element
        fprintf(stream,"static TcodeEle " ELE_NAME " = {",ucode);
        fprintf(stream," .ucode=0x%08X",e->ucode); 
        fprintf(stream," , .zcode=0x%02X",e->zcode); 
        fprintf(stream," , .htmlname=%-20s",htmlname); 
        fprintf(stream," , .pdfglyph=%-20s",pdfglyph); 
        fprintf(stream," , .npsglyph=%2d"  ,e->npsglyph); 
        fprintf(stream," , .psglyphs=%-25s",psglyphs); 
        fprintf(stream," };\n");
    }
}

static void cs_codtable_genTableDef(FILE* stream, TcodeTable* t,
                          CstrBrief c_tname, CstrBrief c_comment)
{
    int i,ucode;
    TcodeEle* e;

    fprintf(stream,
        "\n"
        "// %s \n"
        "\n"
        "#define CS_NBELES_%s (%d)\n"
        "TcodeEle* %s_eles[CS_NBELES_%s] = {"
        , c_comment ? c_comment : c_tname
        , c_tname , t->maxcode-t->mincode
        , c_tname , c_tname
    );

    char* sep = "  ";
    char temp[100];
    for ( i=t->mincode ; i<t->maxcode ; i++) {
        e = t->eles[i-t->mincode];
        if ( e==0 ) 
            sprintf(temp,"/*u+%06d*/ NULL",i);
        else {
            ucode = CS_UNICODE(e->ucode);
//           if ( ucode!=i ) {
//               fprintf(stderr,"%s: ucode missmatch %d and %d\n",
//                       __func__,ucode,i);
//           }
            sprintf(temp,"&" ELE_NAME,ucode);
        }
        if ( ((i-t->mincode)%8)==0 )
            fprintf(stream,"\n   ");
        fprintf(stream,"%s%-20s",sep,temp);
        sep=" ,";
    }

    fprintf(stream,
        "};\n"
        "\n"
        "// Finally the %s table \n"
        "\n"
        "TcodeTable     %s = { %d, %d, %s_eles };\n"
        , c_tname
        , c_tname , t->mincode, t->maxcode, c_tname
    );
    fprintf(stream,
        "TcodeTable* gl_%s = &%s;\n"
        , c_tname , c_tname
    );
}

/*======================================================================*/
