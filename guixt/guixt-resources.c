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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: guixt-resources.c 146 2018-06-01 16:58:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/guixt-resources.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource API (EGS driver).                                     =*/
/*=                                                                    =*/
/*= See the "vgui/vgui-resources.h" header for the type description,   =*/
/*= the function description and for the documentation.                =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/vgui-resources.h"
#define USE_ResourceX11 1
#include "vgui/vgui-resources-table.h"

/*======================================================================*/

#define RDB_ID "Yfig"

struct _Tgui_resDB {
    char*       manager;  // handled by Xlib, must not be freed
    XrmDatabase db;
};

/*======================================================================*/

extern Tgui_resDB* gui_res_new (Tvgui*gui)
{
    Tgui_main* main = gui->g_main;
    Tgui_resDB* rdb = malloc(sizeof(*rdb));

    rdb->db = XtScreenDatabase(main->m_screen);
    if ( rdb->db==0 ) {
        vgui_wrnerr_printf(gui,"can not get data of application ressources.");
        free(rdb);
        return 0;
    }
    //XrmPutFileDatabase(rdb->db,"/tmp/AAA"); //system("cat /tmp/AAA");
    return rdb;
#if 0
    rdb->manager = XResourceManagerString(main->m_display);
    if ( rdb->manager==0 ) {
        tool_warn_printf("can not get data of application ressources.");
        rdb->db=0;
        rdb->db = XtScreenDatabase(main->m_screen);
        int argc=1;
        char* argv[] = {"Figia", 0 };
        //XrmParseCommand(&rdb->db,(void*)0,0,"Figia",&argc,argv);
        tool_warn_printf("can not get data of application ressources %p.",(void*)rdb->db);

        free(rdb);
        return 0;
    }
printf("%s: res_manager=\n%s\n------------------\n",__func__,rdb->manager);
    rdb->db = XrmGetStringDatabase(rdb->manager);
    if (rdb->db == NULL) {
        tool_warn_printf("can not get application ressources.");
        //printf("%s: res_manager=\n%s\n------------------\n",__func__,res_manager);
        free(rdb);
        return 0;
    }

    return rdb;
#endif
}

extern void gui_res_free(Tvgui*gui, Tgui_resDB* rdb)
{
    if (rdb==0 )
        return;
    //XrmDestroyDatabase(rdb->db);
    free(rdb);
}

extern char* gui_res_get (Tvgui*gui, Tgui_resDB* rdb, const char* rn)
{
    XrmValue     ret;
    char        *type;
    memset(&ret,0,sizeof(ret)); type = 0;
    char        res_class[1024];
    sprintf(res_class,"%s.%s",RDB_ID,rn);
    if ( rdb->db==0 )
        return 0;
    if ( XrmGetResource(rdb->db, res_class, RDB_ID, &type, &ret)==0 ) {
        vgui_inferr_printf(gui,
              "\"%s\" resource not found in resource data base.",res_class);
        return 0;
    }
    if ( type==0 || strcmp("String",type)!=0)  {
        vgui_inferr_printf(gui,"%s type is unexpected for %s resource",
                    type?type: "???",res_class);
        return 0;
    }
    return strdup(ret.addr);
}

/*======================================================================*/
