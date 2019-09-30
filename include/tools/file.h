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
 *      $Id: file.h 377 2018-12-09 22:39:34Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/tools/file.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Miscellaneous utilities about files.                               =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_TOOLS_FILE_H
#define FILE_TOOLS_FILE_H
#include "CPLUSPLUS.h"
/*======================================================================*/

#include "tools/base-types.h"
#include "tools/strings.h"

/*======================================================================*/

/**
 * The tool_chdir0 function sets the CWD to the d directory and
 * updates the gl_currDir globale variable.
 * If changing the current directory to d fails, the gl_currDir variable
 * is not modified.
 *
 * Return:
 * The function returns 0 on success otherwise it returns -1.
**/ 
extern int tool_chdir0(CstrBrief d);

/**
 * The tool_getcwd_brief function returns the absolute path of the
 * current working directory.
 * Caution: Calling the tool_chdir function discards the returned pointer.
 * 
 * The tool_getcwd_eated function returns the absolute path of the
 * current working directory. The returned pointer is a private duplication
 * of the CWD, so the caller must free it after use.
**/ 
extern CstrBrief tool_getcwd_brief();
extern TstrEaten tool_getcwd_eated();

/**
 * Returns the timestamp (modified time) of the fn file
 * on success or -1 otherwise.
**/
extern time_t tool_fileTimeStamp(CstrBrief fn);

/**
 * Returns 1 if the paths point to the the same file, otherwise 0.
 * "The same file" means that if you open them and read them, you will get
 * get the same data (assuming it's a regular file).
**/
extern int tool_areSameFile(CstrBrief fn1, CstrBrief fn2);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/**
 * The tool_tempfile function creates a new file in the gl_tempDir directory
 * and opens it in RW mode and returns the file descriptor.
 * If the path argument is NULL, the tempory file will be deleted
 * when the caller closes the file descriptor.
 * If the path argument is not NULL, the function sets *path to the
 * aboslute path of the tempory file.
 *
 * The tool_tempfileInDir function creates is similar to the tool_tempfile
 * function except that the new file is created in the dir directory.
 *
 * Return:
 *  On success the function returns a file descriptor to the tempory file
 *  otherwise it retuns the NULL pointer.
**/
extern FILE* tool_tempfile     (TstrEaten* path);
extern FILE* tool_tempfileInDir(TstrEaten* path, CstrBrief dir);

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/**
 * The TrecentFiles type handles a LRU cache of file paths.
 * 
 * The tool_rf_new() function creates an empty cache, its maximum size
 * is sz and the maximum length of paths in rf_short is len characters.
 * 
 * The tool_rf_delete() function frees the memory, the self recent-files
 * allocated.
 *
 * The tool_rf_addClone and tool_rf_addEaten adds the fn path to the
 * self recent-file. The tool_rf_addEaten function will free the fn
 * string.
 * If the fn path is already present in the recent-file or if it does
 * not exist, it is not added,
 *
 * The tool_rf_chgSize sets to sz the maximum size of the self recent-files.
**/

extern TrecentFiles*tool_rf_new      (int sr, int len);
extern TrecentFiles*tool_rf_newByFile(int sz, int len, Tmsgdrv* msgdrv, TstrBrief fn);

extern void         tool_rf_delete    (TrecentFiles*self);
extern void         tool_rf_addEaten  (TrecentFiles*self, TstrEaten fn);
Inline void         tool_rf_addClone  (TrecentFiles*self, CstrBrief fn) { tool_rf_addEaten(self,tool_strdup(fn)); }
extern void         tool_rf_sup       (TrecentFiles*self, int pos);
extern void         tool_rf_chgSize   (TrecentFiles*self, int sz);
extern void         tool_rf_chgLength (TrecentFiles*self, int len);
extern void         tool_rf_load      (TrecentFiles*self, Tmsgdrv* msgdrv, TstrBrief fn);
extern void         tool_rf_save      (TrecentFiles*self, Tmsgdrv* msgdrv, TstrBrief fn);

struct _TrecentFiles {
    int rf_cacheSize; // max number of paths in LRU cache
    int rf_length;    // max length of strings in short set.
    TastrSet*rf_absolute; // absolute paths (the more recent is at 0 index)
    TastrSet*rf_short;    // same files as rf_absolute but relative to
                          // HOME or WD and truncated to rf_length.
};

/*=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =*/
/**
 * The tool_fileMove function renames the "from" file into the "to" file.
 * The tool_fileCopy function duplicates the "from" file into the "to" file.
 * The tool_fileOpenXXCheck functions check if the path file is a
 * regular file and if it can be open in the XX mode.
 * The tool_fileDirXXXCheck functions check if the path file is a
 * directory and if it has the XXX permissions.
 *
 * Return:
 *  On success, the tool_fileMove and tool_fileCopy functions returns 0.
 *  On success, the tool_fileOpenXXCheck functions returns 0.
 *  On success, the tool_fileDirXXXCheck functions returns 0.
 **/
extern FILE* tool_fileOpenWO    (Tmsgdrv*md, CstrBrief path);
extern FILE* tool_fileOpenRO    (Tmsgdrv*md, CstrBrief path);
extern FILE* tool_fileOpenWR    (Tmsgdrv*md, CstrBrief path);
extern int   tool_fileCreat     (Tmsgdrv*md, CstrBrief path);
extern int   tool_fileMove      (Tmsgdrv*md, CstrBrief from, CstrBrief to);
extern int   tool_fileCopy      (Tmsgdrv*md, CstrBrief from, CstrBrief to);
extern int   tool_fileCopyStream(Tmsgdrv*md, CstrBrief from, FILE*to);

extern int   tool_fileOpenWOCheck(CstrBrief path);
extern int   tool_fileOpenROCheck(CstrBrief path);
extern int   tool_fileOpenWRCheck(CstrBrief path);
extern int   tool_fileDirXXXCheck(CstrBrief path);
extern int   tool_fileDirRxECheck(CstrBrief path);
extern int   tool_fileDirXWECheck(CstrBrief path);
extern int   tool_fileDirRWECheck(CstrBrief path);

/**
 * The tool_fileMkdir function creates the path directoty if it
 * does not exist.
 * The tool_fileMkdirR function creates the path directoty and all the
 * parent directories of path if they do not exist.
 * The tool_fileMkdirOfPath (R) function is similar to the
 * tool_fileMkdirOfPath (R) function except that it creates the
 * dirname of path.
 *
 * Return:
 *  On success (the directories existed or were created) these functions
 *  return 0.
**/
extern int   tool_fileMkdir       (Tmsgdrv*md, CstrBrief path);
extern int   tool_fileMkdirR      (Tmsgdrv*md, CstrBrief path);
Inline int   tool_fileMkdirOfPath (Tmsgdrv*md, CstrBrief path) { TstrEaten dir=tool_dirname(path); int status=tool_fileMkdir (md,dir); tool_free(dir); return status; }
Inline int   tool_fileMkdirOfPathR(Tmsgdrv*md, CstrBrief path) { TstrEaten dir=tool_dirname(path); int status=tool_fileMkdirR(md,dir); tool_free(dir); return status; }

/*======================================================================*/

/**
 * The tool_fileEleOfDirD function returns a pointer to a string set that
 * contains all the directory names of the dp directory.
 * The first element is always "..".
 *
 * The tool_fileEleOfDirF function returns a pointer to a string set that
 * contains all the plain file names of the dp directory that match
 * the re regular expression.
 *
 * The returned string set must be freed after use with the strset_free
 * function.
 *
 * Return:
 *  On success, the functions return a not NULL pointer otherwise
 *  they return the NULL pointer.
 *
 * Parameters:
 *  - dp: directory path
 *  - wh: 0: do'nt add hidden files/directories to the set.
 *        1: add hidden files/directories to the set (starting by a dot).
 *  - re: a regular expression or NULL for all the file names.
 *        The re format is the one used in shell (see glob(7) manual). 
 *  - md: the message driver for printing messages
 *        or NULL for gl_msgdrv_default.
 *  - wpp: not used.
**/
extern TastrSet* tool_fileEleOfDirD(Tmsgdrv*md, CstrBrief dp, int wh, int wpp);
extern TastrSet* tool_fileEleOfDirF(Tmsgdrv*md, CstrBrief dp, CstrBrief re, int wh);

/*======================================================================*/

//extern int       tool_filePathLength(CstrBrief path);
//extern TstrEaten tool_fileRelPathToDir  (CstrBrief file, CstrBrief dir);
//extern TstrEaten tool_fileRelPathToFile (CstrBrief file, CstrBrief from) { TstrEaten dir = tool_dirname(from); TstrEaten rp=tool_fileRelPathToDir(file,dir); tool_free(dir); return rp; }

/*======================================================================*/
TOOLS_CPLUSPLUS_CLOSE
/*======================================================================*/
#endif  // FILE_TOOLS_FILE_H
/*======================================================================*/
