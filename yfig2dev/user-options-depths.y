%{
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
 *$Software: YFIG2DEV
 *      $Id: yfig-main.c 329 2018-10-30 21:01:49Z ia $
 * $HeadURL: svn://lunix120.ensiie.fr/yfig/trunk/yfig/common/yfig-main.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Parsing user data describing depths.                               =*/
/*======================================================================*/

#include "yfig2dev-config.h"

/*======================================================================*/

static int  yylex(char**ptr);
static void yyerror(int*depths,int depthn, char**strcur, const char* msg);

Inline void VRG(int i, int j)
{
    if ( i>j ) {
        TstrEaten astr = cmdline_aswitch2str(&userParams.depths,0);
        tool_errerr_printf(NULL,"%d:%d is an invalid range for switch: %s",i,j,astr);
        tool_ftlerr_printf(NULL,"Did you mean %d:%d ?",j,i);
      //tool_free(astr);
    }
}
%}

%parse-param {int*depths} {int depthn} {char**strcur}
%lex-param   {char**strcur}

%union {
    int       num;
    TsintSet* set;
}

%token<num> TK_INT
%token      TK_InvChar
%type<set>  set

%%
    start
      : '+' { memset(depths,0,depthn*sizeof(depths[0])); } set { int i; for (i=0;i<$3->eleNb;i+=1) depths[$3->eles[i]]=1; sintset_delete($3); }
      | '-' { memset(depths,1,depthn*sizeof(depths[0])); } set { int i; for (i=0;i<$3->eleNb;i+=1) depths[$3->eles[i]]=0; sintset_delete($3); }
;

set : set ',' TK_INT              { int i=$3; VRG(i, i); $$=$1;                                 sintset_add($$,i); }
    | set ',' TK_INT ':' TK_INT   { int i=$3; VRG(i,$5); $$=$1;               for (;i<=$5;i+=1) sintset_add($$,i); }
    | TK_INT                      { int i=$1; VRG(i, i); $$=sintset_new(100);                   sintset_add($$,i); }
    | TK_INT ':' TK_INT           { int i=$1; VRG(i,$3); $$=sintset_new(100); for (;i<=$3;i+=1) sintset_add($$,i); }
;

%%

static int yylex(char**ptr)
{
    yylval.num=0;
//printf("0:%s: %15s\n",__func__,*ptr);
    char c=**ptr;

    while (c==' ' || c=='\t' || c=='\r' || c=='\n') { *ptr+=1; c=**ptr; }
    if ( c==0 )  return c; // EOF
    *ptr+=1;
    if ( c=='+') return c;
    if ( c=='-') return c;
    if ( c==',') return c;
    if ( c==':') return c;
    if ( '0'<=c && c<='9' ) {
        while ( 1 ) {
            yylval.num = yylval.num*10 + c-'0';
            c=**ptr;
            if ( ! ('0'<=c && c<='9') ) break;
            *ptr+=1;
        }
//printf("n:%s: %15s %d\n",__func__,*ptr,yylval.num);
        return TK_INT;
    }
//printf("e:%s: %15s\n",__func__,*ptr);
    return TK_InvChar;
}

void yyerror(int*depths,int depthn, char**strcur, const char* msg)
{
    tool_ftlerr_printf(NULL,"bad formatted -D option : %s",*strcur);
}
