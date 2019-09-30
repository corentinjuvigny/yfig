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
 *      $Id: messagedriver.c 128 2018-04-14 15:53:02Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/messagedriver.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for printing messages dedicated to final user.           =*/
/*= See the base-types.h header for printing functions.                =*/
/*= See the messagedriver.h header for driver interface.               =*/
/*======================================================================*/

#include "tools-config.h"
#include "../include/tools/globalvars.h"
#include "../include/tools/messagedriver.h"

/*======================================================================*/
/*= default printing functions                                         =*/

extern void tool_usrmsg_default(Cmsgdrv*drv, CstrBrief msg)
{ fprintf(stdout,"%s\n",msg); }

extern void tool_errmsg_default(Cmsgdrv*drv, CstrBrief msg, int lvl)
{ fprintf(stderr,"%s\n",msg); }

/*======================================================================*/
/*= Interface for printing messages                                    =*/

static TstrEaten tool_genMsg(CstrBrief fmt, va_list ap)
{
    int len;
    TstrEaten msg = tool_vaprintf(fmt,ap);
    for ( len=strlen(msg)-1 ; len>=0 ; len-=1 ) {
        if ( msg[len]=='\n' ) {
            msg[len]=0;
        } else
            break;
    }
    return msg;
}

extern void tool_usridt_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    Tmsgdrv*  p;
    TstrEaten msg=0;
    if (drv==0) drv = &gl_msgdrv_default;

    for (p=drv; p!=0 ; p=p->next ) {
        if ( p->usridt==0 || p->usrmsg_print==0 ) continue;
        if ( msg==0 ) 
            msg = tool_genMsg(fmt,ap);
        p->usrmsg_print(p,msg);
    }
    tool_free(msg);
}

extern void tool_usrmsg_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    Tmsgdrv*  p;
    TstrEaten msg=0;
    if (drv==0) drv = &gl_msgdrv_default;

    for (p=drv; p!=0 ; p=p->next ) {
        if ( p->usrmsg==0 || p->usrmsg_print==0 ) continue;
        if ( msg==0 ) 
            msg = tool_genMsg(fmt,ap);
        p->usrmsg_print(p,msg);
    }
    tool_free(msg);
}

static void tool_xxxerr_do(Tmsgdrv* drv, int lvl, CstrBrief _msg)
{
    if ( (drv->errmsg_print==0 && lvl!=0) ||
          lvl>drv->errlvl ) return;

    char mask[2000]; int masklen=0 ; int first=1;
    if ( drv->addapp!=0 ) {
        masklen += sprintf(mask+masklen,"%s%s",first ? "" : " ",gl_progname);
        first=0;
    }
    if ( drv->addlvl!=0 ) {
        masklen += sprintf(mask+masklen,"%s%-6s",
                first ? "" : ":",
                lvl==0 ? "fatal" : lvl==1 ? "error" : lvl==2 ? "warning" : "info"
        );
        first=0;
    }
    sprintf(mask+masklen,"%s%%s",first ? "" : ": ");
    TstrEaten msg = tool_aprintf(mask,_msg);
    if ( drv->errmsg_print==0 )
        // fatal error only
        tool_errmsg_default(drv,msg,lvl);
    else
        drv->errmsg_print(drv,msg,lvl);
    tool_free(msg);
}

// return 1 if there is nothing to do otherwise 0
static int tool_xxxerr_nothingToDo(Tmsgdrv* drv, int lvl)
{
    for (drv; drv!=0 ; drv=drv->next ) {
        if ( drv->errmsg_print!=0 && lvl<=drv->errlvl ) 
            return 0;
    }
    return 1;
}

extern void tool_inferr_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    int lvl=3;
    Tmsgdrv*p;
    if (drv==0) drv = &gl_msgdrv_default;
    if ( tool_xxxerr_nothingToDo(drv,lvl) ) return;
 
    TstrEaten msg = tool_genMsg(fmt,ap);
    for (p=drv; p!=0 ; p=p->next ) {
        tool_xxxerr_do(p,lvl,msg);
    }
    tool_free(msg);
}

extern void tool_wrnerr_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    int lvl=2;
    Tmsgdrv*p;
    if (drv==0) drv = &gl_msgdrv_default;
    if ( tool_xxxerr_nothingToDo(drv,lvl) ) return;
 
    TstrEaten msg = tool_genMsg(fmt,ap);
    for (p=drv; p!=0 ; p=p->next ) {
        tool_xxxerr_do(p,lvl,msg);
    }
    tool_free(msg);
}

extern void tool_errerr_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    int lvl=1;
    Tmsgdrv*p;
    if (drv==0) drv = &gl_msgdrv_default;
    if ( tool_xxxerr_nothingToDo(drv,lvl) ) return;
 
    TstrEaten msg = tool_genMsg(fmt,ap);
    for (p=drv; p!=0 ; p=p->next ) {
        tool_xxxerr_do(p,lvl,msg);
    }
    tool_free(msg);
}

extern void tool_ftlerr_vprintf (Tmsgdrv* drv, CstrBrief fmt, va_list ap)
{
    int lvl=1;
    Tmsgdrv*p;
    TstrEaten msg = tool_genMsg(fmt,ap);
    if (drv==0) drv = &gl_msgdrv_default;
    for (p=drv; p!=0 ; p=p->next ) {
        tool_xxxerr_do(p,lvl,msg);
    }
    tool_free(msg);

    if ( drv->exit ) {
        drv->exit(drv,1);
    } else {
        tool_xxxerr_do(drv,lvl,"can not continue");
        exit(1);
    }
}

extern void tool_errerr_nomem  (Tmsgdrv* drv) { tool_errerr_printf(drv,"no more memory"); }
extern void tool_ftlerr_nomem  (Tmsgdrv* drv) { tool_ftlerr_printf(drv,"no more memory"); }

/*======================================================================*/
