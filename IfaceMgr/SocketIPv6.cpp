/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SocketIPv6.cpp,v 1.16 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.14  2004/12/02 00:51:05  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.13  2004/11/25 01:22:22  thomson
 * Comment tags converted from javadoc to doxygen format.
 *
 * Revision 1.12  2004/11/01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.11  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.10  2004/09/07 15:37:44  thomson
 * Socket handling changes.
 *
 * Revision 1.9  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.8  2004/09/03 20:58:35  thomson
 * *** empty log message ***
 *
 * Revision 1.7  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.6  2004/04/10 12:18:01  thomson
 * Numerous fixes: LogName, LogMode options added, dns-servers changed to
 * dns-server, '' around strings are no longer needed.
 *
 * Revision 1.5  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */


#include <string>
#include "SocketIPv6.h"
#include "Portable.h"
#include "DHCPConst.h"
#include "Logger.h"

/**
 * IfaceSocketIPv6 - represents network socket 
 * @date 2003-10-26
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

/**
 * static elements of TIfaceSocket class
 */ 
int TIfaceSocket::Count=0;

/**
 * creates socket bound to specific address on this interface
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param addr - address 
 * @param ifaceonly - force interface-only flag in setsockopt()
 */
TIfaceSocket::TIfaceSocket(char * iface, int ifaceid, int port,
				   SmartPtr<TIPv6Addr> addr, bool ifaceonly, bool reuse) { 
    if (this->Count==0) {
	FD_ZERO(getFDS());
    }
    this->Count++;
    this->createSocket(iface, ifaceid, addr, port, ifaceonly, reuse);
}

enum EState TIfaceSocket::getStatus() {
    return this->Status;
}


/**
 * creates socket bound to this interface
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param ifaceonly - force interface-only flag in setsockopt()
 */
TIfaceSocket::TIfaceSocket(char * iface,int ifaceid, int port,bool ifaceonly, bool reuse) {
    if (this->Count==0) {
	FD_ZERO(getFDS());
    }

    // bind it to any address (::)
    char anyaddr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    SmartPtr<TIPv6Addr> smartAny (new TIPv6Addr(anyaddr));   
    this->createSocket(iface, ifaceid, smartAny, port, ifaceonly, reuse);
    this->Count++;
}

/**
 * creates socket on this interface.
 * @param iface - interface name
 * @param ifaceid - interface ID
 * @param port - port, to which socket will be bound
 * @param addr - address 
 * @param ifaceonly - force interface-only flag in setsockopt()
 * returns error code (or 0 if everything is ok)
 */
int TIfaceSocket::createSocket(char * iface, int ifaceid, SmartPtr<TIPv6Addr> addr, 
				   int port, bool ifaceonly, bool reuse) {
    int sock;

    // store info about this socket 
    strncpy(this->Iface,iface,MAX_IFNAME_LENGTH);
    this->IfaceID = ifaceid;
    this->Port = port;
    this->IfaceOnly = ifaceonly;
    this->Status = UNKNOWN;
    this->Addr   = addr;
    
    // is this address multicast? So the socket is.
    if ((addr->getAddr())[0]==(char)0xff) 
        this->Multicast = true;
    else
        this->Multicast = false;

    // create socket
    sock = sock_add(this->Iface, this->IfaceID, addr->getPlain(), 
		    this->Port, ifaceonly?1:0, reuse?1:0);

    // detect errors
    switch (sock) {
    case -1:
        Log(Error) << "Unable to create socket. Is IPv6 protocol supported in your system?" 
		   << LogEnd;
        break;
    case -2:
        Log(Error) << "Unable to bind socket to interface " << this->Iface << "/" 
		   << this->IfaceID << "." << LogEnd;
        break;
    case -3: // this error no longer could occur in Linux version
        Log(Error) << "Unable to create a network address structure (addr=" 
		  << addr->getPlain() << ")" << LogEnd;
    case -4:
        Log(Error) << "Unable to bind socket (iface=" << iface << "/" << ifaceid 
		   << ", addr=" << addr->getPlain() << ", port=" 
		   << this->Port << ")." << LogEnd;
        break;
    case -5:
        Log(Error) << "Unable to getaddrinfo() of multicast group. (iface= " << iface << "/" << ifaceid  
		   << ", addr=" << *this->Addr << ", port=" << this->Port << ")" << LogEnd;
        break;
    case -6:
        Log(Error) << "Unable to join multicast group." << LogEnd;
	break;
        break;
    case -7:
        Log(Error) << "getaddrinfo() failed. Is IPv6 protocol supported by your system?" << LogEnd;
        break;
    case -8:
        Log(Error) << "Unable to set up socket option IPV6_PKTINFO" << LogEnd;
        break;
    case -9:
        Log(Error) << "Unable to set up socket option SO_REUSEADDR" << LogEnd;
        break;
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

/**
 * sends data through socket
 * @param buf - buffer to send
 * @param len - number of bytes to send
 * @param addr - where send this data
 * @param port - to which port
 * returns number of bytes sent or -1 if something went wrong
 */
int TIfaceSocket::send(char * buf,int len,SmartPtr<TIPv6Addr> addr,int port) {

    int result;
    
    //extern "C" int sock_send(int fd, char * addr, char * buf, int buflen, int port, int ifaceID);
    result = sock_send(this->FD, addr->getPlain(), buf, len, port, this->IfaceID);
    
    switch(result) {
    case -2:
	Log(Error) << "Unable to create a network address structure (addr=" 
		   << *addr << ")" << LogEnd;
	return -1;
    }
    return result;
}

/**
 * receives data from socket
 * @param buf - received data are stored here
 * @param addr - will contain info about sender
 */
int TIfaceSocket::recv(char * buf, SmartPtr<TIPv6Addr> addr) {
    char myPlainAddr[48];
    char peerPlainAddr[48];

    // maximum DHCPv6 packet size
    int len=1500;

    len = sock_recv(this->FD, myPlainAddr, peerPlainAddr, buf, len);

    if ( len  < 0 ) {
        Log(Error) << "Error during sock_recv() on socket " << this->FD << LogEnd;
        return -1;
    }

    // convert to packed form (plain->16-byte)
    char packedAddr[16];
    inet_pton6(peerPlainAddr,packedAddr);
    addr->setAddr(packedAddr);
    return len;
}

/**
 * returns FDS - FileDescriptorSet 
 * it's some really weird POSIX macro. It uses FD_SET, FD_ZERO and FD_CLR macros
 * defined somewhere in system headers
 */
fd_set * TIfaceSocket::getFDS() {
    static fd_set FDS;
    return &FDS;
}

/**
 * returns FileDescritor
 */
int TIfaceSocket::getFD() {
    return this->FD;
}

/**
 * returns interface ID
 */
int TIfaceSocket::getIfaceID() {
    return this->IfaceID;
}

/**
 * returns port
 */
int TIfaceSocket::getPort() {
    return this->Port;
}

/**
 * returns address
 */
SmartPtr<TIPv6Addr> TIfaceSocket::getAddr() {
    return this->Addr;
}

/**
 * closes socket, and removes its number from FDS
 */
TIfaceSocket::~TIfaceSocket() {
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
ostream & operator <<(ostream & strum, TIfaceSocket &x)
{
    strum << dec 
	  << "<IfaceSocket"
	  << " fd=\"" << x.getFD() << "\""
	  << " port=\"" << x.getPort() << "\""
	  << " iface=\"" << x.Iface << "\""
	  << " addr=\"" << *x.Addr << "\"";
    if (x.Multicast) 
	strum << " multicast=\"true\"";
    if (x.IfaceOnly)
	strum << " ifaceonly=\"true\"";
    strum << " status=\"" << StateToString(x.Status)
	  << "\"" << " />" << endl;
    return strum;
}
