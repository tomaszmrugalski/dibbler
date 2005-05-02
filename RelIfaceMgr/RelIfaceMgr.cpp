/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelIfaceMgr.cpp,v 1.8 2005-05-02 21:08:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/05/02 20:58:13  thomson
 * Support for multiple relays added. (bug #107)
 *
 * Revision 1.6  2005/04/29 00:34:16  thomson
 * *** empty log message ***
 *
 * Revision 1.5  2005/04/25 00:19:20  thomson
 * Changes in progress.
 *
 * Revision 1.4  2005/01/23 23:17:53  thomson
 * Relay/global address support related improvements.
 *
 * Revision 1.3  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.2  2005/01/11 23:35:22  thomson
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include "RelCommon.h"
#include "Logger.h"
#include "IPv6Addr.h"
#include "Iface.h"
#include "SocketIPv6.h"
#include "RelIfaceMgr.h"
#include "RelMsgGeneric.h"
#include "RelMsgRelayForw.h"
#include "RelMsgRelayRepl.h"
#include "RelOptInterfaceID.h"

/*
 * constructor. Do nothing particular, just invoke IfaceMgr constructor
 */
TRelIfaceMgr::TRelIfaceMgr(string xmlFile) 
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
 * @param iface - interface ID
 * @param msg - buffer containing message ready to send
 * @param size - size of message
 * @param addr - destination address
 * returns true if message was send successfully
 */
bool TRelIfaceMgr::send(int ifindex, char *data, int dataLen, 
			SmartPtr<TIPv6Addr> addr, int port) {
    // find this interface
    SmartPtr<TIfaceIface> iface = this->getIfaceByID(ifindex);
    if (!iface) {
	Log(Error)  << "Send failed: No such interface id=" << ifindex << LogEnd;
	return false;
    }

    // find this socket
    SmartPtr<TIfaceSocket> ptrSocket;
    iface->firstSocket();
    ptrSocket = iface->getSocket();
    if (!ptrSocket) {
	Log(Error) << "Send failed: interface " << iface->getName() 
		   << "/" << iface->getID() << " has no open sockets." << LogEnd;
	return false;
    }

    // send it!
    return ptrSocket->send(data, dataLen, addr, port);
}

/**
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SmartPtr to message object
 */
SmartPtr<TRelMsg> TRelIfaceMgr::select(unsigned long timeout) {
    
    // static buffer speeds things up
    static char data[2048];
    int dataLen=2048;

    SmartPtr<TIPv6Addr> peer (new TIPv6Addr());
    int sockid;
    int msgtype;

    // read data
    sockid = TIfaceMgr::select(timeout, data, dataLen, peer);
    if (sockid>0) {
	if (dataLen<4) {
	    Log(Warning) << "Received message is truncated (" << dataLen << " bytes)." << LogEnd;
	    return 0; //NULL
	}
	
	// check message type
	msgtype = data[0];
	SmartPtr<TMsg> ptr;
	SmartPtr<TIfaceIface> iface;
	SmartPtr<TIfaceSocket> sock;

	// get interface
	iface = this->getIfaceBySocket(sockid);

	sock = iface->getSocketByFD(sockid);

	Log(Debug) << "Received " << dataLen << " bytes on the " << iface->getName() << "/" 
		   << iface->getID() << " interface (socket=" << sockid << ", addr=" << peer->getPlain() 
		   << ", port=" << sock->getPort() << ")." << LogEnd;
	
	if (sock->getPort()!=DHCPSERVER_PORT) {
	    Log(Error) << "Message was received on invalid (" << sock->getPort() << ") port." << LogEnd;
	    return 0;
	}

	return this->decodeMsg(iface, peer, data, dataLen);
    } 
    return 0;
}

SmartPtr<TRelMsg> TRelIfaceMgr::decodeRelayForw(SmartPtr<TIfaceIface> iface, 
						SmartPtr<TIPv6Addr> peer, 
						char * data, int dataLen) {
    int ifindex = iface->getID();
    SmartPtr<TRelMsg> msg = new TRelMsgRelayForw(this->Ctx, ifindex, peer, data, dataLen);
    return msg;
}

SmartPtr<TRelMsg> TRelIfaceMgr::decodeGeneric(SmartPtr<TIfaceIface> iface, 
					      SmartPtr<TIPv6Addr> peer, 
					      char * buf, int bufsize) {
    int ifindex = iface->getID();
    return new TRelMsgGeneric(this->Ctx, ifindex, peer, buf, bufsize);
}

SmartPtr<TRelMsg> TRelIfaceMgr::decodeRelayRepl(SmartPtr<TIfaceIface> iface, 
						SmartPtr<TIPv6Addr> peer, 
						char * buf, int bufsize) {
    SmartPtr<TIPv6Addr> linkAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TIPv6Addr> peerAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TRelOptInterfaceID> interfaceIDTbl[HOP_COUNT_LIMIT];
    int hopTbl[HOP_COUNT_LIMIT];
    SmartPtr<TRelOptInterfaceID> ptrIfaceID;
    bool relay;
    int relays=0;

    /* decode RELAY_FORW message */
    if (bufsize < 34) {
	Log(Warning) << "Truncated RELAY_REPL message received." << LogEnd;
	return 0;
    }

    // char type = buf[0];    // ignore it
    int hopCount = buf[1]; // this one is not currently needed either
    SmartPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
    SmartPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);
    buf+=34;
    bufsize-=34;
    
    relay = false;
    // options: only INTERFACEID and RELAY_MSG are allowed
    while (!relay && bufsize>=4) {
	short code = ntohs( * ((short*) (buf)));
	short len  = ntohs(*((short*)(buf+2)));
	buf     += 4;
	bufsize -= 4;
	switch (code) {
	case OPTION_INTERFACE_ID:
	    if (bufsize<8) {
		Log(Warning) << "Truncated INTERFACE_ID option in RELAY_REPL message. Message dropped." << LogEnd;
		return 0;
	    }
	    ptrIfaceID = new TRelOptInterfaceID(buf, bufsize, 0);
	    break;
	case OPTION_RELAY_MSG:
	    relay = true;
	    break;
	default:
	    Log(Warning) << "Invalid option " << code << " in RELAY_REPL message. Message dropped." << LogEnd;
	}
    }

    linkAddrTbl[relays] = linkAddr;
    peerAddrTbl[relays] = peerAddr;
    interfaceIDTbl[relays] = ptrIfaceID;
    hopTbl[relays] = hopCount;
    relays++;
    
    Log(Info) << "RELAY_REPL was decapsulated: link=" << linkAddr->getPlain() << ", peer=" << peerAddr->getPlain();
    if (ptrIfaceID)
	Log(Cont) << ", interfaceID=" << ptrIfaceID->getValue();
    Log(Cont) << LogEnd;
    
    if (!ptrIfaceID) {
	Log(Warning) << "InterfaceID option is missing, unable to forward. Packet dropped." << LogEnd;
	return 0;
    }

    SmartPtr<TRelMsg> msg;
    switch (buf[0]) {
    case RELAY_REPL_MSG:
	//msg = this->decodeRelayRepl(iface, peer, buf, bufsize);
	msg = new TRelMsgRelayRepl(this->Ctx, iface->getID(), peer, buf, bufsize);
	break;
    case RELAY_FORW_MSG:
	Log(Error) << "RELAY_REPL contains RELAY_FORW message." << LogEnd;
	return 0;
    default:
	msg = this->decodeGeneric(iface, peer, buf, bufsize);
    };
    // inform that this message should be sent to the peerAddr address on the ptrIface interface.
    msg->setDestination(ptrIfaceID->getValue(), peerAddr);
    return (Ptr*)msg;
}

SmartPtr<TRelMsg> TRelIfaceMgr::decodeMsg(SmartPtr<TIfaceIface> iface, 
					  SmartPtr<TIPv6Addr> peer, 
					  char * data, int dataLen) {
    if (dataLen <= 0) 
	return 0;
    
    // create specific message object
    switch (data[0]) {
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
	return this->decodeGeneric(iface, peer, data, dataLen);
    case RELAY_FORW_MSG: 
	return this->decodeRelayForw(iface, peer, data, dataLen);
    case RELAY_REPL_MSG:
	return this->decodeRelayRepl(iface, peer, data, dataLen);
    case RECONFIGURE_MSG:
    default:
	Log(Warning) << "Message type " << (int)(data[0]) << " is not supported." << LogEnd;
	return 0;
    }
}


ostream & operator <<(ostream & strum, TRelIfaceMgr &x) {
    strum << "<RelIfaceMgr>" << std::endl;
    SmartPtr<TIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
	strum << *ptr;
    }
    strum << "</RelIfaceMgr>" << std::endl;
    return strum;
}
