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
 *      $Id: imgfmt.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/imgfmt.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Standard image formats.                                            =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/imgfmt.h"

/*======================================================================*/
/*= Table of image forrmats.                                           =*/

static const TimgFmtDt gl_imgfmts[] = {
  { IMGFMT_PDF, 0, "pdf", { "pdf", "PDF", NULL }, 
        "PDF", "Portable Document Format" },
  { IMGFMT_SVG, 0, "svg", { "svg", "SVG", NULL },
        "SVG", "Scalable Vector Graphics" },
  { IMGFMT_TEX, 0, "tex", { "tex", "TEX", "latex", "LATEX", NULL },
        "latex", "Lamport Tex" },
  { IMGFMT_PNG, 1, "png", { "png", "PNG", NULL },
        "PNG", "Portable Network Graphic" },
  { IMGFMT_TIF, 1, "tif", { "tif", "tiff", "TIF", "TIFF", NULL },
        "TIFF", "Tagged Image File Format." },
  { IMGFMT_JPG, 1, "jpg", { "jpg", "JPG", "jpeg", "JPEG", NULL },
        "JPEG", "Joint Photographic Experts Group" },
};

/*======================================================================*/
/*= Interface.                                                         =*/

extern CimgFmtDt* tool_imgfmt_getByKd(TimgFmtKd kd)
{
    TEMP_asf_printf( kd<0 || kd>=(IMGFMT_VecNb+IMGFMT_RasNb),
        "%d is an invalid kind of image format (expected range: [%d,%d[)",
        kd,0,IMGFMT_VecNb+IMGFMT_RasNb);

    int i;
    for (i=0 ; i<TOOL_TABLE_NbEle(gl_imgfmts) ; i+=1) {
        if ( gl_imgfmts[i].kind==kd )
            return gl_imgfmts+i;
    }
    return NULL;
}

static int tool_imgfmt_isExtOfDt(CimgFmtDt*dt, CstrBrief ext)
{
    int i;
    for (i=0; dt->exts[i]!=0 ; i+=1)
        if ( strcasecmp(dt->exts[i],ext)==0) return 1;
    return 0;
}

extern CimgFmtDt* tool_imgfmt_getByExt (CstrBrief ext)
{
    int i; CimgFmtDt* dt;
    for (i=0 ; i<TOOL_TABLE_NbEle(gl_imgfmts) ; i+=1) {
        dt = &gl_imgfmts[i];
        if ( tool_imgfmt_isExtOfKd(dt->kind,ext)!=0 )
            return dt;
    }
    return NULL;
}

extern int tool_imgfmt_isExtOfKd    (TimgFmtKd kd, CstrBrief ext)
{
    int i; CimgFmtDt* dt;
    if ( TOOL_strIsEmpty(ext) ) return 0;
    if ( (dt=tool_imgfmt_getByKd(kd))==0 ) return 0;
    return tool_imgfmt_isExtOfDt(dt,ext);
}

/*======================================================================*/
/*= Image converter.                                                   =*/

extern int tool_imgfmt_convIM(Tmsgdrv* md, TstrBrief sp, TimgFmtKd sk,
                                           TstrBrief dp, TimgFmtKd dk,
                                           double ppi, double q)
{
    CimgFmtDt* sdt = sk==-1 ? NULL : tool_imgfmt_getByKd( sk );
    CimgFmtDt* ddt = tool_imgfmt_getByKd( dk );
    TstrEaten  dfmtopt = 0; // option for destination fmt: -format fmt
    TstrEaten  ppiopt  = 0; // option for PPI:     -density <ppi>
    TstrEaten  qtyopt  = 0; // option for quality: -quality n   (n entier de 0 à 100)

    // set destination format
    dfmtopt = tool_aprintf("%s:",ddt->shortname);

    // check for ppi
    if ( (sdt==0 || sdt->raster==0) && ddt->raster!=0 ) {
        int maxppi = 144*20;
        int ippi   = round( ppi );
        if ( ippi<=0 )        ippi=144;
        else if ( ippi<8 )    ippi=8;
        else if (ippi>maxppi) ippi=maxppi;
        ppiopt = tool_aprintf("-density %d",ippi);
    }

    // check for quality
    if ( 1 ) {
        int iq = round( q );
        if ( iq<10 )     iq=10;
        else if (iq>100) iq=100;
        qtyopt = tool_aprintf("-quality %d",iq);
    }

    // build the cmd and run it
    TstrEaten  cmd = tool_aprintf("convert %s %s%s %s %s%s",
            ppiopt ? ppiopt : "",
            "",sp,
            qtyopt ? qtyopt : "",
            dfmtopt,dp
    );
TEMP_spw_printf("running command: %s",cmd);
    int status = system(cmd);
    if (status==-1) 
        status = -1;
    else if ( WIFEXITED(status) && WEXITSTATUS(status)==0 )
        status = 0;
    else
        status = 1;

    if ( status!=0 ) 
        tool_errerr_printf(md,"Image conversion failed: %s",cmd);

    tool_free( dfmtopt );
    tool_free( ppiopt  );
    tool_free( qtyopt  );
    tool_free( cmd     );
    return status;
}

/*======================================================================*/
