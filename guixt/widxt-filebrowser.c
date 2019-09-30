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
 *$Software: YFIG-XT (Interface to Xt and Xawd or Xaw3d)
 *      $Id: widxt-filebrowser.c 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-filebrowser.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= File browser widget (EGS driver).                                  =*/
/*=                                                                    =*/
/*= See the vgui/wid-filebrowser.h header for documentation.           =*/
/*======================================================================*/

#include "guixt.h"
#include "guixt-dw.h"

#include "vgui/wid-filebrowser.h"
#include "vgui/wid-button.h"
#include "vgui/wid-buttonPanel.h"

#include "tools/globalvars.h"
#include "tools/strings.h"
#include "tools/file.h"

#define VGUI_MWP_BorderWidth 1

/*======================================================================*/
/*= Data structure                                                     =*/
/*=                                                                    =*/
/*=   layout of topDW is                                               =*/
/*=        lin1                                                        =*/
/*=      col1 col2                                                     =*/
/*=       btnctl                                                       =*/
/*=                                                                    =*/
/*=   layout of col1                                                   =*/
/*=       btnHhrDw     ( home hide rescan )                            =*/
/*=       fileKindDw   ( label text-in )                               =*/
/*=       dirs--list                                                   =*/
/*=                                                                    =*/
/*=   layout of col2                                                   =*/
/*=       label text-in    ( base name )                               =*/
/*=       files---list                                                 =*/

typedef struct _Tgui_fileBrowser Tgui_fileBrowser;

struct _Tgui_fileBrowser {
    Tvgui_dw*   fb_topDW;   // top form of widget
    Tegs_widget fb_form;

    Tvgui_dw*    fb_lin1Dw; // sub dialog of top
    Tvgui_dw*    fb_col1Dw; // sub dialog of lin2
    Tvgui_dw*    fb_col2Dw; // sub dialog of lin2
#define          fb_lin1DwEGS(fb)  vgui_dw_getWidEGS(fb->fb_lin1Dw)
#define          fb_col1DwEGS(fb)  vgui_dw_getWidEGS(fb->fb_col1Dw)
#define          fb_col2DwEGS(fb)  vgui_dw_getWidEGS(fb->fb_col2Dw)

    Tvgui_dw*    fb_btnHhrDw; 
    Twid_button* fb_btnHome;
    Twid_button* fb_btnCwd;
    Twid_button* fb_btnRescan;
    Twid_button* fb_btnHidden;
    Tvgui_dw*    fb_fileKindDw; 
    Tegs_widget  fb_fileKindLAB;
    Twid_slin*   fb_fileKindTIN;
#define          fb_btnHhrDwEGS(fb)  vgui_dw_getWidEGS(fb->fb_btnHhrDw)
#define          fb_btnHomeEGS       fb_btnHome->but_widEGS
#define          fb_btnCwdEGS        fb_btnCwd->but_widEGS
#define          fb_btnRescanEGS     fb_btnRescan->but_widEGS
#define          fb_btnHiddenEGS     fb_btnHidden->but_widEGS
#define          fb_fileKindDwEGS(fb)  vgui_dw_getWidEGS(fb->fb_fileKindDw)

    Tegs_widget fb_currDirLAB;
    Twid_slin*  fb_currDirTIN;
    Tegs_widget fb_dirsVPORT;
    Tegs_widget fb_dirsLIST;
    TastrSet*   fb_dirs;

    Tegs_widget fb_currFileLAB;
    Twid_slin*  fb_currFileTIN;
    Tegs_widget fb_filesVPORT;
    Tegs_widget fb_filesLIST;
    TastrSet*   fb_files;
    
    Twid_buttonPanel*
                 fb_ctlbtn; // control buttons
#define          fb_ctlbtnEGS(fb) fb->fb_ctlbtn ? fb->fb_ctlbtn->bp_widEGSext : NULL
};

/*======================================================================*/

static TstrEaten xt_filebrowser_getDirPath(Twid_filebrowser* fbd);
static TstrEaten xt_filebrowser_getBasName(Twid_filebrowser* fbd);

/*======================================================================*/
/*= handle the state of OK button (making it enabled or disabled).     =*/

#define OPT_EnableDisable_CtlBtnOk 0

#if ! OPT_EnableDisable_CtlBtnOk 
//Inline void xt_ctlbtnok_update (Twid_filebrowser* fbd) { }
Inline void xt_ctlbtnok_enable (Twid_filebrowser* fbd) { }
Inline void xt_ctlbtnok_disable(Twid_filebrowser* fbd) { }
#else
#define fb_ctlbtnOk(fb)  wid_buttonPanel_getBtnByUid(fb->fb_ctlbtn,WID_DIALSTATUS_Ok)
//Inline void xt_ctlbtnok_update (Twid_filebrowser* fbd) { }
Inline void xt_ctlbtnok_enable (Twid_filebrowser* fbd)
{
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    if ( fbd->embedded ) {
        wid_button_enable(  fb_ctlbtnOk(fb) );
    }
}
Inline void xt_ctlbtnok_disable(Twid_filebrowser* fbd)
{
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    if ( fbd->embedded ) {
        wid_button_enable(  fb_ctlbtnOk(fb) );
    }
}
#endif

/*======================================================================*/
/*= handle the state of the filebrowser widget.                        =*/
/*=                                                                    =*/
/*= The supported events that change the state of the filebrowser      =*/
/*= widget are:                                                        =*/
/*=   - the scan directory changed (dir parameter)                     =*/
/*=   - the file mask changed (filemask parameter)                     =*/
/*=                                                                    =*/
/*= The rescan parameter allows to force to build again the lists of   =*/
/*= the files and directories.                                         =*/

#define ELES(s)  ((s)!=0 ? s->eles  : NULL)
#define ELENB(s) ((s)!=0 ? s->eleNb : 0)

static void fileBrowser_update(Twid_filebrowser* fbd,
        CstrBrief dir, CstrBrief filemask, int rescan, int noCB)
{
    Tmsgdrv*          md = vgui_dw_getMsgDrv( fbd->pdialog );
    Tgui_fileBrowser* fb = fbd->guiUserdata;

    // set up the dir name widget
    if ( ! TOOL_strIsEmpty(dir) ) {
      char* tmp = fbd->dir;
      if ( strcmp(dir,"..")==0 ) {
        fbd->dir = tool_dirname(fbd->dir); // malloced
      } else if ( dir[0]!='/' ) {
            fbd->dir = tool_aprintf("%s/%s",fbd->dir, dir);
      } else {
            fbd->dir = tool_strdup(dir);
      }
      free(tmp);
      astrset_delete(fb->fb_dirs);  fb->fb_dirs  = 0;
      astrset_delete(fb->fb_files); fb->fb_files = 0;
    }
    gui_slin_setText(fb->fb_currDirTIN, fbd->dir);

    // set up the mask file widget
    if ( filemask!=0 ) {
        astrset_delete(fb->fb_files); fb->fb_files = 0;
        gui_slin_setText(fb->fb_fileKindTIN, filemask);
    } else {
        XtVaGetValues(fb->fb_fileKindTIN->widEGS,XtNstring,&filemask,NULL);
    }
    if ( filemask==0 )
        gui_slin_setText(fb->fb_fileKindTIN, "");

    // if rescan is asked
    if ( rescan!=0 ) {
      astrset_delete(fb->fb_dirs);  fb->fb_dirs  = 0;
      astrset_delete(fb->fb_files); fb->fb_files = 0;
    }

    // update the directory list widget
    if ( fb->fb_dirs==0 ) {
        fb->fb_dirs = tool_fileEleOfDirD(md,fbd->dir,0,0);
        xt_listString_chgStrs(fb->fb_dirsLIST,ELES(fb->fb_dirs),ELENB(fb->fb_dirs));
        if ( fbd->existingPath!=0 ) {
            gui_slin_setText(fb->fb_currFileTIN,0);
            if ( ! noCB ) {
                fbd->guiFileIsSelectedCB(fbd, 0);
            }
        }
    }

    // update the file list widget
    if ( fb->fb_files==0 ) {
        int with_hidden = (fb->fb_btnHidden->but_onoff_state&WID_BUT_STATE_On)!=0;
        TastrSet* masks = astrset_newStrWords(filemask);
        if ( masks->eleNb==0 ) {
            fb->fb_files = tool_fileEleOfDirF(md,fbd->dir,NULL,with_hidden);
        } else {
            int i;
            fb->fb_files = astrset_new(10);
            for (i=0 ; i<masks->eleNb ; i+=1) {
                TastrSet* tmp = tool_fileEleOfDirF(md,fbd->dir,masks->eles[i],with_hidden);
                astrset_append(fb->fb_files,tmp);
                astrset_delete(tmp);
            }
        }
        astrset_delete(masks);
        xt_listString_chgStrs(fb->fb_filesLIST,
                fb->fb_files->eles,fb->fb_files->eleNb);
    }
}

/*======================================================================*/
/* home, hiden and rescan buttons callbacks                            =*/

static void btn_home_cb(Tvgui_dw* vdw, int uid, void*ud)
{
    Twid_filebrowser* fbd = ud;
    xt_ctlbtnok_disable(fbd);
    fileBrowser_update(fbd,gl_homeDir,NULL,0,0);
}

static void btn_cwd_cb(Tvgui_dw* vdw, int uid, void*ud)
{
    Twid_filebrowser* fbd = ud;
    xt_ctlbtnok_disable(fbd);
    fileBrowser_update(fbd,gl_startDir,NULL,0,0);
}

static void btn_hidden_cb(Tvgui_dw* vdw, int uid, void*ud)
{
    Twid_filebrowser* fbd = ud;
    fileBrowser_update(fbd,NULL,NULL,1,0);
}

static void btn_rescan_cb(Tvgui_dw* vdw, int uid, void*ud)
{
    Twid_filebrowser* fbd = ud;
    fileBrowser_update(fbd,NULL,NULL,1,0);
}

/*======================================================================*/
/* file mask, dir names and file name slin callbacks                   =*/ 

static void slin_fileMask_cb(Twid_slin*tin, void* ud)
{
    char* mask=0;
    Twid_filebrowser* fbd = ud;
    Tgui_fileBrowser* fb  = fbd->guiUserdata;
    XtVaGetValues(tin->widEGS,XtNstring,&mask,NULL);
    fileBrowser_update(fbd,NULL,mask,0,0);
}

static void slin_currDir_cb(Twid_slin*tin, void* ud)
{
    char* dir=0;
    Twid_filebrowser* fbd = ud;
    Tgui_fileBrowser* fb  = fbd->guiUserdata;
    XtVaGetValues(tin->widEGS,XtNstring,&dir,NULL);
    fileBrowser_update(fbd,dir,NULL,0,0);
}

static void slin_currFile_cb(Twid_slin*tin, void* ud)
{
    Twid_filebrowser* fbd = ud;
    TstrEaten dir  = xt_filebrowser_getDirPath(fbd); // malloced
    TstrEaten base = xt_filebrowser_getBasName(fbd); // malloced
    TstrEaten path = tool_aprintf("%s/%s",dir,base); // malloced
    fbd->guiFileIsSelectedCB(fbd,path);
    free(dir); free(base); free(path);
}

/*======================================================================*/
/* selection callbacks                                                 =*/

static void list_dirSelected_cb(Widget w,
        XtPointer client_data, XtPointer call_data)
{
    XT_DeclareArgs(10);
    Twid_filebrowser* fbd = client_data;
    XawListReturnStruct *ret_struct = (XawListReturnStruct *) call_data;

    fileBrowser_update(fbd,ret_struct->string,NULL,0,0);
}

static void list_fileSelected_cb(Widget w,
        XtPointer client_data, XtPointer call_data)
{
    Twid_filebrowser* fbd      = client_data;
    Tgui_fileBrowser* fb       = fbd->guiUserdata;
    XawListReturnStruct *ret_struct = call_data;

    xt_ctlbtnok_enable(fbd);
    gui_slin_setText(fb->fb_currFileTIN,ret_struct->string);
    TstrEaten dir  = xt_filebrowser_getDirPath(fbd); // malloced
    TstrEaten base = xt_filebrowser_getBasName(fbd); // malloced
    TstrEaten path = tool_aprintf("%s/%s",dir,base); // malloced
    fbd->guiFileIsSelectedCB(fbd,path);
    free(dir); free(base); free(path);
}

/*======================================================================*/
/*= Virtual entries                                                    =*/

static Tvgui_dw* xt_filebrowser_nbfSubDw(Tvgui_dw* parent)
{
    Tvgui_dw* vdw = create_dwform(parent,
        0,0,0,0, EGSCOLOR_default, EGSCOLOR_default);
    vdw->guiUserdata = tool_malloc_0( sizeof(TGUI_dialog) );
    return vdw;
}

static Tegs_widget xt_filebrowser_dbf_I(Tvgui_dw* pdialog, Twid_filebrowser* fbd)
{
    XT_DeclareArgs(20);

    Tvgui*     gui       = vgui_dw_getVGui(pdialog);
    //Tgui_main* main      = gui->g_main;
    Widget     parent    = vgui_dw_getWidEGS(pdialog);
    Tgui_fileBrowser* fb = tool_malloc_0( sizeof(*fb) );
    fbd->guiUserdata     = fb;
    int dx               = fbd->dx<350 ? 350 : fbd->dx;
    int dy               = fbd->dy<250 ? 250 : fbd->dx;
    int dxfiles          = 100; // (dx*100)/60;   //  0
    int dyfiles          =   0; // dy-50;         // 50
    int dxdirs           =  70; // dx-dxfiles-20; // 80
    int dydirs           =  70; // 200;           // 80
    int hvsep            =   5;

    //dxfiles /= 10; dyfiles /= 10; dxdirs /=10 ; dydirs /= 10;

    /* first a form placed into parent widget and its 3 sub-widget*/
    fb->fb_topDW = create_dwform(pdialog,
        2,2,0,0, EGSCOLOR_default, EGSCOLOR_default);
    fb->fb_topDW->guiUserdata = tool_malloc_0( sizeof(TGUI_dialog) );
    fb->fb_form = vgui_dw_getWidEGS(fb->fb_topDW);

    fb->fb_lin1Dw = xt_filebrowser_nbfSubDw(fb->fb_topDW);
    fb->fb_col1Dw = xt_filebrowser_nbfSubDw(fb->fb_topDW);
    fb->fb_col2Dw = xt_filebrowser_nbfSubDw(fb->fb_topDW);

  /**
   * line 1: Current directory
  **/
    fb->fb_currDirLAB = create_label(gui, fb_lin1DwEGS(fb), wn_dialLabel,
            0,0,0,EGSCOLOR_default,EGSCOLOR_default,"Current Dir.");
    fb->fb_currDirTIN = create_slin(
            gui, fb_lin1DwEGS(fb), wn_dialSlin,
            0,0,0, EGSCOLOR_default,EGSCOLOR_default,
            NULL, 0, slin_currDir_cb,fbd);
    vgui_dw_finishIL(fb->fb_lin1Dw,
        0,fb->fb_currDirLAB, 0|VDWLF_EH,fb->fb_currDirTIN->widEGS,-1);

  /**
   * col1: Current directory
  **/
    // line 1 of col1
    char* btnsize1 = "Working";
    char* btnsize2 = "Working dir.";
    fb->fb_btnHhrDw = xt_filebrowser_nbfSubDw(fb->fb_col1Dw);
    fb->fb_btnHome = widD_button_std_nbfLab(fb->fb_btnHhrDw,
         NULL,                     // helpText
         btn_home_cb, fbd, NULL,   // cb & ud & ud-free
        "Home", btnsize1);         // label & width
    fb->fb_btnCwd = widD_button_std_nbfLab(fb->fb_btnHhrDw,
         NULL,                     // helpText
         btn_cwd_cb, fbd, NULL,    // cb & ud & ud-free
        "Working dir.", btnsize2); // label & width
    fb->fb_btnRescan = widD_button_std_nbfLab(fb->fb_btnHhrDw,
         NULL,                     // helpText
         btn_rescan_cb, fbd, NULL, // cb & ud & ud-free
        "Rescan", btnsize1);       // label & width
    fb->fb_btnHidden = widD_button_check_nbfLab(fb->fb_btnHhrDw,
        NULL,                           // helpText
        btn_hidden_cb, fbd, NULL,       // cb & ud & ud-free
       "Show Hidden file", NULL);       // label & size
    vgui_dw_finishMTX(fb->fb_btnHhrDw
      ,VDWLF_ROW(0), 0,         fb->fb_btnHomeEGS   ,1,fb->fb_btnCwdEGS     ,2,fb->fb_btnRescanEGS
      ,VDWLF_ROW(1), 0|VDWLF_CB,fb->fb_btnHiddenEGS ,1,fb->fb_btnHiddenEGS ,2,fb->fb_btnHiddenEGS
      ,-1);

    // line 2 of col1
    fb->fb_fileKindDw = xt_filebrowser_nbfSubDw(fb->fb_col1Dw);
    fb->fb_fileKindLAB = create_label(gui, fb_fileKindDwEGS(fb), wn_dialLabel,
            0,0,0,EGSCOLOR_default,EGSCOLOR_default,"File Kind");
    fb->fb_fileKindTIN = create_slin(
            gui, fb_fileKindDwEGS(fb), wn_dialSlin,
            0,0,0, EGSCOLOR_default,EGSCOLOR_default,
            fbd->filemask, 0, slin_fileMask_cb,fbd);
    vgui_dw_finishIL(fb->fb_fileKindDw,
        0,fb->fb_fileKindLAB,0|VDWLF_EH,fb->fb_fileKindTIN->widEGS,-1);
    // line 3 of col1
    fb->fb_dirsLIST = create_listString(
        &fb->fb_dirsVPORT,gui,fb_col1DwEGS(fb),wn_dialListDir,
        hvsep, VGUI_MWP_BorderWidth,dxdirs,dydirs,EGSCOLOR_default,EGSCOLOR_default,
        list_dirSelected_cb, fbd);

  /**
   * col2: base names
  **/
    fb->fb_currFileLAB = create_label(gui, fb_col2DwEGS(fb), wn_dialLabel,
            0,0,0,EGSCOLOR_default,EGSCOLOR_default,"Current File");
    fb->fb_currFileTIN = create_slin(
            gui, fb_col2DwEGS(fb), wn_dialSlin,
            0,0,0, EGSCOLOR_default,EGSCOLOR_default,
            NULL, 0, slin_currFile_cb,fbd);
    fb->fb_filesLIST = create_listString(
        &fb->fb_filesVPORT,gui,fb_col2DwEGS(fb),wn_dialListFile,
        hvsep, VGUI_MWP_BorderWidth,dxfiles,dyfiles, EGSCOLOR_default,EGSCOLOR_default,
        list_fileSelected_cb, fbd);

  /**
   * update file list, but disabling the callback, cause it can
   * be use only when the dialog is fully realized.
  **/
    fileBrowser_update(fbd,NULL,NULL,1,1);

  /**
   * add control button if needed.
  **/
    if ( fbd->embedded ) {
#     define ADD_CTLBTN(help,label,cb,ud) do { \
        Twid_button* btn = widD_button_std_newLab(fb->fb_topDW, \
            help, cb, NULL, NULL, label,NULL); \
        wid_buttonPanel_addButton(fb->fb_ctlbtn, btn); \
        btn->but_uid = ud; top->dt_buttsMaskOrg |= ud; \
      } while (0)
        extern void vgui_dw_ctlbutton_cb_sub(Tvgui_dw*vdw, int, void*s);
        Tvgui_dwTop* top = vgui_dw_getTopData( fb->fb_topDW );
        TEMP_asf_printf(top==0, "%s", "no top dialog data for  fb->fb_topDW");
        fb->fb_ctlbtn = widD_buttonPanel_newR1Cn(fb->fb_topDW,60,25);
        fb->fb_ctlbtn->bp_buttonHVsep = 12;
        fb->fb_ctlbtn->bp_adjustR1C1  = 1;
        ADD_CTLBTN(NULL, fbd->cancelText, vgui_dw_ctlbutton_cb_sub, WID_DIALSTATUS_CancelX);
        if (  fbd->extraText!=0 )
        ADD_CTLBTN(NULL, fbd->extraText,  vgui_dw_ctlbutton_cb_sub, fbd->extraStatus);
        ADD_CTLBTN(NULL, fbd->okText,     vgui_dw_ctlbutton_cb_sub, WID_DIALSTATUS_Ok);
        wid_buttonPanel_dbf(fb->fb_ctlbtn,NULL);
        xt_ctlbtnok_disable(fbd);
    }

  /* final layout */
    vgui_dw_finishIC(fb->fb_col1Dw,
        0,fb_btnHhrDwEGS(fb), 0|VDWLF_EH,fb_fileKindDwEGS(fb), 0|VDWLF_EB,fb->fb_dirsVPORT,-1);
    vgui_dw_finishMTX(fb->fb_col2Dw
         ,VDWLF_ROW(0)   ,0         ,fb->fb_currFileLAB    ,1|VDWLF_EH,fb->fb_currFileTIN->widEGS
         ,VDWLF_ROW(1)   ,0|VDWLF_EB,fb->fb_filesVPORT     ,1|VDWLF_EB,fb->fb_filesVPORT
         ,-1);
    vgui_dw_finishMTX(fb->fb_topDW
        ,VDWLF_ROW(0)   ,0|VDWLF_EH,fb_lin1DwEGS(fb)  ,1|0       ,fb_lin1DwEGS(fb)
        ,VDWLF_ROW(1)   ,0|VDWLF_EV,fb_col1DwEGS(fb)  ,1|VDWLF_EB,fb_col2DwEGS(fb)
        ,VDWLF_ROW(2)   ,0|VDWLF_EH,fb_ctlbtnEGS(fb)  ,1|VDWLF_EH,fb_ctlbtnEGS(fb)
        ,-1);
    if ( fbd->embedded ) {
        // bad hook
        extern void gui_dw_resize_setBellow(Tvgui_dw* self, Tegs_widget w, Tegs_widget b);
        widget_place_GrowToEastSW(fb_ctlbtnEGS(fb),5,NULL,5,NULL);
        gui_dw_resize_setBellow(fb->fb_topDW,fb_col1DwEGS(fb),fb_ctlbtnEGS(fb));
        gui_dw_resize_setBellow(fb->fb_topDW,fb_col2DwEGS(fb),fb_ctlbtnEGS(fb));
    }

    return fb->fb_form;
}
static Tegs_widget xt_filebrowser_dbf(Twid_filebrowser* fbd)
{
    Widget widget;
    if ( fbd->embedded ) {
        widget = xt_filebrowser_dbf_I(fbd->pdialog,fbd);
    } else {
        Tvgui_dw* pdialog = vgui_dw_newTop(sizeof(*pdialog),
                vgui_dw_getMw(fbd->pdialog),NULL,WID_DIALSTATUS_Ok|WID_DIALSTATUS_CancelX);
        Widget fbwidget = xt_filebrowser_dbf_I(pdialog,fbd);
        Tgui_fileBrowser* fb = fbd->guiUserdata;
        fb->fb_topDW = pdialog;
        vgui_dw_finishIC(pdialog,0|VDWLF_EB,fbwidget,-1);
        widget = vgui_dw_getWidEGS(pdialog);
    }
    return widget;
}

static void xt_filebrowser_free(Twid_filebrowser*fbd)
{
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    if ( fb==0 )
        return;

    fbd->guiUserdata = 0;

    vgui_dw_delete       ( fb->fb_lin1Dw );
    vgui_dw_delete       ( fb->fb_col1Dw );
    vgui_dw_delete       ( fb->fb_col2Dw );

    vgui_dw_delete       ( fb->fb_btnHhrDw );
    wid_button_delete    ( fb->fb_btnHome );
    wid_button_delete    ( fb->fb_btnCwd  );
    wid_button_delete    ( fb->fb_btnRescan );
    wid_button_delete    ( fb->fb_btnHidden );
    wid_slin_free        ( fb->fb_fileKindTIN );
                        
    wid_slin_free        ( fb->fb_currDirTIN );
    astrset_delete       ( fb->fb_dirs ); 
    wid_slin_free        ( fb->fb_currFileTIN );
    astrset_delete       ( fb->fb_files ); 
  if ( fb->fb_ctlbtn )
    wid_buttonPanel_free ( fb->fb_ctlbtn ); 

    free                 ( fb );  
}

static TstrEaten xt_filebrowser_getDirPath(Twid_filebrowser* fbd) { return tool_strdup(fbd->dir); }
static TstrEaten xt_filebrowser_getBasName(Twid_filebrowser* fbd) { Tgui_fileBrowser* fb = fbd->guiUserdata; return gui_slin_getText(fb->fb_currFileTIN); }

static void xt_filebrowser_setBasName(Twid_filebrowser* fbd, CstrBrief bn)
{
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    gui_slin_setText(fb->fb_currFileTIN,bn);
}

static int  xt_filebrowser_modalRun   (Twid_filebrowser* fbd) {
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    int status = vgui_dw_run(fb->fb_topDW);
    return status;
}
static void xt_filebrowser_modalDelete(Twid_filebrowser* fbd) {
    Tgui_fileBrowser* fb = fbd->guiUserdata;
    vgui_dw_delete( fb->fb_topDW );
    xt_filebrowser_free(fbd);
}

/*======================================================================*/
/*= Interface                                                          =*/

extern void gui_filebrowser_init(Twid_filebrowser* fbd )
{
    fbd->vt_dbf          = xt_filebrowser_dbf;
    fbd->vt_free         = xt_filebrowser_free;
  //fbd->vt_getFilPath   = NULL;
    fbd->vt_getDirPath   = xt_filebrowser_getDirPath;
    fbd->vt_getBasName   = xt_filebrowser_getBasName;
    fbd->vt_setBasName   = xt_filebrowser_setBasName;
    fbd->vt_modal_run    = xt_filebrowser_modalRun;
    fbd->vt_modal_delete = xt_filebrowser_modalDelete;
}

extern void xt_filebrowser_resize(Twid_filebrowser* fbd )
{
    Tgui_fileBrowser* fb = fbd->guiUserdata;

    widget_resize_toEast(fbd->widEGS,0);
}

/*======================================================================*/
