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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-accelkey.c 217 2018-06-13 14:40:12Z cj $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-accelkey.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for defining accelerator key.                            =*/
/*=                                                                    =*/
/*= See the vgui-types.h header for the type description.              =*/
/*= See the vgui-accelkey.h for the function description and           =*/
/*= the documentation.                                                 =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-accelkey.h"

/*======================================================================*/

extern char* accel_toString(TaccelDef ad)
{ 
    char buf[100];
    int len; char* plus;

    buf[0]=0;
    if ( ad==0 )
        return 0;
    plus="";
    if ( ad&GL_ACCEL_Meta ) {
        strcat(buf,plus);
        strcat(buf,"Meta");
        plus = "+";
    }
    if (   ad&GL_ACCEL_Control ) {
        strcat(buf,plus);
        strcat(buf,"Ctrl");
        plus = "+";
    }
    if (   ad&GL_ACCEL_Shift ) {
        strcat(buf,plus);
        strcat(buf,"Shift");
        plus = "+";
    }
    strcat(buf,plus);
    len = strlen(buf);
    buf[len]   = tolower(  ad&0xff );
    buf[len+1] = 0;
    return strdup(buf);
}

extern char* accel_appendToString(const char* base,
        TaccelDef ad1, TaccelDef ad2)
{
    char* ret;
    if      (ad1==0 && ad2==0) {
        ret = strdup(base);
    } else if (ad1!=0 && ad2!=0) {
        char* astr0=accel_toString(ad1);
        char* astr1=accel_toString(ad2);
        ret = tool_aprintf("%s (%s or %s)",base,astr0,astr1);
        free(astr0); free(astr1);
    } else if (ad1!=0 && ad2==0) {
        char* astr=accel_toString(ad1);
        ret = tool_aprintf("%s (%s)",base,astr);
        free(astr);
    } else { // ad1==0 && ad2!=0
        char* astr=accel_toString(ad2);
        ret = tool_aprintf("%s (%s)",base,astr);
        free(astr);
    }
    return ret;
}

extern int   accel_byAmpersand(char*dest, const char*src, TaccelDef* accel, int flags)
{
    TaccelDef accel_tmp;
    char      dest_tmp[500];
    if (accel==0) accel=&accel_tmp;
    if (dest==0) dest=dest_tmp;
    char* p;
    if ( (p=index(src,'&'))==0 ) {
        strcpy(dest,src);
        *accel=0;
        return -1;
    }
    int c=p[1];
    int ret = p-src;
    strncpy(dest   ,src,ret);
    strcpy(dest+ret,src+ret+1);
    *accel = GL_ACCEL_Control | GL_ACCEL_Key(c);
    char* tmp  = accel_toString(*accel);
    char* tmp2 = tool_aprintf("%-15s%10s",dest,tmp);
    if (flags) strcpy(dest,tmp2);
    free(tmp2); 
    free(tmp); 
    return ret;
}

/*======================================================================*/
