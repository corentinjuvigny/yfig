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
 *      $Id: vgui-resources.c 128 2018-04-14 15:53:02Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-resources.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource module.                                               =*/
/*= See the vgui-resource.h header for documentation.                  =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui-resources.h"

/*======================================================================*/

extern void vgui_res_initEGS(Tvgui * gui)
{
    Tresource*  r;
    Tgui_resDB* resDB;
    int i;
    for (i=0 ; i<gui_appResources.eleNb ; i+=1) {
        r = gui_appResources.eles+i;
        if ( r->r_class!=0 && r->r_value==0 )
            goto get_app_resources;
    }
    return;

get_app_resources:
    resDB = gui_res_new(gui);
    if ( resDB==0 )
        return;
    for (i=0 ; i<gui_appResources.eleNb ; i+=1) {
        r = gui_appResources.eles+i;
        if ( r->r_class==0 ) continue;
        if ( r->r_value!=0 ) continue;
        char* rv = gui_res_get(gui,resDB,r->r_class);
        if (rv!=0) {
            vgui_inferr_printf(gui,"%s Resource sets to \"%s\".",r->r_name,rv);
            res_set(&gui_appResources,r->r_name,rv,0);
            free(rv);
        }
    }
    gui_res_free(gui,resDB);
}

extern void vgui_res_freeEGS(Tvgui * gui)
{
    Tresource*  r;
    int i;
    for (i=0 ; i<gui_appResources.eleNb ; i+=1) {
        r = gui_appResources.eles+i;
        if ( r->r_value!=0 ) free( r->r_value );
    }
}

/*======================================================================*/

static Tresource* res_get(TappResources* s, const char* id)
{
    int i;
    for (i=0 ; i<s->eleNb ; i++) {
        if ( strcmp(s->eles[i].r_name,id)==0 )
            return s->eles+i;
    }
    return 0;
}

/*======================================================================*/

extern int res_set(TappResources* s, const char* id, const char* v,
                   int overwrite)
{
    Tresource *r;
    if ( (r=res_get(s,id))==0 )
        return -1;
    if ( r->r_value==0 ) {
        r->r_value = s!=0 ? strdup(v) : 0;
        return 0;
    }
    if ( !overwrite )
        return 1;
    else {
        free(r->r_value);
        r->r_value = s!=0 ? strdup(v) : 0;
        return 0;
    }
}

extern int res_getStr(TappResources* s, const char* id, const char** v)
{
    Tresource *r;
    if ( (r=res_get(s,id))==0 )
        return -1;
    if ( (*v=r->r_value)==0 )
        *v=r->r_defval;
    return 0;
}

extern int res_getInt(TappResources* s, const char* id, int* v)
{
    int i;
    const char* str;
    Tresource *r;
    if ( res_getStr(s,id,&str)!=0 )
        return -1;
    if ( str==0 )
        return 1;
    if ( strcasecmp(str,"true")==0 ) {
        *v=1;
        return 0;
    }
    if ( strcasecmp(str,"false")==0 ) {
        *v=0;
        return 0;
    }
    if ( strcasecmp(str,"yes")==0 ) {
        *v=1;
        return 0;
    }
    if ( strcasecmp(str,"no")==0 ) {
        *v=0;
        return 0;
    }
    for (i=0 ; str[i]!=0 ; i+=1) {
        if ( isdigit(str[i])==0 )
            return 1;
    }
    if ( sscanf(str,"%d",v)==1 )
        return 0;
    return 1;
}

extern int res_getDbl(TappResources* s, const char* id, double* v)
{
    int i;
    const char* str;
    Tresource *r;
    if ( res_getStr(s,id,&str)!=0 )
        return -1;
    if ( str==0 )
        return 1;
    int nbdot=0;
    int nbdigit=0;
    for (i=0 ; str[i]!=0 ; i+=1) {
        if ( str[i]=='.' )
            nbdot += 1;
        else if ( isdigit(str[i]) )
            nbdigit +=1;
        else
            return 1;
    }
    if ( nbdot>1 || nbdigit==0 )
        return 1;
    if ( sscanf(str,"%lf",v)==1 )
        return 0;
    return 1;
}

/*======================================================================*/
