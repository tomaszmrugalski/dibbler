/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntParsIfaceOpt.h"
#include "DHCPDefaults.h"
#include "Logger.h"
#include <iomanip>
#include <iostream>

using namespace std;

TClntParsIfaceOpt::TClntParsIfaceOpt()
    : TClntParsIAOpt(),
      Stateless_(false),
      Unicast(CLIENT_DEFAULT_UNICAST),
      RapidCommit(CLIENT_DEFAULT_RAPID_COMMIT),
      Timezone(""),
      FQDN(""),
      NISDomain(""),
      Lifetime(false),
      ReqDNSServer(false),
      ReqDomain(false),
      ReqNTPServer(false),
      ReqTimezone(false),
      ReqSIPServer(false),
      ReqSIPDomain(false),
      ReqFQDN(false),
      ReqNISServer(false),
      ReqNISPServer(false),
      ReqNISDomain(false),
      ReqNISPDomain(false),
      ReqLifetime(false),
      ReqPrefixDelegation(false),
      ReqVendorSpec(false) {}

bool TClntParsIfaceOpt::getStateful() { return !Stateless_; }

void TClntParsIfaceOpt::setStateful(bool state) { Stateless_ = !state; }

void TClntParsIfaceOpt::setUnicast(bool unicast) { Unicast = unicast; }

bool TClntParsIfaceOpt::getUnicast() { return this->Unicast; }

bool TClntParsIfaceOpt::getRapidCommit() { return this->RapidCommit; }
void TClntParsIfaceOpt::setRapidCommit(bool rapCom) { this->RapidCommit = rapCom; }

TClntParsIfaceOpt::~TClntParsIfaceOpt() {}

// --- option: DNS server ---

List(TIPv6Addr) * TClntParsIfaceOpt::getDNSServerLst() { return &this->DNSServerLst; }
void TClntParsIfaceOpt::setDNSServerLst(List(TIPv6Addr) * lst) {
  this->DNSServerLst = *lst;
  this->ReqDNSServer = true;
}
bool TClntParsIfaceOpt::getReqDNSServer() { return this->ReqDNSServer; }

// --- option: domain ---
List(std::string) * TClntParsIfaceOpt::getDomainLst() { return &this->DomainLst; }
void TClntParsIfaceOpt::setDomainLst(List(std::string) * domain) {
  this->DomainLst = *domain;
  this->ReqDomain = true;
}
bool TClntParsIfaceOpt::getReqDomain() { return this->ReqDomain; }

// --- option: NTP-SERVERS ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNTPServerLst() { return &this->NTPServerLst; }
void TClntParsIfaceOpt::setNTPServerLst(List(TIPv6Addr) * lst) {
  this->NTPServerLst = *lst;
  this->ReqNTPServer = true;
}
bool TClntParsIfaceOpt::getReqNTPServer() { return this->ReqNTPServer; }

// --- option: Timezone ---
void TClntParsIfaceOpt::setTimezone(const std::string &Timezone) {
  this->Timezone = Timezone;
  this->ReqTimezone = true;
}
bool TClntParsIfaceOpt::getReqTimezone() { return this->ReqTimezone; }
string TClntParsIfaceOpt::getTimezone() { return this->Timezone; }

// --- option: SIP server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getSIPServerLst() { return &this->SIPServerLst; }
void TClntParsIfaceOpt::setSIPServerLst(List(TIPv6Addr) * lst) {
  this->SIPServerLst = *lst;
  this->ReqSIPServer = true;
}
bool TClntParsIfaceOpt::getReqSIPServer() { return this->ReqSIPServer; }

// --- option: SIP domain ---
List(std::string) * TClntParsIfaceOpt::getSIPDomainLst() { return &this->SIPDomainLst; }
void TClntParsIfaceOpt::setSIPDomainLst(List(std::string) * domain) {
  this->SIPDomainLst = *domain;
  this->ReqSIPDomain = true;
}
bool TClntParsIfaceOpt::getReqSIPDomain() { return this->ReqSIPDomain; }

// --- option: FQDN ---
void TClntParsIfaceOpt::setFQDN(const std::string &fqdn) {
  this->FQDN = fqdn;
  this->ReqFQDN = true;
}

bool TClntParsIfaceOpt::getReqFQDN() { return this->ReqFQDN; }

string TClntParsIfaceOpt::getFQDN() { return this->FQDN; }

// --- option: Prefix Delegation ---
void TClntParsIfaceOpt::setPrefixDelegation() { this->ReqPrefixDelegation = true; }

bool TClntParsIfaceOpt::getReqPrefixDelegation() { return this->ReqPrefixDelegation; }

// --- option: NIS server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNISServerLst() { return &this->NISServerLst; }

void TClntParsIfaceOpt::setNISServerLst(List(TIPv6Addr) * lst) {
  this->NISServerLst = *lst;
  this->ReqNISServer = true;
}

bool TClntParsIfaceOpt::getReqNISServer() { return this->ReqNISServer; }

// --- option: NIS domain ---
string TClntParsIfaceOpt::getNISDomain() { return this->NISDomain; }

void TClntParsIfaceOpt::setNISDomain(const std::string &domain) {
  this->NISDomain = domain;
  this->ReqNISDomain = true;
}

bool TClntParsIfaceOpt::getReqNISDomain() { return this->ReqNISDomain; }

// --- option: NIS+ server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNISPServerLst() { return &this->NISPServerLst; }

void TClntParsIfaceOpt::setNISPServerLst(List(TIPv6Addr) * lst) {
  this->NISPServerLst = *lst;
  this->ReqNISPServer = true;
}

bool TClntParsIfaceOpt::getReqNISPServer() { return this->ReqNISPServer; }

// --- option: NIS+ domain ---
string TClntParsIfaceOpt::getNISPDomain() { return this->NISPDomain; }
void TClntParsIfaceOpt::setNISPDomain(const std::string &domain) {
  this->NISPDomain = domain;
  this->ReqNISPDomain = true;
}
bool TClntParsIfaceOpt::getReqNISPDomain() { return this->ReqNISPDomain; }

// --- option: Lifetime ---

bool TClntParsIfaceOpt::getLifetime() { return this->Lifetime; }
void TClntParsIfaceOpt::setLifetime() {
  this->Lifetime = true;
  this->ReqLifetime = true;
}
bool TClntParsIfaceOpt::getReqLifetime() { return this->ReqLifetime; }

// --- option: vendor-spec ---
void TClntParsIfaceOpt::setVendorSpec(List(TOptVendorSpecInfo) vendorSpec) {
  VendorSpec = vendorSpec;
  ReqVendorSpec = true;
}

void TClntParsIfaceOpt::setVendorSpec() { ReqVendorSpec = true; }

bool TClntParsIfaceOpt::getReqVendorSpec() { return ReqVendorSpec; }

List(TOptVendorSpecInfo) TClntParsIfaceOpt::getVendorSpec() { return VendorSpec; }
