/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.cpp,v 1.6 2004-07-05 00:53:03 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.4  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 * Revision 1.3  2004/05/23 20:13:12  thomson
 * *** empty log message ***
 *
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
    this->DNSSrvState=NOTCONFIGURED;
    this->NTPSrvState=NOTCONFIGURED;
    this->DomainNameState=NOTCONFIGURED;
    this->TimeZoneState=NOTCONFIGURED;
}

TClntCfgIface::TClntCfgIface(int ifaceNr) {
    NoConfig=false;
    ID=ifaceNr;
    IfaceName="[unknown]";
    this->DNSSrvState=NOTCONFIGURED;
    this->NTPSrvState=NOTCONFIGURED;
    this->DomainNameState=NOTCONFIGURED;
    this->TimeZoneState=NOTCONFIGURED;
}

void TClntCfgIface::setOptions(SmartPtr<TClntParsGlobalOpt> opt) {
    ReqDNSSrv=opt->getReqDNSSrv();
    ReqNTPSrv=opt->getReqNTPSrv();
    ReqDomainName=opt->getReqDomainName();
    ReqTimeZone=opt->getReqTimeZone();

    DNSReqOpt=opt->getDNSSrvReqOpt();
    NTPReqOpt=opt->getNTPSrvReqOpt();

    //NISReq=opt->NISSrvReqOpt;
    DNSSendOpt=opt->getDNSSrvSendOpt();
    NTPSendOpt=opt->getNTPSrvSendOpt();
    //NISOptions=opt->NISSrvSendOpt;
    
    Domain=opt->getDomain();
    DomainReqOpt=opt->getDomainReqOpt();
    DomainSendOpt=opt->getDomainSendOpt();

    TZone=opt->getTimeZone();
    TimeZoneReqOpt=opt->getTimeZoneReqOpt();
    TimeZoneSendOpt=opt->getTimeZoneSendOpt();
    
    SmartPtr<TIPv6Addr> StationIDPtr;
    opt->firstDNSSrv();
    while(StationIDPtr=opt->getDNSSrv())
        this->DNSSrv.append(StationIDPtr);

    opt->firstAppDNSSrv();
    while(StationIDPtr=opt->getAppDNSSrv())
        this->AppDNSSrv.append(StationIDPtr);

    opt->firstPrepDNSSrv();
    while(StationIDPtr=opt->getPrepDNSSrv())
        this->PrepDNSSrv.append(StationIDPtr);

    opt->firstNTPSrv();
    while(StationIDPtr=opt->getNTPSrv())
        this->NTPSrv.append(StationIDPtr);

    opt->firstAppNTPSrv();
    while(StationIDPtr=opt->getAppNTPSrv())
        this->AppNTPSrv.append(StationIDPtr);

    opt->firstPrepNTPSrv();
    while(StationIDPtr=opt->getPrepNTPSrv())
        this->PrepNTPSrv.append(StationIDPtr);
    
    this->isIA = opt->getIsIAs();
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

 bool TClntCfgIface::isReqDNSSrv()
{
    return this->ReqDNSSrv;
}

 bool TClntCfgIface::isReqNTPSrv()
{
    return this->ReqNTPSrv;
}

 bool TClntCfgIface::isReqDomainName()
{
    return this->ReqDomainName;
}

 bool TClntCfgIface::isReqTimeZone()
{
    return this->ReqTimeZone;
}


 EState TClntCfgIface::getDNSSrvState()
{
    return DNSSrvState;
}

 EState TClntCfgIface::getNTPSrvState()
{
    return NTPSrvState;
}

 EState TClntCfgIface::getDomainNameState()
{
    return DomainNameState;
}

 EState TClntCfgIface::getTimeZoneState()
{
    return TimeZoneState;
}

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

TContainer<SmartPtr<TIPv6Addr> > TClntCfgIface::getProposedDNSSrv()
{
    return this->DNSSrv;
}

TContainer<SmartPtr<TIPv6Addr> > TClntCfgIface::getProposedNTPSrv()
{
    return this->NTPSrv;
}

string TClntCfgIface::getProposedDomainName()
{
    return this->Domain;
}

string TClntCfgIface::getProposedTimeZone()
{
    return this->TZone;
}

void TClntCfgIface::setDNSSrvState(EState state)
{
    this->DNSSrvState=state;
}

void TClntCfgIface::setNTPSrvState(EState state)
{
    this->NTPSrvState=state;
}

void TClntCfgIface::setDomainNameState(EState state)
{
    this->DomainNameState=state;
}

void TClntCfgIface::setTimeZoneState(EState state)
{
    this->TimeZoneState=state;
}

bool TClntCfgIface::onlyInformationRequest()
{
    return !this->isIA;
}

ESendOpt TClntCfgIface::getDNSSendOpt()
{
    return this->DNSSendOpt;
}

EReqOpt  TClntCfgIface::getDNSReqOpt()
{
    return this->DNSReqOpt;
}

ESendOpt TClntCfgIface::getNTPSendOpt()
{
    return this->NTPSendOpt;
}

EReqOpt  TClntCfgIface::getNTPReqOpt()
{
    return this->NTPReqOpt;
}

ESendOpt TClntCfgIface::getDomainSendOpt()
{
    return this->DomainSendOpt;
}

EReqOpt  TClntCfgIface::getDomainReqOpt()
{
    return this->DomainReqOpt;
}

ESendOpt TClntCfgIface::getTimeZoneSendOpt()
{
    return this->TimeZoneSendOpt;
}

EReqOpt  TClntCfgIface::getTimeZoneReqOpt()
{
    return this->TimeZoneReqOpt;
}

bool TClntCfgIface::noConfig() {
    return NoConfig;
}

ostream& operator<<(ostream& out,TClntCfgIface& iface)
{
    SmartPtr<TIPv6Addr> Station;

    out<<"  <ClntCfgIface ";
    if (iface.NoConfig) {
        out << "no-config=\"true\" />" << endl;
        return out;
    }

    out << "name=\"" << iface.IfaceName << "\""
        << " id=\"" << iface.ID << "\">" << endl;

    // --- DNS-servers ---
    out << "    <DNSServers>" << endl;
    out << "      <Request count=\""<<iface.DNSSrv.count() << "\">" << endl;
    iface.DNSSrv.first();
    while(Station=iface.DNSSrv.get())
        out << *Station ;  
    out << "      </Request>" << endl;

    // appended DNS-servers 
    out << "      <Append count=\"" << iface.AppDNSSrv.count() << "\">" << endl;
    iface.AppDNSSrv.first();
    while(Station=iface.AppDNSSrv.get())
        out << *Station ;  
    out << "      </Append>" << endl;

    // prepended DNS-servers
    out << "      <Prefered count=\"" << iface.PrepDNSSrv.count() << "\">" << endl;
    iface.PrepDNSSrv.first();
    while(Station=iface.PrepDNSSrv.get())
        out << *Station;  
    out << "      </Prefered>" << endl;

    // required
    out << "      <Required>" << iface.DNSReqOpt << "</Required>" << endl;

    // DNSOptions
    out << "      <Options>" << iface.DNSSendOpt << "</Options>" << endl;
    out << "    </DNSServers>" << endl;


    // --- DOMAIN ---
    out << "    <Domain>" << iface.Domain << endl;
    out << "      <DomainReq>" << iface.DomainReqOpt << "</DomainReq>" << endl;
    out << "      <DomainOptions>" << iface.DomainSendOpt << "</DomainOptions>" << endl;
    out << "    </Domain>" << endl;

    // --- NTP servers ---
    out << "    <NTPServers>" << endl;

    out << "      <request count=\"" << iface.NTPSrv.count() << "\">" << endl;
    iface.NTPSrv.first();
    while(Station=iface.NTPSrv.get())
        out << *Station;  
    out << "      </request>" << endl;

    out << "      <append count=\"" << iface.AppNTPSrv.count() << "\">" << endl;
    iface.AppNTPSrv.first();
    while(Station=iface.AppNTPSrv.get())
        out << *Station;  
    out << "      </append>" << endl;


    out << "      <prefered count=\"" << iface.PrepNTPSrv.count() << "\">"  << logger::endl;
    iface.PrepNTPSrv.first();
    while(Station=iface.PrepNTPSrv.get())
        cout << *Station;  
    out << "      </prefered>" << endl;

    // required NTP
    out << "      <Req value=\"" <<  iface.NTPReqOpt << "\" NTPOptions=\""
        << iface.NTPSendOpt << "\"/>" << endl;
    out << "    </NTPServers>" << endl;

    // --- Timezone ---

    out << "    <TimeZone>:"<<iface.TZone << logger::endl;
    out << "      <DomainReq>" << iface.TimeZoneReqOpt << "</DomainReq>" << endl;
    out << "      <Options>" << iface.TimeZoneSendOpt << "</Options>" << endl;
    out << "    </TimeZone>" << endl;

    out << "    <groups count=\"" << iface.ClntCfgGroupLst.count() << "\">" << endl;
    SmartPtr<TClntCfgGroup>	groupPtr;
    iface.ClntCfgGroupLst.first();
    while(groupPtr=iface.ClntCfgGroupLst.get())
    {	
        out << *groupPtr;
    }
    out << "    </groups>" << endl;

    out << "  </ClntCfgIface>" << endl;

    return out;
}
