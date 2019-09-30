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
 *$Software: YFIG-QT (VGUI driver to Qt)
 *      $Id: guiqt-top.cc 48 2017-03-17 11:24:49Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-top.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Basic utilities.                                                   =*/
/*=                                                                    =*/
/*= See the guiqt/guiqt-top.h header for the type description, the     =*/
/*= function description and the documentation.                        =*/
/*======================================================================*/

#include "guiqt.h"

#include "vgui/vgui-accelkey.h"

/*======================================================================*/

extern void conv_accelDef2str(char* buf, TaccelDef accelDef)
{
    int len; char* plus;

    buf[0]=0;
    if ( accelDef==0)
        return;
    plus="";
    if ( accelDef&GL_ACCEL_Meta ) {
        strcat(buf,plus);
        strcat(buf,"Meta");
        plus = "+";
    }
    if (   accelDef&GL_ACCEL_Control ) {
        strcat(buf,plus);
        strcat(buf,"Ctrl");
        plus = "+";
    }
    if (   accelDef&GL_ACCEL_Shift ) {
        strcat(buf,plus);
        strcat(buf,"Shift");
        plus = "+";
    }
    strcat(buf,plus);
    len = strlen(buf);
    buf[len]   = tolower(  accelDef&0xff );
    buf[len+1] = 0;
}

/*======================================================================*/
