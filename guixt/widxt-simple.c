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
 *      $Id: widxt-simple.c 136 2018-05-21 19:30:46Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-simple.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Label widget                                                       =*/ 

extern Tegs_widget gui_label_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    Widget w;
    w = create_label(gui, pw, wn, bdw, dx, dy, bg, fg, text);
    if ( dx>0 ) {
        Dimension rdx,rdy; // real size of label
        XtVaGetValues(w, XtNwidth, &rdx,
                         XtNheight,&rdy,
                         NULL);
        Dimension edx = dx;
        Dimension edy = rdy<10 ? 14 : rdy;
        XtVaSetValues(w,
                     XtNwidth, edx,
                     XtNheight,edy,
                     XtNresize,(Boolean)False,
                     NULL);
    }
    return w;
}

extern void gui_label_setTextAS  (Tegs_widget self, CstrBrief  text)
{   widget_setLabel(self,text); }
extern void gui_label_setTextSS  (Tegs_widget self, CstrBrief  text)
{  
    Dimension dx,dy;
    XtVaGetValues(self, XtNwidth,&dx, XtNheight,&dy, NULL);
    widget_setLabel(self,text);
    XtVaSetValues(self, XtNwidth,dx, XtNheight,dy, NULL);
}

extern void gui_label_setPixmap(Tegs_widget self, TpixmapEGS  pm)
{
    //XtVaSetValues(self, XtNbackgroundPixmap,(Pixmap)pm, NULL);
    XtVaSetValues(self, XtNbitmap,PXMEGS2X11(pm), NULL);
    //gui_widegs_setSize(self,100,40);
}

/*======================================================================*/
/*= Text input widgets (Single Line)                                   =*/
/*= (done in guixt/guixt-simple-slin.c)                                =*/

/*======================================================================*/
/*= Text output widgets (Single Line)                                  =*/

extern Tegs_widget gui_slout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    Widget widget;

    XT_DeclareArgs(10);

    if ( text==0 ) text="";

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bdw>=0)               XT_NextArg(XtNborderWidth, bdw);
    if (dx>0)                 XT_NextArg(XtNwidth,       dx);
    if (dy>0)                 XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNleftMargin, 4);
    XT_NextArg(XtNstring,     text);
    widget = XtCreateManagedWidget(
            wn, asciiTextWidgetClass, pw, Args, ArgCount);
    return widget;
}

extern TstrEaten gui_slout_getText (Tegs_widget self)
{
    CstrBrief str;
    str = widget_getString(self);
    return tool_strdup( str );
}
extern void gui_slout_setText(Tegs_widget self, CstrBrief str)
    { widget_setString(self,str); }
extern void gui_slout_setTextWrn(Tegs_widget self, CstrBrief str)
    { widget_setString(self,str); }
extern void gui_slout_setTextErr(Tegs_widget self, CstrBrief str)
    { widget_setString(self,str); }

/*======================================================================*/
/*= Text output widgets (Multiple Line with scroll bar)                =*/

extern Tegs_widget gui_mlout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        int sb)
{
    Widget widget;

    XT_DeclareArgs(20);

    widget_getColors(gui,wn,&bg,&fg);

    ArgCount=0;
    if (bdw>=0)               XT_NextArg(XtNborderWidth, bdw);
    if (dx>0)                 XT_NextArg(XtNwidth,       dx);
    if (dy>0)                 XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNleftMargin, 4);
    XT_NextArg(XtNstring,     "");
    XT_NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
    //XT_NextArg(XtNscrollVertical, XawtextScrollAlways);
    XT_NextArg(XtNscrollVertical, XawtextScrollWhenNeeded);
    XT_NextArg(XtNdisplayCaret, False);

    XT_NextArg(XtNeditType, XawtextAppend);
    widget = XtCreateManagedWidget(
            wn, asciiTextWidgetClass, pw, Args, ArgCount);
    XawTextEnableRedisplay (widget);
    return widget;
}

extern void gui_mlout_setText(Twid_mlout* self, CstrBrief str)
    { widget_setString(self->widEGS,str); }
extern void gui_mlout_setTextWrn(Twid_mlout* self, CstrBrief str)
    { widget_setString(self->widEGS,str); }
extern void gui_mlout_setTextErr(Twid_mlout* self, CstrBrief str)
    { widget_setString(self->widEGS,str); }

static void gui_mlout_appTextWC(Twid_mlout* self, CstrBrief str,
                                TcolorEGS* c)
{
    int i;
    if ( str==0 || strlen(str)==0 ) return;

    /* set color */
    // it does not seem possible ? ( FIXME )
    
    /* get the text actually that self handles and its lines number. */
    Cstr text;
    int  lnText=0;    // line number
    int  leText=0;    // last line is ended (last char of text is LF)
    int  pos=0;       // strlen(text)
    XtVaGetValues(self->widEGS, XtNstring,&text, NULL);
    if ( ! (text==0 || *text==0) )  {
      lnText = 1;
      for (i=0 ; text[i] ; i++ ) if ( text[i]=='\n' ) lnText+=1;
      if ( text[i-1]=='\n' ) { lnText -= 1; leText=1; }
      pos=i;
    }
    
    // get the number of lines of the self widget.
    Widget sink;
    Dimension dx,dy;
    XtVaGetValues(self->widEGS, XtNwidth,&dx, XtNheight,&dy, NULL);
    XtVaGetValues(self->widEGS, XtNtextSink,&sink, NULL);
    int lnMax   = XawTextSinkMaxLines( sink, dy);
    
    /* the text to append */
    int lnStr=0,leStr=0;
    XawTextBlock textblock;
    textblock.firstPos = 0;
    textblock.length = strlen (str);
    textblock.ptr = (char*)str;
    textblock.format = XawFmt8Bit;
    if ( ! (str==0 || *str==0) )  {
      lnStr = 1;
      for (i=0 ; str[i] ; i++ ) if ( str[i]=='\n' ) lnStr+=1;
      if ( str[i-1]=='\n' ) { lnStr -= 1; leStr=1; }
    }
    
    /* the number of lines of text + str */
    int ln = lnText +  (leText==1 ? lnStr : lnStr-1);
    
    DBG1_MloutM(
        "ln/leText=%d/%d ln/leStr=%d/%d ln=%d lnMax=%d dy=%d pos=%d\n",
         lnText,leText,   lnStr,leStr,  ln,   lnMax,   dy,   pos);
    
    //  XawTextPosition p = XawTextGetInsertionPoint (self);
    
    XawTextReplace (self->widEGS, pos, pos, &textblock);
    if ( ln>lnMax ) {
      XawTextPosition p = XawTextGetInsertionPoint (self->widEGS);
      XawTextSetInsertionPoint (self->widEGS,pos+textblock.length);
    } else {
      XawTextSetInsertionPoint (self->widEGS,0);
    }
}

extern void gui_mlout_appText(Twid_mlout* self, CstrBrief str)
    { gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextWrn(Twid_mlout* self, CstrBrief str)
    { gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextErr(Twid_mlout* self, CstrBrief str)
    { gui_mlout_appTextWC(self,str,0); }

/*======================================================================*/
/*= Spinner widget.                                                    =*/
/*= (done in guixt/guixt-simple-spinner.c)                             =*/

/*======================================================================*/
/*= Slider widget.                                                     =*/

typedef double Float;

static void gui_slider_cbThumbed (Widget w, XtPointer ud, XtPointer data);
static void gui_slider_cbScrolled(Widget w, XtPointer ud, XtPointer data);

extern Tegs_widget  gui_slider_create(Twid_slider*self,
                                      Tegs_widget pw, CstrBrief wn,
                                      int bdt, int dx, int dy)
{
    Widget widget;
    TcolorEGS bg,fg;

    XT_DeclareArgs(10);

    vgui_wn_getColorEGS(self->gui,wn,&fg,&bg);

    ArgCount=0;
    if (bdt>=0)               XT_NextArg(XtNborderWidth, bdt);
    if (dx>0)                 XT_NextArg(XtNwidth,       dx);
    if (dy>0)                 XT_NextArg(XtNheight,      dy);
    if (bg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNbackground,  bg.ulong);
    if (fg.ulong!=EGSCOLOR_NONE)    XT_NextArg(XtNforeground,  fg.ulong);
    XT_NextArg(XtNthumb, None);
    widget = XtCreateManagedWidget(
            "slider", scrollbarWidgetClass, pw, Args, ArgCount);
    //gui_slider_set(self,self->val);
    XtAddCallback(widget, XtNjumpProc,
                  gui_slider_cbThumbed,  (XtPointer)self);
    XtAddCallback(widget, XtNscrollProc,
                  gui_slider_cbScrolled, (XtPointer)self);

    return widget;
}

extern double       gui_slider_get (Twid_slider*self)
{
    Float val;
    XtVaGetValues(self->widEGS, XtNshown,&val, NULL);
    DBG0_SliderM("slider=%p ; val=%f egsval=%f\n",self->val, val);
    return val;
}

extern void         gui_slider_set (Twid_slider*self, double v)
{
    Float val = v;
    DBG0_SliderM("slider=%p ; val=%f egsval=%f\n",self->val, val);
    XawScrollbarSetThumb(self->widEGS,v,self->inc);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

static void gui_slider_cbThumbed(Widget w,
        XtPointer ud, XtPointer xtdata)
{
    Twid_slider* self  = ud;
    double       ratio = *(float*)xtdata;
    DBG0_SliderE("slider=%p val=%f : ratio=%f",self,self->val,ratio);
    self->val = ratio;
    if ( self->chg )
        self->chg(self,self->ud,self->val);
    DBG1_SliderL("slider=%p val=%f : ratio=%f",self,self->val,ratio);
}

static void gui_slider_cbScrolled(Widget w, XtPointer ud, XtPointer xtdata)
{
    Twid_slider* self  = ud;
    int       advance = (intptr_t)xtdata;
    DBG0_SliderE("slider=%p val=%f : adv=%d",self,self->val,advance);
    self->val += advance<-20 ? -0.1 : advance<0 ? -self->inc :
                 advance> 20 ? +0.1 :             +self->inc;
    if ( self->val < 0.0 )
        self->val = 0;
    else if ( self->val > 1.0 )
        self->val = 1.0;
    gui_slider_set(self,self->val);
    if ( self->chg )
        self->chg(self,self->ud,self->val);

    DBG1_SliderL("slider=%p val=%f : adv=%d",self,self->val,advance);
}

/*======================================================================*/
