/*
    Posadis - A DNS Server
    Universal include file for string functions, since different OS'ses use different directories
    Copyright (C) 2001  Meilof Veeningen <meilof@users.sourceforge.net>

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

#ifndef __POSLIB_SYSSTL_H
#define __POSLIB_SYSSTL_H

#include "config.h"

#include "sysstring.h"

#include <list>

#ifdef HAVE_SLIST
#  include <slist>
#else
#  ifdef HAVE_EXT_SLIST
#    define HAVE_SLIST
#    include <ext/slist>
#  else
#    define slist list
#  endif
#endif

#include <string>

#ifdef _LEAKCHECK_
/* Use malloc_alloc for leak checking
 *
 * This code is _very_ platform-specific. I know this not to work on Mandrake
 * Linux using gcc 3.2, and, iirc, it doesn't work with gcc-2.96 on mdk either.
 * It does work on my debian box however, so I'll do my leak tests on that
 * one.
 */
#define stl_string basic_string<char, std::string_char_traits<char>, malloc_alloc >
#define stl_slist(type) slist<type, malloc_alloc >
#define stl_list(type) list<type, malloc_alloc >
#else
#define stl_slist(type) slist<type>
#define stl_list(type) list<type>
#define stl_string string
#endif

using namespace std;

#ifdef HAVE_EXT_SLIST
using namespace __gnu_cxx;
#endif

#endif /* __POSLIB_SYSSTL_H */
