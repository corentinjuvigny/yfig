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
 *      $Id: guiqt-dw-layout.cc 135 2018-05-19 16:49:57Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-dw-layout.cc $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for building dialog popup widgets (EGS drivers)          =*/
/*=                                                                    =*/
/*= Implementation of dialog layouts.                                  =*/
/*=                                                                    =*/
/*= See the vgui/vgui-dw.h header for documentation.                   =*/
/*======================================================================*/

#include "guiqt-dw.h"
#include "guiqt/guiqt-create.h"

/*======================================================================*/
//#define SEP_BP_V 10

#define SEP_H     VDW_SEP_H
#define SEP_V     VDW_SEP_V

/*======================================================================*/

extern void gui_dw_finishMil(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,cc;
    int l=0,c=0;
    int sep_h=SEP_H;
    int sep_v=SEP_V;
    QGridLayout*    grid = VGUI_D_isTopDial(self)
                         ? ((TGUI_dialog*)self->D_widEGS)->grid
                         : ((TGUI_subdialog*)self->D_widEGS)->grid;

    /* place the title */
    if ( title )
        widget_place_EF(grid,(QWidget*)title,l++,0,1,nc);


    /* place the widget */
    for ( ll=0 ; ll<nl ; ll+=1 ) {
        QHBoxLayout* box = new QHBoxLayout();
        grid->addLayout(box,l++,0);
        for ( cc=0; cc<nc ; cc++ ) {
            if ( mtx[ll][cc].widEGS==0 ) continue;
            QWidget* qw = (QWidget*)mtx[ll][cc].widEGS;
            if (  mtx[ll][cc].grow==0 ) {
                qw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            } else {
                qw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }
            box->addSpacing(sep_v+mtx[ll][cc].sepHV);
            box->addWidget(qw);
        }
        QLabel* fill = new QLabel();
        fill->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        box->addWidget(fill);
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        Cvgui_dwTop* top = self->D_top;
        if ( top==0 )
            widget_place_FF(grid,(QWidget*)buttons,l,0,1,1);
        else switch ( (top->dt_btnLayout>1)&3 ) {
            case 1: // east
              widget_place_FF(grid,(QWidget*)buttons,0,1,l,1);
              break;
            default:
              widget_place_FF(grid,(QWidget*)buttons,l,0,1,1);
              break;
        }
    }
}

/*======================================================================*/

extern void gui_dw_finishMic(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,aboveSep;
    int l=0,c=0;
    int sep_h=self->D_hmargin;
    int sep_v=self->D_vspace;
    QGridLayout*    grid = VGUI_D_isTopDial(self)
                         ? ((TGUI_dialog*)self->D_widEGS)->grid
                         : ((TGUI_subdialog*)self->D_widEGS)->grid;

    /* place the title */
    if ( title )
        widget_place_EF(grid,(QWidget*)title,l++,0,1,nc);


    /* place the widget */
    for ( c=0 ; c<nc ; c+=1 ) {
        QVBoxLayout* box = new QVBoxLayout();
        grid->addLayout(box,l,c);
        box->setContentsMargins(0,0,0,0);
        box->setSpacing(0);
        //box->setSizeConstraint(QLayout::SetFixedSize);
        int addFiller = 1;
        for ( ll=0; ll<nl ; ll++ ) {
            if ( mtx[ll][c].widEGS==0 ) continue;
            QWidget* qw = (QWidget*)mtx[ll][c].widEGS;
#if 0
            if ( (mtx[ll][c].flag&VDM_V2E_BCenter)!=0 ) {
                qw->setAlignment(Qt::AlignHCenter);
            }
#endif
            if (  (mtx[ll][c].flag&VDM_V2E_BGrow)==0 ) {
                qw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            } else if (  (mtx[ll][c].flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                qw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                addFiller = 0;
            } else if (  (mtx[ll][c].flag&VDM_V2E_HGrow)!=0 ) {
                qw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            } else if (  (mtx[ll][c].flag&VDM_V2E_VGrow)!=0 ) {
                qw->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
                addFiller = 0;
            }
            box->addSpacing(sep_v+mtx[ll][c].sepHV);
            box->addWidget(qw);
        }
#if 1
        if ( addFiller ) {
            QLabel* fill = new QLabel();
                fill->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            box->addWidget(fill);
        }
#endif
    }
    l += 1;

    /* place the buttons */
    if ( buttons!=0 ) {
        widget_place_FF(grid,(QWidget*)buttons,l,0,1,nc);
    }
}

/*======================================================================*/

extern void gui_dw_finishMfa(Tvgui_dw*self,
                             Tegs_widget title, Tegs_widget buttons,
                             Tvgui_dwMTX mtx, int nl, int nc)
{
    int ll,aboveSep;
    int l=0,c=0;
    int sep_h=SEP_H;
    int sep_v=SEP_V;
    QGridLayout*    grid = VGUI_D_isTopDial(self)
                         ? ((TGUI_dialog*)self->D_widEGS)->grid
                         : ((TGUI_subdialog*)self->D_widEGS)->grid;

    /* place the title */
    if ( title ) {
        widget_place_EF(grid,(QWidget*)title,l,0,1,nc);
        l += 1;
    }

    /* place the widget */
    for ( ll=0; ll<nl ; ll++,l+=1 ) {
        for ( c=0 ; c<nc ; c+=1 ) {
            Tvgui_dwMtxCell* p = &mtx[ll][c];
            if ( p->widEGS==0 ) continue;
            if ( p->xs==0 || p->ys==0 ) continue;
            if ( (p->flag&VDM_V2E_BGrow)==0 ) {
                widget_place_FF(grid,(QWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_BGrow)==VDM_V2E_BGrow ) {
                widget_place_EE(grid,(QWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_HGrow)!=0 ) {
                widget_place_EF(grid,(QWidget*)p->widEGS,l,c,p->ys,p->xs);
            } else if ( (p->flag&VDM_V2E_VGrow)!=0 ) {
                widget_place_FE(grid,(QWidget*)p->widEGS,l,c,p->ys,p->xs);
            }
        }
    }

    /* place the buttons */
    if ( buttons!=0 ) {
        widget_place_FF(grid,(QWidget*)buttons,l,0,1,nc);
    }
}

/*======================================================================*/

