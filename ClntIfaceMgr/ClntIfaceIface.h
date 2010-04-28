/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntIfaceIface.h,v 1.10 2008-08-29 00:07:27 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2008-08-21 00:25:08  thomson
 * Notify mechanism implemented.
 *
 * Revision 1.8  2008-08-19 23:27:33  thomson
 * TunnelMode additional support.
 *
 *
 */

#ifndef CLNTIFACEIFACE_H
#define CLNTIFACEIFACE_H

#include "Iface.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "ClntIfaceMgr.h"
#include "OptFQDN.h"

class TClntIfaceIface: public TIfaceIface {
 public:
    friend ostream & operator <<(ostream & strum, TClntIfaceIface &x);
    TClntIfaceIface(char * name, int id, unsigned int flags, char* mac, 
		    int maclen, char* llAddr, int llAddrCnt, char * globalAddr, 
		    int globalAddrCnt, int hwType);
    ~TClntIfaceIface();

    bool setDNSServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(string) domains);
    bool setNTPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setTimezone(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, string timezone);
    bool setSIPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setSIPDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(string) domains);
    bool setFQDN(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, string fqdn);
    bool setNISServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, string domain);
    bool setNISPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISPDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, string domain);
    bool setLifetime(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, unsigned int life);

    bool setDsLiteTunnel(SPtr<TIPv6Addr> remoteEndpoint);
    SPtr<TIPv6Addr> getDsLiteTunnel();

    void removeAllOpts();
    unsigned int getTimeout();

    string getFQDN();
    List(TIPv6Addr) getDNSServerLst();

 private:

    void addString(const char * filename, const char * str);
    void delString(const char * filename, const char * str);
    void setString(const char * filename, const char * str);

    List(TIPv6Addr) DNSServerLst;
    SPtr<TIPv6Addr> DNSServerLstAddr;
    SPtr<TDUID>     DNSServerLstDUID;

    List(string)    DomainLst;
    SPtr<TIPv6Addr> DomainLstAddr;
    SPtr<TDUID>     DomainLstDUID;

    List(TIPv6Addr) NTPServerLst;
    SPtr<TIPv6Addr> NTPServerLstAddr;
    SPtr<TDUID>     NTPServerLstDUID;

    string Timezone;
    SPtr<TIPv6Addr> TimezoneAddr;
    SPtr<TDUID>     TimezoneDUID;

    string FQDN;
    SPtr<TIPv6Addr> FQDNAddr;
    SPtr<TDUID>     FQDNDUID;

    List(TIPv6Addr) SIPServerLst;
    SPtr<TIPv6Addr> SIPServerLstAddr;
    SPtr<TDUID>     SIPServerLstDUID;

    List(string)    SIPDomainLst;
    SPtr<TIPv6Addr> SIPDomainLstAddr;
    SPtr<TDUID>     SIPDomainLstDUID;

    List(TIPv6Addr) NISServerLst;
    SPtr<TIPv6Addr> NISServerLstAddr;
    SPtr<TDUID>     NISServerLstDUID;

    string          NISDomain;
    SPtr<TIPv6Addr> NISDomainAddr;
    SPtr<TDUID>     NISDomainDUID;

    List(TIPv6Addr) NISPServerLst;
    SPtr<TIPv6Addr> NISPServerLstAddr;
    SPtr<TDUID>     NISPServerLstDUID;

    string          NISPDomain;
    SPtr<TIPv6Addr> NISPDomainAddr;
    SPtr<TDUID>     NISPDomainDUID;

    SPtr<TIPv6Addr> TunnelEndpoint;
    
    unsigned int LifetimeTimeout;
    unsigned int LifetimeTimestamp;

};

#endif
