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
 *$Software: YFIG-TMB (Top Menu Bar)
 *      $Id: tb-file.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/yfig/common/topbar/tb-file.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Creation of the file menu.                                         =*/
/*======================================================================*/

#include "topbar.h"
#include "yfig-dsds.h"
#include "ydw/ydw.h"
#include "ydw/tb-file-OMSF.h"

#include "fig/figure.h"
#include "fig/fig-scene.h"
#include "fig/object.h"
#include "tools/strings.h"
#include "tools/math-macros.h"
#include "tools/file.h"

/*======================================================================*/
/*= Interface                                                          =*/

static void action_FileNew    (Twidm_any*m, Twidm_entry* e);
static void action_FileOpen   (Twidm_any*m, Twidm_entry* e);
static void action_FileMerge  (Twidm_any*m, Twidm_entry* e);
static void action_FileSave   (Twidm_any*m, Twidm_entry* e);
static void action_FileSaveAs (Twidm_any*m, Twidm_entry* e);
static void action_FileExport (Twidm_any*m, Twidm_entry* e);
static void action_FilePrint  (Twidm_any*m, Twidm_entry* e);
static void action_FileExit   (Twidm_any*m, Twidm_entry* e);
static void action_FileLOP    (Twidm_any*m, Twidm_entry* e); // Last Open File

static Twidm_item table[] = {
//                me_label   , checkmark , me_action
  WIDM_ITEM_STR(  "New"      ,    0,0    , action_FileNew       ),
  WIDM_ITEM_STR(  "Open"     ,    0,0    , action_FileOpen      ),
  WIDM_ITEM_STR(  "Merge"    ,    0,0    , action_FileMerge     ),
#ifdef DIGITIZE 
  WIDM_ITEM_STR(  "Digitize" ,    0,0    , popup_digitize_panel ),
#endif /* DIGITIZE */
  WIDM_ITEM_STR(  "&Save"    ,    0,0    , action_FileSave      ),
  WIDM_ITEM_STR(  "Save As"  ,    0,0    , action_FileSaveAs    ),
  WIDM_ITEM_STR(  "Export"   ,    0,0    , action_FileExport    ),
  WIDM_ITEM_STR(  "&Print"   ,    0,0    , action_FilePrint     ),
  WIDM_ITEM_STR(  "&Exit"    ,    0,0    , action_FileExit      ),
  WIDM_ITEM_SEP(  /* makes a line separator */                  ),
  /* recently loaded files */
};

extern Twidm_item*  tb_file_t() { return table; }
extern int          tb_file_n() { return TOOL_TABLE_NbEle(table); }
extern void         tb_file_update(Cyfig_mw*ymw, Twidm_any* m)
{
    Tyfig_dsds* dsds  = ymw->ymw_dsds;
    TastrSet*   files = dsds->ds_recentFiles->rf_short;
    int         nbe   = tb_file_n();
    int i;

    for (i=0 ; i<files->eleNb ; i++) {
        char* path  = files->eles[i];
        int   meidx = i+nbe;
        if ( meidx<m->ma_entries->eleNb ) {
            Twidm_entry* me =  m->ma_entries->eles[meidx];
            me->me_label = path;
            widm_entry_update(me);
        } else {
            widm_any_addEntryByStrCbIv(m,path,action_FileLOP,i);
        }
    }
    while ( m->ma_entries->eleNb>(nbe+files->eleNb) ) {
        widm_any_supEntryLast(m);
    }
    for (i=0 ; i<files->eleNb ; i++) {
        char* path  = files->eles[i];
        int   meidx = i+nbe;
        m->ma_entries->eles[meidx]->me_ident = any_initBySInt(i);
    }
}

/*======================================================================*/
/*= Messages                                                           =*/

static const char* OPENMESS_NotSavedInNew =
"You asked for starting a new figure.\n"
"But the current figure is modified. Do you want\n"
"  - to start the new figure discarding the changes of the current one,\n"
"  - to preserve the current figure modifications by aborting the\n"
"    start of a new figure.\n"
"  - to preserve the current figure modifications by saving it\n"
"    before starting the new figure,\n" ;

static const char* OPENMESS_NotSavedInOpen =
"You asked for loading the figure:\n"
"  %s\n"
"But the current figure is modified. Do you want\n"
"  - to load the new figure discarding the changes of current one,\n"
"  - to preserve the current figure modifications by aborting the\n"
"    loading of the new figure.\n"
"  - to preserve the current figure modifications by saving it\n"
"    before loading the new figure,\n" ;

static const char* OPENMESS_OveridingFile =
"The \"%s\" file already exists.\n"
"\n"
"Do you want overide it?\n" ;

/*======================================================================*/
/*= Callbacks                                                          =*/

static void action_OMSF(Tyfig_mw*ymw, Tdial_OMSFkind type);

static void action_FileNew(Twidm_any*m, Twidm_entry* e)
{
    Twidm_popup* menu = e->me_menuPP;
    Tvgui*           gui  = menu->m_gui;
    Tyfig_mw*        ymw  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    if ( yfig_cursce_isModified(ymw) ) {
        int status = wid_pum_criticalWBL(&ymw->any,
            WID_DIALSTATUS_CancelX,     NULL,
            WID_DIALSTATUS_NewDiscard,  NULL,
            WID_DIALSTATUS_Save,       "Save current\n& continue",
            WID_DIALSTATUS_None,
            OPENMESS_NotSavedInNew);
        switch ( status ) {
            case WID_DIALSTATUS_Save:
                if ( yfig_cursce_save(ymw,0)!=0 ) return;
                break;
            case WID_DIALSTATUS_NewDiscard:  break;
            default: /* cancel */     return;
        }
    }
    yfig_cursce_new(ymw);
    yfig_cursce_redraw(ymw);
    DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
}

static void action_FileOpen(Twidm_any*m, Twidm_entry* e)
{
    Twidm_popup* menu = e->me_menuPP;
    Tyfig_mw*    yds  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    action_OMSF(yds, OMSF_KD_Open);
    DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
}

static void action_FileMerge(Twidm_any*m, Twidm_entry* e)
{
    Twidm_popup* menu = e->me_menuPP;
    Tyfig_mw*    yds  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    action_OMSF(yds, OMSF_KD_Merge);
    DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
}

static void action_FileSave(Twidm_any*m, Twidm_entry* e)
{
    Twidm_popup* menu = e->me_menuPP;
    Tyfig_mw*    ymw  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    if ( yfig_cursce_isModified(ymw)==0 ) {
        DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
        yfig_mw_wrnerr_printf(ymw,
            "Figure is not saved because file is up to date."
            "Use the \"File->Save As\" item to force the back-up.");
        return;
    }
    if ( yfig_cursce_saveFilename(ymw)==0 ) {
        action_OMSF(ymw, OMSF_KD_SaveAs);
        DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
        return;
    }
    yfig_cursce_save(ymw,0);
    DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
}

static void action_FileSaveAs(Twidm_any*m, Twidm_entry* e)
{
    Twidm_popup* menu = e->me_menuPP;
    Tyfig_mw*    yds  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    action_OMSF(yds, OMSF_KD_SaveAs);
    DBG1_MenuL("top->%s->%s\n",menu->m_label,e->me_label);
}

static void action_FileExportPrint(Twidm_any*m, Twidm_entry* e, int flag)
{
    Tvgui*       gui  = m->ma_gui;
    Twidm_popup* menu = e->me_menuPP;
    Tyfig_mw*    ymw  = menu->m_appCBud;
    DBG0_MenuE("top->%s->%s\n",menu->m_label,e->me_label);
    Tydw_export*    dialog = ydw_export_nbf(ymw,flag);
    Twid_dialStatus status;
    int quit=0;
    while ( ! quit ) {
        status = ydw_export_run(dialog);
        if ( status==WID_DIALSTATUS_Ok ) {
            quit = ydw_export_do(dialog)==0;
            if ( quit ) ydw_export_updateOption(dialog);
        } else {
            quit=1;
        }
    }

    ydw_export_delete(dialog);
    DBG1_MenuL("top->%s->%s stautus=%s\n",menu->m_label,e->me_label,vgui_dw_status2buttLab(status));
}

static void action_FileExport(Twidm_any*m, Twidm_entry* e) { action_FileExportPrint(m,e,0); }
static void action_FilePrint (Twidm_any*m, Twidm_entry* e) { action_FileExportPrint(m,e,1); }

static void action_FileExit(Twidm_any*m, Twidm_entry* e)
{
    Tvgui*       gui  = m->ma_gui;
    Twidm_popup* menu = e->me_menuPP;
    DBG0_MenuSelectedE(e);
    gui_loop_break( gui );
    DBG1_MenuSelectedL(e);
}

/*======================================================================*/
/*= Engine for figure files                                            =*/

static void action_OMSF(Tyfig_mw*ymw, Tdial_OMSFkind type)
{
    Tvgui*           gui  = ymw->ymw_gui;
    Tyfig_dsds*      dsds = gui->g_ud;
    Twid_dialStatus  status,status2;
    Tdial_OMSF*      d;

    DBG1_MenuE("type=%d",type);
    if ( type==OMSF_KD_Open ) {
        d = dial_OMSF_new(ymw,type,ymw->ymw_OMSFdir,0);
    } else if ( type==OMSF_KD_Merge ) {
        Cstr path     =  yfig_cursce_saveFilename(ymw);
        if ( path==0 ) {
            d = dial_OMSF_new(ymw,type,ymw->ymw_OMSFdir,0);
        } else {
            TstrEaten dirname  =  tool_dirname ( path );
            d = dial_OMSF_new(ymw,type,dirname,NULL);
            tool_free(dirname);
        }
    } else   /* type==OMSF_KD_SaveAs */ {
        Cstr path     =  yfig_cursce_saveFilename(ymw);
        if ( path==0 ) {
            d = dial_OMSF_new(ymw,type,ymw->ymw_OMSFdir,0);
        } else {
            TstrEaten basename =  tool_basename( path );
            TstrEaten dirname  =  tool_dirname ( path );
            d = dial_OMSF_new(ymw,type,dirname,basename);
            tool_free(basename); tool_free(dirname);
        }
    }

    TstrEaten dir=0,path=0;
    double    x,y,scale;
    int       aok;

#define ERR_NoOrInvalidFile() vgui_dw_errerr_printf(&d->any,"No file path or invalid file path.")
    
retry:
    tool_strfreeS0( &path );
    status = vgui_dw_run(&d->any);
    TEMP_ase_printf(status==0,"%s","gui_dw_run returned 0.");
    path = dial_OMSF_getParam(d,&x,&y,&scale);
    if ( status==WID_DIALSTATUS_Cancel ) {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        DBG1_MenuM("OMSF Canceled",0);
    } else if ( status==WID_DIALSTATUS_New ) {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        DBG1_MenuM("OMSF New figure",0);
        if ( yfig_cursce_isModified(ymw) ) {
            status2 = wid_pum_criticalWBL(&ymw->any,
              WID_DIALSTATUS_CancelX,     NULL,
              WID_DIALSTATUS_NewDiscard,  NULL,
              WID_DIALSTATUS_Save,       "Save current\n& continue",
              WID_DIALSTATUS_None,
              OPENMESS_NotSavedInNew);
            switch ( status2 ) {
                case WID_DIALSTATUS_Save:
                    if ( yfig_cursce_save(ymw,0)!=0 ) goto end;
                    break;
                case WID_DIALSTATUS_NewDiscard:  break;
                default: /* cancel */            goto end;
            }
        }
        yfig_cursce_new(ymw);
    } else if ( status==WID_DIALSTATUS_Ok && type==OMSF_KD_Open ) {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        DBG1_MenuM("OMSF Open File: x=%f y=%f scale=%f path=%s",x,y,scale,path);
        if ( path == 0 ) {
            ERR_NoOrInvalidFile();
            goto retry;
        }
        if ( yfig_cursce_isModified(ymw) ) {
            status2 = wid_pum_criticalWBL(&ymw->any,
              WID_DIALSTATUS_CancelX,     NULL,
              WID_DIALSTATUS_OpenDiscard, NULL,
              WID_DIALSTATUS_Save,       "Save current\n& continue",
              WID_DIALSTATUS_None,
              OPENMESS_NotSavedInOpen,path);
            switch ( status2 ) {
                case WID_DIALSTATUS_Save:
                    if ( yfig_cursce_save(ymw,0)!=0 )
                        goto end;
                    break;
                case WID_DIALSTATUS_OpenDiscard:
                    break;
                default: /* cancel */
                   goto end;
            }
        }
            
        if ( tool_fileOpenROCheck(path)!=0 ) {
            DBG2_MenuM("OMSF Open File: new fig with path=%s",path);
            yfig_cursce_new(ymw);
            yfig_cursce_chgFigPath(ymw,path);
            yfig_cursce_redraw(ymw);
            vgui_dw_wrnerr_printf(&d->any,"Switched to figure: %s\n",path);
        } else if ( d->df_fig!=0 && tool_strcmp(d->df_fig->path,path)==0 ) {
            CboxDble* box = fig_scene_BBox(d->df_preview->Fscene);
            double tx = x-Box_x(*box); if ( dble_close0AtP(tx,1e-1) ) tx=0;
            double ty = y-Box_y(*box); if ( dble_close0AtP(ty,1e-1) ) ty=0;
            DBG2_MenuM("OMSF Open File: load fig with shift & scale :"
                    " tx=%f ty=%f scale=%f path=%s",tx,ty,scale,path);
            yfig_cursce_newByPathScaleAndTrans(ymw,path,scale,tx,ty);
        } else {
            DBG2_MenuM("OMSF Open File: load fig without shift & scale : ath=%s",path);
            yfig_cursce_newByPath(ymw,path);
        }
    } else if ( status==WID_DIALSTATUS_Ok && type==OMSF_KD_Merge ) {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        DBG1_MenuM("OMSF Merge File: x=%f y=%f scale=%f path=%s",x,y,scale,path);
        if ( path == 0 ) {
            ERR_NoOrInvalidFile();
            goto retry;
        }
        Tfigure* fig = fig_new_byFile(ymw->ymw_msgdrv,path,dsds->ds_baseColors);
        if ( fig==0 || fig->objects==0 ) {
            DBG2_MenuM("OMSF Merge File: FAILED\n",0);
            vgui_dw_errerr_printf(&d->any,
                "Can not open file for reading or bad formatted file: %s",path);
            goto retry;
        }
        fig_update_forDrawing(fig,gui->colorDriver);
        TobjAll clone = obj_newClone( obj_ObjAll(fig->objects) );
        obj_chgPpi( clone, YMW_figure(ymw)->fig_ppi );
        if ( d->df_fig!=0 && tool_strcmp(d->df_fig->path,path)==0 ) {
            CboxDble* box = obj_BBox( clone );
            double tx = x - Box_x(*box);
            double ty = y - Box_y(*box);
            if ( ! (dble_close0AtP(scale,0.01)) )
                obj_zoom( clone, scale);
            if ( ! (dble_close0AtP(tx,0.01) && dble_close0AtP(ty,0.01)) )
                obj_translation( clone, tx, ty);
            DBG2_MenuM("OMSF Merge File: load fig with shift & scale :"
                    " tx=%f ty=%f scale=%f path=%s",tx,ty,scale,path);
        }
        yfig_cursce_addObjWHis(ymw, clone);
        fig_delete( fig );
        yfig_cursce_redraw(ymw);
    } else if ( status==WID_DIALSTATUS_Ok && type==OMSF_KD_SaveAs ) {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        DBG1_MenuM("OMSF SaveAs File: %s",path);
        if ( path == 0 ) {
            ERR_NoOrInvalidFile();
            goto retry;
        }
        if ( tool_fileOpenWOCheck(path)==0 ) {
            if ( wid_pum_criticalWBL(&ymw->any,
                    WID_DIALSTATUS_No,   NULL,
                    WID_DIALSTATUS_Yes,  NULL,
                    WID_DIALSTATUS_None,
                    OPENMESS_OveridingFile,path)!=WID_DIALSTATUS_Yes ) 
                goto retry;
        } else {
            TstrEaten dir = tool_dirname(path);
            if ( tool_fileDirXWECheck(dir) ) {
                vgui_dw_errerr_printf(&d->any,"Can not open file for writing: %s",path);
                tool_free( dir );
                goto retry;
            }
        }
        Tfigure*  fig    = yfig_cursce_fig(ymw);
        TstrEaten old_fn = tool_strdup( fig->path );
        yfig_cursce_chgFigPath(ymw,path);
        if ( yfig_cursce_save(ymw,1)!=0 ) {
            vgui_dw_errerr_printf(&d->any,"Saving current figure failed: %s",path);
            yfig_cursce_chgFigPath(ymw,old_fn);
            tool_free( old_fn );
            goto retry;
        } /* else  // no message here because yfig_cursce_save prints it
           * vgui_dw_inferr_printf(&d->any,"Current figure saved into \"%s\" * file.",path); */
        tool_free(old_fn);
    }
end:
    dir  = dial_OMSF_getDir(d);
    if ( dir )
        tool_strchgEaten(&ymw->ymw_OMSFdir,dir);
    tool_strfreeS0( &path );
    dial_OMSF_free(d);
    DBG1_MenuL("type=%d",type);
}

/*======================================================================*/

static void action_FileLOP    (Twidm_any*m, Twidm_entry*me)
{
    int status;
    Tyfig_mw*     ymw  = m->ma_appCBud;
    Tyfig_dsds*   dsds = ymw->ymw_dsds;
    TrecentFiles* rfs  = dsds->ds_recentFiles;
    Cstr path          = rfs->rf_absolute->eles[me->me_ident.sint];
    DBG0_MenuE("Open Recent File: path=%s",path);

    if ( tool_fileOpenROCheck(path)!=0 ) {
        yfig_mw_errerr_printf(ymw,"\"%s\" file can not be read.",path);
        goto remove_file;
    }
    if ( yfig_cursce_isModified(ymw) ) {
        status = wid_pum_criticalWBL(&ymw->any,
          WID_DIALSTATUS_CancelX,     NULL,
          WID_DIALSTATUS_OpenDiscard, NULL,
          WID_DIALSTATUS_Save,       "Save current\n& continue",
          WID_DIALSTATUS_None,
          OPENMESS_NotSavedInOpen,path);
        switch ( status ) {
            case WID_DIALSTATUS_Save:
                if ( yfig_cursce_save(ymw,0)!=0 )
                    goto end;
                break;
            case WID_DIALSTATUS_OpenDiscard:
                break;
            default: /* cancel */
               goto end;
        }
    }
    DBG0_MenuM("Open Recent File: path=%s (trying to load it)",path);
    if ( yfig_cursce_newByPath(ymw,path) ) {
        yfig_mw_errerr_printf(ymw,"loading \"%s\" file failed.",path);
remove_file:
        tool_rf_sup(rfs,me->me_ident.sint);
        tb_file_update(ymw,m);
    }

end:
    DBG0_MenuL("Open Recent File: path=%s",path);
    return;
}

/*======================================================================*/
