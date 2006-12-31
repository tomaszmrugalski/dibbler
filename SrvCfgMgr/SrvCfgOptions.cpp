/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgOptions.cpp,v 1.1 2006-12-31 11:46:09 thomson Exp $
 *
 */

#include "SrvCfgOptions.h"

TSrvCfgOptions::TSrvCfgOptions() {

    this->DNSServerSupport        = false;
    this->DomainSupport           = false;
    this->NTPServerSupport        = false;
    this->TimezoneSupport         = false;
    this->SIPServerSupport        = false;
    this->SIPDomainSupport        = false;
    this->NISServerSupport        = false;
    this->NISDomainSupport        = false;
    this->NISPServerSupport       = false;
    this->NISPDomainSupport       = false;
    this->LifetimeSupport         = false;
}

// --------------------------------------------------------------------
// --- options --------------------------------------------------------
// --------------------------------------------------------------------
// --- option: DNS servers ---
void TSrvCfgOptions::setDNSServerLst(List(TIPv6Addr) *lst) {
    this->DNSServerLst = *lst;
    this->DNSServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgOptions::getDNSServerLst() {
    return &this->DNSServerLst;
}
bool TSrvCfgOptions::supportDNSServer(){
    return this->DNSServerSupport;
}

// --- option: DOMAIN ---
void TSrvCfgOptions::setDomainLst(List(string) * lst) {
    this->DomainLst = *lst;
    this->DomainSupport = true;
}
List(string) * TSrvCfgOptions::getDomainLst() {
    return &this->DomainLst;
}
bool TSrvCfgOptions::supportDomain(){
    return this->DomainSupport;
}

// --- option: NTP-SERVERS ---
void TSrvCfgOptions::setNTPServerLst(List(TIPv6Addr) * lst) {
    this->NTPServerLst = *lst;
    this->NTPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgOptions::getNTPServerLst() {
    return &this->NTPServerLst;
}
bool TSrvCfgOptions::supportNTPServer(){
    return this->NTPServerSupport;
}

// --- option: TIMEZONE ---
void TSrvCfgOptions::setTimezone(string timezone) {
    this->Timezone=timezone;
    this->TimezoneSupport = true;
}
string TSrvCfgOptions::getTimezone() {
    return this->Timezone;
}
bool TSrvCfgOptions::supportTimezone(){
    return this->NTPServerSupport;
}

// --- option: SIP server ---
void TSrvCfgOptions::setSIPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->SIPServerLst = *lst;
    this->SIPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgOptions::getSIPServerLst() {
    return &this->SIPServerLst;
}
bool TSrvCfgOptions::supportSIPServer(){
    return this->SIPServerSupport;
}

// --- option: SIP domain ---
List(string) * TSrvCfgOptions::getSIPDomainLst() { 
    return &this->SIPDomainLst;
}
void TSrvCfgOptions::setSIPDomainLst(List(string) * domain) { 
    this->SIPDomainLst = *domain;
    this->SIPDomainSupport = true;
}
bool TSrvCfgOptions::supportSIPDomain() {
    return this->SIPDomainSupport;
}

// --- option: NIS server ---
void TSrvCfgOptions::setNISServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISServerLst     = *lst;
    this->NISServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgOptions::getNISServerLst() {
    return &this->NISServerLst;
}
bool TSrvCfgOptions::supportNISServer(){
    return this->NISServerSupport;
}

// --- option: NIS domain ---
void TSrvCfgOptions::setNISDomain(string domain) { 
    this->NISDomain=domain;
    this->NISDomainSupport=true;
}
string TSrvCfgOptions::getNISDomain() { 
    return this->NISDomain;
}
bool TSrvCfgOptions::supportNISDomain() {
    return this->NISDomainSupport;
}

// --- option: NIS+ server ---
void TSrvCfgOptions::setNISPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISPServerLst = *lst;
    this->NISPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgOptions::getNISPServerLst() {
    return &this->NISPServerLst;
}
bool TSrvCfgOptions::supportNISPServer(){
    return this->NISPServerSupport;
}

// --- option: NIS+ domain ---
void TSrvCfgOptions::setNISPDomain(string domain) { 
    this->NISPDomain=domain;
    this->NISPDomainSupport=true;
}
string TSrvCfgOptions::getNISPDomain() { 
    return this->NISPDomain;
}
bool TSrvCfgOptions::supportNISPDomain() {
    return this->NISPDomainSupport;
}

// --- option: LIFETIME ---
void TSrvCfgOptions::setLifetime(unsigned int x) {
    this->Lifetime = x;
    this->LifetimeSupport = true;
}
unsigned int TSrvCfgOptions::getLifetime() {
    return this->Lifetime;
}

bool TSrvCfgOptions::supportLifetime() {
    return this->LifetimeSupport;
}

// --- option: VENDOR-SPEC INFO ---
bool TSrvCfgOptions::supportVendorSpec() {
    if (this->VendorSpec.count())
	return true;
    return false;
}

SPtr<TSrvOptVendorSpec> TSrvCfgOptions::getVendorSpec(int vendor) {
    SPtr<TSrvOptVendorSpec> x = 0;
    if (!VendorSpec.count())
	return 0;
    
    // enterprise number not specified => return first one
    VendorSpec.first();
    if (vendor==0)
	return VendorSpec.get();

    // search for requested enterprise number
    while (x=VendorSpec.get()) {
	if (x->getVendor()==vendor)
	    return x;
    }

    // enterprise number not found, return first one
    VendorSpec.first();
    return VendorSpec.get();
}

void TSrvCfgOptions::setVendorSpec(List(TSrvOptVendorSpec) vendor) {
    this->VendorSpec = vendor;
}
