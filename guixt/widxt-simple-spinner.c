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
 *      $Id: widxt-simple-spinner.c 277 2018-07-05 08:42:59Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/guixt/widxt-simple-spinner.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Spinner widgets (EGS driver).                                      =*/
/*=                                                                    =*/
/*= See the vgui/wid-simple.h header for the type description and      =*/
/*= the function description and for documentation.                    =*/
/*======================================================================*/

#include "guixt.h"

#include "vgui/wid-simple.h"

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

