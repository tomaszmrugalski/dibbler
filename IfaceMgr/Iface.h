/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: Iface.h,v 1.18 2008-08-29 00:07:30 thomson Exp $
 *
 */

#ifndef IFACEIFACE_H
#define IFACEIFACE_H

#include <list>
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
    friend std::ostream & operator <<(std::ostream & strum, TIfaceIface &x);

    TIfaceIface(const char * name, int id, unsigned int flags, char* mac, 
                int maclen, char* llAddr, int llAddrCnt, char * globalAddr, int globalCnt, int hwType);
    char * getName();
    int getID();
    std::string getFullName();

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
    SPtr<TIPv6Addr> getGlobalAddr();
    unsigned int countGlobalAddr();
    void addGlobalAddr(SPtr<TIPv6Addr> addr);
    void delGlobalAddr(SPtr<TIPv6Addr> addr);

    // ---address related---
    bool addAddr(SPtr<TIPv6Addr> addr, long pref, long valid, int prefixLen);
    bool delAddr(SPtr<TIPv6Addr> addr, int prefixLen);
    bool updateAddr(SPtr<TIPv6Addr> addr, long pref, long valid);
    void setPrefixLength(int len);
    int getPrefixLength();
    
    // ---socket related---
    bool addSocket(SPtr<TIPv6Addr> addr,int port, bool ifaceonly, bool reuse);
    // bool addSocket(int port, bool ifaceonly, bool reuse); 
    bool delSocket(int id);
    void firstSocket();
    SPtr <TIfaceSocket> getSocketByFD(int fd);
    SPtr <TIfaceSocket> getSocket();
    SPtr<TIfaceSocket> getSocketByAddr(SPtr<TIPv6Addr> addr);
    int countSocket();

    virtual ~TIfaceIface();

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

typedef std::list < SPtr<TIfaceIface> > TIfaceIfaceLst;

#endif
