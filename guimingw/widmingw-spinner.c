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
 *$Software: YFIG-WIN32 (Interface to WIN32 EGS under MinGw)
 *      $Id: widmingw-spinner.c 338 2018-11-05 23:06:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guimingw/widmingw-spinner.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Spinner widgets (EGS driver).                                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guimingw.h"
#include "guimingw/guimingw-widget.h"

#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Registration                                                       =*/

MINGW_WidProc_Declaration(spinner);

extern ATOM mingw_spinner_register(Tvgui* gui)
{
#if 0 // FIXME:MOVE to gui_open
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    icex.dwICC = ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&icex);
    icex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);
#endif
    COLORREF clr = RGB(50,50,220);
    
    WNDCLASSEX wcex; memset(&wcex,0,sizeof(wcex));
    wcex.cbSize        = sizeof(wcex);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = MINGW_WidProc_Symbol(spinner);
  //wcex.cbClsExtra    = 0;
  //wcex.cbWndExtra    = 0;
    wcex.hIcon         = LoadIcon  (NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(clr) ; //(HBRUSH) GetStockObject (GRAY_BRUSH);
    wcex.lpszClassName = MINGW_WC_Spinner;
    wcex.hInstance     = E_hInstance;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    return RegisterClassEx(&wcex);
}

/*======================================================================*/
/*= Widget Procedure                                                   =*/

typedef struct _Tgui_spinner {
   Tvgui*        gui;
   HWND          topw;   // top widget, it contains txtw and updw
   HWND          txtw;   // text widget, where number is written
   HWND          updw;   // widget that allows to in/decrement the number
                         // by clicking (a top-down arrow).
   Twid_spinner* spin;   // the vgui spinner data.
} Tgui_spinner;

MINGW_WidProc_Declaration(spinner)
{
//  TspinIntData* usrdt = (TspinIntData*)GetWindowLongPtr(hwnd,GWL_USERDATA);
    Tgui_spinner* self  = (Tgui_spinner*)GetWindowLongPtr(hwnd,GWL_USERDATA);
    Tvgui*        gui   = self ? self->gui    : NULL;
    Twid_spinner* spin  = self ? self->spin   : NULL;
    Trange*       range = spin ? &spin->range : NULL;
//if (usrdt!=0) if ( message!=0x84 ) if ( message!=0x200 ) if ( message!=0x20 ) printf("X %p %p %04x : %d=%d: %p %p %p %.2f\n",hwnd,usrdt,message,sizeof(HWND),sizeof(void*),usrdt->topw,usrdt->tew,usrdt->udw,usrdt->val);
    int te_x=5,te_y=5,ud_dx=25;
    int x,y,dx,dy;
    char text[1024];
    RECT re;

    switch(message) {
        case WM_CREATE:
            if (self==0) {
                self  = ((CREATESTRUCT*)lParam)->lpCreateParams;
                gui   = self->gui;
                spin  = self->spin;
                range = &spin->range;
            }
            self->topw=hwnd;
            SetWindowLongPtr(self->topw,GWL_USERDATA,(long)self);
            GetWindowRect(self->topw,&re);
            int top_dx = re.right  - re.left;
            int top_dy = re.bottom - re.top;
            INT nHeight = HIWORD(lParam);
            x=te_x; y=te_y; dx=top_dx-te_x-3-ud_dx; dy=top_dy-5-5;
            self->txtw= CreateWindowEx(
                    WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY,
                    WC_EDIT,
                    range_dble(text,range),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
                    x, y, dx,dy,
                    hwnd, (HMENU)1000,E_hInstance ,NULL);
            SetWindowLongPtr(self->txtw,GWL_USERDATA,(long)self);
          //SendMessage(te,EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,MAKELPARAM(3,20));
            x += dx+5; dx=ud_dx;
            self->updw= CreateWindowEx(
                    0, // WS_EX_LEFT | WS_EX_LTRREADING,
                    UPDOWN_CLASS, //"EDIT",
                    NULL, 
                    WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS  | WS_EX_NOPARENTNOTIFY, 
                    x, y, dx, dy,
                    hwnd, (HMENU)1001 , E_hInstance, NULL);
            SetWindowLongPtr(self->updw,GWL_USERDATA,(long)self);
            SendMessage(self->updw,UDM_SETRANGE32,-10,+10);
            SendMessage(self->updw,UDM_SETPOS32,0,0);
//printf(DBGVM_prtUfmt(CREATE) ":2: box=%p te=%p ud=%p\n",DBGVM_prtUarg(CREATE),self->topw,self->txtw,self->updw);
            return 0;

        case WM_VSCROLL : {
//            DBGVM_prtU(VSCROLL);
            int v = SendMessage(self->updw,UDM_GETPOS32,0,0);
            if ( v<0 ) {
                SendMessage(self->updw,UDM_SETPOS32,0,0);
                // FIXME dec(usrdt);
                SetWindowText(self->txtw,range_dble(text,range));
            } if ( v>0 ) {
                SendMessage(self->updw,UDM_SETPOS32,0,0);
                // FIXME inc(usrdt);
                SetWindowText(self->txtw,range_dble(text,range));
            }
            return 1;
            }

        case WM_CTLCOLOREDIT : {
//            DBGVM_prtU(CTLCOLOREDIT);
            HDC hdc   = (HDC)wParam;
            if ( (HWND)lParam==self->txtw ) {
                COLORREF clr = RGB(220,50,50);
                SetBkColor(hdc, clr); // Set to red
                SetDCBrushColor(hdc, clr);
                return (LRESULT) GetStockObject(DC_BRUSH);
            } else {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
            break;
            }
        case WM_SIZE:
//            DBGVM_prtU(SIZE);
            if ( hwnd==self->topw ) {
#if 0
                RECT rect;
                INT nWidth  = LOWORD(lParam);
                INT nHeight = HIWORD(lParam);
                GetWindowRect(usrdt->topw,&rect);
                printf("nWidth/nHeight=%d/%d box=%d/%d\n",nWidth,nHeight,rect.right-rect.left,rect.bottom-rect.top);
                GetWindowRect(usrdt->tew,&rect);
                printf("nWidth/nHeight=%d/%d te =%d/%d\n",nWidth,nHeight,rect.right-rect.left,rect.bottom-rect.top);
                GetWindowRect(usrdt->udw,&rect);
                printf("nWidth/nHeight=%d/%d ud =%d/%d\n",nWidth,nHeight,rect.right-rect.left,rect.bottom-rect.top);
                GetWindowText(usrdt->tew,text,1024);
                SendMessage(usrdt->tew, EM_GETRECT, 0, (LPARAM)&rect);
                printf("nWidth/nHeight=%d/%d te =%d/%d <<%s>>\n",nWidth,nHeight,rect.right-rect.left,rect.bottom-rect.top,text);
                //MoveWindow(hEditBox, 8, 4, nWidth - 70, 20, TRUE);
                //MoveWindow(hEnterButton, nWidth - 57, 4, 50, 20, TRUE);
#endif
                return 0;
            }
            break;

        case WM_COMMAND:
//            DBGVM_prtU(COMMAND);
            if ( (HWND)lParam==self->txtw) {
                if ( HIWORD(wParam)==EN_UPDATE ) {
                    GetWindowText(self->txtw,text,1024);
// FIXME                    int v = atoi(text);
// FIXME                    usrdt->val = v;
                } else if ( HIWORD(wParam)==EN_CHANGE)
                    return DefWindowProc(hwnd, message, wParam, lParam);
            }
            return 0;
        default:
            return dbgvm_all(__func__,hwnd,message,wParam,lParam);
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

#if 0
/*======================================================================*/

static void gui_spinner_CBup(void* ud)
{
    Twid_spinner* spin  = ud;
    Trange*       range = &spin->range;
    DBG0_SPINE("spin=%p v=%f",spin,range->val);
    if ( spin->enabled==0 ) {
        DBG1_SPINL("spin=%p v=%f : NOT enabled",spin,range->val);
        return;
    }

    double ov = range->val; // old value
    range_inc(range);
    double nv = range->val; // new value
    if ( ov!=nv ) {
        gui_spinner_setNum(spin,nv);
        if ( spin->cbChg!=0 ) spin->cbChg(spin,spin->cbUd,nv);
    }
    DBG1_SPINL("spin=%p v=%f --> %f: CHANGED",spin,ov,nv);
}

static void gui_spinner_CBdown(void* ud)
{
    Twid_spinner* spin   = ud;
    Trange*       range  = &spin->range;
    DBG0_SPINE("spin=%p v=%f",spin,range->val);
    if ( spin->enabled==0 ) {
        DBG1_SPINL("spin=%p v=%f : NOT enabled",spin,range->val);
        return;
    }

    double ov = range->val; // old value
    range_dec(range);
    double nv = range->val; // new value
    if ( ov!=nv ) {
        gui_spinner_setNum(spin,nv);
        if ( spin->cbChg!=0 ) spin->cbChg(spin,spin->cbUd,nv);
    }
    DBG1_SPINL("spin=%p v=%f --> %f: CHANGED",spin,ov,nv);
}

static void gui_spinner_CBchg(Twid_slin* slin, void* ud)
{
    Twid_spinner* spin   = ud;
    Trange*       range  = &spin->range;
    double        oldval = range->val;
    DBG0_SPINE("spin=%p slin=%p oldval=%f",spin,slin,oldval);

    CstrBrief  str = gui_slin_getText(slin);
    if ( strlen(str)>100 ) 
        goto bad_format;

    char   buf[100];
    double newval;
    int    ret;
    if ( range->nAdt==0 ) {
        int tmp=0;
        ret = sscanf(str,"%d%s",&tmp,buf);
        newval = tmp;
    } else {
        ret = sscanf(str,"%lf%s",&newval,buf);
    }
    if ( ret!=1 ) 
        goto bad_format;

    // str is a valid numeric value (int or real).
    // newval contains the conversion of str.
    // now we check if it is in the allowed value range.
    // newval2 contains the right value (either newval or min or max).
    double newval2;
    range_setVal(range,newval);
    newval2 = range->val;
    if ( newval2!=newval ) gui_spinner_setNum(spin,newval2);

    // update the spinner value and call the callback if needed
    if ( oldval!=newval2 ) {
        //spin->val = newval2;
        DBG1_SPINM("spin=%p slin=%p oldval=%f str=%s newval=%f",
                spin,slin,oldval,str,newval2);
        if ( spin->cbChg!=0 ) spin->cbChg(spin,spin->cbUd,newval2);
    }
    DBG1_SPINL("spin=%p slin=%p oldval=%f newval=%f",spin,slin,oldval,newval2);
    return;

bad_format:
     gui_spinner_setNum(spin,newval2);
     DBG1_SPINL("spin=%p slin=%p oldval=%f value NOT changed : bad str \"%s\"",
                spin,slin,oldval,str);
    return;
}

/*======================================================================*/
/*= Spinner widget (EGS Interface)                                     =*/

extern void gui_spinner_dbf(
                   Twid_spinner*spin, Tvgui*gui, Tegs_widget parent)
{
    Trange* range = &spin->range;
    spin->enabled  = 1;

    Widget topForm, pixForm, pixTop, pixBot;
    Twid_slin* slin;
 
    /* topForm */
    topForm = create_form(gui,parent,wn_dialSpin,
            0,1,0,0,
            EGSCOLOR_default,EGSCOLOR_default);
    spin->self = topForm;

    /* slin */
    int dx = range->nAll*9;
    slin = create_slin_place(gui,topForm,wn_dialSlin,
            dx,-1, 0, gui_spinner_CBchg,0,spin, 0,0);
    spin->wVal = slin;
    XtVaSetValues( slin->widEGS,
            XtNinternalWidth,  (Dimension)0,
            XtNinternalHeight, (Dimension)0,
            NULL);

    /* pixForm with pixTop and pixBbot buttons */
    pixForm = create_form(gui,topForm,wn_dialSub,
            0,1,0,0,
            EGSCOLOR_none,EGSCOLOR_none);
    spin->wSpi = pixForm;
    XtVaSetValues( pixForm,
            XtNdefaultDistance, (int)0,
            XtNinternalWidth,  (Dimension)0,
            XtNinternalHeight, (Dimension)0,
            NULL);
    widget_place_FixedSize(pixForm,0,slin->widEGS,0,0);

    pixTop = create_button(gui,pixForm, wn_dialSpin,
            0,11,7, EGSCOLOR_default, EGSCOLOR_default,
            0, 0, gui_spinner_CBup, spin, 0);
    pixBot = create_button(gui,pixForm, wn_dialSpin,
            0,11,7, EGSCOLOR_default, EGSCOLOR_default,
            0, 0, gui_spinner_CBdown, spin, 0);
    XtVaSetValues( pixTop,
            XtNbitmap, gui->g_main->m_pmSpinup, 
            XtNinternalWidth,  (Dimension)1,
            XtNinternalHeight, (Dimension)1,
            NULL);
    XtVaSetValues( pixBot,
            XtNbitmap, gui->g_main->m_pmSpindown,
            XtNinternalWidth,  (Dimension)1,
            XtNinternalHeight, (Dimension)1,
            NULL);
    widget_place_widget( pixTop,WPW_FixedSize,0,0,0,0);
    widget_place_widget( pixBot,WPW_FixedSize,0,0,0,pixTop);
}

extern void          gui_spinner_free   (Twid_spinner* spin)
{
    wid_slin_free( spin->wVal );
}

extern void gui_spinner_update (Twid_spinner* spin) { }
extern void gui_spinner_setText(Twid_spinner*spin, CstrBrief str)
{ widget_setString(spin->wVal->widEGS,str); }

extern void gui_spinner_setNum (Twid_spinner*spin, double nv)
{
    char buf[100];
    Trange* range = &spin->range;
    DBG0_SPINE("spin=%p nv=%f",spin,nv);
    range_setVal(range, nv);
    nv = range->val;
    range_dble(buf,range);
    gui_spinner_setText(spin,buf);
    DBG1_SPINL("spin=%p nv=%f : set to %s",spin,nv,buf);
}

extern void          gui_spinner_clear  (Twid_spinner* spin)
{ wid_slin_setText(spin->wVal,""); }

extern void          gui_spinner_enable (Twid_spinner* spin)
{
    Trange* range = &spin->range;
    DBG0_SPINE("spin=%p v=%f",spin,range->val);
    wid_slin_enable(spin->wVal);
    DBG1_SPINL("spin=%p v=%f",spin,range->val);
}

extern void          gui_spinner_disable(Twid_spinner* spin)
{
    Trange* range = &spin->range;
    DBG0_SPINE("spin=%p v=%f",spin,range->val);
    wid_slin_disable(spin->wVal);
    DBG1_SPINL("spin=%p v=%f",spin,range->val);
}

/*======================================================================*/
#endif
