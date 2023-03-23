/*
    Posadis - A DNS Server
    Universal include file for string functions
    Copyright (C) 2002 Meilof Veeningen <meilof@users.sourceforge.net>

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

#ifndef __POSLIB_SYSSTRING_H
#define __POSLIB_SYSSTRING_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* linux & bloodshed string comp functions have other names than the win32 ones
 */
#if !defined(__BORLANDC__) && !defined(WIN32)
#define stricmp strcasecmp
#define strcmpi strcasecmp
#define strncmpi strncasecmp
#endif

#ifdef WIN32
#define strncmpi _strnicmp
#endif

#include "vsnprintf.h"

#endif /* __POSLIB_SYSSTRING_H */
