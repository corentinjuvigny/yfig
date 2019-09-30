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
 *      $Id: widxt-simple-slin.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-simple-slin.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*= Implemenent single line input widget.                              =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/wid-simple.h"
#include "tools/charset.h"

/*======================================================================*/

static void gui_slin_cb_changed(Widget w, XtPointer ud, XtPointer closure);
static void gui_slin_cb_validated(void* ud);

/*======================================================================*/
/* Interface                                                           =*/

extern Tegs_widget gui_slin_dbf(Twid_slin*self,
                    Tegs_widget pw, CstrBrief wn,
                    int bd, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
                    CstrBrief text)
{
    Tvgui* gui = self->gui;
    Widget widget;

    XT_DeclareArgs(15);


    if ( bg.ulong==EGSCOLOR_DEFAULT || fg.ulong==EGSCOLOR_DEFAULT ) {
        Tegs_color fgd,bgd;
        vgui_wn_getColorEGS(gui,wn, &fgd, &bgd);
        if ( fg.ulong==EGSCOLOR_DEFAULT ) fg=fgd;
        if ( bg.ulong==EGSCOLOR_DEFAULT ) bg=bgd;
    }

    ArgCount=0;
    if (bd>=0)                XT_NextArg(XtNborderWidth, bd);
    if (dx>0)                 XT_NextArg(XtNwidth,       dx);
    if (dy>0)                 XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE)  XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE)  XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNleftMargin, 4);
    XT_NextArg(XtNstring,     text!=0 ? text : "");
    XT_NextArg(XtNeditType,   XawtextEdit);
    //XT_NextArg(XtNinsertPosition, text==0 ? 0 : strlen(text) );
    widget = XtCreateManagedWidget(
            wn, asciiTextWidgetClass, pw, Args, ArgCount);
    //widget_place_widget(widget, 0,0,0,0, 0,0, 0,0);

    Widget source;
    if ( self->chg!=0 ) {
        // gui_tin_cb works for both "changed value" and "validated value" event
        XtVaGetValues(widget,XtNtextSource, &source,NULL);
        XtAddCallback(source, XtNcallback, gui_slin_cb_changed, (XtPointer) self);
    } else if ( self->val!=0 ) {
        XtVaGetValues(widget,XtNtextSource, &source,NULL);
        widget_cb_addU(widget, self,
                YFIG_EVENT_KeyReturnPressed, gui_slin_cb_validated,
                NULL);
    }
#if 0
XtSetKeyboardFocus(pw,widget);
XGrabPointer(E_Display,_XtWindow(widget) // Window grab_window 
                      ,False             // Bool owner_events
                      ,0                 // int event_mask
                      ,0                 // int pointer_mode
                      ,GrabModeAsync     // int keyboard_mode
                      ,None              // Window confine_to,
                      ,None              // Cursor cursor
                      ,CurrentTime);
#endif
    return widget;
}

extern TstrEaten  gui_slin_getText   (Twid_slin* self)
{
    CstrBrief str;
    XtVaGetValues(self->widEGS, XtNstring, &str, NULL);
    return cs_conv_La15toUtf8(str);
}

extern void       gui_slin_setText(Twid_slin* self, CstrBrief str)
{
    if ( str!=0 ) {
        char* la16 = cs_conv_Utf8toLa15(str);
        XtVaSetValues(self->widEGS, XtNstring, la16, NULL);
        free( la16 );
    } else {
        XtVaSetValues(self->widEGS, XtNstring, str, NULL);
    }
}
static void       gui_slin_setTextLa16(Twid_slin* self, CstrBrief str)
{ XtVaSetValues(self->widEGS, XtNstring, str, NULL); }

extern void        gui_slin_disable(Twid_slin* self)
{
    XawTextEditType mode = XawtextRead;
    XtVaSetValues(self->widEGS, XtNeditType,mode, NULL);
}

extern void        gui_slin_enable (Twid_slin* self)
{
    XawTextEditType mode = XawtextEdit;
    XtVaSetValues(self->widEGS, XtNeditType,mode, NULL);
}

/*======================================================================*/
/*= callback                                                           =*/

static int  gui_slin_getPosStr(const char**str, Cwid_slin*self);
static void gui_slin_getPrevStr(char*buf, CstrBrief str,int pos);

#define STRLEN_MAX 127

static void gui_slin_cb_changed(Widget w, XtPointer ud, XtPointer closure)
{
    char buf[STRLEN_MAX+1];
    Twid_slin* self = ud;
    DBG1_MSlinE("slin=%p ud=%p",self,self->ud);

    const char* str=0;
    int pos = gui_slin_getPosStr(&str,self);
    DBG1_MSlinM("slin=%p ud=%p : pos=%d c=0x%04x str=%s",self,self->ud,
            pos, str!=0 && pos>=0 && pos<strlen(str) ? str[pos] : -1, 
            str==0 ? "NULL" : str);
    if ( str!=0 && 0<=pos && pos<=STRLEN_MAX && pos<strlen(str) ) {
        DBG1_MSlinM("slin=%p ud=%p : accept char",self,self->ud);
        char c = str[pos];
        if ( c=='\n' || c=='\r' ) {
            gui_slin_getPrevStr(buf,str,pos);
            DBG1_MSlinM("slin=%p ud=%p : validation char : %s",self,self->ud,buf);
            gui_slin_setTextLa16(self,buf);
XtVaSetValues(self->widEGS, XtNinsertPosition,(XawTextPosition)pos, NULL );
            if ( self->val!=0 ) self->val(self,self->ud);
        } else {
            if ( self->chg!=0 ) self->chg(self,self->ud);
        }
    } else if ( str!=0 ) {
        DBG1_MSlinM("slin=%p ud=%p : remove char",self,self->ud);
        strncpy(buf,str,STRLEN_MAX); buf[STRLEN_MAX]=0;
        gui_slin_setTextLa16(self,buf);
    }
    DBG1_MSlinL("slin=%p ud=%p",self,self->ud);
}

static void gui_slin_cb_validated(void* ud)
{
    Twid_slin* self = ud;
    DBG1_MSlinE("slin=%p ud=%p",self,self->ud);
    if ( self->val!=0 )  self->val(self,self->ud);
    DBG1_MSlinL("slin=%p ud=%p",self,self->ud);
}

/*======================================================================*/

static int gui_slin_getPosStr(const char**str, Cwid_slin*self)
{
    XawTextPosition pos;
    XtVaGetValues(self->widEGS, XtNinsertPosition,&pos, XtNstring,str, NULL);
    return pos;
}

static void gui_slin_getPrevStr(char*buf, CstrBrief str,int pos)
{
    memcpy(buf,str,pos);
    strcpy(buf+pos,str+pos+1);
}

/*======================================================================*/
