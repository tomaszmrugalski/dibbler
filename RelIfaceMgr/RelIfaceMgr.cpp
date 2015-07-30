/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <cstdlib>
#include <fstream>
#include "Logger.h"
#include "IPv6Addr.h"
#include "Iface.h"
#include "SocketIPv6.h"
#include "RelIfaceMgr.h"
#include "RelCfgMgr.h"
#include "RelMsgGeneric.h"
#include "RelMsgRelayForw.h"
#include "RelMsgRelayRepl.h"
#include "RelOptInterfaceID.h"
#include "Portable.h"

TRelIfaceMgr * TRelIfaceMgr::Instance = 0;

using namespace std;

/*
 * constructor. Do nothing particular, just invoke IfaceMgr constructor
 */
TRelIfaceMgr::TRelIfaceMgr(const std::string& xmlFile) 
    : TIfaceMgr(xmlFile, true) {
}

TRelIfaceMgr::~TRelIfaceMgr() {
    Log(Debug) << "RelIfaceMgr cleanup." << LogEnd;
}

void TRelIfaceMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open( this->XmlFile.c_str() );
    xmlDump << *this;
    xmlDump.close();
}

/**
 * sends data to client. Uses multicast address as source
 * @param ifindex interface ID
 * @param data    buffer containing message ready to send
 * @param dataLen size of message
 * @param addr    destination address
 * @param port    UDP port to send the data to
 * returns true if message was send successfully
 */
bool TRelIfaceMgr::send(int ifindex, char *data, int dataLen, SPtr<TIPv6Addr> addr, int port)
{
    // find this interface
    SPtr<TIfaceIface> iface = this->getIfaceByID(ifindex);
    if (!iface) {
	Log(Error)  << "Send failed: No such interface id=" << ifindex << LogEnd;
	return false;
    }

    // find this socket
    SPtr<TIfaceSocket> ptrSocket;
    iface->firstSocket();
    ptrSocket = iface->getSocket();
    if (!ptrSocket) {
	Log(Error) << "Send failed: interface " << iface->getName() 
		   << "/" << iface->getID() << " has no open sockets." << LogEnd;
	return false;
    }

    // send it!
    if (ptrSocket->send(data, dataLen, addr, port) < 0)
	return false;
    return true;
}

/**
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SPtr to message object
 */
SPtr<TRelMsg> TRelIfaceMgr::select(unsigned long timeout) {
    
    // static buffer speeds things up
    static char data[2048];
    int dataLen=2048;

    SPtr<TIPv6Addr> peer (new TIPv6Addr());
    SPtr<TIPv6Addr> myaddr(new TIPv6Addr());
    int sockid;

    // read data
    sockid = TIfaceMgr::select(timeout, data, dataLen, peer, myaddr);
    if (sockid < 0) {
        Log(Warning) << "Socket read error: " << sockid << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    }

    if (dataLen<4) {
        Log(Warning) << "Received message is truncated (" << dataLen << " bytes)." << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    }

    // check message type
    int msgtype = data[0];

    if (msgtype > LEASEQUERY_REPLY_MSG) {
        Log(Warning) << "Invalid message type " << msgtype << " received." << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    }
    SPtr<TMsg> ptr;
    SPtr<TIfaceIface> iface;
    SPtr<TIfaceSocket> sock;

    // get interface
    iface = this->getIfaceBySocket(sockid);

    sock = iface->getSocketByFD(sockid);

    Log(Debug) << "Received " << dataLen << " bytes on the " << iface->getName() << "/"
               << iface->getID() << " interface (socket=" << sockid << ", addr="
               << peer->getPlain() << ", port=" << sock->getPort() << ")." << LogEnd;

    if (sock->getPort()!=DHCPSERVER_PORT) {
        Log(Error) << "Message was received on invalid (" << sock->getPort() << ") port." << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    }

    return decodeMsg(iface, peer, data, dataLen);
}

SPtr<TRelMsg> TRelIfaceMgr::decodeRelayForw(SPtr<TIfaceIface> iface,
						SPtr<TIPv6Addr> peer,
						char * data, int dataLen) {
    int ifindex = iface->getID();
    SPtr<TRelMsg> msg = new TRelMsgRelayForw(ifindex, peer, data, dataLen);
    return msg;
}

SPtr<TRelMsg> TRelIfaceMgr::decodeGeneric(SPtr<TIfaceIface> iface,
					      SPtr<TIPv6Addr> peer,
					      char * buf, int bufsize) {
    int ifindex = iface->getID();
    return new TRelMsgGeneric(ifindex, peer, buf, bufsize);
}

SPtr<TRelMsg> TRelIfaceMgr::decodeRelayRepl(SPtr<TIfaceIface> iface, 
						SPtr<TIPv6Addr> peer, 
						char * buf, int bufsize) {
    // SPtr<TIPv6Addr> linkAddrTbl[HOP_COUNT_LIMIT];
    // SPtr<TIPv6Addr> peerAddrTbl[HOP_COUNT_LIMIT];
    //SPtr<TRelOptInterfaceID> interfaceIDTbl[HOP_COUNT_LIMIT];
    //int hopTbl[HOP_COUNT_LIMIT];
    SPtr<TRelOptInterfaceID> ptrIfaceID;
    char * relayBuf=0;
    int relayLen = 0;
    int relays=0;

    /* decode RELAY_FORW message */
    if (bufsize < 34) {
	Log(Warning) << "Truncated RELAY_REPL message received." << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    }

    // char type = buf[0]; // ignore it
    // int hopCount = buf[1]; // this one is not currently needed either
    SPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
    SPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);
    buf+=34;
    bufsize-=34;
    
    // options: only INTERFACEID and RELAY_MSG are allowed
    while (bufsize>=4) {

	unsigned short code = readUint16(buf);
	unsigned short len  = readUint16(buf + sizeof(uint16_t));
	if (len>bufsize) {
	    Log(Error) << "Message RELAY-REPL truncated. There are " << (bufsize-len) 
		       << " bytes left to parse. Message dropped." << LogEnd;
            return SPtr<TRelMsg>(); // NULL
	}
	buf     += 4;
	bufsize -= 4;
	switch (code) {
	case OPTION_INTERFACE_ID:
	    if (bufsize<4) {
		Log(Warning) << "Truncated INTERFACE_ID option in RELAY_REPL message. Message dropped." << LogEnd;
                return SPtr<TRelMsg>(); // NULL
	    }
	    if (len!=4) {
		Log(Warning) << "Invalid INTERFACE_ID option, expected length " << 4
			     << ", actual length " << len << "." << LogEnd;
                return SPtr<TRelMsg>(); // NULL
	    }

	    ptrIfaceID = new TRelOptInterfaceID(buf, bufsize, 0);
	    buf     += ptrIfaceID->getSize()-4;
	    bufsize -= ptrIfaceID->getSize()-4;
	    break;
	case OPTION_RELAY_MSG:
	    relayBuf = buf;
	    relayLen = len;
	    buf     += relayLen;
	    bufsize -= relayLen;
	    break;
	default:
	    SPtr<TRelOptEcho> echo = RelCfgMgr().getEcho();
	    if (echo && echo->isOption(code)) {
		Log(Notice) << "Received echoed back option " << code << "." << LogEnd;
	    } else {
		Log(Warning) << "Invalid option " << code << " in RELAY_REPL message. Option ignored." << LogEnd;
	    }
	    buf     += len;
	    bufsize -= len;
	    continue;
	}
    }
    
    Log(Info) << "RELAY_REPL was decapsulated: link=" << linkAddr->getPlain() << ", peer=" << peerAddr->getPlain();
    if (ptrIfaceID)
	Log(Cont) << ", interfaceID=" << ptrIfaceID->getValue();
    Log(Cont) << LogEnd;

    if (!ptrIfaceID) {
	if (!RelCfgMgr().guessMode()) {
	    /* guessMode disabled */
	    Log(Warning) << "InterfaceID option is missing, guessMode disabled, unable to forward. Packet dropped." << LogEnd;
            return SPtr<TRelMsg>(); // NULL
	}

	/* guess mode enabled, let's find any interface */
	SPtr<TRelCfgIface> tmp;
	RelCfgMgr().firstIface();
	while (tmp = RelCfgMgr().getIface()) {
	    if (tmp->getID() == iface->getID()) 
		continue; // skip the interface we've received the data on
	    break;
	}
	if (!tmp) {
	    Log(Error) << "Guess-mode failed. Unable to find any interface the message can be relayed on. Please send interface-id option in server replies." << LogEnd;
            return SPtr<TRelMsg>(); // NULL
	}
	Log(Notice) << "Guess-mode successful. Using " << tmp->getFullName() << " as destination interface." << LogEnd;
	ptrIfaceID = new TRelOptInterfaceID(tmp->getInterfaceID(), 0);
    }

    //linkAddrTbl[relays] = linkAddr;
    //peerAddrTbl[relays] = peerAddr;
    //interfaceIDTbl[relays] = ptrIfaceID;
    //hopTbl[relays] = hopCount;
    relays++;

    SPtr<TRelMsg> msg;
    switch (relayBuf[0]) {
    case RELAY_REPL_MSG:
	//msg = this->decodeRelayRepl(iface, peer, buf, bufsize);
	msg = new TRelMsgRelayRepl(iface->getID(), peer, relayBuf, relayLen);
	break;
    case RELAY_FORW_MSG:
	Log(Error) << "RELAY_REPL contains RELAY_FORW message." << LogEnd;
        return SPtr<TRelMsg>(); // NULL
    default:
	msg = this->decodeGeneric(iface, peer, relayBuf, relayLen);
    };
    // inform that this message should be sent to the peerAddr address on the ptrIface interface.
    msg->setDestination(ptrIfaceID->getValue(), peerAddr);
    return (Ptr*)msg;
}

SPtr<TRelMsg> TRelIfaceMgr::decodeMsg(SPtr<TIfaceIface> iface, 
					  SPtr<TIPv6Addr> peer, 
					  char * data, int dataLen) {
    if (dataLen <= 0) 
        return SPtr<TRelMsg>(); // NULL
    
    // create specific message object
    switch (data[0]) {
    default: {
	Log(Warning) << "Unknown message type " << (int)(data[0]) << ", relaying anyway." << LogEnd;
        // no break here
    }
    case SOLICIT_MSG:
    case REQUEST_MSG:
    case CONFIRM_MSG:
    case RENEW_MSG:
    case REBIND_MSG:
    case RELEASE_MSG:
    case DECLINE_MSG:
    case INFORMATION_REQUEST_MSG:
    case ADVERTISE_MSG:
    case REPLY_MSG:
	return decodeGeneric(iface, peer, data, dataLen);
    case RELAY_FORW_MSG: 
	return decodeRelayForw(iface, peer, data, dataLen);
    case RELAY_REPL_MSG:
	return decodeRelayRepl(iface, peer, data, dataLen);
    case RECONFIGURE_MSG:
        return SPtr<TRelMsg>(); // NULL
    }
}

void TRelIfaceMgr::instanceCreate(const std::string& xmlFile)
{
    if (Instance)
      Log(Crit) << "RelIfaceMgr instance already created. Application error!" << LogEnd;
    Instance = new TRelIfaceMgr(xmlFile);
}

TRelIfaceMgr& TRelIfaceMgr::instance()
{
    if (!Instance) {
      Log(Crit) << "RelIfaceMgr istance not created yet. Application error. Emergency shutdown." << LogEnd;
      exit(EXIT_FAILURE);
    }
    return *Instance;
}

ostream & operator <<(ostream & strum, TRelIfaceMgr &x) {
    strum << "<RelIfaceMgr>" << std::endl;
    SPtr<TIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
	strum << *ptr;
    }
    strum << "</RelIfaceMgr>" << std::endl;
    return strum;
}
