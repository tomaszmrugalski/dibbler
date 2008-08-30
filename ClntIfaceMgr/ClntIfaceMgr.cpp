/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.50 2008-08-30 21:41:12 thomson Exp $
 */

#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "ClntAddrMgr.h"
#include "Msg.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

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
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr->getAuthAcceptMethods())) {
                    Log(Error) << "Message dropped, authentication validation failed." << LogEnd;
                    return 0;
            }
#endif
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
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr->getAuthAcceptMethods())) {
                    Log(Error) << "Message dropped, authentication validation failed." << LogEnd;
                    return 0;
            }
#endif
            return ptr;

        case RECONFIGURE_MSG:
            Log(Warning) << "Reconfigure Message is currently not supported." << LogEnd;
            return 0; // NULL
        case RELAY_FORW_MSG: // those two msgs should not be visible for client
        case RELAY_REPL_MSG:
        default:
            Log(Warning) << "Message type " << msgtype << " is not supposed to be received by client. Check your relay/server configuration." << LogEnd;
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

void TClntIfaceMgr::setContext(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
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
    IfaceLst.clear();
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
    return modifyPrefix(iface, prefix, prefixLen, pref, valid, PREFIX_MODIFY_ADD);
}

bool TClntIfaceMgr::updatePrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid)
{
    return modifyPrefix(iface, prefix, prefixLen, pref, valid, PREFIX_MODIFY_UPDATE);
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
    return modifyPrefix(iface, prefix, prefixLen, 0, 0, PREFIX_MODIFY_DEL);
}

bool TClntIfaceMgr::modifyPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid,
    PrefixModifyMode mode)
{
    SPtr<TClntIfaceIface> ptrIface = (Ptr*)getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex=" << iface << ", prefix add operation failed." << LogEnd;
        return false;
    }


    if (ClntCfgMgr->getMappingPrefix()) {
	char buf[128];
	char cmd1[]="./mappingprefixadd";
	char cmd2[]="./mappingprefixdel";
	int returnCode = 0;
	switch (mode) {
	case PREFIX_MODIFY_ADD:
	{
	    sprintf(buf, "sh %s %s", cmd1, prefix->getPlain());
	    Log(Notice) << "Executing external command to ADD prefix: " << buf << LogEnd;
	    returnCode = system(buf);
	    Log(Notice) << "ReturnCode = " << returnCode << LogEnd;
	    break;
	}
	case PREFIX_MODIFY_DEL:
	{
	    sprintf(buf, "sh %s %s", cmd2, prefix->getPlain());
	    Log(Notice) << "Executing external command to DEL prefix: " << buf << LogEnd;

	    returnCode = system(buf);
	    Log(Notice) << "ReturnCode = " << returnCode << LogEnd;
	    break;
	}
	default:
	{

	}
	}
	
	return true; // added successfully
    }


    string action;
    int status = -1;

    switch (mode) {
    case PREFIX_MODIFY_ADD:
	    action = "Adding";
	    break;
    case PREFIX_MODIFY_UPDATE:
	    action = "Updating";
	    break;
    case PREFIX_MODIFY_DEL:
	    action = "Deleting";
	    break;
    }

    if (!prefix_forwarding_enabled()) {
        // option 1: add/update/delete this prefix to this interface only
        Log(Debug) << "PD: IPv6 forwarding disabled, so prefix operation will apply to this (" << ptrIface->getFullName() 
                   << ") interface only." << LogEnd;

	    Log(Notice) << "PD: " << action << " prefix " << prefix->getPlain() << "/" << (int)prefixLen << " on the "
		            << ptrIface->getFullName() << " interface." << LogEnd;
	    switch (mode) {
	    case PREFIX_MODIFY_ADD:
	        status = prefix_add(ptrIface->getName(), iface, prefix->getPlain(), prefixLen, pref, valid);
	        break;
	    case PREFIX_MODIFY_UPDATE:
	        status = prefix_update(ptrIface->getName(), iface, prefix->getPlain(), prefixLen, pref, valid);
	        break;
	    case PREFIX_MODIFY_DEL:
    	    status = prefix_del(ptrIface->getName(), iface, prefix->getPlain(), prefixLen);
	        break;
        }

        if (status!=LOWLEVEL_NO_ERROR) {
            string tmp = error_message();
            Log(Error) << "Prefix error encountered during " << action << " operation: " << tmp << LogEnd;
            return false;
        }
        return true;
    }

    // option 2: split this prefix and add it to all interfaces
    Log(Notice) << "PD: IPv6 forwarding enabled, so prefix operation will apply to all interfaces." << LogEnd;
    Log(Notice) << "PD: " << action << " prefix " << prefix->getPlain() << "/" << (int)prefixLen 
		<< " to all interfaces (prefix will be split to /" << int(prefixLen+8) << " prefixes)." << LogEnd;

    if (prefixLen>120) {
        Log(Error) << "PD: Unable to perform prefix operation: prefix /" << prefixLen 
                   << " can't be split. At least /120 prefix is required." << LogEnd;
        return false;
    }

    char buf[16];
    memmove(buf, prefix->getAddr(), 16);
    int offset = prefixLen/8;
    if (prefixLen%8)
	offset++;

    SPtr<TClntIfaceIface> x;
    firstIface();
    while ( x = (Ptr*)getIface() ) {
	if (x->getID() == ptrIface->getID()) {
	    Log(Debug) << "PD: Interface " << x->getFullName() << " is the interface, where prefix has been obtained, skipping." 
		       << LogEnd;
	    continue;
	}
	
	// for each interface present in the system...
	if (!x->flagUp()) {
	    Log(Debug) << "PD: Interface " << x->getFullName() << " is down, ignoring." << LogEnd;
	    continue;
	}
	if (!x->flagRunning()) {
	    Log(Debug) << "PD: Interface " << x->getFullName()
		       << " has flag RUNNING not set, ignoring." << LogEnd;
	    continue;
	}
	if (!x->flagMulticast()) {
	    Log(Debug) << "PD: Interface " << x->getFullName()
		       << " is not multicast capable, ignoring." << LogEnd;
	    continue;
	}
	if ( !(x->getMacLen() > 5) ) {
	    Log(Debug) << "PD: Interface " << x->getFullName() 
		       << " has MAC address length " << x->getMacLen() 
		       << " (6 or more required), ignoring." << LogEnd;
		continue;
	}
	x->firstLLAddress();
	if (!x->getLLAddress()) {
	    Log(Debug) << "PD: Interface " << x->getFullName()
		       << " has no link-local address, ignoring. (Disconnected? Not associated? No-link?)" << LogEnd;
	    continue;
	}


	buf[offset] = x->getID();
	SPtr<TIPv6Addr> tmpAddr = new TIPv6Addr(buf, false);

	Log(Notice) << "PD: " << action << " prefix " << prefix->getPlain() << "/" << int(prefixLen) << " on the "
		    << x->getFullName() << " interface." << LogEnd;
	    
	switch (mode) {
	case PREFIX_MODIFY_ADD:
	    status = prefix_add(x->getName(), x->getID(), prefix->getPlain(), prefixLen, pref, valid);
		break;
	case PREFIX_MODIFY_UPDATE:
	    status = prefix_update(x->getName(), x->getID(), prefix->getPlain(), prefixLen, pref, valid);
	    break;
	case PREFIX_MODIFY_DEL:
	    status = prefix_del(x->getName(), x->getID(), prefix->getPlain(), prefixLen);
	    break;
	}
	if (status!=LOWLEVEL_NO_ERROR) {
	    string tmp = error_message();
	    Log(Error) << "Prefix error encountered during " << action << " operation: " << tmp << LogEnd;
	    return false;
	}
    }
    return true;
}

void TClntIfaceMgr::redetectIfaces() {
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

void TClntIfaceMgr::notifyScripts(int msgType, int ifindex)
{
    if (!ClntCfgMgr->getNotifyScripts()) {
	Log(Debug) << "Not executing external script (Notify script disabled)." << LogEnd;
	return;
    }

    PrefixModifyMode mode;
    string action;

    switch (msgType)
    {
    case REQUEST_MSG:
	mode = PREFIX_MODIFY_ADD;
	action = "add";
	break;
    case RELEASE_MSG:
	mode = PREFIX_MODIFY_DEL;
	action = "delete";
	break;
    case RENEW_MSG:
    default:
	mode = PREFIX_MODIFY_UPDATE;
	action = "update";
    }

    SPtr<TClntIfaceIface> iface = (Ptr*)getIfaceByID(ifindex);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << ifindex << ". Notification NOT sent." << LogEnd;
	return;
    }

    int tunnelMode = iface->getTunnelMode();
    SPtr<TIPv6Addr> ip;
    SPtr<TIPv6Addr> remoteEndpoint;
    SPtr<TAddrPrefix> prefix;

    if (tunnelMode!=0)
    {
	remoteEndpoint = iface->getTunnelEndpoint();
    }

    ClntAddrMgr->firstIA();
    SPtr<TAddrIA> ia = ClntAddrMgr->getIA();
    if (!ia)
    {
	Log(Error) << "Unable to find any IAs defined in the AddrMgr." << LogEnd;
	return;
    }

    ia->firstAddr();
    if (ia->countAddr())
    {
	SPtr<TAddrAddr> addr = ia->getAddr();
	ip = addr->get();
    }

    ClntAddrMgr->firstPD();
    ia = ClntAddrMgr->getPD();
    if (ia)
    {
	ia->firstPrefix();
	prefix = ia->getPrefix();
    }

    if (!ip)
	ip = new TIPv6Addr("::", true);

    if (!remoteEndpoint)
	remoteEndpoint = new TIPv6Addr("::", true);
    
    if (!prefix) 
	prefix = new TAddrPrefix(new TIPv6Addr("::", true), 0, 0, 0);

    stringstream tmp;
    tmp << "sh ./notify " << " " << ip->getPlain() << " "
	<< prefix->get()->getPlain() << " " << prefix->getLength() << " "
	<< remoteEndpoint->getPlain() << " " << tunnelMode << " " << action;
    Log(Info) << "About to execute command: " << tmp.str() << LogEnd;

    int returnCode = system(tmp.str().c_str());

    Log(Info) << "Return code=" << returnCode << LogEnd;
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
