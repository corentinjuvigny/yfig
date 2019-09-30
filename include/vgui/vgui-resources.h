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
 *      $Id: vgui-resources.h 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-resources.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= The resource API.                                                  =*/
/*=                                                                    =*/
/*= A resource is simply a graphic parameter such as the background of =*/
/*= a given widget, the font of an other one. Using a resources for    =*/
/*= a graphic parameter allows:                                        =*/
/*=   - to hide it when programming, cause it will be handled by the   =*/
/*=     EGS core.                                                      =*/
/*=   - to the end user to change it by modifying resource             =*/
/*=     configuration file of the EGS.                                 =*/
/*=                                                                    =*/
/*= Each resource is defined by 2 macross:                             =*/
/*=    VGUI_ResN_Xxx: the name that identifies the resource, it is     =*/
/*=                   used in VGUI programming.                        =*/
/*=    VGUI_ResC_Xxx: the name that identifies  the resource, in the   =*/
/*=                   EGS configuration file.                          =*/
/*= Those are usually set in the vgui-resource-macros.h header.        =*/
/*=                                                                    =*/
/*= These resource are grouped in a table (TappResources) that is      =*/
/*= defined in the vgui-resource-table.h header.                       =*/
/*= At start time, in this table a resource is composed of:            =*/
/*=   - its ResN and ResC key,                                         =*/
/*=   - a default value,                                               =*/
/*= After the initialisation process, it contains also its values.     =*/
/*= This value is either the default value or an other one got from    =*/
/*= configuration files.                                               =*/
/*=                                                                    =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_RESOURCES_H
#define FILE_VGUI_RESOURCES_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"
#include "vgui-resources-macro.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Tresource {
    TstrStatic r_name;    // the resources id to use in res_set/get
    TstrStatic r_class;   // the resources id used by the external
                          // graphic system.
    TstrStatic r_defval;  // the defautl value of resources.
    TstrEaten  r_value;   // the  value of resources 
} Tresource;

typedef struct _TappResources {
    int        eleNb; // element numbers of eles array
    Tresource* eles;  // eleNb resources
} TappResources;

/*======================================================================*/
/*= Interface                                                          =*/ 

/**
 * The vgui_res_initEGS function retrieves from the gui EGS, the
 * resource values of the gui_appResources table and stores them
 * in the r_value field of the ressource.
 * It skips the resources that have already a value (r_value==NULL)
 * and those which have not class name (r_class==NULL).
 * For retrieving the resources values, it uses the gui_res_xxx
 * functions.
 *
 * Usually it is called once at startup after the EGS is initialized.
 **/
extern void vgui_res_initEGS(Tvgui* gui);
extern void vgui_res_freeEGS(Tvgui* gui);

/**
 * res_set sets the value of id resource of the s resource set to v.
 * If the id resource has already a value (r_value!=0), it is changed
 * only if the overwrite flag is not null.
 *
 * Return value: res_set returns 0 on success otherwise:
 *   -1 : the set does not contains the id resource .
 *    1 : value of resource is already set.
**/
extern int res_set(TappResources* s, const char* id, const char* v,
                   int overwrite);

/**
 * res_getStr retrieves the value of the id resource of the s resource sets
 * and stores it into *v.
 * 
 * res_getInt retrieves the value of the id resource of the s resource sets,
 * converts it to integer and stores it into *v.
 * In the conversion from string to integer, the insensitive case tokens
 * true and yes are recognized as 1 value and the tokens false no as the
 * 0 value.
 * 
 * res_getDbl retrieves the value of the id resource of the s resource sets,
 * converts it to double and stores it into *v.
 *
 * Return value: the res_getXxx functions return 0 on success otherwise:
 *    -1 : the set does not contain the id resource.
 *     1 : bad format (conversion from string to int or double failed).
 * When the functions fails, they leave *v unchanged.
**/
extern int res_getStr(TappResources* s, const char* id, const char** v);
extern int res_getInt(TappResources* s, const char* id, int* v);
extern int res_getDbl(TappResources* s, const char* id, double* v);

/*======================================================================*/
/*= Interface to EGS (External Graphic System)                         =*/ 

extern TappResources gui_appResources;

typedef struct _Tgui_resDB Tgui_resDB;

extern Tgui_resDB* gui_res_new (Tvgui*gui);
extern void        gui_res_free(Tvgui*gui, Tgui_resDB* rdb);
extern char*       gui_res_get (Tvgui*gui, Tgui_resDB* rdb, const char* id);

/*======================================================================*/
/*= Debug                                                              =*/ 

#define DBG0res(fmt,...)   DBG0_printf("resource",          fmt,__VA_ARGS__)
#define DBG1res(fmt,...)   DBG1_printf("resource",          fmt,__VA_ARGS__)
#define DBG2res(fmt,...)   DBG2_printf("resource",          fmt,__VA_ARGS__)

#define DBG0res_E(fmt,...) DBG0_printf("resource","Enter : "fmt,__VA_ARGS__)
#define DBG0res_M(fmt,...) DBG0_printf("resource","Middle: "fmt,__VA_ARGS__)
#define DBG0res_L(fmt,...) DBG0_printf("resource","Leave : "fmt,__VA_ARGS__)
#define DBG1res_E(fmt,...) DBG1_printf("resource","Enter : "fmt,__VA_ARGS__)
#define DBG1res_M(fmt,...) DBG1_printf("resource","Middle: "fmt,__VA_ARGS__)
#define DBG1res_L(fmt,...) DBG1_printf("resource","Leave : "fmt,__VA_ARGS__)
#define DBG2res_E(fmt,...) DBG2_printf("resource","Enter : "fmt,__VA_ARGS__)
#define DBG2res_M(fmt,...) DBG2_printf("resource","Middle: "fmt,__VA_ARGS__)
#define DBG2res_L(fmt,...) DBG2_printf("resource","Leave : "fmt,__VA_ARGS__)

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif //FILE_VGUI_RESOURCES_H
/*======================================================================*/
