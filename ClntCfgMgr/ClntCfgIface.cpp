/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.cpp,v 1.8 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.6  2004/07/05 00:53:03  thomson
 * Various changes.
 *                                                                           
 */

#include <iostream>
#include <iomanip>
#include "ClntCfgIface.h"
#include "Logger.h"
#include "Portable.h"
using namespace std;

TClntCfgIface::TClntCfgIface(string ifaceName) {
    NoConfig=false;
    IfaceName=ifaceName;
    ID=-1;
    this->DNSServerState  = NOTCONFIGURED;
    this->DomainState     = NOTCONFIGURED;
    this->NTPServerState  = NOTCONFIGURED;
    this->TimezoneState   = NOTCONFIGURED;
    this->SIPServerState  = NOTCONFIGURED;
    this->SIPDomainState  = NOTCONFIGURED;
    this->FQDNState       = NOTCONFIGURED;
    this->NISServerState  = NOTCONFIGURED;
    this->NISPServerState = NOTCONFIGURED;
    this->NISDomainState  = NOTCONFIGURED;
    this->NISPDomainState = NOTCONFIGURED;
}

TClntCfgIface::TClntCfgIface(int ifaceNr) {
    NoConfig=false;
    ID=ifaceNr;
    IfaceName="[unknown]";
    this->DNSServerState  = NOTCONFIGURED;
    this->DomainState     = NOTCONFIGURED;
    this->NTPServerState  = NOTCONFIGURED;
    this->TimezoneState   = NOTCONFIGURED;
    this->SIPServerState  = NOTCONFIGURED;
    this->SIPDomainState  = NOTCONFIGURED;
    this->FQDNState       = NOTCONFIGURED;
    this->NISServerState  = NOTCONFIGURED;
    this->NISPServerState = NOTCONFIGURED;
    this->NISDomainState  = NOTCONFIGURED;
    this->NISPDomainState = NOTCONFIGURED;
}

void TClntCfgIface::setOptions(SmartPtr<TClntParsGlobalOpt> opt) {
    this->isIA = opt->getIsIAs();
    this->Unicast = opt->getUnicast();

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
}

 void TClntCfgIface::firstGroup()
{
    ClntCfgGroupLst.first();
}

 int TClntCfgIface::countGroup()
{
    return ClntCfgGroupLst.count();
}

 SmartPtr<TClntCfgGroup> TClntCfgIface::getGroup()
{
    return ClntCfgGroupLst.get();
}

 void TClntCfgIface::addGroup(SmartPtr<TClntCfgGroup> ptr)
{
    ClntCfgGroupLst.append(ptr);
}

 SmartPtr<TClntCfgGroup> TClntCfgIface::getLastGroup()
{
    return ClntCfgGroupLst.getLast();
}

 string TClntCfgIface::getName(void)
{
    return IfaceName;    
}

 int	TClntCfgIface::getID(void)
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

bool TClntCfgIface::onlyInformationRequest()
{
    return !this->isIA;
}

bool TClntCfgIface::noConfig() {
    return NoConfig;
}

bool TClntCfgIface::getUnicast() {
    return this->Unicast;
}

// --------------------------------------------------------------------------------
// --- options below --------------------------------------------------------------
// --------------------------------------------------------------------------------
bool TClntCfgIface::isReqDNSServer() {
    return this->ReqDNSServer;
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


/*
 void TClntCfgIface::setDNSSrv(
    TContainer<SmartPtr<TIPv6Addr> > newSrvLst,
    SmartPtr<TDUID> duid)
{
    switch(DNSSrvState) {
        //Is this a first configuration (so prob. reply has been received to:
        //solicit(rap.commit), request or information request message
    case NOTCONFIGURED: {
	std::clog << logger::logDebug << "DNSes for interface " << this->getName() 
		  << " are in state NOTCONFIGURED" << logger::endl;
	DNSSrvState=CONFIGURED;
	newSrvLst.first();
	DNSSrv.clear();
	SmartPtr<TIPv6Addr> dnsAddr;
	while (dnsAddr=newSrvLst.get()) {
	    DNSSrv.append(dnsAddr);
	    dns_add(this->IfaceName.c_str(), this->ID, dnsAddr->getPlain());
	}
	GiverDNSSrvDUID=duid;
    }
	
        //Is this parameter is in renewing process 
        //(renew/rebind/information request message was sent)
    case INPROCESS: {
	if (DNSSrv.count()) {
	    std::clog << logger::logDebug << "DNSes for interface " << this->getName() 
		      << " are in state INPROCESS -> CONFIGURED" << logger::endl;
	    DNSSrvState=CONFIGURED;
	    newSrvLst.first();
	    DNSSrv.clear();
	    SmartPtr<TIPv6Addr> dnsAddr;
	    while (dnsAddr=newSrvLst.get()) {
		DNSSrv.append(dnsAddr);
		dns_add(this->IfaceName.c_str(), this->ID, dnsAddr->getPlain());
	    }
	}
	else
	    DNSSrvState=NOTCONFIGURED;
    }
    default: { // configured 
	//FIXME: check if current list != new received list
    }
    };

}
*/

/*
 void TClntCfgIface::setNTPSrv(
    TContainer<SmartPtr<TIPv6Addr> > newSrvLst,
    SmartPtr<TDUID> duid)
{
    switch(NTPSrvState)
    {
        //Is this a first configuration (so prob. reply has been received to:
        //solicit(rap.commit), request or information request message
    case NOTCONFIGURED: {
	NTPSrvState=CONFIGURED;
	clog<< logger::logNotice << "New NTP server list received:";
	newSrvLst.first();
	NTPSrv.clear();
	SmartPtr<TIPv6Addr> dnsAddr;
	while (dnsAddr=newSrvLst.get()) {
	    NTPSrv.append(dnsAddr);
	    clog<< *dnsAddr<<",";
	}
	GiverNTPSrvDUID=duid;
	clog << "(server duid:"<<*duid<<")" << logger::endl;
	break;
    }
    case INPROCESS: {
        //Is this parameter is in renewing process 
        //(renew/rebind/information request message was sent)
	if (NTPSrv.count())
	{
	    NTPSrvState=CONFIGURED;
	    newSrvLst.first();
	    NTPSrv.clear();
	    SmartPtr<TIPv6Addr> ntpAddr;
	    while (ntpAddr=newSrvLst.get())
		NTPSrv.append(ntpAddr);
	}
	else
	    NTPSrvState=NOTCONFIGURED;
	break;
    }
    default: {
	break;
    }
    };
    //FIXME: Here should be also set options somewhere in the system ???
}
*/

/*
void TClntCfgIface::setDomainName(string domainName,SmartPtr<TDUID> duid) {
    switch(DomainNameState) {
	//Is this a first configuration (so prob. reply has been received to:
        //solicit(rap.commit), request or information request message
    case NOTCONFIGURED: {
	DomainNameState=CONFIGURED;
	this->Domain=domainName;
	GiverNTPSrvDUID=duid;
	domain_add(this->IfaceName.c_str(), this->ID, (char*)domainName.c_str());
	break;
    }
        //Is this parameter is in renewing process 
        //(renew/rebind/information request message was sent)
    case INPROCESS: {
	if (domainName!="") {
	    DomainNameState=CONFIGURED;
	    Domain=domainName;
	    domain_add(this->IfaceName.c_str(), this->ID, (char*)domainName.c_str());
	}
	else {
	    DomainNameState=NOTCONFIGURED;
	}
	break;
    }
    default: {
	// FIXME: check if domain has changed
    }
    };
}
*/

/*
 void TClntCfgIface::setTimeZone(
    string timeZone,
    SmartPtr<TDUID> duid)
{
    switch(TimeZoneState) {
        //Is this a first configuration (so prob. reply has been received to:
        //solicit(rap.commit), request or information request message
    case NOTCONFIGURED: {
	TimeZoneState=CONFIGURED;
	this->TZone=timeZone;
	GiverNTPSrvDUID=duid;
	break;
    }
        //Is this parameter is in renewing process 
        //(renew/rebind/information request message was sent)
    case INPROCESS: {
	if (timeZone!="") {
	    TimeZoneState=CONFIGURED;
	    this->TZone=timeZone;
	} else {
	    TimeZoneState=NOTCONFIGURED;
	}
    }
    default: {
	
    }
    };
    //FIXME: Here should be also set options somewhere in the system ???
}
*/

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

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TClntCfgIface& iface)
{
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

    out<<"  <ClntCfgIface ";
    if (iface.NoConfig) {
        out << "no-config=\"true\" />" << endl;
        return out;
    }

    out << "name=\"" << iface.IfaceName << "\""
        << " id=\"" << iface.ID << "\">" << endl;

    out << "    <!-- addresses -->" << endl;
    out << "    <groups count=\"" << iface.ClntCfgGroupLst.count() << "\">" << endl;
    SmartPtr<TClntCfgGroup>	groupPtr;
    iface.ClntCfgGroupLst.first();
    while(groupPtr=iface.ClntCfgGroupLst.get())
    {	
        out << *groupPtr;
    }
    out << "    </groups>" << endl;


    out << "    <!-- options -->" << endl;

    // --- option: DNS-servers ---
    if (iface.isReqDNSServer()) {
	out << "    <dns-servers hints=\"" << iface.DNSServerLst.count() << "\"/>" << endl;
	iface.DNSServerLst.first();
	while(addr=iface.DNSServerLst.get())
	    out << "      <dns-server>" << *addr << "</dns-server>" << endl;  
    } else {
	out << "    <!-- <dns-servers/> -->" << endl;
    }

    // --- option: DOMAIN ---
    if (iface.isReqDomain()) {
	out << "    <domains hints=\"" << iface.DomainLst.count() << "\">" << endl;
	iface.DomainLst.first();
	while (str = iface.DomainLst.get())
	    out << "      <domain>" << *str <<"</domain>" << endl;
    } else {
	out << "    <!-- <domains/> -->" << endl;
    }
	
    // --- option: NTP servers ---
    if (iface.isReqNTPServer()) {
	out << "    <ntp-servers hints=\"" << iface.NTPServerLst.count() << "\"/>" << endl;
	iface.NTPServerLst.first();
	while(addr=iface.NTPServerLst.get())
	    out << "      <ntp-server>" << *addr << "</ntp-server>" << endl;  
    } else {
	out << "    <!-- <ntp-servers/> -->" << endl;
    }
    
    // --- option: Timezone ---
    if (iface.isReqTimezone()) {
	out << "    <timezone>" << iface.Timezone << "</timezone>" << endl;
    } else {
	out << "    <!-- <timezone/> -->" << endl;
    }

    // --- option: SIP servers ---
    if (iface.isReqSIPServer()) {
	out << "    <sip-servers hints=\"" << iface.NTPServerLst.count() << "\"/>" << endl;
	iface.NTPServerLst.first();
	while(addr=iface.NTPServerLst.get())
	    out << "      <sip-server>" << *addr << "</sip-server>" << endl;  
    } else {
	out << "    <!-- <sip-servers/> -->" << endl;
    }

    // --- option: SIP domains ---
    if (iface.isReqSIPDomain()) {
	out << "    <sip-domains hints=\"" << iface.DomainLst.count() << "\"/>" << endl;
	iface.SIPDomainLst.first();
	while (str = iface.SIPDomainLst.get())
	    out << "      <sip-domain>" << *str <<"</sip-domain>" << endl;
    } else {
	out << "    <!-- <sip-domains/> -->" << endl;
    }

    // --- option: FQDN ---
    if (iface.isReqFQDN()) {
	out << "    <fqdn>" << iface.FQDN << "</fqdn>" << endl;
    } else {
	out << "    <!-- <fqdn/> -->" << endl;
    }

    // --- option: NIS server ---
    if (iface.isReqNISServer()) {
	out << "    <nis-servers hints=\"" << iface.NISServerLst.count() << "\"/>" << endl;
	iface.NISServerLst.first();
	while(addr=iface.NISServerLst.get())
	    out << "      <nis-server>" << *addr << "</nis-server>" << endl;  
    } else {
	out << "    <!-- <nis-servers/> -->" << endl;
    }

    // --- option: NIS domains ---
    if (iface.isReqNISDomain()) {
	out << "    <nis-domain>" << iface.NISDomain << "</nis-domain>" << endl;
    } else {
	out << "    <!-- <nis-domain/> -->" << endl;
    }

    // --- option: NISP server ---
    if (iface.isReqNISPServer()) {
	out << "    <nisplus-servers hints=\"" << iface.NISPServerLst.count() << "\"/>" << endl;
	iface.NISPServerLst.first();
	while(addr=iface.NISPServerLst.get())
	    out << "      <nisplus-server>" << *addr << "</nisplus-server>" << endl;  
    } else {
	out << "    <!-- <nisplus-servers/> -->" << endl;
    }

    // --- option: NISP domains ---
    if (iface.isReqNISPDomain()) {
	out << "    <nisplus-domain>" << iface.NISPDomain << "</nisplus-domain>" << endl;
    } else {
	out << "    <!-- <nisplus-domain> -->" << endl;
    }

    out << "  </ClntCfgIface>" << endl;

    return out;
}
