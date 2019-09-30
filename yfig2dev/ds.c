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
 *$Software: YFIG2DEV
 *      $Id: ds.c 337 2018-11-05 22:44:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig2dev/ds.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Data structure of YFIG2DEV.                                        =*/
/*======================================================================*/

#include "yfig2dev-config.h"

#include "tools/globalvars.h"
#include "tools/strings.h"
#include "tools/imgfmt.h"
#include "tools/lenunit.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"

/*======================================================================*/
/*= Initialization & freeing                                           =*/

Inline Tds ds_init()
{
    Tds ds; bzero(&ds,sizeof(ds));
    ds.wMsgdrv = NULL; //&gl_msgdrv_silence;
    return ds;
}

// stdin  & stdout
extern Tds ds_init0()
{
    Tds ds = ds_init();
    ds.inname    = tool_strdup("stdin");
    ds.instream  = stdin;
    ds.outformat = tool_strdup(userParam_getOutFormat(1));
    ds.outname   = tool_strdup("stdout");
    ds.outstream = stdout;
    return ds;
}

// fig file & stdout
extern Tds ds_init1(CstrBrief in)
{
    Tds ds = ds_init();
    ds.inname    =  tool_strdup(in);
  //ds.instream  =  0;
    ds.outformat =  tool_strdup(userParam_getOutFormat(1));
    ds.outname   =  tool_chgPathExt(ds.inname,ds.outformat);
  //ds.outstream =  0;
    return ds;
}

// fig & out files
extern Tds ds_init2(CstrBrief in, CstrBrief out)
{
    char tmp[100];
    Tds ds = ds_init();
    ds.inname    =  tool_strdup(in);
  //ds.instream  =  0;
    ds.outformat =  tool_strdup(userParam_getOutFormat(0));
    ds.outname   =  tool_strdup(out);
  //ds.outstream =  0;
    if ( ds.outformat==0 ) {
        char* extname = tool_basenameExt( ds.outname );
        if ( extname==0 ) tool_ftlerr_printf(NULL,"can not detect the output format. Provide it with the %s option.",cmdline_switch2str(tmp,&userParams.outformat,'"'));
        CimgFmtDt* imgfmt = tool_imgfmt_getByExt(extname);
        if ( imgfmt==0 )  tool_ftlerr_printf(NULL,"%s image format is not recognized.",extname);
        ds.outformat = extname;
    }
    return ds;
}

extern Tds ds_free (Tds* ds)
{
    tool_free       ( ds->inname    );
    tool_free       ( ds->outname   );
    tool_free       ( ds->outformat );
    fig_scene_delete( ds->scene     );
    tool_free       ( ds->pdfname   );
}

/*======================================================================*/
/*= Update data structure                                              =*/

extern void ds_updateOption(Tds*ds)
{
    // opt_imgfmt
    ds->opt_imgfmt = tool_imgfmt_getByExt(ds->outformat);
    TEMP_asf_printf( ds->opt_imgfmt==0,"%s","here ds->opt_imgfmt must not be null.");

    // opt_srcBBKind
    if ( cmdline_isSwSet(&userParams.depths) ) {
        ds->opt_srcBBKind = SrcBB_UsedDepth;
    } else {
        ds->opt_srcBBKind = SrcBB_AllDepth;
    }

    // opt_paperKind;
    if ( ds->opt_imgfmt->raster ) {
        ds->opt_paperKind = PaKd_SrcBB;
    } else {
        // assuming it's PDF
        if ( userParam_getPaperDef()==0 ) {
            if ( ds_getZoomFactor(ds)==1.0 )
                ds->opt_paperKind = PaKd_SrcBB;
            else
                ds->opt_paperKind = PaKd_SrcBBZ;
        } else {
            ds->opt_paperKind = PaKd_SetByUser;
        }
    }

    // opt_paperKind;
    if ( ds->opt_paperKind == PaKd_SetByUser ) {
        int nrow,ncol;
        if ( userParam_getRowColZTF(&nrow,&ncol) ) {
            ds->opt_placeKind = PlaceKd_ZoomToFit;
        } else {
            if ( cmdline_isSwSet(&userParams.multipage) )
                ds->opt_placeKind = PlaceKd_ZoomUserAPM;
            else
                ds->opt_placeKind = PlaceKd_ZoomUser;
        }
    }
}

/*======================================================================*/
/*= Smart getters                                                      =*/

extern double  ds_getZoomFactor(Tds*ds)
{
    TboxDble box = ds_getSrcBBCm(ds);
    double dx=Box_dx( box ), dy=Box_dx( box );
    if ( ds->scene->sc_dessin->settings.userUnit.uu_id==LUNIT_ID_Inch ) {
        dx = LUNIT_CmToIn( dx );
        dy = LUNIT_CmToIn( dy );
    }
    double zoom = userParam_getZoomFactor(dx,dy);
    return zoom;
}

extern TboxDble  ds_getSrcBBCm(Tds*ds)
{
    Tfig_scene*    sc   = ds->scene;
    TboxDble       box;
    int            uu   = LUNIT_ID_Cm;

    if ( ds->opt_srcBBKind==SrcBB_UsedDepth ) {
        if ( cmdline_isSwSet(&userParams.adjustBB) )
            box = fig_scene_BBoxTopSelU(sc,uu);
        else
            box = fig_scene_BBoxTopAllU(sc,uu);
    } else {
        box = fig_scene_BBoxTopAllU(sc,uu);
        if ( ds->opt_srcBBKind==SrcBB_AllDepth00 ) {
            if ( Box_x(box) > 0 ) { Box_dx(box) += Box_x(box); Box_x(box)=0; }
            if ( Box_y(box) > 0 ) { Box_dy(box) += Box_y(box); Box_y(box)=0; }
        }
    }
    return box;
}

extern int       ds_isLandscape  (Tds*ds)
{
    if ( cmdline_isSwSet(&userParams.pageOrientLandscape) )
        return 1;
    if ( cmdline_isSwSet(&userParams.pageOrientPortrait) )
        return 0;
    return ds->scene->sc_dessin->settings.landscape!=0 ;
}

extern TdirWR    ds_getGravity   (Tds*ds)
{
    Tstr* pstr = cmdline_getStrM(&userParams.gravity,
                                 &userParams.gravityCC,
                                 &userParams.gravityEE,
                                 NULL);
    TEMP_asf_printf(pstr==0,"%s","unexpected null gravity string");
    if      ( strcasecmp(*pstr,"NN")==0 ) return DirWR_NN;
    else if ( strcasecmp(*pstr,"NE")==0 ) return DirWR_NE;
    else if ( strcasecmp(*pstr,"EE")==0 ) return DirWR_EE;
    else if ( strcasecmp(*pstr,"SE")==0 ) return DirWR_SE;
    else if ( strcasecmp(*pstr,"SS")==0 ) return DirWR_SS;
    else if ( strcasecmp(*pstr,"SW")==0 ) return DirWR_SW;
    else if ( strcasecmp(*pstr,"WW")==0 ) return DirWR_WW;
    else if ( strcasecmp(*pstr,"NW")==0 ) return DirWR_NW;
    else if ( strcasecmp(*pstr,"CC")==0 ) return DirWR_CC;
    else
        tool_ftlerr_printf(NULL,"\"%s\" is an invalid wind rose direction",*pstr);
    return 0;
}

/*======================================================================*/
