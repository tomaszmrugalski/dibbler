/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.cpp,v 1.16 2006-03-21 20:02:01 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2006/03/20 23:04:05  thomson
 * TA option is now parsed properly and SOLICIT is sent as expected.
 *
 * Revision 1.14  2006/03/05 21:38:47  thomson
 * TA support merged.
 *
 * Revision 1.13.2.1  2006/02/05 23:38:06  thomson
 * Devel branch with Temporary addresses support added.
 *
 * Revision 1.13  2005/01/03 21:53:01  thomson
 * id is now called ifindex
 *
 * Revision 1.12  2004/11/30 00:42:50  thomson
 * Client no longer sends RapidCommit, unless told to do so (bug #55)
 *
 * Revision 1.11  2004/11/29 21:21:56  thomson
 * Client parser now supports 'option lifetime' directive (bug #75)
 *
 * Revision 1.10  2004/11/01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.9  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.8  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
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
bool TClntCfgIface::isReqLifetime() {
    return this->ReqLifetime;
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
void TClntCfgIface::setLifetimeState(EState state) {
    this->LifetimeState=state;
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

    out << "  </ClntCfgIface>" << endl;

    return out;
}
