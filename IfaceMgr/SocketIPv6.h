class TIfaceSocketIPv6;
#ifndef IFACESOCKETIPV6_H
#define IFACESOCKETIPV6_H

/*
 * IfaceSocketIPv6 - represents network socket 
 * @date 2003-10-26
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

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
class TIfaceSocketIPv6 {
    friend ostream& operator<<(ostream& strum, TIfaceSocketIPv6 &x);
 public:
    TIfaceSocketIPv6(char * iface,int ifaceid, int port, 
		     SmartPtr<TIPv6Addr> addr, bool ifaceonly=true);
    TIfaceSocketIPv6(char * iface,int ifaceid, int port,
		     bool ifaceonly=true);
   
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

    ~TIfaceSocketIPv6();
 private:
    // adds socket to this interface
    int createSocket(char * iface, int ifaceid, SmartPtr<TIPv6Addr> addr, 
		     int port, bool ifaceonly);

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

