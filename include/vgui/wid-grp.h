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
 *      $Id: wid-grp.h 365 2018-11-18 23:45:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-grp.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * Some widget groups (based on Twid_grp class).
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_GRP_H
#define FILE_WID_GRP_H
#include "tools/CPLUSPLUS.h"
/*======================================================================*/

#include "wid.h"

/*======================================================================*/

#define MTX_IOPT_VXYx(c,grp) (c),wid_grp_lioEGS(grp),(c)+1,wid_grp_xioEGS(grp),(c)+2,wid_grp_yioEGS(grp)
#define MTX_IOPT_VXYU(c,grp) MTX_IOPT_VXYx(c,grp),(c)+4,wid_grp_uioEGS(grp)
#define MTX_IOAD_VAU(c,grp)  (c),wid_grp_lioEGS(grp),(c)+1,wid_grp_xioEGS(grp),(c)+2,wid_grp_uioEGS(grp)

extern Twid_grp* wid_iopt_nbf   (Twid_box* box,    CstrBrief label,
                                 Twid_grpCB cbvld, Twid_grpCB cbchg, void* cbud,
                                 Tdble2 iv, double au, int uu, CstrBrief fmt);

extern Twid_grp* wid_ioa180_nbf(Twid_box* box, CstrBrief label,
                                Twid_grpCB cbvld, Twid_grpCB cbchg, void* cbud,
                                double iv);
extern Twid_grp* wid_ioa360_nbf(Twid_box* box, CstrBrief label,
                                Twid_grpCB cbvld, Twid_grpCB cbchg, void* cbud,
                                double iv);

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
#endif // FILE_WID_GRP_H
/*======================================================================*/
