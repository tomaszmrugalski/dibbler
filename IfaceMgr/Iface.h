/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: Iface.h,v 1.15.2.1 2007-04-15 21:23:32 thomson Exp $
 *
 */

#ifndef IFACEIFACE_H
#define IFACEIFACE_H

#include "Portable.h"
#include "SmartPtr.h"
#include "Container.h"
#include "SocketIPv6.h"
#include "IPv6Addr.h"

/*
 * represents network interface 
 */
class TIfaceIface{
 public:
    friend ostream & operator <<(ostream & strum, TIfaceIface &x);

    TIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
                int maclen, char* llAddr, int llAddrCnt, char * globalAddr, int globalCnt, int hwType);
    char * getName();
    int getID();
    string getFullName();

    // ---flags related---
    unsigned int getFlags();
    bool flagUp();
    bool flagRunning();
    bool flagMulticast();
    bool flagLoopback();
    void updateState(struct iface * x);

    // ---layer-2 related---
    int   getMacLen();
    char* getMac();
    int   getHardwareType();
    char* firstLLAddress();
    char* getLLAddress();
    int   countLLAddress();

    void firstGlobalAddr();
    SmartPtr<TIPv6Addr> getGlobalAddr();
    unsigned int countGlobalAddr();
    void addGlobalAddr(SmartPtr<TIPv6Addr> addr);
    void delGlobalAddr(SmartPtr<TIPv6Addr> addr);

    // ---address related---
    bool addAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid, int prefixLen);
    bool delAddr(SmartPtr<TIPv6Addr> addr, int prefixLen);
    bool updateAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid);
    void setPrefixLength(int len);
    int getPrefixLength();
    
    // ---socket related---
    bool addSocket(SmartPtr<TIPv6Addr> addr,int port, bool ifaceonly, bool reuse);
    bool addSocket(int port, bool ifaceonly, bool reuse); 
    bool delSocket(int id);
    void firstSocket();
    SmartPtr <TIfaceSocket> getSocketByFD(int fd);
    SmartPtr <TIfaceSocket> getSocket();
    SmartPtr<TIfaceSocket> getSocketByAddr(SmartPtr<TIPv6Addr> addr);
    int countSocket();

    ~TIfaceIface();

 protected:
    // ---interface data---
    char Name[MAX_IFNAME_LENGTH];
    int ID;
    unsigned int Flags;
    char* Mac;
    int Maclen;
    char* LLAddr;
    int LLAddrCnt;

    List(TIPv6Addr) GlobalAddrLst;
    
//    char * GlobalAddr;
//    int GlobalAddrCnt;
    
    int HWType;

    // sockets
    List(TIfaceSocket) SocketsLst;
    char*   PresLLAddr;

    int PrefixLen; // used during address adding
};

#endif
