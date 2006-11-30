/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsg.cpp,v 1.33 2006-11-30 03:16:54 thomson Exp $
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
#include "SrvOptIA_PD.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptPreference.h"
#include "SrvOptElapsed.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "SrvOptTA.h"
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
#include "DNSUpdate.h"
#include "SrvOptAuthentication.h"

#include "Logger.h"
#include "SrvIfaceMgr.h"
#include "AddrClient.h"

/** 
 * this constructor is used to build message as a reply to the received message
 * (i.e. it is used to contruct ADVERTISE or REPLY)
 * 
 * @param IfaceMgr 
 * @param TransMgr 
 * @param CfgMgr 
 * @param AddrMgr 
 * @param iface 
 * @param addr 
 * @param msgType 
 * @param transID 
 */
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

/** 
 * this constructor builds message based on the buffer 
 * (i.e. SOLICIT, REQUEST, RENEW, REBIND, RELEASE, INF-REQUEST, DECLINE)
 * 
 * @param IfaceMgr 
 * @param TransMgr 
 * @param CfgMgr 
 * @param AddrMgr 
 * @param iface 
 * @param addr 
 * @param buf 
 * @param bufSize 
 */
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
	    ptr = new TSrvOptTA(buf+pos, length, this);
	    break;
	case OPTION_IA_PD:
	    ptr = new TSrvOptIA_PD(buf+pos, length, this);
	    break;
	case OPTION_AUTH:
        this->DigestType = DIGEST_HMAC_SHA1;
	    // FIXME: Detect digest type
            this->DigestType = DIGEST_HMAC_SHA1;
	    ptr = new TSrvOptAuthentication(buf+pos, length, this);
	    break;
	case OPTION_VENDOR_OPTS:
	    ptr = new TSrvOptVendorSpec(buf+pos, length, this);
	    break;
	case OPTION_RECONF_ACCEPT:
        this->DigestType = DIGEST_HMAC_SHA1;
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
	case OPTION_RECONF_MSG:
	case OPTION_RELAY_MSG:
	default:
	    Log(Warning) << "Option " << code << " not supported, so it was ignored." << LogEnd;
	    break;
	}
	if ( (ptr) && (ptr->isValid()) )
	    Options.append( ptr );
	else
	    Log(Warning) << "Option type " << code << " invalid. Option ignored." << LogEnd;
        pos+=length;
    }

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

/** 
 * this function appends all standard options
 * 
 * @param duid 
 * @param addr 
 * @param iface 
 * @param reqOpts 
 * 
 * @return true, if any options (conveying configuration paramter) has been appended
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


    // --- option: PREFIX DELEGATION ---
    if ( reqOpts->isOption(OPTION_IA_PD) && ptrIface->supportPrefixDelegation() ) {
	// FIXME: T1, T2 is hardcoded
	SmartPtr<TSrvOptIA_PD> optPrefixDelegation = new TSrvOptIA_PD(1,2000,3000, this);
	Options.append( (Ptr*)optPrefixDelegation);
        newOptionAssigned = true;
    }    

    // --- option: VENDOR SPEC ---
    if ( reqOpts->isOption(OPTION_VENDOR_OPTS) && ptrIface->supportVendorSpec()) {
	Options.append( (Ptr*)ptrIface->getVendorSpec());
	newOptionAssigned = true;
    }

    // --- option: LIFETIME ---
    // this option should be checked last 
    if ( newOptionAssigned && ptrIface->supportLifetime() ) {
	SmartPtr<TSrvOptLifetime> optLifetime = new TSrvOptLifetime(ptrIface->getLifetime(), this);
	Options.append( (Ptr*)optLifetime);
	newOptionAssigned = true;
    }

    // --- option: AUTH ---
    // FIXME: implement some logic here
    // if server is configured to allow DIGEST_NONE and client didn't send AUTH - don't send
    // if server is configured to allow DIGEST_HMAC_SHA1 only, then send it
    // FIXME: change to real DigestType
    this->DigestType = DIGEST_HMAC_SHA1;
    // if ...
    // tips: use SrvCfgMgr->getDigest() 
    if ( reqOpts->isOption(OPTION_AUTH) ) { // [s] - co¶ takiego jeszcze do³o¿yæ trzeba do tego if'a: && ptrIface->supportLifetime() ) {
    // FIXME: change to real DigestType
    this->DigestType = DIGEST_HMAC_SHA1;
	SmartPtr<TSrvOptAuthentication> optAuthentication = new TSrvOptAuthentication(this);
	Options.append( (Ptr*)optAuthentication);
    }

    return newOptionAssigned;
}

/**
 * this function prints all options specified in the ORO option
 */
string TSrvMsg::showRequestedOptions(SmartPtr<TSrvOptOptionRequest> oro) {
    ostringstream x;
    int i = oro->count();
    x << i << " opts";
    if (i)
	x << ":";
    // FIXME: change to real DigestType
    this->DigestType = DIGEST_HMAC_SHA1;
    for (i=0;i<oro->count();i++) {
	x << " " << oro->getReqOpt(i);
    }
    return x.str();
}

/** 
 * This function creates FQDN option and executes DNS Update procedure (if necessary)
 * 
 * @param requestFQDN 
 * @param clntDuid 
 * @param clntAddr 
 * @param doRealUpdate - should the real update be performed (for example if response for
 *                       SOLICIT is prepare, this should be set to false)
 * 
 * @return 
 */
SPtr<TSrvOptFQDN> TSrvMsg::prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
				       SPtr<TIPv6Addr> clntAddr, string hint, bool doRealUpdate) {

    SmartPtr<TSrvOptFQDN> optFQDN;
    SmartPtr<TSrvCfgIface> ptrIface = SrvCfgMgr->getIfaceByID( this->Iface );
    if (!ptrIface) {
	Log(Crit) << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << LogEnd;
	this->IsDone = true;
	return 0;
    }
    // FQDN is chosen, "" if no name for this host is found.

    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr=" << clntAddr->getPlain() << LogEnd;
	
    SPtr<TFQDN> fqdn = ptrIface->getFQDNName(clntDuid,clntAddr, hint);
    if (!fqdn) {
	Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
	return 0;
    } 

    optFQDN = new TSrvOptFQDN(fqdn->getName(), this);
    // FIXME: For the moment, only the client make the DNS update
    optFQDN->setSFlag(false);
    // Setting the O Flag correctly according to the difference between O flags
    optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
    string fqdnName = fqdn->getName();

    if (requestFQDN->getNFlag()) {
	Log(Notice) << "FQDN: No DNS Update required." << LogEnd;
	return optFQDN;
    }

    if (!doRealUpdate) {
	Log(Debug) << "FQDN: Skipping update (probably a SOLICIT message)." << LogEnd;
	return optFQDN;
    }

    fqdn->setUsed(true);

    int FQDNMode = ptrIface->getFQDNMode();
    Log(Debug) << "FQDN: Adding FQDN Option in REPLY message: " << fqdnName << ", FQDNMode=" << FQDNMode << LogEnd;

    if ( FQDNMode==1 || FQDNMode==2 ) {
	Log(Debug) << "FQDN: Server configuration allow DNS updates for " << clntDuid->getPlain() << LogEnd;
	
	if (FQDNMode == 1) optFQDN->setSFlag(false);
	else if (FQDNMode == 2)  optFQDN->setSFlag(true); // letting client update his AAAA
	// Setting the O Flag correctly according to the difference between O flags
	optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
	// Here we check if all parameters are set, and do the DNS update if possible
	List(TIPv6Addr) DNSSrvLst = *ptrIface->getDNSServerLst();
	SmartPtr<TIPv6Addr> DNSAddr;
	
	// For the moment, we just take the first DNS entry.
	DNSSrvLst.first();
	DNSAddr = DNSSrvLst.get();
	
	SmartPtr<TAddrClient> ptrAddrClient = SrvAddrMgr->getClient(clntDuid);	
	if (!ptrAddrClient) { 
	    Log(Warning) << "Unable to find client."; 
	    return 0;
	}
	ptrAddrClient->firstIA();
	SmartPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
	if (!ptrAddrIA) { 
	    Log(Warning) << "Client does not have any addresses assigned." << LogEnd; 
	    return 0;
	}
	ptrAddrIA->firstAddr();
	SmartPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
	SmartPtr<TIPv6Addr> IPv6Addr = addr->get();
	
	Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr << ", IP=" << *IPv6Addr << " and FQDN=" 
                    << fqdnName << LogEnd;
	
	//Test for DNS update
	char zoneroot[128];
	doRevDnsZoneRoot(IPv6Addr->getAddr(), zoneroot, ptrIface->getRevDNSZoneRootLength());
#ifndef MOD_SRV_DISABLE_DNSUPDATE
	if (FQDNMode==1){
	    /* add PTR only */
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), DNSUPDATE_PTR);
	    result = act->run();
	    act->showResult(result);
	    delete act;
	} // fqdnMode == 1
	else if (FQDNMode==2){
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), DNSUPDATE_PTR);
	    result = act->run();
	    act->showResult(result);
	    delete act;
	    
	    DnsUpdateResult result2 = DNSUPDATE_SKIP;
	    DNSUpdate *act2 = new DNSUpdate(DNSAddr->getPlain(), "", fqdnName, IPv6Addr->getPlain(), DNSUPDATE_AAAA);
	    result2 = act2->run();
	    act2->showResult(result);
	    delete act;
	} // fqdnMode == 2

	// regardless of the result, store the info
	ptrAddrIA->setFQDN(fqdn);
	ptrAddrIA->setFQDNDnsServer(DNSAddr);
	
#else
	Log(Error) << "This server is compiled without DNS Update support." << LogEnd;
#endif

    } else {
	Log(Debug) << "Server configuration does NOT allow DNS updates for " << *clntDuid << LogEnd;
	optFQDN->setNFlag(true);
    }
    
    return optFQDN;
}

void TSrvMsg::fqdnRelease(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn)
{
#ifdef MOD_CLNT_DISABLE_DNSUPDATE
    Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
    return;
#else

    string fqdnName = fqdn->getName();
    int FQDNMode = ptrIface->getFQDNMode();
    fqdn->setUsed(false);
    int result;

    SPtr<TIPv6Addr> dns = ptrIA->getFQDNDnsServer();
    if (!dns) {
	Log(Warning) << "Unable to find DNS Server for IA=" << ptrIA->getIAID() << LogEnd;
	return;
    }

    ptrIA->firstAddr();
    SPtr<TAddrAddr> addrAddr = ptrIA->getAddr();
    SPtr<TIPv6Addr> clntAddr;
    if (!addrAddr) {
	Log(Error) << "Client does not have any addresses asigned to IA (IAID=" << ptrIA->getIAID() << ")." << LogEnd;
	return;
    }
    clntAddr = addrAddr->get();

    char zoneroot[128];
    doRevDnsZoneRoot(clntAddr->getAddr(), zoneroot, ptrIface->getRevDNSZoneRootLength());

    if (FQDNMode == DNSUPDATE_MODE_PTR){
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), DNSUPDATE_PTR_CLEANUP);
	result = act->run();
	act->showResult(result);
	delete act;
	
    } // fqdn mode 1 (PTR only)
    else if (FQDNMode == DNSUPDATE_MODE_BOTH){
	/* AAAA Cleanup */
	Log(Notice) << "FQDN: Attempting to clean up AAAA and PTR record in DNS Server " << * dns << ", IP = " 
		    << *clntAddr << " and FQDN=" << fqdn->getName() << LogEnd;
	
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdnName, clntAddr->getPlain(), DNSUPDATE_AAAA_CLEANUP);
	result = act->run();
	act->showResult(result);
	delete act;
	
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act2 = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), DNSUPDATE_PTR_CLEANUP);
	result = act2->run();
	act2->showResult(result);
	delete act2;
    } // fqdn mode 2 (AAAA and PTR)
#endif
}

bool TSrvMsg::check(bool clntIDmandatory, bool srvIDmandatory) {
    bool status = TMsg::check(clntIDmandatory, srvIDmandatory);

    SPtr<TSrvOptServerIdentifier> optSrvID = (Ptr*) this->getOption(OPTION_SERVERID);
    if (optSrvID) {
	if ( !( *(SrvCfgMgr->getDUID()) == *(optSrvID->getDUID()) ) ) {
	    Log(Debug) << "Wrong ServerID value detected. This message is not for me. Message ignored." << LogEnd;
	    return false;
	}
    }

    return status;
}

