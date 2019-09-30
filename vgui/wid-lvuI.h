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
 *      $Id: wid-lvuI.h 376 2018-12-08 18:01:09Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-lvuI.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * "Label Value Unit" composite widget.
 *
 * This header defines the functions that are shared by the Twid_olvu
 * and Twid_ilvu implementation.
 *
 * It is private to the LVU widget.
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_LVUI_H
#define FILE_WID_LVUI_H
#include "vgui/vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/wid-lvu.h"

/*======================================================================*/
/*= Functions                                                          =*/

// for numeric widgets only
// Wu=Widget Unit ; Fu=FIG unit (reference unit)
extern double lvu_Wu2Fu(Cwid_lvuBase0* self, double val);
extern double lvu_Fu2Wu(Cwid_lvuBase0* self, double val);

#define Wu2Fu(self, val) lvu_Wu2Fu(&self->base,val)
#define Fu2Wu(self, val) lvu_Fu2Wu(&self->base,val)

// callback to set to the unit DDL widget that updades the
// printing of the value numeric widget. 
extern void lvu_cb_UnitChg (Twid_lenunit*lu, int unit, void*ud);

/*======================================================================*/
/*= adding / building / freing options                                 =*/

extern void   lvu_labelAdd (Twid_lvuBase0* self,
                            CstrBrief text, CstrBrief width);
extern void   lvu_labelDBF (Twid_lvuBase0* self);
extern void   lvu_labelFree(Twid_lvuBase0* self);

extern void   lvu_unitLabAdd (Twid_lvuBase0* self,
                              CstrBrief text, CstrBrief width);
extern void   lvu_unitLabDBF (Twid_lvuBase0* self);
extern void   lvu_unitLabFree(Twid_lvuBase0* self);

extern void   lvu_unitStdAdd (Twid_lvuBase0* self);
extern void   lvu_unitStdDBF (Twid_lvuBase0* self, Twid_cb_LiU cb);
extern void   lvu_unitStdFree(Twid_lvuBase0* self);
extern void   lvu_unitStdChg (Cwid_lvuBase0* self, int unit);

extern void   lvu_unitBtnAdd (Twid_lvuBase0* self, Twid_button* btn);
extern void   lvu_unitBtnDBF (Twid_lvuBase0* self);
extern void   lvu_unitBtnFree(Twid_lvuBase0* self);

extern void   lvu_delete     (Twid_lvuBase0* self);

/*======================================================================*/
/*= get and change values and control                                  =*/

extern CstrBrief
              wid_lvu_getStr(Cwid_lvuBase* self);
extern void   wid_lvu_setStr(Twid_lvuBase* self, CstrBrief v);

extern double wid_lvu_getNum10(Cwid_lvuBase* self);
extern double wid_lvu_getNum01(Cwid_lvuBase* self);
extern void   wid_lvu_setNum10(Twid_lvuBase* self, double v);
extern void   wid_lvu_setNum01(Twid_lvuBase* self, double v2);
extern void   wid_lvu_setNum11(Twid_lvuBase* self, double v, double v2);

extern void   wid_lvu_reset   (Twid_lvuBase* self);
extern void   wid_lvu_clear   (Twid_lvuBase* self);
extern void   wid_lvu_unshow  (Twid_lvuBase* self);
extern void   wid_lvu_show    (Twid_lvuBase* self);
extern void   wid_lvu_disable (Twid_lvuBase* self);
extern void   wid_lvu_enable  (Twid_lvuBase* self);

/*======================================================================*/
/*= Constructors of value item                                         =*/

extern Tlvu_valBase* lvu_newOValTxtLab(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width);

extern Tlvu_valBase* lvu_newOValClrLab(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width,
                        Ccolor*   ic, int dx, int dy);

extern Tlvu_valBase* lvu_newOValNumLab10(Twid_olvu* ow,
                        double  iv,
                        CstrBrief  width, int dx, int dy);
extern Tlvu_valBase* lvu_newOValNumLab11(Twid_olvu* ow,
                        double  iv, double iv2,
                        CstrBrief  width, int dx, int dy);

extern Tlvu_valBase* lvu_newIValTxt(Twid_olvu* ow,
                        CstrBrief  iv, CstrBrief  width);

extern Tlvu_valBase* lvu_newIValInt(Twid_olvu* ow,
                        double  iv, int min, int max,
                        CstrBrief  width, int dx, int dy);

extern Tlvu_valBase* lvu_newIValDle(Twid_olvu* ow,
                        double  iv, double min, double max,
                        CstrBrief  width, int dx, int dy);

extern Tlvu_valBase* lvu_newOValDdlStr(Twid_lvuBase0* _lvu,
                     int  iv,char**strs,
                     Twid_cb_Uap cb, void*ud);
extern Tlvu_valBase* lvu_newIValDdlStr(Twid_ilvu* lvu,
                     int  iv,char**strs);

extern Tlvu_valBase* lvu_newOValDdlPxm(Twid_lvuBase0* _lvu,
                        int  iv,TpixmapEGS*ps, int pn,
                        int dx, int dy, Twid_cb_Uap cb, void*ud);
extern Tlvu_valBase* lvu_newIValDdlPxm(Twid_ilvu* lvu,
                     int  iv,TpixmapEGS*ps, int pn, int dx, int dy);
    
/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_LVUI_H
/*======================================================================*/
