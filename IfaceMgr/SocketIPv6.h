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

/*
 * repesents network socket
 */
class TIfaceSocket {
    friend std::ostream& operator<<(std::ostream& strum, TIfaceSocket &x);
 public:
    TIfaceSocket(char * iface,int ifaceid, int port, 
		     SPtr<TIPv6Addr> addr, bool ifaceonly, bool reuse);
    TIfaceSocket(char * iface,int ifaceid, int port,
		     bool ifaceonly, bool reuse);
   
    // ---transmission---
    int send(char * buf,int len, SPtr<TIPv6Addr> addr,int port);
    int recv(char * buf,SPtr<TIPv6Addr> addr);
    
    // ---get info---
    inline static int getCount() { return Count; }
    int getFD();
    int getPort();
    int getIfaceID();
    SPtr<TIPv6Addr> getAddr();
    enum EState getStatus();

    // ---select() stuff---
    // FileDescriptors Set, for use with select()
    // (it's really messed up. fd_set is some really weird macro used
    //  with POSIX select() function. )
    static fd_set * getFDS();
    inline static int getMaxFD() { return MaxFD; }
    inline bool multicast() { return Multicast; }

    ~TIfaceSocket();
 private:
    // adds socket to this interface
    int createSocket(char * iface, int ifaceid, SPtr<TIPv6Addr> addr, 
		     int port, bool ifaceonly, bool reuse);
    void printError(int error, char * iface, int ifaceid, SPtr<TIPv6Addr> addr, int port);

    // FileDescriptor
    int FD;

    // bounded port
    int Port;

    // socket status
    enum EState Status;

    // error 
    std::string Error;

    // interface name, on which this socket has been created
    char Iface[MAX_IFNAME_LENGTH];

    // interface ID, on which this socket has been created
    int  IfaceID;

    // bounded address 
    SPtr<TIPv6Addr> Addr;

    // true = bounded to this interface only
    bool IfaceOnly;

    // true = bounded to multicast socket
    bool Multicast;

    // Static element. Class needs to know, when first object is
    // created. It call FD_SET to zero fd_set 
    static int Count;
    static int MaxFD; // needed instead of FD_MAXSIZE on Macs
};


#endif

