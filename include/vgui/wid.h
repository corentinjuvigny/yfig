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
 *      $Id: wid.h 370 2018-11-23 00:35:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * Base widget classes.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_H
#define FILE_WID_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

//#include "vgui-types.h"
//#include "vgui-dw.h"
#include "wid-simple.h"

/*======================================================================*/

#if 1
typedef struct _Twid_anyVtable Twid_anyVtable;
typedef const   Twid_anyVtable Cwid_anyVtable;
typedef struct _Twid_widVtable Twid_widVtable;
typedef const   Twid_widVtable Cwid_widVtable;
typedef struct _Twid_boxVtable Twid_boxVtable;
typedef const   Twid_boxVtable Cwid_boxVtable;
typedef struct _Twid_grpVtable Twid_grpVtable;
typedef const   Twid_grpVtable Cwid_grpVtable;
#else
typedef struct _Twid_grpVtable Twid_grpVtable;
typedef const   Twid_grpVtable Cwid_grpVtable;
#endif

typedef struct _Twid_any Twid_any;
typedef const   Twid_any Cwid_any;
typedef struct _Twid_wid Twid_wid;
typedef const   Twid_wid Cwid_wid;
typedef         Tvgui_dw Twid_box;
typedef         Cvgui_dw Cwid_box;


typedef struct _Twid_grp Twid_grp;
typedef const   Twid_grp Cwid_grp;

typedef         Tegs_widget TwidEGS;
#define WIDEGS_none NULL

typedef void (*Twid_widCB)(Twid_wid*wid, void*ud);
typedef void (*Twid_boxCB)(Twid_box*box, void*ud);
typedef void (*Twid_grpCB)(Twid_grp*grp, void*ud);

Inline Twid_any* Wid2Any(Twid_wid* wid) { return (Twid_any*)wid; }
Inline Twid_wid* Box2Wid(Twid_box* box) { return (Twid_wid*) box; }
Inline Twid_any* Box2Any(Twid_box* box) { return (Twid_any*)box; }
Inline Twid_any* Grp2Any(Twid_grp* grp) { return (Twid_any*)grp; }

/*======================================================================*/
/*= Twid_any                                                           =*/

#define WID_ANY_FIELDS()   \
    Twid_anyVtable *wa_vtable;    \
    Tvgui*          wa_vgui;      \
    Twid_box*       wa_parent    /* parent of this widgets */

struct _Twid_any { WID_ANY_FIELDS(); };

struct _Twid_anyVtable {
    int              bytesize;
    Twid_anyVtable*  bcvtable; // the vtable of the base class
    void           (*free)      (Twid_any* any);
    Tvgui_mw*      (*getMainWid)(Twid_any* wid);
    Twid_box*      (*getBoxWid) (Twid_any* wid);
    Twid_box*      (*getDialWid)(Twid_any* wid);

    Twid_widVtable* widvtable;
    Twid_boxVtable* boxvtable;
    Twid_grpVtable* grpvtable;
    void*           usrvtable;
};

extern void wid_any_delete(Twid_any*any);

/*======================================================================*/
/*= Twid_wid                                                           =*/

#define WID_WID_FIELDS() WID_ANY_FIELDS(); \
    TwidEGS      ww_widEGS;    /* the EGS widget see by parent (external view) */ \
    void*        ww_widEGSud;  /* user-data dedicated to EGS driver */ \
    Twid_widCB   appCB;     /* appication callback */ \
    void*        appCBud    /* userdata for appCB */ 

struct _Twid_wid { WID_WID_FIELDS(); };

/*======================================================================*/
/*= Twid_box                                                           =*/
#define TBXSB_None 0
typedef Twid_dialStatus  Twid_status;

#define WID_BOX_FIELDS() WID_WID_FIELDS(); \
    Tvgui_mw*    wb_vmw    /* associated main window */

extern Twid_anyVtable wid_anyVtable_tbx;
extern Twid_boxVtable wid_boxVtable_tbx;

#define WID_ANY_SCVT_TBX(bsz,bcvt,freefunc,usrvt) \
    .bytesize=bsz, .bcvtable=bcvt, .free=freefunc, .usrvtable=usrvt, \
    .boxvtable=&wid_boxVtable_tbx

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_box* WIDBOX(void*x);
extern TwidEGS   wid_box_widEGS(Twid_box*box);

extern Twid_box* wid_tbx_new    (Twid_anyVtable* vt,Tvgui_mw* vmw);
extern Twid_box* wid_tbx_newBtn (Twid_anyVtable* vt,Tvgui_mw* vmw, Twid_status btn);
extern Twid_box* wid_tbx_newT   (Twid_anyVtable* vt,Tvgui_mw* vmw, TstrBrief fmt, ...);
extern Twid_box* wid_tbx_newBtnT(Twid_anyVtable* vt,Tvgui_mw* vmw,
                                 Twid_status btn, TstrBrief fmt, ...);
Inline void      wid_box_delete (Twid_box*box)    { wid_any_delete( Box2Any(box) ); }

/*======================================================================*/
/*= Twid_grp                                                           =*/

#define WID_GRP_FIELDS() WID_ANY_FIELDS(); \
    Twid_grpCB   appCB;   /* appication callback */ \
    void*        appCBud  /* userdata for appCB */

struct _Twid_grp { WID_GRP_FIELDS(); };

struct _Twid_grpVtable {
    TwidEGS  (*lioEGS)    (Cwid_grp* grp);  // main label
    TwidEGS  (*xioEGS)    (Cwid_grp* grp);  // 1st value (x coordinate)
    TwidEGS  (*yioEGS)    (Cwid_grp* grp);  // 2nd value (y coordinate)
    TwidEGS  (*uioEGS)    (Cwid_grp* grp);  // unit widget

    void     (*clear)     (Twid_grp* grp);
    void     (*enable)    (Twid_grp* grp);
    void     (*disable)   (Twid_grp* grp);

    void     (*updMin)    (Twid_grp* grp, double v);
    void     (*updMax)    (Twid_grp* grp, double v);
    void     (*updInc)    (Twid_grp* grp, double v);
    double   (*getDble10) (Twid_grp* grp);
    double   (*getDble01) (Twid_grp* grp);
    Tdble2   (*getDblePt) (Twid_grp* grp);
    void     (*setDble10) (Twid_grp* grp, double v);
    void     (*setDble01) (Twid_grp* grp, double v);
    void     (*setDblePt) (Twid_grp* grp, Tdble2 pt);

    void     (*chgUsrUnit)(Twid_grp* grp, int uu);
};

//extern Twid_anyVtable wid_anyVtable_grp;
#define WID_ANY_SCVT_GRP(bsz,freefunc,grpvt) \
    .bytesize=bsz, .free=freefunc, .grpvtable=&grpvt

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern Twid_box* WIDGRP(void*x);

extern Twid_grp* wid_grp_newEmpty   (Twid_anyVtable*vt);
extern Twid_grp* wid_grp_newByBox   (Twid_anyVtable*vt, Twid_box*box);
Inline void      wid_grp_delete     (Twid_grp* grp) { wid_any_delete( Grp2Any(grp) ); }
Inline void      wid_grp_deleteN    (Twid_grp**grp) { wid_grp_delete(*grp); *grp=0; }

extern TwidEGS   wid_grp_lioEGS(Cwid_grp* grp);
extern TwidEGS   wid_grp_xioEGS(Cwid_grp* grp); 
extern TwidEGS   wid_grp_yioEGS(Cwid_grp* grp); 
extern TwidEGS   wid_grp_uioEGS(Cwid_grp* grp); 

extern void      wid_grp_chgUsrUnit (Twid_grp*grp, int uu);

extern void      wid_grp_clear    (Twid_grp*grp);
extern void      wid_grp_enable   (Twid_grp*grp);
extern void      wid_grp_disable  (Twid_grp*grp);

extern void      wid_grp_updMin   (Twid_grp* grp, double v);
extern void      wid_grp_updMax   (Twid_grp* grp, double v);
Inline void      wid_grp_updMinMax(Twid_grp* grp, double min, double max) { wid_grp_updMin(grp,min); wid_grp_updMax(grp,max); }
extern double    wid_grp_getDble10(Twid_grp*grp);
extern double    wid_grp_getDble01(Twid_grp*grp);
extern Tdble2    wid_grp_getDblePt(Twid_grp*grp);
extern void      wid_grp_setDble10(Twid_grp*grp, double v);
extern void      wid_grp_setDble01(Twid_grp*grp, double v);
extern void      wid_grp_setDblePt(Twid_grp*grp, Tdble2 pt);

/*======================================================================*/

#if 0
#define WID_ANY_FIELDS() \
    Twid_anyVtable *vtable;    \
    Tvgui*       gui;          \
    Twid_box*    parent;  /* parent of the internal widget */    \
    Twid_grpCB   appCB;   /* appication callback */              \
    Twid_grpCB   appCBud; /* userdata for appCB */               \
    TwidEGS      widEGS   /* the EGS widget that the parent views */

struct _Twid_any { WID_ANY_FIELDS(); };
#endif

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
#endif // FILE_WID_H
/*======================================================================*/
