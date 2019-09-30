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
 *      $Id: wid-lvu.h 140 2018-05-27 12:13:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-lvu.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * "Label Value Unit" composite widget.
 *
 * These widgets show a value with a label and an third widget that we
 * call the unit.
 *
 * Its layout is: 
 *    label   value  unit
 *
 * The label and unit items are optional.
 * There are 2 main types:
 *   - Twid_olvu : (Output Label Value Unit) it shows the 3 items  
 *   - Twid_ilvu : (Input Label Value Unit) it also shows the 3 items,
 *                 but the user can change the value through the GUI.
 *
 * The table below summarizes the configurations supported by this widget:
 *    +-----------------+-------+-----------+-----------+
 *    |                 | label | value     |  unit     |
 *    +-----------------+-------+-----------+-----------+
 *    | optional        |   *   |           |   *       |
 *    +-----------------+-------+-----------+-----------+
 *    | widget for olvu | label | label     | label     |
 *    |                 |       | slout     | lenunit   |
 *    |                 |       | label-clr | boutton   |
 *    |                 |       | pxm DDL   | label-clr |
 *    +-----------------+-------+-----------+-----------+
 *    | widget for ilvu | label | slin      | label     |
 *    |                 |       | spinner   | lenunit   |
 *    |                 |       | pxm DDl   | boutton   |
 *    |                 |       |           | label-clr |
 *    +-----------------+-------+-----------+-----------+
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_LVU_H
#define FILE_WID_LVU_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "wid-simple.h"

/*======================================================================*/
/*= Base types                                                         =*/

typedef struct _Tlvu_valVtable Tlvu_valVtable;
typedef struct _Twid_lvuBase   Twid_lvuBase;
typedef const   Twid_lvuBase   Cwid_lvuBase;

// define the options
enum _Twid_lvuOpt {
    LVUOPT_None    = 0x0000, // just for initializing
    LVUOPT_Label   = 0x0001, // has a label widget
    LVUOPT_Unit    = 0x0008, // has an unit widget

    LVUOPT_UnitLab = 0x1008, // has an unit widget (label)
    LVUOPT_UnitDDL = 0x2008, // has an unit widget (DDL)
    LVUOPT_UnitBtn = 0x4008, // has an unit widget (DDL)

    LVUOPT_FreePxm = 0x10000, // frees pixmaps if any
};
typedef enum   _Twid_lvuOpt   Twid_lvuOpt;

struct _Tlvu_valVtable {
    void        (*dbf)      (Twid_lvuBase* lvu);
    void        (*free)     (Twid_lvuBase* lvu);
    Tegs_widget (*widEGS)   (Twid_lvuBase* lvu);
    Twidm_ddl*  (*valDdl)   (Twid_lvuBase* lvu);
    void        (*print)    (Twid_lvuBase* lvu);
    CstrBrief   (*getText)  (Twid_lvuBase* lvu);
    void        (*setText)  (Twid_lvuBase* lvu, CstrBrief v);
    double      (*getNum10) (Twid_lvuBase* lvu);
    double      (*getNum01) (Twid_lvuBase* lvu);
    void        (*setNum10) (Twid_lvuBase* lvu, double v);
    void        (*setNum01) (Twid_lvuBase* lvu, double v2);
    void        (*setNum11) (Twid_lvuBase* lvu, double v,double v2);
    void        (*reset)    (Twid_lvuBase* lvu);
    void        (*clear)    (Twid_lvuBase* lvu);
    void        (*disable)  (Twid_lvuBase* lvu);
    void        (*enable)   (Twid_lvuBase* lvu);
    void        (*setBgClr) (Twid_lvuBase* lvu, Ccolor* c);
    void        (*setFgClr) (Twid_lvuBase* lvu, Ccolor* c);
    void        (*setBgPxm) (Twid_lvuBase* lvu, TpixmapEGS tile);
    void        (*setBgBtm) (Twid_lvuBase* lvu, Cvgui_icon* icon);
    Trange*     (*getRange) (Twid_lvuBase* lvu);
    void        (*unitChged)(Twid_lvuBase* lvu, int newunit);
    void        (*chgValMin)(Twid_lvuBase* lvu, double v);
    void        (*chgValMax)(Twid_lvuBase* lvu, double v);
} ;

typedef struct _Tlvu_valBase0  Tlvu_valBase0;
struct _Tlvu_valBase0 {
    Tlvu_valVtable* vtable;
};

typedef struct _Tlvu_valBase  Tlvu_valBase;
typedef const   Tlvu_valBase  Clvu_valBase;
struct _Tlvu_valBase {
    Tlvu_valBase0 base;
};

typedef struct _Tlvu_valText   Tlvu_valText;
typedef struct _Tlvu_valNum    Tlvu_valNum ;
typedef struct _Tlvu_ivalText  Tlvu_ivalText;
typedef struct _Tlvu_ivalNum   Tlvu_ivalNum ;
typedef struct _Tlvu_valDdlStr Tlvu_valDdlStr ;
typedef struct _Tlvu_valDdlPxm Tlvu_valDdlPxm ;

/*======================================================================*/
/*= Base main type                                                     =*/

typedef struct _Twid_lvuBase0 Twid_lvuBase0;
typedef const   Twid_lvuBase0 Cwid_lvuBase0;

struct _Twid_lvuBase0 {
    Twid_lvuOpt     options;     // options
    Tvgui_dw*       vdw;         // VGUI dialog widget

    Tegs_widget     labEGS;      // the label item widget or null
    Tegs_widget     valEGS;      // the value item widget
    Tegs_widget     uniEGS;      // the unit  item widget or null
#define lvu_options          base.options
#define lvu_vdw              base.vdw
#define lvu_labEGS           base.labEGS
#define lvu_valEGS           base.valEGS
#define lvu_uniEGS           base.uniEGS

  /* fields for the label item */
    TstrEaten      labText,labWidth;
    Tegs_widget    labWidget;   // label widget
#define lvu_labText          base.labText
#define lvu_labWidth         base.labWidth
#define lvu_labWidget        base.labWidget

  /* fields for the value item */
    union {
        Tlvu_valBase   *valBase;
        Tlvu_valText   *valText;
        Tlvu_valNum    *valNum;
        Tlvu_ivalText  *ivalText;
        Tlvu_ivalNum   *ivalNum;
        Tlvu_valDdlStr *valDdlStr;
        Tlvu_valDdlPxm *valDdlPxm;
    } valItem;
#define lvu_valBase   base.valItem.valBase
#define lvu_valVtable lvu_valBase->base.vtable
#define lvu_valText   base.valItem.valText
#define lvu_valNum    base.valItem.valNum 
#define lvu_ivalText  base.valItem.ivalText
#define lvu_ivalNum   base.valItem.ivalNum 
#define lvu_valDdlStr base.valItem.valDdlStr 
#define lvu_valDdlPxm base.valItem.valDdlPxm 

  /* fields for the unit item */
    TstrEaten      uniText;       // only used if label  
    TstrEaten      uniWidth;
#define lvu_uniText      base.uniText
#define lvu_uniWidth     base.uniWidth
    union {
      void*           misc;
      Tegs_widget     label;   // label widget
      Twid_lenunit*   ddl;     // lenunit DDL (drop down list)
      Twid_button*    btn; 
    } uniWidget;
#define lvu_uniMisWidget base.uniWidget.misc
#define lvu_uniLabWidget base.uniWidget.label
#define lvu_uniDdlWidget base.uniWidget.ddl
#define lvu_uniBtnWidget base.uniWidget.btn
} ;

/*======================================================================*/
/*= widget types                                                       =*/

struct _Twid_lvuBase {
    Twid_lvuBase0 base;
};

struct _Twid_olvu {
    Twid_lvuBase0 base;
};

struct _Twid_ilvu {
    Twid_lvuBase0 base;
    Twid_cb_Uap   lvu_cbVld;  // called when the value is VaLiDated
                              // (eg: return is typed)
    Twid_cb_Uap   lvu_cbChg;  // called when the value CHanGes
    void*         lvu_ud;     // User Data transmits to callbacks
};

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_LVU_H
/*======================================================================*/
