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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: grid.h 102 2017-12-05 18:37:41Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/grid.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= This header defines parameters to define grids, graduated ruler    =*/
/*= and magnetic grids.                                                =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_GRID_H
#define FILE_TOOLS_GRID_H
/*======================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*======================================================================*/

#include "base-types.h"
#include "lenunit.h"

/*======================================================================*/
/*= Data structure                                                     =*/

typedef struct _Tgrid Tgrid;
typedef const   Tgrid Cgrid;

struct _Tgrid {
    char     flag;  // see GRID_FL_...
    TuserUnit unit;

    // the next fields are set by the grid_updateRuler function
    double rulerMajorFU; // the length of major ticks in FU unit.
    double rulerMinorFU; // the length of minor ticks in FU unit.

    // the next fields are set by the grid_updateGrid function
    double gridMajorFU; // the length of major ticks in FU unit.
    double gridMinorFU; // the length of minor ticks in FU unit.

    // the next field is set by the grid_updateMagnet
    double magnetFU;
};

#define GRID_FL_DoGrid 1  // grid must be drawn if possible
#define GRID_FL_Magnet 2  // magnetic grid is enabled

#define GRID_ToMagnet(g,x) (round((x)/(g)->magnetFU)*(g)->magnetFU)

/*======================================================================*/
/*= Interface                                                          =*/

/**
 * The grid_init function initializes the self grid using the \a u unit
 * base, the f unit factor and the sdn sub division number.
 *
 * The grid_new function allocates a Tgrid structure and then it initializes
 * it using: grid_init(self,u,f,st).
 * 
 * The grid_delete frees all the memory allocated by the self grid. The
 * self grid itself is not freed.
 *
 * The grid_delete frees all the memory allocated by the self grid including
 * the self grid itself.
**/
extern void   grid_init(Tgrid*self, int u, double f, double st);
extern void   grid_free(Tgrid*self);
extern Tgrid* grid_new (int u, double f, double st);
extern void   grid_delete(Tgrid*self);

/**
 * The grid_updateGrid function updates the self->gridMajorFU and
 * the self->gridMinorFU taking into account the self->unit and
 * the zf zoom factor and the sdn number of sub-divisions of the user
 * unit.
 * Let U the length of unit in FU.
 *   - self->gridMinorFU is set to the U/ndn or 0 if 2 consecutive
 *     sub ticks are too close to be drawable.
 *   - self->gridMajorFU is set to the U*(1<<k) where k is the smaller
 *     integer such that two  consecutive ticks are drawable.
 * If sdn is null, self->unit.sub is used.
 *
 * The grid_updateRuler function is similar to the grid_updateFunction
 * function but it updates the self->rulerMajorFU and self->rulerMinorFU
 * fields and it always uses self->unit->sub for the number
 * of sub-divisions
 *
 * The grid_updateMagnet function sets self->magnetFU to the length
 * in FU unit of a 1/sdn user unit.
 * If sdn is null, self->unit.sub is used.
**/
extern void   grid_updateGrid  (Tgrid*self, double zf, double sdn);
extern void   grid_updateRuler (Tgrid*self, double zf);
extern void   grid_updateMagnet(Tgrid*self, double sdf);

/**
 * The grid_enableGrid (respectively grid_enableMagnet) enables
 * or disables the grid drawing (resp: the magnetic feature) whether
 * the on parameter is 1 or 0.
 *
 * More precisely, these functions just affect the behavior of the
 * grid_isThereGridToDraw and grid_isMagnetEnable function.
**/

extern void   grid_enableGrid    (Tgrid*self, int on);
extern void   grid_enableMagnet  (Tgrid*self, int on);

/**
 * The grid_isThereGridToDraw function returns 1 if the self grid
 * must be drawn otherwise it returns 0.
 * More precisely, 1 is returned if:
 *   - a minor or major tick was defined (grid_updateGrid was called),
 *   - and grid drawing was enabled (using the grid_enableGrid function).
 * Furthermore if self is the NULL pointer, it returns 0.
 *
 * The grid_isMagnetEnabled function returns 1 if both
 * a magnetic step was defined using the grid_updateMagnet function
 * and the magnetic feature was turned on using the grid_enableMagnet
 * function.
 * Otherwise, it returns 0.
 * Furthermore if self is the NULL pointer, it returns 0.
**/
extern int grid_isThereGridToDraw(Cgrid* self);
extern int grid_isMagnetEnabled  (Cgrid* self);

/**
 * The grid_getGridTicks function returns the positions where grid lines
 * must be drawn between the s and e positions for the self grid.
 * The grid_getGridTicksMajor function returns the positions where major
 * grid lines (they are aligned to user unit) must be drawn between
 * the s and e positions for the self grid.
 * The grid_getGridTicksMinor function returns the positions where minor
 * grid lines (they are placed on the subdivisions of the user unit)
 * must be drawn between the s and e positions for the self grid.
 * Be T, MAJ and MIN the results of the former functions, then
 * we have: T is the union of MAJ and MIN, and the intersection of 
 * MAJ and MIN is the empty set.
 *
 * The grid_getRulerTicks/Major/Minor functions are similar to the
 * grid_getGridTicks/Major/Minor functions but they returns the ruler
 * position.
 *
 * Return
 *  These functions return a pointer to a double set if at least a tick
 *  position exists otherwise they return the NULL pointer.
 *
 *  The caller must free the returned point set after use.
 *
 * Parameters
 *  - self: the grid definition
 *  - s,e:  get the tick positions from the s start position
 *          to the e end position in FU unit ( s< pos <e).
 *
 * Insure
 *  Before calling these functions be sure that the self grid is 
 *  updated to the right zoom factor.
 *  If the grid was not updated, these functions return the NULL pointer. 
**/
extern TdbleSet* grid_getGridTicks      (Cgrid*self, double s, double e);
extern TdbleSet* grid_getGridTicksMajor (Cgrid*self, double s, double e);
extern TdbleSet* grid_getGridTicksMinor (Cgrid*self, double s, double e);
extern TdbleSet* grid_getRulerTicks     (Cgrid*self, double s, double e);
extern TdbleSet* grid_getRulerTicksMajor(Cgrid*self, double s, double e);
extern TdbleSet* grid_getRulerTicksMinor(Cgrid*self, double s, double e);

/*======================================================================*/
#ifdef __cplusplus
}
#endif
/*======================================================================*/
#endif // FILE_TOOLS_GRID_H
/*======================================================================*/
