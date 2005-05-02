#include <malloc.h>
#include <stdio.h>
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
#include "SrvOptInterfaceID.h"
#include "IPv6Addr.h"
#include "AddrClient.h"
#include "SrvIfaceIface.h"

/**
 * ServerInterfaceManager - sends and receives messages (server version)
 * @date 2003-10-14
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

using namespace std;

/*
 * constructor. Do nothing particular, just invoke IfaceMgr constructor
 */
TSrvIfaceMgr::TSrvIfaceMgr(string xmlFile) 
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
        Log(Notice) << "Detected iface " << ptr->name << "/" << ptr->id << ", flags=" 
		    << ptr->flags << ", MAC=" << this->printMac(ptr->mac, ptr->maclen) << "." << LogEnd;
	
        SmartPtr<TIfaceIface> iface(new TSrvIfaceIface(ptr->name,ptr->id,
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
 * sends data to client. Uses multicast address as source
 * @param iface - interface ID
 * @param msg - buffer containing message ready to send
 * @param size - size of message
 * @param addr - destination address
 * returns true if message was send successfully
 */
bool TSrvIfaceMgr::send(int iface, char *msg, int size, 
			SmartPtr<TIPv6Addr> addr, int port) {
    // find this interface
    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error)  << "Send failed: No such interface id=" << iface << LogEnd;
	return false;
    }

    // find this socket
    SmartPtr<TIfaceSocket> ptrSocket;
    ptrIface->firstSocket();
    ptrSocket = ptrIface->getSocket();
    if (!ptrSocket) {
	Log(Error) << "Send failed: interface " << ptrIface->getName() 
		   << "/" << iface << " has no open sockets." << LogEnd;
	return false;
    }

    // send it!
    return ptrSocket->send(msg,size,addr,port);
}

/**
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SmartPtr to message object
 */
SmartPtr<TSrvMsg> TSrvIfaceMgr::select(unsigned long timeout) {
    
    // static buffer speeds things up
    static char buf[4096];
    int bufsize=4096;

    SmartPtr<TIPv6Addr> peer (new TIPv6Addr());
    int sockid;
    int msgtype;

    // read data
    sockid = TIfaceMgr::select(timeout,buf,bufsize,peer);
    if (sockid>0) {
	if (bufsize<4) {
	    Log(Warning) << "Received message is too short (" << bufsize << ") bytes." << LogEnd;
	    return 0; //NULL
	}
	
	// check message type
	msgtype = buf[0];
	SmartPtr<TMsg> ptr;
	SmartPtr<TSrvIfaceIface> ptrIface;

	// get interface
	ptrIface = (Ptr*)this->getIfaceBySocket(sockid);

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
	    return this->decodeMsg(ptrIface, peer, buf, bufsize);
	case RELAY_FORW_MSG: {
	    return this->decodeRelayForw(ptrIface, peer, buf, bufsize);
	}
	case ADVERTISE_MSG:
	case REPLY_MSG:
	case RECONFIGURE_MSG:
	case RELAY_REPL_MSG:
	    Log(Warning) << "Illegal message type " << msgtype << " received." << LogEnd;
	    return 0; //NULL;;
	default:
	    Log(Warning) << "Message type " << msgtype << " not supported. Ignoring." << LogEnd;
	    return 0; //NULL
	}
    } else {
	return 0; //NULL
    }
}

bool TSrvIfaceMgr::setupRelay(string name, int ifindex, int underIfindex, int interfaceID) {
    SmartPtr<TSrvIfaceIface> under = (Ptr*)this->getIfaceByID(underIfindex);
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

    SmartPtr<TSrvIfaceIface> relay = new TSrvIfaceIface((const char*)name.c_str(), ifindex, 
							IF_UP | IF_RUNNING | IF_MULTICAST,   // flags
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

SmartPtr<TSrvMsg> TSrvIfaceMgr::decodeRelayForw(SmartPtr<TSrvIfaceIface> ptrIface, 
						SmartPtr<TIPv6Addr> peer, 
						char * buf, int bufsize) {
    SmartPtr<TSrvIfaceIface> relayIface;

    SmartPtr<TIPv6Addr> linkAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TIPv6Addr> peerAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TSrvOptInterfaceID> interfaceIDTbl[HOP_COUNT_LIMIT];
    int hopTbl[HOP_COUNT_LIMIT];
    int relays=0;
    
    while (bufsize>0 && buf[0]==RELAY_FORW_MSG) {
	/* decode RELAY_FORW message */
	if (bufsize < 34) {
	    Log(Warning) << "Truncated RELAY_FORW message received." << LogEnd;
	    return 0;
	}

	char type = buf[0];
	if (type!=RELAY_FORW_MSG)
	    return 0;
	int hopCount = buf[1];
	bool relay;
	SmartPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
	SmartPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);
	buf+=34;
	bufsize-=34;

	SmartPtr<TSrvOptInterfaceID> ptrIfaceID;
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
		    Log(Warning) << "Truncated INTERFACE_ID option in RELAY_FORW message. Message dropped." << LogEnd;
		    return 0;
		}
		ptrIfaceID = new TSrvOptInterfaceID(buf, bufsize, 0);
		break;
	    case OPTION_RELAY_MSG:
		relay = true;
		break;
	    default:
		Log(Warning) << "Invalid option " << code << " in RELAY_FORW message. Message dropped.\n" << LogEnd;
	    }
	    if (!relay) { // check next option
		buf     += len;
		bufsize -= len;
	    }
	}

	linkAddrTbl[relays] = linkAddr;
	peerAddrTbl[relays] = peerAddr;
	interfaceIDTbl[relays] = ptrIfaceID;
	hopTbl[relays] = hopCount;
	relays++;

	Log(Info) << "RELAY_FORW was decapsulated: link=" << linkAddr->getPlain() << ", peer=" << peerAddr->getPlain();
	if (ptrIfaceID)
	    Log(Cont) << ", interfaceID=" << ptrIfaceID->getValue();
	Log(Cont) << LogEnd;

	relayIface = ptrIface->getRelayByInterfaceID(ptrIfaceID->getValue());
	if (!relayIface) {
	    Log(Warning) << "Unable to find relay interface with interfaceID=" << ptrIfaceID->getValue() << " defined on the " 
			 << ptrIface->getName() << "/" << ptrIface->getID() << " interface." << LogEnd;
	    return 0;
	}
	ptrIface = relayIface;

    }

    // now switch to relay interface

    SmartPtr<TSrvMsg> msg = this->decodeMsg(ptrIface, peer, buf, bufsize);
    for (int i=0; i<relays; i++) {
	msg->addRelayInfo(linkAddrTbl[i], peerAddrTbl[i], hopTbl[i], interfaceIDTbl[i]);
    }
    return (Ptr*)msg;
 }

SmartPtr<TSrvMsg> TSrvIfaceMgr::decodeMsg(SmartPtr<TSrvIfaceIface> ptrIface, 
					     SmartPtr<TIPv6Addr> peer, 
					     char * buf, int bufsize) {
    int ifaceid = ptrIface->getID();
    if (bufsize<4) 
	return 0;
    switch (buf[0]) {
    case SOLICIT_MSG:
	return new TSrvMsgSolicit(That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid, peer, buf, bufsize);
    case REQUEST_MSG:
	return new TSrvMsgRequest(That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, peer, buf, bufsize);
    case CONFIRM_MSG:
	return new TSrvMsgConfirm(That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid,  peer, buf, bufsize);
    case RENEW_MSG:
	return new TSrvMsgRenew  (That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid,  peer, buf, bufsize);
    case REBIND_MSG:
	return new TSrvMsgRebind (That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid, peer, buf, bufsize);
    case RELEASE_MSG:
	return new TSrvMsgRelease(That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid, peer, buf, bufsize);
    case DECLINE_MSG:
	return new TSrvMsgDecline(That, this->SrvTransMgr, 
				  this->SrvCfgMgr, this->SrvAddrMgr,
				  ifaceid, peer, buf, bufsize);
    case INFORMATION_REQUEST_MSG:
	return new TSrvMsgInfRequest(That, this->SrvTransMgr, 
				     this->SrvCfgMgr, this->SrvAddrMgr,
				     ifaceid, peer, buf, bufsize);
    default:
	Log(Warning) << "Illegal message type " << (int)(buf[0]) << " received." << LogEnd;
	return 0; //NULL;;
    }
}

/*
 * remember SmartPtrs to all managers (including this one)
 */
void TSrvIfaceMgr::setThats(SmartPtr<TSrvIfaceMgr> srvIfaceMgr,
			    SmartPtr<TSrvTransMgr> srvTransMgr,
			    SmartPtr<TSrvCfgMgr> srvCfgMgr,
			    SmartPtr<TSrvAddrMgr> srvAddrMgr) {
    SrvCfgMgr=srvCfgMgr;
    SrvAddrMgr=srvAddrMgr;
    SrvTransMgr=srvTransMgr;
    That=srvIfaceMgr;
}

ostream & operator <<(ostream & strum, TSrvIfaceMgr &x) {
    strum << "<SrvIfaceMgr>" << std::endl;
    SmartPtr<TSrvIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
	strum << *ptr;
    }
    strum << "</SrvIfaceMgr>" << std::endl;
    return strum;
}
