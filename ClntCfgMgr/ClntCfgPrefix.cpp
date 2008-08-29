/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 *
 */

#include <iostream>
#include <iomanip>
#include <limits.h>
#include "DHCPConst.h"
#include "ClntCfgPrefix.h"
#include "Logger.h"

SmartPtr<TIPv6Addr> TClntCfgPrefix::get() {
    return Prefix;
}

unsigned long TClntCfgPrefix::getValid() {
    return Valid;
}

unsigned long TClntCfgPrefix::getPref() {
    return Pref;
}


TClntCfgPrefix::TClntCfgPrefix(SmartPtr<TIPv6Addr> prefix, long valid, long pref, char prefixLength)
{
    Prefix=prefix;
    Valid = valid;
    Pref = pref;
    PrefixLength = prefixLength;
}

TClntCfgPrefix::~TClntCfgPrefix() {

}

void TClntCfgPrefix::setOptions(SmartPtr<TClntParsGlobalOpt> opt) {
    this->Valid=opt->getValid();
    this->Pref=opt->getPref();
}

TClntCfgPrefix::TClntCfgPrefix() {
    this->Valid=ULONG_MAX;
    this->Pref=ULONG_MAX;
    Prefix = new TIPv6Addr();
    PrefixLength = 0;
}

TClntCfgPrefix::TClntCfgPrefix(SmartPtr<TIPv6Addr> prefix, char prefixLength) {
    Prefix=prefix;
    Valid = ULONG_MAX;
    Pref = ULONG_MAX;
    PrefixLength = prefixLength;
    
}

ostream& operator<<(ostream& out,TClntCfgPrefix& pref) {
    out << "<prefix preferred=\"" << pref.Pref 
	<< "\" valid=\"" << pref.Valid << "\">";
    out << *pref.Prefix << "</prefix>" << std::endl;	
    return out;
}
