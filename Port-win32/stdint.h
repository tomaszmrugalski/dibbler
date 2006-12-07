/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * dummy wrapper for WIN32 compilation
 *
 * $Id: stdint.h,v 1.1 2006-12-07 21:57:10 thomson Exp $
 */

#ifdef WIN32

/* uint64_t, uint_t definitions */
#include "Portable.h"

typedef unsigned long uint32_t;


#endif