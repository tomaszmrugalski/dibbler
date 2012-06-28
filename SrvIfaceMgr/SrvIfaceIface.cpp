/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Logger.h"
#include "SrvIfaceIface.h"

using namespace std;

/*
  this one is used to create relay interfaces
**/
TSrvIfaceIface::TSrvIfaceIface(const char * name, int id, unsigned int flags, char* mac,
                               int maclen, char* llAddr, int llAddrCnt, char * globalAddr, int globalAddrCnt,
                               int hwType)
    :TIfaceIface(name,id, flags, mac, maclen, llAddr, llAddrCnt, globalAddr, globalAddrCnt, hwType)
{
    Relay_ = false;
    RelaysCnt_ = 0;
}

void TSrvIfaceIface::setUnderlaying(SPtr<TSrvIfaceIface> under) {
    UnderRelay_ = under;
    Relay_ = true;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getUnderlaying() {
    return UnderRelay_;
}

bool TSrvIfaceIface::appendRelay(SPtr<TSrvIfaceIface> relay, SPtr<TSrvOptInterfaceID> interfaceID) {
    if (RelaysCnt_ >= HOP_COUNT_LIMIT)
        return false;
    Relays_[RelaysCnt_].iface       = relay;
    Relays_[RelaysCnt_].ifindex     = relay->getID();
    Relays_[RelaysCnt_].interfaceID = interfaceID;
    RelaysCnt_++;
    return true;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID) {
    int i=0;
    if (!interfaceID) {
        return 0;
    }

    if (RelaysCnt_ == 0) {
        Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
        return 0;
    }
    for (i = 0; i < RelaysCnt_; i++) {
        if (Relays_[i].interfaceID && (*Relays_[i].interfaceID == *interfaceID) )
            return Relays_[i].iface;
    }
    return 0;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getRelayByLinkAddr(SPtr<TIPv6Addr> addr) {
    if (RelaysCnt_ == 0) {
        Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
        return 0;
    }

    //// @todo: Implement finding RELAYs using link address
    Log(Error) << "Finding RELAYs using link address is not implemented yet. Using first relay:"
              << Relays_[0].iface->getFullName() << LogEnd;
    return Relays_[0].iface;
}

SPtr<TSrvIfaceIface> TSrvIfaceIface::getAnyRelay() {
    if (RelaysCnt_==0) {
        Log(Warning) << "No relay interface defined on the " << this->getFullName() << LogEnd;
        return 0;
    }
    return Relays_[0].iface;
}

int TSrvIfaceIface::getRelayCnt() {
    return RelaysCnt_;
}


// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

/// @brief Prints important informations (debugging & logging).
std::ostream & operator <<(std::ostream & strum, TSrvIfaceIface &x) {
    char buf[48];
    int i;

    strum << dec;
    strum << "  <SrvIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " ifindex=\"" << x.ID << "\"";
    strum << " hwType=\"" << x.getHardwareType() << "\"";
    strum << " flags=\"" << x.Flags << "\">" << endl;

    if (x.Relay_) {
        strum << "    <UnderlayingRelay name=\"" << x.UnderRelay_->getName() << "\" ifindex=\""
              << x.UnderRelay_->getID() << "\" />" << endl;
    }

    if (x.RelaysCnt_) {
        for (i=0; i< x.RelaysCnt_; i++) {
            strum << "    <OverlayingRelay name=\"" << x.Relays_[i].iface->getName()
                  << "\" ifindex=\"" << x.Relays_[i].ifindex;
            if (x.Relays_[i].interfaceID)
              strum << "\" interfaceID=\"" << x.Relays_[i].interfaceID->getPlain();
            else
              strum << "\" interfaceID=null\"";
            strum << "\" />" << endl;
        }
    }

    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
        inet_ntop6(x.LLAddr+i*16,buf);
        strum << "    <Addr scope=\"local\">" << buf << "</Addr>" << endl;
    }

    strum << "    <!-- " << x.countGlobalAddr() << " non-local (global) addrs -->" << endl;

    x.firstGlobalAddr();
    SPtr<TIPv6Addr> addr;
    while (addr = x.getGlobalAddr()) {
        strum << "    <Addr scope=\"global\">" << *addr << "</Addr>" << endl;
    }

    strum << "    <Mac length=\"" << x.Maclen << "\">";
    for (int i=0; i<x.Maclen; i++) {
        strum.fill('0');
        strum.width(2);
        strum << (hex) << (int) x.Mac[i];
        if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << endl;

    strum << "    <!-- " << x.countSocket() << " sockets -->" << endl;
    SPtr<TIfaceSocket> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
        strum << "    " << *sock;
    }
    strum << "  </SrvIfaceIface>" << endl;
    return strum;
}
