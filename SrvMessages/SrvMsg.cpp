/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.cpp,v 1.14 2006-03-03 21:02:16 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2005/04/29 00:08:20  thomson
 * *** empty log message ***
 *
 * Revision 1.12  2005/02/07 20:51:56  thomson
 * Server stateless mode fixed (bug #103)
 *
 * Revision 1.11  2005/01/30 23:12:28  thomson
 * *** empty log message ***
 *
 * Revision 1.10  2005/01/30 22:53:28  thomson
 * *** empty log message ***
 *
 * Revision 1.9  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.8  2004/12/04 23:43:26  thomson
 * Server no longer crashes after receiving the same INF-REQUEST (bug #84)
 *
 * Revision 1.7  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.6  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.5  2004/06/20 17:25:06  thomson
 * getName() method implemented, clean up
 *
 *
 */

#include <sstream>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif

#include "SrvMsg.h"
#include "SrvTransMgr.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptPreference.h"
#include "SrvOptElapsed.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
// --- options ---
#include "SrvOptDNSServers.h"
#include "SrvOptDomainName.h"
#include "SrvOptNTPServers.h"
#include "SrvOptTimeZone.h"
#include "SrvOptSIPServer.h"
#include "SrvOptSIPDomain.h"
#include "SrvOptFQDN.h"
#include "SrvOptNISServer.h"
#include "SrvOptNISDomain.h"
#include "SrvOptNISPServer.h"
#include "SrvOptNISPDomain.h"
#include "SrvOptLifetime.h"

#include "Logger.h"
#include "SrvIfaceMgr.h"
#include "AddrClient.h"

//Constructor builds message on the basis of received message
TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID)
                 :TMsg(iface, addr, msgType, transID)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
    this->Relays = 0;
}

//Constructor builds message on the basis of buffer
TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface,  SmartPtr<TIPv6Addr> addr,
                 char* buf,  int bufSize)
                 :TMsg(iface, addr, buf, bufSize)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
    this->Relays = 0;

    int pos=0;
    while (pos<bufSize)	{
        short code = ntohs( * ((short*) (buf+pos)));
        pos+=2;
        short length = ntohs(*((short*)(buf+pos)));
        pos+=2;
        SmartPtr<TOpt> ptr;

	if (!allowOptInMsg(MsgType,code)) {
	    Log(Warning) << "Option " << code << " not allowed in message type="<< MsgType <<". Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}
	if (!allowOptInOpt(MsgType,0,code)) {
	    Log(Warning) <<"Option " << code << " can't be present in message (type="
			 << MsgType <<") directly. Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}
	ptr= 0;
	switch (code) {
	case OPTION_CLIENTID:
	    ptr = new TSrvOptClientIdentifier(buf+pos,length,this);
	    break;
	case OPTION_SERVERID:
	    ptr =new TSrvOptServerIdentifier(buf+pos,length,this);
	    break;
	case OPTION_IA:
	    ptr = new TSrvOptIA_NA(buf+pos,length,this);
	    break;
	case OPTION_ORO:
	    ptr = new TSrvOptOptionRequest(buf+pos,length,this);
	    break;
	case OPTION_PREFERENCE:
	    ptr = new TSrvOptPreference(buf+pos,length,this);
	    break;
	case OPTION_ELAPSED_TIME:
	    ptr = new TSrvOptElapsed(buf+pos,length,this);
	    break;
	case OPTION_UNICAST:
	    ptr = new TSrvOptServerUnicast(buf+pos,length,this);
	    break;
	case OPTION_STATUS_CODE:
	    ptr = new TSrvOptStatusCode(buf+pos,length,this);
	    break;
	case OPTION_RAPID_COMMIT:
	    ptr = new TSrvOptRapidCommit(buf+pos,length,this);
	    break;
	case OPTION_DNS_RESOLVERS:
	    ptr = new TSrvOptDNSServers(buf+pos,length,this);
                    break;
	case OPTION_NTP_SERVERS:
	    ptr = new TSrvOptNTPServers(buf+pos,length,this);
	    break;
	case OPTION_DOMAIN_LIST:
	    ptr = new TSrvOptDomainName(buf+pos, length,this);
	    break;
	case OPTION_TIME_ZONE:
	    ptr = new TSrvOptTimeZone(buf+pos, length,this);
	    break;
	case OPTION_SIP_SERVERS:
	    ptr = new TSrvOptSIPServers(buf+pos, length, this);
	    break;
	case OPTION_SIP_DOMAINS:
	    ptr = new TSrvOptSIPDomain(buf+pos, length, this);
	    break;
	case OPTION_NIS_SERVERS:
	    ptr = new TSrvOptNISServers(buf+pos, length, this);
	    break;
	case OPTION_NIS_DOMAIN_NAME:
	    ptr = new TSrvOptNISDomain(buf+pos, length, this);
	    break;
	case OPTION_NISP_SERVERS:
	    ptr = new TSrvOptNISPServers(buf+pos, length, this);
	    break;
	case OPTION_NISP_DOMAIN_NAME:
	    ptr = new TSrvOptNISPDomain(buf+pos, length, this);
	    break;
	case OPTION_FQDN:
	    ptr = new TSrvOptFQDN(buf+pos, length, this);
	    break;
	case OPTION_LIFETIME:
	    ptr = new TSrvOptLifetime(buf+pos, length, this);
	    break;

	case OPTION_IA_TA:
	case OPTION_RECONF_ACCEPT:
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
	case OPTION_VENDOR_OPTS:
	case OPTION_RECONF_MSG:
	case OPTION_AUTH_MSG:
	case OPTION_RELAY_MSG:
	default:
	    break;
	}
	if ( (ptr) && (ptr->isValid()) )
	    Options.append( ptr );
	else
	    Log(Warning) << "Option type " << code << " invalid. Option ignored." << LogEnd;
        pos+=length;
    }

}

TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface, 
                 SmartPtr<TIPv6Addr> addr,  int msgType)
                 :TMsg(iface,addr,msgType)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
    this->Relays = 0;
}

void TSrvMsg::setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                         SmartPtr<TSrvTransMgr> TransMgr, 
                         SmartPtr<TSrvCfgMgr> CfgMgr,
                         SmartPtr<TSrvAddrMgr> AddrMgr)
{
    SrvTransMgr=TransMgr;	
    SrvIfaceMgr=IfaceMgr;	
    SrvCfgMgr=CfgMgr;
    SrvAddrMgr=AddrMgr;
    FirstTimeStamp = now();
    this->MRT=0;

    /*    			
    LastTimeStamp  = now();			

    RC = 0;
    RT = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;*/
}

void TSrvMsg::doDuties() {
    if ( !this->getTimeout() )
        this->IsDone = true;
}

unsigned long TSrvMsg::getTimeout() {
    if (this->FirstTimeStamp+this->MRT - now() > 0 )
        return this->FirstTimeStamp+this->MRT - now(); 
    else
        return 0;
}

void TSrvMsg::addRelayInfo(SmartPtr<TIPv6Addr> linkAddr,
			   SmartPtr<TIPv6Addr> peerAddr,
			   int hop,
			   SmartPtr<TSrvOptInterfaceID> interfaceID) {
    this->LinkAddrTbl[this->Relays] = linkAddr;
    this->PeerAddrTbl[this->Relays] = peerAddr;
    this->HopTbl[this->Relays]      = hop;
    this->InterfaceIDTbl[this->Relays] = interfaceID;
    this->Relays++;
}

int TSrvMsg::getRelayCount() {
    return this->Relays;
}

void TSrvMsg::send()
{
    static char buf[2048];
    int offset = 0;
    int port;

    SmartPtr<TSrvIfaceIface> ptrIface;
    SmartPtr<TSrvIfaceIface> under;
    ptrIface = (Ptr*) SrvIfaceMgr->getIfaceByID(this->Iface);
    Log(Notice) << "Sending " << this->getName() << " on " << ptrIface->getName() << "/" << this->Iface
		<< hex << ",transID=0x" << this->getTransID() << dec << ", opts:";
    SmartPtr<TOpt> ptrOpt;
    this->firstOption();
    while (ptrOpt = this->getOption() )
        Log(Cont) << " " << ptrOpt->getOptType();
    Log(Cont) << ", " << this->Relays << " relay(s)." << LogEnd;

    port = DHCPCLIENT_PORT;
    if (this->Relays>0) {
	port = DHCPSERVER_PORT;
	if (this->Relays>HOP_COUNT_LIMIT) {
	    Log(Error) << "Unable to send message. Got " << this->Relays << " relay entries (" << HOP_COUNT_LIMIT
		       << " is allowed maximum." << LogEnd;
	    return;
	}
	int len[HOP_COUNT_LIMIT];

	// calculate lengths of all relays
	len[this->Relays-1]=this->getSize();
	for (int i=this->Relays-1; i>0; i--) {
	    len[i-1]= len[i] + 34;
	    if (this->InterfaceIDTbl[i])
		len[i-1]+=this->InterfaceIDTbl[i]->getSize();
	}
	
	for (int i=0; i < this->Relays; i++) {
	    under = ptrIface->getUnderlaying();
	    if (!under) {
		Log(Error) << "Sending message on the " << ptrIface->getName() << "/" << ptrIface->getID()
			   << " failed: No underlaying interface found." << LogEnd;
		return;
	    }
	    ptrIface = under;

	    buf[offset++]=RELAY_REPL_MSG;
	    buf[offset++]=this->HopTbl[i];
	    this->LinkAddrTbl[i]->storeSelf(buf+offset);
	    this->PeerAddrTbl[i]->storeSelf(buf+offset+16);
	    offset +=32;
	    if (this->InterfaceIDTbl[i]) {
		this->InterfaceIDTbl[i]->storeSelf(buf+offset);
		offset += this->InterfaceIDTbl[i]->getSize();
	    }
	    *(short*)(buf+offset) = htons(OPTION_RELAY_MSG);
	    offset+=2;
	    *(short*)(buf+offset) = htons(len[i]);
	    offset+=2;
	    Log(Debug) << "RELAY_REPL header has been built (" << offset << " bytes)." << LogEnd;
	}

	Log(Debug) << "Sending " << this->getSize() << "(packet)+" << offset << "(relay headers) data on the "
		   << ptrIface->getName() << "/" << ptrIface->getID() << " interface." << LogEnd;
    }

    this->storeSelf(buf+offset);
    this->SrvIfaceMgr->send(ptrIface->getID(), buf, offset+this->getSize(), this->PeerAddr, port);
}

void TSrvMsg::copyRelayInfo(SmartPtr<TSrvMsg> q) {
    this->Relays = q->Relays;
    for (int i=0; i < this->Relays; i++) {
	this->LinkAddrTbl[i]   = q->LinkAddrTbl[i];
	this->PeerAddrTbl[i]   = q->PeerAddrTbl[i];
	this->InterfaceIDTbl[i]= q->InterfaceIDTbl[i];
	this->HopTbl[i]        = q->HopTbl[i];
    }
}

/*
 * this function appends all standard options
 */
bool TSrvMsg::appendRequestedOptions(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, 
				     int iface, SmartPtr<TSrvOptOptionRequest> reqOpts)
{
    bool newOptionAssigned = false;
    // client didn't want any option? Or maybe we're not supporting this client?
    if (!reqOpts->count() || !SrvCfgMgr->isClntSupported(duid,addr,iface))
	return false;

    SmartPtr<TSrvCfgIface>  ptrIface=SrvCfgMgr->getIfaceByID(iface);
    
    // --- option: DNS resolvers ---
    if ( reqOpts->isOption(OPTION_DNS_RESOLVERS) && ptrIface->supportDNSServer() ) {
	SmartPtr<TSrvOptDNSServers> optDNS = new TSrvOptDNSServers(*ptrIface->getDNSServerLst(),this);
	Options.append((Ptr*)optDNS);
	newOptionAssigned = true;
    };

    // --- option: DOMAIN LIST ---
    if ( reqOpts->isOption(OPTION_DOMAIN_LIST) && ptrIface->supportDomain() ) {
	SmartPtr<TSrvOptDomainName> optDomain= new TSrvOptDomainName(*ptrIface->getDomainLst(),this);
	Options.append((Ptr*)optDomain);
	newOptionAssigned = true;
    };
    
    // --- option: NTP servers ---
    if ( reqOpts->isOption(OPTION_NTP_SERVERS) && ptrIface->supportNTPServer() ) {
	SmartPtr<TSrvOptNTPServers> optNTP = new TSrvOptNTPServers(*ptrIface->getNTPServerLst(),this);
	Options.append((Ptr*)optNTP);
	newOptionAssigned = true;
    };
    
    // --- option: TIMEZONE --- 
    if ( reqOpts->isOption(OPTION_TIME_ZONE) && ptrIface->supportTimezone() ) {
	SmartPtr<TSrvOptTimeZone> optTimezone = new TSrvOptTimeZone(ptrIface->getTimezone(),this);
	Options.append((Ptr*)optTimezone);
	newOptionAssigned = true;
    };

    // --- option: SIP SERVERS ---
    if ( reqOpts->isOption(OPTION_SIP_SERVERS) && ptrIface->supportSIPServer() ) {
	SmartPtr<TSrvOptSIPServers> optSIPServer = new TSrvOptSIPServers(*ptrIface->getSIPServerLst(),this);
	Options.append((Ptr*)optSIPServer);
	newOptionAssigned = true;
    };

    // --- option: SIP DOMAINS ---
    if ( reqOpts->isOption(OPTION_SIP_DOMAINS) && ptrIface->supportSIPDomain() ) {
	SmartPtr<TSrvOptSIPDomain> optSIPDomain= new TSrvOptSIPDomain(*ptrIface->getSIPDomainLst(),this);
	Options.append((Ptr*)optSIPDomain);
	newOptionAssigned = true;
    };

    // --- option: FQDN ---
    if ( reqOpts->isOption(OPTION_FQDN) && ptrIface->supportFQDN() ) {
	/*
	 * Unfortunatelly, we need the the option sent by the client to set the flag correctly.
	 * This cannot be done here...
	 SmartPtr<TSrvOptFQDN> opt = new TSrvOptFQDN(ptrIface->getFQDNName(duid), this);
	 Options.append((Ptr*)opt);
	 newOptionAssigned = true;
	 
	 FIXME: This option should be set here, flags should be updated later.
	*/
    };

    // --- option: NIS SERVERS ---
    if ( reqOpts->isOption(OPTION_NIS_SERVERS) && ptrIface->supportNISServer() ) {
	SmartPtr<TSrvOptNISServers> opt = new TSrvOptNISServers(*ptrIface->getNISServerLst(),this);
	Options.append((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: NIS DOMAIN ---
    if ( reqOpts->isOption(OPTION_NIS_DOMAIN_NAME) && ptrIface->supportNISDomain() ) {
	SmartPtr<TSrvOptNISDomain> opt = new TSrvOptNISDomain(ptrIface->getNISDomain(),this);
	Options.append((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: NISP SERVERS ---
    if ( reqOpts->isOption(OPTION_NISP_SERVERS) && ptrIface->supportNISPServer() ) {
	SmartPtr<TSrvOptNISPServers> opt = new TSrvOptNISPServers(*ptrIface->getNISPServerLst(),this);
	Options.append((Ptr*) opt);
	newOptionAssigned = true;
    };

    // --- option: NISP DOMAIN ---
    if ( reqOpts->isOption(OPTION_NISP_DOMAIN_NAME) && ptrIface->supportNISPDomain() ) {
	SmartPtr<TSrvOptNISPDomain> opt = new TSrvOptNISPDomain(ptrIface->getNISPDomain(), this);
	Options.append((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: LIFETIME ---
    if ( newOptionAssigned && ptrIface->supportLifetime() ) {
	SmartPtr<TSrvOptLifetime> optLifetime = new TSrvOptLifetime(ptrIface->getLifetime(), this);
	Options.append( (Ptr*)optLifetime);
    }
    
    return newOptionAssigned;
}

/**
 * this function enumerates all options specified in the ORO option
 */
string TSrvMsg::showRequestedOptions(SmartPtr<TSrvOptOptionRequest> oro) {
    ostringstream x;
    int i = oro->count();
    x << i << " opts";
    if (i)
	x << ":";
    for (i=0;i<oro->count();i++) {
	x << " " << oro->getReqOpt(i);
    }
    return x.str();
}
