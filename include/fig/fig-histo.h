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
 *      $Id: fig-histo.h 374 2018-12-01 13:56:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/fig/fig-histo.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= History of changes of an object hierarchy.                         =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_FIG_FIG_HISTO_H
#define FILE_FIG_FIG_HISTO_H
/*======================================================================*/

#include "fig-types.h"

/*======================================================================*/
/*= Item of an History                                                 =*/


typedef void (*Tfig_hfunc)(Cfig_hele* he, int undo);
typedef void (*Tfig_hfree)(Tfig_hele* he);

typedef struct _Tfig_hele0 {
    Tfig_hfunc unredo;
    Tfig_hfree free;
    Tfigure*   figure;
} Tfig_hele0 ;

struct _Tfig_hele {
    Tfig_hele0 base;
#define he_unredo base.unredo
#define he_free   base.free
#define he_figure base.figure
};

extern void* fig_histo_ele_new(int size, Tfigure* figure,
                               Tfig_hfunc unredo, Tfig_hfree free);
extern void  fig_histo_ele_free(Tfig_hele* e);

/*======================================================================*/
/*= Base operations                                                    =*/

extern Tfig_histo* fig_histo_new(Tfigure*f, int nb);
extern void        fig_histo_delete(Tfig_histo* h);
extern void        fig_histo_push(Tfig_histo* h, Tfig_hele* he);
extern void        fig_histo_pop(Tfig_histo* h);
extern int         fig_histo_modifNb(Tfig_histo* h);

/*======================================================================*/
/*= Moving in history                                                  =*/

extern int         fig_histo_undoNb(Tfig_histo* h);
extern int         fig_histo_redoNb(Tfig_histo* h);
extern void        fig_histo_undo  (Tfig_histo* h);
extern void        fig_histo_redo  (Tfig_histo* h);
extern void        fig_histo_commit(Tfig_histo* h);

/*======================================================================*/
/*= Adding figure modifications                                        =*/

extern void fig_histo_pushAddObj  (Tfig_histo* h, Tobject obj);
extern void fig_histo_pushDelObj  (Tfig_histo* h, Tobject obj);
extern void fig_histo_pushDelObjsS(Tfig_histo* h, CobjAllSet*set,
                                                  TobjDir dir);
extern void fig_histo_pushDelObjsT(Tfig_histo* h, TobjAll*objs,
                                   int objnb,     TobjDir dir);
extern void fig_histo_pushCpdDo   (Tfig_histo* h, TobjDir dir);
extern void fig_histo_pushCpdFlat (Tfig_histo* h, TobjDir dir);
extern void fig_histo_pushChgObj  (Tfig_histo* h, Tobject old,
                                                  Tobject latest);
extern void fig_histo_pushMovObj  (Tfig_histo* h, Tobject obj, Tdble2 tv);
extern void fig_histo_pushZoomObj (Tfig_histo* h, Tobject obj,
                                   double zf);

/*======================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*======================================================================*/

/*======================================================================*/
#ifdef __cplusplus
}
#endif
/*======================================================================*/
#endif // FILE_FIG_FIG_HISTO_H
/*======================================================================*/
