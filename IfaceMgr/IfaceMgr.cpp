/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: IfaceMgr.cpp,v 1.16 2004-12-04 23:45:40 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2004/11/01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.14  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.13  2004/09/28 20:33:57  thomson
 * Address check in WIN32 should be disabled.
 *
 * Revision 1.12  2004/09/07 15:37:44  thomson
 * Socket handling changes.
 *
 * Revision 1.11  2004/09/05 15:37:44  thomson
 * Linux server now properly supports unicast.
 *
 * Revision 1.10  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.9  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.8  2004/05/23 23:45:19  thomson
 * getInterfaceByName bug fixed.
 *
 * Revision 1.7  2004/05/23 20:27:23  thomson
 * *** empty log message ***
 *
 * Revision 1.6  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include <iostream>
#include <string>
#include "Portable.h"
#include "IfaceMgr.h"
#include "Iface.h"
#include "SocketIPv6.h"

#include "Logger.h"

/*
 * InterfaceManager - provides access to all network interfaces present in the system
 * @date 2003-10-26
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

/*
 * creates list of interfaces
 */
TIfaceMgr::TIfaceMgr(bool getIfaces)
{
    IsDone = false;
    struct iface * ptr;
    struct iface * ifaceList;

    if (!getIfaces)
	return;

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
		    << ptr->flags << ", maclen=" << ptr->maclen << LogEnd;
	
        SmartPtr<TIfaceIface> smartptr(new TIfaceIface(ptr->name,ptr->id,
						       ptr->flags,
						       ptr->mac,
						       ptr->maclen,
						       ptr->linkaddr,
						       ptr->linkaddrcount,
						       ptr->hardwareType));
        this->IfaceLst.append(smartptr);
        ptr = ptr->next;
    }
    if_list_release(ifaceList); // allocated in pure C, and so release it there
}

/*
 * returns true if IfaceMgr has finished it's job (failed to get info about interfaces)
 */
bool TIfaceMgr::isDone() {
    return IsDone;
}

/*
 * rewinds interface list to first interface
 */
void TIfaceMgr::firstIface() {
    IfaceLst.first();
}

/*
 * gets next interface on list
 */
SmartPtr<TIfaceIface> TIfaceMgr::getIface() {
    return IfaceLst.get();
}


/*
 * gets interface by it's name (or NULL if no such inteface exists)
 * @param name - interface name
 */
SmartPtr<TIfaceIface> TIfaceMgr::getIfaceByName(string name) {
    SmartPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( !strcmp(name.c_str(),ptr->getName()) )
	    return ptr;
    }
    return 0; // NULL
}

/*
 * gets interface by it ID (or NULL if no such interface exists)
 * @param id - interface id
 */ 
SmartPtr<TIfaceIface> TIfaceMgr::getIfaceByID(int id) {
    SmartPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( id == ptr->getID() )
	    return ptr;
    }
    return 0; //NULL
}

/*
 * gets interface by socket descriptor (or NULL if no such interface exists)
 */
SmartPtr<TIfaceIface> TIfaceMgr::getIfaceBySocket(int fd) {
    SmartPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( ptr->getSocketByFD(fd) )
	    return ptr;
    }
    return 0;
}

/*
 * tries to read data from any socket on all interfaces 
 * returns after time seconds.
 * @param time - listens for time seconds
 * @param buf - buffer
 * @param bufsize - buffer size
 * @param peer - informations about sender
 * returns socketID
 */
int TIfaceMgr::select(unsigned long time, char *buf, 
		      int &bufsize, SmartPtr<TIPv6Addr> peer) {
    struct timeval czas;
    int result;
    if (time > DHCPV6_INFINITY/2)
	time /=2;
    
    czas.tv_sec=time;
    czas.tv_usec=0;

    // tricks with FDS macros
    fd_set fds;
    fds = *TIfaceSocket::getFDS();

    result = ::select(FD_SETSIZE,&fds,NULL, NULL, &czas);

    // something received

    if (result<=0) { // timeout, nothing received
        bufsize = 0;
        return 0; 
    }

    SmartPtr<TIfaceIface> iface;
    SmartPtr<TIfaceSocket> sock;
    bool found = 0;
    IfaceLst.first();
    while ( (!found) && (iface = IfaceLst.get()) ) {
	iface->firstSocket();
	while ( sock = iface->getSocket() ) {
	    if (FD_ISSET(sock->getFD(),&fds)) {
		found = true;
		break;
	    }	
	}
    }
    
    char myPlainAddr[48];   // my plain address
    char peerPlainAddr[48]; // peer plain address

    // receive data (pure C function used)
    result = sock_recv(sock->getFD(), myPlainAddr, peerPlainAddr, buf, bufsize);
    char peerAddrPacked[16];
    char myAddrPacked[16];
    inet_pton6(peerPlainAddr,peerAddrPacked);
    inet_pton6(myPlainAddr,myAddrPacked);
    peer->setAddr(peerAddrPacked);

    if (result==-1) {
	Log(Error) << "sock_recv(" << sock->getFD() << "," << "...) failed." << LogEnd;
	bufsize = 0;
	return -1;
    }

#ifndef WIN32
    // check if we've received data addressed to us. There's problem with sockets binding. 
    // If there are 2 open sockets (one bound to multicast and one to global address),
    // each packet sent on multicast address is also received on unicast socket.
    if (!iface->flagLoopback() && memcmp(sock->getAddr()->getAddr(), myAddrPacked, 16)) {
	Log(Debug) << "Received data on address " << myPlainAddr << ", expected " 
		   << *sock->getAddr() << ", message ignored." << LogEnd;
	bufsize = 0;
	return 0;
    }  
#endif

    bufsize = result;
    return sock->getFD();
}

/*
 * returns interface count
 */
int TIfaceMgr::countIface() {
	return IfaceLst.count();
}

/*
 * dump yourself to file
 */
void TIfaceMgr::dump(char * file)
{
    std::ofstream xmlDump;
    xmlDump.open(file);
    xmlDump << *this;
    xmlDump.close();
}

/*
 * destructor. Does really nothing. (SmartPtr is a sweet thing, isn't it?)
 */
TIfaceMgr::~TIfaceMgr()
{
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just flush this data in XML format (used for logging and debugging purposes only)
 */
ostream & operator <<(ostream & strum, TIfaceMgr &x)
{
    strum << "<IfaceMgr>" << endl;
    SmartPtr<TIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr=x.IfaceLst.get() ) {
	strum << *ptr;
    }
    
    strum << "</IfaceMgr>" << endl;
    return strum;
}
