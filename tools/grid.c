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
 *      $Id: grid.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/grid.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Tgrid implementation.                                              =*/
/*=                                                                    =*/
/*= See the tools/grid.h header for documentation.                     =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/grid.h"
#include "tools/lenunit.h"

/*======================================================================*/

extern void   grid_init(Tgrid*self, int u, double f, double st)
{
    memset(self,0,sizeof(*self));
    self->unit.uu_id     = u;
    self->unit.uu_factor = f;
    self->unit.uu_sub    = st;
}

extern void   grid_free(Tgrid*self) { }

extern Tgrid* grid_new (int u, double f, double st)
{ 
    Tgrid* self = tool_malloc( sizeof(*self) );
    grid_init(self,u,f,st);
    return self;
}

extern void   grid_delete(Tgrid*self)
{
    grid_free(self);
    tool_free(self);
}

/*======================================================================*/

#define MIN_PIX 6

static void   grid_updateMajorMinor(Tgrid*self, double zf, double sdn,
                                    double *major, double*minor)
{
    double stepPix;
    if ( sdn<1 ) {
        *major = lu_UsrToFu(&self->unit,1.0/sdn);
        stepPix = *major*zf;
        while ( stepPix < MIN_PIX ) {
            *major *= 2.;
            stepPix = *major*zf;
        }
        *minor = 0;
        return;
    }

    *major = lu_UsrToFu(&self->unit,1.0);
    // matches major to zoom factor and resolution
    stepPix = *major*zf;
    if ( stepPix < MIN_PIX ) {
        while ( stepPix < MIN_PIX ) {
            *major *= 2.;
            stepPix = *major*zf;
        }
        *minor = 0;
    } else {
        double i;
        for ( i=sdn ; i>=2 ; i-=1 ) {
            *minor = lu_UsrToFu(&self->unit,1.0/i);
            stepPix = *minor*zf;
            if ( stepPix >= MIN_PIX )
                break;
        }
        if (i<2)
            *minor = 0;
    }
}

extern void   grid_updateGrid  (Tgrid*self, double zf, double sdn)
{
    return grid_updateMajorMinor(self,zf,(sdn!=0 ? sdn : self->unit.uu_sub),
            &self->gridMajorFU, &self->gridMinorFU); }

extern void   grid_updateRuler (Tgrid*self, double zf)
{
    return grid_updateMajorMinor(self,zf, self->unit.uu_sub,
            &self->rulerMajorFU, &self->rulerMinorFU); }

extern void   grid_updateMagnet(Tgrid*self, double sdn)
{
    double step;
    step = 1.0/(sdn!=0 ? sdn : self->unit.uu_sub);
    step = lu_UsrToFu(&self->unit,step);
    self->magnetFU = step;
}

/*======================================================================*/

extern void   grid_enableGrid    (Tgrid*self, int on)
{
    if ( on )
        self->flag |= GRID_FL_DoGrid;
    else
        self->flag &= ~GRID_FL_DoGrid;
}

extern void   grid_enableMagnet  (Tgrid*self, int on)
{
    if ( on )
        self->flag |= GRID_FL_Magnet;
    else
        self->flag &= ~GRID_FL_Magnet;
}

extern int    grid_isThereGridToDraw(Cgrid* self)
{
    if ( self==0 ) return 0;
    if ( (self->flag&GRID_FL_DoGrid)==0 ) return 0;
    if ( (self->gridMajorFU)==0 && (self->gridMinorFU)==0 ) return 0;
    return 1;
}

extern int grid_isMagnetEnabled(Cgrid* self)
{
    if ( self==0 ) return 0;
    if ( (self->flag&GRID_FL_Magnet)==0 ) return 0;
    if ( self->magnetFU==0 ) return 0;
    return 1;
}

/*======================================================================*/

static TdbleSet* grid_getTicksByStep(Cgrid*self, double s, double e,
                                    double step)
{
    double x,start;
    if (step==0)
        return NULL;
    TdbleSet* set = dbleset_new(200);
    start = lu_FuToUsr( &self->unit, s ); 
    start = lu_UsrToFu( &self->unit, floor(start) );
    while ( start>s ) start -= step;
    start += step;
    for ( x=start ; x<e ; x+=step) {
        dbleset_add(set,x);
    }

    return set;
}

static TdbleSet* grid_getTicksByStep0(Cgrid*self, double s, double e,
                                    double step)
{
    double x,ks;
    if (step==0)
        return NULL;
    TdbleSet* set = dbleset_new(200);
    ks    = floor( s/step );
    x = ks*step;
    while ( x<=e ) {
        if ( x>=s )
            dbleset_add(set,x);
        ks+=1;
        x = ks*step;
    }

    return set;
}

extern TdbleSet* grid_getGridTicks (Cgrid*self, double s, double e)
{
    double x,start,step;
    step = self->gridMinorFU;
    if (step==0)
        step =  self->gridMajorFU;
    return grid_getTicksByStep0(self,s,e,step);
}
extern TdbleSet* grid_getRulerTicks (Cgrid*self, double s, double e)
{
    double x,start,step;
    step = self->rulerMinorFU;
    if (step==0)
        step =  self->rulerMajorFU;
    return grid_getTicksByStep(self,s,e,step);
}
extern TdbleSet* grid_getGridTicksMajor(Cgrid*self, double s, double e)
{
    if ( s<=0 && 0<=e )
        return grid_getTicksByStep0(self,s,e,self->gridMajorFU);
    else
        return grid_getTicksByStep(self,s,e,self->gridMajorFU);
}
extern TdbleSet* grid_getRulerTicksMajor(Cgrid*self, double s, double e)
{
    if ( s<=0 && 0<=e )
        return grid_getTicksByStep0(self,s,e,self->rulerMajorFU);
    else
        return grid_getTicksByStep(self,s,e,self->rulerMajorFU);
}

static TdbleSet* grid_getTicksMinor(Cgrid*self, double s, double e,
                                    double majorFU, double minorFU)
{
    double x,start,step,stepM,xM;
    stepM = majorFU;  // Major step
    step  = minorFU;   // Minor step
    if (step==0)
        return NULL;
    TdbleSet* set = dbleset_new(200);
    start = lu_FuToUsr( &self->unit, s ); 
    start = lu_UsrToFu( &self->unit, floor(start) );
    xM    = start;
    while ( start>s ) start -= step;
    start += step;
    for ( x=start ; x<e ; x+=step) {
        if (x!=xM)
            dbleset_add(set,x);
        else
            xM += stepM;
    }

    return set;
}

extern TdbleSet* grid_getGridTicksMinor(Cgrid*self, double s, double e)
{ return grid_getTicksMinor(self,s,e,
                            self->gridMajorFU, self->gridMinorFU); }
extern TdbleSet* grid_getRulerTicksMinor(Cgrid*self, double s, double e)
{ return grid_getTicksMinor(self,s,e,
                            self->rulerMajorFU, self->rulerMinorFU); }

/*======================================================================*/
