/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                         
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.h,v 1.18 2008-11-11 22:41:49 thomson Exp $
 *
 */

#ifndef TSRCPARSIFACEOPT_H_
#define TSRCPARSIFACEOPT_H_

#include "OptVendorSpecInfo.h"
#include "SrvOptInterfaceID.h"
#include "SrvParsClassOpt.h"
#include "FQDN.h"

class TSrvParsIfaceOpt : public TSrvParsClassOpt
{
public:
    TSrvParsIfaceOpt(void);
    ~TSrvParsIfaceOpt(void);

    bool uniAddress();
    void setUniAddress(bool isUni);
    void setClntMaxLease(long maxLeases);
    long getClntMaxLease();
    void setIfaceMaxLease(long maxLease);
    long getIfaceMaxLease();

    void setPreference(char pref);
    char getPreference();

    void setRapidCommit(bool rapidComm);
    bool getRapidCommit();

    void setUnicast(SPtr<TIPv6Addr> addr);
    SPtr<TIPv6Addr> getUnicast();

    void setRelayName(string name);
    void setRelayID(int ifindex);
    void setRelayInterfaceID(SPtr<TSrvOptInterfaceID> id);
    string getRelayName();
    int getRelayID();
    SPtr<TSrvOptInterfaceID> getRelayInterfaceID();
    bool isRelay();

    // leasequery support
    void setLeaseQuerySupport(bool support);
    bool getLeaseQuerySupport();

    //-- options related methods --
    // option: DNS Servers
    List(TIPv6Addr) * getDNSServerLst();
    void setDNSServerLst(List(TIPv6Addr) *lst);
    bool supportDNSServer();

    // option: Domain
    List(string) * getDomainLst();
    void setDomainLst(List(string) * domains);
    bool supportDomain();

    // option: NTP servers
    List(TIPv6Addr) * getNTPServerLst();
    void setNTPServerLst(List(TIPv6Addr) *lst);
    bool supportNTPServer();

    // option: Timezone
    string getTimezone();
    void setTimezone(string timeZone);
    bool supportTimezone();

    // option: SIP servers
    List(TIPv6Addr) * getSIPServerLst();
    void setSIPServerLst(List(TIPv6Addr) *addr);
    bool supportSIPServer();

    // option: SIP domains
    List(string) * getSIPDomainLst();
    void setSIPDomainLst(List(string) *domainlist);
    bool supportSIPDomain();

    // option: FQDN
    List(TFQDN) *getFQDNLst();
    string getFQDNName(SPtr<TDUID> duid);
    string getFQDNName(SPtr<TIPv6Addr> addr);
    string getFQDNName();
    int getRevDNSZoneRootLength();
    void setRevDNSZoneRootLength(int revDNSZoneRootLength);
    void setUnknownFQDN(EUnknownFQDNMode mode, const std::string domain);
    EUnknownFQDNMode getUnknownFQDN();
    std::string getFQDNDomain();

    SPtr<TDUID> getFQDNDuid(string name);
    void setFQDNLst(List(TFQDN) *fqdn);
    bool supportFQDN();

    int getFQDNMode();
    void setFQDNMode(int FQDNMode);

    // option: NIS servers
    List(TIPv6Addr) * getNISServerLst();
    void setNISServerLst( List(TIPv6Addr) *nislist);
    bool supportNISServer();

    // option: NIS+ servers
    List(TIPv6Addr) * getNISPServerLst();
    void setNISPServerLst( List(TIPv6Addr) *nisplist);
    bool supportNISPServer();

    // option: NIS domain
    string getNISDomain();
    void setNISDomain(string domain);
    bool supportNISDomain();

    // option: NISP domain
    string getNISPDomain();
    void setNISPDomain(string domain);
    bool supportNISPDomain();

    // option: LIFETIME
    void setLifetime(unsigned int life);
    unsigned int getLifetime();
    bool supportLifetime();


#if 0
    // option: VENDOR-SPEC INFO
    void setVendorSpec(List(TOptVendorSpecInfo) vendor);
    bool supportVendorSpec();
    List(TOptVendorSpecInfo) getVendorSpec();
#endif

private:
    char Preference;
    bool RapidCommit;
    long IfaceMaxLease;
    long ClntMaxLease;
    SPtr<TIPv6Addr> Unicast;
    bool LeaseQuery; // support for leasequery

    // relay
    bool Relay;
    string RelayName;
    int RelayID;
    SPtr<TSrvOptInterfaceID> RelayInterfaceID;

    // options
    bool DNSServerSupport;
    bool DomainSupport;
    bool NTPServerSupport;
    bool TimezoneSupport;
    bool SIPServerSupport;
    bool SIPDomainSupport;
    bool FQDNSupport;
    bool NISServerSupport;
    bool NISDomainSupport;
    bool NISPServerSupport;
    bool NISPDomainSupport;
    bool LifetimeSupport;
    bool VendorSpecSupport;

    List(TIPv6Addr) DNSServerLst;
    List(string) DomainLst;			
    List(TIPv6Addr) NTPServerLst;
    string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(string) SIPDomainLst;
    List(TFQDN) FQDNLst;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
    unsigned int Lifetime;

    List(TOptVendorSpecInfo) VendorSpec;

    // FQDN
    int FQDNMode;
    EUnknownFQDNMode UnknownFQDN; // accept, reject, append domain, generate procedurally
    std::string FQDNDomain;
    int revDNSZoneRootLength;
};

#endif
