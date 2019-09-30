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
 *      $Id: wid-filebrowser.h 331 2018-10-30 21:35:47Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/wid-filebrowser.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================
 * File browser widget.
 *
 * The vgui module does not creates file browser because most of EGS
 * already have a such widget. So this module just provides an interface
 * to an EGS filebrowser widget.
 * The EGS file browser widget is 
 *   - either embedded to a dialog widget as a sub-dialog.
 *   - or it generates a sub-dialog that contains a line 
 *         file-label text-input browse-button
 * Clicking the browse button starts a modal file browser dialog.
 * The module chooses implementation kind using gui->g_unsprtEmbeddedFb.
 *
 * A callback is performed every time the user pre-selects a file.
 * At dialog end, the dialog status (Twid_dialStatus) indicates if
 * the user has really selected a file or aborted the dialog.
 *
**======================================================================*/

/*======================================================================*/
#ifndef FILE_WID_FILEBROWSER_H
#define FILE_WID_FILEBROWSER_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui/vgui-types.h"
#include "vgui/wid-simple.h"  // FIXME for Twid_dialStatus enum type

/*======================================================================*/
/*= Data structure                                                     =*/
/*=                                                                    =*/
/*= - Twid_filebrowser   : structure for input and output parameters.  =*/

// the exchange data structure
struct _Twid_filebrowser {
    Tvgui_dw*    pdialog;     // the parent dialog
    int          embedded;    // 1: the EGS file browser widget is added to the pdialog widget.
                              // 0: a "|file-text-in| |browse-button|" sub-dialog is added to
                              //    the pdialog widget. A click the "browse" button starts a
                              //    EGS file browser top-dialog in modal mode.

    int        existingPath;  // Search for a file path:
                              // 0: file may not exist in FS.
                              // 1: an existing one in FS.

    int          addBtns;     // 1:EGS need to add buttons on filebrowsers
    TstrEaten    cancelText;  // label of cancel button
    TstrEaten    okText;      // label of ok button
    TstrEaten    extraText;   // Label of an extra button and the status that
    Twid_dialStatus           // that is returned when it is clicked.
                 extraStatus; // Leaving them to default disables it.

    TstrEaten    filemaskId;  // Human Ident. for filemask (eg: "JPEG")
    TstrEaten    filemask;    // regexprs for file name (eg: "*.jpg *.JPG *.jpeg")
    TastrSet*    filemasks;   // format of an entry:
                              //   [<human idend.>:]<file name regexpr list>
                              // ex: "JPEG:*.jpg *.JPG *.jpeg"
                              // or  "*.jpg *.JPG *.jpeg"

    TstrEaten    dir;         // start to browse from this directory
    int          dx,dy;       // preferred geometry (<=0 means default)

    // callback functions:
    //   appFileIsSelectedCB: From VGUI to top appliccation.
    //   guiFileIsSelectedCB: From EGS driver to VGUI. It's never null.
    void       (*appFileIsSelectedCB) (Tvgui_dw* d, CstrBrief path);
    void       (*guiFileIsSelectedCB) (Twid_filebrowser* fbd, CstrBrief path); 
 
    Tegs_widget  widEGS;      // the sub-dialog widget to insert into parent dialog (pdialog).
                              //   - embedded=1 : the EGS file browser widget.
                              //   - embedded=0 : modal_dw->dw_widEGS

    // embedded=0 : The dw sub-dialog that is added to the parent dialog. 
    //                  (|file-text-in-widget| |browse-button|)
    //              It type is a derived class of Tvgui_dw
    void*   modal_dw;    

  /**
    * All theses fields must be set by the gui_filebrowser_init function.
    * The modfor unembedded modal file browser (embedded==0) and
    *
    * vt_dbf       : It builds the EGS filebrowser widget.
    * vt_free      : It frees the memory that vt_dbf allocated.
    *                Iit can be left to null
    * vt_getFilPath: It returns the absolute path of the selected file.
    * vt_getDirPath: It returns dirname of the absolute path of the
    *                selected file.
    * vt_getBasName: It returns the base name of the selected file.
    * vt_setBasName: The base name the file browser must show.
    * vt_modal_run : This function is called for starting the modal
    *                top dialog. It returns:
    *                  1: The user selected a file.
    *                  0: The user did not select a file (canceled the dialog).
    * vt_modal_delete: This function is called to destroy the file browser
    *                modal top dialog.
    * NOTE:
    *   - You must provide either vt_getFilPath or (vt_getDirPath and
    *     vt_getBasName).
    *   - modal_run and modal_delete must be set for unembedded modal
    *     file browser (embedded==0) and they must be left to null for
    *     embedded modal file browser (embedded==1).
   **/
    void*       guiUserdata; // reserved to gui.
    Tegs_widget (*vt_dbf)         (Twid_filebrowser* fbd);
    void        (*vt_free)        (Twid_filebrowser* fbd);
    TstrEaten   (*vt_getFilPath)  (Twid_filebrowser* fbd);
    TstrEaten   (*vt_getDirPath)  (Twid_filebrowser* fbd);
    TstrEaten   (*vt_getBasName)  (Twid_filebrowser* fbd);
    void        (*vt_setBasName)  (Twid_filebrowser* fbd, CstrBrief bn);
    int         (*vt_modal_run)   (Twid_filebrowser*fbd);
    void        (*vt_modal_delete)(Twid_filebrowser*fbd);

    // VGUI private data
    TstrEaten    filename;    // the name of file when the filebrowser starts.
    TstrEaten    lastSelPath; // path of the last selected file.
};

/*======================================================================*/
/*= Constructors/destructor                                            =*/

/**
 *  - wid_filebrowser_new:    creates a default Twid_filebrowser.
 *  - wid_filebrowser_delete: frees the exchange structure 
 *
 *  - vgui_filebrowser_getDirPath: get the current directory path where
 *                           files are browsed.
 *  - gui_filebrowser_getFilePath: get the path of selected file, useful  
 *                           at the dialog end.
**/
extern Twid_filebrowser*
                  wid_filebrowser_new   (Tvgui_dw*dw, int epath,
                                         void (*cb)(Tvgui_dw* d, CstrBrief path),
                                         CstrBrief dir);
extern void       wid_filebrowser_dbf   (Twid_filebrowser*fbd);
extern void       wid_filebrowser_delete(Twid_filebrowser*fbd);
extern void       gui_filebrowser_init  (Twid_filebrowser*fbd);

// init. filemaskId, filemask and filemasks from tool image data DB
// for image format that are defined by the kds/kdn array.
// The function assumes that the elements of the kds array are
// values of the TimgFmtKd enum.
extern void      vgui_filebrowser_initFileMaskT(Twid_filebrowser* fbd,
                                                const int*kds, int kdn);
// Initialize the file path that the file widget will show at start time
// for "save" filebrowsers.
// fn must be an absolute path.
// CAUTION: This function must be called before the creation of the
//   associated EGS widget
extern void      vgui_filebrowser_initSaveFilName(Twid_filebrowser* fbd, CstrBrief fn);

/*======================================================================*/
/*= Utilities for run time                                             =*/

// change the base name the filebrowser widget currently skows.
extern void      vgui_filebrowser_setBasName (Twid_filebrowser* fbd, CstrBrief bn);

// CAUTION: these functions must be only used:
//   1) in the fileIsSelected callback.
//   2) once file browser returned a OK status.
extern TstrEaten vgui_filebrowser_getDirPath (Twid_filebrowser* fbd);
extern TstrEaten vgui_filebrowser_getBasName (Twid_filebrowser* fbd);
extern TstrEaten vgui_filebrowser_getFilPath (Twid_filebrowser* fbd);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_WID_FILEBROWSER_H
/*======================================================================*/
