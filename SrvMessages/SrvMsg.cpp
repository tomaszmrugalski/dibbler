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
 * @param iface
 * @param addr
 * @param msgType
 * @param transID
 */
TSrvMsg::TSrvMsg(int iface, SPtr<TIPv6Addr> addr, int msgType, long transID)
    :TMsg(iface, addr, msgType, transID), FirstTimeStamp_(now()), MRT_(0)
{
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
    setDefaults();

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

void TSrvMsg::setDefaults() {
#ifndef MOD_DISABLE_AUTH
    DigestType = SrvCfgMgr().getDigest();
    AuthKeys = SrvCfgMgr().AuthKeys;
#endif

    FirstTimeStamp_ = now();
    MRT_ = 0;
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
    if (FirstTimeStamp_ + MRT_ - now() > 0 )
        return FirstTimeStamp_ + MRT_ - now();
    else
        return 0;
}

void TSrvMsg::addRelayInfo(SPtr<TIPv6Addr> linkAddr,
                           SPtr<TIPv6Addr> peerAddr,
                           int hop,
                           SPtr<TSrvOptInterfaceID> interfaceID,
                           List(TOptGeneric) echolist) {
    RelayInfo info;
    info.LinkAddr_ = linkAddr;
    info.PeerAddr_ = peerAddr;
    info.InterfaceID_ = interfaceID;
    info.Hop_      = hop;
    info.EchoList_ = echolist;
    info.Len_      = 0; /// @todo: what about this?
    RelayInfo_.push_back(info);
}

int TSrvMsg::getRelayCount() {
    return RelayInfo_.size();
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
    Log(Cont) << ", " << RelayInfo_.size() << " relay(s)." << LogEnd;

    port = DHCPCLIENT_PORT;
    if (!RelayInfo_.empty()) {
        port = DHCPSERVER_PORT;
        if (RelayInfo_.size() > HOP_COUNT_LIMIT) {
            Log(Error) << "Unable to send message. Got " << RelayInfo_.size()
                       << " relay entries (" << HOP_COUNT_LIMIT
                       << " is allowed maximum." << LogEnd;
            return;
        }


        size_t len = getSize();
        RelayInfo_.back().Len_ = len;



        for (int i = RelayInfo_.size() - 1; i > 0; i--) {
            // 38 = 34 bytes (relay header) + 4 bytes (relay-msg option header)
            RelayInfo_[i-1].Len_ = RelayInfo_[i].Len_ + 38;
            if (RelayInfo_[i].InterfaceID_ && (SrvCfgMgr().getInterfaceIDOrder()!=SRV_IFACE_ID_ORDER_NONE)) {
                RelayInfo_[i-1].Len_ += RelayInfo_[i].InterfaceID_->getSize();

                RelayInfo_[i].EchoList_.first();
                while (gen = RelayInfo_[i].EchoList_.get()) {
                    RelayInfo_[i-1].Len_ += gen->getSize();
                }

            }

        }

#if 0
        // calculate lengths of all relays
        Len_[Relays_ - 1] = getSize();
        for (int i = Relays_ - 1; i > 0; i--) {
            Len_[i-1] = Len_[i] + 38; // 34 bytes (relay header) + 4 bytes (relay-msg option header)
            if (InterfaceIDTbl_[i] && (SrvCfgMgr().getInterfaceIDOrder()!=SRV_IFACE_ID_ORDER_NONE)) {
                Len_[i-1] += InterfaceIDTbl_[i]->getSize();

                EchoListTbl_[i].first();
                while (gen = EchoListTbl_[i].get()) {
                    Len_[i-1] += gen->getSize();
                }

            }
        }
#endif

        // recursive storeSelf
        offset += storeSelfRelay(buf, 0, SrvCfgMgr().getInterfaceIDOrder() );

        // check if there are underlaying interfaces
        for (unsigned int i=0; i < RelayInfo_.size(); i++) {
            under = ptrIface->getUnderlaying();
            if (!under) {
                Log(Error) << "Sending message on the " << ptrIface->getFullName()
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

    SPtr<TOptDUID> clientid = (Ptr*) opt;
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
    /// @todo: Make this method also usable for RELEASE message
    string hint = requestFQDN->getFQDN();
    SPtr<TSrvOptFQDN> optFQDN;

    SPtr<TIPv6Addr> clntAssignedAddr = SrvAddrMgr().getFirstAddr(ClientDUID);
    if (!clntAssignedAddr)
        /// @todo: Perhaps we should not do the update at all?
        clntAssignedAddr = PeerAddr; // it's better than nothing

    optFQDN = addFQDN(Iface, requestFQDN, ClientDUID, clntAssignedAddr, hint, false);
    /// @todo: Why is doRealUpdate set to false???

    if (optFQDN)
        Options.push_back((Ptr*) optFQDN);
}

/**
 * creates FQDN option and executes DNS Update procedure (if necessary)
 *
 * @param iface interface index
 * @param requestFQDN  requested Fully Qualified Domain Name
 * @param clntDuid     client DUID
 * @param clntAddr     client address
 * @param hint         hint used to get name (it may or may not be used)
 * @param doRealUpdate - should the real update be performed (for example if response for
 *                       SOLICIT is prepare, this should be set to false)
 *
 * @return FQDN option
 */
SPtr<TSrvOptFQDN> TSrvMsg::addFQDN(int iface, SPtr<TSrvOptFQDN> requestFQDN,
                                   SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
                                   std::string hint, bool doRealUpdate) {
    SPtr<TSrvOptFQDN> optFQDN;
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface);
    if (!cfgIface) {
        Log(Crit) << "Msg received through not configured interface. "
            "Somebody call an exorcist!" << LogEnd;
        this->IsDone = true;
        return 0;
    }
    // FQDN is chosen, "" if no name for this host is found.
    if (!cfgIface->supportFQDN()) {
        Log(Error) << "FQDN is not defined on " << cfgIface->getFullName() << " interface."
                   << LogEnd;
        return 0;
    }

    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr="
               << clntAddr->getPlain() << LogEnd;

    SPtr<TFQDN> fqdn = cfgIface->getFQDNName(clntDuid,clntAddr, hint);
    if (!fqdn) {
        Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
        return 0;
    }

    optFQDN = new TSrvOptFQDN(fqdn->getName(), NULL);
    optFQDN->setSFlag(false);
    // Setting the O Flag correctly according to the difference between O flags
    optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
    string fqdnName = fqdn->getName();

    if (requestFQDN->getNFlag()) {
              Log(Notice) << "FQDN: No DNS Update required." << LogEnd;
              return optFQDN;
    }

    fqdn->setUsed(true);

    int FQDNMode = cfgIface->getFQDNMode();
    Log(Debug) << "FQDN: Adding FQDN Option in REPLY message: " << fqdnName << ", FQDNMode="
               << FQDNMode << LogEnd;

    if ( FQDNMode==1 || FQDNMode==2 ) {
        Log(Debug) << "FQDN: Server configuration allow DNS updates for " << clntDuid->getPlain()
                   << LogEnd;

        if (FQDNMode == 1)
            optFQDN->setSFlag(false);
        else
            if (FQDNMode == 2)
                optFQDN->setSFlag(true); // letting client update his AAAA
        // Setting the O Flag correctly according to the difference between O flags
        optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);

        SPtr<TIPv6Addr> DNSAddr = SrvCfgMgr().getDDNSAddress(iface);
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

        Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << DNSAddr->getPlain() << ", IP="
                    << IPv6Addr->getPlain() << " and FQDN=" << fqdnName << LogEnd;

        // regardless of the result, store the info
        ptrAddrIA->setFQDN(fqdn);
        ptrAddrIA->setFQDNDnsServer(DNSAddr);

        SrvIfaceMgr().addFQDN(cfgIface->getID(), DNSAddr, addr->get(), fqdnName);
    } else {
        Log(Debug) << "Server configuration does NOT allow DNS updates for " << clntDuid->getPlain() << LogEnd;
        optFQDN->setNFlag(true);
    }

    return optFQDN;

}

void TSrvMsg::delFQDN(SPtr<TSrvCfgIface> cfgIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn) {
    string fqdnName = fqdn->getName();
    fqdn->setUsed(false);

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
    if (!clntAddr) {
        // WTF? Removing FQDN for a client without address?
        Log(Error) << "Failed to remove FQDN for client: "
                   << "no address assigned for this client in database." << LogEnd;
        return;
    }

    // do the actual update over wire
    SrvIfaceMgr().delFQDN(cfgIface->getID(), dns, clntAddr, fqdnName);
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
    if (!RelayInfo_.empty()) {
       return RelayInfo_[0].PeerAddr_; //first hop ?
   }
   return PeerAddr;
}

void TSrvMsg::copyRelayInfo(SPtr<TSrvMsg> q) {
    RelayInfo_ = q->RelayInfo_;
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
int TSrvMsg::storeSelfRelay(char * buf, uint8_t relayDepth, ESrvIfaceIdOrder order)
{
    int offset = 0;
    if (relayDepth == RelayInfo_.size()) {
        return storeSelf(buf);
    }
    buf[offset++] = RELAY_REPL_MSG;
    buf[offset++] = RelayInfo_[relayDepth].Hop_;
    RelayInfo_[relayDepth].LinkAddr_->storeSelf(buf+offset);
    RelayInfo_[relayDepth].PeerAddr_->storeSelf(buf+offset+16);
    offset += 32;

    if (order == SRV_IFACE_ID_ORDER_BEFORE)
    {
        if (RelayInfo_[relayDepth].InterfaceID_) {
            RelayInfo_[relayDepth].InterfaceID_->storeSelf(buf+offset);
            offset += RelayInfo_[relayDepth].InterfaceID_->getSize();
        }
    }

    writeUint16((buf+offset), OPTION_RELAY_MSG);
    offset += sizeof(uint16_t);
    writeUint16((buf+offset), RelayInfo_[relayDepth].Len_);
    offset += sizeof(uint16_t);

    offset += storeSelfRelay(buf+offset, relayDepth+1, order);

    if (order == SRV_IFACE_ID_ORDER_AFTER)
    {
        if (RelayInfo_[relayDepth].InterfaceID_) {
            RelayInfo_[relayDepth].InterfaceID_->storeSelf(buf+offset);
            offset += RelayInfo_[relayDepth].InterfaceID_->getSize();
        }
    }

    SPtr<TOptGeneric> gen;
    RelayInfo_[relayDepth].EchoList_.first();
    while (gen = RelayInfo_[relayDepth].EchoList_.get()) {
        Log(Debug) << "Echoing back option " << gen->getOptType() << ", length "
                   << gen->getSize() << LogEnd;
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
