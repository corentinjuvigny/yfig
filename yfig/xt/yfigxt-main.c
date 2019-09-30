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
 *$Software: YFIG  (YFIG driver to Xt and Xawd or Xaw3d)
 *      $Id: yfigxt-main.c 148 2018-06-04 06:59:33Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/xt/yfigxt-main.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= main: it just calls YFIG_main.                                     =*/
/*======================================================================*/

#include "yfig-config.h"

/*======================================================================*/

extern int YFIG_main(int argc, char**argv);

/*======================================================================*/

int main(int argc, char**argv)
{
    return YFIG_main(argc,argv);
}

/*======================================================================*/
