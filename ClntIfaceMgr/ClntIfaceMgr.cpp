/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.31 2007-01-07 20:18:44 thomson Exp $
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
            if (!ptr->validateAuthInfo(buf, bufsize))
                    return 0;
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
            if (!ptr->validateAuthInfo(buf, bufsize))
                    return 0;
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
	
        SmartPtr<TIfaceIface> iface = new TClntIfaceIface(ptr->name,ptr->id,
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

    /* that is an ugly workaround for SmartPtr() deficiency. IfaceLst contains pointers
       to the TIfaceIface objects. When TClntIfaceIface object is stored, and then deleted
       it is being destroyed in a wrong way, i.e. treated as IfaceIface objects, so only
       ~TIfaceIface() will be called, and not ~TClntIfaceIface() and then ~TIfaceIface(). */
    SPtr<TClntIfaceIface> ptr;
    IfaceLst.first();
    while (ptr = (Ptr*)IfaceLst.get()) {
	IfaceLst.del();
	IfaceLst.first();
	// last SmartPtr() pointing to an object is of proper type
    }
    this->IfaceLst.clear();
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
	    if (cfgIface->getFQDNState() == STATE_INPROCESS) {
		// Here we check if all parameters are set, and do the DNS update if possible
		List(TIPv6Addr) DNSSrvLst = iface->getDNSServerLst();
		string fqdn = iface->getFQDN();
		if (ClntAddrMgr->countIA() > 0 && DNSSrvLst.count() > 0 && fqdn.size() > 0) {

		    Log(Warning) << "FIXME: Sleeping 3 seconds before performing DNS Update." << LogEnd;
		    /* FIXME: sleep cannot be performed here. What if client has to perform other 
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
    ClntAddrMgr->dump();
    this->dump();
    return true;
}

bool TClntIfaceMgr::fqdnAdd(SmartPtr<TClntIfaceIface> iface, string fqdn)
{
    SmartPtr<TIPv6Addr> DNSAddr;
    SmartPtr<TIPv6Addr> addr;

    SmartPtr<TClntCfgIface> cfgIface;
    cfgIface = ClntCfgMgr->getIface(iface->getID());
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
    SmartPtr<TAddrIA> ptrAddrIA;
    ClntAddrMgr->firstIA();
    ptrAddrIA = ClntAddrMgr->getIA();
    
    if (ptrAddrIA->countAddr() > 0) {
	ptrAddrIA->firstAddr();
	addr = ptrAddrIA->getAddr()->get();
	
	Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr << ", IP=" << *addr 
		    << " and FQDN=" << fqdn << LogEnd;
	
			// remember DNS Address (used during address release)
	ptrAddrIA->setFQDNDnsServer(DNSAddr);
	
#ifndef MOD_CLNT_DISABLE_DNSUPDATE
	/* add AAAA record */
	DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), "", fqdn, addr->getPlain(), DNSUPDATE_AAAA);
	int result = act->run();
	act->showResult(result);
	delete act;

#else
	Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
	return false;
#endif
    }
    return true;
}

bool TClntIfaceMgr::fqdnDel(SmartPtr<TClntIfaceIface> iface, SmartPtr<TAddrIA> ia, string fqdn)
{
    SPtr<TIPv6Addr> dns = ia->getFQDNDnsServer();
    
    // let's do deleting update
    SmartPtr<TIPv6Addr> clntAddr;
    ia->firstAddr();
    SPtr<TAddrAddr> tmpAddr = ia->getAddr();
    if (!tmpAddr) {
	Log(Error) << "FQDN: Unable to delete FQDN: IA (IAID=" << ia->getIAID() << ") does not have any addresses." 
		   << LogEnd;
	return false;
    }
    SPtr<TIPv6Addr> myAddr = tmpAddr->get();
    
    SmartPtr<TClntCfgIface> ptrIface = ClntCfgMgr->getIface(iface->getID());
    
    Log(Debug) << "FQDN: Cleaning up DNS AAAA record in server " << *dns << ", for IP=" << *myAddr
	       << " and FQDN=" << fqdn << LogEnd;
#ifndef MOD_CLNT_DISABLE_DNSUPDATE
    DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdn, myAddr->getPlain(), DNSUPDATE_AAAA_CLEANUP);
    int result = act->run();
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
 * configures prefix in the operating system
 * 
 * @param iface 
 * @param prefix 
 * @param prefixLen 
 * @param pref 
 * @param valid 
 * 
 * @return true if operation was successful, false otherwise
 */
bool TClntIfaceMgr::addPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid)
{
    SPtr<TClntIfaceIface> ptrIface = (Ptr*)getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Unable to find interface with ifindex=" << iface << ", prefix add operation failed." << LogEnd;
	return false;
    }

    // FIXME: Implement split between interfaces if forwarding (i.e. this computer is a router) is enabled

    Log(Notice) << "Prefix " << prefix->getPlain() << "/" << (int)prefixLen << " added on the "
		<< ptrIface->getFullName() << " interface." << LogEnd;
    int status = prefix_add(ptrIface->getName(), iface, prefix->getPlain(), prefixLen, pref, valid);
    if (status<0) {
	string tmp = error_message();
	Log(Error) << "Prefix error encountered during add operation: " << tmp << LogEnd;
	return false;
    }

    return true;
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
    SPtr<TClntIfaceIface> ptrIface = (Ptr*)getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Unable to find interface with ifindex=" << iface << ", prefix add operation failed." << LogEnd;
	return false;
    }

    // FIXME: Implement split between interfaces if forwarding (i.e. this computer is a router) is enabled

    Log(Notice) << "Prefix " << prefix->getPlain() << "/" << (int)prefixLen << " removed from the "
		<< ptrIface->getFullName() << " interface." << LogEnd;
    int status = prefix_del(ptrIface->getName(), iface, prefix->getPlain(), prefixLen);
    if (status<0) {
	string tmp = error_message();
	Log(Error) << "Prefix error encountered during add operation: " << tmp << LogEnd;
	return false;
    }

    return true;
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
