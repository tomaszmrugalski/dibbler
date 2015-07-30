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
#include "OptUserClass.h"
#include "OptVendorClass.h"
#include "OptAuthentication.h"

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
    :TMsg(iface, addr, msgType, transID), FirstTimeStamp_((uint32_t)time(NULL)),
     MRT_(0), forceMsgType_(0), physicalIface_(iface)
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
    :TMsg(iface, addr, buf, bufSize), forceMsgType_(0), physicalIface_(iface)
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
        ptr.reset();
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
            ptr = new TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN,
				  buf+pos, length, this);
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
            ptr = new TOptInteger(OPTION_INFORMATION_REFRESH_TIME,
				  OPTION_INFORMATION_REFRESH_TIME_LEN,
				  buf+pos, length, this);
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

        case OPTION_AUTH:
            ptr = new TOptAuthentication(buf+pos, length, this);
#ifndef MOD_DISABLE_AUTH
            if (SrvCfgMgr().getDigest() != DIGEST_NONE) {

                SPtr<TOptDUID> optDUID = (SPtr<TOptDUID>)this->getOption(OPTION_CLIENTID);
                if (optDUID) {
                    SPtr<TAddrClient> client = SrvAddrMgr().getClient(optDUID->getDUID());
                    if (client)
                        client->setSPI(SPI_);
                }
            }
#endif
            break;

        case OPTION_VENDOR_OPTS:
            ptr = new TOptVendorSpecInfo(code, buf+pos, length, this);
            break;
	case OPTION_RECONF_ACCEPT:
	    ptr = new TOptEmpty(code, buf+pos, length, this);
	    break;
        case OPTION_USER_CLASS:
	    ptr = new TOptUserClass(code, buf+pos, length, this);
	    break;
        case OPTION_VENDOR_CLASS:
	    ptr = new TOptVendorClass(code, buf+pos, length, this);
	    break;
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
    DigestType_ = SrvCfgMgr().getDigest();
    // AuthKeys = SrvCfgMgr().AuthKeys;
#endif

    FirstTimeStamp_ = (uint32_t)time(NULL);
    MRT_ = 0;
}

/// Processes options in incoming message and assigns leases and options, if possible.
/// This method may be used in any message that causes server to assign anything, i.e.
/// SOLICIT, REQUEST, RENEW, REBIND (and possibly CONFIRM)
///
/// @param clientMsg client message
/// @param quiet (false will make this method verbose)
void TSrvMsg::processOptions(SPtr<TSrvMsg> clientMsg, bool quiet) {

    SPtr<TOpt> opt;
    SPtr<TIPv6Addr> clntAddr = PeerAddr_;

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

        case OPTION_PREFERENCE:
        case OPTION_UNICAST:
        case OPTION_RELAY_MSG:
        case OPTION_INTERFACE_ID:
        case OPTION_RECONF_MSG :
        case OPTION_STATUS_CODE : {
            Log(Warning) << "Invalid option (" << opt->getOptType() << ") received. "
                         << "Client is not supposed to send it. Option ignored." << LogEnd;
            break;
        }

        case OPTION_RECONF_ACCEPT: {
            /// @todo: remember that client supports reconfigure
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
    uint32_t now = (uint32_t)time(NULL);
    if (FirstTimeStamp_ + MRT_ - now > 0 )
        return FirstTimeStamp_ + MRT_ - now;
    else
        return 0;
}

void TSrvMsg::addRelayInfo(SPtr<TIPv6Addr> linkAddr,
                           SPtr<TIPv6Addr> peerAddr,
                           int hop,
                           const TOptList&  echolist) {
    RelayInfo info;
    info.LinkAddr_ = linkAddr;
    info.PeerAddr_ = peerAddr;
    info.Hop_      = hop;
    info.EchoList_ = echolist;
    info.Len_      = 0; /// @todo: what about this?
    RelayInfo_.push_back(info);
}

void TSrvMsg::send(int dstPort /* = 0 */)
{
    char* buf = new char[getSize() < 2048? 2048:getSize()];
    int offset = 0;
    int port;

    SPtr<TOptGeneric> gen;
    SPtr<TIfaceIface> ptrIface;
    SPtr<TIfaceIface> under;
    ptrIface = (Ptr*) SrvIfaceMgr().getIfaceByID(this->Iface);
    if (!ptrIface) {
        SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(this->Iface);
        if (!cfgIface) {
            Log(Error) << "Can't send message: interface with ifindex=" << this->Iface
                       << " not found." << LogEnd;
            delete [] buf;
            return;
        }
        if (cfgIface->getRelayID()==-1) {
            Log(Error) << "Can't send message: interface " << cfgIface->getFullName()
                       << " is invalid relay." << LogEnd;
            delete [] buf;
            return;
        }
        ptrIface = (Ptr*) SrvIfaceMgr().getIfaceByID(cfgIface->getRelayID());
        if (!ptrIface) {
            Log(Error) << "Can't send message: interface " << cfgIface->getFullName()
                       << " has invalid physical interface defined (ifindex="
                       << cfgIface->getRelayID() << "." << LogEnd;
            delete [] buf;
            return;
        }
    }
    Log(Notice) << "Sending " << this->getName() << " on " << ptrIface->getFullName()
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
            delete [] buf;
            return;
        }

        size_t len = getSize();
        RelayInfo_.back().Len_ = len;

        for (int i = RelayInfo_.size() - 1; i > 0; i--) {

            SPtr<TOpt> interface_id = TOpt::getOption(RelayInfo_[i].EchoList_, OPTION_INTERFACE_ID);
            // 38 = 34 bytes (relay header) + 4 bytes (relay-msg option header)
            RelayInfo_[i-1].Len_ = RelayInfo_[i].Len_ + 38;
            if (interface_id && (SrvCfgMgr().getInterfaceIDOrder()!=SRV_IFACE_ID_ORDER_NONE)) {
                RelayInfo_[i-1].Len_ += interface_id->getSize();

                for (TOptList::iterator it = RelayInfo_[i].EchoList_.begin();
                     it != RelayInfo_[i].EchoList_.end(); ++it) {
                    RelayInfo_[i-1].Len_ += (*it)->getSize();
                }
                //RelayInfo_[i].EchoList_.first();
                //while (gen = RelayInfo_[i].EchoList_.get()) {
                //    RelayInfo_[i-1].Len_ += gen->getSize();
                //}

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

        Log(Debug) << "Sending " << this->getSize() << "(packet)+" << offset
                   << "(relay headers) data on the "
                   << ptrIface->getFullName() << " interface." << LogEnd;
    } else {
        offset += this->storeSelf(buf+offset);
    }

    if (dstPort) {
        port = dstPort;
    }

    SrvIfaceMgr().send(ptrIface->getID(), buf, offset, PeerAddr_, port);
    delete [] buf;
}

SPtr<TDUID> TSrvMsg::getClientDUID() {
    SPtr<TOpt> opt;
    opt = getOption(OPTION_CLIENTID);
    if (!opt)
        return SPtr<TDUID>(); // NULL

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
    optTA = new TSrvOptTA(queryOpt, clientMsg, clientMsg->getType(), this);
    Options.push_back(optTA);
}

void TSrvMsg::processIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt) {
    SPtr<TOpt> optPD;
    optPD = new TSrvOptIA_PD(clientMsg, queryOpt, this);
    Options.push_back(optPD);
}

#ifndef MOD_DISABLE_AUTH
void TSrvMsg::appendReconfigureKey() {

    // Let's see if we have a key for this particular client
    SPtr<TAddrClient> client = SrvAddrMgr().getClient(ClientDUID);
    if (!client) {
        return; // something is wrong (or this is solicit)
    }

    // are we using something better than reconfigure key already?
    if (getOption(OPTION_AUTH)) {
        // yes, there's auth option included already. Let's not use RECONFIGURE_KEY then
        return;
    }

    SPtr<TOptAuthentication> auth = new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY, 1,
                                                           AUTH_REPLAY_NONE, this);
    switch (MsgType) {
    case REPLY_MSG: {
        // If there is no reconfigure key nonce generated
        client->generateReconfKey();
        // insert reconfigure nonce
        vector<uint8_t> key(17, 0);
        key[0] = 1; // reconfigure key (see RFC3315, 21.5.1)
        memcpy(&key[1], &client->ReconfKey_[0], 16);
        auth->setPayload(key);
        break;
    }
    case RECONFIGURE_MSG: {
        // insert hash-key
        vector<uint8_t> key(0, 17);
        key[0] = 2; // HMAC-MD5 (see RFC3315, 21.5.1)
        auth->setPayload(key);
        break;
    }
    default: {
        // don't include reconfigure-key in any other message type
        return;
    }

    }

    Options.push_back((Ptr*)auth);
}
#endif

void TSrvMsg::processFQDN(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptFQDN> requestFQDN) {
    string hint = requestFQDN->getFQDN();
    SPtr<TSrvOptFQDN> optFQDN;

    bool doRealUpdate = false;
    if (clientMsg->getType() == REQUEST_MSG ||
        clientMsg->getType() == RELEASE_MSG) {
        doRealUpdate = true;
    }

    SPtr<TIPv6Addr> clntAssignedAddr = SrvAddrMgr().getFirstAddr(ClientDUID);
    if (!clntAssignedAddr) {
        clntAssignedAddr = PeerAddr_; // it's better than nothing. Put it in FQDN option,
        // doRealUpdate = false; // but do not do the actual update
    }

    optFQDN = addFQDN(Iface, requestFQDN, ClientDUID, clntAssignedAddr, hint, doRealUpdate);

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
        return SPtr<TSrvOptFQDN>(); // NULL
    }
    // FQDN is chosen, "" if no name for this host is found.
    if (!cfgIface->supportFQDN()) {
        Log(Error) << "FQDN is not defined on " << cfgIface->getFullName() << " interface."
                   << LogEnd;
        return SPtr<TSrvOptFQDN>(); // NULL
    }

    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr="
               << clntAddr->getPlain() << LogEnd;

    SPtr<TFQDN> fqdn = cfgIface->getFQDNName(clntDuid,clntAddr, hint);
    if (!fqdn) {
        Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
        return SPtr<TSrvOptFQDN>(); // NULL
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
        //Log(Debug) << "FQDN: Server configuration allows DNS updates for " << clntDuid->getPlain()
        //           << LogEnd;

        if (FQDNMode == 1)
            optFQDN->setSFlag(false);
        else
            optFQDN->setSFlag(true); // letting client update his AAAA
        // Setting the O Flag correctly according to the difference between O flags
        optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);

        SPtr<TIPv6Addr> DNSAddr = SrvCfgMgr().getDDNSAddress(iface);
        if (!DNSAddr) {
            Log(Error) << "DDNS: DNS Update aborted. DNS server address is not specified." << LogEnd;
            return SPtr<TSrvOptFQDN>(); // NULL
        }

        SPtr<TAddrClient> ptrAddrClient = SrvAddrMgr().getClient(clntDuid);
        if (!ptrAddrClient) {
            Log(Warning) << "Unable to find client." << LogEnd;
            return SPtr<TSrvOptFQDN>(); // NULL
        }

        ptrAddrClient->firstIA();
        SPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
        if (!ptrAddrIA) {
            Log(Warning) << "Client does not have any IA(s) assigned." << LogEnd;
            return SPtr<TSrvOptFQDN>(); // NULL
        }
        ptrAddrIA->firstAddr();
        SPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
        if (!addr) {
            Log(Warning) << "Client does not have any address(es) assigned." << LogEnd;
            return SPtr<TSrvOptFQDN>(); // NULL
        }

        // regardless of the result, store the info
        ptrAddrIA->setFQDN(fqdn);
        ptrAddrIA->setFQDNDnsServer(DNSAddr);

        if (doRealUpdate) {
            Log(Notice) << "FQDN: About to perform DNS Update: IP="
                        << addr->get()->getPlain() << " and FQDN=" << fqdnName << LogEnd;
            SrvIfaceMgr().addFQDN(cfgIface->getID(), DNSAddr, addr->get(), fqdnName);
        } else {
            Log(Debug) << "FQDN: Skipping DNS Update." << LogEnd;
        }
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
   return PeerAddr_;
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
    buf[offset++] = forceMsgType_?forceMsgType_:RELAY_REPL_MSG;
    buf[offset++] = RelayInfo_[relayDepth].Hop_;
    RelayInfo_[relayDepth].LinkAddr_->storeSelf(buf+offset);
    RelayInfo_[relayDepth].PeerAddr_->storeSelf(buf+offset+16);
    offset += 32;

    SPtr<TOpt> interfaceid = TOpt::getOption(RelayInfo_[relayDepth].EchoList_, OPTION_INTERFACE_ID);

    if (order == SRV_IFACE_ID_ORDER_BEFORE)
    {
        if (interfaceid) {
            interfaceid->storeSelf(buf+offset);
            offset += interfaceid->getSize();
        }
    }

    writeUint16((buf+offset), OPTION_RELAY_MSG);
    offset += sizeof(uint16_t);
    writeUint16((buf+offset), RelayInfo_[relayDepth].Len_);
    offset += sizeof(uint16_t);

    offset += storeSelfRelay(buf+offset, relayDepth+1, order);

    if (order == SRV_IFACE_ID_ORDER_AFTER)
    {
        if (interfaceid) {
            interfaceid->storeSelf(buf+offset);
            offset += interfaceid->getSize();
        }
    }

    SPtr<TOpt> echo;
    for (TOptList::const_iterator it = RelayInfo_[relayDepth].EchoList_.begin();
         it != RelayInfo_[relayDepth].EchoList_.end(); ++it) {
        if ((*it)->getOptType() == OPTION_ERO) {
            echo = *it;
        }
    }

    if (echo) {
        SPtr<TOptOptionRequest> ero = (Ptr*) echo;

        for (TOptList::const_iterator it = RelayInfo_[relayDepth].EchoList_.begin();
             it != RelayInfo_[relayDepth].EchoList_.end(); ++it) {
            if (ero->isOption((*it)->getOptType())) {
                    Log(Debug) << "Echoing back option " << (*it)->getOptType() << ", length "
                               << (*it)->getSize() << LogEnd;
                    (*it)->storeSelf(buf+offset);
                    offset += (*it)->getSize();
            }
        }
    }

#if 0
    SPtr<TOptGeneric> gen;
    RelayInfo_[relayDepth].EchoList_.first();
    while (gen = RelayInfo_[relayDepth].EchoList_.get()) {
        Log(Debug) << "Echoing back option " << gen->getOptType() << ", length "
                   << gen->getSize() << LogEnd;
        gen->storeSelf(buf+offset);
        offset += gen->getSize();
    }
#endif

    return offset;
}


void TSrvMsg::copyAAASPI(SPtr<TSrvMsg> q) {
#ifndef MOD_DISABLE_AUTH
    //this->AAASPI = q->getAAASPI();
    SPI_ = q->SPI_;
    AuthKey_ = q->AuthKey_;
    DigestType_ = q->DigestType_;
    
#endif
}

/**
 * this function appends authentication option
 *
 */
void TSrvMsg::appendAuthenticationOption(SPtr<TDUID> duid)
{

    uint8_t algo = 0;

#ifndef MOD_DISABLE_AUTH
    if (!duid) {
        Log(Error) << "Auth: No duid! Probably internal error. Authentication option not appended." << LogEnd;
        return;
    }

    switch (SrvCfgMgr().getAuthProtocol()) {
    case AUTH_PROTO_NONE:
        return;
    case AUTH_PROTO_DELAYED: {
        uint32_t key_id = SrvCfgMgr().getDelayedAuthKeyID(SRV_KEYMAP_FILE, duid);
        if (!key_id) {
            Log(Info) << "AUTH: no key-id specified for client with DUID " << duid->getPlain() << LogEnd;
            return;
        }
        setSPI(key_id);
        if (!loadAuthKey()) {
            Log(Error) << "AUTH: Failed to load key with key-id: " << hex << key_id << LogEnd;
            return;
        }

        algo = 1; // HMAC-MD5

        break;
    }
    case AUTH_PROTO_RECONFIGURE_KEY:
        // Server is supposed to include reconfigure-key in RECONFIGURE message only
        if (MsgType != RECONFIGURE_MSG)
            return;
    case AUTH_PROTO_DIBBLER:

        /// @todo: server now forces its default algorithm. It should be possible
        /// for the server to keep using whatever the client chose.
        DigestType_ = SrvCfgMgr().getDigest();
        if (DigestType_ == DIGEST_NONE) {
            return;
        }
        algo = DigestType_;

        Log(Debug) << "Auth: Dibbler protocol, setting digest type to "
                   << getDigestName(DigestType_) << LogEnd;

        SPtr<TAddrClient> client = SrvAddrMgr().getClient(duid);
        if (client && !client->getSPI() && getSPI())
            client->setSPI(getSPI());

        if (getSPI() == 0) {
            Log(Info) << "Auth: no key selected (SPI=0) for this message, will not include AUTH option."
                      << LogEnd;
            return;
        }
    }

    if (!getOption(OPTION_AUTH)) {
        SPtr<TOptAuthentication> auth = new TOptAuthentication(SrvCfgMgr().getAuthProtocol(),
                                                               algo,
                                                               SrvCfgMgr().getAuthReplay(),
                                                               this);
        auth->setReplayDetection(SrvAddrMgr().getNextReplayDetectionValue());

        auth->setRealm(SrvCfgMgr().getAuthRealm()); // defined for delayed-auth only

        Options.push_back((Ptr*)auth);
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
    // see processFQDN() method

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

#ifdef AUTH_CRAP
    // --- option: KEYGEN ---
#ifndef MOD_DISABLE_AUTH
    if ( reqOpts->isOption(OPTION_KEYGEN) && SrvCfgMgr().getDigest() != DIGEST_NONE )
    { // && this->MsgType == ADVERTISE_MSG ) {
        SPtr<TSrvOptKeyGeneration> optKeyGeneration = new TSrvOptKeyGeneration(this);
        Options.push_back( (Ptr*)optKeyGeneration);
    }
#endif
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

#ifndef MOD_DISABLE_AUTH
/// verifies if the received packet is a replayed message
///
/// @return true if message is ok (false if it is replayed and should be dropped)
bool TSrvMsg::validateReplayDetection() {

    if (SrvCfgMgr().getAuthReplay() == AUTH_REPLAY_NONE) {
        // we don't care about replay detection
        return true;
    }

    // get the client's information
    SPtr<TAddrClient> client;
    SPtr<TOptDUID> optDUID = (Ptr*)getOption(OPTION_CLIENTID);
    if (optDUID) {
        client = SrvAddrMgr().getClient(optDUID->getDUID());
    }

    // This is either anonymous inf-request,
    // Or this is the first transmission from the client
    if (!client) {
        return true;
    }

    SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
    if (!auth) {
        // there's no auth option. We can't protect against replays
        return true;
    }

    uint64_t received = auth->getReplayDetection();
    uint64_t last_received = client->getReplayDetectionRcvd();

    if (last_received < received) {
        Log(Debug) << "Auth: Replay detection field should be greater than "
                   << last_received << " and it actually is ("
                   << received << ")" << LogEnd;
        client->setReplayDetectionRcvd(received);
        return true;
    } else {
        Log(Warning) << "Auth: Replayed message detected: previously received: "
                     << last_received << ", now received " << received << LogEnd;
        return false;
    }

    return true; // not really needed
}
#endif

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

    // RECONF_ACCEPT is the last standard option defined in RFC3315
    // All other options are considered extensions
    if (opt > OPTION_RECONF_ACCEPT && !ORO->isOption(opt) && !getOption(opt)) {
        ORO->addOption(opt);
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

/// @brief sets physical interface index
///
/// This may be different than Iface_ for relayed messages
void TSrvMsg::setPhysicalIface(int iface) {
    physicalIface_ = iface;
}

/// @brief returns physical interface index
///
/// This may be different than Iface_ for relayed messages
int TSrvMsg::getPhysicalIface() const {
    return physicalIface_;
}
