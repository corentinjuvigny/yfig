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
 *      $Id: wid-filebrowser.c 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/vgui/wid-filebrowser.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for file browser widget.                                 =*/
/*= See the vgui/wid-filebrowser.h header for documentation.           =*/
/*======================================================================*/

#include "vgui-config.h"
#include "vgui/vgui.h"
#include "vgui/vgui-dw.h"
#include "vgui/wid-simple.h"
#include "vgui/wid-button.h"
#include "vgui/wid-filebrowser.h"
#include "tools/file.h"
#include "tools/imgfmt.h"
#include "tools/strings.h"

/*======================================================================*/
/*= Embedded sub-dialog for unembedded filebrowser.                    =*/

typedef struct _Tmodal_filebrowser {
    Tvgui_dw           any;
    Tegs_widget        fileLab;
    Twid_slin*         fileIn;
    Twid_button*       browseBtn;
    Twid_filebrowser*  fbd;
    int                isRunning;
} Tmodal_filebrowser;

static void  modal_filebrowser_run(Tvgui_dw*vdw, int btnId, void*ud)
{
    Tstr str,str2;
    Tmodal_filebrowser* self = (Tmodal_filebrowser*)vdw;
    Twid_filebrowser*  fbd   = self->fbd; 
    Tegs_widget        fbwid;

    if ( ! TOOL_strIsEmpty(fbd->filename) ) {
        str = tool_dirname(fbd->filename);
        tool_strchgEaten(&fbd->dir,str);
    }
    fbwid = fbd->vt_dbf(fbd);
    if ( ! TOOL_strIsEmpty(fbd->filename) ) {
        str = tool_basename(fbd->filename);
        fbd->vt_setBasName(fbd,str);
        tool_free(str);
    }
    TstrEaten lastSelectedfile = tool_strdup( fbd->lastSelPath );
    self->isRunning = 1;
    int status = fbd->vt_modal_run(fbd);
    TEMP_asf_printf(status!=WID_DIALSTATUS_Ok && status!=WID_DIALSTATUS_Cancel
                 && status!=fbd->extraStatus, 
            "vt_modal_run(fbd)=%08x is invalid. "
            "Expected %08x (ok), %08x (cancel) or %08x (extra)",
            status, WID_DIALSTATUS_Ok, WID_DIALSTATUS_Cancel, fbd->extraStatus);

    if ( status==WID_DIALSTATUS_Ok ) {
        TstrEaten filename;
        if ( fbd->vt_getFilPath )
            filename =  fbd->vt_getFilPath(fbd);
        else {
            str      =  fbd->vt_getDirPath(fbd);
            str2     =  fbd->vt_getBasName(fbd);
            filename =  tool_aprintf("%s/%s",str,str2);
            tool_free(str); tool_free(str2);
        }
        tool_strchgClone(&fbd->lastSelPath,filename);
        tool_strchgEaten(&fbd->filename   ,filename);
        wid_slin_setText(self->fileIn, fbd->filename);
    } else { // Cancel or extra
        fbd->guiFileIsSelectedCB(fbd,lastSelectedfile);
    }
    tool_free(lastSelectedfile);

    fbd->vt_modal_delete(fbd);
    self->isRunning = 0;
}

#if 0
static void  modal_filebrowser_chg_cb(Twid_slin*self, void*ud)
{ Twid_filebrowser* fbd  = ud; Tstr tmp=wid_slin_getText(self);
  Tvgui_dwTop* top = vgui_dw_getTopData(fbd->pdialog);
  PERM_spw_printf("tin=%s mask=%08x",tmp,top->dt_buttsMaskCur); tool_free(tmp); }
#else
#define modal_filebrowser_chg_cb NULL
#endif

static void  modal_filebrowser_val_cb(Twid_slin*self, void*ud)
{
    Twid_filebrowser* fbd  = ud; 
    TstrEaten         path = wid_slin_getText(self);

//{ Tvgui_dwTop* top = vgui_dw_getTopData(fbd->pdialog); Tstr tmp=wid_slin_getText(self);
//  TEMP_spw_printf("tin=%s mask=%08x",tmp,top->dt_buttsMaskCur); tool_free(tmp); }
    Tvgui_dw* topdw =vgui_dw_getTopDialog(fbd->modal_dw);
    Tvgui_dwTop* top  =  topdw->top;
    tool_strchgEaten(&fbd->filename,path);

    path = vgui_filebrowser_getFilPath(fbd);
    int quit = path!=0;
    tool_free( path );
    if ( quit )
        vgui_dw_breakloop(vgui_dw_getTopDialog(fbd->modal_dw), WID_DIALSTATUS_Ok);
    else if ( ! TOOL_strIsEmpty(fbd->filename) ) {
        vgui_dw_errerr_printf(fbd->pdialog,"Can not use the \"%s\" file.",fbd->filename);
    }
}

/*======================================================================*/
/*= callback for EGS driver                                            =*/

static void filebrowser_sel_cb(Twid_filebrowser* fbd, CstrBrief path)
{
    tool_strchgClone(&fbd->lastSelPath,path);
    if ( fbd->appFileIsSelectedCB )
        fbd->appFileIsSelectedCB( fbd->pdialog, fbd->lastSelPath );
}

/*======================================================================*/
/*= Constructors/destructor                                            =*/

extern Twid_filebrowser* wid_filebrowser_new(Tvgui_dw*dw,
        int epath, void (*cb)(Tvgui_dw* d, CstrBrief path), CstrBrief dir)
{
    Tvgui* gui = vgui_dw_getVGui(dw);
    Twid_filebrowser* fbd = tool_malloc_0( sizeof(*fbd) );
    fbd->pdialog         = dw;
    if ( gui->g_unsprtEmbeddedFb==0 )
        fbd->embedded    = 1;
    fbd->existingPath    = epath;
    if ( TOOL_strIsEmpty(dir) )
        fbd->dir         = tool_getcwd_eated();
    else
        fbd->dir         = tool_strdup(dir);
    fbd->cancelText      = tool_strdup("Cancel");
    fbd->okText          = tool_strdup("Ok");
    fbd->dx              = 350;
    fbd->dy              = 250;
    fbd->appFileIsSelectedCB = cb;
    fbd->guiFileIsSelectedCB = filebrowser_sel_cb;
    //fbd->filename        = tool_strdup("Current file");
    fbd->addBtns         = 1;

    gui_filebrowser_init(fbd);
    TEMP_asf_printf( fbd->embedded==0 && (fbd->vt_modal_run==0 || fbd->vt_modal_delete==0),
            "%s", "gui_filebrowser_init returned a bad configuration for modal filebrowser");
    return fbd;
}

extern void   wid_filebrowser_dbf (Twid_filebrowser*fbd)
{ 
    Tvgui* gui = vgui_dw_getVGui(fbd->pdialog);

    if ( fbd->embedded ) {
        fbd->widEGS = fbd->vt_dbf(fbd);
        vgui_dw_enableBUTorg(fbd->pdialog,WID_DIALSTATUS_Ok);
        vgui_dw_enableBUTorg(fbd->pdialog,WID_DIALSTATUS_Cancel);
      if ( fbd->extraText )
        vgui_dw_enableBUTorg(fbd->pdialog,fbd->extraStatus);
    } else {
        Tmodal_filebrowser* self = (Tmodal_filebrowser*) vgui_dw_newSub(sizeof(*self), fbd->pdialog, NULL);
        self->fbd = fbd;
        fbd->modal_dw = self;
        fbd->widEGS   = self->dw_widEGS;
        self->fileLab = widD_label_nbf(SelfA, "file", NULL);
        self->fileIn  = widD_slin_nbf(SelfA, fbd->filename, "-----------------------------------", 
            modal_filebrowser_chg_cb, modal_filebrowser_val_cb, fbd);
        self->browseBtn = widD_button_std_nbfLab(SelfA, "Click to select the file path", modal_filebrowser_run, NULL, NULL, "Browse", NULL);
        if ( fbd->addBtns ) {
            vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_Cancel  , fbd->cancelText);
          if ( fbd->extraText )
            vgui_dw_addBtnCtl(SelfA,fbd->extraStatus       , fbd->extraText);
            vgui_dw_addBtnCtl(SelfA,WID_DIALSTATUS_Ok      , fbd->okText);
        }
        vgui_dw_finishIL(SelfA, 0,self->fileLab, VDWLF_EH,self->fileIn->widEGS, 0,self->browseBtn->but_widEGS, -1);
    }
}

extern void wid_filebrowser_delete(Twid_filebrowser*fbd)
{
    if ( fbd->vt_free ) fbd->vt_free(fbd);

    tool_free     ( fbd->dir );
    tool_free     ( fbd->cancelText );
    tool_free     ( fbd->okText );
    tool_free     ( fbd->extraText );
    tool_free     ( fbd->filemaskId );
    tool_free     ( fbd->filemask );
    tool_free     ( fbd->filename );
    astrset_delete( fbd->filemasks );
    free          ( fbd );
}

extern void vgui_filebrowser_initFileMaskT(Twid_filebrowser* fbd,
                                                const int*kds, int kdn)
{
    CimgFmtDt* dt;
    int i,j;
    char all[100*IMGFMT_RasNb]; int allidx=0;

    fbd->filemasks = astrset_new(10);

    for (i=0 ; i<kdn ; i+=1) {
      dt = tool_imgfmt_getByKd( kds[i] );
      char tmp[200]; int tmpidx=0;
      for (j=0 ; dt->exts[j] ; j+=1)
        tmpidx += sprintf(tmp+tmpidx,"%s*.%s",j==0 ? "" : " ",dt->exts[j]);
      if ( j>0 ) {
        astrset_add_printf(fbd->filemasks,"%s:%s",dt->shortname,tmp);
        allidx += sprintf(all+allidx,"%s%s",i==0 ? "" : " ",tmp);
      }
    }
    fbd->filemaskId = tool_strdup( "Image" );
    fbd->filemask   = tool_strdup( all     );
}

extern void vgui_filebrowser_initSaveFilName(Twid_filebrowser* fbd, CstrBrief fn)
{
    TEMP_asf_printf(fbd->modal_dw,"%s","Invalid call. The EGS widget is already built");
    tool_strchgClone(&fbd->filename,fn);
}

/*======================================================================*/
/*= Utilities for run time                                             =*/

extern TstrEaten vgui_filebrowser_getDirPath(Twid_filebrowser* fbd)
{
    char* dir = fbd->dir;

    if ( TOOL_strIsEmpty(dir) ) return 0;

    if ( access(dir,R_OK)!=0 ) return 0;

    return tool_strdup(dir);
}

extern TstrEaten vgui_filebrowser_getBasName (Twid_filebrowser* fbd)
{
    Tstr filepath,basename;
    Tmodal_filebrowser* modal_dw = fbd->modal_dw;
    TEMP_asf_printf(!fbd->embedded && !modal_dw,"%s","found unexpected state: fbd->embedded==0 && fbd->modal_dw==0");
    if ( fbd->embedded || modal_dw->isRunning ) {
        if ( fbd->vt_getFilPath ) {
            filepath = fbd->vt_getFilPath(fbd);
            basename = tool_basename(filepath);
            tool_free(filepath);
        } else {
            TEMP_asf_printf(fbd->vt_getBasName==0,"%s","vt_getFilPath & vt_getBasName are both null");
            basename = fbd->vt_getBasName(fbd);
        }
    } else {
        // modal top dialog is not running, filename contains the filename.
        basename = tool_basename(fbd->filename);
    }
    return basename;
}

extern TstrEaten vgui_filebrowser_getFilPath (Twid_filebrowser* fbd)
{
    Tstr filepath=0,dirpath,basename;
    Tmodal_filebrowser* modal_dw = fbd->modal_dw;
    TEMP_asf_printf(!fbd->embedded && !modal_dw,"%s","found unexpected state: fbd->embedded==0 && fbd->modal_dw==0");
    if ( fbd->embedded || modal_dw->isRunning ) {
        if ( fbd->vt_getFilPath ) {
            filepath = fbd->vt_getFilPath(fbd);
            if ( TOOL_strIsEmpty(filepath) ) {
                tool_freeS0((void**)&filepath);
            }
        } else {
            TEMP_asf_printf(fbd->vt_getBasName==0,"%s","vt_getFilPath & vt_getBasName are both null");
            TEMP_asf_printf(fbd->vt_getDirPath==0,"%s","vt_getFilPath & vt_getBasName are both null");
            basename = fbd->vt_getBasName(fbd);
            if ( TOOL_strIsEmpty(basename) ) {
                tool_free(basename);
            } else {
                TEMP_ase_printf( strstr(basename,"(null)")!=0,"fbd->vt_getBasName returned '(null)': %s",basename);
                dirpath  = fbd->vt_getDirPath(fbd);
                if ( TOOL_strIsEmpty(dirpath) ) {
                    tool_free(dirpath);
                } else {
                    TEMP_ase_printf( strstr(dirpath,"(null)")!=0,"fbd->vt_getDirPath returned '(null)': %s",dirpath);
                    filepath = tool_aprintf("%s/%s",dirpath,basename);
                    tool_free(dirpath); tool_free(basename);
                }
            }
        }
    } else {
        // modal top dialog is not running, the file name that the user selected
        // is in modal_dw->fileIn and/or filename.
        Tmodal_filebrowser* modal_dw = fbd->modal_dw;
        filepath = wid_slin_getText( modal_dw->fileIn);
        if ( TOOL_strIsEmpty(filepath) ) {
            tool_free(filepath);
            filepath = tool_strdup(fbd->filename);
        }
        if ( TOOL_strIsEmpty(filepath) )
            tool_freeS0((void**)&filepath);
    }

    // check validty of filepath
    if ( filepath!=0 ) {
        if ( fbd->existingPath ) {
            // file must exist and readable
            if ( tool_fileOpenROCheck(filepath)!=0 )
                tool_freeS0((void**)&filepath);
        }
    }

    return filepath;
}

extern void      vgui_filebrowser_setBasName (Twid_filebrowser* fbd, CstrBrief bn)
{
    Tmodal_filebrowser* modal_dw = fbd->modal_dw;
    TEMP_asf_printf(!fbd->embedded && modal_dw==0,"%s","found unexpected state: fbd->embedded==0 && fbd->modal_dw==0");
    if  ( ! fbd->embedded ) {
        Tstr filepath = tool_aprintf("%s/%s",fbd->dir,bn);
        wid_slin_setText(modal_dw->fileIn,filepath);
        tool_free(filepath);
    } else
        fbd->vt_setBasName(fbd,bn);
}

/*======================================================================*/
