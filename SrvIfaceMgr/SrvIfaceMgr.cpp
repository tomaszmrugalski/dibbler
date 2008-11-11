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
 * $Id: SrvIfaceMgr.cpp,v 1.34 2008-11-11 22:41:49 thomson Exp $
 *
 */

#include <stdio.h> // required for DEV-CPP compilation
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
#include "SrvIfaceIface.h"
#include "SrvOptEcho.h"
#include "SrvOptGeneric.h"

using namespace std;

/*
 * constructor. 
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
        Log(Notice) << "Detected iface " << ptr->name << "/" << ptr->id 
                 // << ", flags=" << ptr->flags 
                    << ", MAC=" << this->printMac(ptr->mac, ptr->maclen) << "." << LogEnd;
	
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
    return (bool)(ptrSocket->send(msg,size,addr,port));
}

/**
 * reads messages from all interfaces
 * it's wrapper around IfaceMgr::select(...) method
 * @param timeout - how long can we wait for packets?
 * returns SmartPtr to message object
 */
SmartPtr<TSrvMsg> TSrvIfaceMgr::select(unsigned long timeout) {
    
    // static buffer speeds things up
    const int maxBufsize = 4096;
    int bufsize=maxBufsize;
    static char buf[maxBufsize];

#if 0
    buf[0]=buf[1]=buf[2]=buf[3]=2;
    buf[4]=buf[5]=buf[6]=buf[7]=2;

    SPtr<TSrvOptInterfaceID> opt1 = new TSrvOptInterfaceID(3, 0);
    SPtr<TSrvOptInterfaceID> opt2 = new TSrvOptInterfaceID(5, 0);

    Log(Info) << "#### opt1->" << opt1->getPlain() << LogEnd;
    Log(Info) << "#### opt2->" << opt2->getPlain() << LogEnd;

    if (*opt1 == *opt2)
	Log(Info) << "#### equal " << LogEnd;
    else
	Log(Info) << "#### NOT equal" << LogEnd;

    exit(-1);
#endif

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
	//SmartPtr<TMsg> ptr;
	SmartPtr<TSrvIfaceIface> ptrIface;

	// get interface
	ptrIface = (Ptr*)this->getIfaceBySocket(sockid);

	Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getName() << "/" 
		   << ptrIface->getID() << " (socket=" << sockid << ", addr=" << *peer << "." 
		   << ")." << LogEnd;

	// create specific message object
    SmartPtr<TSrvMsg> ptr;
	switch (msgtype) {
	case SOLICIT_MSG:
	case REQUEST_MSG:
	case CONFIRM_MSG:
	case RENEW_MSG:
	case REBIND_MSG:
	case RELEASE_MSG:
	case DECLINE_MSG:
	case INFORMATION_REQUEST_MSG:
	case LEASEQUERY_MSG:
	    ptr = this->decodeMsg(ptrIface, peer, buf, bufsize);
        if (!ptr->validateReplayDetection() ||
            !ptr->validateAuthInfo(buf, bufsize)) {
            Log(Error) << "Auth: Authorization failed, message dropped." << LogEnd;
            return 0;
        }
        return ptr;
	case RELAY_FORW_MSG:
	    ptr = this->decodeRelayForw(ptrIface, peer, buf, bufsize);
        if (!ptr)
            return 0;
        if (!ptr->validateReplayDetection() ||
            !ptr->validateAuthInfo(buf, bufsize)) {
            Log(Error) << "Auth: validation failed, message dropped." << LogEnd;
            return 0;
        }
        return ptr;
	case ADVERTISE_MSG:
	case REPLY_MSG:
	case RECONFIGURE_MSG:
	case RELAY_REPL_MSG:
	case LEASEQUERY_REPLY_MSG:
	    Log(Warning) << "Illegal message type " << msgtype << " received." << LogEnd;
	    return 0; //NULL;
	default:
	    Log(Warning) << "Message type " << msgtype << " not supported. Ignoring." << LogEnd;
	    return 0; //NULL
	}
    } else {
	return 0; //NULL
    }
}

bool TSrvIfaceMgr::setupRelay(string name, int ifindex, int underIfindex, SPtr<TSrvOptInterfaceID> interfaceID) {
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

    SmartPtr<TIPv6Addr> linkAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TIPv6Addr> peerAddrTbl[HOP_COUNT_LIMIT];
    SmartPtr<TSrvOptInterfaceID> interfaceIDTbl[HOP_COUNT_LIMIT];
    int hopTbl[HOP_COUNT_LIMIT];
    List(TSrvOptGeneric) echoListTbl[HOP_COUNT_LIMIT];
    SmartPtr<TSrvIfaceIface> relayIface;
    int relays=0; // number of nested RELAY_FORW messages
    SPtr<TSrvOptRemoteID> remoteID = 0;
    SPtr<TSrvOptEcho> echo = 0;
    SPtr<TSrvOptGeneric> gen = 0;

    char * relay_buf = buf;
    int relay_bufsize = bufsize;

    for (int j=0;j<HOP_COUNT_LIMIT; j++)
	echoListTbl[j].clear();

    while (bufsize>0 && buf[0]==RELAY_FORW_MSG) {
	/* decode RELAY_FORW message */
	if (bufsize < 34) {
	    Log(Warning) << "Truncated RELAY_FORW message received." << LogEnd;
	    return 0;
	}

	SmartPtr<TSrvOptInterfaceID> ptrIfaceID;
	ptrIfaceID = 0;

	char type = buf[0];
	if (type!=RELAY_FORW_MSG)
	    return 0;
	int hopCount = buf[1];
	int optRelayCnt = 0;
	int optIfaceIDCnt = 0;

	SmartPtr<TIPv6Addr> linkAddr = new TIPv6Addr(buf+2,false);
	SmartPtr<TIPv6Addr> peerAddr = new TIPv6Addr(buf+18, false);
	buf+=34;
	bufsize-=34;

	// options: only INTERFACE-ID and RELAY_MSG are allowed
	while (bufsize>=4) {
	    unsigned short code = ntohs( *((unsigned short*)(buf)));
	    unsigned short len  = ntohs( *((unsigned short*)(buf+2)));
	    buf     += 4;
	    bufsize -= 4;
	    
	    if (len > bufsize) {
		Log(Warning) << "Truncated option " << code << ": " << bufsize << " bytes remaining, but length is " << len 
			     << "." << LogEnd;
		return 0;
	    }

	    switch (code) {
	    case OPTION_INTERFACE_ID:
		if (bufsize<4) {
		    Log(Warning) << "Truncated INTERFACE_ID option (length: " << bufsize  
				 << ") in RELAY_FORW message. Message dropped." << LogEnd;
		    return 0;
		}
		ptrIfaceID = new TSrvOptInterfaceID(buf, len, 0);
		optIfaceIDCnt++;
		break;
	    case OPTION_RELAY_MSG:
		relay_buf = buf;
		relay_bufsize = len;
		optRelayCnt++;
		break;
	    case OPTION_REMOTE_ID:
		remoteID = new TSrvOptRemoteID(buf, len, 0);
		break;
	    case OPTION_ERO:
		Log(Debug) << "Echo Request received in RELAY_FORW." << LogEnd;
		echo = new TSrvOptEcho(buf, len, 0);
		break;
	    default:
		gen = new TSrvOptGeneric(code, buf, len, 0);
		echoListTbl[relays].append(gen);

	    }
	    buf     += len;
	    bufsize -= len;
	}

	// remember options to be echoed
	echoListTbl[relays].first();
	while (gen = echoListTbl[relays].get()) {
	    if (!echo) {
		Log(Warning) << "Invalid option (" << gen->getOptType() << ") in RELAY_FORW message was ignored." << LogEnd;
		echoListTbl[relays].del();
	    } else {
		if (!echo->isOption(gen->getOptType())) {
		    Log(Warning) << "Invalid option (" << gen->getOptType() << ") in RELAY_FORW message was ignored." << LogEnd;
		    echoListTbl[relays].del();
		} else {
		    Log(Info) << "Option " << gen->getOptType() << " will be echoed back." << LogEnd;
		}
	    }
	    
	}


	// remember those links
	linkAddrTbl[relays] = linkAddr;
	peerAddrTbl[relays] = peerAddr;
	interfaceIDTbl[relays] = ptrIfaceID;
	hopTbl[relays] = hopCount;
	relays++;

	if (relays> HOP_COUNT_LIMIT) {
	    Log(Error) << "Message is nested more than allowed " << HOP_COUNT_LIMIT << " times. Message dropped." << LogEnd;
	    return 0;
	}

	if (optRelayCnt!=1) {
	    Log(Error) << optRelayCnt << " RELAY_MSG options received, but exactly one was expected. Message dropped." << LogEnd;
	    return 0;
	}
	if (optIfaceIDCnt>1) {
	    Log(Error) << "More than one (" << optIfaceIDCnt 
		       << ") interface-ID options received, but at most 1 was expected. Message dropped." << LogEnd;
	    return 0;
	}

	Log(Info) << "RELAY_FORW was decapsulated: link=" << linkAddr->getPlain() << ", peer=" << peerAddr->getPlain();

	bool guessMode = SrvCfgMgr->guessMode();

	if (ptrIfaceID) {
	    // find relay interface based on the interface-id option
	    Log(Cont) << ", interfaceID len=" << ptrIfaceID->getSize() << LogEnd;
	    relayIface = ptrIface->getRelayByInterfaceID(ptrIfaceID);
	    if (!relayIface) {
	        if (!guessMode) {
		    Log(Error) << "Unable to find relay interface with interfaceID=" << ptrIfaceID->getPlain() << " defined on the " 
			       << ptrIface->getName() << "/" << ptrIface->getID() << " interface." << LogEnd;
		    return 0;
		} 
	    } 
	}
	else {
	    // find relay interface based on the link address
	    Log(Cont) << ", interfaceID option missing." << LogEnd;
	    Log(Warning) << "InterfaceID option missing, trying to find proper interface using link address: " 
			 << linkAddr->getPlain() << " (expect troubles)."<< LogEnd;
	    relayIface = ptrIface->getRelayByLinkAddr(linkAddr);
	    if (!relayIface) {
		Log(Error) << "Unable to find relay interface using link address: " << linkAddr->getPlain() << LogEnd;
		if (!guessMode) {
		  return 0;
		}
	    }
	}
	if (!relayIface && guessMode) {
	    relayIface = ptrIface->getAnyRelay();
	    if (!relayIface) {
	        Log(Error) << "Guess-mode: Unable to find any relays on " << ptrIface->getFullName() << LogEnd;
	        return 0;
	    }
	    interfaceIDTbl[relays] = -1;
	    Log(Notice) << "Guess-mode: Relayed interface guessed as " << relayIface->getFullName() << LogEnd;
	}

	// now switch to relay interface
	ptrIface = relayIface;
	buf = relay_buf;
	bufsize = relay_bufsize;
    }
    
    SmartPtr<TSrvMsg> msg = this->decodeMsg(ptrIface, peer, relay_buf, relay_bufsize);
    for (int i=0; i<relays; i++) {
	msg->addRelayInfo(linkAddrTbl[i], peerAddrTbl[i], hopTbl[i], interfaceIDTbl[i], echoListTbl[i]);
    }
    if (remoteID) {
	Log(Debug) << "RemoteID received: vendor=" << remoteID->getVendor() << ", length=" << remoteID->getVendorDataLen() << "." << LogEnd;
	msg->setRemoteID(remoteID);
	remoteID = 0;
	remoteID = msg->getRemoteID();
	PrintHex("RemoteID:", remoteID->getVendorData(), remoteID->getVendorDataLen());
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
    case LEASEQUERY_MSG:
	return new TSrvMsgLeaseQuery(That, SrvCfgMgr, SrvAddrMgr, ifaceid, 
				     peer, buf, bufsize);
    default:
	Log(Warning) << "Illegal message type " << (int)(buf[0]) << " received." << LogEnd;
	return 0; //NULL;;
    }
}

/*
 * remember SmartPtrs to all managers (including this one)
 */
void TSrvIfaceMgr::setContext(SmartPtr<TSrvIfaceMgr> srvIfaceMgr,
			      SmartPtr<TSrvTransMgr> srvTransMgr,
			      SmartPtr<TSrvCfgMgr> srvCfgMgr,
			      SmartPtr<TSrvAddrMgr> srvAddrMgr) {
    SrvCfgMgr=srvCfgMgr;
    SrvAddrMgr=srvAddrMgr;
    SrvTransMgr=srvTransMgr;
    That=srvIfaceMgr;
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
	    Log(Notice) << "Flags on interface " << iface->getFullName() << " has changed (old=" << hex <<iface->getFlags()
			<< ", new=" << ptr->flags << ")." << dec << LogEnd;
	    iface->updateState(ptr);
	}
	ptr = ptr->next;
    }

    if_list_release(ifaceList); // allocated in pure C, and so release it there
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
