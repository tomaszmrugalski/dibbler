/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: Iface.h,v 1.15 2007-04-01 04:53:19 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2006-08-22 00:01:20  thomson
 * Client /64 prefix, strict-rfc-no-routing feature added.
 *
 * Revision 1.13  2006-01-24 00:12:43  thomson
 * Global addresses support improved.
 *
 * Revision 1.12  2005/04/29 00:08:20  thomson
 * *** empty log message ***
 *
 * Revision 1.11  2005/01/23 23:17:53  thomson
 * Relay/global address support related improvements.
 *
 * Revision 1.10  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.9  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 * Revision 1.8  2005/01/03 21:53:41  thomson
 * const modifier added.
 *
 * Revision 1.7  2004/12/27 20:48:22  thomson
 * Problem with absent link local addresses fixed (bugs #90, #91)
 *
 * Revision 1.6  2004/11/01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.5  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
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
    bool addAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid);
    bool delAddr(SmartPtr<TIPv6Addr> addr);
    bool updateAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid);
    void setPrefixLength(int len);
    
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
