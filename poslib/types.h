/*
    Posadis - A DNS Server
    Data types
    Copyright (C) 2002  Meilof Veeningen <meilof@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __POSLIB_TYPES_H
#define __POSLIB_TYPES_H

#include "dibbler-config.h"

#if defined(__BORLANDC__) && defined(HAVE_INTTYPES_H) && defined(__WCHAR_TYPE__)
/* the following is a fix for a bug in the GNU C header inttypes.h, raised by borland c */
#  ifdef __WCHAR_TYPE__
     typedef __WCHAR_TYPE__ __gwchar_t;
#  else
#    define __need_wchar_t
#    include <stddef.h>
     typedef wchar_t __gwchar_t;
#  endif
#  define ____gwchar_t_defined	1
#endif


#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#if !defined(HAVE_INTTYPES_H) && !defined(HAVE_STDINT_H) && !defined(uint16_t) 
#  define uint16_t unsigned short
#  define uint32_t unsigned int
#endif

/*! \file poslib/types.h
 * \brief typedefs
 *
 * Contains some typedefs used in Poslib
 */

typedef uint16_t u_int16;       /**< Represents a 16-bit unsigned number. */
typedef uint32_t u_int32;       /**< Represents a 32-bit unsigned number. */

typedef char u_int4;            /**< Represents an unsigned number containing at least 4 bits. */
typedef char u_int3;            /**< Represents an unsigned number containing at least 3 bits. */

typedef unsigned char* _domain;        /**< Represents a binary domain name. */
typedef const unsigned char* _cdomain; /**< Const domainname */

#endif /* __POSLIB_TYPES_H */
