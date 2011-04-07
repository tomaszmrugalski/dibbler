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

SPtr<TIPv6Addr> TClntCfgPrefix::get() {
    return Prefix;
}

unsigned long TClntCfgPrefix::getValid() {
    return Valid;
}

unsigned long TClntCfgPrefix::getPref() {
    return Pref;
}


TClntCfgPrefix::TClntCfgPrefix(SPtr<TIPv6Addr> prefix, 
                               unsigned long valid, 
                               unsigned long pref, 
                               unsigned char len)
    :Prefix(prefix), Valid(valid), Pref(pref), PrefixLength(len)
{
}

TClntCfgPrefix::~TClntCfgPrefix() {

}

void TClntCfgPrefix::setOptions(SPtr<TClntParsGlobalOpt> opt) 
{
    this->Valid=opt->getValid();
    this->Pref=opt->getPref();
}

TClntCfgPrefix::TClntCfgPrefix() 
{
    this->Valid=ULONG_MAX;
    this->Pref=ULONG_MAX;
    Prefix = new TIPv6Addr();
    PrefixLength = 0;
}

TClntCfgPrefix::TClntCfgPrefix(SPtr<TIPv6Addr> prefix, 
                               unsigned char len)
    :Prefix(prefix), Valid(ULONG_MAX), Pref(ULONG_MAX), PrefixLength(len)
{
}

ostream& operator<<(ostream& out,TClntCfgPrefix& pref) {
    out << "<prefix length=\"" << (unsigned int)pref.PrefixLength << "\" preferred=\"" << pref.Pref 
	<< "\" valid=\"" << pref.Valid << "\">";
    out << *pref.Prefix << "</prefix>" << std::endl;	
    return out;
}
