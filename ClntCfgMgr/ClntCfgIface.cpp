/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include "ClntCfgIface.h"
#include "Logger.h"
#include "Portable.h"
#include "OptVendorSpecInfo.h"
using namespace std;

TClntCfgIface::TClntCfgIface(string ifaceName) {
    setDefaults();

    NoConfig=false;
    IfaceName=ifaceName;
    ID=-1;
}

TClntCfgIface::TClntCfgIface(int ifaceNr) {
    setDefaults();
    NoConfig=false;
    ID=ifaceNr;
    IfaceName="[unknown]";
}

void TClntCfgIface::setDefaults() {
    DNSServerState  = STATE_DISABLED;
    DomainState     = STATE_DISABLED;
    NTPServerState  = STATE_DISABLED;
    TimezoneState   = STATE_DISABLED;
    SIPServerState  = STATE_DISABLED;
    SIPDomainState  = STATE_DISABLED;
    FQDNState       = STATE_DISABLED;
    NISServerState  = STATE_DISABLED;
    NISPServerState = STATE_DISABLED;
    NISDomainState  = STATE_DISABLED;
    NISPDomainState = STATE_DISABLED;
    LifetimeState   = STATE_DISABLED;
    PrefixDelegationState = STATE_DISABLED;
    VendorSpecState = STATE_DISABLED;

    ExtraOpts.clear();
}

void TClntCfgIface::setOptions(SPtr<TClntParsGlobalOpt> opt) {
    isIA        = opt->getIsIAs();
    Unicast     = opt->getUnicast();
    RapidCommit = opt->getRapidCommit();

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
    ReqVendorSpec= opt->getReqVendorSpec();

    // copy parameters
    DNSServerLst = *opt->getDNSServerLst();
    DomainLst    = *opt->getDomainLst();
    Timezone     = opt->getTimezone();
    NTPServerLst = *opt->getNTPServerLst();
    SIPServerLst = *opt->getSIPServerLst();
    SIPDomainLst = *opt->getSIPDomainLst();
    FQDN         = opt->getFQDN();
    NISServerLst = *opt->getNISServerLst();
    NISDomain    = opt->getNISDomain();
    NISPServerLst= *opt->getNISPServerLst();
    NISPDomain   = opt->getNISPDomain();
    VendorSpecLst= opt->getVendorSpec();

    if (ReqDNSServer)  setDNSServerState(STATE_NOTCONFIGURED);
    if (ReqDomain)     setDomainState(STATE_NOTCONFIGURED);
    if (ReqNTPServer)  setNTPServerState(STATE_NOTCONFIGURED);
    if (ReqTimezone)   setTimezoneState(STATE_NOTCONFIGURED);
    if (ReqSIPServer)  setSIPServerState(STATE_NOTCONFIGURED);
    if (ReqSIPDomain)  setSIPDomainState(STATE_NOTCONFIGURED);
    if (ReqFQDN)       setFQDNState(STATE_NOTCONFIGURED);
    if (ReqNISServer)  setNISServerState(STATE_NOTCONFIGURED);
    if (ReqNISDomain)  setNISDomainState(STATE_NOTCONFIGURED);
    if (ReqNISPServer) setNISPServerState(STATE_NOTCONFIGURED);
    if (ReqNISPDomain) setNISPDomainState(STATE_NOTCONFIGURED);
    if (ReqLifetime)   setLifetimeState(STATE_NOTCONFIGURED);
    if (ReqVendorSpec) setVendorSpecState(STATE_NOTCONFIGURED);

    // copy preferred-server list
    SPtr<TStationID> station;
    opt->firstPrefSrv();
    while (station = opt->getPrefSrv())
	PrefSrvLst.append(station);

    // copy rejected-server list
    opt->firstRejedSrv();
    while (station = opt->getRejedSrv())
	RejectedSrvLst.append(station);
}

bool TClntCfgIface::isServerRejected(SPtr<TIPv6Addr> addr,SPtr<TDUID> duid)
{
    RejectedSrvLst.first();
    SPtr<TStationID> RejectedSrv;
    while(RejectedSrv=RejectedSrvLst.get())
    {
	if (((*RejectedSrv)==addr)||((*RejectedSrv)==duid))
	    return true;
    }
    return false;
}

void TClntCfgIface::firstTA() {
    ClntCfgTALst.first();
}

SPtr<TClntCfgTA> TClntCfgIface::getTA() {
    return ClntCfgTALst.get();
}

void  TClntCfgIface::addTA(SPtr<TClntCfgTA> ta) {
    ClntCfgTALst.append(ta);
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

 SPtr<TClntCfgIA> TClntCfgIface::getIA()
{
    return IALst.get();
}

SPtr<TClntCfgIA> TClntCfgIface::getIA(int iaid)
{
    SPtr<TClntCfgIA> ia;
    IALst.first();
    while (ia = IALst.get() ) {
	if (ia->getIAID() == iaid)
	    return ia;
    }
    return 0;
}

void TClntCfgIface::addIA(SPtr<TClntCfgIA> ptr)
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

 SPtr<TClntCfgPD> TClntCfgIface::getPD()
{
    return PDLst.get();
}

SPtr<TClntCfgPD> TClntCfgIface::getPD(int iaid)
{
    SPtr<TClntCfgPD> ia;
    PDLst.first();
    while (ia = PDLst.get() ) {
	if (ia->getIAID() == iaid)
	    return ia;
    }
    return 0;
}

void TClntCfgIface::addPD(SPtr<TClntCfgPD> ptr)
{
    PDLst.append(ptr);
}

 string TClntCfgIface::getName(void)
{
    return IfaceName;
}

string TClntCfgIface::getFullName() {
    ostringstream oss;
    oss << ID;
    return string(IfaceName)
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
    this->ReqVendorSpec   = support;
    this->VendorSpecState = STATE_NOTCONFIGURED;
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

EState TClntCfgIface::getVendorSpecState() {
    return VendorSpecState;
}
EState TClntCfgIface::getKeyGenerationState() {
    return KeyGenerationState;
}
EState TClntCfgIface::getAuthenticationState() {
    return AuthenticationState;
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

void TClntCfgIface::firstVendorSpec() {
    VendorSpecLst.first();
}

SPtr<TOptVendorSpecInfo> TClntCfgIface::getVendorSpec() {
    return this->VendorSpecLst.get();
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

void TClntCfgIface::setVendorSpecState(EState state) {
    this->VendorSpecState = state;
}

void TClntCfgIface::setVendorSpec(List(TOptVendorSpecInfo) vendorSpecList)
{
    VendorSpecLst = vendorSpecList;
}

void TClntCfgIface::setKeyGenerationState(EState state) {
    this->KeyGenerationState = state;
}

void TClntCfgIface::setAuthenticationState(EState state) {
    this->AuthenticationState = state;
}

void TClntCfgIface::setPrefixLength(int len) {
    this->PrefixLength = len;
}

int  TClntCfgIface::getPrefixLength() {
    return this->PrefixLength;
}


/**
 * add extra option to the list of supported extra options
 *
 * @param extra smart pointer to the supported option (to be sent)
 * @param sendAlways should this option be always sent? Even when already configured?
 */
void TClntCfgIface::addExtraOption(SPtr<TOpt> extra, TOpt::EOptionLayout layout, bool sendAlways) {
    SPtr<TOptionStatus> optStatus = new TOptionStatus();
    optStatus->OptionType = extra->getOptType();
    optStatus->Option = extra;
    optStatus->Always = sendAlways;
    optStatus->Layout = layout;
    ExtraOpts.push_back(optStatus);
}

/**
 * add extra option to the list of supported extra options
 *
 * @param opttype code of the supported option
 * @param layout defines layout (see TOpt::EOptionLayout)
 * @param sendAlways should this option be always sent? Even when already configured?
 */
void TClntCfgIface::addExtraOption(int opttype, TOpt::EOptionLayout layout, bool sendAlways) {
    SPtr<TOptionStatus> optStatus = new TOptionStatus();
    optStatus->OptionType = opttype;
    optStatus->Always = sendAlways;
    optStatus->Layout = layout;
    ExtraOpts.push_back(optStatus);
}

TClntCfgIface::TOptionStatusLst& TClntCfgIface::getExtraOptions() {
    return ExtraOpts;
}

SPtr<TClntCfgIface::TOptionStatus> TClntCfgIface::getExtaOptionState(int type) {
    for (TOptionStatusLst::iterator opt=ExtraOpts.begin(); opt!=ExtraOpts.end(); ++opt)
	if ((*opt)->OptionType == type)
	    return (*opt); // these are the droids you are looking for

    return 0; // not found
}


// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TClntCfgIface& iface)
{
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

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
    SPtr<TClntCfgIA> ia;
    iface.IALst.first();
    while(ia=iface.IALst.get())
    {
	out << *ia;
    }
    out << "    </iaLst>" << endl;

    out << "    <!-- temporary addresses -->" << endl;
    SPtr<TClntCfgTA> ta;
    iface.firstTA();
    while (ta = iface.getTA() ) {
	out << *ta;
    }

    out << "    <!-- options -->" << endl;

    out << "    <!-- prefix delegation -->" << endl;
    out << "    <pdLst count=\"" << iface.countPD() << "\">" << endl;
    SPtr<TClntCfgPD> pd;
    iface.firstPD();
    while (pd = iface.getPD()) {
	out << *pd;
    }
    out << "    </pdLst>" << endl;

    // --- option: DNS-servers ---
    if (iface.isReqDNSServer()) {
	out << "    <dns-servers state=\"" << StateToString(iface.getDNSServerState())
	    << "\" hints=\"" << iface.DNSServerLst.count() << "\" />" << endl;

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
	SPtr<TOptVendorSpecInfo> opt;
	out << "    <vendorSpecLst count=\"" << iface.VendorSpecLst.count() << "\">" << endl;
	iface.VendorSpecLst.first();
	while (opt = iface.VendorSpecLst.get()) {
	    out << "      <vendorSpec vendor=\"" << opt->getVendor() << "\">"
		<< opt->getPlain() << "</vendorSpec>" << endl;
	}
	out << "    <vendorSpecLst/>" << endl;
    } else {
	out << "    <!-- <vendorSpecLst/> -->" << endl;
    }

    out << "  </ClntCfgIface>" << endl;

    return out;
}
