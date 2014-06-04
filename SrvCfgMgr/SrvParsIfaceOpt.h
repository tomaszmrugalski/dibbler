/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
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

    void setRelayName(std::string name);
    void setRelayID(int ifindex);
    void setRelayInterfaceID(SPtr<TSrvOptInterfaceID> id);
    std::string getRelayName();
    int getRelayID();
    SPtr<TSrvOptInterfaceID> getRelayInterfaceID();
    bool isRelay();

    // leasequery support
    void setLeaseQuerySupport(bool support);
    bool getLeaseQuerySupport();

    //-- options related methods --
    // option: DNS Servers servers is now handled with extra options mechanism
    // option: Domain servers is now handled with extra options mechanism
    // option: NTP servers servers is now handled with extra options mechanism
    // option: Timezone servers is now handled with extra options mechanism
    // option: SIP servers is now handled with extra options mechanism
    // option: SIP domains is now handled with extra options mechanism
    // option: NIS servers is now handled with extra options mechanism
    // option: NIS+ servers is now handled with extra options mechanism
    // option: NIS domain is now handled with extra options mechanism
    // option: NISP domain is now handled with extra options mechanism
    // option: LIFETIME servers is now handled with extra options mechanism

    // option: FQDN
    List(TFQDN) *getFQDNLst();
    int getRevDNSZoneRootLength();
    void setRevDNSZoneRootLength(int revDNSZoneRootLength);
    void setUnknownFQDN(EUnknownFQDNMode mode, const std::string& domain);
    EUnknownFQDNMode getUnknownFQDN();
    std::string getFQDNDomain();

    void setFQDNLst(List(TFQDN) *fqdn);
    bool supportFQDN();

    int getFQDNMode();
    void setFQDNMode(int FQDNMode);

#if 0
    // option: VENDOR-SPEC INFO
    void setVendorSpec(List(TOptVendorSpecInfo) vendor);
    bool supportVendorSpec();
    List(TOptVendorSpecInfo) getVendorSpec();
#endif

    // extra options
    void addExtraOption(SPtr<TOpt> extra, bool always);
    const TOptList& getExtraOptions();
    SPtr<TOpt> getExtraOption(uint16_t type);
    const TOptList& getForcedOptions();

private:

    void addOption(TOptList& list, SPtr<TOpt> opt);

    /// @todo: Preference should be a global value
    char Preference_;
    bool RapidCommit_;
    long IfaceMaxLease_;
    long ClntMaxLease_;
    SPtr<TIPv6Addr> Unicast_;
    bool LeaseQuery_; // support for leasequery

    // relay
    bool Relay_;
    std::string RelayName_;
    int RelayID_;
    SPtr<TSrvOptInterfaceID> RelayInterfaceID_;

    // options
    List(TOptVendorSpecInfo) VendorSpec_;

    // FQDN
    bool FQDNSupport_;
    List(TFQDN) FQDNLst_;
    int FQDNMode_;
    EUnknownFQDNMode UnknownFQDN_; // accept, reject, append domain, generate procedurally
    std::string FQDNDomain_;
    int RevDNSZoneRootLength_;

    /// @brief extra options ALWAYS sent to client (may also include ForcedOpts)
    TOptList ExtraOpts;
    /// @brief list of options that are forced to client
    TOptList ForcedOpts;
};

#endif
