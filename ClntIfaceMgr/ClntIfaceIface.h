/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceIface.h,v 1.6 2006-08-21 22:18:27 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2006-03-02 00:57:46  thomson
 * FQDN support initial checkin.
 *
 * Revision 1.4  2005/01/25 00:32:26  thomson
 * Global addrs support added.
 *
 * Revision 1.3  2004/11/01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.2  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.1  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
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
    unsigned int getTimeout();

    //Getters for FQDN
    string getFQDN();
    List(TIPv6Addr) getDNSServerLst();
 private:

    void addString(const char * filename, const char * str);
    void delString(const char * filename, const char * str);
    void setString(const char * filename, const char * str);

    List(TIPv6Addr)     DNSServerLst;
    SmartPtr<TIPv6Addr> DNSServerLstAddr;
    SmartPtr<TDUID>     DNSServerLstDUID;

    List(string)        DomainLst;
    SmartPtr<TIPv6Addr> DomainLstAddr;
    SmartPtr<TDUID>     DomainLstDUID;

    List(TIPv6Addr)     NTPServerLst;
    SmartPtr<TIPv6Addr> NTPServerLstAddr;
    SmartPtr<TDUID>     NTPServerLstDUID;

    string Timezone;
    SmartPtr<TIPv6Addr> TimezoneAddr;
    SmartPtr<TDUID>     TimezoneDUID;

    string FQDN;
    SmartPtr<TIPv6Addr> FQDNAddr;
    SmartPtr<TDUID>     FQDNDUID;

    List(TIPv6Addr)     SIPServerLst;
    SmartPtr<TIPv6Addr> SIPServerLstAddr;
    SmartPtr<TDUID>     SIPServerLstDUID;

    List(string)        SIPDomainLst;
    SmartPtr<TIPv6Addr> SIPDomainLstAddr;
    SmartPtr<TDUID>     SIPDomainLstDUID;

    List(TIPv6Addr)     NISServerLst;
    SmartPtr<TIPv6Addr> NISServerLstAddr;
    SmartPtr<TDUID>     NISServerLstDUID;

    string              NISDomain;
    SmartPtr<TIPv6Addr> NISDomainAddr;
    SmartPtr<TDUID>     NISDomainDUID;

    List(TIPv6Addr)     NISPServerLst;
    SmartPtr<TIPv6Addr> NISPServerLstAddr;
    SmartPtr<TDUID>     NISPServerLstDUID;

    string              NISPDomain;
    SmartPtr<TIPv6Addr> NISPDomainAddr;
    SmartPtr<TDUID>     NISPDomainDUID;
    
    unsigned int LifetimeTimeout;
    unsigned int LifetimeTimestamp;
};

#endif
