/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTPARSEIFACEOPT_H
#define CLNTPARSEIFACEOPT_H

#include <iostream>
#include <string>
#include "DHCPConst.h"
#include "Container.h"
#include "ClntParsIAOpt.h"
#include "HostID.h"
#include "IPv6Addr.h"
#include "OptVendorSpecInfo.h"

class TClntParsIfaceOpt : public TClntParsIAOpt
{
 public:
    TClntParsIfaceOpt();
    ~TClntParsIfaceOpt();
    void setUnicast(bool unicast);
    bool getUnicast();
    bool getRapidCommit();
    void setRapidCommit(bool rapid);
    bool getStateful();
    void setStateful(bool state);

    //-- options related methods --
    // option: DNS Servers
    List(TIPv6Addr) * getDNSServerLst();
    void setDNSServerLst(List(TIPv6Addr) *lst);
    bool getReqDNSServer();

    // option: Domain
    List(std::string) * getDomainLst();
    void setDomainLst(List(std::string) * domains);
    bool getReqDomain();

    // option: NTP servers
    List(TIPv6Addr) * getNTPServerLst();
    void setNTPServerLst(List(TIPv6Addr) *lst);
    bool getReqNTPServer();

    // option: Timezone
    std::string getTimezone();
    void setTimezone(const std::string& timeZone);
    bool getReqTimezone();

    // option: SIP servers
    List(TIPv6Addr) * getSIPServerLst();
    void setSIPServerLst(List(TIPv6Addr) *addr);
    bool getReqSIPServer();

    // option: SIP domains
    List(std::string) * getSIPDomainLst();
    void setSIPDomainLst(List(std::string) *domainlist);
    bool getReqSIPDomain();

    // option: FQDN
    std::string getFQDN();
    void setFQDN(const std::string& fqdn);
    bool getReqFQDN();

    // option: NIS servers
    List(TIPv6Addr) * getNISServerLst();
    void setNISServerLst( List(TIPv6Addr) *nislist);
    bool getReqNISServer();

    // option: NIS+ servers
    List(TIPv6Addr) * getNISPServerLst();
    void setNISPServerLst( List(TIPv6Addr) *nisplist);
    bool getReqNISPServer();

    // option: NIS domain
    std::string getNISDomain();
    void setNISDomain(const std::string& domain);
    bool getReqNISDomain();

    // option: NISP domain
    std::string getNISPDomain();
    void setNISPDomain(const std::string& domain);
    bool getReqNISPDomain();

    // option: Lifetime
    bool getLifetime();
    void setLifetime();
    bool getReqLifetime();

    // option: Prefix Delegation
    void setPrefixDelegation();
    bool getReqPrefixDelegation();

    // option: vendor-spec
    void setVendorSpec(List(TOptVendorSpecInfo) vendorSpec);
    void setVendorSpec();
    bool getReqVendorSpec();
    List(TOptVendorSpecInfo) getVendorSpec();

private:
    /// defined whether this interface is running in stateless mode
    bool Stateless_;

    /// do we accept unicast?
    bool Unicast;

    /// should we try to use rapid-commit?
    bool RapidCommit;

    List(TIPv6Addr) DNSServerLst;
    List(std::string) DomainLst;
    List(TIPv6Addr) NTPServerLst;
    std::string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(std::string) SIPDomainLst;
    std::string FQDN;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    std::string NISDomain;
    std::string NISPDomain;
    bool Lifetime;
    List(TOptVendorSpecInfo) VendorSpec;

    bool ReqDNSServer;
    bool ReqDomain;
    bool ReqNTPServer;
    bool ReqTimezone;
    bool ReqSIPServer;
    bool ReqSIPDomain;
    bool ReqFQDN;
    bool ReqNISServer;
    bool ReqNISPServer;
    bool ReqNISDomain;
    bool ReqNISPDomain;
    bool ReqLifetime;
    bool ReqPrefixDelegation;
    bool ReqVendorSpec;
};


#endif
