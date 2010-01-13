/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 only licence                                */

#include <string.h>
#include "long128.h"

ulong128::ulong128()
{
    memset(bytes,0,16);
}

ulong128::ulong128(SPtr<TIPv6Addr> addr)
{
    memcpy(this->bytes,addr->getAddr(),16);
}

ulong128 ulong128::operator+(ulong128& other)
{
    ulong128 ret1;
    memcpy(ret1.bytes,this->bytes,16);

    unsigned int c=0;
    for (int j=0;j<16;j++)
    {
        unsigned int i=c+(unsigned int)other.bytes[j]+
                         (unsigned int)this->bytes[j];
        ret1.bytes[j]=(i&0xFF);
        c=i>>8;
    }

    return ret1;
}

