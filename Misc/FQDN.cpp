/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>                             
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: FQDN.cpp,v 1.7 2008-08-29 00:07:30 thomson Exp $
 *
 */

#include <stdlib.h>
#include <string.h>
#include "FQDN.h"
#include "Portable.h"

/// @todo: TFQDN should be implemented using SPtr()


TFQDN::TFQDN() {
    this->Duid = 0;
    this->Addr = 0;
    this->Name = "";
    this->used = false;
}

TFQDN::TFQDN(TDUID *duid_, string name_,bool used_) {
    this->Addr = 0;
    this->Duid = duid_;
    this->Name = name_;
    this->used = used_;
}

TFQDN::TFQDN(TIPv6Addr *addr_, string name_,bool used_) {
    this->Duid = 0;
    this->Addr = addr_;
    this->Name = name_;
    this->used = used_;
}

TFQDN::TFQDN(string name_,bool used_) {
    this->Duid = 0;
    this->Addr = 0;
    this->Name = name_;
    this->used = used_;
}

TDUID TFQDN::getDuid() {
    return *this->Duid;
}

TIPv6Addr TFQDN::getAddr() {
    return *this->Addr;
}

string TFQDN::getName() {
    return this->Name;
}

bool TFQDN::isUsed() {
    return this->used;
}

void TFQDN::setUsed(bool used) {
    this->used = used;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out, TFQDN& fqdn) {
  out << "<fqdn";
  if (fqdn.Duid) {
    out << " duid=\"" << fqdn.Duid->getPlain() << "\"";
  }
  if (fqdn.Addr) {
    out << " addr=\"" << fqdn.Addr->getPlain() << "\"";
  }

  out << " used=\"" << (fqdn.used?"TRUE":"FALSE") << "\">" << fqdn.getName() << "</fqdn>" << endl;
  return out;
}
