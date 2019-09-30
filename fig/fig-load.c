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
 *      $Id: fig-load.c 142 2018-05-28 16:35:10Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/fig/fig-load.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= handle the loading of figure.                                      =*/
/*======================================================================*/

#include "fig-io.h"

/*======================================================================*/

extern Tfigure* fig_new_byFile  (Tmsgdrv*md, CstrBrief path, CcolorSet* sc)
{
    FILE*    stream;
    Tfigure* ret;

    if ( (stream=fopen(path, "r")) == NULL ) {
        tool_errerr_printf(md,"can not open %s file for reading: %s", path,strerror(errno));
        return 0;
    }

    ret = fig_new_byStream(md,path,sc,stream);
    fclose(stream);
    return ret;
}

/*======================================================================*/

extern Tfigure* fig_new_byStream(Tmsgdrv*md, CstrBrief path,
                                 CcolorSet* sc, FILE* stream)
{
    Tfigure* fig = fig_new_empty(md,path,0,sc);

    if ( fig==0 ) {
        tool_errerr_nomem(md);
        return fig;
    }

    Tloadpar param;
    memset(&param,0,sizeof(param));
    param.msgdrv       = md;
    odsty_initFill(&param.stf_white, sc->eles[7]);
    param.stf_white.stf_style = DA_FS_Solid;
    param.file_name    = path;
    param.line_no      = 1;
    param.stream       = stream;
    param.fig          = fig;

    int status;
    // try original xfig format 
    rewind(param.stream);
    switch ( xfig32_check(&param) ) {
      case  0:
        rewind(param.stream);
        status=xfig32_load(&param);
        goto end_parsed;
      case -1:
        tool_errerr_printf(param.msgdrv,"%s file seems to be a XFIG-%f file",
                param.file_name, param.ffmt);
        tool_errerr_printf(param.msgdrv,"use xfig to convert it to XFIG-3.2 format");
        goto end_nofmt;
    }

end_parsed:
    tool_free     ( param.comment );
    odsty_freeFill(&param.stf_white);
    sintset_delete( param.clrs );
    if (status != 0) {
        tool_errerr_printf(param.msgdrv,"%s file is bad formated",param.file_name);
        tool_errerr_printf(param.msgdrv,"figure loading is aborted");
        fig_delete(fig);
        return 0;
    }

    tool_inferr_printf(md,"%s file loaded, %d objects found",
            param.file_name,param.num_object);
    return fig;

end_nofmt:
    tool_free     ( param.comment );
    odsty_freeFill(&param.stf_white);
    sintset_delete( param.clrs );
    tool_errerr_printf(param.msgdrv,"unrecognized format for %s file",param.file_name);
    tool_errerr_printf(param.msgdrv,"figure loading is aborted");
    fig_delete(fig);
    return 0;
}

/*======================================================================*/

extern void figio_parse_vprintf(void(*func)(Tmsgdrv*,CstrBrief,va_list),
                         Tloadpar* param, CstrBrief fmt, va_list ap)
{
    char *fmt2 = tool_aprintf("%s:%d: %s",
            param->file_name!=0 ? param->file_name : "unnamed stream",
            param->line_no,fmt);
    if (fmt2==0)
        return;
    func(param->msgdrv,fmt2,ap);
    free(fmt2);
}

extern void figio_parse_printf(void(*func)(Tmsgdrv*,CstrBrief,va_list),
                         Tloadpar* param, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    figio_parse_vprintf(func,param,fmt,ap);
    va_end(ap);
}

/*======================================================================*/
