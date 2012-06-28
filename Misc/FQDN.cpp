/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>                             
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

#include <stdlib.h>
#include <string.h>
#include "FQDN.h"
#include "Portable.h"

/// @todo: TFQDN should be implemented using SPtr()

using namespace std;

TFQDN::TFQDN() {
    this->Duid_ = 0;
    this->Addr_ = 0;
    this->Name_ = "";
    this->Used_ = false;
}

TFQDN::TFQDN(TDUID *duid, const std::string& name, bool used) {
    this->Addr_ = 0;
    this->Duid_ = duid;
    this->Name_ = name;
    this->Used_ = used;
}

TFQDN::TFQDN(TIPv6Addr *addr, const std::string& name, bool used) {
    this->Duid_ = 0;
    this->Addr_ = addr;
    this->Name_ = name;
    this->Used_ = used;
}

TFQDN::TFQDN(const std::string& name, bool used) {
    this->Duid_ = 0;
    this->Addr_ = 0;
    this->Name_ = name;
    this->Used_ = used;
}

TDUID* TFQDN::getDuid() {
    return Duid_;
}

TIPv6Addr* TFQDN::getAddr() {
    return Addr_;
}

string TFQDN::getName() {
    return Name_;
}

bool TFQDN::isUsed() {
    return Used_;
}

void TFQDN::setUsed(bool used) {
    Used_ = used;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out, TFQDN& fqdn) {
  out << "<fqdn";
  if (fqdn.Duid_) {
    out << " duid=\"" << fqdn.Duid_->getPlain() << "\"";
  }
  if (fqdn.Addr_) {
    out << " addr=\"" << fqdn.Addr_->getPlain() << "\"";
  }

  out << " used=\"" << (fqdn.Used_?"TRUE":"FALSE") << "\">" << fqdn.getName() << "</fqdn>" << endl;
  return out;
}
