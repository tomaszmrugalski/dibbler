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

/*
 * ServerInterfaceManager - sends and receives messages (server version)
 * @date 2003-10-14
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

using namespace std;
using namespace logger;

/*
 * constructor. Do nothing particular, just invoke IfaceMgr constructor
 */
TSrvIfaceMgr::TSrvIfaceMgr() 
    :TIfaceMgr() {
}

/* 
 * sends data to client. Uses multicast address as source
 * @param iface - interface ID
 * @param msg - buffer containing message ready to send
 * @param size - size of message
 * @param addr - destination address
 * returns true if message was send successfully
 */
bool TSrvIfaceMgr::sendMulticast  (int iface, char *msg, int size, 
				   SmartPtr<TIPv6Addr> addr) {
    // find this interface
    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	// uh oh. No such interface
	std::clog << logger::logError 
		  << "Send failed: No such interface id=" 
		  << iface << logger::endl;
	return false;
    }

    // find this socket
    SmartPtr<TIfaceSocketIPv6> ptrSocket;
    ptrIface->firstSocket();
    ptrSocket = ptrIface->getSocket();
    if (!ptrSocket) {
	// uh oh.
	clog << logError << "Send failed: interface \"" << ptrIface->getName() 
	     << "\",id=" << iface << " has no open sockets." << logger::endl;
	return false;
    }

    // send it!
    return ptrSocket->send(msg,size,addr,DHCPCLIENT_PORT);
}

/*
 * sends data to addr address from unicast link-scoped addr
 * @param iface - interface ID
 * @param msg - buffer containing message ready to send
 * @param size - size of message
 * @param addr - destination address
 * returns true if message was send successfully
 */
bool TSrvIfaceMgr::sendUnicast(int iface, char *msg, 
			       int size, SmartPtr<TIPv6Addr> addr) {
    //FIXME: Is this method really needed on server side?
    //Sure it is. I'm just too lazy to implement Unicast support :)
    return false;
}

/*
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SmartPtr to message object
 */
SmartPtr<TMsg> TSrvIfaceMgr::select(unsigned long timeout) {
    
    // static buffer speeds things up
    static char buf[4096];
    int bufsize=4096;

    SmartPtr<TIPv6Addr> Peer (new TIPv6Addr());
    char peer[16];
    int sockid;
    int msgtype;

    // read data
    sockid = TIfaceMgr::select(timeout,buf,bufsize,Peer);
    if (sockid>0) {
	if (bufsize<4) {
	    std::clog << logger::logWarning 
		      << "Received message is too short (" << bufsize 
		      << ") bytes." << logger::endl;
	    return 0; //NULL
	}
	
	// check message type
	msgtype = buf[0];
	SmartPtr<TMsg> ptr;
	SmartPtr<TIfaceIface> ptrIface;

	// get interface
	ptrIface = this->getIfaceBySocket(sockid);

	int ifaceid = ptrIface->getID();
	std::clog << logger::logNotice << "Received " << bufsize 
		  << " bytes via " << sockid 
		  << " socket, ifaceid=" << ifaceid 
		  << ",type=" << msgtype << logger::endl;

	// create specific message object
	switch (msgtype) {
	case SOLICIT_MSG:
	    return new TSrvMsgSolicit(That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, Peer, buf, bufsize);
	case REQUEST_MSG:
            return new TSrvMsgRequest(That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, Peer, buf, bufsize);
	case CONFIRM_MSG:
	    return new TSrvMsgConfirm(That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid,  Peer, buf, bufsize);
	case RENEW_MSG:
	    return new TSrvMsgRenew  (That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid,  Peer, buf, bufsize);
	case REBIND_MSG:
	    return new TSrvMsgRebind (That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, Peer, buf, bufsize);
	case RELEASE_MSG:
	    return new TSrvMsgRelease(That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, Peer, buf, bufsize);
	case DECLINE_MSG:
	    return new TSrvMsgDecline(That, this->SrvTransMgr, 
				      this->SrvCfgMgr, this->SrvAddrMgr,
				      ifaceid, Peer, buf, bufsize);
	case INFORMATION_REQUEST_MSG:
	    return new TSrvMsgInfRequest(That, this->SrvTransMgr, 
					 this->SrvCfgMgr, this->SrvAddrMgr,
					 ifaceid, Peer, buf, bufsize);
	case ADVERTISE_MSG:
	case REPLY_MSG:
	case RECONFIGURE_MSG:
	    std::clog << logger::logWarning << "Illegal message type " 
		      << msgtype << " received." << logger::endl;
	    return 0; //NULL;;
	case RELAY_FORW:
	case RELAY_REPL:
	default:
	    std::clog << logger::logWarning << "Message type " 
		      << msgtype << " not supported. Ignoring." << logger::endl;
	    return 0; //NULL
	}
    } else {
	return 0; //NULL
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
