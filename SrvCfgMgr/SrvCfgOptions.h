/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVCFGOPTIONS_H
#define SRVCFGOPTIONS_H

#include <iostream>
#include <string>
#include <list>

#include "SmartPtr.h"
#include "Container.h"
#include "IPv6Addr.h"
#include "SrvParsGlobalOpt.h"

#include "OptVendorSpecInfo.h"
#include "OptVendorData.h"
#include "OptGeneric.h"

class TSrvCfgIface;

class TSrvCfgOptions 
{
    friend std::ostream& operator<<(std::ostream& out,TSrvCfgIface& iface);
    friend std::ostream& operator<<(std::ostream& out,TSrvCfgOptions& opt);

public:
    TSrvCfgOptions();
    TSrvCfgOptions(SPtr<TDUID> duid);
    TSrvCfgOptions(SPtr<TOptVendorData> remoteid);
    bool setOptions(SPtr<TSrvParsGlobalOpt> opt);
    SPtr<TDUID> getDuid() const;
    SPtr<TOptVendorData> getRemoteID() const;

    // address reservation
    void setAddr(SPtr<TIPv6Addr> addr);
    SPtr<TIPv6Addr> getAddr() const;

    // option: DNS Servers
    List(TIPv6Addr) * getDNSServerLst();
    void setDNSServerLst(List(TIPv6Addr) *lst);
    bool supportDNSServer();

    // option: Domain
    List(std::string) * getDomainLst();
    void setDomainLst(List(std::string) * domains);
    bool supportDomain();

    // option: NTP servers
    List(TIPv6Addr) * getNTPServerLst();
    void setNTPServerLst(List(TIPv6Addr) *lst);
    bool supportNTPServer();

    // option: Timezone
    std::string getTimezone();
    void setTimezone(std::string timeZone);
    bool supportTimezone();

    // option: SIP servers
    List(TIPv6Addr) * getSIPServerLst();
    void setSIPServerLst(List(TIPv6Addr) *addr);
    bool supportSIPServer();

    // option: SIP domains
    List(std::string) * getSIPDomainLst();
    void setSIPDomainLst(List(std::string) *domainlist);
    bool supportSIPDomain();

    // option: NIS servers
    List(TIPv6Addr) * getNISServerLst();
    void setNISServerLst( List(TIPv6Addr) *nislist);
    bool supportNISServer();

    // option: NIS+ servers
    List(TIPv6Addr) * getNISPServerLst();
    void setNISPServerLst( List(TIPv6Addr) *nisplist);
    bool supportNISPServer();

    // option: NIS domain
    std::string getNISDomain();
    void setNISDomain(std::string domain);
    bool supportNISDomain();

    // option: NIS+ domain is now handled with extra options mechanism

    // option: LIFETIME
    void setLifetime(unsigned int life);
    unsigned int getLifetime();
    bool supportLifetime();

    // option: VENDOR-SPEC
    List(TOptVendorSpecInfo) getVendorSpecLst(unsigned int vendor=0);

    void addExtraOption(SPtr<TOpt> extra, bool always);
    const TOptList& getExtraOptions();
    SPtr<TOpt> getExtraOption(uint16_t type);
    const TOptList& getForcedOptions();
    void addExtraOptions(const TOptList& extra);
    void addForcedOptions(const TOptList& extra);

private:
    // options
    bool DNSServerSupport;
    bool DomainSupport;
    bool NTPServerSupport;
    bool TimezoneSupport;
    bool SIPServerSupport;
    bool SIPDomainSupport;
    bool NISServerSupport;
    bool NISDomainSupport;
    bool NISPServerSupport;
    bool NISPDomainSupport;
    bool LifetimeSupport;
    bool VendorSpecSupport;

    // address reservation
    SPtr<TIPv6Addr> Addr;

    // options reservation
    List(TIPv6Addr) DNSServerLst;
    List(std::string) DomainLst;
    List(TIPv6Addr) NTPServerLst;
    std::string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(std::string) SIPDomainLst;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    std::string NISDomain;
    std::string NISPDomain;
    TOptList ExtraOpts_;  // extra options ALWAYS sent to client (may also include ForcedOpts)
    TOptList ForcedOpts_; // list of options that are forced to client
    unsigned int Lifetime;

    void SetDefaults();

    SPtr<TOptVendorData> RemoteID;
    SPtr<TDUID> Duid;
};

#endif
