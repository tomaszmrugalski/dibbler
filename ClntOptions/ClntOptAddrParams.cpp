/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAddrParams.cpp,v 1.1.2.1 2007-04-15 21:23:31 thomson Exp $
 *
 */

#include "ClntOptAddrParams.h"
#include "DHCPConst.h"

TClntOptAddrParams::TClntOptAddrParams(char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_ADDRPARAMS, 2, buf, n, parent)
{

}

int TClntOptAddrParams::getPrefix()
{
    return (Value >> 8) & 0xff;
}

int TClntOptAddrParams::getBitfield()
{
    return Value & 0xff;
}

bool TClntOptAddrParams::doDuties() 
{
    return true;
}
