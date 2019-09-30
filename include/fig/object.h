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
 *      $Id: object.h 391 2019-03-25 17:49:56Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/object.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This file groups the object services that are dedicated to         =*/
/*= applications.                                                      =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJECT_H
#define FILE_OBJECT_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "fig-types.h"

#include "outdev/style.h"
#include "tools/boxes.h"
#include "tools/bitmap-types.h"
#include "tools/math-macros.h"

/*======================================================================*/

// Every object has a numeric identifier FIG_KD_....
#define FIG_KD_Ellipse      1
#define FIG_KD_Line         2
#define FIG_KD_Spline       3
#define FIG_KD_Text         4
#define FIG_KD_Arc          5
#define FIG_KD_Compound     6
#define FIG_KD_Boxhv        7
#define FIG_KD_ArrowT00     8  // simple (-->)
#define FIG_KD_ArrowT01     9  // triangle white or parent filled --▷ --▶
#define FIG_KD_ArrowT02    10  // like T01 but the mid pt of the vertical
                               // edge is moved a few inside the triangle.
#define FIG_KD_ArrowTend   FIG_KD_ArrowT02  // last arrow kind (included).
#define FIG_KD_LowerFreeId 11

// It is the default VTT. When no VTT is specified (vtt parameter of
// function set to NULL), the FIG API uses this VTT.
extern void* obj_vtable[];

/*======================================================================*/
/*= Arc objects                                                        =*/
/*=                                                                    =*/
/*= The obj_arc_newF and obj_arc_newA functions create an arc (only    =*/
/*= circle, not elliptic) object and return it:                        =*/
/*=   - the kind of which is type (see FIGOAR_T_... macros)            =*/
/*=   - its 3 vertices are given by the pts point array.               =*/
/*=      pts[0] is the start point of the arc.                         =*/
/*=      pts[1] is the middle point of the arc.                        =*/
/*=      pts[2] is the end point of the arc.                           =*/
/*= CAUTION: The 3 vertices must not be colinear.                      =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  pts: A 3 vertice array.                                           =*/
/*=  vtt: the table of virtual table to use (NULL for default VTT).    =*/
/*=  fdp: The used fields are:                                         =*/
/*=         - sty_ud: the table of virtual to use.                     =*/
/*=           Set it to NULL for the default virtual table.            =*/
/*=         - sty_depth, sty_line (except join style), sty_fill.       =*/

#define        FIGOAR_T_Open      1
#define        FIGOAR_T_PiePiece  2

extern TobjAll obj_arc_newF(TobjVTT vtt, 
                        int depth, Csty_line* stl, Csty_fill* stf,
                        int type, Tdble2*pts);
extern TobjAll obj_arc_newA (Cfdp* att, int type, Cdble2*pts);
extern TobjAll obj_arc_newA3(Cfdp* att, int type,
                             Cdble2 spt, Cdble2 mpt, Cdble2 ept);

/*======================================================================*/
/*= Ellipse objects                                                    =*/
/*=                                                                    =*/
/*= The obj_ellipse_newF and obj_ellipse_newA functions create a full  =*/
/*= ellipse object and return it:                                      =*/
/*=   - the kind of which is type (see the FIGOELL_T_... macros)       =*/
/*=   - the pt1..pt2 line segment defines it (see FIGOELL_T_...) with  =*/
/*=     its axis horizontal and vertical.                              =*/
/*=   - the the horizontal/vertical ellipse is rotated by alpha        =*/
/*=     radians "dans le sens trigonométrique", the rotation center    =*/
/*      being pt1.                                                     =*/
/*= Note that there is no restriction on pt1, pt2 and alpha value, for =*/
/*= extreme values the shape becomes a line or a point.                =*/
/*=                                                                    =*/
/*= The obj_ellipse_newA is similar to obj_ellipse_newA, except that   =*/
/*= the horizontal/vertical ellipse is given by its center and its     =*/
/*= radius.                                                            =*/
/*= For circle the radius is set to the radius average.                =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  pt1,pt2: The control points.                                      =*/
/*=  alpha  : The rotation angle in radian.                            =*/
/*=  vtt: the table of virtual table to use (NULL for default VTT).    =*/
/*=  fdp: The used fields are:                                         =*/
/*=        - sty_ud: the table of virtual to use.                      =*/
/*=          Set it to NULL for the default virtual table.             =*/
/*=        - sty_depth, sty_line (without cup & join styles), sty_fill.=*/
/*=  ctr: The ellipse center.                                          =*/
/*=  rds: The ellipse radius.                                          =*/

#define FIGOELL_T_EllByRad    1  // p1 is the center, p2 a BB corner
#define FIGOELL_T_EllByDia    2  // p1 & p2 are opposite BB corners.
#define FIGOELL_T_CirByRad    3  // p1 is the center, p1 a circle point
#define FIGOELL_T_CirByDia    4  // p1 p2 is a circle diameter 

extern TobjAll obj_ellipse_newF   (TobjVTT vtt,
                 int depth, Csty_line* stl, Csty_fill* stf,
                 int type, Tdble2 pt1, Tdble2 pt2, double alpha);
extern TobjAll obj_ellipse_newA   (Cfdp* fdp,
                 int type, Tdble2 pt1, Tdble2 pt2, double alpha);
extern TobjAll obj_ellipse_newAcra(Cfdp* fdp,
                 int type, Tdble2 ctr, Tdble2 rds, double alpha);

/*======================================================================*/
/*= Line based objects                                                 =*/
/*=                                                                    =*/
/*= The obj_line_newF and obj_line_newA functions create a line        =*/
/*= object and return it:                                              =*/
/*=   - the kind of which is type (see FIGOLI_T_... macros)            =*/
/*=   - its vertices are given by the pts point set.                   =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  pts: The vertices. CAUTION: it is eaten by the object.            =*/
/*=  vtt: the table of virtual table to use (NULL for default VTT).    =*/
/*=  fdp: The used fields are:                                         =*/
/*=         - sty_ud: the table of virtual to use.                     =*/
/*=           Set it to NULL for the default virtual table.            =*/
/*=         -sty_depth, sty_line, sty_fill.                            =*/

#define FIGOLI_T_MLine    1  // Multi-line: a line along the vertices
                             // N vertices --> N-1 edges
#define FIGOLI_T_Box      2  // a 4 vertices/edge polygon 
#define FIGOLI_T_Polygon  3  // a closed Multi-line
                             // N vertices --> N edges
#define FIGOLI_T_RoundBox 4  // a T_Box with rounded corner.

extern TobjAll    obj_line_newF(TobjVTT vtt,
                        int depth, Csty_line* stl, Csty_fill* stf,
                        int type, Tdble2Set* pts);
extern TobjAll    obj_line_newA(Cfdp* fdp, int type, Tdble2Set* pts);

extern TobjAll    obj_sqrrect_newAPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2);
extern TobjAll    obj_sqrrect_newAXYWH(Cfdp* fdp, double x,  double y,
                                                  double dx, double dy);
extern TobjAll    obj_arcrect_newAPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2);
extern TobjAll    obj_arcrect_newAXYWH(Cfdp* fdp, double x,  double y,
                                                  double dx, double dy);

/*======================================================================*/
/*= Spline based objects                                               =*/
/*=                                                                    =*/
/*= The obj_spline_newF and obj_spline_newA functions create a spline  =*/
/*= object and return it:                                              =*/
/*=   - the kind of which is type (see FIGOSP_T_... macros)            =*/
/*=   - its control points are given by the ctls point set.            =*/
/*=   - the nodes are automatically set depending on the spline type.  =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  ctls: The control points. CAUTION: it is eaten by the object.     =*/
/*=  vtt: the table of virtual table to use (NULL for default VTT).    =*/
/*=  fdp: The used fields are:                                         =*/
/*=         - sty_ud: the table of virtual to use.                     =*/
/*=           Set it to NULL for the default virtual table.            =*/
/*=         -sty_depth, sty_line, sty_fill.                            =*/

#define FIGOSP_T_OpenApprox    0    // spline inside the vertices polygon
#define FIGOSP_T_ClosedApprox  1
#define FIGOSP_T_OpenInterp    2    // spline outside the vertices polygon
#define FIGOSP_T_ClosedInterp  3
//#define FIGOSP_T_OPEN_XSPLINE    4       // defined in xfig ????
//#define FIGOSP_T_CLOSED_XSPLINE  5       // defined in xfig ????

extern TobjAll obj_spline_newF(TobjVTT vtt,
                        int depth, Csty_line* stl, Csty_fill* stf,
                        int type, Tdble2Set* ctls);
extern TobjAll obj_spline_newA(Cfdp* fdp, int type, Tdble2Set* ctls);

/*======================================================================*/
/*= text objects                                                       =*/
/*=                                                                    =*/
/*= The obj_text_newF and obj_text_newA functions create a text object =*/
/*= and return it:                                                     =*/
/*=   - the kind of which is type (see the FIGOTE_T_... macros)        =*/
/*=   - the pt defines the text position.                              =*/
/*=   - str is the string.                                             =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  pt    : The position of the text.                                 =*/
/*=  l,h   : The width and height of the text when it is written       =*/
/*=          horizontally.                                             =*/
/*=  str   : The text under UTF-8 format with unicode character set.   =*/
/*=  flag  : Not yet used (use zero).                                  =*/
/*=  vtt: the table of virtual table to use (NULL for default VTT).    =*/
/*=  fdp: The used fields are:                                         =*/
/*=        - sty_ud: the table of virtual to use.                      =*/
/*=          Set it to NULL for the default virtual table.             =*/
/*=        - sty_depth, sty_text.                                      =*/

#define FIGOTE_T_LEFT    STY_T_LEFT    // position is at the left hand of the text
#define FIGOTE_T_CENTER  STY_T_CENTER  // position is in middle of the text
#define FIGOTE_T_RIGHT   STY_T_RIGHT   // position is at the right hand of the text

extern TobjAll obj_text_newF(TobjVTT vtt, int depth, Csty_text* stt,
                    int type,  Tdble2 pt, double l, double h,
                    CstrBrief str, int flag);
extern TobjAll obj_text_newA(Cfdp* fdp,
                    int type,  Tdble2 pt, double l, double h,
                    CstrBrief str, int flag);

/**
 * The obj_text_setTextFU/OU functions change to the str string, the text
 * of the self text object and update the object bounding box using
 * l (width) and h (height) of the str string written horizontally.
 *
 * In the obj_text_setTextFU (resp: OU) the l and h lengthis are in FU
 * unit (resp: in object unit (self->O_ppi)).
**/
extern void obj_text_setTextFU(TobjText* self, CstrBrief str, double l, double h);
extern void obj_text_setTextOU(TobjText* self, CstrBrief str, double l, double h);

/**
 * The obj_text_updateGeom function set the OTE_length, OTE_height
 * fields respectively to the l and h parameters.
 * It also updates the bounding boxes of the self text object and sets
 * the FIG_TAG_BBoxUpToDate flag of the self text object.
 *
 * The ascent parameter is used internally to compute the bounding box.
 *
 * Parameters:
 *  self: The text object to update.
 *  l,h:  The text length and height of text layout in figure unit for the
 *        font and font size that are defined in the self text object.
 *  ascent: The font ascent of the font and font size that are defined in
 *        the self text object.
 *        if this parameter is less than zero, the function uses an
 *        approximated value.
**/
extern void obj_text_updateGeom(TobjText* self,
                double l, double h, double ascent);

/*======================================================================*/
/*= object(s) container the shape of which is a rectangular boxes      =*/
/*=                                                                    =*/
/*= The obj_image_newFbyXYWH and obj_image_newAbyXYWH functions        =*/
/*= create a boxhv object that contains an image and return it:        =*/
/*=   - the type of which is FIGOBHV_T_Image                           =*/
/*=   - the box starts at the (x,y) position and its width is dx       =*/
/*=     and its height is dy.                                          =*/
/*=   - the image is set to the img parameter.                         =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  x,y:   The position of the box in the figure in figure unit.      =*/
/*=  dx,dy: The width and height of rectangle in figure unit.          =*/
/*=         They must be greater than zero.                            =*/
/*=         (x,y)...(x+dx,y+dy) define a box diagonal.                 =*/
/*=  vtt:   The table of object virtual table.                         =*/
/*=         Set it to NULL for the default virtual table.              =*/
/*=  fdp:   The used fields are:                                       =*/
/*=          - sty_ud: the table of virtual table to use.              =*/
/*=            Set it to NULL for the default virtual table.           =*/
/*=          - sty_depth                                               =*/
/*= img:    The image to store. Caution it is eaten by the object.     =*/

#define FIGOBHV_T_Image   1    // the boxhv object contains an image

extern TobjAll obj_image_newFbyXYWH(TobjVTT vtt, int depth,
                    double x, double y, double dx, double dy, Timage* img);
extern TobjAll obj_image_newAbyXYWH(Cfdp* fdp,
                    double x, double y, double dx, double dy, Timage* img);
extern TobjAll obj_image_newAbyPTPT(Cfdp* fdp, Tdble2 pt1, Tdble2 pt2, Timage* img);

extern void    obj_boxhv_setRectXYWH(TobjAll obj,
                    double x, double y, double dx, double dy);

/*======================================================================*/
/*= Arrow Object                                                       =*/
/*=                                                                    =*/
/*= Arrows objects can not be created directly, they can only be       =*/
/*= added to or supressed from an existing object that has arrow       =*/
/*= support.                                                           =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=   - op    : the parent object the arrow belongs to.                =*/
/*=   - atBeg : 0/1 to change the end or begin arrow.                  =*/
/*=   - vt    : the virtual table to use (define the kind of arrow).   =*/
/*=   - stf   : the style the arrow uses for filling its shape.        =*/
/*=   - len   : the arrow length in FU.                                =*/
/*=   - a     : the angle of the arrowhead.                            =*/
/*=   - th    : the line thickness of the arrow in PT.                 =*/

extern void obj_arrow_chgRel (TobjAll op, int atBeg, void* vt,
                              Csty_fill* stf, double len, double a);
extern void obj_arrow_chgAbs (TobjAll op, int atBeg, void* vt,
                              Csty_fill* stf, double len, double a, double th);
extern void obj_arrow_permute(TobjAll op);
extern void obj_arrow_delete (TobjAll op, int atBeg);
extern void obj_arrow_getParams(TobjAll ao, CobjVtable**vtable, int*abs, Csty_fill**fill,
                                double*len, double*angle, double*th );

/*======================================================================*/
/*= directoty objects                                                  =*/
/*=                                                                    =*/
/*= The obj_dir_newF and obj_dir_newA functions create a directory     =*/
/*= object and return it. The created directory is empty.              =*/
/*=                                                                    =*/
/*= The obj_dir_mkdir functions adds to the dir object an empty        =*/
/*= directory and returns the created empty directory.                 =*/
/*= CAUTION: The function assumes that the dir object (1) is defined   =*/
/*= (not OBJ_none) and (2) is really a directory object.               =*/
/*=                                                                    =*/
/*= The obj_dir_flat function removes the dir hierarchy level in the   =*/
/*= object tree of the dir parent directory.                           =*/
/*= The dir object is deleted.                                         =*/
/*=                                                                    =*/
/*= The obj_dir_addObj function adds the obj object to the dir object. =*/
/*=                                                                    =*/
/*= The obj_dir_remObj function removes the obj object from the pdir   =*/
/*= object and returns obj.                                            =*/
/*= The caller is responsible to delete the obj object.                =*/
/*=                                                                    =*/
/*= The obj_dir_delObj function removes the obj object from the pdir   =*/
/*= object and then delete the obj object.                             =*/
/*=                                                                    =*/
/*= The obj_dir_movObj function moves the obj object from the pdir     =*/
/*= object to the "to" dir object. It is fully equivalent to           =*/
/*=      obj_dir_addObj(to,obj_dir_remObj(pdir,obj))                   =*/          
/*= but it optimizes the work.                                         =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=  dir : a directory object.                                         =*/
/*=  pdir: the parent directory of the obj object.                     =*/
/*=        CAUTION: the functions assumes this.                        =*/
/*=  to  : a directory object.                                         =*/
/*=  vtt : the table of virtual table to use (NULL for default VTT).   =*/
/*=  fdp : The used fields are:                                        =*/
/*=         - sty_ud: the table of virtual to use.                     =*/
/*=           Set it to NULL for the default virtual table.            =*/

extern TobjAll obj_dir_newF      (TobjVTT vtt);
extern TobjAll obj_dir_newA      (Cfdp*   fdp);
extern TobjAll obj_dir_mkdir     (TobjAll dir);

extern void    obj_dir_flat    (TobjAll dir);
extern void    obj_dir_addObj  (TobjAll dir,  TobjAll obj);
extern TobjAll obj_dir_remObj  (TobjAll pdir, TobjAll obj);
extern void    obj_dir_delObj  (TobjAll pdir, TobjAll obj);
extern void    obj_dir_movObj  (TobjAll pdir, TobjAll obj, TobjAll to);
extern TobjAll obj_dir_remObjQ (TobjAll obj);
extern void    obj_dir_delObjQ (TobjAll obj);
Inline void    obj_dir_delObjQN(TobjAll*obj) { obj_dir_delObjQ(*obj); *obj=OBJ_none; }
extern void    obj_dir_movObjQ (TobjAll obj, TobjAll to);

/*======================================================================*/
/*= Object getters                                                     =*/
/*=                                                                    =*/
/*= Most of these getters are obvious.                                 =*/
/*=                                                                    =*/

Inline TobjAll obj_ObjAll(void*o) { TobjAll oa; oa.any=o; return oa; }

extern int         obj_kind       (Cobject obj);
extern int         obj_Kind       (CobjAll obj);
extern int         obj_SubKind    (CobjAll obj);
extern CstrBrief   obj_Name       (CobjAll obj);
extern int         obj_Mask       (CobjAll obj);
extern double      obj_Ppi        (CobjAll obj);       // 1 in obj = 1/ppi Inch
extern Tfigure*    obj_Figure     (CobjAll obj);
extern Tfig_colors*obj_ClrFig     (CobjAll obj);
extern Ccolor*     obj_FigClrBlack(CobjAll obj);
extern Ccolor*     obj_FigClrWhite(CobjAll obj);
#if 1
extern Tsty_text*  obj_StyText    (CobjAll obj);
extern Tsty_line*  obj_StyCurve   (CobjAll obj);
extern Tsty_line*  obj_StyLine    (CobjAll obj);
extern Tsty_fill*  obj_StyFill    (CobjAll obj);
#endif
extern int         obj_isDrawDataReady  (CobjAll obj); // 0:if not
extern int         obj_isDrawingDisabled(CobjAll obj); // 0:if not
extern CboxDble*   obj_BBox             (CobjAll obj);

extern int         obj_arrow_IsSupported(CobjAll obj); // 0: can not have arrow
extern TobjArrow*  obj_arrow_Beg(CobjAll obj);         // 0: have not a  beg arrow
extern TobjArrow*  obj_arrow_End(CobjAll obj);         // 0: have not an end arrow
extern TobjAll*    obj_arrow_BegPtr(CobjAll obj);
extern TobjAll*    obj_arrow_EndPtr(CobjAll obj);

/*======================================================================*/
/*= Object selection.                                                  =*/
/*=                                                                    =*/
/*= The obj_getChildren function returns the child objects of the dir  =*/
/*= object into an object set. If the dir object does not have any     =*/
/*= child, it returns the NULL pointer. This later case occurs if      =*/
/*=   - dir is not a directory object,                                 =*/
/*=   - or dir is a directory but empty.                               =*/
/*=                                                                    =*/
/*= The other obj_getChildren... function return also the children of  =*/
/*= the dir object as obj_getChildren does but they filter them:       =*/
/*=   - D: Only the children with at least an active detph:            =*/
/*=               dt[child depth]>0                                    =*/
/*=   - B: Only the children the bounding box of which overlaps        =*/
/*=            with the bb bounding box.                               =*/
/*=   - M: Only the children that the msk mask select. See the         =*/
/*=            FIG_KDM_... macros                                      =*/
/*= These functions never return a NULL pointer and the caller is      =*/
/*= responsible to delete the children set after use.                  =*/
/*=                                                                    =*/
/*= The obj_getObjCloseToPtFirst function searches in the s object set =*/
/*= an object that is at less d object/figure units of the pt point.   =*/
/*= As soon as it founds a such object, it stops, fills and returns    =*/
/*= a TobjCloseToPt structure.                                         =*/
typedef struct {
    TobjAll obj;    // the selected object
    Tdble2  mkr;    // the nearest object marker to the pt point.
    Tdble2  shpt;   // the nearest object point to the pt point.
                    // when available otherwise set to mkr.
    int     idx;    // the index of marker in the selected object.
    Tdble2  ctr;    // the center of object
} TobjCloseToPt;
/*= If a such object does not exist in the s set, it returns the       =*/
/*= structure { OBJ_none, {0,0},{0,0},-1,(0,0) }.                      =*/
/*=                                                                    =*/
/*= The obj_getObjCloseToPtFirstNC function is similar to the former   =*/
/*= but skips the markers that are object center too.                  =*/


extern CobjAllSet* obj_getChildren   (TobjAll dir);
extern TobjAllSet* obj_getChildrenB  (TobjAll dir, CboxDble* bb);
extern TobjAllSet* obj_getChildrenD  (TobjAll dir, Cscdepth* depths);
extern TobjAllSet* obj_getChildrenM  (TobjAll dir, int msk);
extern TobjAllSet* obj_getChildrenBD (TobjAll dir, CboxDble* bb, Cscdepth* depths);
extern TobjAllSet* obj_getChildrenBM (TobjAll dir, CboxDble* bb, int msk);
extern TobjAllSet* obj_getChildrenBDM(TobjAll dir, CboxDble* bb,
                                      Cscdepth* depths, int msk);

extern TobjCloseToPt obj_getObjCloseToPtFirst  (CobjAllSet* objs, double d, Tdble2  pt);
extern TobjCloseToPt obj_getObjCloseToPtFirstNC(CobjAllSet* objs, double d, Tdble2  pt);

/*======================================================================*/
/*= Object center and markers.                                         =*/
/*=                                                                    =*/
/*= All these functions handle points of an figure object. When the    =*/
/*= documentation does not refer the unit of the point coordinates,    =*/
/*= the unit is the object unit (eg: obj->O_ppi).                      =*/
/*=                                                                    =*/
/*= The obj_center function returns the center of the obj object.      =*/
/*= The obj_markerNb function returns the marker number of the obj     =*/
/*= object.                                                            =*/
/*=                                                                    =*/
/*= The obj_markers function returns the markers of the obj object.    =*/
/*= The returned set must be freed by the caller after usage.          =*/
/*= The obj_markersUU function is similar to the obj_markers function  =*/
/*= except that the unit of the returned points is uu.                 =*/
/*=                                                                    =*/
/*= The obj_markersAndCtr function is similar to the obj_marker        =*/
/*= function. Furthermore it returns into *ctridx                      =*/
/*=   - either the index in returned set of the marker that is also    =*/
/*=     the object center,                                             =*/
/*=   - or -1 if the object center is not a marker.                    =*/
/*=     CAUTION: -1 does not mean that the returned set does not       =*/
/*=              contains the object center.                           =*/
/*=                                                                    =*/
/*= The obj_markerOpe function returns a mask that gives the marker    =*/
/*= operations that the obj object supports. The mask is a combination =*/
/*= of zero or more the FIGO_MkrOpe_... flags. A standard usage is:    =*/
/*=   int mask = FIGO_MkrOpe_Del | FIGO_MkrOpe_Ins;                    =*/
/*=   if ( (obj_markerOpe(obj)&mask)==mask ) {                         =*/
/*=     // we can suppress and add makers to the obj object.           =*/
/*=     ...                                                            =*/
/*=     }                                                              =*/
/*=                                                                    =*/
/*= The obj_markerIns function adds a marker before the idx th       =*/
/*= marker. So the idx th marker becomes the idx+1 th marker.          =*/
/*= The function returns the coordinates of the new marker.            =*/
/*= For adding a marker after the last marker, set idx to the number   =*/
/*= of markers.                                                        =*/
/*= If pos is not null, the new marker is set to *pos otherwise it     =*/
/*= is computed using the closest markers.                             =*/
/*= The function assumes that the obj object supports the operation    =*/
/*= and that idx is less or equal to the maker number.                 =*/
/*=                                                                    =*/
/*= The obj_markerChg function sets the idx th marker of the obj       =*/
/*= to the pt point.                                                   =*/
/*= The function assumes that the obj object supports the operation    =*/
/*= and that idx is less than the maker number.                        =*/
/*=                                                                    =*/
/*= The obj_markerDel function removes the idx th marker of the obj    =*/
/*= to the pt point.                                                   =*/
/*= The function assumes that the obj object supports the operation    =*/
/*= and that idx is less than the maker number.                        =*/
/*=                                                                    =*/
/*= Parameters:                                                        =*/
/*=   - obj    : The target figure object.                             =*/
/*=   - uu     : The point unit, a LUNIT_ID... macro.                  =*/
/*=   - ctridx : Return value.                                         =*/
/*=   - pos    : The marker to insert or NULL.                         =*/
/*=   - pt     : The marker to change.                                 =*/
/*=   - idx    : A marker index in the range [0,obj_markerNb(obj)[ or  =*/
/*=              [0,obj_markerNb(obj)] depending on the function.      =*/

extern Tdble2     obj_center       (TobjAll obj);
extern int        obj_markerNb     (TobjAll obj);
extern Tdble2Set* obj_markers      (TobjAll obj);
extern Tdble2Set* obj_markersUU    (TobjAll obj, int uu);
extern Tdble2Set* obj_markersAndCtr(TobjAll obj, int* ctridx);

#define FIGO_MkrOpe_Ins  0x01 // the Marker Insert operation is enabled.
#define FIGO_MkrOpe_Chg  0x02 // the Marker Change operation is enabled.
#define FIGO_MkrOpe_Del  0x04 // the Marker Delete operation is enabled.
extern int        obj_markerOpe    (TobjAll obj);
extern Tdble2     obj_markerIns    (TobjAll obj, int idx, Cdble2*pos);
extern void       obj_markerChg    (TobjAll obj, int idx, Tdble2 pt);
extern void       obj_markerChgUU  (TobjAll obj, int idx, Tdble2 pt, int uu);
extern void       obj_markerDel    (TobjAll obj, int idx);

/*======================================================================*/
/*= Drawing parameters.                                                =*/
/*=                                                                    =*/
/*= The obj_FdpRaw function copies into fdp the drawing/style          =*/
/*= parameters that are defined in the obj object. It returns a value  =*/
/*= that indicates which parameters have been updated. This value      =*/
/*= includes zero or more parameter mask (enum TstyleMask).            =*/
/*= CAUTION: The object parameters are copied RAWLY into the fdp       =*/
/*= structure so the fdp structure must not be freed.                  =*/
/*=                                                                    =*/
/*= The figdp_free/cmpMask/merge1/merge2 functions                     =*/
/*= are wrappers to the odsty_free/... functions taking into account   =*/
/*= the extra drawing parameters (fdp_depth, fdp_dir, ...) of the      =*/
/*= Tfdp structure.                                                    =*/
/*= See the style.h header for the documentation of these functions.   =*/

extern TstyleMask figdp_getFdpRaw(Tfdp*dp, CobjAll obj);

extern void       figdp_free   (Tfdp* fdp);
extern TstyleMask figdp_cmpMask(TstyleMask m, Cfdp*l,  Cfdp*r);
extern Tfdp       figdp_merge1(TstyleMask mall, Cfdp*all,
                               TstyleMask mset, Cfdp*set ); 
extern Tfdp       figdp_merge2(TstyleMask mall, Cfdp*all,
                               TstyleMask mset, Cfdp*set ); 

/*======================================================================*/
/*= Object services.                                                   =*/

extern TobjAll obj_newClone(CobjAll obj);
extern TobjAll obj_newLazy (CobjAll obj);
extern TobjAll obj_newChangingDP(CobjAll obj, Cfdp*dp, Toutdev*od);

extern void obj_doDrawData(CobjAll obj);
extern void obj_clrEGScache(CobjAll obj, void* od);

extern double obj_getLength(TobjAll obj);

/*======================================================================*/
/**
 * The obj_usedDepths function retrieves the depths of all the
 * objects of the dir object tree. The used depths are stored into
 * t array: t[i]=N means that there is at least 1 object at the N depth.
 * The t array is sorted in increasing order.
 *
 * The obj_allDepths function retrieves the depths of all the
 * objects of the dir object tree. The used depths are stored into
 * t array: t[i]=N means that there is N objects at the i depth.
 *
 * Returns:
 *   The both function return the number of elements that
 *   have been stored into the t table.
 *   For the obj_allDepths, it is always FIG_MAX_DEPTH.
 * 
 * Parameters:
 *  - dir: the object tree
 *  - t:   where the resulting depths are stored.
 *
 * Insure:
 *   The t table must be large enough to contain FIG_MAX_DEPTH
 *   elements.
**/
extern int   obj_usedDepths(CobjDir dir, int*     t);
extern int   obj_allDepths (CobjDir dir, Tscdepth*t);

extern void  obj_vtable_chg(Tobject obj, void*vt[FIG_KD_LowerFreeId]);

/*======================================================================*/
/*= Geometric transformations.                                         =*/

// Scaling attribues
#define FIGO_SA_AllDim      0x01
#define FIGO_SA_Text        0x02
#define FIGO_SA_Thickness   0x04
#define FIGO_SA_Arrow       0x08
#define FIGO_SA_All         0x0F

/**
 * The obj_translation function translates the self object with the
 * (tx,ty) vector: OM' = OM + (tx,ty).
 *
 * The obj_homothety00 function applies on the self object an homothety
 * the ratio being f and the center being (0,0).
 * The object components that the function changes are given by the wtcf
 * parameter (What To Change Flag):
 *   - Independently of the wtcf value:
 *       The points and dimensions of the object shape that are given
 *       object unit are always transformed.
 *   - If FIGO_SA_AllDim is set into the wtcf flag: 
 *       The dimensions of the object shape that are in a given unit
 *       are also scaled.
 *   - If FIGO_SA_Text is set into the wtcf flag: 
 *       The text components are scaled too.
 *   - If FIGO_SA_Thickness is set into the wtcf flag: 
 *       The line thickness of the line style of object is modified.
 *   - If FIGO_SA_Arrow is sets into the wtcf flag: 
 *       The size of the object arrows (if any) is modified too.
 * The obj_homothetyCC function is similar to the obj_homothety00
 * function but the center is the (cx,cy) point instead of (0,0).
 *
 * The obj_scale00/CC functions scales non-uniformly the self object,
 * the ratios being fx and fy and the center being (0,0) or (cx,cy).
 * The wctf flag is the same than the ones used in the obj_homothety00/CC
 * functions but the FIGO_SA_Thickness flag is ignored.
 *
 * The obj_rotate function rotates the self object by "a" radians (sens
 * trigonométrique), the center being (cx,cy).
 *
 * The obj_flip function performs in order the following geometric
 * transformations on the obj object:
 *  - h!=0: a horizontal symmetry (axis is vertical:   x=cx).
 *  - v!=0: a vertical   symmetry (axis is horizontal: y=cy).
 *  - r!=0: a rotation by r*90 degrees arround (cx,cy) center.
**/
extern void obj_translation(TobjAll self, double tx, double ty);
//extern void obj_homothetyCU(TobjAll self, double ppi);
extern void obj_homothety00(TobjAll self, int wtcf, double f);
extern void obj_homothetyCC(TobjAll self, int wtcf, double f, double cx, double cy);
extern void obj_scale00    (TobjAll self, int wtcf, double fx, double fy);
extern void obj_scaleCC    (TobjAll self, int wtcf, double fx, double fy,
                            double cx, double cy);
Inline void obj_zoom       (TobjAll self, double f)   { obj_homothety00(self,FIGO_SA_All,f); }
Inline void obj_zoomShape  (TobjAll self, double f)   { obj_homothety00(self,0,f); }
extern void obj_rotate     (TobjAll self, double a, double cx, double cy);
extern void obj_flip       (TobjAll self, int h, int v, int r, double cx, double cy);

/**
 * The obj_setPpi function sets the unit of the self object to ppi.
 * The other self object data are left unchanged.
 *
 * The obj_homoPpi function scales the object points by ppi/obj_Ppi(self).
 * The object PPI is left unchanged.
 *
 * The obj_chgPpi function changes the unit of the self object to ppi
 * by changing both the object point and the object PPI.
**/
extern void obj_setPpi (CobjAll self, double ppi); // 1 in obj = 1/ppi Inch
Inline void obj_homoPpi(TobjAll self, double ppi) { double f=ppi/obj_Ppi(self); if ( !dble_close0(f) ) obj_zoomShape(self,f ); }
Inline void obj_chgPpi (TobjAll self, double ppi) { double f=ppi/obj_Ppi(self); if ( !dble_close0(f) ) {
                                                    obj_zoomShape(self,f ); obj_setPpi(self,ppi); } }

/**
 * The obj_homotPlace applies on the self object an homothetic
 * transformation of the f ratio, then it places the north-west corner
 * of the resulting bounding box at the (px,py) position.
 * 
 * The obj_scalePlace function is similar to the former but
 * it performs a non-uniform scaling of fx and fy ratios.
**/
extern void obj_homotPlace ( Tobject self, double f,
                                 double px, double py);
extern void obj_scalePlace ( Tobject self,
                                 double fx, double fy,
                                 double px, double py);

/**
 * The obj_align0 function moves the objects of the s object set
 * according to the f align flag and the b reference box.
 *  - ALIGN_XSS: place object such as the smaller x of the object and
 *               the b box are the same.
 *               The y of the object is left unchanged.
 *  - ALIGN_XCC: place object such as the x centers of the object and
 *               the b box are the same.
 *               The y of the object is left unchanged.
 *  - ALIGN_XGG: place object such as the greater x of the object and
 *               the b box are the same.
 *               The y of the object is left unchanged.
 *  - ALIGN_YGG, ALIGN_YGG, ALIGN_YGG: these flags are similar to the
 *               ALIGN_X.. but the work on the y coordinate.
 * CAUTION: The function assumes that:
 *   - all the objects of s set use the same length unit (O_ppi). 
 *   - and this length unit is also the unit used for the b box.
 *
 * The obj_alignD function performs
 *   align0( self children, f, self bounding box)
 * and then resizes the self bounding box.
**/
typedef enum _TalignOpe {
    ALIGN_NONE=0,
    ALIGN_XSS=1<<0, ALIGN_XCC=2<<0, ALIGN_XGG=4<<0, 
    ALIGN_YSS=1<<4, ALIGN_YCC=2<<4, ALIGN_YGG=4<<4, 
} TalignOpe;
extern void obj_align0( CobjAllSet* s, TalignOpe f, CboxDble b);
extern void obj_alignD( TobjDir  self, TalignOpe f);

/*======================================================================*/
/*= Object set                                                         =*/

#define TPLSET objAll
#define TPLSET_Func(f) objAllSet_##f
#define TPLSET_AddWithoutPtr
#include "tools/set-header-template.h"

extern int  objAllSet_existObj   (TobjAllSet* s, TobjAll obj); // 1:exist ; 0:does't exist
extern int  objAllSet_indexObj   (TobjAllSet* s, TobjAll obj); //>=0:idx  ;<0:does't exist
extern void objAllSet_supObjQuick(TobjAllSet* s, TobjAll obj);
extern void objAllSet_supObjShift(TobjAllSet* s, TobjAll obj);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJECT_H
/*======================================================================*/
