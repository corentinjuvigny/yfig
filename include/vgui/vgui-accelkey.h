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
 *$Software: YFIG-VGUI (Virtual Graphic User Interface)
 *      $Id: vgui-accelkey.h 344 2018-11-09 16:16:16Z ia $
 * $HeadURL: http://lunix120.ensiie.fr/yfig/trunk/include/vgui/vgui-accelkey.h $
 *  $Author: Ivan Auge (Email: auge@ensiie.fr)
**======================================================================*/

/*======================================================================*/
/*= Utilities for defining accelerator key.                            =*/
/*=                                                                    =*/
/*= An accelerator key is represented by the TaccelDef type that is    =*/
/*= defined in the vgui-types.h header.                                =*/
/*======================================================================*/

/*======================================================================*/
#ifndef FILE_VGUI_ACCEL_H
#define FILE_VGUI_ACCEL_H
#include "vgui-CPLUSPLUS.h"
/*======================================================================*/

#include "vgui-types.h"

/*======================================================================*/
/*= Accelerator key sequence                                           =*/

#define GL_ACCEL_Key(l)   ((l)<< 0)   // lower case for letter
#define GL_ACCEL_Shift    ( 1 << 8)
#define GL_ACCEL_Control  ( 1 << 9)
#define GL_ACCEL_Meta     ( 1 <<10)
#define GL_ACCEL_ALL      ( 7 << 8)
#define GL_ACCEL_GetKey(a) ((a)&0xff)

/*======================================================================*/
/*= Interface                                                          =*/

/**
 * accel_toString converts ad into an human readable string and returns it.
 * accel_toString2 converts ad1 and ad2 into an human readable string and
 * returns it. The format of the returned string is either "ad1" or 
 * "ad2" or "ad1 or ad2".
 * accel_appendToString converts ad1 and ad2 into an human readable string,
 * appends it to the base string and then returns it.
 * The format of the appended string is either "(ad1)", "(ad2)" or
 * "(ad1 or ad2)".
 *
 * All the functions return a null pointer if the resulting string
 * is empty otherwise a pointer to a memory allocated buffer, the caller
 * must free after use.
**/
extern char* accel_toString(TaccelDef ad);
extern char* accel_appendToString(const char* base, TaccelDef ad1,
                                                    TaccelDef ad2);

/**
 * accel_byAmpersand generates an accelerate key "ctrl+X" into accel
 * where X is the letter of the src string that follows the first ampersand
 * ('&').
 * If the src string does not contains any ampersand, accel is set to zero.
 * Furthermore the function copies the src string into the dest string
 * without the first ampersand and if COPY_IN_AMPERSAND is given as flags,
 * it appends to the dest string the accelerate key (ctrl+X).
 *
 * Return:
 *   The function returns the position of the first ampersand in src or
 *   -1 if src does not contain any ampersand.
 *
 * Note: dest and accel can be set to null pointer.
**/
#define COPY_IN_AMPERSAND 1
#define NOT_COPY_IN_AMPERSAND 0
extern int   accel_byAmpersand(char*dest, const char*src, TaccelDef* accel, int flags);

/*======================================================================*/
VGUI_CPLUSPLUS_CLOSE
#endif // FILE_VGUI_ACCEL_H
/*======================================================================*/
