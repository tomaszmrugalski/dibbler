/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

#include <sstream>
#include "Portable.h"
#include "SrvMsg.h"
#include "SrvTransMgr.h"
#include "OptEmpty.h"
#include "OptDUID.h"
#include "OptAddr.h"
#include "OptString.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include "OptStatusCode.h"
#include "OptGeneric.h"
#include "SrvOptLQ.h"
#include "SrvOptTA.h"
#include "SrvCfgOptions.h"
#include "SrvOptFQDN.h"
#include "OptAddrLst.h"
#include "OptDomainLst.h"


#ifndef MOD_DISABLE_AUTH
#include "SrvOptAAAAuthentication.h"
#include "SrvOptKeyGeneration.h"
#include "SrvOptAuthentication.h"
#endif

#include "Logger.h"
#include "SrvIfaceMgr.h"
#include "AddrClient.h"

using namespace std;

/** 
 * this constructor is used to build message as a reply to the received message
 * (i.e. it is used to contruct ADVERTISE or REPLY)
 * 
 * @param iface interface index
 * @param addr peer address
 * @param msgType message type
 * @param transID transaction-id
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
 * @param iface 
 * @param addr 
 * @param buf 
 * @param bufSize 
 */
TSrvMsg::TSrvMsg(int iface, SPtr<TIPv6Addr> addr,
                 char* buf, int bufSize)
    :TMsg(iface, addr, buf, bufSize)
{
    Relays = 0;

    for (int i=0; i<32; i++) {
        HopTbl[i] = 0;
        len[i] = 0;
    }

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
	    ptr = new TOptDUID(OPTION_CLIENTID, buf+pos, length, this);
	    break;
	case OPTION_SERVERID:
	    ptr = new TOptDUID(OPTION_SERVERID, buf+pos, length, this);
	    break;
	case OPTION_IA_NA:
	    ptr = new TSrvOptIA_NA(buf+pos,length,this);
	    break;
	case OPTION_ORO:
	    ptr = new TOptOptionRequest(OPTION_ORO, buf+pos, length, this);
	    break;
	case OPTION_PREFERENCE:
	    ptr = new TOptInteger(OPTION_PREFERENCE, 1, buf+pos, length, this);
	    break;
	case OPTION_ELAPSED_TIME:
	    ptr = new TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, buf+pos, length, this);
	    break;
	case OPTION_UNICAST:
	    ptr = new TOptAddr(OPTION_UNICAST, buf+pos, length, this);
	    break;
	case OPTION_STATUS_CODE:
	    ptr = new TOptStatusCode(buf+pos,length,this);
	    break;
	case OPTION_RAPID_COMMIT:
	    ptr = new TOptEmpty(code, buf+pos, length, this);
	    break;
	case OPTION_DNS_SERVERS:
	case OPTION_SNTP_SERVERS:
	case OPTION_SIP_SERVER_A:
	case OPTION_NIS_SERVERS:
	case OPTION_NISP_SERVERS:
	    ptr = new TOptAddrLst(code, buf+pos, length, this);
	    break;
	case OPTION_DOMAIN_LIST:
	case OPTION_SIP_SERVER_D:
	case OPTION_NIS_DOMAIN_NAME:
	case OPTION_NISP_DOMAIN_NAME:
	    ptr = new TOptDomainLst(code, buf+pos, length, this);
	    break;
	case OPTION_NEW_TZDB_TIMEZONE:
	    ptr = new TOptString(OPTION_NEW_TZDB_TIMEZONE, buf+pos, length, this);
	    break;
	case OPTION_FQDN:
	    ptr = new TSrvOptFQDN(buf+pos, length, this);
	    break;
	case OPTION_INFORMATION_REFRESH_TIME:
	    ptr = new TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, buf+pos, length, this);
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
        pos += length;
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

    writeUint16((buf+offset), OPTION_RELAY_MSG);
    offset += sizeof(uint16_t);
    writeUint16((buf+offset), len[relayDepth]);
    offset += sizeof(uint16_t);

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

bool TSrvMsg::appendMandatoryOptions(SPtr<TOptOptionRequest> oro, bool clientID /* =true */)
{
    // include our DUID (Server ID)
    SPtr<TOptDUID> ptrSrvID;
    ptrSrvID = new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this);
    Options.push_back((Ptr*)ptrSrvID);
    oro->delOption(OPTION_SERVERID);

    // include his DUID (Client ID)
    if (clientID && ClientDUID) {
	SPtr<TOptDUID> clientDuid = new TOptDUID(OPTION_CLIENTID, ClientDUID, this);
	Options.push_back( (Ptr*)clientDuid);
    }

    // ... and our preference
    SPtr<TOptInteger> ptrPreference;
    unsigned char preference = SrvCfgMgr().getIfaceByID(Iface)->getPreference();
    Log(Debug) << "Preference set to " << (int)preference << "." << LogEnd;
    ptrPreference = new TOptInteger(OPTION_PREFERENCE, 1, preference, this);
    Options.push_back((Ptr*)ptrPreference);
    oro->delOption(OPTION_PREFERENCE);

    // does this server support unicast?
    SPtr<TIPv6Addr> unicastAddr = SrvCfgMgr().getIfaceByID(Iface)->getUnicast();
    if (unicastAddr) {
	SPtr<TOptAddr> optUnicast = new TOptAddr(OPTION_UNICAST, unicastAddr, this);
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
				     int iface, SPtr<TOptOptionRequest> reqOpts)
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
    // --- option: DOMAIN LIST ---
    // --- option: NTP servers ---
    // --- option: TIMEZONE --- 
    // --- option: SIP SERVERS is now handled with common extra options mechanism ---
    // --- option: SIP DOMAINS is now handled with common extra options mechanism ---
    // --- option: NIS SERVERS is now handled with common extra options mechanism ---
    // --- option: NIS DOMAIN is now handled with common extra options mechanism ---
    // --- option: NISP SERVERS is now handled with common extra options mechanism ---
    // --- option: NISP DOMAIN is now handled with common extra options mechanism ---

    // --- option: FQDN ---
    // see prepareFQDN() method

    // --- option: VENDOR SPEC ---
    if ( reqOpts->isOption(OPTION_VENDOR_OPTS)) {
	if (appendVendorSpec(duid, iface, 0, reqOpts))
	    newOptionAssigned = true;
    }

    // --- option: INFORMATION_REFRESH_TIME ---
    // this option should be checked last 

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
string TSrvMsg::showRequestedOptions(SPtr<TOptOptionRequest> oro) {
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

#if 0
SPtr<TSrvOptFQDN> TSrvMsg::prepareFQDN(SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
				       SPtr<TIPv6Addr> clntAddr, std::string hint, bool doRealUpdate) {
    return SrvTransMgr().addFQDN(this->Iface, requestFQDN, clntDuid, clntAddr, hint, doRealUpdate);
}

void TSrvMsg::fqdnRelease(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn)
{
    SrvTransMgr().removeFQDN(ptrIface, ptrIA, fqdn);
}
#endif

bool TSrvMsg::check(bool clntIDmandatory, bool srvIDmandatory) {
    bool status = TMsg::check(clntIDmandatory, srvIDmandatory);

    SPtr<TOptDUID> optSrvID = (Ptr*) this->getOption(OPTION_SERVERID);
    if (optSrvID) {
	if ( !( *(SrvCfgMgr().getDUID()) == *(optSrvID->getDUID()) ) ) {
	    Log(Debug) << "Wrong ServerID value detected. This message is not for me. Message ignored." << LogEnd;
	    return false;
	}
    }
    return status;
}

bool TSrvMsg::appendVendorSpec(SPtr<TDUID> duid, int iface, int vendor, SPtr<TOptOptionRequest> reqOpt)
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
    SPtr<TOptStatusCode> rootLevel, optLevel;
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
		    rootLevel = new TOptStatusCode(optLevel->getCode(), optLevel->getText(), this);
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
        ORO = new TOptOptionRequest(OPTION_ORO, this);
}
