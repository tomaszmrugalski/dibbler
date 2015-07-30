/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#define MAX_PACKET_LEN 1452
#define RELAY_FORW_MSG_LEN 36

#include <cstdlib>
#include <fstream>
#include <vector>
#include <string.h>
#include "RelTransMgr.h"
#include "RelCfgMgr.h"
#include "RelIfaceMgr.h"
#include "RelOptInterfaceID.h"
#include "RelOptEcho.h"
#include "RelOptGeneric.h"
#include "Logger.h"
#include "Portable.h"

TRelTransMgr * TRelTransMgr::Instance = 0; // singleton implementation

TRelTransMgr::TRelTransMgr(const std::string& xmlFile)
    :XmlFile(xmlFile), IsDone(false)
{
    // for each interface in CfgMgr, create socket (in IfaceMgr)
    SPtr<TRelCfgIface> confIface;
    RelCfgMgr().firstIface();
    while (confIface=RelCfgMgr().getIface()) {
        if (!this->openSocket(confIface)) {
            this->IsDone = true;
            break;
        }
    }
}

/*
 * opens proper (multicast or unicast) socket on interface
 */
bool TRelTransMgr::openSocket(SPtr<TRelCfgIface> cfgIface) {

    SPtr<TIfaceIface> iface = RelIfaceMgr().getIfaceByID(cfgIface->getID());
    if (!iface) {
        Log(Crit) << "Unable to find " << cfgIface->getName() << "/" << cfgIface->getID()
                  << " interface in the IfaceMgr." << LogEnd;
        return false;
    }

    SPtr<TIPv6Addr> srvUnicast = cfgIface->getServerUnicast();
    SPtr<TIPv6Addr> clntUnicast = cfgIface->getClientUnicast();
    SPtr<TIPv6Addr> addr;

    if (cfgIface->getServerMulticast() || srvUnicast) {

        iface->firstGlobalAddr();
        addr = iface->getGlobalAddr();
        if (!addr) {
            Log(Warning) << "No global address defined on the " << iface->getFullName() << " interface."
                         << " Trying to bind link local address, but expect troubles with relaying." << LogEnd;
            iface->firstLLAddress();
            addr = new TIPv6Addr(iface->getLLAddress());
        }
        Log(Notice) << "Creating srv unicast (" << addr->getPlain() << ") socket on the "
                    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
        if (!iface->addSocket(addr, DHCPSERVER_PORT, true, false)) {
            Log(Crit) << "Proper socket creation failed." << LogEnd;
            return false;
        }
    }

    if (cfgIface->getClientMulticast()) {
        addr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
        Log(Notice) << "Creating clnt multicast (" << addr->getPlain() << ") socket on the "
                    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
        if (!iface->addSocket(addr, DHCPSERVER_PORT, true, false)) {
            Log(Crit) << "Proper socket creation failed." << LogEnd;
            return false;
        }
    }

    if (clntUnicast) {
        addr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
        Log(Notice) << "Creating clnt unicast (" << clntUnicast->getPlain() << ") socket on the "
                    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
        if (!iface->addSocket(clntUnicast, DHCPSERVER_PORT, true, false)) {
            Log(Crit) << "Proper socket creation failed." << LogEnd;
            return false;
        }
    }

    return true;
}


/**
 * relays normal (i.e. not server replies) messages to defined servers
 */
void TRelTransMgr::relayMsg(SPtr<TRelMsg> msg)
{
    static char buf[MAX_PACKET_LEN];
    int offset = 0;
    int bufLen;
    int hopCount = 0;
    if (!msg->check()) {
        Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }

    if (msg->getDestAddr()) {
        this->relayMsgRepl(msg);
        return;
    }

    if (msg->getType() == RELAY_FORW_MSG) {
        hopCount = msg->getHopCount()+1;
    }

    // prepare message
    SPtr<TIfaceIface> iface = RelIfaceMgr().getIfaceByID(msg->getIface());
    SPtr<TIPv6Addr> addr;

    // store header
    buf[offset++] = RELAY_FORW_MSG;
    buf[offset++] = hopCount;

    // store link-addr
    iface->firstGlobalAddr();
    addr = iface->getGlobalAddr();
    if (!addr) {
        Log(Warning) << "Interface " << iface->getFullName() << " does not have global address." << LogEnd;
        addr = new TIPv6Addr("::", true);
    }
    addr->storeSelf(buf+offset);
    offset += 16;

    // store peer-addr
    addr = msg->getRemoteAddr();
    addr->storeSelf(buf+offset);
    offset += 16;

    SPtr<TRelCfgIface> cfgIface;
    cfgIface = RelCfgMgr().getIfaceByID(msg->getIface());
    TRelOptInterfaceID ifaceID(cfgIface->getInterfaceID(), 0);

    if (RelCfgMgr().getInterfaceIDOrder()==REL_IFACE_ID_ORDER_BEFORE)
    {
        // store InterfaceID option
        ifaceID.storeSelf(buf + offset);
        offset += ifaceID.getSize();
        Log(Debug) << "Interface-id option added before relayed message." << LogEnd;
    }

    // store relay msg option
    writeUint16((buf+offset), OPTION_RELAY_MSG);
    offset += sizeof(uint16_t);
    writeUint16((buf+offset), msg->getSize());
    offset += sizeof(uint16_t);
    bufLen = msg->storeSelf(buf+offset);
    offset += bufLen;

    if (RelCfgMgr().getInterfaceIDOrder()==REL_IFACE_ID_ORDER_AFTER)
    {
        // store InterfaceID option
        ifaceID.storeSelf(buf + offset);
        offset += ifaceID.getSize();
        Log(Debug) << "Interface-id option added after relayed message." << LogEnd;
    }

    if (RelCfgMgr().getInterfaceIDOrder()==REL_IFACE_ID_ORDER_NONE)
    {
        Log(Warning) << "Interface-id option not added (interface-id-order omit used in relay.conf). "
                     << "That is a debugging feature and violates RFC3315. Use with caution." << LogEnd;
    }

    SPtr<TOptVendorData> remoteID = RelCfgMgr().getRemoteID();
    if (remoteID) {
        remoteID->storeSelf(buf+offset);
        offset += remoteID->getSize();
        Log(Debug) << "Appended RemoteID with " << remoteID->getVendorDataLen()
                   << "-byte long data (option length="
                   << remoteID->getSize() << ")." << LogEnd;
    }

    SPtr<TOpt> relayID = RelCfgMgr().getRelayID();
    if (relayID) {
        relayID->storeSelf(buf + offset);
        offset += relayID->getSize();

        Log(Debug) << "Appended Relay-ID with " << relayID->getSize() << " bytes." << LogEnd;
    }

    if (RelCfgMgr().getClientLinkLayerAddress()) {
        SPtr<TOpt> lladdr = getClientLinkLayerAddr(msg);
        if (lladdr) {
            Log(Debug) << "Appended client link-layer address option with "
		       << lladdr->getSize() << " bytes." << LogEnd;
            lladdr->storeSelf(buf + offset);
            offset += lladdr->getSize();
        }
    }

    SPtr<TRelOptEcho> echo = RelCfgMgr().getEcho();
    if (echo) {
        echo->storeSelf(buf+offset);
        offset += echo->getSize();
        Log(Debug) << "Appended EchoRequest option with ";

        int i=0;
        char tmpBuf[256];
        for (i=0;i<255;i++)
            tmpBuf[i] = 255-i;

        for (int i=0; i<echo->count(); i++) {
            int code = echo->getReqOpt(i);
            SPtr<TRelOptGeneric> gen = new TRelOptGeneric(code, tmpBuf, 4, 0);
            gen->storeSelf(buf+offset);
            offset += gen->getSize();
            Log(Cont) << code << " ";
        }
        Log(Cont) << " opt(s)." << LogEnd;
    }

    RelCfgMgr().firstIface();
    while (cfgIface = RelCfgMgr().getIface()) {
        if (cfgIface->getServerUnicast()) {
            Log(Notice) << "Relaying encapsulated " << msg->getName() << " message on the "
                        << cfgIface->getFullName() << " interface to unicast ("
                        << cfgIface->getServerUnicast()->getPlain() << ") address, port "
                        << DHCPSERVER_PORT << "." << LogEnd;

            if (!RelIfaceMgr().send(cfgIface->getID(), buf, offset,
                                    cfgIface->getServerUnicast(), DHCPSERVER_PORT)) {
                Log(Error) << "Failed to send data to server unicast address." << LogEnd;
            }

        }
        if (cfgIface->getServerMulticast()) {
            addr = new TIPv6Addr(ALL_DHCP_SERVERS, true);
            Log(Notice) << "Relaying encapsulated " << msg->getName() << " message on the "
                        << cfgIface->getFullName() << " interface to multicast ("
                        << addr->getPlain() << ") address, port " << DHCPSERVER_PORT
                        << "." << LogEnd;
            if (!RelIfaceMgr().send(cfgIface->getID(), buf, offset, addr, DHCPSERVER_PORT)) {
                Log(Error) << "Failed to send data to server multicast address." << LogEnd;
            }
        }
    }

    // save DB state regardless of action taken
    RelCfgMgr().dump();
}

void TRelTransMgr::relayMsgRepl(SPtr<TRelMsg> msg) {
    int port;
    SPtr<TRelCfgIface> cfgIface = RelCfgMgr().getIfaceByInterfaceID(msg->getDestIface());
    if (!cfgIface) {
        Log(Error) << "Unable to relay message: Invalid interfaceID value:"
                   << msg->getDestIface() << LogEnd;
        return;
    }

    SPtr<TIfaceIface> iface = RelIfaceMgr().getIfaceByID(cfgIface->getID());
    SPtr<TIPv6Addr> addr = msg->getDestAddr();
    static char buf[MAX_PACKET_LEN];
    int bufLen;

    if (!iface) {
        Log(Warning) << "Unable to find interface with interfaceID=" << msg->getDestIface()
                     << LogEnd;
        return;
    }

    bufLen = msg->storeSelf(buf);
    if (msg->getType() == RELAY_REPL_MSG)
        port = DHCPSERVER_PORT;
    else
        port = DHCPCLIENT_PORT;
    Log(Notice) << "Relaying decapsulated " << msg->getName() << " message on the "
                << iface->getFullName() << " interface to the " << addr->getPlain()
                << ", port " << port << "." << LogEnd;

    if (!RelIfaceMgr().send(iface->getID(), buf, bufLen, addr, port)) {
        Log(Error) << "Failed to decapsulated data." << LogEnd;
    }

}

SPtr<TOpt> TRelTransMgr::getLinkAddrFromDuid(SPtr<TOpt> duid_opt) {
    if (!duid_opt)
        return TOptPtr(); // NULL

    // We need at least option header and duid type
    if (duid_opt->getSize() < 6)
        return TOptPtr(); // NULL

    // Create a vector and store whole DUID there.
    std::vector<uint8_t> buffer(duid_opt->getSize(), 0);

    duid_opt->storeSelf((char*)&buffer[0]);

    char* buf = (char*)&buffer[0];
    buf += sizeof(uint16_t); // skip first 2 bytes (option code)

    uint16_t len = readUint16(buf); // read option length
    buf += sizeof(uint16_t);

    // stored length must be total option size, without the header
    if (len + 4u != duid_opt->getSize())
        return TOptPtr(); // NULL

    uint16_t duid_type = readUint16(buf); // read duid type
    buf += sizeof(uint16_t);
    len -= sizeof(uint16_t);

    std::vector<uint8_t> linkaddr;

    switch (duid_type) {
    case DUID_TYPE_LLT: {
        // 7: hw type (16 bits), time (32 bits), at least 1 byte of MAC
        if (len < 7) {
            return TOptPtr(); // NULL
        }

        // Read hardware type
        uint16_t hw_type = readUint16(buf);
        buf += sizeof(uint16_t);
        len -= sizeof(uint16_t);

        // Skip duid creation time
        buf += sizeof(uint32_t);
        len -= sizeof(uint32_t);

        // Now store hardware type + MAC
        linkaddr.resize(len + sizeof(uint16_t));
        char* out = (char*)&linkaddr[0];
        out = writeUint16(out, hw_type); // hw type (2)
        memcpy(out, buf, len);

        return SPtr<TOpt>(new TOptGeneric(OPTION_CLIENT_LINKLAYER_ADDR,
                                          (const char*)&linkaddr[0], linkaddr.size(),
                                          NULL));
    }
    case DUID_TYPE_LL: {
        // 3: hw type (16 bits), at least 1 byte of MAC
        if (len < 3) {
            return TOptPtr(); // NULL
        }

        // Read hardware type
        uint16_t hw_type = readUint16(buf);
        buf += sizeof(uint16_t);
        len -= sizeof(uint16_t);

        // Now store hardware type + MAC
        linkaddr.resize(len + sizeof(uint16_t));
        char* out = (char*)&linkaddr[0];
        out = writeUint16(out, hw_type);
        memcpy(out, buf, len);

        return SPtr<TOpt>(new TOptGeneric(OPTION_CLIENT_LINKLAYER_ADDR,
                                          (const char*)&linkaddr[0], linkaddr.size(),
                                          NULL));
    }
    default:
        return TOptPtr(); // NULL
    }

}

SPtr<TOpt> TRelTransMgr::getLinkAddrFromSrcAddr(SPtr<TRelMsg> msg) {
    SPtr<TIPv6Addr> srcAddr = msg->getRemoteAddr();
    if (!srcAddr || !srcAddr->linkLocal())
        return TOptPtr(); // NULL

    std::vector<uint8_t> mac(8,0);

    // store hardware type
    SPtr<TIfaceIface> iface = RelIfaceMgr().getIfaceByID(msg->getIface());
    if (!iface) {
        // Should never happen
        return TOptPtr(); // NULL
    }
    writeUint16((char*)&mac[0], iface->getHardwareType());

    // now extract MAC address from the source address
    char* addr = srcAddr->getAddr();
    if ( (addr[11] != 0xff) || (addr[12] != 0xfe) ) {
        return TOptPtr(); // NULL
    }

    memcpy(&mac[2], addr + 8, 3);
    memcpy(&mac[5], addr + 13, 3);

    // Ok, create the option and return it
    return SPtr<TOpt>(new TOptGeneric(OPTION_CLIENT_LINKLAYER_ADDR,
                                      (char*)&mac[0], 8, NULL));
}

SPtr<TOpt> TRelTransMgr::getClientLinkLayerAddr(SPtr<TRelMsg> msg) {

    // Ignore messages that are not directly from client
    if ( (msg->getType() == RELAY_FORW_MSG) ||
         (msg->getType() == RELAY_REPL_MSG))
        return TOptPtr(); // NULL

    // Ok, this seems to be a message from a client. Let's try to
    // extract its DUID

    SPtr<TOpt> opt = msg->getOption(OPTION_CLIENTID);
    if (opt) {
        SPtr<TOpt> linkaddr = getLinkAddrFromDuid(opt);
        if (linkaddr) {
            return linkaddr;
        }
    }

    // Ok, let's try to extract the MAC address from source link-local
    // address.
    return getLinkAddrFromSrcAddr(msg);
}

void TRelTransMgr::shutdown() {
    IsDone = true;
}

bool TRelTransMgr::isDone() {
    return this->IsDone;
}

bool TRelTransMgr::doDuties() {
    return false;
}

char* TRelTransMgr::getCtrlAddr() {
    return this->ctrlAddr;
}

int  TRelTransMgr::getCtrlIface() {
    return this->ctrlIface;
}

void TRelTransMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

TRelTransMgr::~TRelTransMgr() {
    Log(Debug) << "RelTransMgr cleanup." << LogEnd;
}

void TRelTransMgr::instanceCreate(const std::string& xmlFile)
{
  if (Instance)
      Log(Crit) << "RelTransMgr instance already created. Application error!" << LogEnd;
  Instance = new TRelTransMgr(xmlFile);
}

TRelTransMgr& TRelTransMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "RelTransMgr istance not created yet. Application error. Emergency shutdown." << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}

std::ostream & operator<<(std::ostream &s, TRelTransMgr &x)
{
    s << "<TRelTransMgr>" << std::endl;
    s << "<!-- RelTransMgr dumps are not implemented yet -->" << std::endl;
    s << "</TRelTransMgr>" << std::endl;
    return s;
}

// Stub definitions, added because they are called in TMsg::storeSelf()
extern "C" {
void *hmac_sha (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type) {
        return NULL;
}

void *hmac_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf) {
        return NULL;
}

}
