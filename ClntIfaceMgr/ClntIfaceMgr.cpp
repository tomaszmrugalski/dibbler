/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.13 2004-12-07 20:53:40 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.11  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.10  2004/11/02 02:14:20  thomson
 * no message
 *
 * Revision 1.9  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.8  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.7  2004/09/27 22:00:32  thomson
 * Sending problem is now verbose.
 *
 * Revision 1.6  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.5  2004/09/07 15:37:44  thomson
 * Socket handling changes.
 *
 * Revision 1.4  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "Msg.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"
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


SmartPtr<TMsg> TClntIfaceMgr::select(unsigned int timeout)
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
            return SmartPtr<TMsg>(); // NULL
        }
        msgtype = buf[0];
        SmartPtr<TMsg> ptr;
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
            return SmartPtr<TMsg>(); // NULL
        case REPLY_MSG:
            ptr = new TClntMsgReply(That, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                ifaceid, peer, buf, bufsize);
            return ptr;

        case RECONFIGURE_MSG:
            Log(Warning) << "Reconfigure Message is currently not supported." << LogEnd;
            return SmartPtr<TMsg>();
        case RELAY_FORW:
        case RELAY_REPL:
        default:
            Log(Warning) << "Message type " << msgtype << " not supported." << LogEnd;
            return SmartPtr<TMsg>();
        }
    } else {
        return SmartPtr<TMsg>();
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
        Log(Notice) << "Detected clnt iface " << ptr->name << "/" << ptr->id << ", flags=" 
		    << ptr->flags << ", maclen=" << ptr->maclen << LogEnd;
	
        SmartPtr<TIfaceIface> iface(new TClntIfaceIface(ptr->name,ptr->id,
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
