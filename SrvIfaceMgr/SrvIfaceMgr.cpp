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
			SmartPtr<TIPv6Addr> addr) {
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
	Log(Error) << "Send failed: interface \"" << ptrIface->getName() 
		   << "\",id=" << iface << " has no open sockets." << LogEnd;
	return false;
    }

    // send it!
    return ptrSocket->send(msg,size,addr,DHCPCLIENT_PORT);
}

/**
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SmartPtr to message object
 */
SmartPtr<TMsg> TSrvIfaceMgr::select(unsigned long timeout) {
    
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

	int ifaceid = ptrIface->getID();
	Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getName() << "/" 
		   << ptrIface->getID() << " (socket=" << sockid << ", addr=" << *peer << "." 
		   << ")." << LogEnd;

	// create specific message object
	switch (msgtype) {
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
	case RELAY_FORW: {
	    return this->decodeRelayForw(ptrIface, peer, buf, bufsize);
	}
	case ADVERTISE_MSG:
	case REPLY_MSG:
	case RECONFIGURE_MSG:
	case RELAY_REPL:
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
							0,  // flags
							0,  // MAC
							0,  // MAC length
							0,  // link address
							0,  // link address count
							0); // hardware type
    relay->setUnderlaying(under);
    this->IfaceLst.append((Ptr*)relay);

    if (!under->appendRelay(relay, interfaceID)) {
	Log(Crit) << "Unable to setup " << name << "/" << ifindex 
		  << " relay: underlaying interface " << under->getName() << "/" << underIfindex 
		  << " already has " << MAX_RELAYS << " relays defined." << LogEnd;
	return false;
    }

    Log(Notice) << "Relay " << name << "/" << ifindex << " (underlaying " << under->getName()
		<< "/" << under->getID() << ") has been configured." << LogEnd;

    return true;
}

 SmartPtr<TMsg> TSrvIfaceMgr::decodeRelayForw(SmartPtr<TSrvIfaceIface> ptrIface, 
					      SmartPtr<TIPv6Addr> peer, 
					      char * buf, int bufsize) {
     /* decode RELAY_FORW message */
     if (bufsize < 34) {
	 Log(Warning) << "Truncated RELAY_FORW message received." << LogEnd;
	 return 0;
     }

     char type = buf[0];
     int hopCount = buf[1];
     SmartPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
     SmartPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);

     Log(Debug) << "### linkAddr="<< linkAddr->getPlain() << " peerAddr=" << peerAddr->getPlain()
		<< " peer=" << peer->getPlain() << LogEnd;

     Log(Debug) << "### RELAY_FORW was received on the " << ptrIface->getName() << " interface." << LogEnd;
     return 0;
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
