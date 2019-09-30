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
 *      $Id: obj-vtable.h 386 2019-01-26 10:37:20Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/obj-vtable.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Definition of base operations that the objects must support to be  =*/
/*= handled by the figure module.                                      =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_OBJ_VTABLEI_H
#define FILE_OBJ_VTABLEI_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "object.h"

/*======================================================================*/

typedef struct _TobjVtable TobjVtable;
typedef const   TobjVtable CobjVtable;

#define FOBJ_NBCOLORS 5
/*======================================================================*/

struct _TobjVtable {

/*======================================================================*/
/*= Top information.                                                   =*/

    int         vt_kind;               // see FIG_KD_...
    int         vt_bytesz;             // the object byte size
    int         vt_mask;               // see FIG_KDM_...
    int         vt_subKind;            // the offset in the object structure
                                       // of an integer that defines the
                                       // sub-type of object (0 if none).
    int         vt_fdt;                // the offset in the object structure
                                       // of a Tfdp_text structure.
    int         vt_fdc;                // the offset in the object structure
                                       // of a Tfdp_curve structure.
    int         vt_fdl;                // the offset in the object structure
                                       // of a Tfdp_line structure.
    int         vt_fdf;                // the offset in the object structure
                                       // of a Tfdp_fill structure.
    int         vt_begArrow;           // the offset in the object structure
                                       // of a TobjAll union.
    int         vt_endArrow;           // the offset in the object structure
                                       // of a TobjAll union.

    CstrStatic   vt_name;                  // kind name
    CstrStatic  (*vt_nameF)(TobjAll obj);  // full kind name
    void        (*vt_dump) (CobjAll obj,   // dump obj object on stdout
                           int     sh);    // insert sh spaces at BOL
    double      (*vt_length)(TobjAll obj); // length of obj

/*======================================================================*/
/*= Constructors & Destructor.                                         =*/

    void     (*vt_vtable_chg)(         // change the virtual table of the obj
                Tobject  obj,          //   object tree to vt[vt_kind].
                void*    vt[FIG_KD_LowerFreeId]);
    /**
     * The vt_free function frees all the memory that the obj object 
     * allocated except of the colors defined in the vt_colors table.
     * The default function performs a free(obj).
     *
     * The vt_dup function returns a clone of obj (see obj_dup for details).
     * The vt_clone function duplicates all the dynamically allocated data
     * of the obj object except of colors defined in the vt_colors table.
     *
     * Both the vt_dup/clone function are used by the obj_dup function.
     * The obj_dup function must do all the work, the obj_clone symplifies
     * it implementation.
     * If your object has no dynamically allocated memory you can set
     * these 2 function pointers to NULL.
     * In any case no more than one of these function pointers must be defined
     * (if you set vt_dup, vt_clone will never be called).
    **/
    void     (*vt_free)  (TobjAll obj);
    TobjAll  (*vt_dup)   (CobjAll obj);          
    void     (*vt_clone) (TobjAll obj);

    /**
     * The vt_dupNDP function creates a clone of the obj object changing
     * the drawing parameters by those of the att structure.
     *
     * Its implementation is mandatory.
    **/
    TobjAll  (*vt_dupNDP)(CobjAll obj, Cfdp*fdp, Toutdev*od);          

    TboxDble (*vt_boundBox)(CobjAll obj);

/*======================================================================*/
/*= Geometric transformation.                                          =*/
/*=                                                                    =*/
/*= Mandatory functions:                                               =*/
/*=   vt_translation, vt_scale,                                        =*/
/*=   vt_flip                                                          =*/
/*=                                                                    =*/
/*= Optional functions:                                                =*/
/*=   vt_homothety00   default is vt_homothetyCC(obj,wtcf,f,0,0)       =*/
/*=                            or v_scale00(obj,wtcf,f,f)              =*/
/*=   vt_homothetyCC   default is vt_scaleCC(obj,wtcf,fx,fy,cx,cy)     =*/
/*=   vt_scale00       default is vt_scaleCC(obj,wtcf,fx,fy,0,0)       =*/
/*=   vt_rotate        default is vt_translation, the translation      =*/
/*=                    vector being the CC' vector where C is the      =*/
/*=                    object center and where C' the rotation of C.   =*/
/*=                                                                    =*/
/*= All these functions change only coordinates, dimensions of the     =*/
/*= object. THEY DO'NT TOUCH the LINE and FILL STYLES.                 =*/

    // The vt_translate function translates the obj object using the
    // (tx,ty) vector (OM' = OM + (tx,ty)).
    // The function must only change the object points.
    // It must modify neither the bounding box nor the style.
    void (*vt_translation)(TobjAll obj, double tx, double ty);

    // The vt_homothety00 function does an uniform transformation 
    // of the obj object with (0,0) center and a scale factor of f.
    // The function must always change the object points and dimensions
    // that are given in the object unit (eg: radius of a circle).
    // If FIGO_SA_AllDim is set in the wtcf flag then
    //   the dimensions in fixed unit must be changed too.
    // If FIGO_SA_Text is set in the wtcf flag then
    //   text font must also be scaled.
    // The other FIG0_SA_... flags must be ignored.
    //
    // CAUTION: 
    //  - The object bounding box is already updated when the function
    //    is invoked.
    //  - The line and fill styles must not be altered.
    //  - The text styles must be updated only when FIGO_SA_Text is
    //    set in wtcf.
    //
    // The vt_homothetyCC function is similar to the vt_homothety00
    // function except that the (cx,cy) point is the center.
    //
    // The vt_scale00/CC functions are similar to the vt_homothety00/CC
    // functions except that they do a non-uniform similitude with the
    // fx and fy scale factors.
    void (*vt_homothety00)(TobjAll obj, int wtcf, double f);
    void (*vt_homothetyCC)(TobjAll obj, int wtcf, double f, double cx, double cy);
    void (*vt_scale00)    (TobjAll obj, int wtcf, double fx, double fy);
    void (*vt_scaleCC)    (TobjAll obj, int wtcf, double fx, double fy,
                                        double cx, double cy);

    // The vt_rotate function rotates the obj object by "a" radians (in
    // sens trigonométrique) around the (cx,cy) center.
    //
    // CAUTION: 
    //  - The tags FIG_TAG_BBoxUpToDate and FIG_TAG_DrawingDataReady are
    //    cleared before the function invocation.
    //  - The line and fill styles must not be altered.
    void (*vt_rotate)(TobjAll obj, double a, double cx, double cy);

    // The vt_flip function performs in order the following geometric
    // transformations on the obj object:
    //   - h!=0: a horizontal symmetry (axis is vertical:   x=cx).
    //   - v!=0: a vertical   symmetry (axis is horizontal: y=cy).
    //   - r!=0: a rotation by r*90 degrees arround (cx,cy) center.
    // CAUTION: 
    //  - The object bounding box is already updated when the function
    //    is invoked.
    //  - The line and fill styles must not be altered.
    void (*vt_flip)(TobjAll obj, int h, int v, int r, double cx, double cy);

/*======================================================================*/
/*= Drawing and filling.                                               =*/

    void     (*vt_doDrawData)(CobjAll obj);
    /**
     * The vt_draw_sc function draws the obj object on the device of the
     * sc scene using the gcd th graphic context and fills it using the
     * gcf GC.
     * The possible value of gcd and gcf are:
     *      gcd             gcf
     *       *           OD_GC_None    deaw only
     *   OD_GC_None          *         fill only
     *       *               *         draw and fill
     *   OD_GC_None      OD_GC_None    nothing to do
    **/
    void   (*vt_draw_sc)(CobjAll   obj, Tfig_scene* sc,
                         Tod_GCdef gcd, Tod_GCdef gcf);
    /**
     * The vt_draw function draws the obj object on the od device
     * using the gcd th graphic context.
     *
     * Insure:
     *  gcd is a true GC fully initialized for drawing (not something
     *  like OD_GC_None, ...)
    **/
    void   (*vt_draw)   (CobjAll    obj, Toutdev* od, Tod_GCdef gcd);

    void   (*vt_clrEGScache)(CobjAll    obj, Toutdev* od);

/*======================================================================*/
/*= Arrow handler.                                                     =*/

    /**
     * The vt_arrowDoBegLine function generates the data used for
     * drawing the arrow placed at the beginning (pts[0]) of the 
     * multi-line defined by the pts point set.
     * It can also adapt the pts set to the arrow, the caller must
     * draw this multi-line instead of the initial one.
     * It can also detect that arrow is not drawable, in this case
     * pts is left unchanged, arrow drawing is disabled and an error
     * message is triggered.
     *
     * The vt_arrowDoEndLine function is similar to the vt_arrowDoBegLine
     * function but the arrow is drawn at the multi-line end (pts[ptsNb-1]).
     * 
     * The vt_arrowDoBegSpline and vt_arrowDoEndSpline function are similar
     * except that pts gives the control points of spline curve.
     * 
     * The vt_arrowDoBegArc and vt_arrowDoEndArc function are similar
     * except that pts gives the control points of the arc:
     *   pts[0]:arc being, pts[1]:arc middle, pts[2]:arc end,
     *   pts[3]:center.
    **/
    void   (*vt_arrowDoBegLine)   (CobjAll obj, Tdble2Set* pts);
    void   (*vt_arrowDoEndLine)   (CobjAll obj, Tdble2Set* pts);
    void   (*vt_arrowDoBegSpline) (CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes);
    void   (*vt_arrowDoEndSpline) (CobjAll obj, Tdble2Set* ctls, TdbleSet* ndes);
    void   (*vt_arrowDoBegArc)    (CobjAll obj, Tdble2 pts[4], int dir);
    void   (*vt_arrowDoEndArc)    (CobjAll obj, Tdble2 pts[4], int dir);

/*======================================================================*/
/*= Localization.                                                      =*/
/*=                                                                    =*/
/*= vt_markerBB     : use the bounding box for markers.                =*/
/*= vt_markerNb     : the number of makers. It is a constant.          =*/
/*= vt_markerNbF    : gives the number of makers.                      =*/
/*= vt_markerNbMinF : gives the minimal number of makers.              =*/
/*= vt_markerNbCstF : True indicates the marker number is a constant.  =*/
/*=                                                                    =*/
/*=  BB  Nb  NbF  NbMinF NbCstF                                        =*/
/*=   1  0   0      0      0        4 makers (BB vertices)             =*/
/*=   0  N   0      0      0        N makers, can not be changed       =*/
/*=   0  0   N      0      1        N makers, can not be changed       =*/
/*=   0  0   N      M      0        M<=N, currently N makers,          =*/
/*=                                 can be increased infinitely,       =*/
/*=                                 can be decreased until M.          =*/
/*=                                                                    =*/
/*= Assuming that N is the current marker number, one can get them:    =*/
/*= vt_markerT  : The offset of a table that contains the N markers.   =*/
/*= vt_markerS  : The offset of a set that contains the N markers.     =*/
/*= vt_markerTF : returns a table that contains the N markers.         =*/
/*= vt_markerSF : returns a set that contains the N markers.           =*/
/*= vt_markerIF : returns the i th marker.                             =*/
/*= vt_markerFF : fill in the t table with the N markers.              =*/
/*=                                                                    =*/
/*=  BB  T  S  TF SF IF FF      BB  T  S  TF SF IF FF                  =*/
/*=  1   0  0  0  0  0  0       0   0  0  0  *  0  0                   =*/
/*=  0   *  0  0  0  0  0       0   0  0  0  0  *  0                   =*/
/*=  0   0  *  0  0  0  0       0   0  0  0  0  0  *                   =*/
/*=  0   0  0  *  0  0  0                                              =*/
/*=                                                                    =*/
/*= The next fields specify the center of object that is required for  =*/
/*= some geometric transformations.                                    =*/
/*= vt_centerMkr    : The center is also a marker whose                =*/
/*= vt_centerMkrIdx : index is given by this field.                    =*/
/*= vt_centerF      : returns the center.                              =*/
/*=                                                                    =*/
/*=  BB  Mkr MkrIdx F                                                  =*/
/*=   1  0     0    0 : the center is the BB center                    =*/
/*=   0  1     N    0 : the N th marker is the center                  =*/
/*=   0  0     0    f : f(obj) returns the center                      =*/
/*=   0  0     0    0 : the center is the gravity center (barycentre)  =*/
/*=                     of makers.                                     =*/
/*=                                                                    =*/
/*= The closeto function indicates if the obj shape is close to the    =*/
/*= pt point (the distance is less than d).                            =*/
/*= If the point is too far, it returns -1, otherwise                  =*/
/*=  - it puts into *shpt the shape point that is the closest of pt,   =*/
/*=  - it returns the index of the marker that is the closest of shpt. =*/
/*= If this function is not implemented, the API provides a default    =*/
/*= based on the markers.                                              =*/

    int vt_markerBB,vt_markerNb;
    int (*vt_markerNbF)   (TobjAll obj);
    int (*vt_markerNbMinF)(TobjAll obj);
    int (*vt_markerNbCstF)(TobjAll obj);
                                        
    int vt_markerT,vt_markerS;
    Cdble2*    (*vt_markerTF) (TobjAll obj);
    Cdble2Set* (*vt_markerSF) (TobjAll obj);
    Cdble2     (*vt_markerIF) (TobjAll obj, int idx); 
    void       (*vt_markerFF) (TobjAll obj, Tdble2* t); 

    int vt_centerMkr,vt_centerMkrIdx;
    Cdble2     (*vt_centerF) (TobjAll obj); 

    int (*vt_closeto)(TobjAll obj, double d,
                      Tdble2  pt, Tdble2 *shpt);

/*======================================================================*/
/*= Modification.                                                      =*/
/*=                                                                    =*/
/*= The vt_mkrOpeInsert function adds a marker before the idx th       =*/
/*= marker. So the idx th marker becomes the idx+1 th marker.          =*/
/*= The function returns the coordinates of the new marker.            =*/
/*= For adding a marker after the last marker, set idx to the number   =*/
/*= of markers.                                                        =*/
/*= If pos is not null, the new marker is set to *pos otherwise it     =*/
/*= is computed using the closest markers.                             =*/
/*=                                                                    =*/
/*= The vt_mkrOpeChange function sets the idx th marker to pt.         =*/
/*=                                                                    =*/
/*= The vt_mkrOpeDelete function suppresses the idx th marker.         =*/
/*=                                                                    =*/
/*= The vt_mkrOpeEnabled function returns zero or several of the       =*/
/*= following flags:                                                   =*/
/*=   - FIGO_MkrOpe_Ins: the mkrInsert operation is enabled.           =*/
/*=   - FIGO_MkrOpe_Chg: the mkrChange operation is enabled.           =*/
/*=   - FIGO_MkrOpe_Del: the mkrDelete operation is enabled.           =*/
/*= A null function pointer means that the corresponding operation     =*/
/*= is not supported by the object.                                    =*/
/*=    ins  chg  del  OpeEnabled.                                      =*/
/*=    null null null   null         no operation is allowed.          =*/
/*=     f    g    h     null         all the operations are allowed.   =*/
/*=    null  g    h     null         2 operations are allowed.         =*/
/*=     f    g    h     x            3 operations are defined, x       =*/
/*=                                  indicates those that are allowed  =*/
/*=                                  for a specific object.            =*/
/*=                                                                    =*/
/*= The vt_mkrOpeInsert, vt_mkrOpeChange and vt_mkrOpeDelete function  =*/
/*= are invoked directly without post or pre treatment. So in addition =*/
/*= to the marker operation, they must handle the bouding box and      =*/
/*= their all own data too.                                            =*/
    Tdble2 (*vt_mkrOpeInsert)   (TobjAll obj, int idx, Cdble2*pos);
    void   (*vt_mkrOpeChange)   (TobjAll obj, int idx, Tdble2 pt);
    void   (*vt_mkrOpeDelete)   (TobjAll obj, int idx);
    int    (*vt_mkrOpeEnabled)  (TobjAll obj);
};

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif // FILE_OBJ_VTABLEI_H
/*======================================================================*/
