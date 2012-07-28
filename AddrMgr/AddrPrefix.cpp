/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <iostream>
#include <time.h>

#include "Portable.h"
#include "AddrPrefix.h"
#include "DHCPConst.h"
#include "Logger.h"

using namespace std;

TAddrPrefix::TAddrPrefix(SPtr<TIPv6Addr> prefix, long pref, long valid, int length)
    :TAddrAddr(prefix, pref, valid)
{
    this->Length = length;
}

int TAddrPrefix::getLength() {
    return this->Length;
}

ostream & operator<<(ostream & strum,TAddrPrefix &x) {
    strum << "<AddrPrefix"
	
	  << " timestamp=\"" << x.Timestamp << "\""
	  << " pref=\"" << x.Prefered <<"\""
	  << " valid=\"" << x.Valid <<  "\""
	  << " length=\"" << x.Length << "\""
	  << ">" << x.Addr->getPlain()<< "</AddrPrefix>" << std::endl;
    return strum;
}

