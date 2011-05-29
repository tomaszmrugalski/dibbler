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
 * $Id: SrvMsg.cpp,v 1.58 2008-11-13 22:40:26 thomson Exp $
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
#include "OptEmpty.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptPreference.h"
#include "SrvOptElapsed.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "OptGeneric.h"
#include "SrvOptLQ.h"
#include "SrvOptTA.h"
#include "SrvCfgOptions.h"

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

#ifndef MOD_SRV_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

#ifndef MOD_DISABLE_AUTH
#include "SrvOptAAAAuthentication.h"
#include "SrvOptKeyGeneration.h"
#include "SrvOptAuthentication.h"
#endif

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
TSrvMsg::TSrvMsg(int iface, SPtr<TIPv6Addr> addr, int msgType, long transID)
                 :TMsg(iface, addr, msgType, transID)
{
    this->Relays = 0;

#ifndef MOD_DISABLE_AUTH
    this->DigestType = SrvCfgMgr().getDigest();
    this->AuthKeys = SrvCfgMgr().AuthKeys;
#endif

    FirstTimeStamp = now();
    this->MRT=0;
    KeyGenNonce = NULL;
    KeyGenNonceLen = 0;
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
TSrvMsg::TSrvMsg(int iface,  SPtr<TIPv6Addr> addr,
                 char* buf,  int bufSize)
                 :TMsg(iface, addr, buf, bufSize)
{
    this->Relays = 0;

#ifndef MOD_DISABLE_AUTH
    this->AuthKeys = SrvCfgMgr().AuthKeys;
    this->DigestType = SrvCfgMgr().getDigest();
#endif

    FirstTimeStamp = now();
    this->MRT=0;
    KeyGenNonce = NULL;
    KeyGenNonceLen = 0;
	
    int pos=0;
    while (pos<bufSize)	{
	if (pos+4>bufSize) {
	    Log(Error) << "Message " << MsgType << " truncated. There are " << (bufSize-pos) 
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	    break;
	}


        unsigned short code   = buf[pos]*256 + buf[pos+1];
        pos+=2;
        unsigned short length = buf[pos]*256 + buf[pos+1];
        pos+=2;

	if (pos+length>bufSize) {
	    Log(Error) << "Malformed option (type=" << code << ", len=" << length 
		       << ", offset from beginning of DHCPv6 message=" << pos+4 /* 4=msg-type+trans-id */
		       << ") received (msgtype=" << MsgType << "). Message dropped." << LogEnd;
	    IsDone = true;
	    return;
	}

        SPtr<TOpt> ptr;

	if (!allowOptInMsg(MsgType,code)) {
	    Log(Warning) << "Option " << code << " not allowed in message type="<< MsgType <<". Option ignored." << LogEnd;
	    pos+=length;
	    continue;
	}
	if (!allowOptInOpt(MsgType,0,code)) {
	    Log(Warning) <<"Option " << code << " can't be present in message (type="
			 << MsgType <<") directly. Option ignored." << LogEnd;
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
	case OPTION_IA_NA:
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
	    ptr = new TOptEmpty(code, buf+pos,length,this);
	    break;
	case OPTION_DNS_SERVERS:
	    ptr = new TSrvOptDNSServers(buf+pos,length,this);
            break;
	case OPTION_SNTP_SERVERS:
	    ptr = new TSrvOptNTPServers(buf+pos,length,this);
	    break;
	case OPTION_DOMAIN_LIST:
	    ptr = new TSrvOptDomainName(buf+pos, length,this);
	    break;
	case OPTION_NEW_TZDB_TIMEZONE:
	    ptr = new TSrvOptTimeZone(buf+pos, length,this);
	    break;
	case OPTION_SIP_SERVER_A:
	    ptr = new TSrvOptSIPServers(buf+pos, length, this);
	    break;
	case OPTION_SIP_SERVER_D:
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
	case OPTION_INFORMATION_REFRESH_TIME:
	    ptr = new TSrvOptLifetime(buf+pos, length, this);
	    break;
	case OPTION_IA_TA:
	    ptr = new TSrvOptTA(buf+pos, length, this);
	    break;
	case OPTION_IA_PD:
	    ptr = new TSrvOptIA_PD(buf+pos, length, this);
	    break;
	case OPTION_LQ_QUERY:
	    ptr = new TSrvOptLQ(buf+pos, length, this);
	    break;
	    // remaining LQ options are not supported to be received by server

#ifndef MOD_DISABLE_AUTH
	case OPTION_AAAAUTH:
	    if (SrvCfgMgr().getDigest() != DIGEST_NONE) {
		this->DigestType = DIGEST_HMAC_SHA1;
		ptr = new TSrvOptAAAAuthentication(buf+pos, length, this);
	    }
	    break;
	case OPTION_KEYGEN:
	    Log(Warning) << "Option OPTION_KEYGEN received by server is invalid, ignoring." << LogEnd;
	    break;
	case OPTION_AUTH:
	    if (SrvCfgMgr().getDigest() != DIGEST_NONE) {
		this->DigestType = SrvCfgMgr().getDigest();
		
		ptr = new TSrvOptAuthentication(buf+pos, length, this);
		SPtr<TOptDUID> optDUID = (SPtr<TOptDUID>)this->getOption(OPTION_CLIENTID);
		if (optDUID) {
		    SPtr<TAddrClient> client = SrvAddrMgr().getClient(optDUID->getDUID());
		    if (client)
			client->setSPI(SPI);
		}
	    }
#endif
	    
        break;
	case OPTION_VENDOR_OPTS:
	    ptr = new TOptVendorSpecInfo(code, buf+pos, length, this);
	    break;
	case OPTION_RECONF_ACCEPT:
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
	case OPTION_RECONF_MSG:
	case OPTION_RELAY_MSG:
	default:
	    Log(Warning) << "Option type " << code << " not supported yet." << LogEnd;
	    break;
	}
	if ( (ptr) && (ptr->isValid()) )
	    Options.push_back( ptr );
	else
	    Log(Warning) << "Option type " << code << " invalid. Option ignored." << LogEnd;
        pos+=length;
    }

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

void TSrvMsg::addRelayInfo(SPtr<TIPv6Addr> linkAddr,
			   SPtr<TIPv6Addr> peerAddr,
			   int hop,
			   SPtr<TSrvOptInterfaceID> interfaceID,
                           List(TOptGeneric) echolist) {
    this->LinkAddrTbl[this->Relays] = linkAddr;
    this->PeerAddrTbl[this->Relays] = peerAddr;
    this->HopTbl[this->Relays]      = hop;
    this->InterfaceIDTbl[this->Relays] = interfaceID;
    this->EchoListTbl[this->Relays]    = echolist;
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

    SPtr<TOptGeneric> gen;
    SPtr<TSrvIfaceIface> ptrIface;
    SPtr<TSrvIfaceIface> under;
    ptrIface = (Ptr*) SrvIfaceMgr().getIfaceByID(this->Iface);
    Log(Notice) << "Sending " << this->getName() << " on " << ptrIface->getName() << "/" << this->Iface
		<< hex << ",transID=0x" << this->getTransID() << dec << ", opts:";
    SPtr<TOpt> ptrOpt;
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

	// calculate lengths of all relays
	len[Relays-1]=this->getSize();
	for (int i=this->Relays-1; i>0; i--) {
	    len[i-1]= len[i] + 38; // 34 bytes (relay header) + 4 bytes (relay-msg option header)
	    if (this->InterfaceIDTbl[i] && (SrvCfgMgr().getInterfaceIDOrder()!=SRV_IFACE_ID_ORDER_NONE)) {
		len[i-1]+=this->InterfaceIDTbl[i]->getSize();

		EchoListTbl[i].first();
		while (gen = EchoListTbl[i].get()) {
		    len[i-1] += gen->getSize();
		}

	    }
	}

	// recursive storeSelf
	offset += storeSelfRelay(buf, 0, SrvCfgMgr().getInterfaceIDOrder() );

	// check if there are underlaying interfaces
	for (int i=0; i < this->Relays; i++) {
	    under = ptrIface->getUnderlaying();
	    if (!under) {
		Log(Error) << "Sending message on the " << ptrIface->getName() << "/" << ptrIface->getID()
			   << " failed: No underlaying interface found." << LogEnd;
		return;
	    }
	    ptrIface = under;
	}
	Log(Debug) << "Sending " << this->getSize() << "(packet)+" << offset << "(relay headers) data on the "
		   << ptrIface->getFullName() << " interface." << LogEnd;
    } else {
	offset += this->storeSelf(buf+offset);
    }
    SrvIfaceMgr().send(ptrIface->getID(), buf, offset, this->PeerAddr, port);
}

void TSrvMsg::copyRemoteID(SPtr<TSrvMsg> q) {
  this->RemoteID = q->getRemoteID();
}

bool TSrvMsg::copyClientID(SPtr<TMsg> donor) {
    SPtr<TOpt> x = donor->getOption(OPTION_CLIENTID);
    if (x) {
	SPtr<TOptDUID> optDuid = (Ptr*) x;
	ClientDUID = optDuid->getDUID();
	return true;
    }
    return false;
}

void TSrvMsg::copyRelayInfo(SPtr<TSrvMsg> q) {
    this->Relays = q->Relays;
    for (int i=0; i < this->Relays; i++) {
	this->LinkAddrTbl[i]   = q->LinkAddrTbl[i];
	this->PeerAddrTbl[i]   = q->PeerAddrTbl[i];
	this->InterfaceIDTbl[i]= q->InterfaceIDTbl[i];
	this->HopTbl[i]        = q->HopTbl[i];
	this->EchoListTbl[i]   = q->EchoListTbl[i];
    }
}


/** 
 * stores message in a buffer, used in relayed messages
 * 
 * @param buf buffer for data to be stored
 * @param relayDepth number or current relay level
 * @param order order of the interface-id option (before, after or omit)
 * 
 * @return - number of bytes used
 */
int TSrvMsg::storeSelfRelay(char * buf, int relayDepth, ESrvIfaceIdOrder order)
{
    int offset = 0;
    if (relayDepth == this->Relays) {
	return storeSelf(buf);
    }
    buf[offset++] = RELAY_REPL_MSG;
    buf[offset++] = HopTbl[relayDepth];
    LinkAddrTbl[relayDepth]->storeSelf(buf+offset);
    PeerAddrTbl[relayDepth]->storeSelf(buf+offset+16);
    offset += 32;

    if (order == SRV_IFACE_ID_ORDER_BEFORE)
    {
	if (InterfaceIDTbl[relayDepth]) {
	    InterfaceIDTbl[relayDepth]->storeSelf(buf+offset);
	    offset += InterfaceIDTbl[relayDepth]->getSize();
	}
    }

    *(short*)(buf+offset) = htons(OPTION_RELAY_MSG);
    offset+=2;
    *(short*)(buf+offset) = htons(len[relayDepth]);
    offset+=2;
    
    offset += storeSelfRelay(buf+offset, relayDepth+1, order);

    if (order == SRV_IFACE_ID_ORDER_AFTER)
    {
	if (InterfaceIDTbl[relayDepth]) {
	    InterfaceIDTbl[relayDepth]->storeSelf(buf+offset);
	    offset += InterfaceIDTbl[relayDepth]->getSize();
	}
    }

    SPtr<TOptGeneric> gen;
    EchoListTbl[relayDepth].first();
    while (gen = EchoListTbl[relayDepth].get()) {
	Log(Debug) << "Echoing back option " << gen->getOptType() << ", length " << gen->getSize() << LogEnd;
	gen->storeSelf(buf+offset);
	offset += gen->getSize();
    }
    
    return offset;
}


void TSrvMsg::copyAAASPI(SPtr<TSrvMsg> q) {
#ifndef MOD_DISABLE_AUTH
    this->AAASPI = q->getAAASPI();
    this->SPI = q->getSPI();
    this->AuthInfoKey = q->getAuthInfoKey();
#endif
}

/** 
 * this function appends authentication option
 * 
 */
void TSrvMsg::appendAuthenticationOption(SPtr<TDUID> duid)
{

#ifndef MOD_DISABLE_AUTH
    if (!duid) {
        Log(Error) << "Auth: No duid! Probably internal error. Authentication option not appended." << LogEnd;
        return;
    }

    DigestType = SrvCfgMgr().getDigest();
    if (DigestType == DIGEST_NONE) {
        // Log(Debug) << "Auth: Authentication is disabled." << LogEnd;
        return;
    }

    Log(Debug) << "Auth: Setting DigestType to: " << this->DigestType << LogEnd;

    if (!getOption(OPTION_AUTH)) {
        SPtr<TAddrClient> client = SrvAddrMgr().getClient(duid);
        if (client && !client->getSPI() && this->getSPI())
            client->setSPI(this->getSPI());

        if (client)
            this->ReplayDetection = client->getNextReplayDetectionSent();
        else
            this->ReplayDetection = 1;
        Options.push_back(new TSrvOptAuthentication(this));
    }
#endif
}

bool TSrvMsg::appendMandatoryOptions(SPtr<TSrvOptOptionRequest> oro, bool clientID /* =true */)
{
    // include our DUID (Server ID)
    SPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(SrvCfgMgr().getDUID(),this);
    Options.push_back((Ptr*)ptrSrvID);
    oro->delOption(OPTION_SERVERID);

    // include his DUID (Client ID)
    if (clientID && ClientDUID) {
	SPtr<TOptDUID> clientDuid = new TOptDUID(OPTION_CLIENTID, ClientDUID, this);
	Options.push_back( (Ptr*)clientDuid);
    }

    // ... and our preference
    SPtr<TSrvOptPreference> ptrPreference;
    unsigned char preference = SrvCfgMgr().getIfaceByID(Iface)->getPreference();
    Log(Debug) << "Preference set to " << (int)preference << "." << LogEnd;
    ptrPreference = new TSrvOptPreference(preference,this);
    Options.push_back((Ptr*)ptrPreference);
    oro->delOption(OPTION_PREFERENCE);

    // does this server support unicast?
    SPtr<TIPv6Addr> unicastAddr = SrvCfgMgr().getIfaceByID(Iface)->getUnicast();
    if (unicastAddr) {
	SPtr<TSrvOptServerUnicast> optUnicast = new TSrvOptServerUnicast(unicastAddr, this);
	Options.push_back((Ptr*)optUnicast);
	oro->delOption(OPTION_UNICAST);
    }
    
    return true;
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
bool TSrvMsg::appendRequestedOptions(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr, 
				     int iface, SPtr<TSrvOptOptionRequest> reqOpts)
{
    bool newOptionAssigned = false;
    // client didn't want any option? Or maybe we're not supporting this client?
    if (!reqOpts->count() || !SrvCfgMgr().isClntSupported(duid,addr,iface))
	return false;

    SPtr<TSrvCfgIface>  ptrIface=SrvCfgMgr().getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Unable to find interface (ifindex=" << iface << "). Something is wrong." << LogEnd;
	return false;
    }

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(duid, getRemoteID(), false/* false = verbose */);

    // --- option: DNS resolvers ---
    if ( reqOpts->isOption(OPTION_DNS_SERVERS) && ptrIface->supportDNSServer() ) {
	SPtr<TSrvOptDNSServers> optDNS;
	if (ex && ex->supportDNSServer())
	    optDNS = new TSrvOptDNSServers(*ex->getDNSServerLst(), this);
	else
	    optDNS = new TSrvOptDNSServers(*ptrIface->getDNSServerLst(),this);
	Options.push_back((Ptr*)optDNS);
	newOptionAssigned = true;
    };

    // --- option: DOMAIN LIST ---
    if ( reqOpts->isOption(OPTION_DOMAIN_LIST) && ptrIface->supportDomain() ) {
	SPtr<TSrvOptDomainName> optDomain;
	if (ex && ex->supportDomain())
	    optDomain = new TSrvOptDomainName(*ex->getDomainLst(),this);
	else
	    optDomain = new TSrvOptDomainName(*ptrIface->getDomainLst(),this);
	Options.push_back((Ptr*)optDomain);
	newOptionAssigned = true;
    };
    
    // --- option: NTP servers ---
    if ( reqOpts->isOption(OPTION_SNTP_SERVERS) && ptrIface->supportNTPServer() ) {
	SPtr<TSrvOptNTPServers> optNTP;
	if (ex && ex->supportNTPServer())
	    optNTP = new TSrvOptNTPServers(*ex->getNTPServerLst(),this);
	else
	    optNTP = new TSrvOptNTPServers(*ptrIface->getNTPServerLst(),this);
	Options.push_back((Ptr*)optNTP);
	newOptionAssigned = true;
    };
    
    // --- option: TIMEZONE --- 
    if ( reqOpts->isOption(OPTION_NEW_TZDB_TIMEZONE) && ptrIface->supportTimezone() ) {
	SPtr<TSrvOptTimeZone> optTimezone;
	if (ex && ex->supportTimezone())
	    optTimezone = new TSrvOptTimeZone(ex->getTimezone(),this);
	else
	    optTimezone = new TSrvOptTimeZone(ptrIface->getTimezone(),this);
	Options.push_back((Ptr*)optTimezone);
	newOptionAssigned = true;
    };

    // --- option: SIP SERVERS ---
    if ( reqOpts->isOption(OPTION_SIP_SERVER_A) && ptrIface->supportSIPServer() ) {
	SPtr<TSrvOptSIPServers> optSIPServer;
	if (ex && ex->supportSIPServer())
	    optSIPServer = new TSrvOptSIPServers(*ex->getSIPServerLst(),this);
	else
	    optSIPServer = new TSrvOptSIPServers(*ptrIface->getSIPServerLst(),this);
	Options.push_back((Ptr*)optSIPServer);
	newOptionAssigned = true;
    };

    // --- option: SIP DOMAINS ---
    if ( reqOpts->isOption(OPTION_SIP_SERVER_D) && ptrIface->supportSIPDomain() ) {
	SPtr<TSrvOptSIPDomain> optSIPDomain;
	if (ex && ex->supportSIPDomain())
	    optSIPDomain= new TSrvOptSIPDomain(*ex->getSIPDomainLst(),this);
	else
	    optSIPDomain= new TSrvOptSIPDomain(*ptrIface->getSIPDomainLst(),this);
	Options.push_back((Ptr*)optSIPDomain);
	newOptionAssigned = true;
    };

    // --- option: FQDN ---
    // see prepareFQDN() method

    // --- option: NIS SERVERS ---
    if ( reqOpts->isOption(OPTION_NIS_SERVERS) && ptrIface->supportNISServer() ) {
	SPtr<TSrvOptNISServers> opt;
	if (ex && ex->supportNISServer())
	    opt = new TSrvOptNISServers(*ex->getNISServerLst(),this);
	else
	    opt = new TSrvOptNISServers(*ptrIface->getNISServerLst(),this);
	Options.push_back((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: NIS DOMAIN ---
    if ( reqOpts->isOption(OPTION_NIS_DOMAIN_NAME) && ptrIface->supportNISDomain() ) {
	SPtr<TSrvOptNISDomain> opt;
	if (ex && ex->supportNISDomain())
	    opt = new TSrvOptNISDomain(ex->getNISDomain(),this);
	else
	    opt = new TSrvOptNISDomain(ptrIface->getNISDomain(),this);
	Options.push_back((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: NISP SERVERS ---
    if ( reqOpts->isOption(OPTION_NISP_SERVERS) && ptrIface->supportNISPServer() ) {
	SPtr<TSrvOptNISPServers> opt;
	if (ex && ex->supportNISPServer())
	    opt = new TSrvOptNISPServers(*ex->getNISPServerLst(),this);
	else
	    opt = new TSrvOptNISPServers(*ptrIface->getNISPServerLst(),this);
	Options.push_back((Ptr*) opt);
	newOptionAssigned = true;
    };

    // --- option: NISP DOMAIN ---
    if ( reqOpts->isOption(OPTION_NISP_DOMAIN_NAME) && ptrIface->supportNISPDomain() ) {
	SPtr<TSrvOptNISPDomain> opt;
	if (ex && ex->supportNISPDomain())
	    opt = new TSrvOptNISPDomain(ex->getNISPDomain(), this);
	else
	    opt = new TSrvOptNISPDomain(ptrIface->getNISPDomain(), this);
	Options.push_back((Ptr*)opt);
	newOptionAssigned = true;
    };

    // --- option: VENDOR SPEC ---
    if ( reqOpts->isOption(OPTION_VENDOR_OPTS)) {
	if (appendVendorSpec(duid, iface, 0, reqOpts))
	    newOptionAssigned = true;
    }

    // --- option: INFORMATION_REFRESH_TIME ---
    // this option should be checked last 
    if ( newOptionAssigned && ptrIface->supportLifetime() ) {
	SPtr<TSrvOptLifetime> optLifetime;
	if (ex && ex->supportLifetime())
	    optLifetime = new TSrvOptLifetime(ex->getLifetime(), this);
	else
	    optLifetime = new TSrvOptLifetime(ptrIface->getLifetime(), this);
	Options.push_back( (Ptr*)optLifetime);
	newOptionAssigned = true;
    }

    // --- option: forced options first ---
    TOptList extraOpts; // possible additional options
    TOptList forcedOpts; // forced (asked for or not, will send them anyway)
    
    if (ex) {
        extraOpts = ex->getExtraOptions();
        forcedOpts = ex->getForcedOptions();
    } 

    if (!ex || !extraOpts.size())
        extraOpts = ptrIface->getExtraOptions();
    
    if (!ex || !forcedOpts.size())
        forcedOpts = ptrIface->getForcedOptions();

    for (TOptList::iterator gen = forcedOpts.begin(); gen!=forcedOpts.end(); ++gen)
    {
	Log(Debug) << "Appending mandatory extra option " << (*gen)->getOptType() 
		   << " (" << (*gen)->getSize() << ")" << LogEnd;
	Options.push_back( (Ptr*) *gen);
        reqOpts->delOption( (*gen)->getOptType() );
	newOptionAssigned = true;
    }

    for (TOptList::iterator gen = extraOpts.begin(); gen!=extraOpts.end(); ++gen)
    {
	if (reqOpts->isOption( (*gen)->getOptType()))
	{
	    Log(Debug) << "Appending requested extra option " << (*gen)->getOptType() 
		       << " (" << (*gen)->getSize() << ")" << LogEnd;
	    Options.push_back( (Ptr*) *gen);
	    newOptionAssigned = true;
	}
    }

    // --- option: KEYGEN ---
#ifndef MOD_DISABLE_AUTH
    if ( reqOpts->isOption(OPTION_KEYGEN) && SrvCfgMgr().getDigest() != DIGEST_NONE ) 
    { // && this->MsgType == ADVERTISE_MSG ) {
        SPtr<TSrvOptKeyGeneration> optKeyGeneration = new TSrvOptKeyGeneration(this);
        Options.push_back( (Ptr*)optKeyGeneration);
    }
#endif

    return newOptionAssigned;
}

/**
 * this function prints all options specified in the ORO option
 */
string TSrvMsg::showRequestedOptions(SPtr<TSrvOptOptionRequest> oro) {
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

/** 
 * creates FQDN option and executes DNS Update procedure (if necessary)
 * 
 * @param requestFQDN  requested Fully Qualified Domain Name
 * @param clntDuid     client DUID
 * @param clntAddr     client address
 * @param hint         hint used to get name (it may or may not be used)
 * @param doRealUpdate - should the real update be performed (for example if response for
 *                       SOLICIT is prepare, this should be set to false)
 * 
 * @return FQDN option
 */
SPtr<TSrvOptFQDN> TSrvMsg::prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
				       SPtr<TIPv6Addr> clntAddr, string hint, bool doRealUpdate) {

    SPtr<TSrvOptFQDN> optFQDN;
    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID( this->Iface );
    if (!ptrIface) {
	Log(Crit) << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << LogEnd;
	this->IsDone = true;
	return 0;
    }
    // FQDN is chosen, "" if no name for this host is found.
    if (!ptrIface->supportFQDN()) {
	Log(Error) << "FQDN is not defined on " << ptrIface->getFullName() << " interface." << LogEnd;
	return 0;
    }
    
    if (!ptrIface->supportDNSServer()) {
	Log(Error) << "DNS server is not supported on " << ptrIface->getFullName() << " interface. DNS Update aborted." << LogEnd;
	return 0;
    }
    
    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr=" << clntAddr->getPlain() << LogEnd;
	
    SPtr<TFQDN> fqdn = ptrIface->getFQDNName(clntDuid,clntAddr, hint);
    if (!fqdn) {
	Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
	return 0;
    } 

    optFQDN = new TSrvOptFQDN(fqdn->getName(), this);
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
	
	if (FQDNMode == 1) 
	    optFQDN->setSFlag(false);
	else 
	    if (FQDNMode == 2) 
		optFQDN->setSFlag(true); // letting client update his AAAA
	// Setting the O Flag correctly according to the difference between O flags
	optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
	
	SPtr<TIPv6Addr> DNSAddr = SrvCfgMgr().fqdnDdnsAddress();
	if (!DNSAddr) {
	    Log(Debug) << "DDNS: DDNS address not specified, using first address used in DNS nameserver option" << LogEnd;

	    // Here we check if all parameters are set, and do the DNS update if possible
	    List(TIPv6Addr) DNSSrvLst = *ptrIface->getDNSServerLst();
	    
	    // For the moment, we just take the first DNS entry.
	    DNSSrvLst.first();
	    if (DNSSrvLst.count())
		DNSAddr = DNSSrvLst.get();
	}
	if (!DNSAddr) {
	    Log(Error) << "DDNS: DNS Update aborted. DNS server address is not specified." << LogEnd;
	    return 0;
	}
      	
	SPtr<TAddrClient> ptrAddrClient = SrvAddrMgr().getClient(clntDuid);	
	if (!ptrAddrClient) { 
	    Log(Warning) << "Unable to find client."; 
	    return 0;
	}
	ptrAddrClient->firstIA();
	SPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
	if (!ptrAddrIA) { 
	    Log(Warning) << "Client does not have any addresses assigned." << LogEnd; 
	    return 0;
	}
	ptrAddrIA->firstAddr();
	SPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
	SPtr<TIPv6Addr> IPv6Addr = addr->get();
	
	Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr << ", IP=" 
		    << *IPv6Addr << " and FQDN=" << fqdnName << LogEnd;
      	
	//Test for DNS update
	char zoneroot[128];
	doRevDnsZoneRoot(IPv6Addr->getAddr(), zoneroot, ptrIface->getRevDNSZoneRootLength());
#ifndef MOD_SRV_DISABLE_DNSUPDATE
	TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
	DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;

	// that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
	// and that would include both poslib and Dibbler headers in one place. Universe would implode then.
	if (proto == TCfgMgr::DNSUPDATE_UDP)
	    proto2 = DNSUpdate::DNSUPDATE_UDP;
	if (proto == TCfgMgr::DNSUPDATE_ANY)
	    proto2 = DNSUpdate::DNSUPDATE_ANY;

	unsigned int timeout = SrvCfgMgr().getDDNSTimeout();
	
	if (FQDNMode==1){
	    /* add PTR only */
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), 
					   DNSUPDATE_PTR, proto2);
	    result = act->run(timeout);
	    act->showResult(result);
	    delete act;
	} // fqdnMode == 1
	else if (FQDNMode==2){
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), 
					   DNSUPDATE_PTR, proto2);
	    result = act->run(timeout);
	    act->showResult(result);
	    delete act;
      	    
	    DnsUpdateResult result2 = DNSUPDATE_SKIP;
	    DNSUpdate *act2 = new DNSUpdate(DNSAddr->getPlain(), "", fqdnName, IPv6Addr->getPlain(), 
					    DNSUPDATE_AAAA, proto2);
	    result2 = act2->run(timeout);
	    act2->showResult(result);
	    delete act2;
	} // fqdnMode == 2
	
	// regardless of the result, store the info
	ptrAddrIA->setFQDN(fqdn);
	ptrAddrIA->setFQDNDnsServer(DNSAddr);
	
#else
      	Log(Error) << "This server is compiled without DNS Update support." << LogEnd;
#endif
    } else {
	Log(Debug) << "Server configuration does NOT allow DNS updates for " << clntDuid->getPlain() << LogEnd;
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

    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    if (FQDNMode == DNSUPDATE_MODE_PTR){
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), DNSUPDATE_PTR_CLEANUP);
	result = act->run(timeout);
	act->showResult(result);
	delete act;
	
    } // fqdn mode 1 (PTR only)
    else if (FQDNMode == DNSUPDATE_MODE_BOTH){
	/* AAAA Cleanup */
	Log(Notice) << "FQDN: Attempting to clean up AAAA and PTR record in DNS Server " << * dns << ", IP = " 
		    << *clntAddr << " and FQDN=" << fqdn->getName() << LogEnd;
	
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdnName, clntAddr->getPlain(), DNSUPDATE_AAAA_CLEANUP);
	result = act->run(timeout);
	act->showResult(result);
	delete act;
	
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act2 = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), DNSUPDATE_PTR_CLEANUP);
	result = act2->run(timeout);
	act2->showResult(result);
	delete act2;
    } // fqdn mode 2 (AAAA and PTR)
#endif
}

bool TSrvMsg::check(bool clntIDmandatory, bool srvIDmandatory) {
    bool status = TMsg::check(clntIDmandatory, srvIDmandatory);

    SPtr<TSrvOptServerIdentifier> optSrvID = (Ptr*) this->getOption(OPTION_SERVERID);
    if (optSrvID) {
	if ( !( *(SrvCfgMgr().getDUID()) == *(optSrvID->getDUID()) ) ) {
	    Log(Debug) << "Wrong ServerID value detected. This message is not for me. Message ignored." << LogEnd;
	    return false;
	}
    }
    return status;
}

bool TSrvMsg::appendVendorSpec(SPtr<TDUID> duid, int iface, int vendor, SPtr<TSrvOptOptionRequest> reqOpt)
{
    reqOpt->delOption(OPTION_VENDOR_OPTS);

    SPtr<TSrvCfgIface> ptrIface=SrvCfgMgr().getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Unable to find interface with ifindex=" << iface << LogEnd;
	return false;
    }

    Log(Debug) << "Client requested vendor-spec. info (vendor=" << vendor 
               << ")." << LogEnd;

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(duid, true);
    SPtr<TOptVendorSpecInfo> vs;
    List(TOptVendorSpecInfo) vsLst;

    if (ex) {
        vsLst = ex->getVendorSpecLst(vendor);
    } else {
        vsLst = ptrIface->getVendorSpecLst(vendor);
    }


    if (!vsLst.count())
        vsLst = ptrIface->getVendorSpecLst(vendor);
        
    if (vsLst.count()) {
        vsLst.first();
        while (vs=vsLst.get())
        {
            Options.push_back( (Ptr*)vs);
        }
        return true;
    }

    Log(Debug) << "Unable to find suitable vendor-specific info option." << LogEnd;
    return false;
}

bool TSrvMsg::validateReplayDetection() {

#ifndef MOD_DISABLE_AUTH
    if (this->MsgType == SOLICIT_MSG)
        return true;

    if (SrvCfgMgr().getDigest() ==  DIGEST_NONE) {
        // Log(Debug) << "Auth: Authentication is disabled." << LogEnd;
        return true;
    }

    SPtr<TAddrClient> client = SrvAddrMgr().getClient(SPI);
    if (!client) {
        Log(Debug) << "Auth: Unable to find client with SPI=" << SPI << "." << LogEnd;
        return true;
    }

    if (!client->getReplayDetectionRcvd() && !this->ReplayDetection)
        return true;

    if (client->getReplayDetectionRcvd() < this->ReplayDetection) {
        client->setReplayDetectionRcvd(this->ReplayDetection);
        return true;
    } else {
        Log(Warning) << "Auth: Replayed message detected, message dropped." << LogEnd;
        return false;
    }
#else
    return true;
#endif
}

void TSrvMsg::setRemoteID(SPtr<TOptVendorData> remoteID)
{
    RemoteID = remoteID;
}

SPtr<TOptVendorData> TSrvMsg::getRemoteID()
{
    return RemoteID;
}

/** 
 * copy status-code to top-level if something is wrong (i.e. status-code!=SUCCESS)
 * 
 */
void TSrvMsg::appendStatusCode()
{
    SPtr<TSrvOptStatusCode> rootLevel, optLevel;
    SPtr<TOpt> opt;
    //rootLevel = getOption(OPTION_STATUS_CODE);

    firstOption();
    while (opt = getOption()) {
	switch ( opt->getOptType() ) {
	case OPTION_IA_NA:
	{
	    if (optLevel= (Ptr*)opt->getOption(OPTION_STATUS_CODE)) {
		if (optLevel->getCode() != STATUSCODE_SUCCESS) {
		    // copy status code to root-level
		    delOption(OPTION_STATUS_CODE);
		    rootLevel = new TSrvOptStatusCode(optLevel->getCode(), optLevel->getText(), this);
		    Options.push_back( (Ptr*) rootLevel);
		    return;
		}
	    }
	}
	default:
	{

	}
	}
	
    }

}

void TSrvMsg::handleDefaultOption(SPtr<TOpt> ptrOpt) {
    int opt = ptrOpt->getOptType();
    switch(opt)
    {
    case OPTION_ELAPSED_TIME :
        break;
    default:
	if (!ORO->isOption(opt) && !getOption(opt))
	    ORO->addOption(opt);
	break;
    }
}

/** 
 * finds OPTION_REQUEST OPTION in options
 * 
 * @param msg - parent message
 */
void TSrvMsg::getORO(SPtr<TMsg> msg)
{
    ORO = (Ptr*)msg->getOption(OPTION_ORO);
    if (!ORO)
        ORO = new TSrvOptOptionRequest(this);
}
