/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgOptions.h,v 1.5 2008-08-29 00:07:33 thomson Exp $
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
    SPtr<TDUID> getDuid();
    SPtr<TOptVendorData> getRemoteID();

    // address reservation
    void setAddr(SPtr<TIPv6Addr> addr);
    SPtr<TIPv6Addr> getAddr();

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

    // option: VENDOR-SPEC
    List(TOptVendorSpecInfo) getVendorSpecLst(unsigned int vendor=0);

    void addExtraOption(SPtr<TOpt> extra, bool always);
    const TOptList& getExtraOptions();
    SPtr<TOpt> getExtraOption(int type);
    const TOptList& getForcedOptions();

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
    List(string) DomainLst;
    List(TIPv6Addr) NTPServerLst;
    string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(string) SIPDomainLst;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
    TOptList ExtraOpts;  // extra options ALWAYS sent to client (may also include ForcedOpts)
    TOptList ForcedOpts; // list of options that are forced to client
    unsigned int Lifetime;

    void SetDefaults();

    SPtr<TOptVendorData> RemoteID;
    SPtr<TDUID> Duid;
};

#endif
