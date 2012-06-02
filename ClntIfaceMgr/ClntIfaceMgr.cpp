/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <sstream>
#include <stdlib.h>
#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

using namespace std;

TClntIfaceMgr * TClntIfaceMgr::Instance = 0;

void TClntIfaceMgr::instanceCreate(const std::string& xmlFile)
{
    if (Instance) {
        Log(Crit) << "Application error: Attempt to create another ClntIfaceMgr instance!" << LogEnd;
        return;
    }
    Instance = new TClntIfaceMgr(xmlFile);
}

TClntIfaceMgr& TClntIfaceMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "Requested IfaceMgr, but it is not created yet." << LogEnd;
        instanceCreate(CLNTIFACEMGR_FILE);
    }
    return *Instance;
}

bool TClntIfaceMgr::sendUnicast(int iface, char *msg, int size, SPtr<TIPv6Addr> addr)
{
    int result;
    // get interface
    SPtr<TIfaceIface> Iface;
    Iface = this->getIfaceByID(iface);
    if (!Iface) {
        Log(Error) << " No such interface (id=" << iface << "). Send failed." << LogEnd;
        return false;
    }

    // are there any sockets on this interface?
    SPtr<TIfaceSocket> sock;
    if (! Iface->countSocket() ) {
        Log(Error) << "Interface " << Iface->getName() << " has no open sockets." << LogEnd;
        return false;
    }

    // yes, there are. Get first of them (there's usually only one anyway.
    // Additional socket is created for unicast communication
    Iface->firstSocket();
    sock = Iface->getSocket();

    result = sock->send( (char*)msg, size, addr, DHCPSERVER_PORT);
    if (result == -1) {
        Log(Error) << "Send failed: " << size << " bytes to " << *addr
                   << " on " << Iface->getFullName()
                   << "(socket " << sock->getFD() << ")." << LogEnd;
        return false;
    }

    return true;
}

bool TClntIfaceMgr::sendMulticast(int iface, char * msg, int msgsize)
{
    // prepare address
    char addr[16];
    inet_pton6(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,addr);
    SPtr<TIPv6Addr> multicastAddr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,true);

    return this->sendUnicast(iface, msg, msgsize, multicastAddr);
}


SPtr<TClntMsg> TClntIfaceMgr::select(unsigned int timeout)
{
    int bufsize=4096;
    static char buf[4096];
    SPtr<TIPv6Addr> peer(new TIPv6Addr());
    int sockid;

    sockid = TIfaceMgr::select(timeout, buf, bufsize, peer);

    if (sockid>0) {
        if (bufsize<4) {
            if (buf[0]!=CONTROL_MSG) {
                Log(Warning) << "Received message is too short (" << bufsize
                             << ") bytes." << LogEnd;
            } else {
                Log(Warning) << "Control message received." << LogEnd;
            }
            return 0; // NULL
        }
        int msgtype = buf[0];
        SPtr<TClntMsg> ptr;
        SPtr<TIfaceIface> ptrIface;
        ptrIface = this->getIfaceBySocket(sockid);
        int ifaceid = ptrIface->getID();
        Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getFullName()
                   << " (socket=" << sockid << ", addr=" << *peer << ")." << LogEnd;

        switch (msgtype) {
        case ADVERTISE_MSG:
            ptr = new TClntMsgAdvertise(ifaceid,peer,buf,bufsize);
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr().getAuthAcceptMethods())) {
                    Log(Error) << "Message dropped, authentication validation failed." << LogEnd;
                    return 0;
            }
#endif
            return ptr;
        case SOLICIT_MSG:
        case REQUEST_MSG:
        case CONFIRM_MSG:
        case RENEW_MSG:
        case REBIND_MSG:
        case RELEASE_MSG:
        case DECLINE_MSG:
        case INFORMATION_REQUEST_MSG:
            Log(Warning) << "Illegal message type " << msgtype << " received." << LogEnd;
            return 0; // NULL
        case REPLY_MSG:
            ptr = new TClntMsgReply(ifaceid, peer, buf, bufsize);
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr().getAuthAcceptMethods())) {
                    Log(Error) << "Message dropped, authentication validation failed." << LogEnd;
                    return 0;
            }
#endif
            return ptr;

        case RECONFIGURE_MSG:
            Log(Warning) << "Reconfigure Message is currently not supported." << LogEnd;
            return 0; // NULL
        case RELAY_FORW_MSG: // those two msgs should not be visible for client
        case RELAY_REPL_MSG:
        default:
            Log(Warning) << "Message type " << msgtype << " is not supposed to "
                         << "be received by client. Check your relay/server configuration." << LogEnd;
            return 0;
        }
    } else {
        return 0;
    }
}

TClntIfaceMgr::TClntIfaceMgr(const std::string& xmlFile)
    : TIfaceMgr(xmlFile, false)
{
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

        SPtr<TIfaceIface> iface = new TClntIfaceIface(ptr->name,ptr->id,
                                                          ptr->flags,
                                                          ptr->mac,
                                                          ptr->maclen,
                                                          ptr->linkaddr,
                                                          ptr->linkaddrcount,
                                                          ptr->globaladdr,
                                                          ptr->globaladdrcount,
                                                          ptr->hardwareType);
        this->IfaceLst.append(iface);

        ptr = ptr->next;
    }
    if_list_release(ifaceList); // allocated in pure C, and so release it there

}

TClntIfaceMgr::~TClntIfaceMgr() {
    IfaceLst.clear();
    Log(Debug) << "ClntIfaceMgr cleanup." << LogEnd;
}

void TClntIfaceMgr::removeAllOpts() {
    SPtr<TIfaceIface> iface;
    SPtr<TClntIfaceIface> clntIface;

    this->firstIface();
    while (iface = this->getIface()) {
        clntIface = (Ptr*) iface;
        clntIface->removeAllOpts();
    }
}

unsigned int TClntIfaceMgr::getTimeout() {
    unsigned int min=DHCPV6_INFINITY, tmp;
    SPtr<TIfaceIface> iface;
    SPtr<TClntIfaceIface> clntIface;

    this->firstIface();
    while (iface = this->getIface()) {
        clntIface = (Ptr*) iface;
        tmp = clntIface->getTimeout();
        if (min > tmp)
            min = tmp;
    }
    return min;
}

bool TClntIfaceMgr::doDuties() {
    SPtr<TClntIfaceIface> iface;
    SPtr<TClntCfgIface> cfgIface;

    this->firstIface();
    while (iface = (Ptr*)this->getIface()) {
              cfgIface = ClntCfgMgr().getIface(iface->getID());
              if (cfgIface) {
            // Log(Debug) << "FQDN State: " << cfgIface->getFQDNState() << " on " << iface->getFullName() << LogEnd;
            if (cfgIface->getFQDNState() == STATE_INPROCESS) {
                            // Here we check if all parameters are set, and do the DNS update if possible
                            List(TIPv6Addr) DNSSrvLst = iface->getDNSServerLst();
                            string fqdn = iface->getFQDN();
                            if (ClntAddrMgr().countIA() > 0 && DNSSrvLst.count() > 0 && fqdn.size() > 0) {

                                Log(Warning) << "Sleeping 3 seconds before performing DNS Update." << LogEnd;
                                /** @todo: sleep cannot be performed here. What if client has to perform other
                                   action during those 3 seconds? */
#ifdef WIN32
                            Sleep(3);
#else
                            sleep(3);
#endif
                            this->fqdnAdd(iface, fqdn);
                        }
                  }
              }
    }
    ClntAddrMgr().dump();
    this->dump();
    return true;
}

bool TClntIfaceMgr::fqdnAdd(SPtr<TClntIfaceIface> iface, const std::string& fqdn)
{
    SPtr<TIPv6Addr> DNSAddr;
    SPtr<TIPv6Addr> addr;

    SPtr<TClntCfgIface> cfgIface;
    cfgIface = ClntCfgMgr().getIface(iface->getID());
    if (!cfgIface) {
        Log(Error) << "Unable to find interface with ifindex=" << iface->getID() << "." << LogEnd;
        return false;
    }

    // For the moment, we just take the first DNS entry.
    List(TIPv6Addr) DNSSrvLst = iface->getDNSServerLst();
    if (!DNSSrvLst.count()) {
        Log(Error) << "Unable to find DNS Server. FQDN add failed." << LogEnd;
        return false;
    }
    DNSSrvLst.first();
    DNSAddr = DNSSrvLst.get();

    // And the first IP address
    SPtr<TAddrIA> ptrAddrIA;
    ClntAddrMgr().firstIA();
    ptrAddrIA = ClntAddrMgr().getIA();

    if (ptrAddrIA->countAddr() > 0) {
        ptrAddrIA->firstAddr();
        addr = ptrAddrIA->getAddr()->get();

        Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr
                    << ", IP=" << *addr << " and FQDN=" << fqdn << LogEnd;

        // remember DNS Address (used during address release)
        ptrAddrIA->setFQDNDnsServer(DNSAddr);

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
        TCfgMgr::DNSUpdateProtocol proto = ClntCfgMgr().getDDNSProtocol();
        DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
        if (proto == TCfgMgr::DNSUPDATE_UDP)
            proto2 = DNSUpdate::DNSUPDATE_UDP;
        if (proto == TCfgMgr::DNSUPDATE_ANY)
            proto2 = DNSUpdate::DNSUPDATE_ANY;
        unsigned int timeout = ClntCfgMgr().getDDNSTimeout();

        /* add AAAA record */
        DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), "", fqdn, addr->getPlain(),
                                       DNSUPDATE_AAAA, proto2);
        int result = act->run(timeout);
        act->showResult(result);
        delete act;
#else
        Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
        return false;
#endif
    }
    return true;
}

bool TClntIfaceMgr::fqdnDel(SPtr<TClntIfaceIface> iface, SPtr<TAddrIA> ia, const std::string& fqdn)
{
    SPtr<TIPv6Addr> dns = ia->getFQDNDnsServer();

    // let's do deleting update
    SPtr<TIPv6Addr> clntAddr;
    ia->firstAddr();
    SPtr<TAddrAddr> tmpAddr = ia->getAddr();
    if (!tmpAddr) {
        Log(Error) << "FQDN: Unable to delete FQDN: IA (IAID=" << ia->getIAID()
                   << ") does not have any addresses." << LogEnd;
        return false;
    }
    SPtr<TIPv6Addr> myAddr = tmpAddr->get();

    SPtr<TClntCfgIface> ptrIface = ClntCfgMgr().getIface(iface->getID());

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
    TCfgMgr::DNSUpdateProtocol proto = ClntCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = ClntCfgMgr().getDDNSTimeout();

    Log(Debug) << "FQDN: Cleaning up DNS AAAA record in server " << *dns << ", for IP=" << *myAddr
               << " and FQDN=" << fqdn << LogEnd;

    DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdn, myAddr->getPlain(),
                                   DNSUPDATE_AAAA_CLEANUP, proto2);
    int result = act->run(timeout);
    act->showResult(result);
    delete act;

#else
    Log(Error) << "This Dibbler version is compiled without DNS Update support." << LogEnd;
#endif

    return false;
}

void TClntIfaceMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open( this->XmlFile.c_str() );
    xmlDump << *this;
    xmlDump.close();
}

/**
 * @brief configures prefix in the operating system
 *
 * configures specified prefix in the operating system
 *
 * @param iface interface index
 * @param prefix prefix to be configured
 * @param prefixLen prefix length
 * @param pref prefered lifetime
 * @param valid valid lifetime
 *
 * @return true if operation was successful, false otherwise
 */
bool TClntIfaceMgr::addPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid)
{
    return modifyPrefix(iface, prefix, prefixLen, pref, valid, PREFIX_MODIFY_ADD);
}

bool TClntIfaceMgr::updatePrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid)
{
    return modifyPrefix(iface, prefix, prefixLen, pref, valid, PREFIX_MODIFY_UPDATE);
}

/**
 * deletes prefix from the operating system
 *
 * @param iface
 * @param prefix
 * @param prefixLen
 *
 * @return true if operation was successful, false otherwise
 */
bool TClntIfaceMgr::delPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen)
{
    return modifyPrefix(iface, prefix, prefixLen, 0, 0, PREFIX_MODIFY_DEL);
}

bool TClntIfaceMgr::modifyPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen,
                                 unsigned int pref, unsigned int valid,
                                 PrefixModifyMode mode)
{
    SPtr<TClntIfaceIface> ptrIface = (Ptr*)getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex=" << iface
                   << ", prefix add/modify operation failed." << LogEnd;
        return false;
    }

    string action;
    int conf = 0; // number of successfully configured prefixes
    int status = -1;

    switch (mode) {
    case PREFIX_MODIFY_ADD:
            action = "Adding";
            break;
    case PREFIX_MODIFY_UPDATE:
            action = "Updating";
            break;
    case PREFIX_MODIFY_DEL:
            action = "Deleting";
            break;
    }

    // option: split this prefix and add it to all interfaces
    Log(Notice) << "PD: " << action << " prefix " << prefix->getPlain() << "/" << (int)prefixLen
                << " to all interfaces (prefix will be split to /"
                << int(prefixLen+8) << " prefixes if necessary)." << LogEnd;

    if (prefixLen>120) {
        Log(Error) << "PD: Unable to perform prefix operation: prefix /" << prefixLen
                   << " can't be split. At least /120 prefix is required." << LogEnd;
        return false;
    }

    // get a list of interfaces that we will assign prefixes to
    TIfaceIfaceLst ifaceLst;
    vector<string> ifaceNames = ClntCfgMgr().getDownlinkPrefixIfaces();
    for (vector<string>::const_iterator name = ifaceNames.begin(); name != ifaceNames.end(); ++name) {
        SPtr<TIfaceIface> x = getIfaceByName(*name);
        if (x)
            ifaceLst.push_back(x);
        else
            Log(Warning) << "Interface " << *name << " specified in downlink-prefix-ifaces is missing." << LogEnd;
    }

    if (ifaceLst.size() == 0) {
        SPtr<TIfaceIface> x;
        firstIface();
        while ( x = (Ptr*)getIface() ) {
            if (x->getID() == ptrIface->getID()) {
                Log(Debug) << "PD: Interface " << x->getFullName()
                           << " is the interface, where prefix has been obtained, skipping." << LogEnd;
                continue;
            }

            // for each interface present in the system...
            if (!x->flagUp()) {
                Log(Debug) << "PD: Interface " << x->getFullName() << " is down, ignoring." << LogEnd;
            continue;
            }
            if (!x->flagRunning()) {
                Log(Debug) << "PD: Interface " << x->getFullName()
                           << " has flag RUNNING not set, ignoring." << LogEnd;
                continue;
            }
            if (!x->flagMulticast()) {
                Log(Debug) << "PD: Interface " << x->getFullName()
                           << " is not multicast capable, ignoring." << LogEnd;
                continue;
            }
            if ( !(x->getMacLen() > 5) ) {
                Log(Debug) << "PD: Interface " << x->getFullName()
                           << " has MAC address length " << x->getMacLen()
                           << " (6 or more required), ignoring." << LogEnd;
                continue;
            }
            x->firstLLAddress();
            if (!x->getLLAddress()) {
                Log(Debug) << "PD: Interface " << x->getFullName()
                           << " has no link-local address, ignoring. (Disconnected? Not associated? No-link?)" << LogEnd;
                continue;
            }

            ifaceLst.push_back(x);
        }
    }

    Log(Info) << "PD: Using " << ifaceLst.size() << " suitable interface(s):";
    TIfaceIfaceLst::const_iterator i;
    for (TIfaceIfaceLst::const_iterator i=ifaceLst.begin(); i!=ifaceLst.end(); ++i) {
        Log(Cont) << (*i)->getName() << " ";
    }
    Log(Cont) << LogEnd;

    if (ifaceLst.size() == 0) {
        Log(Warning) << "Suitable interfaces not found. Delegated prefix not split." << LogEnd;
        return true;
    }

    for (TIfaceIfaceLst::const_iterator i=ifaceLst.begin(); i!=ifaceLst.end(); ++i) {

        char buf[16];
        int subprefixLen;
        memmove(buf, prefix->getAddr(), 16);

        if (ifaceLst.size() == 1) {
            // just one interface - use delegated prefix as is
            subprefixLen = prefixLen;
        } else if (ifaceLst.size()<256) {
            subprefixLen = prefixLen + 8;
            int offset = prefixLen/8;
            if (prefixLen%8 == 0) {
                // that's easy, just put ID in the next octet
                buf[offset] = (*i)->getID();
            } else {
                // here's fun
                uint16_t existing = readUint16(buf+offset);
                uint16_t bitmask = 0xff00;
                uint16_t infixmask = ((uint8_t)(*i)->getID()) << 8;
                bitmask = bitmask >> (prefixLen%8);
                infixmask = infixmask >> (prefixLen%8);

                // clear out if there is anything there, i.e. server assigned prefix
                // with garbage in host section
                existing = existing & (~bitmask);
                existing = existing | (bitmask & infixmask);
                writeUint16(buf+offset, existing);
            }

        } else {
            // users with too much time that play with virtual interfaces are out of luck
            Log(Error) << "Something is wrong. Detected more than 256 interface." << LogEnd;
            return false;
        }

        SPtr<TIPv6Addr> tmpAddr = new TIPv6Addr(buf, false);

        Log(Notice) << "PD: " << action << " prefix " << tmpAddr->getPlain() << "/" << subprefixLen
                    << " on the " << (*i)->getFullName() << " interface." << LogEnd;

        switch (mode) {
        case PREFIX_MODIFY_ADD:
            status = prefix_add( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen, pref, valid);
            break;
        case PREFIX_MODIFY_UPDATE:
            status = prefix_update( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen, pref, valid);
            break;
        case PREFIX_MODIFY_DEL:
          status = prefix_del( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen);
            break;
        }
        if (status==LOWLEVEL_NO_ERROR) {
            conf++;
        } else {
            string tmp = error_message();
            Log(Error) << "Prefix error encountered during " << action << " operation: " << tmp << LogEnd;
        }

    }

    if (conf) // at least one prefix configured successfully
        return true;
    else
        return false;
}

void TClntIfaceMgr::redetectIfaces() {
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
            Log(Notice) << "Flags on interface " << iface->getFullName() << " has changed (old=" << hex <<iface->getFlags()
                        << ", new=" << ptr->flags << ")." << dec << LogEnd;
            iface->updateState(ptr);
        }
        ptr = ptr->next;
    }

    if_list_release(ifaceList); // allocated in pure C, and so release it there
}

#ifdef MOD_REMOTE_AUTOCONF
bool TClntIfaceMgr::notifyRemoteScripts(SPtr<TIPv6Addr> rcvdAddr, SPtr<TIPv6Addr> srvAddr, int ifindex) {

    Log(Info) << "Received address " << rcvdAddr->getPlain()
              << " from remote server located at " << srvAddr->getPlain() << LogEnd;

    SPtr<TIfaceIface> iface = getIfaceByID(ifindex);

    stringstream tmp;
    tmp << "./remote-autoconf " << rcvdAddr->getPlain() << " " << srvAddr->getPlain()
        << " " << iface->getName() << " " << iface->getID();

    int returnCode = system(tmp.str().c_str());
    Log(Info) << "Executed command: " << tmp.str() << ", return code=" << returnCode << LogEnd;

    return true;
}
#endif

ostream & operator <<(ostream & strum, TClntIfaceMgr &x) {
    strum << "<ClntIfaceMgr>" << std::endl;
    SPtr<TClntIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
        strum << *ptr;
    }
    strum << "</ClntIfaceMgr>" << std::endl;
    return strum;
}
