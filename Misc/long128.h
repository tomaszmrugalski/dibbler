/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef LONG128_H_
#define LONG128_H_
#include "IPv6Addr.h"
#include "SmartPtr.h"

class ulong128
{
public:
    ulong128();
    ulong128(SmartPtr<TIPv6Addr> addr);
    ulong128 operator+(ulong128& other);

private:
    unsigned char bytes[16];
};
#endif
