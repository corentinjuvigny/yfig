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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: object-priv.h 119 2018-01-28 16:17:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/object-priv.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This file groups the object services that are rather dedicated     =*/
/*= to be used internally by the FDS API.                              =*/
/*=                                                                    =*/
/*= The applications must as far as possible use the FDS services      =*/
/*= that the object.h header define.                                   =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJECT_PRIV_H
#define FILE_OBJECT_PRIV_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "object.h"

/*======================================================================*/
/*= object constructors & destructor                                   =*/
/*=                                                                    =*/
/*= The obj_new function creates a new object. All the fields are      =*/
/*= set to their default value (0 usually) except of the virtual       =*/
/*= table fields that is set to vt.                                    =*/
/*=                                                                    =*/
/*= The obj_newDepth function creates a new object. All the fields     =*/
/*= are set to their default value (0 usually) except of the virtual   =*/
/*= table fields that is set to vt and the depth set to the depth      =*/
/*= parameter.                                                         =*/
/*=                                                                    =*/
/*= The obj_newByOid and obj_newByOidDepth functions are similar to    =*/
/*= the former except that the virtual table is set to vtt[oid].       =*/
/*=                                                                    =*/
/*= The obj_newByODS function creates a new object by invoking         =*/
/*=      obj_newByOidDepth(oid,vtt,depth)                              =*/
/*= then it update the drawing styles (curve/line, fill, text) that    =*/
/*= exists in object with the corresponding styles (stl,stf,stt).      =*/
/*=                                                                    =*/
/*= The obj_newByFdp function is similar to the obj_newByODS function  =*/
/*= except that the parameters are grouped in a Tfdp structure.        =*/
/*=                                                                    =*/
/*= Return:                                                            =*/
/*=  All these functions return the new object. It is always defined.  =*/
/*=  If a problem of memory allocation occurs the function exit.       =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=   vt    : a pointer to the virtual table.                          =*/
/*=   depth : the depth of the object in the range                     =*/
/*=           [FIG_MIN_DEPTH,FIG_MAX_DEPTH[                            =*/
/*=           Set to -1 disables the depth initialization.             =*/
/*=   vtt   : the table of virtual table to use.                       =*/
/*=           A NULL value selects the default VTT.                    =*/
/*=   stl   : the line style of object.                                =*/
/*=           If the object has no line style such as text object,     =*/
/*=           it can be set to the NULL ointer.                       =*/
/*=   stf   : same as stl but for fill style.                          =*/
/*=   stt   : same as stl but for text style.                          =*/

extern TobjAll  obj_new     (void* vt);
extern TobjAll  obj_newDepth(void* vt, int depth);
extern TobjAll  obj_newByOid     (int oid,TobjVTT vtt);
extern TobjAll  obj_newByOidDepth(int oid, TobjVTT vtt, int depth);

extern TobjAll  obj_newByODS(int oid, TobjVTT vtt, int depth, 
                             Csty_line*stl, Csty_fill*stf, Csty_text*stt);
extern TobjAll  obj_newByFdp(int oid, Cfdp* fdp);

extern void     obj_delete  (TobjAll obj);
extern void     obj_deleteS0(TobjAll*obj);

extern void     obj_fig_attach(TobjAll obj,Tfigure*fig);
extern void     obj_fig_detach(TobjAll obj);

/*======================================================================*/
/*= Object services.                                                   =*/

extern void      obj_dump(TobjAll obj, int sh);

/*======================================================================*/
/*= Utilities for implementing the arrow feature.                      =*/

extern void obj_arrow_doBegLine(TobjAll oa, Tdble2Set* pts);
extern void obj_arrow_doEndLine(TobjAll oa, Tdble2Set* pts);

extern void obj_arrow_doBegSpline(TobjAll oa, Tdble2Set* ctls, TdbleSet* ndes);
extern void obj_arrow_doEndSpline(TobjAll oa, Tdble2Set* ctls, TdbleSet* ndes);

extern void obj_arrow_doBegArc(TobjAll oa, Tdble2 pts[4], int dir);
extern void obj_arrow_doEndArc(TobjAll oa, Tdble2 pts[4], int dir);

/*======================================================================*/
/*= Utilities for implementing the vt_dump service.                    =*/

extern TstrBrief obj_dump_anyStr(TstrBrief buf,TobjAll obj);
extern void      obj_dump_any(TobjAll obj, int shi, CstrBrief fn);
extern void      obj_dump_dble2   (Cdble2* s, int nb, int sh, CstrBrief fn);
extern void      obj_dump_dble2set(Cdble2Set* s,      int sh, CstrBrief fn);

/*======================================================================*/
/*= Miscellaneous utilities                                            =*/



/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJECT_PRIV_H
/*======================================================================*/
