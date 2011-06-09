/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.51 2009-03-24 23:17:17 thomson Exp $
 */

#include <sstream>
#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

TClntIfaceMgr * TClntIfaceMgr::Instance = 0;

void TClntIfaceMgr::instanceCreate(const std::string xmlFile)
{
    if (Instance) {
        Log(Crit) << "Application error: Attempt to create another ClntIfaceMgr instance!" << LogEnd;
        return;
    }
    Instance = new TClntIfaceMgr(xmlFile);
}

TClntIfaceMgr& TClntIfaceMgr::instance()
{
    if (!Instance)
		Log(Crit) << "Requested IfaceMgr, but it is not created yet." << LogEnd;
    return *Instance;
}

bool TClntIfaceMgr::sendUnicast(int iface, char *msg, int size, SPtr<TIPv6Addr> addr)
{
    int result;
    // get interface
    SPtr<TIfaceIface> Iface;
    Iface = this->getIfaceByID(iface);
    if (!Iface) {
        Log(Error) << " No such interface (id=" << iface << "). Send failed." << LogEnd;
        return false;
    }

    // are there any sockets on this interface?
    SPtr<TIfaceSocket> sock; 
    if (! Iface->countSocket() ) {
        Log(Error) << "Interface " << Iface->getName() << " has no open sockets." << LogEnd;
        return false;
    }

    // yes, there are. Get first of them (there's usually only one anyway.
    // Additional socket is created for unicast communication
    Iface->firstSocket();
    sock = Iface->getSocket();

    result = sock->send( (char*)msg, size, addr, DHCPSERVER_PORT);
    if (result == -1) {
	Log(Error) << "Send failed: " << size << " bytes to " << *addr 
		   << " on " << Iface->getFullName()
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
    SPtr<TIPv6Addr> multicastAddr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,true);
    
    return this->sendUnicast(iface, msg, msgsize, multicastAddr);
}


SPtr<TClntMsg> TClntIfaceMgr::select(unsigned int timeout)
{
    int bufsize=4096;
    static char buf[4096];
    SPtr<TIPv6Addr> peer(new TIPv6Addr());
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
        SPtr<TClntMsg> ptr;
        SPtr<TIfaceIface> ptrIface;
        ptrIface = this->getIfaceBySocket(sockid);
        ifaceid = ptrIface->getID();
	Log(Debug) << "Received " << bufsize << " bytes on interface " << ptrIface->getName() << "/" 
		   << ptrIface->getID() << " (socket=" << sockid << ", addr=" << *peer << "." 
		   << ")." << LogEnd;
	
        switch (msgtype) {
        case ADVERTISE_MSG:
            ptr = new TClntMsgAdvertise(ifaceid,peer,buf,bufsize);
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr().getAuthAcceptMethods())) {
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
            ptr = new TClntMsgReply(ifaceid, peer, buf, bufsize);
#ifndef MOD_DISABLE_AUTH
            if (!ptr->validateAuthInfo(buf, bufsize, ClntCfgMgr().getAuthAcceptMethods())) {
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
            Log(Warning) << "Message type " << msgtype << " is not supposed to "
			 << "be received by client. Check your relay/server configuration." << LogEnd;
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
	
        SPtr<TIfaceIface> iface = new TClntIfaceIface(ptr->name,ptr->id,
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

TClntIfaceMgr::~TClntIfaceMgr() {
    IfaceLst.clear();
    Log(Debug) << "ClntIfaceMgr cleanup." << LogEnd;
}

void TClntIfaceMgr::removeAllOpts() {
    SPtr<TIfaceIface> iface;
    SPtr<TClntIfaceIface> clntIface;

    this->firstIface();
    while (iface = this->getIface()) {
	clntIface = (Ptr*) iface;
	clntIface->removeAllOpts();
    }
}

unsigned int TClntIfaceMgr::getTimeout() {
    unsigned int min=DHCPV6_INFINITY, tmp;
    SPtr<TIfaceIface> iface;
    SPtr<TClntIfaceIface> clntIface;

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
    SPtr<TClntIfaceIface> iface;
    SPtr<TClntCfgIface> cfgIface;
    
    this->firstIface();
    while (iface = (Ptr*)this->getIface()) {
	      cfgIface = ClntCfgMgr().getIface(iface->getID());
	      if (cfgIface) {
            // Log(Debug) << "FQDN State: " << cfgIface->getFQDNState() << " on " << iface->getFullName() << LogEnd;
            if (cfgIface->getFQDNState() == STATE_INPROCESS) {
		            // Here we check if all parameters are set, and do the DNS update if possible
		            List(TIPv6Addr) DNSSrvLst = iface->getDNSServerLst();
		            string fqdn = iface->getFQDN();
		            if (ClntAddrMgr().countIA() > 0 && DNSSrvLst.count() > 0 && fqdn.size() > 0) {

		                Log(Warning) << "Sleeping 3 seconds before performing DNS Update." << LogEnd;
		                /** @todo: sleep cannot be performed here. What if client has to perform other 
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
    ClntAddrMgr().dump();
    this->dump();
    return true;
}

bool TClntIfaceMgr::fqdnAdd(SPtr<TClntIfaceIface> iface, string fqdn)
{
    SPtr<TIPv6Addr> DNSAddr;
    SPtr<TIPv6Addr> addr;

    SPtr<TClntCfgIface> cfgIface;
    cfgIface = ClntCfgMgr().getIface(iface->getID());
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
    SPtr<TAddrIA> ptrAddrIA;
    ClntAddrMgr().firstIA();
    ptrAddrIA = ClntAddrMgr().getIA();
    
    if (ptrAddrIA->countAddr() > 0) {
        ptrAddrIA->firstAddr();
        addr = ptrAddrIA->getAddr()->get();
	
        Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr 
		    << ", IP=" << *addr << " and FQDN=" << fqdn << LogEnd;
	
	// remember DNS Address (used during address release)
        ptrAddrIA->setFQDNDnsServer(DNSAddr);
	
	unsigned int timeout = ClntCfgMgr().getDDNSTimeout();

#ifndef MOD_CLNT_DISABLE_DNSUPDATE
      	/* add AAAA record */
        DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), "", fqdn, addr->getPlain(), DNSUPDATE_AAAA);
        int result = act->run(timeout);
        act->showResult(result);
        delete act;
#else
        Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
        return false;
#endif
    }
    return true;
}

bool TClntIfaceMgr::fqdnDel(SPtr<TClntIfaceIface> iface, SPtr<TAddrIA> ia, string fqdn)
{
    SPtr<TIPv6Addr> dns = ia->getFQDNDnsServer();
    
    // let's do deleting update
    SPtr<TIPv6Addr> clntAddr;
    ia->firstAddr();
    SPtr<TAddrAddr> tmpAddr = ia->getAddr();
    if (!tmpAddr) {
	Log(Error) << "FQDN: Unable to delete FQDN: IA (IAID=" << ia->getIAID() 
		   << ") does not have any addresses." << LogEnd;
	return false;
    }
    SPtr<TIPv6Addr> myAddr = tmpAddr->get();
    
    SPtr<TClntCfgIface> ptrIface = ClntCfgMgr().getIface(iface->getID());
    
    unsigned int timeout = ClntCfgMgr().getDDNSTimeout();

    Log(Debug) << "FQDN: Cleaning up DNS AAAA record in server " << *dns << ", for IP=" << *myAddr
	       << " and FQDN=" << fqdn << LogEnd;
#ifndef MOD_CLNT_DISABLE_DNSUPDATE
    DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdn, myAddr->getPlain(), DNSUPDATE_AAAA_CLEANUP);
    int result = act->run(timeout);
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
 * @brief configures prefix in the operating system
 *
 * configures specified prefix in the operating system
 * 
 * @param iface interface index
 * @param prefix prefix to be configured
 * @param prefixLen prefix length
 * @param pref prefered lifetime
 * @param valid valid lifetime
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


    if (ClntCfgMgr().getMappingPrefix()) 
    {
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


    // option: split this prefix and add it to all interfaces
    Log(Notice) << "PD: " << action << " prefix " << prefix->getPlain() << "/" << (int)prefixLen 
		<< " to all interfaces (prefix will be split to /" << int(prefixLen+8) << " prefixes if necessary)." << LogEnd;

    if (prefixLen>120) {
        Log(Error) << "PD: Unable to perform prefix operation: prefix /" << prefixLen 
                   << " can't be split. At least /120 prefix is required." << LogEnd;
        return false;
    }

    // get a list of interfaces that we will assign prefixes to
    TIfaceIfaceLst ifaceLst;
    SPtr<TIfaceIface> x;
    firstIface();
    while ( x = (Ptr*)getIface() ) {
	if (x->getID() == ptrIface->getID()) {
	    Log(Debug) << "PD: Interface " << x->getFullName() 
		       << " is the interface, where prefix has been obtained, skipping." << LogEnd;
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

	ifaceLst.push_back(x);
    }

    Log(Info) << "Found " << ifaceLst.size() << " suitable interface(s):";
    TIfaceIfaceLst::const_iterator i;
    for (TIfaceIfaceLst::const_iterator i=ifaceLst.begin(); i!=ifaceLst.end(); ++i) {
        Log(Cont) << (*i)->getName() << " ";
    }
    Log(Cont) << LogEnd;

    if (ifaceLst.size() == 0) {
	Log(Info) << "Suitable interfaces not found. Delegated prefix not split." << LogEnd;
	return false;
    }

    for (TIfaceIfaceLst::const_iterator i=ifaceLst.begin(); i!=ifaceLst.end(); ++i) {

	char buf[16];
	int subprefixLen;
	memmove(buf, prefix->getAddr(), 16);

	if (ifaceLst.size() == 1) {
	    // just one interface - use delegated prefix as is
	    subprefixLen = prefixLen;
	} else if (ifaceLst.size()<256) {
	    subprefixLen = prefixLen + 8; // int( (ceil( log(ifaceList.size()))) );
	    int offset = prefixLen/8;
	    if (prefixLen%8)
		offset++;
	    buf[offset] = x->getID();
	} else {
	    // users with too much time that play with virtual interfaces are out of luck
	    Log(Error) << "Something is wrong. Detected more than 256 interface." << LogEnd;
	    return false;
	}
	
	SPtr<TIPv6Addr> tmpAddr = new TIPv6Addr(buf, false);
	
	Log(Notice) << "PD: " << action << " prefix " << tmpAddr->getPlain() << "/" << subprefixLen 
		    << " on the " << (*i)->getFullName() << " interface." << LogEnd;
	    
	switch (mode) {
	case PREFIX_MODIFY_ADD:
	    status = prefix_add( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen, pref, valid);
	    break;
	case PREFIX_MODIFY_UPDATE:
	    status = prefix_update( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen, pref, valid);
	    break;
	case PREFIX_MODIFY_DEL:
	  status = prefix_del( (*i)->getName(), (*i)->getID(), tmpAddr->getPlain(), subprefixLen);
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

void TClntIfaceMgr::notifyScripts(SPtr<TClntMsg> question, SPtr<TClntMsg> reply)
{
    if (!ClntCfgMgr().getNotifyScripts()) {
	Log(Debug) << "Not executing external script (Notify script disabled)." << LogEnd;
	return;
    }

    PrefixModifyMode mode;
    string action;

    switch (question->getType())
    {
    case REQUEST_MSG:
    case CONFIRM_MSG:
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

    int ifindex = reply->getIface();
    SPtr<TClntIfaceIface> iface = (Ptr*)getIfaceByID(ifindex);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << ifindex << ". Notification NOT sent." << LogEnd;
	return;
    }

    SPtr<TIPv6Addr> ip;
    SPtr<TIPv6Addr> remoteEndpoint = iface->getDsLiteTunnel();
    SPtr<TAddrPrefix> prefix;

    ClntAddrMgr().firstIA();
    SPtr<TAddrIA> ia = ClntAddrMgr().getIA();
    if (ia)
    {
	ia->firstAddr();
	if (ia->countAddr())
	{
	    SPtr<TAddrAddr> addr = ia->getAddr();
	    ip = addr->get();
	}
    }
    
    ClntAddrMgr().firstPD();
    ia = ClntAddrMgr().getPD();
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
	<< remoteEndpoint->getPlain() << " " << action;
    Log(Info) << "About to execute command: " << tmp.str() << LogEnd;

    short int returnCode = system(tmp.str().c_str());

    Log(Info) << "Return code=" << returnCode << LogEnd;
}

#ifdef MOD_REMOTE_AUTOCONF
bool TClntIfaceMgr::notifyRemoteScripts(SPtr<TIPv6Addr> rcvdAddr, SPtr<TIPv6Addr> srvAddr, int ifindex) {

    Log(Info) << "Received address " << rcvdAddr->getPlain() 
	      << " from remote server located at " << srvAddr->getPlain() << LogEnd;

    SPtr<TIfaceIface> iface = getIfaceByID(ifindex);

    stringstream tmp;
    tmp << "./remote-autoconf " << rcvdAddr->getPlain() << " " << srvAddr->getPlain()
	<< " " << iface->getName() << " " << iface->getID();
    
    int returnCode = system(tmp.str().c_str());
    Log(Info) << "Executed command: " << tmp.str() << ", return code=" << returnCode << LogEnd;

    return true;
}
#endif

ostream & operator <<(ostream & strum, TClntIfaceMgr &x) {
    strum << "<ClntIfaceMgr>" << std::endl;
    SPtr<TClntIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr= (Ptr*) x.IfaceLst.get() ) {
	strum << *ptr;
    }
    strum << "</ClntIfaceMgr>" << std::endl;
    return strum;
}
