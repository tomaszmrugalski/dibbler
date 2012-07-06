/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>                             
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

#include <string>
#include "FQDN.h"
#include "SmartPtr.h"

using namespace std;

TFQDN::TFQDN() {
    this->Name_ = "";
    this->Used_ = false;
}

TFQDN::TFQDN(SPtr<TDUID> duid, const std::string& name, bool used)
    :Duid_(duid), Name_(name), Used_(used) {
}

TFQDN::TFQDN(SPtr<TIPv6Addr> addr, const std::string& name, bool used)
    :Addr_(addr), Name_(name), Used_(used) {
}

TFQDN::TFQDN(const std::string& name, bool used)
    :Name_(name), Used_(used) {
}

SPtr<TDUID> TFQDN::getDuid() {
    return Duid_;
}

SPtr<TIPv6Addr> TFQDN::getAddr() {
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
