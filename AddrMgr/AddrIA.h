/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: AddrIA.h,v 1.5 2006-08-03 00:43:15 thomson Exp $
 *
 */

class TAddrIA;
#ifndef ADDRIA_H
#define ADDRIA_H

#include <iostream>
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "AddrAddr.h"
#include "DUID.h"
#include "FQDN.h"

using namespace std;

class TAddrIA
{
  public:
    friend ostream & operator<<(ostream & strum,TAddrIA &x);
    TAddrIA(int iface, SmartPtr<TIPv6Addr> addr, SmartPtr<TDUID> duid, 
	    unsigned long T1, unsigned long T2,unsigned long ID);
    ~TAddrIA();

    //---IA state---
    enum EState getState();
    void setState(enum EState state);
    void setT1(unsigned long T1);
    void setT2(unsigned long T2);
    unsigned long getT1();
    unsigned long getT2();
    unsigned long getIAID();

    //---Iface---
    int getIface();

    //---Server's DUID---
    void setDUID(SmartPtr<TDUID> duid);
    SmartPtr<TDUID> getDUID();

    //---Contact with server using Unicast/Multicast---
    void setUnicast(SmartPtr<TIPv6Addr> addr);
    void setMulticast();
    SmartPtr<TIPv6Addr> getSrvAddr();

    //---list related methods---
    void addAddr(SmartPtr<TAddrAddr> x);
    void addAddr(SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid);
    int getAddrCount();

    // --- address management ---
    void firstAddr();
    SmartPtr<TAddrAddr> getAddr();
    SmartPtr<TAddrAddr> getAddr(SmartPtr<TIPv6Addr> addr);
    int countAddr();
    int delAddr(SmartPtr<TIPv6Addr> addr);
    
    // timestamp
    void setTimestamp(unsigned long ts);
    void setTimestamp();
    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();
    unsigned long getMaxValidTimeout();
    unsigned long getMaxValid();

    //---tentative---
    unsigned long getTentativeTimeout();
    enum ETentative getTentative();
    void setTentative();

    //---DNS Updates---
    void setFQDNDnsServer(SPtr<TIPv6Addr> srvAddr);
    SPtr<TIPv6Addr> getFQDNDnsServer();
    void setFQDN(SPtr<TFQDN> fqdn);
    SPtr<TFQDN> getFQDN();

private:
    TContainer< SmartPtr<TAddrAddr> > AddrLst;

    unsigned long IAID;
    unsigned long T1;
    unsigned long T2;

    enum EState State; // State of this IA
    enum ETentative Tentative;

    unsigned long Timestamp; // timestamp of last IA refresh (renew/rebind/confirm etc.)

    SmartPtr<TDUID> DUID;

    // Server which maintains this IA is connected by unicast or multicast
    bool Unicast;
    SmartPtr<TIPv6Addr> SrvAddr;

    // Iface ID
    int Iface;

    SPtr<TIPv6Addr> fqdnDnsServer; // DNS Updates was performed to that server
    SPtr<TFQDN> fqdn;              // this FQDN object was used to perform update
};

#endif 
