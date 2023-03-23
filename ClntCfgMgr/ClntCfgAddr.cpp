/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntCfgAddr.cpp,v 1.5 2008-08-29 00:07:27 thomson Exp $
 *
 */

#include "ClntCfgAddr.h"
#include "DHCPConst.h"
#include "Logger.h"
#include <iomanip>
#include <iostream>
#include <limits.h>

SPtr<TIPv6Addr> TClntCfgAddr::get() { return Addr; }

unsigned long TClntCfgAddr::getValid() { return Valid; }

unsigned long TClntCfgAddr::getPref() { return Pref; }

TClntCfgAddr::TClntCfgAddr(SPtr<TIPv6Addr> addr, long valid, long pref) {
  Addr = addr;
  Valid = valid;
  Pref = pref;
}

TClntCfgAddr::~TClntCfgAddr() {}

void TClntCfgAddr::setOptions(SPtr<TClntParsGlobalOpt> opt) {
  this->Valid = opt->getValid();
  this->Pref = opt->getPref();
}

TClntCfgAddr::TClntCfgAddr() {
  this->Valid = UINT_MAX;
  this->Pref = UINT_MAX;
  Addr = new TIPv6Addr();
}

TClntCfgAddr::TClntCfgAddr(SPtr<TIPv6Addr> addr) {
  Addr = addr;
  Valid = UINT_MAX;
  Pref = UINT_MAX;
}

std::ostream &operator<<(std::ostream &out, TClntCfgAddr &addr) {
  out << "<addr preferred=\"" << addr.Pref << "\" valid=\"" << addr.Valid << "\">";
  out << *addr.Addr << "</addr>" << std::endl;
  return out;
}
