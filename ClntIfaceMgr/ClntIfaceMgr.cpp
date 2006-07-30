/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.24 2006-07-30 22:34:09 thomson Exp $
 */

#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "Msg.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"
#include "DNSUpdate.h"

using namespace logger;
using namespace std;

bool TClntIfaceMgr::sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr)
{
    int result;
    // get interface
    SmartPtr<TIfaceIface> Iface;
    Iface = this->getIfaceByID(iface);
    if (!Iface) {
        Log(Error) << " No such interface (id=" << iface << "). Send failed." << LogEnd;
        return false;
    }

    // are there any sockets on this interface?
    SmartPtr<TIfaceSocket> sock; 
    if (! Iface->countSocket() ) {
        Log(Error) << "Interface " << Iface->getName() << " has no open sockets." << LogEnd;
        return false;
    }

    // yes, there are. Get first of them.
    Iface->firstSocket();
    sock = Iface->getSocket();

    result = sock->send( (char*)msg, size, addr, DHCPSERVER_PORT);
    if (result == -1) {
	Log(Error) << "Send failed: " << size << " bytes to " << *addr 
		   << " on " << Iface->getName() << "/" << Iface->getID() 
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
    SmartPtr<TIPv6Addr> multicastAddr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,true);
    
    return this->sendUnicast(iface, msg, msgsize, multicastAddr);
}


SmartPtr<TClntMsg> TClntIfaceMgr::select(unsigned int timeout)
{
    int bufsize=4096;
    static char buf[4096];
    SmartPtr<TIPv6Addr> peer(new TIPv6Addr());
    int sockid;
    int msgtype;
    int ifaceid;

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
        msgtype = buf[0];
        SmartPtr<TClntMsg> ptr;
        SmartPtr<TIfaceIface> ptrIface;
        ptrIface = this->getIfaceBySocket(sockid);
        ifaceid = ptrIface->getID();
	Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getName() << "/" 
		   << ptrIface->getID() << " (socket=" << sockid << ", addr=" << *peer << "." 
		   << ")." << LogEnd;
	
        switch (msgtype) {
        case ADVERTISE_MSG:
            ptr = new TClntMsgAdvertise(That, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                ifaceid,peer,buf,bufsize);
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
            ptr = new TClntMsgReply(That, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                ifaceid, peer, buf, bufsize);
            return ptr;

        case RECONFIGURE_MSG:
            Log(Warning) << "Reconfigure Message is currently not supported." << LogEnd;
            return 0; // NULL
        case RELAY_FORW_MSG: // those two msgs should not be visible for client
        case RELAY_REPL_MSG:
        default:
            Log(Warning) << "Message type " << msgtype << " must not be recevied by client. Check your relay/server configuration." << LogEnd;
            return 0;
        }
    } else {
        return 0;
    }
}

TClntIfaceMgr::TClntIfaceMgr(string xmlFile)
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
	
        SmartPtr<TIfaceIface> iface(new TClntIfaceIface(ptr->name,ptr->id,
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

void TClntIfaceMgr::setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
                             SmartPtr<TClntTransMgr> clntTransMgr,
                             SmartPtr<TClntCfgMgr> clntCfgMgr,
                             SmartPtr<TClntAddrMgr> clntAddrMgr)
{
    ClntCfgMgr=clntCfgMgr;
    ClntAddrMgr=clntAddrMgr;
    ClntTransMgr=clntTransMgr;
    That=clntIfaceMgr;
}

TClntIfaceMgr::~TClntIfaceMgr() {
    Log(Debug) << "ClntIfaceMgr cleanup." << LogEnd;
}

void TClntIfaceMgr::removeAllOpts() {
    SmartPtr<TIfaceIface> iface;
    SmartPtr<TClntIfaceIface> clntIface;

    this->firstIface();
    while (iface = this->getIface()) {
	clntIface = (Ptr*) iface;
	clntIface->removeAllOpts();
    }
}

unsigned int TClntIfaceMgr::getTimeout() {
    unsigned int min=DHCPV6_INFINITY, tmp;
    SmartPtr<TIfaceIface> iface;
    SmartPtr<TClntIfaceIface> clntIface;

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
    SmartPtr<TClntIfaceIface> iface;
    SmartPtr<TClntCfgIface> cfgIface;
    
    this->firstIface();
    while (iface = (Ptr*)this->getIface()) {
	cfgIface = ClntCfgMgr->getIface(iface->getID());
	if (cfgIface) {
	    // Log(Debug) << "FQDN State: " << cfgIface->getFQDNState() << " on " << iface->getFullName() << LogEnd;
	    if (cfgIface->getFQDNState() == INPROCESS) {
		// Here we check if all parameters are set, and do the DNS update if possible
		List(TIPv6Addr) DNSSrvLst = iface->getDNSServerLst();
		string fqdn = iface->getFQDN();
		if (ClntAddrMgr->countIA() > 0 && DNSSrvLst.count() > 0 && fqdn.size() > 0) {
		    Log(Debug) << "All parameters seem to be set to perform DNS update. Selecting first DNS server" << LogEnd;
		    SmartPtr<TIPv6Addr> DNSAddr;
		    SmartPtr<TIPv6Addr> addr;
		    
                    // For the moment, we just take the first DNS entry.
		    DNSSrvLst.first();
		    DNSAddr = DNSSrvLst.get();
		    
// 					And the first IP address
		    SmartPtr<TAddrIA> ptrAddrIA;
		    ClntAddrMgr->firstIA();
		    ptrAddrIA = ClntAddrMgr->getIA();
		    
		    if (ptrAddrIA->countAddr() > 0) {
			ptrAddrIA->firstAddr();
			addr = ptrAddrIA->getAddr()->get();
			
			Log(Notice) << "About to perform DNS Update: DNS server=" << *DNSAddr << ", IP (" << *addr 
				    << ") and FQDN=" << fqdn << LogEnd;
			Log(Warning) << "FIXME: Sleeping 3 seconds before FQDN update" << LogEnd;
			/* FIXME: sleep cannot be performed here. What if client has to perform other 
			   action during those 3 seconds? */
#ifdef WIN32
			Sleep(3);
#else
			sleep(3);
#endif
			Log(Warning) << "FIXME: Waking up !" << LogEnd;
			
			//Test for DNS update
			unsigned int dotpos = fqdn.find(".");
			string hostname = "";
			string domain = "";
			if (dotpos == string::npos) {
			    Log(Warning) << "Name provided for DNS update is not a FQDN. [" << fqdn 
					 << "] Trying to do the update..." << LogEnd;
			    hostname = fqdn;
			} else {
			    hostname = fqdn.substr(0, dotpos);
			    domain = fqdn.substr(dotpos + 1, fqdn.length() - dotpos - 1);
			}
#ifndef MOD_CLNT_DISABLE_DNSUPDATE
			DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), (char*) domain.c_str(), 
						       (char*) hostname.c_str(), addr->getPlain(), "2h",3);
			int result = act->run();
			delete act;
			
			if (result == DNSUPDATE_SUCCESS) {
			    cfgIface->setFQDNState(CONFIGURED);
			    Log(Notice) << "FQDN Configured successfully !" << LogEnd;
			} else {
			    Log(Warning) << "Unable to perform DNS update. Disabling FQDN on " 
					 << iface->getFullName() << LogEnd;
			    cfgIface->setFQDNState(DISABLED);
			}
#else
			Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
#endif
		    }
		}
	    }
	}
    }
    return true;
}

void TClntIfaceMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open( this->XmlFile.c_str() );
    xmlDump << *this;
    xmlDump.close();
}

ostream & operator <<(ostream & strum, TClntIfaceMgr &x) {
    strum << "<ClntIfaceMgr>" << std::endl;
    SmartPtr<TClntIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
	strum << *ptr;
    }
    strum << "</ClntIfaceMgr>" << std::endl;
    return strum;
}
