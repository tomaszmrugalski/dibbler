/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgOptions.cpp,v 1.7 2008-08-29 00:07:33 thomson Exp $
 *
 */

#include "Logger.h"
#include "SrvCfgOptions.h"

TSrvCfgOptions::TSrvCfgOptions() {
    SetDefaults();
}

TSrvCfgOptions::TSrvCfgOptions(SPtr<TDUID> duid) {
    SetDefaults();
    Duid = duid;
}

TSrvCfgOptions::TSrvCfgOptions(SPtr<TSrvOptRemoteID> remoteID) {
    SetDefaults();
    RemoteID = remoteID;
}

SPtr<TDUID> TSrvCfgOptions::getDuid() {
    return Duid;
}

SPtr<TSrvOptRemoteID> TSrvCfgOptions::getRemoteID() {
    return RemoteID;
}

void TSrvCfgOptions::SetDefaults() {
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
    this->VendorSpecSupport       = false;

    Duid = 0;
    RemoteID = 0;
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

void TSrvCfgOptions::setAddr(SPtr<TIPv6Addr> addr) {
    Addr = addr;
}

SPtr<TIPv6Addr> TSrvCfgOptions::getAddr() {
    return Addr;
}

void TSrvCfgOptions::setVendorSpec(List(TSrvOptVendorSpec) vendor) {
    this->VendorSpec = vendor;
}

void TSrvCfgOptions::setExtraOptions(List(TSrvOptGeneric) extra)
{
    Log(Debug) << "Setting " << extra.count() << " generic option(s)." << LogEnd;
    ExtraOpts = extra;
}

List(TSrvOptGeneric) TSrvCfgOptions::getExtraOptions()
{
    return ExtraOpts;
}


bool TSrvCfgOptions::setOptions(SPtr<TSrvParsGlobalOpt> opt)
{
    if (opt->supportDNSServer())  this->setDNSServerLst(opt->getDNSServerLst());
    if (opt->supportDomain())     this->setDomainLst(opt->getDomainLst());
    if (opt->supportNTPServer())  this->setNTPServerLst(opt->getNTPServerLst());
    if (opt->supportTimezone())   this->setTimezone(opt->getTimezone());
    if (opt->supportSIPServer())  this->setSIPServerLst(opt->getSIPServerLst());
    if (opt->supportSIPDomain())  this->setSIPDomainLst(opt->getSIPDomainLst());
    if (opt->supportNISServer())  this->setNISServerLst(opt->getNISServerLst());
    if (opt->supportNISDomain())  this->setNISDomain(opt->getNISDomain());
    if (opt->supportNISPServer()) this->setNISPServerLst(opt->getNISPServerLst());
    if (opt->supportNISPDomain()) this->setNISPDomain(opt->getNISPDomain());
    if (opt->supportLifetime())   this->setLifetime(opt->getLifetime());
    if (opt->supportVendorSpec()) this->setVendorSpec(opt->getVendorSpec());
    if (opt->getTunnelMode())     
    {
	VendorSpecSupport = true;
	this->VendorSpec.append(opt->getTunnelVendorSpec());
    }
    setExtraOptions(opt->getExtraOptions());
    return true;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
ostream& operator<<(ostream& out,TSrvCfgOptions& iface) {

    SPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

    out << "    <client>" << endl;
    if (iface.Duid)
	out << "      " << *iface.Duid;
    else
	out << "      <!-- <duid/> -->" << endl;
    if (iface.RemoteID)
	out << "      <RemoteID enterprise=\"" << iface.RemoteID->getVendor() << "\" length=\""
	    << iface.RemoteID->getVendorDataLen() << "\">" << iface.RemoteID->getVendorDataPlain() << "</RemoteID>" << endl;
    else
	out << "      <!-- <RemoteID/> -->" << endl;

    out << "      <!-- paramteres start here -->" << endl;

    if (iface.Addr) {
	out << "      <addr>" << iface.Addr->getPlain() << "</addr>" << endl;
    } else {
	out << "      <!-- <addr/> -->" << endl;
    }

    // option: DNS-SERVERS
    out << "      <!-- <dns-servers count=\"" << iface.DNSServerLst.count() << "\"> -->" << endl;
    iface.DNSServerLst.first();
    while (addr = iface.DNSServerLst.get()) {
        out << "      <dns-server>" << *addr << "</dns-server>" << endl;
    }

    // option: DOMAINS
    out << "      <!-- <domains count=\"" << iface.DomainLst.count() << "\"> -->" << endl;
    iface.DomainLst.first();
    while (str = iface.DomainLst.get()) {
        out << "      <domain>" << *str << "</domain>" << endl;
    }

    // NTP-SERVERS
    out << "      <!-- <ntp-servers count=\"" << iface.NTPServerLst.count() << "\"> -->" << endl;
    iface.NTPServerLst.first();
    while (addr = iface.NTPServerLst.get()) {
        out << "      <ntp-server>" << *addr << "</ntp-server>" << endl;
    }

    // option: TIMEZONE
    if (iface.supportTimezone()) {
        out << "      <timezone>" << iface.Timezone << "</timezone>" << endl;
    } else {
        out << "      <!-- <timezone/> -->" << endl;
    }

    // option: SIP-SERVERS
    out << "      <!-- <sip-servers count=\"" << iface.SIPServerLst.count() << "\"> -->" << endl;
    iface.SIPServerLst.first();
    while (addr = iface.SIPServerLst.get()) {
        out << "      <sip-server>" << *addr << "</sip-server>" << endl;
    }

    // option: SIP-DOMAINS
    out << "      <!-- <sip-domains count=\"" << iface.SIPDomainLst.count() << "\"> -->" << endl;
    iface.SIPDomainLst.first();
    while (str = iface.SIPDomainLst.get()) {
        out << "      <sip-domain>" << *str << "</sip-domain>" << endl;
    }

    // option: NIS-SERVERS
    out << "      <!-- <nis-servers count=\"" << iface.NISServerLst.count() << "\"> -->" << endl;
    iface.NISServerLst.first();
    while (addr = iface.NISServerLst.get()) {
        out << "      <nis-server>" << *addr << "</nis-server>" << endl;
    }

    // option: NIS-DOMAIN
    if (iface.supportNISDomain()) {
        out << "      <nis-domain>" << iface.NISDomain << "</nis-domain>" << endl;
    } else {
        out << "      <!-- <nis-domain/> -->" << endl;
    }

    // option: NIS+-SERVERS
    out << "      <!-- <nisplus-servers count=\"" << iface.NISPServerLst.count() << "\"> -->" << endl;
    iface.NISPServerLst.first();
    while (addr = iface.NISPServerLst.get()) {
        out << "      <nisplus-server>" << *addr << "</nisplus-server>" << endl;
    }

    // option: NIS+-DOMAIN
    if (iface.supportNISPDomain()) {
        out << "      <nisplus-domain>" << iface.NISPDomain << "</nisplus-domain>" << endl;
    } else {
        out << "      <!-- <nisplus-domain/> -->" << endl;
    }

    // option: LIFETIME
    if (iface.supportLifetime()) {
        out << "      <lifetime>" << iface.Lifetime << "</lifetime>" << endl;
    } else {
        out << "      <!-- <lifetime/> -->" << endl;
    }


    // option: VENDOR-SPEC
    if (iface.supportVendorSpec()) {
	out << "      <vendorSpecList count=\"" << iface.VendorSpec.count() << "\">" << endl;
	iface.VendorSpec.first();
        SPtr<TSrvOptVendorSpec> v;
	while (v = iface.VendorSpec.get()) {
	    out << "        <vendorSpec vendor=\"" << v->getVendor() << "\" length=\"" << v->getVendorDataLen() 
	    << "\">" << v->getVendorDataPlain() << "</vendorSpec>" << endl;
	}
	out << "      </vendorSpecList>" << endl;
    } else {
	out << "      <!-- <vendorSpec/> -->" << endl;
    }

    out << "    </client>" << endl;

    return out;
}
