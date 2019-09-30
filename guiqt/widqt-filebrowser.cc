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
 *      $Id: widqt-filebrowser.cc 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-filebrowser.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= File browser widget (EGS driver).                                  =*/
/*=                                                                    =*/
/*= See the vgui/wid-filebrowser.h header for documentation.           =*/
/*======================================================================*/

#include "widqt-filebrowser.h"
#include "tools/strings.h"

/*======================================================================*/
/* Implementation of TGUI_fileBrowser                                  =*/

TGUI_fileBrowser::TGUI_fileBrowser(TGUI_dialog*_d, Twid_filebrowser* _fbd)
:  fbd(_fbd)
{
    char* tmp;
    fbd->guiUserdata = (Tegs_widget)this;
    setDirectory(fbd->dir);
    QStringList filters;
    if ( fbd->filemaskId && fbd->filemask ) {
        tmp=tool_aprintf("%s files (%s)",fbd->filemaskId,fbd->filemask);
        filters << tmp;
        free(tmp);
    } else if ( fbd->filemask ) {
        tmp=tool_aprintf("(%s)",fbd->filemask);
        filters << tmp;
        free(tmp);
    }
    if ( fbd->filemasks!=0 ) for (int i=0 ; i<fbd->filemasks->eleNb ; i+=1) {
        char *str = fbd->filemasks->eles[i];
        char *sep = strchr(str,':');
        if ( sep==0 ) {
            tmp=tool_aprintf("(%s)",str);
            filters << tmp;
            free(tmp);
        } else {
            *sep=0;
            tmp=tool_aprintf("%s files (%s)",str,sep+1);
            filters << tmp;
            free(tmp);
            *sep=':';
        }
    }

    filters << "Any files (*)";
    setNameFilters(filters);
    setSizeGripEnabled(false);
    if ( fbd->embedded ) {
        setLabelText(QFileDialog::Accept,fbd->okText);
        setLabelText(QFileDialog::Reject,fbd->cancelText);
    } else {
        setLabelText(QFileDialog::Accept,"Ok");
    }
    if ( fbd->existingPath!=0 ) {
        setFileMode(QFileDialog::ExistingFile);
    } else {
        setFileMode(QFileDialog::AnyFile);
    }

    if ( fbd->dx>0 ) setMinimumWidth( fbd->dx);
    if ( fbd->dy>0 ) {
        setMinimumHeight(fbd->dy);
        setMaximumHeight(fbd->dy+fbd->dy/4);
    }

//  connect(this, SIGNAL(fileSelected(QString)),   this, SLOT(fileSelected(QString)));
    connect(this, SIGNAL(currentChanged(QString)), this, SLOT(currentChanged(QString)));
#if defined(TOS_MINGW) && TOS_MINGW
    setOption(QFileDialog::DontUseNativeDialog,0);
#elif defined(TOS_LINUX) && TOS_LINUX
    setOption(QFileDialog::DontUseNativeDialog,1);
#endif
}

bool TGUI_fileBrowser::close() { return false; }

void TGUI_fileBrowser::done(int r) { QFileDialog::done(r); }

void TGUI_fileBrowser::accept()
{
    // notes:
    //   - here getSelPath is not NULL (at least "/").
    //   - getSelPath() exist or not, is a file or a directory or ....
    //   - this even with QFileDialog::ExistingFile. In fact this
    //     flag only enables/disables the open button of the FBW,
    //     an user can enter a file and terminates the FBW with <enter>.
    if ( fbd->embedded==0 ) {
        QFileDialog::accept();
    } else {
        vgui_dw_breakloop(fbd->pdialog,WID_DIALSTATUS_Ok);
    }
}

void TGUI_fileBrowser::reject()
{ 
    if ( fbd->embedded==0 ) {
        QFileDialog::reject(); 
    } else {
        vgui_dw_breakloop(fbd->pdialog,WID_DIALSTATUS_Cancel);
    }
}

#if 0 
void TGUI_fileBrowser::fileSelected(QString qstr)
{
    TstrEaten file;
    if ( qstr.isEmpty() )
        file = 0;
    else
        file = dup_QStr2CStr(qstr);
}
#endif

void TGUI_fileBrowser::currentChanged(QString qstr)
{
    TstrEaten file;
    if ( qstr.isEmpty() ) {
        fbd->guiFileIsSelectedCB(fbd, 0);
    } else {
        file = dup_QStr2CStr(qstr);
        fbd->guiFileIsSelectedCB(fbd, file);
        tool_free( file );
    }
}

TstrEaten TGUI_fileBrowser::getSelPath()
{
    TstrEaten file;
    QStringList qstrs = selectedFiles();
    if ( qstrs.isEmpty() )
        file = 0;
    else
      file = dup_QStr2CStr(qstrs.first());
    return file;
}

/*======================================================================*/
/*= Private C interfce interface to Tgui_fileBrowser class             =*/

static Tegs_widget qt_filebrowser_dbf(Twid_filebrowser* fbd )
{
    QWidget* qwidget;
    if ( fbd->embedded ) {
        TGUI_dialog* qtd = (TGUI_dialog*) fbd->pdialog->D_widEGS;
        qwidget = new TGUI_fileBrowser(qtd,fbd);
    } else {
        qwidget = new TGUI_fileBrowser(NULL,fbd);
    }
    return (Tegs_widget) qwidget;
}

static void qt_filebrowser_free (Twid_filebrowser*fbd) { }

static TstrEaten qt_filebrowser_getFilPath(Twid_filebrowser* fbd)
{
    TGUI_fileBrowser* qfb = (TGUI_fileBrowser*)fbd->guiUserdata;
    char* file = qfb ? qfb->getSelPath() : NULL;
    return file;
}

static void qt_filebrowser_setBasName(Twid_filebrowser* fbd, CstrBrief bn)
{
    TGUI_fileBrowser* qfb = (TGUI_fileBrowser*)fbd->guiUserdata;
    qfb->selectFile( bn );
}

static void qt_filebrowser_modalDelete(Twid_filebrowser*fbd) { delete (TGUI_fileBrowser*)fbd->guiUserdata; }
static int  qt_filebrowser_modalRun (Twid_filebrowser*fbd)
{
    TGUI_fileBrowser* qfb = (TGUI_fileBrowser*)fbd->guiUserdata;
    int status = qfb->exec() ;
    return status!=0 ? WID_DIALSTATUS_Ok : WID_DIALSTATUS_Cancel;
}

/*======================================================================*/
/*= Interface                                                          =*/

extern void gui_filebrowser_init(Twid_filebrowser* fbd )
{
    fbd->vt_dbf          = qt_filebrowser_dbf;
    fbd->vt_free         = qt_filebrowser_free;
    fbd->vt_getFilPath   = qt_filebrowser_getFilPath;
  //fbd->vt_getDirPath   = qt_filebrowser_getDirPath;
  //fbd->vt_getBasName   = qt_filebrowser_getBasName;
    fbd->vt_setBasName   = qt_filebrowser_setBasName;
    fbd->vt_modal_run    = qt_filebrowser_modalRun;
    fbd->vt_modal_delete = qt_filebrowser_modalDelete;
}

/*======================================================================*/
