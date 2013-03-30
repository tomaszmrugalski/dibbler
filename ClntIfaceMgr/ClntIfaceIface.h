/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
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
    friend std::ostream & operator <<(std::ostream & strum, TClntIfaceIface &x);
    TClntIfaceIface(char * name, int id, unsigned int flags, char* mac, 
		    int maclen, char* llAddr, int llAddrCnt, char * globalAddr, 
		    int globalAddrCnt, int hwType);
    ~TClntIfaceIface();

    bool setDNSServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(std::string) domains);
    bool setNTPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setTimezone(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& timezone);
    bool setSIPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setSIPDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(std::string) domains);
    bool setFQDN(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& fqdn);
    bool setNISServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& domain);
    bool setNISPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    bool setNISPDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& domain);
    bool setLifetime(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, unsigned int life);

    bool setDsLiteTunnel(SPtr<TIPv6Addr> remoteEndpoint);
    SPtr<TIPv6Addr> getDsLiteTunnel();

    void removeAllOpts();
    unsigned int getTimeout();

    std::string getFQDN();
    List(TIPv6Addr) getDNSServerLst();

 private:

    void addString(const char * filename, const char * str);
    void delString(const char * filename, const char * str);
    void setString(const char * filename, const char * str);

    List(TIPv6Addr) DNSServerLst;
    List(std::string)    DomainLst;
    List(TIPv6Addr) NTPServerLst;
    std::string Timezone;
    std::string FQDN;
    List(TIPv6Addr) SIPServerLst;
    List(std::string)    SIPDomainLst;
    List(TIPv6Addr) NISServerLst;
    std::string     NISDomain;
    List(TIPv6Addr) NISPServerLst;
    std::string     NISPDomain;
    SPtr<TIPv6Addr> TunnelEndpoint;
    
    unsigned int LifetimeTimeout;
    unsigned int LifetimeTimestamp;

    /// @brief specifies if the DNS configuration should be wiped out during
    ///        first configuration
    ///
    /// Controlled with FLUSH_OTHER_CONFIGURED_DNS_SERVERS in Misc/Portable.h
    bool DnsConfigured;

};

#endif
