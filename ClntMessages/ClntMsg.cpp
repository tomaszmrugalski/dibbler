/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsg.cpp,v 1.10.2.1 2006-02-05 23:38:07 thomson Exp $
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef LINUX
#include <netinet/in.h>
#endif

#include <cmath>


#include "ClntMsg.h"
#include "ClntTransMgr.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptIA_NA.h"
#include "ClntOptTA.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptPreference.h"
#include "ClntOptElapsed.h"
#include "ClntOptServerUnicast.h"
#include "ClntOptStatusCode.h"
#include "ClntOptRapidCommit.h"

#include "ClntOptDNSServers.h"
#include "ClntOptDomainName.h"
#include "ClntOptNTPServers.h"
#include "ClntOptTimeZone.h"
#include "ClntOptSIPServer.h"
#include "ClntOptSIPDomain.h"
#include "ClntOptFQDN.h"
#include "ClntOptNISServer.h"
#include "ClntOptNISDomain.h"
#include "ClntOptNISPServer.h"
#include "ClntOptNISPDomain.h"
#include "ClntOptLifetime.h"
#include "Logger.h"

string msgs[] = { "",
		  "SOLICIT",
		  "ADVERTISE",
		  "REQUEST",
		  "CONFIRM",
		  "RENEW",
		  "REBIND",
		  "REPLY",
		  "RELEASE",
		  "DECLINE",
		  "RECONFIGURE",
		  "INF-REQUEST",
		  "RELAY-FORWARD"
		  "RELAY-REPLY"};

void TClntMsg::invalidAllowOptInMsg(int msg, int opt) {
    string name;
    if (msg<=13)
	name = msgs[msg];
    Log(Warning) << "Option " << opt << " is not allowed in the " << name
		 << " message. Ignoring." << LogEnd;
}

void TClntMsg::invalidAllowOptInOpt(int msg, int parentOpt, int childOpt) {
    string name;
    if (msg<=13)
	name = msgs[msg];

    if (parentOpt==0)
	Log(Warning) << "Option " << childOpt << " is not allowed directly in " << name
		     << " message. Ignoring." << LogEnd;
    else
	Log(Warning) << "Option " << childOpt << " is not allowed in the " << parentOpt
		     << " in the " << name << " message. Ignoring." << LogEnd;
}

//Constructor builds message on the basis of buffer
TClntMsg::TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                   SmartPtr<TClntTransMgr> TransMgr, 
                   SmartPtr<TClntCfgMgr>   CfgMgr,
                   SmartPtr<TClntAddrMgr>  AddrMgr,
                   int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize)
                   :TMsg(iface, addr, buf, bufSize)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
    //After reading message code and transactionID   
    //read options contained in message    
    int pos=0;
    SmartPtr<TOpt> ptr;
    while (pos<bufSize) {
	ptr = 0;
        short code = ntohs( *((short*) (buf+pos)));
        pos+=2;
        short length = ntohs(*((short*) (buf+pos)));
        pos+=2;

	if (!allowOptInMsg(MsgType,code)) {
	    this->invalidAllowOptInMsg(MsgType, code);
	    pos+=length;
	    continue;
	}
	if (!allowOptInOpt(MsgType,0,code)) {
	    this->invalidAllowOptInOpt(MsgType, 0, code);
	    pos+=length;
	    continue;
	}
	switch (code) {
	case OPTION_CLIENTID:
	    ptr = new TClntOptClientIdentifier(buf+pos,length,this);
	    break;
	case OPTION_SERVERID:
	    ptr =new TClntOptServerIdentifier(buf+pos,length,this);
	    break;
	case OPTION_IA:
	    ptr = new TClntOptIA_NA(buf+pos,length,this);
	    break;
	case OPTION_ORO:
	    ptr = new TClntOptOptionRequest(buf+pos,length,this);
	    break;
	case OPTION_PREFERENCE:
	    ptr = new TClntOptPreference(buf+pos,length,this);
	    break;
	case OPTION_ELAPSED_TIME:
	    ptr = new TClntOptElapsed(buf+pos,length,this);
	    break;
	case OPTION_UNICAST:
	    ptr = new TClntOptServerUnicast(buf+pos,length,this);
	    break;
	case OPTION_STATUS_CODE:
	    ptr = new TClntOptStatusCode(buf+pos,length,this);
	    break;
	case OPTION_RAPID_COMMIT:
	    ptr = new TClntOptRapidCommit(buf+pos,length,this);
	    break;
	case OPTION_DNS_RESOLVERS:
	    ptr = new TClntOptDNSServers(buf+pos,length,this);
	    break;
	case OPTION_NTP_SERVERS:
	    ptr = new TClntOptNTPServers(buf+pos,length,this);
	    break;
	case OPTION_DOMAIN_LIST:
	    ptr = new TClntOptDomainName(buf+pos, length, this);
	    break;
	case OPTION_TIME_ZONE:
	    ptr = new TClntOptTimeZone(buf+pos, length,this);
	    break;
	case OPTION_SIP_SERVERS:
	    ptr = new TClntOptSIPServers(buf+pos, length, this);
	    break;
	case OPTION_SIP_DOMAINS:
	    ptr = new TClntOptSIPDomain(buf+pos, length, this);
	    break;
	case OPTION_NIS_SERVERS:
	    ptr = new TClntOptNISServers(buf+pos, length, this);
	    break;
	case OPTION_NIS_DOMAIN_NAME:
	    ptr = new TClntOptNISDomain(buf+pos, length, this);
	    break;
	case OPTION_NISP_SERVERS:
	    ptr = new TClntOptNISPServers(buf+pos, length, this);
	    break;
	case OPTION_NISP_DOMAIN_NAME:
	    ptr = new TClntOptNISPDomain(buf+pos, length, this);
	    break;
	case OPTION_FQDN:
	    ptr = new TClntOptFQDN(buf+pos, length, this);
	    break;
	case OPTION_LIFETIME:
	    ptr = new TClntOptLifetime(buf+pos, length, this);
	    break;
	case OPTION_IA_TA: {
	    SmartPtr<TClntOptTA> ta = new TClntOptTA(buf+pos, length, this);
	    ta->setContext(AddrMgr, IfaceMgr, CfgMgr, Iface, addr);
	    ptr = (Ptr*) ta;
	    break;
	}
	case OPTION_RECONF_ACCEPT:
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
	case OPTION_VENDOR_OPTS:
	case OPTION_RECONF_MSG:
	case OPTION_RELAY_MSG:
	case OPTION_AUTH_MSG:
	case OPTION_INTERFACE_ID:
	    Log(Warning) << "Option " << code<< " in message " 
			 << MsgType << " is not supported." << LogEnd;
	    break;
	default: 
	    Log(Warning) << "Unknown option: " << code << ", length=" << length 
			 << ". Ignored." << LogEnd;
	    pos+=length;
	    continue;
	}
	
	if ( (ptr) && (ptr->isValid()) ) {
                    Options.append( ptr );
	} else {
	    Log(Warning) << "Option " << code << " is invalid. Ignoring." << LogEnd;
	}
        pos+=length;
    }

    SmartPtr<TClntOptServerIdentifier> optSrvID = (Ptr*)this->getOption(OPTION_SERVERID);
    if (!optSrvID) {
	Log(Warning) << "Message " << this->MsgType 
		     << " does not contain SERVERID option. Ignoring." << LogEnd;
	this->IsDone = true;
	return;
    }

    this->firstOption();
    SmartPtr<TOpt> opt;
    while ( opt = getOption() )
	opt->setDUID(optSrvID->getDUID());
}

TClntMsg::TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                   SmartPtr<TClntTransMgr> TransMgr, 
                   SmartPtr<TClntCfgMgr> CfgMgr,
                   SmartPtr<TClntAddrMgr> AddrMgr,
                   int iface, 
                   SmartPtr<TIPv6Addr> addr, int msgType)
                   :TMsg(iface,addr,msgType)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
}

void TClntMsg::setAttribs(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                          SmartPtr<TClntTransMgr> TransMgr, 
                          SmartPtr<TClntCfgMgr> CfgMgr,
                          SmartPtr<TClntAddrMgr> AddrMgr)
{
    ClntTransMgr=TransMgr;	
    ClntIfaceMgr=IfaceMgr;	
    ClntCfgMgr=CfgMgr;
    ClntAddrMgr=AddrMgr;

    FirstTimeStamp = now();			
    LastTimeStamp  = now();			

    RC = 0;
    RT = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;
}

unsigned long TClntMsg::getTimeout()
{
    long diff = (LastTimeStamp+RT) - now();
    return (diff<0) ? 0 : diff;
}

void TClntMsg::send()
{
    if (!RC)
        RT=(int)floor(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    else
	RT =(int) floor(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if (MRT != 0 && RT>MRT) 
	RT = (int) floor(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    
    if ((MRD != 0) && (RT>MRD))
        RT = MRD;
    if (MRD) MRD-=RT;
    
    RC++;

    this->storeSelf(this->pkt);

    SmartPtr<TIfaceIface> ptrIface = ClntIfaceMgr->getIfaceByID(Iface);
    if (PeerAddr) {
	Log(Debug) << "Sending " << this->getName() << " on " << ptrIface->getName() 
		   << "/" << Iface << " to unicast addr " << *PeerAddr << "." << LogEnd;
	ClntIfaceMgr->sendUnicast(Iface,pkt,getSize(),PeerAddr);
    } else {
	Log(Debug) << "Sending " << this->getName() << " on " << ptrIface->getName() 
		   << "/" << Iface << " to multicast." << LogEnd;
	ClntIfaceMgr->sendMulticast(Iface, pkt, getSize());
    }
    LastTimeStamp = now();
}

SmartPtr<TClntTransMgr> TClntMsg::getClntTransMgr()
{
    return this->ClntTransMgr;
}

SmartPtr<TClntAddrMgr> TClntMsg::getClntAddrMgr()
{
    return this->ClntAddrMgr;
}

SmartPtr<TClntCfgMgr> TClntMsg::getClntCfgMgr()
{
    return this->ClntCfgMgr;
}

SmartPtr<TClntIfaceMgr> TClntMsg::getClntIfaceMgr()
{
    return this->ClntIfaceMgr;
}

void TClntMsg::setIface(int iface) {
    this->Iface = iface;
    SmartPtr<TOpt> opt;
    this->Options.first();
    while (opt = Options.get()) {
	switch ( opt->getOptType() ) {
	case OPTION_IA: {
	    SmartPtr<TClntOptIA_NA> ia = (Ptr*) opt;
	    ia->setIface(iface);
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TClntOptTA> ta = (Ptr*) opt;
	    ta->setIface(iface);
	    break;
	}
	default:
	    continue;
	}
    }
}


/*
 * this method adds requested (which have status==NOTCONFIGURED) options
 */
void TClntMsg::appendRequestedOptions() {

    // find configuration specified in config file
    SmartPtr<TClntCfgIface> iface = ClntCfgMgr->getIface(this->Iface);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << this->Iface << LogEnd;
	return;
    }
    
    SmartPtr<TClntOptOptionRequest> optORO = new TClntOptOptionRequest(iface, this);

    // --- option: DNS-SERVERS ---
    if ( iface->isReqDNSServer() && (iface->getDNSServerState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_DNS_RESOLVERS);
	
	List(TIPv6Addr) * dnsLst = iface->getProposedDNSServerLst();
	if (dnsLst->count()) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptDNSServers> opt = new TClntOptDNSServers(dnsLst,this);
	    Options.append( (Ptr*)opt );
	}
	iface->setDNSServerState(INPROCESS);
    }

    // --- option: DOMAINS --
    if ( iface->isReqDomain() && (iface->getDomainState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_DOMAIN_LIST);

	List(string) * domainsLst = iface->getProposedDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
            SmartPtr<TClntOptDomainName> opt = new TClntOptDomainName(domainsLst,this);
            Options.append( (Ptr*)opt );
	}
	iface->setDomainState(INPROCESS);
    }

    // --- option: NTP SERVER ---
    if ( iface->isReqNTPServer() && (iface->getNTPServerState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_NTP_SERVERS);

	List(TIPv6Addr) * ntpLst = iface->getProposedNTPServerLst();
	if (ntpLst->count()) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptNTPServers> opt = new TClntOptNTPServers(ntpLst,this);
	    Options.append( (Ptr*)opt );
	}
	iface->setNTPServerState(INPROCESS);
    }
        
    // --- option: TIMEZONE ---
    if ( iface->isReqTimezone() && (iface->getTimezoneState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_TIME_ZONE);

	string timezone = iface->getProposedTimezone();
	if (timezone.length()) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptTimeZone> opt = new TClntOptTimeZone(timezone,this);
	    Options.append( (Ptr*)opt );
	}
	iface->setTimezoneState(INPROCESS);
    }

    // --- option: SIP-SERVERS ---
    if ( iface->isReqSIPServer() && (iface->getSIPServerState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_SIP_SERVERS);
	
	List(TIPv6Addr) * lst = iface->getProposedSIPServerLst();
	if ( lst->count()) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptSIPServers> opt = new TClntOptSIPServers( lst, this );
	    Options.append( (Ptr*)opt );
	}
	iface->setSIPServerState(INPROCESS);
    }

    // --- option: SIP-DOMAINS ---
    if ( iface->isReqSIPDomain() && (iface->getSIPDomainState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_SIP_DOMAINS);

	List(string) * domainsLst = iface->getProposedSIPDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
            SmartPtr<TClntOptSIPDomain> opt = new TClntOptSIPDomain( domainsLst,this );
            Options.append( (Ptr*)opt );
	}
	iface->setSIPDomainState(INPROCESS);
    }

    // --- option: FQDN ---
    if ( iface->isReqFQDN() && (iface->getFQDNState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_FQDN);

	string fqdn = iface->getProposedFQDN();
	if (fqdn.length()) {
	    SmartPtr<TClntOptFQDN> opt = new TClntOptFQDN( fqdn,this );
	    Options.append( (Ptr*)opt );
	}
	iface->setFQDNState(INPROCESS);
    }

    // --- option: NIS-SERVERS ---
    if ( iface->isReqNISServer() && (iface->getNISServerState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_NIS_SERVERS);
	
	List(TIPv6Addr) * lst = iface->getProposedNISServerLst();
	if ( lst->count() ) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptNISServers> opt = new TClntOptNISServers( lst,this );
	    Options.append( (Ptr*)opt );
	}
	iface->setNISServerState(INPROCESS);
    }

    // --- option: NIS-DOMAIN ---
    if ( iface->isReqNISDomain() && (iface->getNISDomainState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_NIS_DOMAIN_NAME);
	string domain = iface->getProposedNISDomain();
	if (domain.length()) {
	    SmartPtr<TClntOptNISDomain> opt = new TClntOptNISDomain( domain,this );
	    Options.append( (Ptr*)opt );
	}
	iface->setNISDomainState(INPROCESS);
    }

    // --- option: NIS+-SERVERS ---
    if ( iface->isReqNISPServer() && (iface->getNISPServerState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_NISP_SERVERS);
	
	List(TIPv6Addr) * lst = iface->getProposedNISPServerLst();
	if ( lst->count() ) {
	    // if there are any hints specified in config file, include them
	    SmartPtr<TClntOptNISPServers> opt = new TClntOptNISPServers( lst,this );
	    Options.append( (Ptr*)opt );
	}
	iface->setNISPServerState(INPROCESS);
    }

    // --- option: NIS+-DOMAIN ---
    if ( iface->isReqNISPDomain() && (iface->getNISPDomainState()==NOTCONFIGURED) ) {
	optORO->addOption(OPTION_NISP_DOMAIN_NAME);
	string domain = iface->getProposedNISPDomain();
	if (domain.length()) {
	    SmartPtr<TClntOptNISPDomain> opt = new TClntOptNISPDomain( domain,this );
	    Options.append( (Ptr*)opt );
	}
	iface->setNISPDomainState(INPROCESS);
    }

    // --- option: LIFETIME ---
    if ( iface->isReqLifetime() && (this->MsgType == INFORMATION_REQUEST_MSG) && optORO->count() )
	optORO->addOption(OPTION_LIFETIME);

    // final setup: Did we add any options at all? 
    if ( optORO->count() ) 
	Options.append( (Ptr*) optORO );
}

/** 
 * append all TA options, which are currently in the NOTCONFIGURED state.
 * 
 * @param switchToInProcess - switch them to INPROCESS state?
 */
void TClntMsg::appendTAOptions(bool switchToInProcess)
{
    SmartPtr<TClntCfgIface> ptrIface;
    SmartPtr<TClntCfgTA> ptrTA;
    ClntCfgMgr->firstIface();
    // for each interface...
    while ( ptrIface = ClntCfgMgr->getIface() ) {
	ptrIface->firstTA();
	// ... find TA...
	while ( ptrTA = ptrIface->getTA() ) {
	    if (ptrTA->getState()!=NOTCONFIGURED)
		continue;
	    // ... which are not yet configured
	    SmartPtr<TOpt> ptrOpt = new TClntOptTA(ptrTA->getIAID(), this);

	    Options.append ( (Ptr*) ptrOpt);
	    Log(Debug) << "TA option (IAID=" << ptrTA->getIAID() << ") was added." << LogEnd;
	    if (switchToInProcess)
		ptrTA->setState(INPROCESS);
	}
    }

}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/01/08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.9  2004/12/08 01:08:23  thomson
 * Warning messages now print proper message names.
 *
 * Revision 1.8  2004/11/29 22:46:45  thomson
 * Lifetime option is only requested if specified in conf file (bug #75)
 *
 * Revision 1.7  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.6  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.5  2004/10/03 21:36:48  thomson
 * Just a typo.
 *
 * Revision 1.4  2004/09/27 22:01:01  thomson
 * Sending is now more verbose.
 *
 * Revision 1.3  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.2  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 */
