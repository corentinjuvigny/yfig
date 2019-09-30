%{
/*======================================================================
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: f1.lex 102 2017-12-05 18:37:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/charset-table-filter/f1.lex $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/**
 * parse from stdin files like unicode-html+ps.dat and writes to stdout
 * a unicode table.
 *
 * sh> flex f1.lex && gcc -I ../../include lex.yy.c && ./a.out < unicode-html+ps.dat > 1.c
**/

#include "../tools/charset-table-generate.c"

#define F1_DEBUG 0

void makeline(char*strs[], int nstr, int ncode, int firstcode, int*colsep);
%}

%option noyywrap
%option yylineno

%%
 int col=0,code=0,firstcode=0;
 char* strs[100];
 int   colsep[100];
 memset(colsep,0,sizeof(colsep));
FIN-FIN-FIN\n  {return 0; }
\&#[^ \n]+\;   { if (code==0) firstcode=col; strs[col++]=strdup(yytext); code++; }
\&[^ \n]+\;    { if (code==0) firstcode=col; strs[col++]=strdup(yytext); code++; }
[^ \n]+        { if (code==0) firstcode=col; strs[col++]=strdup(yytext); }
\n             { int i;
                 if (col!=0) {
                    makeline(strs,col,code,firstcode,colsep); 
                    for (i=0;i<col;i++) free(strs[i]);
                    col=0;
                    code=0;
                    firstcode=0;
                    memset(colsep,0,sizeof(colsep));
               } }
[ ]+           { colsep[col]=yyleng; }
"//".*\n       ;

%%

#define NB_NAMES 25

typedef struct _Tuni {
    int   ucode;
    char* utf8;
    char* htmlname;
    int   npsglyph;
    char* psglyphs[NB_NAMES];;
    char* comment;
} Tuni;

#if F1_DEBUG == 0

#define NB_UNICODES 0x100000
Tuni table[NB_UNICODES];

#define print(x) while (0)

#else
void print(Tuni*curr)
{
    int i;
    printf("%5d ",curr->ucode);
    printf("/* %s */ ",curr->utf8 ? curr->utf8 : " ");
    printf("%-30s ",curr->htmlname ? curr->htmlname : "");
    if ( curr->npsglyph==0 ) {
        printf("%30s ","");
    } else {
      for (i=0 ; i<curr->npsglyph ; i++)
        printf("%-30s ",curr->psglyphs[i]);
    }
    printf("\n");
}
#endif

int getUnicode(char* deci, char*hexa)
{
    int idec = atoi(deci+2);
    int ihex; sscanf(hexa+3,"%x",&ihex);
    if ( idec!=ihex )
        fprintf(stderr,"%d: found different ucode %s=%x != %s=%d\n",
                yylineno,deci,idec,hexa,ihex);
    return idec;
}

void add_utf8(Tuni*curr, int firstcode, char** strs)
{
    if ( firstcode==0 ) {
        // no UTF8
    } else if ( firstcode==1 ) {
        curr->utf8 = strdup(strs[0]);
    } else if ( firstcode==2 ) {
        if ( strcmp(strs[0],strs[1])!=0 ) {
            fprintf(stderr,"%d: found 2 different UTF8 value (%s))\n",yylineno,strs[0]);
        }
        curr->utf8 = strdup(strs[0]);
    } else if ( firstcode==3 ) {
        if ( strcmp(strs[0],strs[1])!=0 ) {
            fprintf(stderr,"%d: found 2 different UTF8 value (%s))\n",yylineno,strs[0]);
        } else if ( strcmp(strs[1],strs[2])!=0 ) {
            fprintf(stderr,"%d: found 2 different UTF8 value (%s))\n",yylineno,strs[0]);
        }
        curr->utf8 = strdup(strs[0]);
    } else {
        fprintf(stderr,"%d: nc==3 and firstcode > 2 (line is ignored)\n",yylineno);
        return ;
    }
}

void add_htmlname(Tuni*curr, char* n)
{
    curr->htmlname = strdup(n);
}

void add_psglyph(Tuni*curr, char* n)
{
    if ( curr->npsglyph==NB_NAMES )
        fprintf(stderr,"%d: too many PS glyphs\n",yylineno);
    else
        curr->psglyphs[curr->npsglyph++] = strdup(n);
}

void add_comment(Tuni*curr, char**strs, int nstr)
{
}

Tuni* pcurr=0;

void new_pcurr(int ucode)
{
    if ( pcurr!=0 )
        print(pcurr);
#if F1_DEBUG == 0
    pcurr = table+ucode;
#else
static Tuni curr;
    pcurr=&curr;
    memset(pcurr,0,sizeof(*pcurr));
#endif
}

void makeline(char*strs[], int nstr, int ncode, int firstcode, int *colsep )
{
    int n;
    if ( nstr==0 ) return;


    if ( ncode==0 ) {
        // strs[0] is probably an alias
        add_psglyph(pcurr,strs[0]);
        add_comment(pcurr,strs+1,nstr-1);
    } else if ( ncode==2 ) {
        // decimal code, hexadecimal code and no HTML name
        n = getUnicode(strs[firstcode], strs[firstcode+1]);
        new_pcurr(n);
        pcurr->ucode = n;
        add_utf8(pcurr,firstcode,strs); 
        n = firstcode + ncode;
        if ( colsep[n]<40 ) {
            add_psglyph(pcurr,strs[n]);
            add_comment(pcurr,strs+(n+1),nstr-(n+1));
        } else {
//printf("C2: %d %s %d %s %d\n", colsep[n-1],strs[n-1], colsep[n-0],  strs[n-0],colsep[n+1]);
            fprintf(stderr,"%d: assuming \"%s\" is not a PS glyph name.\n",
                    yylineno,strs[n]);
            add_comment(pcurr,strs+n,nstr-n);
        }
    } else if ( ncode==3 ) {
        // decimal code, hexadecimal code and HTML name
        n = getUnicode(strs[firstcode], strs[firstcode+1]);
        new_pcurr(n);
        pcurr->ucode = n;
        add_utf8(pcurr,firstcode,strs); 
        add_htmlname(pcurr,strs[firstcode+2]);
        n = firstcode + ncode;
        if ( colsep[n]<20 ) {
            add_psglyph(pcurr,strs[n]);
            add_comment(pcurr,strs+(n+1),nstr-(n+1));
        } else {
//printf("C3: %d %s %d %s %d\n", colsep[n-1],strs[n-1], colsep[n-0],  strs[n-0],colsep[n+1]);
            fprintf(stderr,"%d: assuming \"%s\" is not a PS glyph name.\n",
                    yylineno,strs[n]);
            add_comment(pcurr,strs+n,nstr-n);
        }
    } else {
        fprintf(stderr,"%d: unexpected ncode %d (expected 0 or 3)\n",yylineno,ncode);
        return ;
    }
}

int main()
{
    if ( yylex()!=0 ) {
        fprintf(stderr,"parse failure\n");
        return 1;
    }
#if F1_DEBUG == 0
    int i,j;
    Tuni* u;

    int codemin=NB_UNICODES,codemax=0;
    for ( i=0 ; i<NB_UNICODES ; i++ ) {
        u = table+i;
        if ( u->ucode==0 ) continue;
        if ( u->ucode<codemin )
            codemin =  u->ucode;
        else if ( u->ucode>codemax )
            codemax =  u->ucode;
    }
//printf("codemin=%d codemax=%d\n",codemin,codemax);

    codemax+=1;
    TcodeTable t;
    t.mincode = codemin;
    t.maxcode = codemax;
    t.eles    = malloc( sizeof(*t.eles)*(codemax-codemin) );
    for ( i=0 ; i<NB_UNICODES ; i++ ) {
        u = table+i;
        if ( u->ucode==0 ) continue;
        TcodeEle* e = calloc( sizeof(*e), 1);
        e->ucode     = u->ucode;
        e->utf8      = u->utf8;
        e->htmlname  = u->htmlname;
        e->npsglyph  = u->npsglyph;
        if ( u->npsglyph==1 )
            e->psglyphs  = *u->psglyphs;
        else
            e->psglyphs  = (char*)u->psglyphs;
        e->comment   = u->comment;
        t.eles[e->ucode-t.mincode] = e;
    }

    cs_codtable_genHeader(stdout);

    cs_codtable_genTableEles(stdout,&t,"unicodes",
                    "unicode elements of unicode table");

    cs_codtable_genTableDef(stdout,&t,"unicodes",
                    "table of recognized unicodes" );
#endif
    fprintf(stderr,"parse succes\n");
    return 0;
}
