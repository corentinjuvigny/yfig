/*======================================================================
 *  Copyright 2018 Timothee Denoux
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
 *$Software: YFIG-GTK (VGUI driver to GTK)
 *      $Id: guiqt-dw.cc 97 2017-11-11 17:20:19Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guiqt/guiqt-dw.cc $
 *  $Author: Timothee Denoux (Email: timothee.denoux@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Base widgets (EGS driver).                                         =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guigtk.h"

#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Label widget                                                        */
//TODO Ajouter des tests de type comme en Qt ?
static GtkWidget *widget_label_create(
        Tvgui* gui, Tegs_widget parent, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    if ( text==0 ) text="";
    GtkWidget* ret = gtk_label_new(text);
    //TODO Ajouter une frame ??
    /*if (bdw>=0)     { ret->setLineWidth(bdw);
                      ret->setFrameStyle(QFrame::Box|QFrame::Plain); }
*/
    if (dx>0) {
        if (dy>0)       gtk_widget_set_size_request(ret,dx,dy);
        else gtk_widget_set_size_request(ret,dx,-1);
    }
    widget_setColors(gui, ret, wn, bg,fg);
    return ret;
}


extern Tegs_widget gui_label_nbf( 
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{ return widget_label_create(gui,pw,wn,bdw,dx,dy,bg,fg,text); }

extern void gui_label_setTextAS(Tegs_widget self,  CstrBrief text)
{ gtk_label_set_text(GTK_LABEL(self),text); }

extern void gui_label_setTextSS(Tegs_widget self,  CstrBrief text)
{ gtk_label_set_text(GTK_LABEL(self),text); }

extern void gui_label_setPixmap(Tegs_widget self, TpixmapEGS pm)
{
    //TODO pixmap sur un label ??
    PERM_error_NYI();
}

/*======================================================================*/
/*= Text input widgets (Single Line)                                   =*/

static void entry_cb(GtkEntry* entry, gpointer user_data)
{
    Twid_slin* self = g_object_get_data(G_OBJECT(entry),"slin");
    if ( self->val!=0 )  self->val(self,self->ud);
}

static void textChanged_cb(GtkEditable* entry, gchar* preedit, gpointer user_data)
{
    Twid_slin* self = g_object_get_data(G_OBJECT(entry),"slin");
    if ( self->chg != 0) self->chg(self,self->ud);
}

static void returnPressed_cb(GtkEntry* entry, gpointer user_data)
{
    Twid_slin* self = (Twid_slin*)user_data;
    if ( self->val ) self->val(self,self->ud);
}

extern Tegs_widget gui_slin_dbf(Twid_slin*self,
                    Tegs_widget pw, CstrBrief wn,
                    int bd, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
                    CstrBrief text)
{
    GtkWidget* widget = gtk_entry_new();
    g_object_set_data(G_OBJECT(widget),"slin",self);
    if (dx>0) {
        if (dy>0)
            gtk_widget_set_size_request (widget,dx,dy);
        else
            gtk_widget_set_size_request (widget,dx,-1);
    } else {
        if (dy>0)
            gtk_widget_set_size_request (widget,-1,dy);
        else
            gtk_widget_set_size_request (widget,-1,-1);
    }
    widget_setColors(self->gui, widget, wn, bg,fg);
    if ( text!=0 )
        gtk_entry_set_text(GTK_ENTRY(widget),text);
    g_signal_connect(widget, "activate", G_CALLBACK(entry_cb),self); 
    g_signal_connect(GTK_EDITABLE(widget), "changed", G_CALLBACK(textChanged_cb),pw); 
    return widget;
}

extern TstrEaten  gui_slin_getText   (Twid_slin* self)
{
    GtkWidget* widget = (GtkWidget*)self->widEGS;
    const char* str = gtk_entry_get_text(GTK_ENTRY(widget));
    return tool_strdup(str);
}

extern void       gui_slin_setText(Twid_slin* self, CstrBrief str)
{
    GtkWidget* widget = (GtkWidget*)self->widEGS;
    gtk_entry_set_text(GTK_ENTRY(widget),str);
}

extern void        gui_slin_disable(Twid_slin* self)
{
    GtkWidget* widget = (GtkWidget*)self->widEGS;
    gtk_editable_set_editable(GTK_EDITABLE(widget),FALSE);
}

extern void        gui_slin_enable (Twid_slin* self)
{
    GtkWidget* widget = (GtkWidget*)self->widEGS;
    gtk_editable_set_editable(GTK_EDITABLE(widget),TRUE);
}
/*======================================================================*/
/*= Text output widgets (Single Line)                                  =*/

extern Tegs_widget gui_slout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        CstrBrief text)
{
    GtkWidget* ret = gtk_label_new(text);
    //TODO Ajouter une frame ??
    /*if (bdw>=0)     { ret->setLineWidth(bdw);
                      ret->setFrameStyle(QFrame::Box|QFrame::Plain); }
*/
    if (dx>0) {
        if (dy>0)
            gtk_widget_set_size_request(ret,dx,dy);
        else 
            gtk_widget_set_size_request(ret,dx,-1);
    }
    else {
        if (dy>0)       gtk_widget_set_size_request(ret,-1,dy);
    }
    widget_setColors(gui, ret, wn, bg,fg);
    return ret;
}

extern TstrEaten gui_slout_getText (Tegs_widget self)
{
    GtkWidget*    label = (GtkWidget*)self;
    char *str  = (char *)gtk_label_get_text(self);
    return str;
}
extern void gui_slout_setText(Tegs_widget self, CstrBrief str)
{
    gtk_label_set_text(GTK_LABEL(self),str);
}
extern void gui_slout_setTextWrn(Tegs_widget self, CstrBrief str)
{ gui_slout_setText(self,str); }
extern void gui_slout_setTextErr(Tegs_widget self, CstrBrief str)
{ gui_slout_setText(self,str); }

/*======================================================================*/
/*= Text output widgets (Multiple Line with scroll bar)                =*/

extern Tegs_widget gui_mlout_nbf(
        Tvgui*gui, Tegs_widget pw, CstrBrief wn,
        int bdw, int dx, int dy, TcolorEGS bg, TcolorEGS fg,
        int sb)
{
    GtkWidget* self = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(self),FALSE);
    //if (bdw>=0)               w->setLineWidth(bdw); // FIXME
    if (dx>0) {
        if (dy>0)       gtk_widget_set_size_request(self,dx,dy);
        else gtk_widget_set_size_request(self,dx,-1);
    }
    else {
        if (dy>0)       gtk_widget_set_size_request(self,-1,dy);
    }
    widget_setColors(gui, self, wn, bg,fg);
    return self;
}

extern void gui_mlout_setText(Twid_mlout* self, CstrBrief str)
{
    GtkWidget* qte = (GtkWidget*)self->widEGS;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(qte));
    gtk_text_buffer_set_text(buffer,str,-1);
}
extern void gui_mlout_setTextWrn(Twid_mlout* self, CstrBrief str)
{ gui_mlout_setText(self,str); }
extern void gui_mlout_setTextErr(Twid_mlout* self, CstrBrief str)
{ gui_mlout_setText(self,str); }

static void gui_mlout_appTextWC(Twid_mlout* self, CstrBrief str,
                                TcolorEGS*c)
{
    GtkWidget* qte = (GtkWidget*)self->widEGS;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(qte));
    gtk_text_buffer_insert(buffer,NULL,str,-1);
    //qte->verticalScrollBar()->setValue(qte->verticalScrollBar()->maximum()); //TODO hadjustement ?
}

extern void gui_mlout_appText(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextWrn(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }
extern void gui_mlout_appTextErr(Twid_mlout* self, CstrBrief str)
{ gui_mlout_appTextWC(self,str,0); }


/*======================================================================*/
/*= Slider widget.                                                    =*/
static double    convVgui2Egs(double vgui, Twid_slider *data) {
        double res = 1./data->inc - (vgui/data->inc);
        return round(res); }
static double convEgs2Vgui(double egs, Twid_slider *data) {
        double res = egs;
        res = 1. - res*data->inc;
        return res; }

static void valueChanged(GtkRange *range, gpointer user_data)
{
   Twid_slider *self = user_data;
   double old_val = self->val;
   double new_val = convEgs2Vgui(gtk_range_get_value(range),self);
   
   if (new_val == old_val) return ;
   self->val = new_val;
   long int lock = (long int) g_object_get_data(G_OBJECT(range),"lock");
   if ( lock==1 ) return ;
   if (self->chg && gtk_widget_is_visible(GTK_WIDGET(range))) {
        g_object_set_data(G_OBJECT(range),"lock",(void*)1);
        self->chg(self,self->ud,self->val);
        g_object_set_data(G_OBJECT(range),"lock",(void*)0);
   }
}


extern Tegs_widget  gui_slider_create(Twid_slider*self,
                                      Tegs_widget pw, CstrBrief wn,
                                      int bdt, int dx, int dy)
{
    GtkWidget* ret = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,0,round(1./self->inc),1);
    if (dx>0 || dy >0) {
        GtkCssProvider* cssprov = gtk_css_provider_new();
        GError* err = NULL;
        char css[100];
        if(dx>0) {
            if (dy>0)
                sprintf(css, "* {min-width: %d; min-height: %d}",dx,dy);
            else
                sprintf(css, "* {min-width: %d}",dx);
        } else {
            if (dy>0)
                sprintf(css, "* {min-height: %d}",dy);
        }
        gtk_css_provider_load_from_data(cssprov, css, 100, &err); 
        gtk_style_context_add_provider(gtk_widget_get_style_context(ret),
                                          (GtkStyleProvider*)cssprov,
                                          GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
    gtk_range_set_value(GTK_RANGE(ret),0);    
    widget_setColors(self->gui, ret, wn,EGSCOLOR_default,EGSCOLOR_default);
    g_signal_connect(GTK_RANGE(ret),"value-changed",G_CALLBACK(valueChanged),self);
    g_object_set_data(G_OBJECT(ret),"lock",(void*)0);
    return ret;
}

extern double       gui_slider_get (Twid_slider*self)
{
    GtkWidget* w = (GtkWidget*)self->widEGS;
    double vguival = convEgs2Vgui(gtk_range_get_value(GTK_RANGE(w)),self);
    return vguival;
}

extern void         gui_slider_set (Twid_slider*self, double v)
{
    GtkWidget* w = (GtkWidget*)self->widEGS;
    double egsval = convVgui2Egs(v,self);
    long int lock = (long int) g_object_get_data(G_OBJECT(w),"lock");
    g_object_set_data(G_OBJECT(w),"lock",(void*)lock+1);
    gtk_range_set_value(GTK_RANGE(w),egsval);
    lock = (long int) g_object_get_data(G_OBJECT(w),"lock");
    g_object_set_data(G_OBJECT(w),"lock",(void*)lock-1);
}

/*======================================================================*/

