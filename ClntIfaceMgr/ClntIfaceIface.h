/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceIface.h,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTIFACEIFACE_H
#define CLNTIFACEIFACE_H

#include "Iface.h"
#include "SmartPtr.h"
#include "DUID.h"

class TClntIfaceIface: public TIfaceIface {
 public:
    friend ostream & operator <<(ostream & strum, TClntIfaceIface &x);
    TClntIfaceIface(char * name, int id, unsigned int flags, char* mac, 
		    int maclen, char* llAddr, int llAddrCnt, int hwType);
    ~TClntIfaceIface();

    bool setDNSServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setDomainLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(string) domains);
    bool setNTPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setTimezone(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string timezone);
    bool setSIPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setSIPDomainLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(string) domains);
    bool setFQDN(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string fqdn);
    bool setNISServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISDomain(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string domain);
    bool setNISPServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISPDomain(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, string domain);
    bool setLifetime(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, unsigned int life);
    void removeAllOpts();

 private:
    List(TIPv6Addr)     DNSServerLst;
    unsigned int        DNSServerLstTimestamp;
    unsigned int        DNSServerLstTimeout;
    SmartPtr<TIPv6Addr> DNSServerLstAddr;
    SmartPtr<TDUID>     DNSServerLstDUID;
};

#endif
