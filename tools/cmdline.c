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
 *      $Id: cmdline.c 369 2018-11-19 12:35:31Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/cmdline.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for parsing the command line: main(argc,argv).           =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/globalvars.h"
#include "tools/strings.h"
#include "tools/cmdline.h"

struct _Tsw { SW_FIELD(); };
 
/*======================================================================*/

#define TPLSET ptrsw
#define TPLSET_EleType Tsw*
#define TPLSET_EleCype const Tsw*
#define TPLSET_AddWithoutPtr
#include "tools/set-body-template.h"

#define V(s) (s)?(s):""

// 1: if sw has no arg or optionnal arg
// 0: sw has at least 1 arg.
static int sw_mayHaveNoArg(Csw*sw);

// 1: if sw value is value.str
// 0: otherwise
static int sw_isSwWithStrVal(Csw*sw);

// for strings that are was static (or allocated by an other module)
static void*     cmdline_malloc (Tcmdline* cl, int sz);
static TstrEaten cmdline_strdup (Tcmdline* cl, CstrBrief str);
static void      cmdline_freeptr(Tcmdline* cl, void* p);

Inline TstrEaten sw_strdup     (Tsw* sw, CstrBrief str) { return cmdline_strdup(sw->cmdline,str); }
static void      sw_freeptr    (Tsw* sw, void*     p)   { cmdline_freeptr(sw->cmdline,p); }

/*======================================================================*/
/*= Interface to Tsw                                                =*/

extern char* cmdline_switch2str(char*buf, Csw*sw,char quote)
{
    char*fmt1 = "%s%s%s";
    char*fmt2 = "%s%s=%s%s";
    char*fmt3 = "%s%s %s%s";
    char quotestr[2]; quotestr[0]=quote>0x20 ? quote : 0; quotestr[1]=0;
    if ( sw->arswitch==0 ) {
       sprintf(buf,fmt1, quotestr, sw->shswitch ? sw->shswitch : sw->lgswitch, quotestr);
    } else if ( sw->shswitch!=0 ) {
       sprintf(buf,fmt3, quotestr, sw->shswitch, sw->arswitch, quotestr);
    } else
       sprintf(buf,fmt2, quotestr, sw->lgswitch, sw->arswitch, quotestr);
    return buf;
}

extern char* cmdline_aswitch2str(Csw*sw,char quote)
{
    char tmp[1000];
    if ( ! cmdline_isSwSet(sw) ) {
        return tool_strdup( cmdline_switch2str(tmp,sw,quote) );
    } else {
        TstrEaten ret;
        char*fmt1 = "%s%s%s";
        char*fmt2 = "%s%s=%s%s";
        char*fmt3 = "%s%s %s%s";
        char quotestr[2]; quotestr[0]=quote>0x20 ? quote : 0; quotestr[1]=0;
        char* arg;
        switch ( sw->argfmt ) {
            case SWAF_None   : arg = 0; break;
            case SWAF_StrOpt : arg = cmdline_getStr(sw); break;
            case SWAF_Str    : arg = cmdline_getStr(sw); break;
            case SWAF_Int    : arg = tmp; sprintf(tmp,"%d",  cmdline_getInt(sw)); break;
            case SWAF_Dble   : arg = tmp; sprintf(tmp,"%.2f",cmdline_getDle(sw)); break;
            default          : arg = sw->arswitch;  break;
        }
        if ( arg==0 ) {
           ret = tool_aprintf(fmt1, "", sw->shswitch ? sw->shswitch : sw->lgswitch, "");
        } else if ( sw->shswitch!=0 ) {
           ret = tool_aprintf(fmt3, quotestr, sw->shswitch, arg, quotestr);
        } else
           ret = tool_aprintf(fmt2, quotestr, sw->lgswitch, arg, quotestr);
        return ret;
    }
}

extern int    cmdline_isSwSet(Csw*sw) { return sw->set!=0;      }
extern int    cmdline_getInt  (Csw*sw) { return sw->value.sint;  }
extern double cmdline_getDle  (Csw*sw) { return sw->value.dreal; }
extern char*  cmdline_getStr  (Csw*sw) { return sw->value.str;   }

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern int   *cmdline_getIntM  (           Tsw*sw,...);
extern double*cmdline_getDleM  (           Tsw*sw,...);
extern Tstr  *cmdline_getStrM  (           Tsw*sw,...)
{
    int i;
    Tsw* sws[100];
    int     swn;

    // get all the switches and put them in sws/swn
    va_list ap;
    va_start(ap,sw);
    for (swn=0 ; sw!=0 ; sw=va_arg(ap,Tsw*) ) {
        TEMP_asf_printf( !sw_isSwWithStrVal(sw), "%d switch has not string value",i);
        sws[swn++]=sw;
    }
    va_end(ap);

//   1) thus of the first sw that is set
    for (i=0 ; i<swn ; i+=1)
        if ( cmdline_isSwSet(sws[i]) ) goto found;

//   2) thus of the first sw that is tagged with SWOK_IsDefSW.
    for (i=0 ; i<swn ; i+=1)
        if ( (sws[i]->optkind&SWOK_IsDefSW)!=0 ) goto found;

//   3) thus of the first sw that is tagged with SWOK_HasDefVal.
    for (i=0 ; i<swn ; i+=1)
        if ( (sws[i]->optkind&SWOK_HasDefVal)!=0 ) goto found;

    return NULL;

found:
    return &sws[i]->value.str;
}

extern int    cmdline_getIntMdv(int    dv, Tsw*sw,...);
extern double cmdline_getDleMdv(double dv, Tsw*sw,...);
extern Tstr   cmdline_getStrMdv(char*  dv, Tsw*sw,...);

/*======================================================================*/
/*= Private utilities                                                  =*/

static void cmdl_printHelp(Tcmdline*cl)
{
    Tsw* pe;
    int i, opt, l, line_nb=TOOL_TABLE_NbEle(pe->doc);

  /* header */
    printf("\n");
    if ( cl->cl_prgargs1 ) printf("%6s %s [OPTIONS] %s\n", "usage:" , gl_progname, cl->cl_prgargs1);
    if ( cl->cl_prgargs2 ) printf("%6s %s [OPTIONS] %s\n", ""       , gl_progname, cl->cl_prgargs2);
    if ( cl->cl_prgargs3 ) printf("%6s %s [OPTIONS] %s\n", ""       , gl_progname, cl->cl_prgargs3);

    for (i=0 ; cl->cl_hhelp[i] ; i++) {
        if (i==0) printf("\n");
        printf(cl->cl_hhelp[i],gl_progname);
        printf("\n");
    }

  /* options */
    printf("\nOPTIONS:\n");

    int  switchlen=30;
    char sepchar,linefmt[1000],switchbuf[1000];
    sprintf(linefmt," %%-%ds%%c%%s\n",switchlen);
    for (opt=0; opt<cl->cl_switches->eleNb ; opt+=1) {
        pe = cl->cl_switches->eles[opt];
        for ( l=0 ; l<line_nb && pe->doc[l]!=0 ; l+=1) {
            if (l==0) { // first line
                char*switchfmt1 = "%5s, %s";
                char*switchfmt2 = "%5s, %s=%s";
                if (pe->argfmt==SWAF_None) 
                    sprintf(switchbuf,switchfmt1,V(pe->shswitch),V(pe->lgswitch));
                else
                    sprintf(switchbuf,switchfmt2,V(pe->shswitch),V(pe->lgswitch),V(pe->arswitch));
                sepchar=':';
            } else {
                    switchbuf[0]=0;
                    sepchar=' ';
            }
            if ( strlen(switchbuf)<switchlen )
                printf(linefmt,switchbuf,sepchar,pe->doc[l]);
            else {
                printf(linefmt,switchbuf,sepchar,"");
                printf(linefmt,""       ,' '    ,pe->doc[l]);
            }
        }
    }
}

// 1: if sw has no arg or optionnal arg
// 0: sw has at least 1 arg.
static int sw_mayHaveNoArg(Csw*sw)
{
    if ( sw->argfmt == SWAF_None   ) return 1;
    if ( sw->argfmt == SWAF_StrOpt ) return 1;
    return 0;
}

// 1: if sw value is value.str
// 0: otherwise
static int sw_isSwWithStrVal(Csw*sw)
{
    if (  sw->argfmt == SWAF_Str         ) return 1;
    if (  sw->argfmt == SWAF_StrOpt      ) return 1;
    if ( (sw->optkind&SWOK_ValTypStr)!=0 ) return 1;
    return 0;
}

static void*     cmdline_malloc(Tcmdline* cl, int sz)
{
    void* p = tool_malloc(sz);
    astrset_addEaten(cl->cl_mallocedstr,p);
    return p;
}

static TstrEaten cmdline_strdup(Tcmdline* cl, CstrBrief str)
{
    if ( str==0 ) return 0;
    char* clone = cmdline_malloc( cl, strlen(str)+1 );
    strcpy(clone,str);
    return clone;
}

static void      cmdline_freeptr(Tcmdline* cl, void* p)
{
    int pos;
    if ( p==0 ) return;
    if ( (pos=astrset_getPos(cl->cl_mallocedstr,p))>=0 )
        astrset_supQuick(cl->cl_mallocedstr,pos);
}

/*======================================================================*/

static void sw_iniIntValByStr(Tsw*sw, CstrBrief str)
{
    char tmp[100]; cmdline_switch2strDQ(tmp,sw);
    int val;
    int minval= sw->valmin.sint;
    int maxval= sw->valmax.sint;

    if ( tool_strtosint32(&val,str)<0 )
        tool_ftlerr_printf(NULL, "%s switch requires an integer value (found: %s)\n", tmp,str);
    if ( (sw->optkind&SWOK_MinVal)!=0 && val<minval) {
        tool_errerr_printf(NULL,"set value of %s switch to %d (found %d; expected in [%d:%d])",tmp,minval,val,minval,maxval);
        val = minval;
    } 
    if ( (sw->optkind&SWOK_MaxVal)!=0 && val>maxval) {
        tool_errerr_printf(NULL,"set value of %s switch to %d (found %d; expected in [%d:%d])",tmp,maxval,val,minval,maxval);
        val = maxval;
    } 
    sw->value.sint=val;
}

static void sw_iniDleValByStr(Tsw*sw, CstrBrief str)
{
    char tmp[100]; cmdline_switch2strDQ(tmp,sw);
    double val;
    double minval= sw->valmin.dreal;
    double maxval= sw->valmax.dreal;

    if ( tool_strtodouble(&val,str)<0 )
        tool_ftlerr_printf(NULL, "%s switch requires a real value (found: %s)\n", tmp,str);
    if ( (sw->optkind&SWOK_MinVal)!=0 && val<minval) {
        tool_errerr_printf(NULL,"set value of %s switch to %.2f (found %.2f; expected in [%.2f:%.2f])",tmp,minval,val,minval,maxval);
        val = minval;
    } 
    if ( (sw->optkind&SWOK_MaxVal)!=0 && val>maxval) {
        tool_errerr_printf(NULL,"set value of %s switch to %.2f (found %.2f; expected in [%.2f:%.2f])",tmp,maxval,val,minval,maxval);
        val = maxval;
    } 
    sw->value.dreal=val;
}

/*======================================================================*/

// It prints an error message and exists
//   - if pe is already set and if it can be present only once.
static void optcheck_multSet(Csw*pe)
{
    char tmp[100];
    if (pe->set!=0 && (pe->optkind&SWOK_Several)==0 ) {
        tool_ftlerr_printf(NULL,"%s switch already set.\n",cmdline_switch2str(tmp,pe,'"'));
    }
}

// It prints an error message and exists
//   - if there are not at least nba args from argv[idx] included
//     ( argv[idx] & argv[idx+1] & ... &  argv[idx+nba-1] != 0 )
static void optcheck_missingArg(Csw*pe, char*const* argv, int idx, int nba)
{
    char swstr[100]; cmdline_switch2str(swstr,pe,'"');
    int i;
    for (i=0 ; i<nba ; i+=1) {
        if ( argv[idx+i]==0 ) {
            tool_ftlerr_printf(NULL,"%s switch requires %d parameter%s\n",swstr,nba,nba>1?"s":"");
            return;
        }
    }
}

static void optcheck_multSetAndMissingArgs(Csw*sw, char*const*const argv, int idx, int nba)
{
    optcheck_multSet(sw);
    optcheck_missingArg(sw,argv,idx,nba);
}

// returns
//   1: if argv[idx] may be a switch argument.
//   0: if argv[idx] is null or seems to be a switch.
static int optcheck_multSetAndHas1OptionnalArgs(Csw*sw, char*const*const argv, int idx)
{
    optcheck_multSet(sw);
    Tstr arg = argv[idx];
    if ( arg==0 )      return 0;
    if ( arg[0]=='-' ) return 0;
    if ( arg[0]=='+' ) return 0;
    return 1;
}

// The long format of a switch is: --long-format="aaa bbb"
// If argv[idx] corresponds to the sw switch under long format,
// the function stores the arg value into the sw structure.
// On success it returns 1, otherwise it returns 0.
static int opt_update_lg(Tsw*sw, char*const*const argv, int idx)
{
    char swstr[100];
    char* arg = argv[idx];

    // simple case
    if ( sw->lgswitch==0 )       return 0; // not recognized
    if ( sw->argfmt==SWAF_None ) {
        if ( tool_strcmp(arg,sw->lgswitch)==0 ) { sw->set=1; return 1; }
        else                                                 return 0;
    }

    int  len = strlen(sw->lgswitch);
    if ( strncasecmp(sw->lgswitch,arg,len)!=0 )     return 0; // not recognized
    if ( sw_mayHaveNoArg(sw) && arg[len]==0 )       { sw->set=1; return 1; }
    if ( sw_mayHaveNoArg(sw) && arg[len]=='=' && arg[len+1]==0 )       { sw->set=1; return 1; }
    if ( arg[len]!='=' )                            return 0; // not recognized

    // treatement of argument
    switch ( sw->argfmt ) {
      case SWAF_Int:
        optcheck_multSet(sw);
        sw_iniIntValByStr(sw,arg+len+1);
        sw->set = 1;
        break;
      case SWAF_Str:
      case SWAF_StrOpt:
        optcheck_multSet(sw);
        sw->value.str = tool_strdup(arg+len+1);
        sw->set = 1;
        break;
      case SWAF_Dble:
        optcheck_multSet(sw);
        sw_iniDleValByStr(sw,arg+len+1);
        sw->set = 1;
        break;
      default:
        PERM_spf_printf("0x%x arg format of %s switch format is not yet supported\n",sw->argfmt,sw->lgswitch);
    }
    return 1;
}

// The short format of a switch is: -short aaa bbb
//
// If argv[idx+[0:n[ ] corresponds to the pe switch under short format,
// the function stores the argv value into the pe structure.
// On success it returns n, otherwise it returns 0.
static int opt_update_sh0(Tsw*pe, char*arg, char*const*const argv, int idx);
static int opt_update_sh (Tsw*pe,           char*const*const argv, int idx) { return opt_update_sh0(pe,argv[idx],argv,idx+1); }
static int opt_update_sh0(Tsw*pe, char*arg, char*const*const argv, int idx)
{
    int i;

    if ( pe->shswitch==0 )return 0;

    // special treatment
    if ( (pe->optkind&SWOK_IncDec)==SWOK_IncDec && arg[0]=='-' && tool_strcmp(arg+1,pe->shswitch+1)==0 ) {
        pe->value.sint -= 1;
        if ( (pe->optkind&SWOK_MinVal)!=0 ) pe->value.sint = max2(pe->value.sint, pe->valmin.sint);
        pe->set += 1;
        return 1;
    }
    if ( (pe->optkind&SWOK_IncDec)==SWOK_IncDec && arg[0]=='+' && tool_strcmp(arg+1,pe->shswitch+1)==0 ) {
        pe->value.sint += 1;
        if ( (pe->optkind&SWOK_MaxVal)!=0 ) pe->value.sint = min2(pe->value.sint, pe->valmax.sint);
        pe->set += 1;
        return 1;
    }

    // standard treatment
    if ( strcmp(arg,pe->shswitch)!=0 ) return 0;

    switch ( pe->argfmt ) {
      case SWAF_Int:
        optcheck_multSetAndMissingArgs(pe,argv,idx,1);
        sw_iniIntValByStr(pe,argv[idx]);
        pe->set = 1;
        return 2;
      case SWAF_Str:
        optcheck_multSetAndMissingArgs(pe,argv,idx,1);
        sw_freeptr(pe,pe->value.str);
        pe->value.str = sw_strdup(pe,argv[idx]);
        pe->set = 1;
        return 2;
      case SWAF_Dble:
        optcheck_multSetAndMissingArgs(pe,argv,idx,1);
        sw_iniDleValByStr(pe,argv[idx]);
        pe->set = 1;
        return 2;
      case SWAF_StrOpt:
        pe->set = 1;
        if ( optcheck_multSetAndHas1OptionnalArgs(pe,argv,idx) ) {
            sw_freeptr(pe,pe->value.str);
            pe->value.str = sw_strdup(pe,argv[idx]);
            return 2;
        } else 
            return 1;
      default:
        optcheck_multSet(pe);
        pe->set = 1;
        return 1;
    }
}

// chains opt_update_lg and opt_update_sh
static int opt_update(Tsw*pe, char*const*const argv, int idx)
{
    int nbEated;
    if ( (nbEated=opt_update_lg(pe,argv,idx))!=0 ) return nbEated;
    return opt_update_sh(pe,argv,idx);
}

/*======================================================================*/
/*= Interface.                                                         =*/

extern Tcmdline* cmdline_newSwSet(void*t,int eleSz, int eleNb,
        CstrStatic prgargs1, CstrStatic prgargs2, CstrStatic prgargs3)
{
    int i;
    Tcmdline* cl = tool_malloc_0(sizeof(*cl));

    cl->cl_switches = ptrswset_new(10);
    for (i=0 ; i<eleNb ; i+=1) {
        char* p = t;
        ptrswset_add(cl->cl_switches, (Tsw*) (p+(i*eleSz)) );
        Tsw* sw = cl->cl_switches->eles[i];
        sw->cmdline = cl;
    }
    cl->cl_prgargs1 = prgargs1;
    cl->cl_prgargs2 = prgargs2;
    cl->cl_prgargs3 = prgargs3;
    cl->cl_mallocedstr = astrset_new(100);

    return cl;
}

extern void      cmdline_delete    (Tcmdline*cl)
{
    ptrswset_delete( cl->cl_switches    );
    astrset_delete ( cl->cl_mallocedstr );
    tool_free      ( cl );
}

extern TastrSet* cmdline_parse (Tcmdline*cl, char*const*const argv)
{
    TptrswSet* opts=cl->cl_switches;
    int      iargv,nbeaten, iopt, optn=opts->eleNb;
    Tsw*  pe;
    TastrSet *argv_notrecognized1,*argv_notrecognized2;

  /* first pass look for the help switch only */
    // gets the help switch from set of switch definitions
    for (iopt=0; iopt<optn ; iopt+=1 ) {
        pe = opts->eles[iopt];
        if ( (pe->optkind&SWOK_Help)!=0 ) break;
    }
    if (iopt==optn) goto pass2; // no help switch
    // search for a help switch in cmd-line (pe is the help switch)
    for (iargv=0 ; argv[iargv] ; iargv+=1) {
        opt_update(pe,argv,iargv);
        if ( pe->set ) {
            cmdl_printHelp(cl);
            exit(0);
        }
    }

  /* second pass look for all the switches but not the help switch */
pass2: while(0);
    argv_notrecognized1 = astrset_new(10);
    for (iargv=0 ; argv[iargv] ; iargv+=nbeaten) {
        for (iopt=0; iopt<optn ; iopt+=1 ) {
            pe = opts->eles[iopt];
            if ( (pe->optkind&SWOK_Help)!=0 ) continue;
            if ( (nbeaten=opt_update(pe,argv,iargv))!=0 ) {
                break;
            }
        }
        if (iopt==optn) { // argv[iargv] not recognized 
            astrset_add(argv_notrecognized1,argv[iargv]);
            nbeaten = 1;
        }
    }

  /**
   * third pass look for all the switches (except the help switch)
   * under merged short format.
   *
   * -c ac -d ad1 ad2 --> -cd ac ad1 ad2
  **/
    // shorts: the short switches that can be merged.
    TptrswSet*shorts = ptrswset_new(10);
    for (iopt=0; iopt<optn ; iopt+=1 ) {
        pe = opts->eles[iopt];
        if ( (pe->optkind&SWOK_Help)!=0 ) continue;
        if ( pe->shswitch!=0 )
            ptrswset_add(shorts,pe);
    }
    // do the job
    argv_notrecognized2 = astrset_new(10);
    for (iargv=0 ; iargv<argv_notrecognized1->eleNb ; iargv+=nbeaten) {
        char*  org = argv_notrecognized1->eles[iargv];
        char*  arg = tool_strdup(org);
        int    argpos = iargv+1;
        while ( (arg[0]=='-' || arg[0]=='+') && arg[1]!=0 ) {
            int nbeaten2;
            char arg2=arg[2]; arg[2]=0;
            for (iopt=0; iopt<shorts->eleNb ; iopt+=1 ) {
                pe = shorts->eles[iopt];
                if ( (nbeaten2=opt_update_sh0(pe,arg,argv_notrecognized1->eles,argpos))!=0 ) {
                    break;
                }
            }
            if (iopt==shorts->eleNb) { // arg not recognized 
                break;
            } else {
                argpos += nbeaten2-1;
                arg[2]=arg2;
                tool_strchgEaten(&arg, tool_aprintf("%c%s",*org,arg+2));
            }
        }
        if ( (arg[0]=='-' || arg[0]=='+') && arg[1]==0 ) {
            nbeaten = 1 + argpos-iargv - 1;
        } else {
            argv_notrecognized1->eles[iargv] = org;
            astrset_add(argv_notrecognized2,org);
            nbeaten = 1;
        }
        tool_free(arg);
    }
    ptrswset_delete( shorts );
    astrset_delete(argv_notrecognized1);


    return argv_notrecognized2;
}

extern void      cmdline_finish(Tcmdline*cl, TastrSet* argv, char*mask, int errlvl)
{
    TptrswSet* sws=cl->cl_switches;
    char swstr[100];
    int iargv, iopt, optn=sws->eleNb;
    Tsw*  pe;

    // check switches
    for (iopt=0; iopt<optn ; iopt+=1 ) {
        pe = sws->eles[iopt];
        if ( (pe->optkind&SWOK_Mandatory)!=0 && pe->set==0 )
            tool_ftlerr_printf(NULL,"%s switch must be specified.",cmdline_switch2str(swstr,pe,'"'));
    }

    // check argv
    char*p,*arg;
    for (p=mask ; p!=0 && *p!=0 ; p+=1) {
        for (iargv=0 ; iargv<argv->eleNb ; ) {
            arg = argv->eles[iargv];
            if ( arg[0]==*p ) {
                switch ( errlvl ) {
                    default: tool_ftlerr_printf(NULL,"unrecognized option: %s.",arg); break;
                    case  1: tool_errerr_printf(NULL,"unrecognized option: %s.",arg); break;
                    case  2: tool_wrnerr_printf(NULL,"unrecognized option: %s.",arg); break;
                    case  3: tool_inferr_printf(NULL,"unrecognized option: %s.",arg); break;
                }
                astrset_supShift(argv,iargv);
            } else {
                iargv+=1;
            }
        }
    }
}

/*======================================================================*/
