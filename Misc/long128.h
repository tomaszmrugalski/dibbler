/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: long128.h,v 1.3 2004-03-29 22:06:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef LONG128_H
#define LONG128_H
#include "IPv6Addr.h"
#include "SmartPtr.h"

class ulong128
{
 public:
    ulong128();
    ulong128(SPtr<TIPv6Addr> addr);
    ulong128 operator+(ulong128& other);
    
 private:
    unsigned char bytes[16];
};
#endif
