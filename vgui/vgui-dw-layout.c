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
 *      $Id: vgui-dw-layout.c 332 2018-10-30 21:36:27Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/vgui-dw-layout.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets.                       =*/
/*=                                                                    =*/
/*= Implementation of layout functions.                                =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "vgui-config.h"

#include "vgui/vgui-dw.h"
#include "vgui/wid-buttonPanel.h"
#include "vgui/vdw-title.h"

#define Verbose 0

/*======================================================================*/

#if  Verbose
static void Dbg_dw_MTX(CstrBrief fn, Tvgui_dwMTX mtx, int nbl, int nbc)
{
    int i,j;
    printf("%s:nbl=%d nbc=%d\n",fn,nbl,nbc);
    for ( i=0 ; i<nbl ; i++) {
        printf("%2d/%2d: ",i,nbl);
        for ( j=0 ; j<nbc ; j++) {
            printf("%12p:%d:%d",mtx[i][j].widEGS,
                                mtx[i][j].xs,mtx[i][j].ys);
        }
        printf("\n");
    }
}
#   define DBG_dw_MTX(mtx,ln,cn) Dbg_dw_MTX(__func__,mtx,ln,cn)
#else
#   define DBG_dw_MTX(mtx,ln,cn) while(0)
#endif

/*======================================================================*/
/*= Layout constructors                                                =*/

extern void vgui_dw_finishIC(Tvgui_dw* vdw, ...)
{
    typedef Tegs_widget Twset[100];
    Tvgui_dwMTX mtx;
    Twset       wsets2   [VGUI_DW_MTX_SZ];
    int         wnbs1    [VGUI_DW_MTX_SZ];
    int         wnbs2    [VGUI_DW_MTX_SZ];
    int         samesizes[VGUI_DW_MTX_SZ]; 
    int nbl=0,nbc=0;  // line an column numbers of mtx
    memset(mtx      ,0, sizeof(mtx));
    memset(wsets2   ,0, sizeof(wsets2));
    memset(wnbs1    ,0, sizeof(wnbs1));
    memset(wnbs2    ,0, sizeof(wnbs2));
    memset(samesizes,0, sizeof(samesizes));
    int l,c; Tegs_widget w;

    // fill the mtx[i,j]/wsets2[j]/samesizes[j] set with the widgets
    // and flags of the variable argument list.
    va_list ap;
    va_start(ap,vdw);
    for ( c=va_arg(ap,int) ; c!=-1 ; c=va_arg(ap,int) ) {
        w = va_arg(ap,Tegs_widget);
        if (w==0) continue;
        int ci = c&0xFF;
        if ( (c&VDWLF_SEP1)!=0 )
            mtx[wnbs1[ci]][ci].sepHV += VDW_SEP1_HV;
        if ( (c&VDWLF_SEP2)!=0 )
            mtx[wnbs1[ci]][ci].sepHV += VDW_SEP2_HV;
        if ( (c&VDWLF_EB)!=0 ) {
            mtx[wnbs1[ci]][ci].grow   = 1;
            if ( (c&VDWLF_EB)==VDWLF_EB )
                mtx[wnbs1[ci]][ci].flag  |= VDM_V2E_BGrow;
            else if ( (c&VDWLF_EH)!=0 )
                mtx[wnbs1[ci]][ci].flag  |= VDM_V2E_HGrow;
            else
                mtx[wnbs1[ci]][ci].flag  |= VDM_V2E_VGrow;
        }
        if ( (c&VDWLF_CB)!=0 )
            mtx[wnbs1[ci]][ci].flag  |= VDM_V2E_BCenter;
        mtx[wnbs1[ci]][ci].widEGS = w;
        mtx[wnbs1[ci]][ci].xs = 1;
        mtx[wnbs1[ci]][ci].ys = 1;
        wsets2[ci][wnbs2[ci]] = w;
        wnbs1[ci] += 1;
        wnbs2[ci] += 1;
        if ( ci>=nbc ) nbc = ci+1;
        continue;
    }
    va_end(ap);

    for (c=0;c<VGUI_DW_MTX_SZ;c+=1) if (wnbs1[c]>nbl) nbl=wnbs1[c];

#if Verbose
{
    int i,j;
    printf("%s:ln=%d cn=%d\n",__func__,nbl,nbc);
    for ( i=0 ; i<nbl ; i++) {
        printf("%2d: ",i);
        for ( j=0 ; j<nbc ; j++) {
            printf("%12p:%2x ",mtx[i][j].widEGS,mtx[i][j].flag);
        }
        printf("\n");
    }
}
#endif

    // do the layout
    if ( vdw->D_title )
        vdw_title_finish( vdw->D_title );
    if ( VGUI_D_isTopDial(vdw) && vdw->D_top->dt_ctlButtons ) {
        Twid_buttonPanel* panel = vdw->D_top->dt_ctlButtons;
        wid_buttonPanel_build(panel, NULL);
        wid_buttonPanel_finish(panel);
    }
    
    for (c=0;c<nbc;c++) {
        if ( samesizes[c] && wnbs2[c]!=0 ) {
            gui_dw_finishSdx(vdw,wsets2[c],wnbs2[c]);
        }
    }

    Twid_buttonPanel* btns = VGUI_D_isTopDial(vdw)
                           ? vdw->D_top->dt_ctlButtons : NULL;
    if ( vdw->D_hspace==-1 )   vdw->D_hspace =VDW_SEP_HV;
    if ( vdw->D_vspace==-1 )   vdw->D_vspace =VDW_SEP_HV;
    if ( vdw->D_hmargin==-1 )  vdw->D_hmargin=VDW_SEP_HV;
    if ( vdw->D_vmargin==-1 )  vdw->D_vmargin=VDW_SEP_HV;
    gui_dw_finishMic(vdw,
         vdw->D_title    ? vdw->D_title->D_widEGS : NULL,
         btns            ? btns->bp_widEGSext     : NULL,
         mtx,nbl,nbc);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void vgui_dw_finishIL (Tvgui_dw*vdw, ...)
{
    typedef Tegs_widget Twset[100];
    Tvgui_dwMTX mtx;
    Twset       wsets2   [VGUI_DW_MTX_SZ];
    int         wnbs1    [VGUI_DW_MTX_SZ];
    int         wnbs2    [VGUI_DW_MTX_SZ];
    int         samesizes[VGUI_DW_MTX_SZ]; 
    int nbl=0,nbc=0;  // line an column numbers of mtx
    memset(mtx      ,0, sizeof(mtx));
    memset(wsets2   ,0, sizeof(wsets2));
    memset(wnbs1    ,0, sizeof(wnbs1));
    memset(wnbs2    ,0, sizeof(wnbs2));
    memset(samesizes,0, sizeof(samesizes));
    int l; Tegs_widget w;

    // fill the mtx[i,j]/wsets2[j]/samesizes[j] set with the widgets
    // and flags of the variable argument list.
    va_list ap;
    va_start(ap,vdw);
    for ( l=va_arg(ap,int) ; l!=-1 ; l=va_arg(ap,int) ) {
        w = va_arg(ap,Tegs_widget);
        if (w==0) continue;
        int li = l&0xFF;
        if ( (l&VDWLF_SEP1)!=0 )
            mtx[li][wnbs1[li]].sepHV += VDW_SEP1_HV;
        if ( (l&VDWLF_SEP2)!=0 )
            mtx[li][wnbs1[li]].sepHV += VDW_SEP2_HV;
        if ( (l&VDWLF_EB)!=0 ) {
            mtx[li][wnbs1[li]].grow   = 1;
            if ( (l&VDWLF_EB)==VDWLF_EB )
                mtx[li][wnbs1[li]].flag  |= VDM_V2E_BGrow;
            else if ( (l&VDWLF_EH)!=0 )
                mtx[li][wnbs1[li]].flag  |= VDM_V2E_HGrow;
            else
                mtx[li][wnbs1[li]].flag  |= VDM_V2E_VGrow;
        }
        mtx[li][wnbs1[li]].widEGS = w;
        mtx[li][wnbs1[li]].xs = 1;
        mtx[li][wnbs1[li]].ys = 1;
        wsets2[li][wnbs2[li]] = w;
        wnbs1[li] += 1;
        wnbs2[li] += 1;
        if ( li>=nbl ) nbl = li+1;
        continue;
    }
    va_end(ap);

    for (l=0;l<VGUI_DW_MTX_SZ;l+=1) if (wnbs1[l]>nbc) nbc=wnbs1[l];

#if Verbose
{
    int i,j;
    printf("%s:ln=%d cn=%d\n",__func__,nbl,nbc);
    for ( i=0 ; i<nbl ; i++) {
        printf("%2d: ",i);
        for ( j=0 ; j<nbc ; j++) {
            printf("%12p+%08x",mtx[i][j].widEGS,mtx[i][j].flag);
        }
        printf("\n");
    }
}
#endif

    // do the layout
    if ( vdw->D_title )
        vdw_title_finish( vdw->D_title );
    if ( VGUI_D_isTopDial(vdw) && vdw->D_top->dt_ctlButtons ) {
        Twid_buttonPanel* panel = vdw->D_top->dt_ctlButtons;
        wid_buttonPanel_build(panel, NULL);
        wid_buttonPanel_finish(panel);
    }
    
#if 0
    for (c=0;c<nbc;c++) {
        if ( samesizes[c] && wnbs2[c]!=0 ) {
            gui_dw_finishSdx(vdw,wsets2[c],wnbs2[c]);
        }
    }
#endif

    Twid_buttonPanel* btns = VGUI_D_isTopDial(vdw)
                           ? vdw->D_top->dt_ctlButtons : NULL;
    gui_dw_finishMil(vdw,
         vdw->D_title    ? vdw->D_title->D_widEGS : NULL,
         btns            ? btns->bp_widEGSext     : NULL,
         mtx,nbl,nbc);

}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

extern void vgui_dw_finishMTX(Tvgui_dw*vdw, ...)
{
    int l,c,i,j,i2,j2;
    Tvgui_dwMTX mtx;
    int ln=0,cn=0,maxcn=0,maxln=0;
    memset(mtx,0,sizeof(mtx));

    // fill mtx with the widgets of the variable  argument list.
    va_list ap;
    va_start(ap,vdw);
    int f; Tegs_widget w;
    for ( f=va_arg(ap,int) ; f!=-1 ; f=va_arg(ap,int) ) {
        if ( VDWLF_IsRow(f) ) {
            ln = VDWLF_RowNum(f);
            if ( (ln+1)>maxln ) maxln = ln+1;
            continue;
        }
        // f is the column number of the following widget
        w = va_arg(ap,Tegs_widget);
        if ( w!=0 ) {
            cn = f&0xFF;
            mtx[ln][cn].widEGS = w;
            mtx[ln][cn].xs     = 1;
            mtx[ln][cn].ys     = 1;
            //mtx[ln][cn].grow   = (f&VDWLF_EB)!=0 ? 1 :0;
            mtx[ln][cn].flag  |= (f&VDWLF_EH)!=0 ? VDM_V2E_HGrow :0;
            mtx[ln][cn].flag  |= (f&VDWLF_EV)!=0 ? VDM_V2E_VGrow :0;
            if ( (cn+1)>maxcn) maxcn=cn+1;
        }
    }
    ln=maxln;
    cn=maxcn;

    // set the spans in mtx
    DBG_dw_MTX(mtx,ln,cn);
    for ( l=0 ; l<ln ; l++) {
        for ( c=0 ; c<cn ; c++) {
            Tvgui_dwMtxCell* curr = &mtx[l][c];
            if ( curr->xs==0 || curr->ys==0 ) continue;
            if ( curr->widEGS==0 ) continue;
            for ( i=l+1 ; i<ln && mtx[i][c].widEGS==curr->widEGS ; i++);
            for ( j=c+1 ; j<cn && mtx[l][j].widEGS==curr->widEGS ; j++);
            for ( i2=l ; i2<i ; i2+=1) {
                for ( j2=c ; j2<j ; j2+=1) {
                    mtx[i2][j2].xs = 0;
                    mtx[i2][j2].ys = 0;
                }
            }
            curr->xs = 1+j-(c+1);
            curr->ys = 1+i-(l+1);
        }
    }
    DBG_dw_MTX(mtx,ln,cn);

    // do the layout
    if ( vdw->D_title )
        vdw_title_finish( vdw->D_title );
    if ( VGUI_D_isTopDial(vdw) && vdw->D_top->dt_ctlButtons ) {
        Twid_buttonPanel* panel = vdw->D_top->dt_ctlButtons;
        wid_buttonPanel_build(panel, NULL);
        wid_buttonPanel_finish(panel);
    }

    Twid_buttonPanel* btns = VGUI_D_isTopDial(vdw)
                           ? vdw->D_top->dt_ctlButtons : NULL;
    gui_dw_finishMfa(vdw,
         vdw->D_title    ? vdw->D_title->D_widEGS : NULL,
         btns            ? btns->bp_widEGSext     : NULL,
         mtx,ln,cn);
}

/*======================================================================*/
