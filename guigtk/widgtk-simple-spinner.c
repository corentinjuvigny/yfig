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
/*= Spinner widgets (EGS driver).                                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/
#include "guigtk.h"

#include "vgui/wid-simple.h"

/*======================================================================*/
/*= Spinner widget (EGS Interface)                                     =*/

static void valueChanged(GtkSpinButton *widget, gpointer user_data)
{
    double incr1,incr2;
    gtk_spin_button_get_increments(widget,&incr1,&incr2);
    Twid_spinner *spin = user_data;
    Trange* range = &spin->range;
    double old_value = range->val;
    double new_value = gtk_spin_button_get_value(widget);
    if (abs(new_value-old_value) < pow(10,-range->nAdt))
        return;
    else {
        if (new_value < old_value)
            range_dec(range);
        else
            range_inc(range);
    }
    new_value = range->val;
    gtk_spin_button_set_value(widget,new_value); 
    if (  spin->cbChg ) {
        if (spin->enabled) spin->cbChg(spin,spin->cbUd,new_value);
    }

}
//TODO ; faire 2 fonctions de callback, une pour une entrée manuelle (chercher signal (entry?editable?)), une pour via boutons
static void valueValidated(GtkSpinButton *widget, gpointer user_data)
{
    Twid_spinner *spin = user_data;
    Trange* range = &spin->range;
    double old_value = range->val;
    double new_value = gtk_spin_button_get_value(widget);
    /*if (abs(new_value-old_value) < pow(10,-range->nAdt) || new_value > old_value + range->iof || new_value < old_value - range->iof)
        range_setVal(range,new_value);
    else {
        if (new_value < old_value)
            range_dec(range);
        else
            range_inc(range);
        new_value = range->val;
    }*/
    //range->val = new_value;
    range_setVal(range,new_value);
    g_signal_handlers_block_by_func(G_OBJECT(widget),G_CALLBACK(valueChanged),spin);
    gtk_spin_button_set_value(widget,new_value);
    g_signal_handlers_unblock_by_func(G_OBJECT(widget),G_CALLBACK(valueChanged),spin);
    if (  spin->cbVal ) {
        if (spin->enabled) spin->cbVal(spin,spin->cbUd,new_value);
    }
    if (  spin->cbChg ) {
        if (spin->enabled) spin->cbChg(spin,spin->cbUd,new_value);
    }
}

extern void gui_spinner_dbf(Twid_spinner*spin, Tvgui*gui, Tegs_widget p)
{
    Trange* range = &spin->range;
    double max,min;
    if ( (range->flag&RANGE_FL_LowBound)!=0 )
            min = range->min;
    if ( (range->flag&RANGE_FL_UppBound)!=0 )
            max = range->max;
    else {  
            min = -pow( 10, range->nBdt);
            max =  pow( 10, range->nBdt);
    }
    GtkWidget* qspin = gtk_spin_button_new_with_range(min,max,0.1);
    gtk_spin_button_set_digits (GTK_SPIN_BUTTON(qspin),range->nAdt);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(qspin),range->iof,2*range->iof);
    widget_setColors(gui, qspin, wn_dialSlin,EGSCOLOR_default,EGSCOLOR_default);
    spin->enabled=1;
    g_signal_connect(qspin,"value-changed",G_CALLBACK(valueValidated),spin);
    spin->self = qspin;
}
extern void          gui_spinner_setNum (Twid_spinner* spin, double v)
{
    Trange* range = &spin->range;
    GtkWidget* qspin = (GtkWidget*)spin->self;
    g_signal_handlers_block_by_func(G_OBJECT(qspin),G_CALLBACK(valueValidated),spin);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(qspin),v);
    g_signal_handlers_unblock_by_func(G_OBJECT(qspin),G_CALLBACK(valueValidated),spin);
}

extern void          gui_spinner_free   (Twid_spinner* spin)
{
    GtkWidget* qspin = (GtkWidget*)spin->self;
    gtk_widget_destroy(qspin);
}

extern void          gui_spinner_update (Twid_spinner* spin)
{
    Trange* range = &spin->range;
    GtkWidget *qspin = (GtkWidget*)spin->self;
    double min,max;
    gtk_spin_button_get_range(GTK_SPIN_BUTTON(qspin),&min,&max);
    if ( (range->flag&RANGE_FL_LowBound)!=0 ) gtk_spin_button_set_range(GTK_SPIN_BUTTON(qspin),range->min,max);
    gtk_spin_button_get_range(GTK_SPIN_BUTTON(qspin),&min,&max);
    if ( (range->flag&RANGE_FL_UppBound)!=0 ) gtk_spin_button_set_range(GTK_SPIN_BUTTON(qspin),min,range->max);
}

extern void          gui_spinner_enable (Twid_spinner* spin)
{
    GtkWidget* qspin = (GtkWidget*)spin->self;
    gtk_widget_set_sensitive (qspin,TRUE);
}

extern void          gui_spinner_disable(Twid_spinner* spin)
{
    GtkWidget* qspin = (GtkWidget*)spin->self;
    gtk_widget_set_sensitive (qspin,FALSE);
}

//extern TstrEaten     gui_spinner_setStr (Twid_spinner* spin);

extern void gui_spinner_clear(Twid_spinner* spin)
{
    GtkWidget* qspin = (GtkWidget*)spin->self;
    g_signal_handlers_block_by_func(G_OBJECT(qspin),G_CALLBACK(valueValidated),spin);
    gtk_entry_set_text(GTK_ENTRY(qspin),"");
    g_signal_handlers_unblock_by_func(G_OBJECT(qspin),G_CALLBACK(valueValidated),spin);
}
