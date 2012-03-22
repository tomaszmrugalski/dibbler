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
#include "SrvOptTimeZone.h"
#include "SrvOptFQDN.h"
#include "SrvOptLifetime.h"
#include "OptAddrLst.h"
#include "OptDomainLst.h"

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
	    ptr = new TSrvOptTimeZone(buf+pos, length,this);
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

/// Processes options in incoming message and assigns leases and options, if possible.
/// This method may be used in any message that causes server to assign anything, i.e.
/// SOLICIT, REQUEST, RENEW, REBIND (and possibly CONFIRM)
///
/// @param clientMsg client message
/// @param quiet (false will make this method verbose)
void TSrvMsg::processOptions(SPtr<TSrvMsg> clientMsg, bool quiet) {

    SPtr<TOpt>       opt;
    SPtr<TIPv6Addr> clntAddr = PeerAddr;

    // --- process this message ---
    clientMsg->firstOption();
    while ( opt = clientMsg->getOption()) {
        switch (opt->getOptType()) {
        case OPTION_IA_NA : {
            processIA_NA((Ptr*)clientMsg, (Ptr*) opt);
            break;
        }
        case OPTION_IA_TA: {
            processIA_TA((Ptr*)clientMsg, (Ptr*) opt);
            break;
        }
        case OPTION_IA_PD: {
            processIA_PD((Ptr*)clientMsg, (Ptr*) opt);
            break;
        }
        case OPTION_AUTH : {
            ORO->addOption(OPTION_AUTH);
            break;
        }
        case OPTION_FQDN : {
            // skip processing for now (we need to process all IA_NA/IA_TA first)
            break;
        }
        case OPTION_VENDOR_OPTS:
        {
            SPtr<TOptVendorData> v = (Ptr*) opt;
            appendVendorSpec(ClientDUID, Iface, v->getVendor(), ORO);
            break;
        }

        case OPTION_AAAAUTH:
        {
            Log(Debug) << "Auth: Option AAAAuthentication received." << LogEnd;
            break;
        }
        case OPTION_PREFERENCE:
        case OPTION_UNICAST:
        case OPTION_RELAY_MSG:
        case OPTION_INTERFACE_ID:
        case OPTION_STATUS_CODE : {
            Log(Warning) << "Invalid option (" << opt->getOptType() << ") received. Client is not supposed to send it. Option ignored." << LogEnd;
            break;
        }

        // options not yet supported
        case OPTION_USER_CLASS :
        case OPTION_VENDOR_CLASS:
        case OPTION_RECONF_MSG :
        case OPTION_RECONF_ACCEPT: {
            Log(Debug) << "Option " << opt->getOptType() << " is not supported." << LogEnd;
            break;
        }

        default: {
            handleDefaultOption(opt);
            break;
        }

        } // end of switch
    } // end of while

    // process FQDN afer all addresses are processed
    opt = clientMsg->getOption(OPTION_FQDN);
    if (opt) {
        processFQDN((Ptr*)clientMsg, (Ptr*) opt);
    }
}


/// @brief Releases all addresses and prefixes specified in this message.
///
/// @param quiet be quiet (false = verbose)
///
/// @return true, if anything was released
bool TSrvMsg::releaseAll(bool quiet) {
    bool released = false;
    SPtr<TOpt> opt;
    this->firstOption();
    while ( opt = this->getOption()) {
        switch (opt->getOptType()) {
        case OPTION_IA_NA: {
            SPtr<TSrvOptIA_NA> ptrOptIA_NA;
            ptrOptIA_NA = (Ptr*) opt;
            ptrOptIA_NA->releaseAllAddrs(quiet);
            released = true;
            break;
        }
        case OPTION_IA_TA: {
            SPtr<TSrvOptTA> ta;
            ta = (Ptr*) opt;
            ta->releaseAllAddrs(quiet);
            released = true;
            break;
        }
        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> pd;
            pd = (Ptr*) opt;
            pd->releaseAllPrefixes(quiet);
            released = true;
            break;
        }
        default: {
            continue;
        }
        }
    }
    return released;
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

SPtr<TDUID> TSrvMsg::getClientDUID() {
    SPtr<TOpt> opt;
    opt = getOption(OPTION_CLIENTID);
    if (!opt)
        return 0;

    SPtr<TSrvOptClientIdentifier> clientid = (Ptr*) opt;
    return clientid->getDUID();
}

void TSrvMsg::processIA_NA(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_NA> queryOpt) {
    SPtr<TOpt> optIA_NA;
    optIA_NA = new TSrvOptIA_NA( queryOpt, clientMsg, this);
    Options.push_back(optIA_NA);
}

void TSrvMsg::processIA_TA(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptTA> queryOpt) {
    SPtr<TOpt> optTA;
    optTA = new TSrvOptTA(queryOpt, clientMsg, SOLICIT_MSG, this);
    Options.push_back(optTA);
}

void TSrvMsg::processIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt) {
    SPtr<TOpt> optPD;
    optPD = new TSrvOptIA_PD(clientMsg, queryOpt, this);
    Options.push_back(optPD);
}

void TSrvMsg::processFQDN(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptFQDN> requestFQDN) {
    string hint = requestFQDN->getFQDN();
    SPtr<TSrvOptFQDN> optFQDN;

    SPtr<TIPv6Addr> clntAssignedAddr = SrvAddrMgr().getFirstAddr(ClientDUID);
    if (clntAssignedAddr)
        optFQDN = this->prepareFQDN(requestFQDN, ClientDUID, clntAssignedAddr, hint, false);
    else
        optFQDN = this->prepareFQDN(requestFQDN, ClientDUID, PeerAddr, hint, false);

		if (optFQDN)
			Options.push_back((Ptr*) optFQDN);
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

SPtr<TIPv6Addr> TSrvMsg::getClientPeer()
{
   if (Relays > 0)
   {
       return PeerAddrTbl[0]; //first hop ?
   }
   return PeerAddr;
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
    if (!reqOpts->count())
        return false;

    SPtr<TSrvCfgIface>  ptrIface=SrvCfgMgr().getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface (ifindex=" << iface << "). Something is wrong." << LogEnd;
        return false;
    }

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(duid, this, false/* false = verbose */);

    /// @todo: Make this an array of options and handle them in an uniform manner

    // --- option: DNS resolvers ---
    if ( reqOpts->isOption(OPTION_DNS_SERVERS) && ptrIface->supportDNSServer() ) {
	SPtr<TOpt> optDNS;
	if (ex && ex->supportDNSServer())
	    optDNS = new TOptAddrLst(OPTION_DNS_SERVERS, *ex->getDNSServerLst(), this);
	else
	    optDNS = new TOptAddrLst(OPTION_DNS_SERVERS, *ptrIface->getDNSServerLst(), this);
	Options.push_back(optDNS);
	newOptionAssigned = true;
    };

    // --- option: DOMAIN LIST ---
    if ( reqOpts->isOption(OPTION_DOMAIN_LIST) && ptrIface->supportDomain() ) {
	SPtr<TOpt> optDomain;
	if (ex && ex->supportDomain())
	    optDomain = new TOptDomainLst(OPTION_DOMAIN_LIST, *ex->getDomainLst(),this);
	else
	    optDomain = new TOptDomainLst(OPTION_DOMAIN_LIST, *ptrIface->getDomainLst(),this);
	Options.push_back((Ptr*)optDomain);
	newOptionAssigned = true;
    };

    // --- option: NTP servers ---
    if ( reqOpts->isOption(OPTION_SNTP_SERVERS) && ptrIface->supportNTPServer() ) {
	SPtr<TOpt> optNTP;
	if (ex && ex->supportNTPServer())
	    optNTP = new TOptAddrLst(OPTION_SNTP_SERVERS, *ex->getNTPServerLst(),this);
	else
	    optNTP = new TOptAddrLst(OPTION_SNTP_SERVERS, *ptrIface->getNTPServerLst(),this);
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
	SPtr<TOpt> optSIPServer;
	if (ex && ex->supportSIPServer())
	    optSIPServer = new TOptAddrLst(OPTION_SIP_SERVER_A, *ex->getSIPServerLst(),this);
	else
	    optSIPServer = new TOptAddrLst(OPTION_SIP_SERVER_A, *ptrIface->getSIPServerLst(),this);
	Options.push_back(optSIPServer);
	newOptionAssigned = true;
    };

    // --- option: SIP DOMAINS ---
    if ( reqOpts->isOption(OPTION_SIP_SERVER_D) && ptrIface->supportSIPDomain() ) {
	SPtr<TOpt> optSIPDomain;
	if (ex && ex->supportSIPDomain())
	    optSIPDomain= new TOptDomainLst(OPTION_SIP_SERVER_D, *ex->getSIPDomainLst(),this);
	else
	    optSIPDomain= new TOptDomainLst(OPTION_SIP_SERVER_D, *ptrIface->getSIPDomainLst(),this);
	Options.push_back(optSIPDomain);
	newOptionAssigned = true;
    };

    // --- option: FQDN ---
    // see prepareFQDN() method

    // --- option: NIS SERVERS ---
    if ( reqOpts->isOption(OPTION_NIS_SERVERS) && ptrIface->supportNISServer() ) {
	SPtr<TOpt> opt;
	if (ex && ex->supportNISServer())
	    opt = new TOptAddrLst(OPTION_NIS_SERVERS, *ex->getNISServerLst(),this);
	else
	    opt = new TOptAddrLst(OPTION_NIS_SERVERS, *ptrIface->getNISServerLst(),this);
	Options.push_back(opt);
	newOptionAssigned = true;
    };

    // --- option: NIS DOMAIN ---
    if ( reqOpts->isOption(OPTION_NIS_DOMAIN_NAME) && ptrIface->supportNISDomain() ) {
	SPtr<TOpt> opt;
	if (ex && ex->supportNISDomain())
	    opt = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, ex->getNISDomain(),this);
	else
	    opt = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, ptrIface->getNISDomain(),this);
	Options.push_back(opt);
	newOptionAssigned = true;
    };

    // --- option: NISP SERVERS ---
    if ( reqOpts->isOption(OPTION_NISP_SERVERS) && ptrIface->supportNISPServer() ) {
	SPtr<TOpt> opt;
	if (ex && ex->supportNISPServer())
	    opt = new TOptAddrLst(OPTION_NISP_SERVERS, *ex->getNISPServerLst(), this);
	else
	    opt = new TOptAddrLst(OPTION_NISP_SERVERS, *ptrIface->getNISPServerLst(), this);
	Options.push_back((Ptr*) opt);
	newOptionAssigned = true;
    };

    // --- option: NISP DOMAIN ---
    if ( reqOpts->isOption(OPTION_NISP_DOMAIN_NAME) && ptrIface->supportNISPDomain() ) {
	SPtr<TOpt> opt;
	if (ex && ex->supportNISPDomain())
	    opt = new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, ex->getNISPDomain(), this);
	else
	    opt = new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, ptrIface->getNISPDomain(), this);
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

        SPtr<TIPv6Addr> DNSAddr = SrvCfgMgr().getDDNSAddress(Iface);
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

#if 0
        // that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
        // and that would include both poslib and Dibbler headers in one place. Universe would implode then.
        TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
        DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
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

#endif
	// that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
	// and that would include both poslib and Dibbler headers in one place. Universe would implode then.
	TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
	DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
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
	    DNSUpdate *act2 = new DNSUpdate(DNSAddr->getPlain(), "", fqdnName, 
                                            IPv6Addr->getPlain(),
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


    // that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
    // and that would include both poslib and Dibbler headers in one place. Universe would implode then.
    TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    if (FQDNMode == DNSUPDATE_MODE_PTR){
        /* PTR cleanup */
        Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr
                    << " and FQDN=" << fqdn->getName() << LogEnd;
        DNSUpdate *act = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(),
                                       DNSUPDATE_PTR_CLEANUP, proto2);
        result = act->run(timeout);
        act->showResult(result);
        delete act;

    } // fqdn mode 1 (PTR only)
    else if (FQDNMode == DNSUPDATE_MODE_BOTH){
        /* AAAA Cleanup */
        Log(Notice) << "FQDN: Attempting to clean up AAAA and PTR record in DNS Server " << * dns << ", IP = "
                    << *clntAddr << " and FQDN=" << fqdn->getName() << LogEnd;

        DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdnName, clntAddr->getPlain(),
                                       DNSUPDATE_AAAA_CLEANUP, proto2);
        result = act->run(timeout);
        act->showResult(result);
        delete act;

        /* PTR cleanup */
        Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr
                    << " and FQDN=" << fqdn->getName() << LogEnd;
        DNSUpdate *act2 = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(),
                                        DNSUPDATE_PTR_CLEANUP, proto2);
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

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(duid, this, true);
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
