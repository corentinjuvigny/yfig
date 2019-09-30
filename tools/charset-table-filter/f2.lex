%{
/*======================================================================
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: f2.lex 102 2017-12-05 18:37:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset-table-filter/f2.lex $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/**
 * parse from stdin files like zapdingbats.dat, merges the inputs
 * into the current unicode table and finally,
 *  - writes into the stdout stream the merged unicode table,
 *  - writes into the stdout stream a code table from zapdingbats to unicode.
 *
 * sh> flex f2.lex && gcc -I ../../include -I .. lex.yy.c && ./a.out < zapdingbats.dat > 2.c
**/

#include "../tools/charset-table-generate.c"

#include "1.c"

%}

%option noyywrap
%option yylineno
%x p1 p2

%%
 int i;
 TcodeEle* e;
 char* str;
 int   ucode,zcode;
<p1>\;       ;
<p1>[0-9a-fA-F][0-9a-fA-F]*[\ ]*\n {
            sscanf(yytext,"%x",&ucode);
            if ( ucode<unicodes.mincode || ucode>=unicodes.maxcode ) {
                fprintf(stderr,"%d unicode out of range [%d:%d[\n",
                        ucode,unicodes.mincode,unicodes.maxcode);
            } else {
                int idx = ucode-unicodes.mincode;
                e = unicodes.eles[idx];
                if ( e==0 ) {
                    // fprintf(stderr,"add %d unicode in table\n",ucode);
                    e = calloc( sizeof(*e), 1); 
                    unicodes.eles[idx] = e;
                } else {
                    fprintf(stderr,"%d:merge %d unicode in table\n",yylineno,ucode);
                }
                e->ucode = CS_UCODE_SET(ucode,CS_FL_ZapfDingbats);
                char tmp[100]; sprintf(tmp,"u%06X",ucode);
                if ( e->pdfglyph!=0 && strcmp(tmp,e->pdfglyph)!=0 ) 
                    fprintf(stderr,"%d:merge %d unicode: %s replaced by %s\n",yylineno,ucode,e->pdfglyph,str);
                e->pdfglyph = str;
            }
        }
<p1>[^;\n ]+;  { yytext[yyleng-1]=0; str=strdup(yytext); }
<p1>"#END"\n  { BEGIN p2; }

<p2>[0-7][0-7][0-7] {
        zcode=(yytext[0]-'0')*8*8+(yytext[1]-'0')*8+(yytext[2]-'0'); 
        for (i=unicodes.mincode ; i<unicodes.maxcode ; i++) {
            e = unicodes.eles[i-unicodes.mincode];
            if ( e==0 ) continue;
            if ( CS_FLAG(e->ucode)!=CS_FL_ZapfDingbats ) continue;
            if ( strcmp(e->pdfglyph,str)!=0 ) continue;
            e->zcode = zcode;
            break;
        }
        if ( i==unicodes.maxcode ) 
            fprintf(stderr,"%d: zcode=%d glyp-name=%s not found in table\n",yylineno,zcode,str);
    }
<p2>"I{"[^}]*"}"  ;
<p2>[^;\n ]+      { str=strdup(yytext); }
<p2>"#END-END"\n  { return 0; }
<p2>" "           ;
<p2>\n            ;

<p1,p2>"#".*\n    ;
%%

int main()
{
    // adding manually the space gliph
    TcodeEle* e = unicodes.eles[32-unicodes.mincode];
    e->ucode = CS_UCODE_SET(32,CS_FL_ZapfDingbats);
    e->pdfglyph = strdup( "space" );

    // update unicodes
    BEGIN p1;
    if ( yylex()!=0 ) {
        fprintf(stderr,"parse failure\n");
        return 1;
    }

    // generate the ZapfDingBats encoding table
    int i,zcode;
    TcodeTable zdb;
    zdb.mincode = 0;
    zdb.maxcode = 256;
    zdb.eles    = calloc(sizeof(*zdb.eles),256);
    for (i=unicodes.mincode ; i<unicodes.maxcode ; i++) {
        e = unicodes.eles[i-unicodes.mincode];
        if ( e==0 ) continue;
        if ( CS_FLAG(e->ucode)!=CS_FL_ZapfDingbats ) continue;
        zcode = e->zcode;
        if ( zcode!=0 )
            zdb.eles[zcode-zdb.mincode] = e;
        else
            fprintf(stderr,"zcode for u+%d is null (%s)\n",i,e->pdfglyph);
    }

    // generate outputs
    cs_codtable_genHeader(stdout);

    cs_codtable_genTableEles(stdout,&unicodes,"unicodes",
                    "unicode elements of unicode table");

    cs_codtable_genTableDef(stdout,&unicodes,"unicodes",
                    "table of recognized unicodes" );

    cs_codtable_genTableDef(stdout,&zdb,"zapfDingBats",
                    "table of zapfDingBats font" );

    fprintf(stderr,"parse succes\n");
    return 0;
}
