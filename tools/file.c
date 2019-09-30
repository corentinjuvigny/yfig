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
 *$Software: YFIG-TOOL (utilities shared by the different modules)
 *      $Id: file.c 373 2018-11-29 00:24:45Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/tools/file.c $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities about files.                               =*/
/*======================================================================*/

#include "tools-config.h"
#include "tools/file.h"
#include "tools/strings.h"
#include "tools/globalvars.h"

/*======================================================================*/

extern int tool_chdir0(CstrBrief d)
{
    int ret=-1;
    if ( chdir(d)>=0 ) {
        tool_free( (void*)gl_currDir );
        gl_currDir = getcwd(NULL,0);
        ret=0;
    }
    return ret;
}

extern CstrBrief tool_getcwd_brief() { return gl_currDir; }
extern TstrEaten tool_getcwd_eated() { return tool_strdup(gl_currDir); }

extern int tool_areSameFile(CstrBrief fn1, CstrBrief fn2)
{
    struct stat info1,info2;

    if ( stat(fn1,&info1)!=0 ||  stat(fn2,&info2)!=0 )
        return 0;
    return info1.st_dev==info2.st_dev && info1.st_ino==info2.st_ino;
}

extern time_t tool_fileTimeStamp(CstrBrief fn)
{
    struct stat info;

    if (stat(fn, &info) != 0)
        return -1;
    return info.st_mtime;
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
extern FILE* tool_tempfile(TstrEaten* path)
{
    int fd;
    FILE* ret = NULL;
    TstrEaten tmpfile = tool_aprintf("%s/%s-XXXXXX",gl_tempDir,gl_progname);
    if ( (fd=mkstemp(tmpfile))==-1 ) {
        tool_free( tmpfile );
        return ret;
    }
    ret = fdopen(fd,"r+");
    if ( ret==NULL ) {
        tool_free( tmpfile );
        return ret;
    }
    if ( path==NULL ) {
        unlink   ( tmpfile );
        tool_free( tmpfile );
    } else
        *path = tmpfile;
    return ret;
}

extern FILE* tool_tempfileInDir(TstrEaten* path, CstrBrief dir);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/

#define RF_SZ_MIN   3
#define RF_SZ_MAX  20
#define RF_LEN_MIN 20
#define RF_LEN_MAX 40

extern TrecentFiles*tool_rf_new(int sz, int len)
{
    TrecentFiles* self = tool_malloc(sizeof(*self));
    self->rf_cacheSize = sz;
    self->rf_length    = len;
    self->rf_absolute  = astrset_new(len);;
    self->rf_short     = astrset_new(len);;
    return self;
}
extern TrecentFiles*tool_rf_newByFile(int sz, int len, Tmsgdrv* msgdrv, TstrBrief fn)
{
    TrecentFiles* self = tool_rf_new(sz,len);
    tool_rf_load( self, msgdrv, fn);
    return self;
}

extern void         tool_rf_delete  (TrecentFiles*self)
{
    astrset_delete( self->rf_short    );
    astrset_delete( self->rf_absolute );
    tool_free     ( self              );
}

extern void         tool_rf_addEaten(TrecentFiles*self, TstrEaten fn)
{
    int i,len; char *p,*tmp,*sh;

    // fn file does not exist --> return
    if ( tool_fileOpenWRCheck(fn) ) return;

    // if fn file exists in self --> just handle LRU
    for (i=0; i<self->rf_absolute->eleNb ; i+=1) {
        p = self->rf_absolute->eles[i];
        if ( tool_areSameFile(fn,p)==1 ) {
            astrset_movI2Pshift(self->rf_absolute,i,0);
            astrset_movI2Pshift(self->rf_short,   i,0);
            return;
        }
    }

    // add fn file to self.
    if ( fn[0]!='/' ) {
        tmp = tool_aprintf("%s/%s",gl_currDir,fn);
        tool_free(fn);
        fn = tmp;
    }

    for (sh=tool_dirname(fn) ; sh[1]!=0 ; tool_strchgEaten(&sh,tool_dirname(sh))) {
        if ( tool_areSameFile(sh,gl_homeDir) ) {
            len = strlen( sh );
            tmp = tool_aprintf("~%s",fn+len);
            tool_strchgEaten(&sh,tmp);
            goto found;
        }
        if ( tool_areSameFile(sh,gl_currDir) ) {
            len = strlen( sh );
            tmp = tool_aprintf(".%s",fn+len);
            tool_strchgEaten(&sh,tmp);
            goto found;
        }
    }
    tool_strchgClone(&sh,fn);

found:
    len = strlen(sh);
    if ( len>self->rf_length ) {
        tmp = tool_strdup( sh+len-self->rf_length);
        tool_strchgEaten(&sh,tmp);
    }
    if ( self->rf_absolute->eleNb==self->rf_cacheSize ) {
        astrset_supLast(self->rf_absolute);
        astrset_supLast(self->rf_short   );
    }
    if ( self->rf_absolute->eleNb==0 ) {
        astrset_addEaten(self->rf_absolute,fn);
        astrset_addEaten(self->rf_short   ,sh);
    } else {
        astrset_addBeforeEaten(self->rf_absolute,0,fn);
        astrset_addBeforeEaten(self->rf_short   ,0,sh);
    }
}

extern void         tool_rf_sup       (TrecentFiles*self, int pos)
{
    astrset_supShift( self->rf_absolute,pos);
    astrset_supShift( self->rf_short,   pos);
}

extern void         tool_rf_chgSize   (TrecentFiles*self, int sz)
{
    if ( sz>=self->rf_absolute->eleNb ) {
        self->rf_cacheSize = sz;
    } else {
        self->rf_cacheSize = sz;
        while ( self->rf_cacheSize<self->rf_absolute->eleNb ) {
            astrset_supLast(self->rf_absolute);
            astrset_supLast(self->rf_short);
        }
    }
}

extern void         tool_rf_chgLength (TrecentFiles*self, int len)
{
#if 1 // FIXME REMOVE
    self->rf_length = len;
#else
    int i;
    TastrSet* save = astrset_newClone( self->rf_absolute );
    astrset_clear(  self->rf_absolute );
    astrset_clear(  self->rf_short    );
    self->rf_length = len;
    for (i=save->eleNb-1 ; i>=0 ; i-=1) 
        tool_rf_addClone( self, save->eles[i] );
    astrset_delete( save );
#endif
}

extern void         tool_rf_load      (TrecentFiles*self, Tmsgdrv* msgdrv, TstrBrief fn)
{
#define BufSz 1000
    char  buf[BufSz+2];
    FILE* stream = tool_fileOpenRO(msgdrv,fn);
    if ( stream==0 ) {
        tool_errerr_printf(msgdrv,"open file history not loaded");
        return;
    }

    int x,sz,len,line=0,lineno=0;
    while (1) {
        lineno += 1;
        if ( fgets(buf,BufSz+1,stream)==0 ) {
            if ( ferror(stream) ) goto error_io;
            break;
        } else {
            len=strlen(buf);
            if ( len==BufSz && buf[BufSz-1]!='\n' ) goto error_fmt;
            buf[len-1] = 0;
        }
        if ( buf[0]=='#') continue;
        if ( tool_strIsEmptyLine(buf) ) continue;
        if ( line==0 ) {
            if ( sscanf(buf,"%d",&x)!=1 ) goto error_fmt;
            if ( x<RF_SZ_MIN ) sz=RF_SZ_MIN; else if (x>RF_SZ_MAX) sz=RF_SZ_MAX; else sz=x;
            if ( x!=sz )
                tool_errerr_printf(msgdrv,"%s:%d: %d is an invalid cache size, changed to %d when parsing the file history file.",fn,lineno,x,sz);
            line+=1;
        } else if ( line==1 ) {
            if ( sscanf(buf,"%d",&x)!=1 ) goto error_fmt;
            if ( x<RF_LEN_MIN ) len=RF_LEN_MIN; else if (x>RF_LEN_MAX) len=RF_LEN_MAX; else len=x;
            if ( x!=len )
                tool_errerr_printf(msgdrv,"%s:%d: %d is an invalid length, changed to %d when parsing the file history file.",fn,lineno,x,len);
            line+=1;
            tool_rf_chgSize  (self,sz);
            tool_rf_chgLength(self,len);
        } else {
            if ( buf[0]=='/' ) {
                tool_rf_addClone(self,buf);
                line+=1;
            } else
                tool_errerr_printf(msgdrv,"%s:%d: not an absolute path: \"%s\"",fn,lineno,buf);
        }
    }
    if ( line<2 ) goto error_fmt;

    fclose( stream );
    tool_inferr_printf(msgdrv,"open file history loaded from file: %s",fn);
    return;

error_io:
    fclose( stream );
    tool_errerr_printf(msgdrv,"%s:%d: io error when reading the file history file.",fn,lineno);
    return;
error_fmt:
    fclose( stream );
    tool_errerr_printf(msgdrv,"%s:%d: the file history file is bad formatted.",fn,lineno);
    return;
}

extern void         tool_rf_save    (TrecentFiles*self, Tmsgdrv* msgdrv, TstrBrief fn)
{
    FILE* stream = tool_fileOpenWO(msgdrv,fn);
    if ( stream==0 ) {
        tool_errerr_printf(msgdrv,"open file history not saved");
        return;
    }

    fprintf(stream,
      "# History of figure files that %s opened.\n"
      "# Syntax is:\n"
      "#   - 1 line with the history size in range [%d:%d].\n"
      "#   - 1 line with the byte size of short path in range [%d:%d].\n"
      "#   - a line for each path starting column 1 by / (absolute path).\n"
      "# Lines starting by the '#' char are ignored.\n"
      "# Empty lines (containing only space and tab chars are ignored.\n"
      "# Lines must be less than 1000 characters.\n"
      "# The parser stops as soon as it encounters a such line.\n\n",
      PACKAGE,RF_SZ_MIN,RF_SZ_MAX,RF_LEN_MIN,RF_LEN_MAX);

    int sz = self->rf_cacheSize, len=self->rf_length;
    if (  sz<RF_SZ_MIN  )  sz=RF_SZ_MIN;  else if (  sz>RF_SZ_MAX  ) sz=RF_SZ_MAX;
    if ( len<RF_LEN_MIN ) len=RF_LEN_MIN; else if ( len>RF_LEN_MAX ) len=RF_LEN_MAX;
    fprintf(stream,"%d\n%d\n",sz,len);

    int i;
    for (i=self->rf_absolute->eleNb-1 ; i>=0 ; i-=1)
        fprintf(stream,"%s\n",self->rf_absolute->eles[i]);

    fclose( stream );
    tool_inferr_printf(msgdrv,"open file history saved into file: %s",fn);
}

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
extern FILE* tool_fileOpenWO(Tmsgdrv*md, CstrBrief path)
{
    FILE* stream;
    if (  (stream=fopen(path,"w"))==NULL )
        tool_errerr_printf(md,"can not open \"%s\" file for writing: %s",path,strerror(errno));
    return stream;
}
extern FILE* tool_fileOpenRO(Tmsgdrv*md, CstrBrief path)
{
    FILE* stream;
    if (  (stream=fopen(path,"r"))==NULL )
        tool_errerr_printf(md,"can not open \"%s\" file for reading: %s",path,strerror(errno));
    return stream;
}
extern FILE* tool_fileOpenWR(Tmsgdrv*md, CstrBrief path);

extern int   tool_fileCreat(Tmsgdrv*md, CstrBrief path)
{
    int status;
    if (  (status=creat(path,0666))<0 )
        tool_errerr_printf(md,"creation of \"%s\" file failed: %s",path,strerror(errno));
    return status;
}

extern int tool_fileMove(Tmsgdrv*md, CstrBrief from, CstrBrief to)
{
    int status;
    if ( (status=rename(from,to))<0 ) {
        if ( errno!=EXDEV ) {
            // files are not on the same filesystem.
            tool_errerr_printf(md,"failed to move %s file to %s: %s", from,to,strerror(errno));
        } else if ( (status=tool_fileCopy(&gl_msgdrv_silence,from,to))==0 ) {
            unlink(from);
        } else 
            tool_errerr_printf(md,"failed to move %s file to %s: %s", from,to,strerror(errno));
    }
    return status;
}
            
extern int tool_fileCopy(Tmsgdrv*md, CstrBrief from, CstrBrief to)
{
    Tmsgdrv* mds = &gl_msgdrv_silence;
    FILE *in,*out;
    if ( (in=tool_fileOpenRO(mds,from))==0 ) {
failed_0:
        tool_errerr_printf(md,"failed to copy %s file to %s: %s", from,to,strerror(errno));
        return 1;
    }
    if ( (out=tool_fileOpenWO(mds,to))==0 ) {
failed_1:
        fclose( in );
        goto failed_0;
    }

    int  bufsz = 1024*10;
    Tstr buf   = tool_malloc(bufsz);

    int nbread,nbwrite;
    while ( (nbread=fread(buf,1,bufsz,in))!=0 ) {
        if ( (nbwrite=fwrite(buf,1,nbread,out))!=nbread ) {
failed_2:
            fclose   (out);
            tool_free(buf);
            goto failed_1;
        }
    }
    if ( ferror(in) ) goto failed_2;

    fclose   (in);
    fclose   (out);
    tool_free(buf);
    return 0;
}

extern int tool_fileCopyStream(Tmsgdrv*md, CstrBrief from, FILE* out)
{
    Tmsgdrv* mds = &gl_msgdrv_silence;
    FILE *in;
    if ( (in=tool_fileOpenRO(mds,from))==0 ) {
        tool_errerr_printf(md,"failed to copy %s file: %s", from,strerror(errno));
        return 1;
    }

    int  bufsz = 1024*10;
    Tstr buf   = tool_malloc(bufsz);

    int nbread,nbwrite;
    while ( (nbread=fread(buf,1,bufsz,in))!=0 ) {
        if ( (nbwrite=fwrite(buf,1,nbread,out))!=nbread ) {
failed:
            fclose   (in);
            tool_free(buf);
            return 1;
        }
    }
    if ( ferror(in) ) goto failed;

    fclose   (in);
    tool_free(buf);
    return 0;
}

static int tool_fileCheck(CstrBrief path, int kind, int mode)
{
    struct stat info;
    if (stat(path, &info) != 0)
        return -1;
    if( (info.st_mode&kind)==0 )
        return -1;
    return mode ? access(path,mode) : 0;
}
extern int tool_fileOpenWOCheck(CstrBrief path) { return tool_fileCheck(path,S_IFREG,R_OK); }
extern int tool_fileOpenROCheck(CstrBrief path) { return tool_fileCheck(path,S_IFREG,W_OK); }
extern int tool_fileOpenWRCheck(CstrBrief path) { return tool_fileCheck(path,S_IFREG,R_OK|W_OK); }
extern int tool_fileDirXXXCheck(CstrBrief path) { return tool_fileCheck(path,S_IFDIR,0); }
extern int tool_fileDirRXECheck(CstrBrief path) { return tool_fileCheck(path,S_IFDIR,R_OK|X_OK); }
extern int tool_fileDirXWECheck(CstrBrief path) { return tool_fileCheck(path,S_IFDIR,W_OK|X_OK); }
extern int tool_fileDirRWECheck(CstrBrief path) { return tool_fileCheck(path,S_IFDIR,R_OK|W_OK|X_OK); }

extern int   tool_fileMkdir       (Tmsgdrv*md, CstrBrief path)
{
#if defined(TOS_MINGW) && TOS_MINGW
    if ( tool_fileDirRWECheck(path)!=0 && mkdir(path)<0 ) {
#else
    if ( tool_fileDirRWECheck(path)!=0 && mkdir(path,0777)<0 ) {
#endif
        tool_errerr_printf(md,"creation of the \"%s\" directory failed: %s", path, strerror(errno));
        return -1;
    } else
        return 0;
}

extern int   tool_fileMkdirR      (Tmsgdrv*md, CstrBrief path)
{
    Tmsgdrv*  mds  = &gl_msgdrv_silence;

    if ( tool_fileMkdir(mds,path)==0 ) {
        return 0;
    } else { 
        TstrEaten pdir = tool_dirname(path);
        if ( tool_fileMkdirR(mds,pdir)==0 ) {
            if ( tool_fileMkdir(mds,path)==0 ) {
                tool_free(pdir);
                return 0;
            } else {
                tool_errerr_printf(md,"creation of the \"%s\" directory failed: %s", path, strerror(errno));
                tool_free(pdir);
                return -1;
            }
        } else {
            tool_errerr_printf(md,"creation of the \"%s\" directory failed: %s", path, strerror(errno));
            tool_free(pdir);
            return -1;
        }
    }
}

/*======================================================================*/
