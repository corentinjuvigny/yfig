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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: guiqt-resources.cc 128 2018-04-14 15:53:02Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-resources.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource API (EGS driver).                                     =*/
/*=                                                                    =*/
/*= See the "vgui/vgui-resources.h" header for the type description,   =*/
/*= the function description and for the documentation.                =*/
/*======================================================================*/

#include "guiqt.h"

#include "vgui/vgui-resources.h"
#define USE_ResourceX11 1
#include "vgui/vgui-resources-table.h"

#ifdef QT_VERS_5
#include <QSettings>
#endif


/*======================================================================*/

#define RDB_ID "Yfig"

struct _Tgui_resDB {
    QSettings* db;
};

/*======================================================================*/

extern Tgui_resDB* gui_res_new (Tvgui*gui)
{
    return 0;
    Tgui_main* main = gui->g_main;
    Tgui_resDB* rdb = new Tgui_resDB;

    rdb->db = new QSettings("",RDB_ID);
    if (rdb->db == NULL) {
        vgui_wrnerr_printf(gui,"can not get application ressources.");
        //printf("%s: res_manager=\n%s\n------------------\n",__func__,res_manager);
        free(rdb);
        return 0;
    }

    return rdb;
}

extern void gui_res_free(Tvgui*gui, Tgui_resDB* rdb)
{
    if (rdb==0 )
        return;
    delete rdb->db;
    delete rdb;
}

extern char* gui_res_get (Tvgui*gui, Tgui_resDB* rdb, const char* rn)
{
    //XrmValue     ret;
    //char        *type;
    if ( rdb->db==0 )
        return 0;
    QString    qstr = rdb->db->value(rn).toString();
    QByteArray ba = qstr.toLatin1();
    const char* ret = ba.data();
    if ( ret==0 || *ret==0 ) {
        vgui_inferr_printf(gui,
              "\"%s\" resource not found in resource data base.",rn);
        return 0;
    } else
        return strdup(ret);
}

/*======================================================================*/
