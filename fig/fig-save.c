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
 *$Software: YFIG-FDS (Figure Data Structure)
 *      $Id: fig-save.c 127 2018-04-14 10:30:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-save.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= handle back-up of figure.                                          =*/
/*======================================================================*/

#include "fig-io.h"

#if defined(HAVE_LOCALE_H) && HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(HAVE_DECL_SETLOCALE) && !HAVE_DECL_SETLOCALE
extern char *setlocale(int category, const char *locale);
#endif

/*======================================================================*/

/**
 * Search for an output format and initialize param with it.
 * Retun 0 on success otherwise the error code -1 (format not supported).
**/
static int fig_save_initFormat(Tsavepar*param, Tmsgdrv*md,  Cfigure*fig, int df)
{
    param->fmt=fig_filefmt_get2(fig,fig->save_format,fig->loaded_format);
    if ( param->fmt==0 && df==1 ) {
        CstrStatic fmt = fig_filefmt_defaultFig(fig);
        param->fmt = fig_filefmt_str2int(fmt);
    }
    if ( param->fmt==0 ) {
        tool_errerr_printf(md,
            SAVEHEAD " No format given or unsupported format: %s",
            fig->path ? fig->path : "???");
        return -1;
    }
    param->fmtstr = fig_filefmt_int2str(param->fmt);
    return 0;
}

static int fig_save_do(Tsavepar* param, Tmsgdrv*md, Cfigure* fig)
{
    int status;
    param->msgdrv = md;

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    char * currentlocale = setlocale(LC_NUMERIC, "C");
    currentlocale=tool_strdup(currentlocale);
#endif

    switch ( param->fmt ) {
      case 32: status = xfig32_save(param,fig); break;
      default:
        TEMP_spe_printf("unexpected format=%d",param->fmt);
        status = -1;
    }

#if defined(HAVE_SETLOCALE) && HAVE_SETLOCALE
    if ( currentlocale ) {
        setlocale(LC_NUMERIC, currentlocale);
        tool_free(currentlocale);
    }
#endif

    return status;
}
    

extern int fig_save_byFile  (Tmsgdrv*md, Cfigure*fig, int df)
{
    int status;
    Tsavepar param;

    /* search for an output format */
    if ( (status=fig_save_initFormat(&param,md,fig,df))!=0 ) return status;

    /* open the output file */
    if ( fig->path==0 ) {
        tool_errerr_printf(md,SAVEHEAD " Missing path file for saving figure");
        return -2;
    }
    if ( (param.stream=fopen(fig->path, "w"))==NULL ) {
        tool_errerr_printf(md,SAVEHEAD " Open %s file failed: %s",
                fig->path, strerror(errno));
        return -2;
    }

    /* do the work */
    status = fig_save_do(&param,md,fig);

    fclose(param.stream);

    return status;
}

extern int fig_save_byStream(Tmsgdrv*md, Cfigure*fig, int df, FILE*s)
{
    int      status;
    Tsavepar param;

    /* search for an output format */
    if ( (status=fig_save_initFormat(&param,md,fig,df))!=0 ) return status;

    /* do the work */
    param.stream = s;
    status = fig_save_do(&param,md,fig);

    return status;
}

/*======================================================================*/
