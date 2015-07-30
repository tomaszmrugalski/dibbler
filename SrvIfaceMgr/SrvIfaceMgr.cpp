/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *          Petr Pisar <petr.pisar(at)atlas(dot)cz>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <sstream>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#ifndef WIN32
#include <sys/socket.h>
#include <net/if.h>
#endif
#include "Portable.h"
#include "SmartPtr.h"
#include "SrvIfaceMgr.h"
#include "Msg.h"
#include "SrvMsg.h"
#include "Logger.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgRequest.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRelease.h"
#include "SrvMsgDecline.h"
#include "SrvMsgInfRequest.h"
#include "SrvMsgLeaseQuery.h"
#include "SrvOptInterfaceID.h"
#include "IPv6Addr.h"
#include "AddrClient.h"
#include "Iface.h"
#include "OptOptionRequest.h"
#include "OptGeneric.h"
#include "OptVendorData.h"
#include "OptIAAddress.h"
#include "OptIAPrefix.h"
#include "DNSUpdate.h"

using namespace std;

TSrvIfaceMgr * TSrvIfaceMgr::Instance = 0;


/*
 * constructor.
 */
TSrvIfaceMgr::TSrvIfaceMgr(const std::string& xmlFile)
    : TIfaceMgr(xmlFile, false) {

    struct iface * ptr;
    struct iface * ifaceList;

    this->XmlFile = xmlFile;

    // get interface list
    ifaceList = if_list_get(); // external (C coded) function
    ptr = ifaceList;

    if  (!ifaceList) {
        IsDone = true;
        Log(Crit) << "Unable to read info interfaces. Make sure "
                  << "you are using proper port (i.e. win32 on WindowsXP or 2003)"
                  << " and you have IPv6 support enabled." << LogEnd;
        return;
    }

    while (ptr!=NULL) {
        Log(Notice) << "Detected iface " << ptr->name << "/" << ptr->id
                 // << ", flags=" << ptr->flags
                    << ", MAC=" << this->printMac(ptr->mac, ptr->maclen) << "." << LogEnd;

        SPtr<TIfaceIface> iface(new TIfaceIface(ptr->name,ptr->id,
                                                ptr->flags,
                                                ptr->mac,
                                                ptr->maclen,
                                                ptr->linkaddr,
                                                ptr->linkaddrcount,
                                                ptr->globaladdr,
                                                ptr->globaladdrcount,
                                                ptr->hardwareType));
        this->IfaceLst.append((Ptr*) iface);
        ptr = ptr->next;
    }
    if_list_release(ifaceList); // allocated in pure C, and so release it there

    dump();
}

TSrvIfaceMgr::~TSrvIfaceMgr() {
    Log(Debug) << "SrvIfaceMgr cleanup." << LogEnd;
}

void TSrvIfaceMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open( this->XmlFile.c_str() );
    xmlDump << *this;
    xmlDump.close();
}


/**
 * sends data to client. Uses unicast address as source
 * @param iface interface index
 * @param msg - buffer containing message ready to send
 * @param size - size of message
 * @param addr  destination IPv6 address
 * @param port  destination UDP port
 * @return true if message was send successfully
 */
bool TSrvIfaceMgr::send(int iface, char *msg, int size,
                        SPtr<TIPv6Addr> addr, int port) {
    // find this interface
    SPtr<TIfaceIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
            Log(Error)  << "Send failed: No such interface id=" << iface << LogEnd;
            return false;
    }

    // find this socket
    SPtr<TIfaceSocket> sock;
    SPtr<TIfaceSocket> backup;
    ptrIface->firstSocket();
    while (sock = ptrIface->getSocket()) {
        if (sock->multicast())
            continue; // don't send anything via multicast sockets
        if (!backup) {
            backup = sock;
        }
        if (!addr->linkLocal() && sock->getAddr()->linkLocal())
            continue; // we need socket bound to global address if dst is global addr
        if (addr->linkLocal() && !sock->getAddr()->linkLocal())
            continue; // lets' not use global address as source is dst is link-local
        break;
    }
    if (!sock && !backup) {
        Log(Error) << "Send failed: interface " << ptrIface->getFullName()
                   << " has no suitable open sockets." << LogEnd;
        return false;
    }
    if (!sock) {
        Log(Warning) << "No preferred socket found for transmission to "
                     << addr->getPlain() << " on interface " << ptrIface->getFullName()
                     << ". Using backup socket " << backup->getFD() << LogEnd;
        sock = backup;
    }

    // send it!
    if (sock->send(msg,size,addr,port) == 0) {
        return true; // all ok
    } else {
        return false;
    }
}

/// @brief tries to receive a packet
///
/// This method is virtual for the purpose of easy faking packet
/// reception in tests
///
/// @param timeout select() timeout in seconds
/// @param buf pointer to reception buffer
/// @param bufsize reference to buffer size (will be updated to received packet size
///        if reception is successful)
/// @param peer address of the pkt sender
/// @param myaddr address the packet was received on
///
/// @return socket descriptor (or negative values for errors)
///
int TSrvIfaceMgr::receive(unsigned long timeout, char* buf, int& bufsize,
                          SPtr<TIPv6Addr> peer, SPtr<TIPv6Addr> myaddr) {
    return TIfaceMgr::select(timeout, buf, bufsize, peer, myaddr);
}

// @brief reads messages from all interfaces
// it's wrapper around IfaceMgr::select(...) method
//
// @param timeout how long can we wait for packets (in seconds)
// @return message object (or NULL)
SPtr<TSrvMsg> TSrvIfaceMgr::select(unsigned long timeout) {

    // static buffer speeds things up. We use maximum size for UDP (almost 64k)
    // to be on the safe side. Otherwise someone could send us a fragmented
    // huge UDP packet and we would be in for a surprise. :)
    const int maxBufsize = 0xffff - 20 - 8;
    int bufsize=maxBufsize;
    static char buf[maxBufsize];

    SPtr<TIPv6Addr> peer(new TIPv6Addr());
    SPtr<TIPv6Addr> myaddr(new TIPv6Addr());
    int sockid;

    // read data
    sockid = receive(timeout, buf, bufsize, peer, myaddr);
    if (sockid < 0) {
        return SPtr<TSrvMsg>(); // NULL
    }

    SPtr<TSrvMsg> ptr;

    if (bufsize<4) {
        if (bufsize == 1 && buf[0] == CONTROL_MSG) {
            Log(Debug) << "Control message received." << LogEnd;
            return SPtr<TSrvMsg>(); // NULL
        }
        Log(Warning) << "Received message is too short (" << bufsize
                     << ") bytes, at least 4 are required." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }

    // check message type
    int msgtype = buf[0];

    SPtr<TIfaceIface> ptrIface;

    // get interface
    ptrIface = (Ptr*)getIfaceBySocket(sockid);

    Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getName() << "/"
               << ptrIface->getID() << " (socket=" << sockid << ", addr=" << *peer << "."
               << ")." << LogEnd;

    // create specific message object
    switch (msgtype) {
    case SOLICIT_MSG:
    case REQUEST_MSG:
    case CONFIRM_MSG:
    case RENEW_MSG:
    case REBIND_MSG:
    case RELEASE_MSG:
    case DECLINE_MSG:
    case INFORMATION_REQUEST_MSG:
    case LEASEQUERY_MSG:  {
            ptr = decodeMsg(ptrIface->getID(), peer, buf, bufsize);
            break;
    }
    case RELAY_FORW_MSG: {
        ptr = decodeRelayForw(ptrIface, peer, buf, bufsize);
        break;
    }
    case ADVERTISE_MSG:
    case REPLY_MSG:
    case RECONFIGURE_MSG:
    case RELAY_REPL_MSG:
    case LEASEQUERY_REPLY_MSG:
        Log(Warning) << "Illegal message type " << msgtype << " received."
                     << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    default:
        Log(Warning) << "Message type " << msgtype << " not supported. Ignoring."
                     << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }

    if (!ptr)
        return SPtr<TSrvMsg>(); // NULL

    ptr->setLocalAddr(myaddr);

#ifndef MOD_DISABLE_AUTH
    if (!ptr->validateReplayDetection()) {
        Log(Warning) << "Auth: message replay detection failed, message dropped"
                     << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }

    bool authOk = ptr->validateAuthInfo(buf, bufsize,
                                        SrvCfgMgr().getAuthProtocol(),
                                        SrvCfgMgr().getAuthDigests());

    if (SrvCfgMgr().getAuthDropUnauthenticated() && !ptr->getSPI()) {
        Log(Warning) << "Auth: authorization is mandatory, but incoming message"
                     << " does not include AUTH option. Message dropped." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }

    if (SrvCfgMgr().getAuthDropUnauthenticated() && !authOk) {
        Log(Warning) << "Auth: Received packet failed validation, which is mandatory."
                     << " Message dropped." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }
#endif

    /// @todo: Implement support for draft-ietf-dhc-link-layer-address-opt

    char mac[20]; // maximum mac address for Infiniband is 20 bytes
    int mac_len = sizeof(mac);

    if (get_mac_from_ipv6(ptrIface->getName(), ptrIface->getID(),
                          peer->getPlain(), mac, &mac_len) == 0) {
        /// @todo: Store MAC address in the message, so it could be logged later or added
        ///        to the database

        TDUID tmp(mac, mac_len); // packed
        Log(Debug) << "Received message from IPv6 address " << peer->getPlain()
                   << ", mac=" << tmp.getPlain()
                   << " on interface " << ptrIface->getFullName() << LogEnd;
    }

    return ptr;
}

#if 0
bool TSrvIfaceMgr::setupRelay(std::string name, int ifindex, int underIfindex,
                              SPtr<TSrvOptInterfaceID> interfaceID) {
    SPtr<TIfaceIface> under = (Ptr*)this->getIfaceByID(underIfindex);
    if (!under) {
        Log(Crit) << "Unable to setup " << name << "/" << ifindex
                  << " relay: underlaying interface with id=" << underIfindex
                  << " is not present in the system or does not support IPv6." << LogEnd;
        return false;
    }

    if (!under->flagUp()) {
        Log(Crit) << "Unable to setup " << name << "/" << ifindex
                  << " relay: underlaying interface " << under->getName() << "/" << underIfindex
                  << " is down." << LogEnd;
        return false;
    }

    SPtr<TSrvIfaceIface> relay = new TSrvIfaceIface((const char*)name.c_str(), ifindex,
                                                        IFF_UP | IFF_RUNNING | IFF_MULTICAST,   // flags
                                                        0,   // MAC
                                                        0,   // MAC length
                                                        0,0, // link address
                                                        0,0, // global addresses
                                                        0);  // hardware type
    relay->setUnderlaying(under);
    this->IfaceLst.append((Ptr*)relay);

    if (!under->appendRelay(relay, interfaceID)) {
        Log(Crit) << "Unable to setup " << name << "/" << ifindex
                  << " relay: underlaying interface " << under->getName() << "/" << underIfindex
                  << " already has " << HOP_COUNT_LIMIT << " relays defined." << LogEnd;
        return false;
    }

    Log(Notice) << "Relay " << name << "/" << ifindex << " (underlaying " << under->getName()
                << "/" << under->getID() << ") has been configured." << LogEnd;

    return true;
}
#endif

SPtr<TSrvMsg> TSrvIfaceMgr::decodeRelayForw(SPtr<TIfaceIface> physicalIface,
                                            SPtr<TIPv6Addr> peer,
                                            char * buf, int bufsize) {

    SPtr<TIPv6Addr> linkAddrTbl[HOP_COUNT_LIMIT];
    SPtr<TIPv6Addr> peerAddrTbl[HOP_COUNT_LIMIT];
    int hopTbl[HOP_COUNT_LIMIT];
    TOptList echoListTbl[HOP_COUNT_LIMIT];
    int relays=0; // number of nested RELAY_FORW messages
    SPtr<TOptVendorData> remoteID;
    SPtr<TOptOptionRequest> echo;
    SPtr<TOpt> gen;
    int ifindex = -1;

    char * relay_buf = buf;
    int relay_bufsize = bufsize;

    for (int j=0;j<HOP_COUNT_LIMIT; j++)
        echoListTbl[j].clear();

    string how_found = "";


    while (bufsize>0 && buf[0]==RELAY_FORW_MSG) {
        /* decode RELAY_FORW message */
        if (bufsize < 34) {
            Log(Warning) << "Truncated RELAY_FORW message received." << LogEnd;
            return SPtr<TSrvMsg>(); // NULL
        }

        SPtr<TSrvOptInterfaceID> ptrIfaceID;

	how_found = "";

        char type = buf[0];
        if (type!=RELAY_FORW_MSG)
            return SPtr<TSrvMsg>(); // NULL
        int hopCount = buf[1];
        int optRelayCnt = 0;
        int optIfaceIDCnt = 0;

        SPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
        SPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);
        buf+=34;
        bufsize-=34;

        // options: only INTERFACE-ID and RELAY_MSG are allowed
        while (bufsize>=4) {
            unsigned short code = readUint16(buf);
            buf += sizeof(uint16_t);
            bufsize -= sizeof(uint16_t);
            int len = readUint16(buf);
            buf += sizeof(uint16_t);
            bufsize -= sizeof(uint16_t);

            gen.reset();

            if (len > bufsize) {
                Log(Warning) << "Truncated option " << code << ": " << bufsize
                             << " bytes remaining, but length is " << len
                             << "." << LogEnd;
                return SPtr<TSrvMsg>(); // NULL
            }

            switch (code) {
            case OPTION_INTERFACE_ID:
                if (bufsize < 1) {
                    Log(Warning) << "Truncated INTERFACE_ID option (length: " << bufsize
                                 << ") in RELAY_FORW message. Message dropped." << LogEnd;
                    return SPtr<TSrvMsg>(); // NULL
                }
                ptrIfaceID = new TSrvOptInterfaceID(buf, len, 0);
                gen = (Ptr*)ptrIfaceID;
                optIfaceIDCnt++;
                break;
            case OPTION_RELAY_MSG:
                relay_buf = buf;
                relay_bufsize = len;
                optRelayCnt++;
                break;
            case OPTION_REMOTE_ID:
                remoteID = new TOptVendorData(OPTION_REMOTE_ID, buf, len, 0);
                gen = (Ptr*) remoteID;
                break;
            case OPTION_ERO:
                Log(Debug) << "Echo Request received in RELAY_FORW." << LogEnd;
                gen = new TOptOptionRequest(OPTION_ERO, buf, len, 0);
                break;
            default:
                gen = new TOptGeneric(code, buf, len, 0);

            }
            if (gen) {
                echoListTbl[relays].push_back(gen);
            }
            buf     += len;
            bufsize -= len;
        }

#if 0
        // remember options to be echoed
        echoListTbl[relays].first();
        while (gen = echoListTbl[relays].get()) {
            if (!echo) {
                Log(Warning) << "Invalid option (" << gen->getOptType()
                             << ") in RELAY_FORW message was ignored." << LogEnd;
                echoListTbl[relays].del();
            } else {
                if (!echo->isOption(gen->getOptType())) {
                    Log(Warning) << "Invalid option (" << gen->getOptType()
                                 << ") in RELAY_FORW message was ignored." << LogEnd;
                    echoListTbl[relays].del();
                } else {
                    Log(Info) << "Option " << gen->getOptType() << " will be echoed back." << LogEnd;
                }
            }

        }
#endif

        // remember those links
        linkAddrTbl[relays] = linkAddr;
        peerAddrTbl[relays] = peerAddr;
        hopTbl[relays] = hopCount;
        relays++;

        if (relays> HOP_COUNT_LIMIT) {
            Log(Error) << "Message is nested more than allowed " << HOP_COUNT_LIMIT
                       << " times. Message dropped." << LogEnd;
            return SPtr<TSrvMsg>(); // NULL
        }

        if (optRelayCnt!=1) {
            Log(Error) << optRelayCnt << " RELAY_MSG options received, but exactly one was "
                       << "expected. Message dropped." << LogEnd;
            return SPtr<TSrvMsg>(); // NULL
        }
        if (optIfaceIDCnt>1) {
            Log(Error) << "More than one (" << optIfaceIDCnt
                       << ") interface-ID options received, but exactly 1 was expected. "
                       << "Message dropped." << LogEnd;
            return SPtr<TSrvMsg>(); // NULL
        }

        Log(Info) << "RELAY_FORW was decapsulated: link=" << linkAddr->getPlain()
                  << ", peer=" << peerAddr->getPlain();

	// --- selectSubnet() starts here ---

        bool guessMode = SrvCfgMgr().guessMode();

        // First try to find a relay based on the interface-id option
        if (ptrIfaceID) {
            Log(Cont) << ", interfaceID len=" << ptrIfaceID->getSize() << LogEnd;
            ifindex = SrvCfgMgr().getRelayByInterfaceID(ptrIfaceID);
            if (ifindex == -1) {
		Log(Debug) << "Unable to find relay interface with interfaceID="
			   << ptrIfaceID->getPlain() << " defined on the "
			   << physicalIface->getFullName() << " interface." << LogEnd;
            } else {
		how_found = "using interface-id=" + ptrIfaceID->getPlain();
	    }
        } else {
            Log(Cont) << ", no interface-id option." << LogEnd;
	}

        // then try to find a relay based on the link address
        if (ifindex == -1) {
            ifindex = SrvCfgMgr().getRelayByLinkAddr(linkAddr);
	    if (ifindex == -1) {
                Log(Info) << "Unable to find relay interface using link address: "
			  << linkAddr->getPlain() << LogEnd;
	    } else {
		how_found = string("using link-addr=") + linkAddr->getPlain();
            }
        }

        // the last hope - use guess-mode to get any relay
        if ((ifindex == -1) && guessMode) {
            ifindex = SrvCfgMgr().getAnyRelay();
            if (ifindex != -1) {
		how_found = "using guess-mode";
            }
        }

	// --- selectSubnet() ends here ---

        // now switch to relay interface
        buf = relay_buf;
        bufsize = relay_bufsize;
    }

    if (ifindex == -1) {
	Log(Warning) << "Unable to find appropriate interface for this RELAY-FORW." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    } else {
	SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(ifindex);
	Log(Notice) << "Found relay " << cfgIface->getFullName()
		    << " by " << how_found << LogEnd;
    }

    SPtr<TSrvMsg> msg = decodeMsg(ifindex, peer, relay_buf, relay_bufsize);
    if (!msg) {
        return SPtr<TSrvMsg>(); // NULL
    }
    for (int i=0; i<relays; i++) {
        msg->addRelayInfo(linkAddrTbl[i], peerAddrTbl[i], hopTbl[i], echoListTbl[i]);
    }
    msg->setPhysicalIface(physicalIface->getID());

    if (remoteID) {
        Log(Debug) << "RemoteID received: vendor=" << remoteID->getVendor()
                   << ", length=" << remoteID->getVendorDataLen() << "." << LogEnd;
        msg->setRemoteID(remoteID);

        /// @todo: WTF is that? ----v
        remoteID.reset();
        remoteID = msg->getRemoteID();
        
        PrintHex("RemoteID:", (uint8_t*)remoteID->getVendorData(), remoteID->getVendorDataLen());
    }

    return (Ptr*)msg;
 }

SPtr<TSrvMsg> TSrvIfaceMgr::decodeMsg(int ifaceid,
                                      SPtr<TIPv6Addr> peer,
                                      char * buf, int bufsize) {
    if (bufsize < 4) {// 4 is the minimum DHCPv6 packet size (type + 3 bytes for transaction-id)
        Log(Warning) << "Truncated message received (len " << bufsize
                     << ", at least 4 is required)." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }
    switch (buf[0]) {
    case SOLICIT_MSG:
        return new TSrvMsgSolicit(ifaceid, peer, buf, bufsize);
    case REQUEST_MSG:
        return new TSrvMsgRequest(ifaceid, peer, buf, bufsize);
    case CONFIRM_MSG:
        return new TSrvMsgConfirm(ifaceid,  peer, buf, bufsize);
    case RENEW_MSG:
        return new TSrvMsgRenew  (ifaceid,  peer, buf, bufsize);
    case REBIND_MSG:
        return new TSrvMsgRebind (ifaceid, peer, buf, bufsize);
    case RELEASE_MSG:
        return new TSrvMsgRelease(ifaceid, peer, buf, bufsize);
    case DECLINE_MSG:
        return new TSrvMsgDecline(ifaceid, peer, buf, bufsize);
    case INFORMATION_REQUEST_MSG:
        return new TSrvMsgInfRequest(ifaceid, peer, buf, bufsize);
    case LEASEQUERY_MSG:
        return new TSrvMsgLeaseQuery(ifaceid, peer, buf, bufsize);
    default:
        Log(Warning) << "Illegal message type " << (int)(buf[0]) << " received." << LogEnd;
        return SPtr<TSrvMsg>(); // NULL
    }
}

/**
 * Compares current flags of interfaces with old flags. If change is detected,
 * stored flags of the interface are updated
 */
void TSrvIfaceMgr::redetectIfaces() {
    struct iface  * ptr;
    struct iface  * ifaceList;
    SPtr<TIfaceIface> iface;
    ifaceList = if_list_get(); // external (C coded) function
    ptr = ifaceList;

    if  (!ifaceList) {
        Log(Error) << "Unable to read interface info. Inactive mode failed." << LogEnd;
        return;
    }
    while (ptr!=NULL) {
        iface = getIfaceByID(ptr->id);
        if (iface && (ptr->flags!=iface->getFlags())) {
            Log(Notice) << "Flags on interface " << iface->getFullName() << " has changed (old="
                        << hex <<iface->getFlags() << ", new=" << ptr->flags << ")." << dec << LogEnd;
            iface->updateState(ptr);
        }
        ptr = ptr->next;
    }

    if_list_release(ifaceList); // allocated in pure C, and so release it there
}

void TSrvIfaceMgr::instanceCreate(const std::string& xmlDumpFile)
{
    if (Instance) {
      Log(Crit) << "SrvIfaceMgr instance already created! Application error." << LogEnd;
      return; // don't create second instance
    }
    Instance = new TSrvIfaceMgr(xmlDumpFile);
}

TSrvIfaceMgr & TSrvIfaceMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "SrvIfaceMgr not create yet. Application error. Emergency shutdown."
                  << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}

bool TSrvIfaceMgr::addFQDN(int iface, SPtr<TIPv6Addr> dnsAddr, SPtr<TIPv6Addr> addr,
                           const std::string& name) {

    bool success = true;

#ifndef MOD_SRV_DISABLE_DNSUPDATE
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface);
    if (!cfgIface) {
        Log(Error) << "Unable find cfgIface with ifindex=" << iface << ", DDNS failed."
                   << LogEnd;
        return false;
    }

    DnsUpdateModeCfg FQDNMode = static_cast<DnsUpdateModeCfg>(cfgIface->getFQDNMode());

    SPtr<TSIGKey> key = SrvCfgMgr().getKey();

    TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    // FQDNMode: 0 = NONE, 1 = PTR only, 2 = BOTH PTR and AAAA
    if ((FQDNMode == DNSUPDATE_MODE_PTR) || (FQDNMode == DNSUPDATE_MODE_BOTH)) {
        //Test for DNS update
        char zoneroot[128];
        doRevDnsZoneRoot(addr->getAddr(), zoneroot, cfgIface->getRevDNSZoneRootLength());
        /* add PTR only */
        DnsUpdateResult result = DNSUPDATE_SKIP;
        DNSUpdate *act = new DNSUpdate(dnsAddr->getPlain(), zoneroot, name, addr->getPlain(),
                                       DNSUPDATE_PTR, proto2);
	
	if (key) {
	    act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
			 key->Fudge_);
	}

        result = act->run(timeout);
        act->showResult(result);
        delete act;

        success = (result == DNSUPDATE_SUCCESS);
    }

    if (FQDNMode == DNSUPDATE_MODE_BOTH) {
        DnsUpdateResult result = DNSUPDATE_SKIP;
        DNSUpdate *act = new DNSUpdate(dnsAddr->getPlain(), "", name,
                                       addr->getPlain(),
                                       DNSUPDATE_AAAA, proto2);

	if (key) {
	    act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
			 key->Fudge_);
	}

        result = act->run(timeout);
        act->showResult(result);
        delete act;
        success = (result == DNSUPDATE_SUCCESS) && success;
    }
#else
    Log(Info) << "DNSUpdate not compiled in. Pretending success." << LogEnd;
#endif

    return success;
}

bool TSrvIfaceMgr::delFQDN(int iface, SPtr<TIPv6Addr> dnsAddr, SPtr<TIPv6Addr> addr,
                           const std::string& name) {

    bool success = true;

#ifndef MOD_SRV_DISABLE_DNSUPDATE
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface);
    if (!cfgIface) {
        Log(Error) << "Unable find cfgIface with ifindex=" << iface << ", DDNS failed."
                   << LogEnd;
        return false;
    }

    DnsUpdateModeCfg FQDNMode = static_cast<DnsUpdateModeCfg>(cfgIface->getFQDNMode());

    SPtr<TSIGKey> key = SrvCfgMgr().getKey();

    char zoneroot[128];
    doRevDnsZoneRoot(addr->getAddr(), zoneroot, cfgIface->getRevDNSZoneRootLength());

    // that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
    // and that would include both poslib and Dibbler headers in one place. Universe would
    // implode then.
    TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    // FQDNMode: 0 = NONE, 1 = PTR only, 2 = BOTH PTR and AAAA
    if ((FQDNMode == DNSUPDATE_MODE_PTR) || (FQDNMode == DNSUPDATE_MODE_BOTH)) {
        /* PTR cleanup */
        // Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server "
        //            << dnsAddr->getPlain() << ", IP = " << addr->getPlain()
        //            << " and FQDN=" << name << LogEnd;
        DNSUpdate *act = new DNSUpdate(dnsAddr->getPlain(), zoneroot, name, addr->getPlain(),
                                       DNSUPDATE_PTR_CLEANUP, proto2);

	if (key) {
	    act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
			 key->Fudge_);
	}

        int result = act->run(timeout);
        act->showResult(result);
        delete act;
        success = (result == DNSUPDATE_SUCCESS);
    }

    if (FQDNMode == DNSUPDATE_MODE_BOTH) {
        /* AAAA Cleanup */
        //Log(Notice) << "FQDN: Attempting to clean up AAAA and PTR record in DNS Server "
        //            << dnsAddr->getPlain() << ", IP = " << addr->getPlain()
        //            << " and FQDN=" << name << LogEnd;

        DNSUpdate *act = new DNSUpdate(dnsAddr->getPlain(), "", name, addr->getPlain(),
                                       DNSUPDATE_AAAA_CLEANUP, proto2);

	if (key) {
	    act->setTSIG(key->Name_, key->getPackedData(), key->getAlgorithmText(),
			 key->Fudge_);
	}

        int result = act->run(timeout);
        act->showResult(result);
        delete act;

        success = (result == DNSUPDATE_SUCCESS) && success;

    }
#else
    Log(Info) << "DNSUpdate not compiled in. Pretending success." << LogEnd;
#endif

    return success;

}

void TSrvIfaceMgr::notifyScripts(const std::string& scriptName, SPtr<TMsg> question,
                                 SPtr<TMsg> answer) {
    TNotifyScriptParams* params = (TNotifyScriptParams*)answer->getNotifyScriptParams();

    // add info about relays
    SPtr<TSrvMsg> reply = (Ptr*)answer;

    const vector<TSrvMsg::RelayInfo> relayInfo = reply->RelayInfo_;

    stringstream relaysNum;
    relaysNum << relayInfo.size();
    params->addParam("RELAYS", relaysNum.str());

    int cnt = 1;
    for (vector<TSrvMsg::RelayInfo>::const_reverse_iterator relay = relayInfo.rbegin();
         relay != relayInfo.rend(); ++relay) {
        stringstream peer;
        stringstream link;
        peer << "RELAY" << cnt << "_PEER";
        link << "RELAY" << cnt << "_LINK";

        params->addParam(peer.str(), relay->PeerAddr_->getPlain());
        params->addParam(link.str(), relay->LinkAddr_->getPlain());

        cnt++;
    }

    TIfaceMgr::notifyScripts(scriptName, question, answer);
}

ostream & operator <<(ostream & strum, TSrvIfaceMgr &x) {
    strum << "<SrvIfaceMgr>" << std::endl;
    SPtr<TIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
        strum << *ptr;
    }
    strum << "</SrvIfaceMgr>" << std::endl;
    return strum;
}
