/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                         
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.cpp,v 1.20 2006-11-03 22:23:15 thomson Exp $
 *
 */

#include <iostream>
#include <iomanip>
#include "ClntCfgIface.h"
#include "Logger.h"
#include "Portable.h"
#include "ClntOptVendorSpec.h"
using namespace std;

TClntCfgIface::TClntCfgIface(string ifaceName) {
    NoConfig=false;
    IfaceName=ifaceName;
    ID=-1;
    this->DNSServerState  = DISABLED;
    this->DomainState     = DISABLED;
    this->NTPServerState  = DISABLED;
    this->TimezoneState   = DISABLED;
    this->SIPServerState  = DISABLED;
    this->SIPDomainState  = DISABLED;
    this->FQDNState       = DISABLED;
    this->NISServerState  = DISABLED;
    this->NISPServerState = DISABLED;
    this->NISDomainState  = DISABLED;
    this->NISPDomainState = DISABLED;
    this->LifetimeState   = DISABLED;
    this->PrefixDelegationState = DISABLED;
    this->VendorSpecState = DISABLED;
}

TClntCfgIface::TClntCfgIface(int ifaceNr) {
    NoConfig=false;
    ID=ifaceNr;
    IfaceName="[unknown]";
    this->DNSServerState  = DISABLED;
    this->DomainState     = DISABLED;
    this->NTPServerState  = DISABLED;
    this->TimezoneState   = DISABLED;
    this->SIPServerState  = DISABLED;
    this->SIPDomainState  = DISABLED;
    this->FQDNState       = DISABLED;
    this->NISServerState  = DISABLED;
    this->NISPServerState = DISABLED;
    this->NISDomainState  = DISABLED;
    this->NISPDomainState = DISABLED;
    this->LifetimeState   = DISABLED;
    this->PrefixDelegationState = DISABLED;
    this->VendorSpecState = DISABLED;
}

void TClntCfgIface::setOptions(SmartPtr<TClntParsGlobalOpt> opt) {
    this->isIA        = opt->getIsIAs();
    this->Unicast     = opt->getUnicast();
    this->RapidCommit = opt->getRapidCommit();

    // copy YES/NO information
    ReqDNSServer = opt->getReqDNSServer();
    ReqDomain    = opt->getReqDomain();
    ReqNTPServer = opt->getReqNTPServer();
    ReqTimezone  = opt->getReqTimezone();
    ReqSIPServer = opt->getReqSIPServer();
    ReqSIPDomain = opt->getReqSIPDomain();
    ReqFQDN      = opt->getReqFQDN();
    ReqNISServer = opt->getReqNISServer();
    ReqNISDomain = opt->getReqNISDomain();
    ReqNISPServer= opt->getReqNISPServer();
    ReqNISPDomain= opt->getReqNISPDomain();
    ReqLifetime  = opt->getReqLifetime();
    ReqPrefixDelegation = opt->getReqPrefixDelegation();


    // copy parameters
    this->DNSServerLst = *opt->getDNSServerLst();
    this->DomainLst    = *opt->getDomainLst();
    this->Timezone     = opt->getTimezone();
    this->NTPServerLst = *opt->getNTPServerLst();
    this->SIPServerLst = *opt->getSIPServerLst();
    this->SIPDomainLst = *opt->getSIPDomainLst();
    this->FQDN         = opt->getFQDN();
    this->NISServerLst = *opt->getNISServerLst();
    this->NISDomain    = opt->getNISDomain();
    this->NISPServerLst= *opt->getNISPServerLst();
    this->NISPDomain   = opt->getNISPDomain();
    

    if (ReqDNSServer)  this->setDNSServerState(NOTCONFIGURED);
    if (ReqDomain)     this->setDomainState(NOTCONFIGURED);
    if (ReqNTPServer)  this->setNTPServerState(NOTCONFIGURED);
    if (ReqTimezone)   this->setTimezoneState(NOTCONFIGURED);
    if (ReqSIPServer)  this->setSIPServerState(NOTCONFIGURED);
    if (ReqSIPDomain)  this->setSIPDomainState(NOTCONFIGURED);
    if (ReqFQDN)       this->setFQDNState(NOTCONFIGURED);
    if (ReqNISServer)  this->setNISServerState(NOTCONFIGURED);
    if (ReqNISDomain)  this->setNISDomainState(NOTCONFIGURED);
    if (ReqNISPServer) this->setNISPServerState(NOTCONFIGURED);
    if (ReqNISPDomain) this->setNISPDomainState(NOTCONFIGURED);
    if (ReqLifetime)   this->setLifetimeState(NOTCONFIGURED);
    if (ReqPrefixDelegation) this->setPrefixDelegationState(NOTCONFIGURED);
    // copy preferred-server list
    SmartPtr<TStationID> station;
    opt->firstPrefSrv();
    while (station = opt->getPrefSrv())
	this->PrefSrvLst.append(station);

    // copy rejected-server list
    opt->firstRejedSrv();
    while (station = opt->getRejedSrv())
	this->RejectedSrvLst.append(station);
}

bool TClntCfgIface::isServerRejected(SmartPtr<TIPv6Addr> addr,SmartPtr<TDUID> duid)
{
    this->RejectedSrvLst.first();
    SmartPtr<TStationID> RejectedSrv;
    while(RejectedSrv=RejectedSrvLst.get())
    {
        if (((*RejectedSrv)==addr)||((*RejectedSrv)==duid))
            return true;
    }
    return false;
}

void TClntCfgIface::firstTA() {
    this->ClntCfgTALst.first();
}

SmartPtr<TClntCfgTA> TClntCfgIface::getTA() {
    return this->ClntCfgTALst.get();
}

void  TClntCfgIface::addTA(SmartPtr<TClntCfgTA> ta) {
    this->ClntCfgTALst.append(ta);
}

int TClntCfgIface::countTA()
{
    return ClntCfgTALst.count();
}

void TClntCfgIface::firstIA()
{
    IALst.first();
}

int TClntCfgIface::countIA()
{
    return IALst.count();
}

 SmartPtr<TClntCfgIA> TClntCfgIface::getIA()
{
    return IALst.get();
}

 void TClntCfgIface::addIA(SmartPtr<TClntCfgIA> ptr)
{
    IALst.append(ptr);
}

void TClntCfgIface::firstPD()
{
    PDLst.first();
}

int TClntCfgIface::countPD()
{
    return PDLst.count();
}

 SmartPtr<TClntCfgPD> TClntCfgIface::getPD()
{
    return PDLst.get();
}

 void TClntCfgIface::addPD(SmartPtr<TClntCfgPD> ptr)
{
    PDLst.append(ptr);
}


#if 0
SmartPtr<TClntCfgIA> TClntCfgIface::getLastIA()
{
    return IALst.getLast();
}
#endif

 string TClntCfgIface::getName(void)
{
    return IfaceName;    
}

string TClntCfgIface::getFullName() {
    ostringstream oss;
    oss << this->ID;
    return string(this->IfaceName)
	+"/"
	+oss.str();
}


int TClntCfgIface::getID(void)
{
    return ID;
}

void TClntCfgIface::setIfaceID(int ifaceID)
{
    ID=ifaceID;
}

void TClntCfgIface::setIfaceName(string ifaceName)
{
    this->IfaceName=ifaceName;
}

void TClntCfgIface::setNoConfig()
{
    NoConfig=true;
}

bool TClntCfgIface::stateless()
{
    return !this->isIA;
}

bool TClntCfgIface::noConfig() {
    return NoConfig;
}

bool TClntCfgIface::getUnicast() {
    return this->Unicast;
}

bool TClntCfgIface::getRapidCommit() {
    return this->RapidCommit;
}

void TClntCfgIface::vendorSpecSupported(bool support)
{
    this->ReqVendorSpec = support;
    this->VendorSpecState = NOTCONFIGURED;
}

void TClntCfgIface::setVendorSpec(SPtr<TClntOptVendorSpec> vendorSpec)
{
    this->VendorSpec = vendorSpec;
    this->ReqVendorSpec = true;
    this->VendorSpecState = NOTCONFIGURED;
}

// --------------------------------------------------------------------------------
// --- options below --------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TClntCfgIface::isReqDNSServer() {
    return this->ReqDNSServer;
}
bool TClntCfgIface::isReqPrefixDelegation() {
    return this->ReqPrefixDelegation;
}
bool TClntCfgIface::isReqDomain() {
    return this->ReqDomain;
}
bool TClntCfgIface::isReqNTPServer() {
    return this->ReqNTPServer;
}
bool TClntCfgIface::isReqTimezone() {
    return this->ReqTimezone;
}
bool TClntCfgIface::isReqSIPServer() {
    return this->ReqSIPServer;
}
bool TClntCfgIface::isReqSIPDomain() {
    return this->ReqSIPDomain;
}
bool TClntCfgIface::isReqFQDN() {
    return this->ReqFQDN;
}
bool TClntCfgIface::isReqNISServer() {
    return this->ReqNISServer;
}
bool TClntCfgIface::isReqNISDomain() {
    return this->ReqNISDomain;
}
bool TClntCfgIface::isReqNISPServer() {
    return this->ReqNISPServer;
}
bool TClntCfgIface::isReqNISPDomain() {
    return this->ReqNISPDomain;
}
bool TClntCfgIface::isReqLifetime() {
    return this->ReqLifetime;
}
bool TClntCfgIface::isReqVendorSpec() {
    return this->ReqVendorSpec;
}

// --------------------------------------------------------------------------------
// --- options: state -------------------------------------------------------------
// --------------------------------------------------------------------------------
EState TClntCfgIface::getDNSServerState() {
    return DNSServerState;
}
EState TClntCfgIface::getDomainState() {
    return DomainState;
}
EState TClntCfgIface::getNTPServerState() {
    return NTPServerState;
}
EState TClntCfgIface::getTimezoneState() {
    return TimezoneState;
}
EState TClntCfgIface::getSIPServerState() {
    return SIPServerState;
}
EState TClntCfgIface::getSIPDomainState() {
    return SIPDomainState;
}
EState TClntCfgIface::getFQDNState() {
    return FQDNState;
}
EState TClntCfgIface::getNISServerState() {
    return NISServerState;
}
EState TClntCfgIface::getNISPServerState() {
    return NISPServerState;
}
EState TClntCfgIface::getNISDomainState() {
    return NISDomainState;
}
EState TClntCfgIface::getNISPDomainState() {
    return NISPDomainState;
}
EState TClntCfgIface::getLifetimeState() {
    return LifetimeState;
}
EState TClntCfgIface::getPrefixDelegationState() {
    return PrefixDelegationState;
}
EState TClntCfgIface::getVendorSpecState() {
    return VendorSpecState;
}
// --------------------------------------------------------------------
// --- options: get option --------------------------------------------
// --------------------------------------------------------------------
List(TIPv6Addr) * TClntCfgIface::getProposedDNSServerLst() {
    return &this->DNSServerLst;
}
List(string) * TClntCfgIface::getProposedDomainLst() {
    return &this->DomainLst;
}
List(TIPv6Addr) * TClntCfgIface::getProposedNTPServerLst() {
    return &this->NTPServerLst;
}
string TClntCfgIface::getProposedTimezone() {
    return this->Timezone;
}
List(TIPv6Addr) * TClntCfgIface::getProposedSIPServerLst() {
    return &this->SIPServerLst;
}
List(string) * TClntCfgIface::getProposedSIPDomainLst() {
    return &this->SIPDomainLst;
}
string TClntCfgIface::getProposedFQDN() {
    return this->FQDN;
}
List(TIPv6Addr) * TClntCfgIface::getProposedNISServerLst() {
    return &this->NISServerLst;
}
List(TIPv6Addr) * TClntCfgIface::getProposedNISPServerLst() {
    return &this->NISPServerLst;
}
string TClntCfgIface::getProposedNISDomain() {
    return this->NISDomain;
}
string TClntCfgIface::getProposedNISPDomain() {
    return this->NISPDomain;
}
SPtr<TClntOptVendorSpec> TClntCfgIface::getVendorSpec() {
    return this->VendorSpec;
}

// --------------------------------------------------------------------
// --- options: setState ----------------------------------------------
// --------------------------------------------------------------------
void TClntCfgIface::setDNSServerState(EState state) {
    this->DNSServerState=state;
}
void TClntCfgIface::setDomainState(EState state) {
    this->DomainState=state;
}
void TClntCfgIface::setNTPServerState(EState state) {
    this->NTPServerState=state;
}
void TClntCfgIface::setTimezoneState(EState state) {
    this->TimezoneState=state;
}
void TClntCfgIface::setSIPServerState(EState state) {
    this->SIPServerState=state;
}
void TClntCfgIface::setSIPDomainState(EState state) {
    this->SIPDomainState=state;
}
void TClntCfgIface::setFQDNState(EState state) {
    this->FQDNState=state;
}
void TClntCfgIface::setNISServerState(EState state) {
    this->NISServerState=state;
}
void TClntCfgIface::setNISPServerState(EState state) {
    this->NISPServerState=state;
}
void TClntCfgIface::setNISDomainState(EState state) {
    this->NISDomainState=state;
}
void TClntCfgIface::setNISPDomainState(EState state) {
    this->NISPDomainState=state;
}
void TClntCfgIface::setLifetimeState(EState state) {
    this->LifetimeState=state;
}
void TClntCfgIface::setPrefixDelegationState(EState state) {
    this->PrefixDelegationState=state;
}
void TClntCfgIface::setVendorSpecState(EState state) {
    this->VendorSpecState = state;
}

void TClntCfgIface::setPrefixLength(int len) {
    this->PrefixLength = len;
}

int  TClntCfgIface::getPrefixLength() {
    return this->PrefixLength;
}



// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TClntCfgIface& iface)
{
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

    out << dec;
    out<<"  <ClntCfgIface ";
    if (iface.NoConfig) {
        out << "no-config=\"true\" />" << endl;
        return out;
    }

    out << "name=\"" << iface.IfaceName << "\""
        << " ifindex=\"" << iface.ID << "\">" << endl;
    
    if (iface.RapidCommit) {
	out << "    <RapidCommit/>" << endl;
    } else {
	out << "    <!-- <RapidCommit/> -->" << endl;
    }

    out << "    <!-- addresses -->" << endl;
    out << "    <iaLst count=\"" << iface.IALst.count() << "\">" << endl;
    SmartPtr<TClntCfgIA> ia;
    iface.IALst.first();
    while(ia=iface.IALst.get())
    {	
        out << *ia;
    }
    out << "    </iaLst>" << endl;

    out << "    <!-- temporary addresses -->" << endl;
    SmartPtr<TClntCfgTA> ta;
    iface.firstTA();
    while (ta = iface.getTA() ) {
	out << *ta;
    }

    out << "    <!-- options -->" << endl;

    // --- option: DNS-servers ---
    if (iface.isReqDNSServer()) {
	out << "    <dns-servers state=\"" << StateToString(iface.getDNSServerState())
	    << "\" state=\"" << iface.DNSServerLst.count() << "\" />" << endl;

	iface.DNSServerLst.first();
	while(addr=iface.DNSServerLst.get())
	    out << "      <dns-server>" << *addr << "</dns-server>" << endl;  
    } else {
	out << "    <!-- <dns-servers/> -->" << endl;
    }

    // --- option: DOMAIN ---
    if (iface.isReqDomain()) {
	out << "    <domains state=\"" << StateToString(iface.getDomainState()) 
	    << "\" hints=\"" << iface.DomainLst.count() << "\" />" << endl;

	iface.DomainLst.first();
	while (str = iface.DomainLst.get())
	    out << "      <domain>" << *str <<"</domain>" << endl;
    } else {
	out << "    <!-- <domains/> -->" << endl;
    }
	
    // --- option: NTP servers ---
    if (iface.isReqNTPServer()) {
	out << "    <ntp-servers state=\"" << StateToString(iface.getNTPServerState()) 
	    << "\" hints=\"" << iface.NTPServerLst.count() << "\" />" << endl;
	iface.NTPServerLst.first();
	while(addr=iface.NTPServerLst.get())
	    out << "      <ntp-server>" << *addr << "</ntp-server>" << endl;  
    } else {
	out << "    <!-- <ntp-servers/> -->" << endl;
    }
    
    // --- option: Timezone ---
    if (iface.isReqTimezone()) {
	out << "    <timezone state=\"" << StateToString(iface.getTimezoneState())
	    << "\">" << iface.Timezone << "</timezone>" << endl;
    } else {
	out << "    <!-- <timezone/> -->" << endl;
    }

    // --- option: SIP servers ---
    if (iface.isReqSIPServer()) {
	out << "    <sip-servers state=\"" << StateToString(iface.getSIPServerState())
	    << "\" hints=\"" << iface.NTPServerLst.count() << "\"/>" << endl;
	iface.NTPServerLst.first();
	while(addr=iface.NTPServerLst.get())
	    out << "      <sip-server>" << *addr << "</sip-server>" << endl;  
    } else {
	out << "    <!-- <sip-servers/> -->" << endl;
    }

    // --- option: SIP domains ---
    if (iface.isReqSIPDomain()) {
	out << "    <sip-domains state=\"" << StateToString(iface.getSIPDomainState())
	    << "\" hints=\"" << iface.DomainLst.count() << "\"/>" << endl;
	iface.SIPDomainLst.first();
	while (str = iface.SIPDomainLst.get())
	    out << "      <sip-domain>" << *str <<"</sip-domain>" << endl;
    } else {
	out << "    <!-- <sip-domains/> -->" << endl;
    }

    // --- option: FQDN ---
    if (iface.isReqFQDN()) {
	out << "    <fqdn state=\"" << StateToString(iface.getFQDNState()) 
	    << "\">" << iface.FQDN << "</fqdn>" << endl;
    } else {
	out << "    <!-- <fqdn/> -->" << endl;
    }

    // --- option: NIS server ---
    if (iface.isReqNISServer()) {
	out << "    <nis-servers state=\"" << StateToString(iface.getNISServerState())
	    << "\" hints=\"" << iface.NISServerLst.count() << "\"/>" << endl;
	iface.NISServerLst.first();
	while(addr=iface.NISServerLst.get())
	    out << "      <nis-server>" << *addr << "</nis-server>" << endl;  
    } else {
	out << "    <!-- <nis-servers/> -->" << endl;
    }

    // --- option: NIS domains ---
    if (iface.isReqNISDomain()) {
	out << "    <nis-domain state=\"" << StateToString(iface.getNISDomainState())
	    << "\" >" << iface.NISDomain << "</nis-domain>" << endl;
    } else {
	out << "    <!-- <nis-domain/> -->" << endl;
    }

    // --- option: NISP server ---
    if (iface.isReqNISPServer()) {
	out << "    <nisplus-servers state=\"" << StateToString(iface.getNISPServerState())
	    << "\" hints=\"" << iface.NISPServerLst.count() << "\"/>" << endl;
	iface.NISPServerLst.first();
	while(addr=iface.NISPServerLst.get())
	    out << "      <nisplus-server>" << *addr << "</nisplus-server>" << endl;  
    } else {
	out << "    <!-- <nisplus-servers/> -->" << endl;
    }

    // --- option: NISP domains ---
    if (iface.isReqNISPDomain()) {
	out << "    <nisplus-domain state=\"" << StateToString(iface.getNISPDomainState())
	    << "\" >" << iface.NISPDomain << "</nisplus-domain>" << endl;
    } else {
	out << "    <!-- <nisplus-domain> -->" << endl;
    }

    // --- option: Lifetime ---
    if (iface.isReqLifetime()) {
	out << "    <lifetime/>" << endl;
    } else {
	out << "    <!-- <lifetime/> -->" << endl;
    }

    // --- option: Vendor-spec ---
    if (iface.isReqVendorSpec()) {
	out << "    <vendorSpec";
	SPtr<TClntOptVendorSpec> opt = iface.getVendorSpec();
	if (opt) {
	    out << " vendor=\"" << opt->getVendor() << "\" length=\"" << opt->getVendorDataLen() << "\"";
	}
	out << ">";
	if (opt) {
	    out << opt->getVendorDataPlain();
	}
	out << "<vendorSpec/>" << endl;
    } else {
	out << "<!-- <vendorSpec/> -->" << endl;
    }

    out << "  </ClntCfgIface>" << endl;

    return out;
}
