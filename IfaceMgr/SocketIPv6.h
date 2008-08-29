/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Micha³ Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SocketIPv6.h,v 1.7 2008-08-29 00:07:30 thomson Exp $
 *
 */

class TIfaceSocket;
#ifndef IFACESOCKETIPV6_H
#define IFACESOCKETIPV6_H

#include <iostream>
#include <string>

#include "Portable.h"
#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "SmartPtr.h"
using namespace std;

/*
 * repesents network socket
 */
class TIfaceSocket {
    friend ostream& operator<<(ostream& strum, TIfaceSocket &x);
 public:
    TIfaceSocket(char * iface,int ifaceid, int port, 
		     SmartPtr<TIPv6Addr> addr, bool ifaceonly, bool reuse);
    TIfaceSocket(char * iface,int ifaceid, int port,
		     bool ifaceonly, bool reuse);
   
    // ---transmission---
    int send(char * buf,int len, SmartPtr<TIPv6Addr> addr,int port);
    int recv(char * buf,SmartPtr<TIPv6Addr> addr);
    
    // ---get info---
    int getFD();
    int getPort();
    int getIfaceID();
    SmartPtr<TIPv6Addr> getAddr();
    enum EState getStatus();

    // ---select() stuff---
    // FileDescriptors Set, for use with select()
    // (it's really messed up. fd_set is some really weird macro used
    //  with POSIX select() function. )
    static fd_set * getFDS();

    ~TIfaceSocket();
 private:
    // adds socket to this interface
    int createSocket(char * iface, int ifaceid, SmartPtr<TIPv6Addr> addr, 
		     int port, bool ifaceonly, bool reuse);
    void printError(int error, char * iface, int ifaceid, SPtr<TIPv6Addr> addr, int port);

    // FileDescriptor
    int FD;

    // bounded port
    int Port;

    // socket status
    enum EState Status;

    // error 
    string Error;

    // interface name, on which this socket has been created
    char Iface[MAX_IFNAME_LENGTH];

    // interface ID, on which this socket has been created
    int  IfaceID;

    // bounded address 
    SmartPtr<TIPv6Addr> Addr;

    // true = bounded to this interface only
    bool IfaceOnly;

    // true = bounded to multicast socket
    bool Multicast;

    // Static element. Class needs to know, when first object is
    // created. It call some weird macro to zero fd_set 
    static int Count;
};


#endif

