/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptAddrParams.cpp,v 1.3 2008-08-29 00:07:35 thomson Exp $
 *
 */

#include "SrvOptAddrParams.h"
#include "DHCPConst.h"

TSrvOptAddrParams::TSrvOptAddrParams(int prefix, int bitfield, TMsg * parent)
    :TOptInteger(OPTION_ADDRPARAMS, 2, 0, parent)
{
    if (prefix>128)
	prefix = 128;
    if (prefix<0)
	prefix = 0;
    int value = (prefix << 8) + (bitfield & 0xff);
    
    Value = value;
}


TSrvOptAddrParams::TSrvOptAddrParams(char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_ADDRPARAMS, 2, buf, n, parent)
{

}

int TSrvOptAddrParams::getPrefix()
{
    return (Value >> 8) & 0xff;
}

int TSrvOptAddrParams::getBitfield()
{
    return Value & 0xff;
}

bool TSrvOptAddrParams::doDuties() 
{
    return true;
}
