/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: IfaceMgr.cpp,v 1.6 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
TIfaceMgr::TIfaceMgr()
{
    IsDone = false;
    struct iface * ptr;
    struct iface * ifaceList;
    
    // get interface list
    ifaceList = if_list_get(); // external (C coded) function
    ptr = ifaceList;
    
    if  (!ifaceList) {
	IsDone = true;
	std::clog << logger::logCrit << "Unable to read info interfaces. Make sure "
	    "you are using proper port (i.e. winxp on winxp and not winxp on win2k)"
	    " and you have IPv6 support enabled." << logger::endl;
	return;
    }
    
    while (ptr!=NULL) {
        std::clog << logger::logNotice << "Detected iface " << ptr->name 
		  << "/id=" << ptr->id << ",flags=" << ptr->flags 
		  << " maclen=" << ptr->maclen
		  << logger::endl;
	
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
    if_list_release(ifaceList); // allocated in pure C, and so release is
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
SmartPtr<TIfaceIface> TIfaceMgr::getIfaceByName(char * name) {
    SmartPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( !strcmp(name,ptr->getName()) )
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
 * returns interface ID
 */
int TIfaceMgr::select(unsigned long time, char *buf, 
		      int &bufsize, SmartPtr<TIPv6Addr> peer) {
    struct timeval czas;
    
    int result;
    
    czas.tv_sec=time;
    czas.tv_usec=0;

    // tricks with FDS macros
    fd_set fds;
    fds = *TIfaceSocketIPv6::getFDS();

    result = ::select(FD_SETSIZE,&fds,NULL, NULL, &czas);

    // something received
    if (result>0) {
        SmartPtr<TIfaceIface> iface;
        SmartPtr<TIfaceSocketIPv6> sock;
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

        char plain[48]; // plain address

        // receive data (pure C function used)
        result = sock_recv(sock->getFD(), plain, buf, bufsize);

	// pack data (convert from plain to 16-byte)
        char peerPacked[16];
        inet_pton6(plain,peerPacked);
        peer->setAddr(peerPacked);

        if (result==-1) {
            std::clog << logger::logError << "sock_recv(" << sock->getFD() << ","
                << plain << "...) failed." << logger::endl;
            bufsize = 0;
            return -1;
        }

        bufsize = result;
        return sock->getFD();

    } else { // timeout, nothing received
        bufsize = 0;
        return 0; 
    }
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
