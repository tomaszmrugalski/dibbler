/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntCfgAddr.cpp,v 1.4 2005-11-03 21:23:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 *
 */

#include <iostream>
#include <iomanip>
#include <limits.h>
#include "DHCPConst.h"
#include "ClntCfgAddr.h"
#include "Logger.h"

SmartPtr<TIPv6Addr> TClntCfgAddr::get() {
    return Addr;
}

unsigned long TClntCfgAddr::getValid() {
    return Valid;
}

unsigned long TClntCfgAddr::getPref() {
    return Pref;
}


TClntCfgAddr::TClntCfgAddr(SmartPtr<TIPv6Addr> addr,long valid,long pref)
{
    Addr=addr;
    Valid = valid;
    Pref = pref;
}

TClntCfgAddr::~TClntCfgAddr() {

}

void TClntCfgAddr::setOptions(SmartPtr<TClntParsGlobalOpt> opt) {
    this->Valid=opt->getValid();
    this->Pref=opt->getPref();
}

TClntCfgAddr::TClntCfgAddr() {
    this->Valid=ULONG_MAX;
    this->Pref=ULONG_MAX;
    Addr= new TIPv6Addr();
}

TClntCfgAddr::TClntCfgAddr(SmartPtr<TIPv6Addr> addr) {
    Addr=addr;
    Valid = ULONG_MAX;
    Pref = ULONG_MAX;
}

ostream& operator<<(ostream& out,TClntCfgAddr& addr) {
    out << "<addr preferred=\"" << addr.Pref 
	<< "\" valid=\"" << addr.Valid << "\">";
    out << *addr.Addr << "</addr>" << std::endl;	
    return out;
}
