/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SocketIPv6.cpp,v 1.5 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */


#include <string>
#include "SocketIPv6.h"
#include "Portable.h"
#include "DHCPConst.h"
#include "Logger.h"

/*
 * IfaceSocketIPv6 - represents network socket 
 * @date 2003-10-26
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

/* 
 * static elements of TIfaceSocketIPv6 class
 */ 
int TIfaceSocketIPv6::Count=0;

/*
 * creates socket bound to specific address on this interface
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param addr - address 
 * @param ifaceonly - force interface-only flag in setsockopt()
 */
TIfaceSocketIPv6::TIfaceSocketIPv6(char * iface, int ifaceid, int port,
				   SmartPtr<TIPv6Addr> addr, bool ifaceonly) { 
    if (this->Count==0) {
	// first socket created, have to run some weird POSIX FD_ZERO macro.
	std::clog << logger::logDebug << "First socket created, zeroing FDS" << logger::endl;
	FD_ZERO(getFDS());
    }
    this->Count++;
    this->createSocket(iface, ifaceid, addr, port, ifaceonly);
}

/*
 * creates socket bound to this interface
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param ifaceonly - force interface-only flag in setsockopt()
 */
TIfaceSocketIPv6::TIfaceSocketIPv6(char * iface,int ifaceid, int port,bool ifaceonly) {
    if (this->Count==0) {
	std::clog << logger::logDebug << "First socket created, zeroing FDS" << logger::endl;
	FD_ZERO(getFDS());
    }

    // bind it to any address (::)
    char anyaddr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    SmartPtr<TIPv6Addr> smartAny (new TIPv6Addr(anyaddr));   
    this->createSocket(iface, ifaceid, smartAny, port, ifaceonly);
    this->Count++;
}

/*
 * creates socket on this interface.
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param addr - address 
 * @param ifaceonly - force interface-only flag in setsockopt()
 * returns error code (or 0 if everything is ok)
 */
int TIfaceSocketIPv6::createSocket(char * iface, int ifaceid, SmartPtr<TIPv6Addr> addr, 
				   int port, bool ifaceonly) {
    int sock;

    // store info about this socket 
    strncpy(this->Iface,iface,MAX_IFNAME_LENGTH);
    this->IfaceID = ifaceid;
    this->Port = port;
    this->IfaceOnly = ifaceonly;
    this->Status = UNKNOWN;

	memcpy(this->Plain,addr->getPlain(),48);

    // is this address multicast? So the socket is.
    if ((addr->getAddr())[0]==(char)0xff) 
        this->Multicast = true;
    else
        this->Multicast = false;

    // create socket
    sock = sock_add(this->Iface, this->IfaceID, addr->getPlain(), 
		    this->Port, (int)this->IfaceOnly);

    // detect errors
    switch (sock) {
    case -7:
        std::clog << logger::logError 
		  << "getaddrinfo() failed. Is IPv6 protocol supported by kernel?" 
		  << logger::endl;
        break;
    case -1:
        std::clog << logger::logError 
		  << "Unable to create socket. Is IPv6 protocol supported by kernel?" 
		  << logger::endl;
        break;
    case -2:
        std::clog << logger::logError 
		  << "Unable to bind socket to interface " << this->Iface << logger::endl;
        break;
    case -8:
        std::clog << logger::logError 
		  << "Unable to set up socket option IPV6_PKTINFO" << logger::endl;
        break;
    case -9:
        std::clog << logger::logError 
		  << "Unable to set up socket option SO_REUSEADDR" << logger::endl;
        break;
    case -3: // this error no longer could occur in Linux version
        std::clog << logger::logError 
		  << "Unable to create a network address structure (addr=" 
		  << addr->getPlain() << ")" << logger::endl;
        break;
    case -4:
        std::clog << logger::logError << "Unable to bind socket (iface=" << ifaceid  
		  << ", addr=" << addr->getPlain() << ", port=" 
		  << this->Port << ")" << logger::endl;
        break;
    case -5:
        std::clog << logger::logError 
		  << "Unable to getaddrinfo() of multicast group. (iface= " << ifaceid  
		  << ", addr=" << this->Plain << ", port=" 
		  << this->Port << ")" << logger::endl;
        break;
    case -6:
        std::clog << logger::logError 
		  << "Unable to join multicast group." << logger::endl;
    default:
        break;
    }

    // set status
    if (sock < 0) {
        this->Status = FAILED;
        return -3;
    }

    this->FD = sock;
    this->Status = CONFIGURED;

    // add FileDescriptior fd_set using FD_SET macro
    FD_SET(this->FD,this->getFDS());
    return 0;
}

/*
 * sends data through socket
 * @param buf - buffer to send
 * @param len - number of bytes to send
 * @param addr - where send this data
 * @param port - to which port
 * returns number of bytes sent or -1 if something went wrong
 */
int TIfaceSocketIPv6::send(char * buf,int len,SmartPtr<TIPv6Addr> addr,int port) {

    int result;
    
    //extern "C" int sock_send(int fd, char * addr, char * buf, int buflen, int port, int ifaceID);
    result = sock_send(this->FD, addr->getPlain(), buf, len, port, this->IfaceID);
    
    switch(result) {
    case -2:
	clog << logger::logError << "Unable to create a network address structure (addr=" 
	     << *addr << ")" << logger::endl;
	return -1;
    }
    return result;
}

/*
 * receives data from socket
 * @param buf - received data are stored here
 * @param addr - will contain info about sender
 */
int TIfaceSocketIPv6::recv(char * buf, SmartPtr<TIPv6Addr> addr) {
    char peeraddr[48];

    // maximum DHCPv6 packet size
    int len=1500;

    len = sock_recv(this->FD, peeraddr, buf, len);

    if ( len  < 0 ) {
        clog << logger::logError << "Error during recvfrom socket " << this->FD << logger::endl;
        return -1;
    }

    // convert to packed form (plain->16-byte)
    char packedAddr[16];
    inet_pton6(peeraddr,packedAddr);
    addr->setAddr(packedAddr);
    return len;
}

/*
 * returns FDS - FileDescriptorSet 
 * it's some really weird POSIX macro. It uses FD_SET, FD_ZERO and FD_CLR macros
 * defined somewhere in system headers
 */
fd_set * TIfaceSocketIPv6::getFDS() {
    static fd_set FDS;
    return &FDS;
}

/*
 * returns FileDescritor
 */
int TIfaceSocketIPv6::getFD() {
    return this->FD;
}

/*
 * returns interface ID
 */
int TIfaceSocketIPv6::getIfaceID() {
    return this->IfaceID;
}

/*
 * returns port
 */
int TIfaceSocketIPv6::getPort() {
    return this->Port;
}

/*
 * closes socket, and removes its number from FDS
 */
TIfaceSocketIPv6::~TIfaceSocketIPv6() {
    if (Status!=CONFIGURED) return;

    //pure C function
    sock_del(this->FD);

    FD_CLR(this->FD,getFDS());

    this->Count--;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * flush this data in XML format
 */
ostream & operator <<(ostream & strum, TIfaceSocketIPv6 &x)
{
    strum << "<IfaceSocket"
	  << " fd=\"" << x.getFD() << "\""
	  << " port=\"" << x.getPort() << "\""
	  << " iface=\"" << x.Iface << "\""
	  << " addr=\"" << x.Plain << "\"";
    if (x.Multicast) 
	strum << " multicast=\"true\"";
    if (x.IfaceOnly)
	strum << " ifaceonly=\"true\"";
    strum << " status==\"";
	
	switch (x.Status) {
	case NOTCONFIGURED: { strum << "NOTCONFIGURED"; break; }
	case INPROCESS: { strum << "INPROCESS"; break; }
	case CONFIGURED:{ strum << "CONFIGURED"; break; }
	case FAILED:{ strum << "FAILED"; break; }
	case UNKNOWN:
	default:
		{ strum << "UNKNOWN"; break; }
	}
		
	strum << "\"" << " />" << endl;
    return strum;
}
