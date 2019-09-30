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
 *      $Id: widqt-filebrowser.h 333 2018-10-30 21:40:51Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/widqt-filebrowser.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= File browser widget (EGS driver).                                  =*/
/*=                                                                    =*/
/*= See the vgui/wid-filebrowser.h header for documentation.           =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_WIDQT_FILEBROWSER_H
#define FILE_WIDQT_FILEBROWSER_H
/*======================================================================*/

#include "guiqt.h"
#include "vgui/wid-filebrowser.h"

#ifdef QT_VERS_5
#include <QFileDialog>
#endif

#include "guiqt-dw.h"

/*======================================================================*/

class TGUI_fileBrowser : public QFileDialog {
    Q_OBJECT
public:
    TGUI_fileBrowser(TGUI_dialog*_d, Twid_filebrowser* _fbd);
    //virtual ~TGUI_fileBrowser();
public:
    virtual bool close();
    virtual void done(int r);
    virtual void accept();
    virtual void reject();
public slots:
//  void fileSelected(QString qstr);
    void currentChanged(QString qstr);
public:
    // retrieve the selected path from the QFileDialog
    TstrEaten getSelPath();
    Twid_filebrowser* fbd;
};

/*======================================================================*/
#endif //FILE_WIDQT_FILEBROWSER_H
/*======================================================================*/


