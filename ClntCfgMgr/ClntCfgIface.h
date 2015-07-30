/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *          Mateusz Ozga <matozga@gmail.com>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTCFGIFACE_H
#define CLNTCFGIFACE_H
#include <list>

#include "Container.h"
#include "HostID.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "ClntCfgTA.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "ClntCfgIA.h"
#include "ClntCfgPD.h"
#include "OptVendorSpecInfo.h"
#include "Opt.h"

class TClntCfgIface
{
    friend std::ostream& operator<<(std::ostream&,TClntCfgIface&);
public:

    class TOptionStatus
    {
    public:
	TOptionStatus(): OptionType(0), State(STATE_NOTCONFIGURED), Option(), Always(true) {};
	unsigned short OptionType;
	EState State;
	SPtr<TOpt> Option;
	TOpt::EOptionLayout Layout;
	bool Always; // should this option be sent always? (even when already configured?)
    };
    typedef std::list< SPtr<TOptionStatus> > TOptionStatusLst;

    TClntCfgIface(const std::string& ifaceName);
    TClntCfgIface(int ifaceNr);

    void setRouting(bool enabled);
    bool isRoutingEnabled();
    EState getRoutingEnabledState();
    void setRoutingEnabledState(EState state);

    bool isServerRejected(SPtr<TIPv6Addr> addr,SPtr<TDUID> duid);

    // IA
    void firstIA();
    int countIA();
    SPtr<TClntCfgIA> getIA();
    SPtr<TClntCfgIA> getIA(int iaid);
    void addIA(SPtr<TClntCfgIA> ptr);

    // PD
    void firstPD();
    int countPD();
    SPtr<TClntCfgPD> getPD();
    SPtr<TClntCfgPD> getPD(int iaid);
    void addPD(SPtr<TClntCfgPD> ptr);

    // TA
    void firstTA();
    void addTA(SPtr<TClntCfgTA> ta);
    SPtr<TClntCfgTA> getTA();
    int countTA();

    std::string getName(void);
    std::string getFullName(void);
    void setOptions(SPtr<TClntParsGlobalOpt> opt);
    int	getID(void);
    void setNoConfig();
    void setIfaceID(int ifaceID);
    void setIfaceName(const std::string& ifaceName);
    bool noConfig();

    bool stateless();
    bool getUnicast();

    bool getRapidCommit();
    void setRapidCommit(bool rapCom);

    // --- option: DNS servers ---
    bool isReqDNSServer();
    EState getDNSServerState();
    void setDNSServerState(EState state);
    unsigned long getDNSServerTimeout();
    List(TIPv6Addr) * getProposedDNSServerLst();

    // --- option: Domain ---
    bool isReqDomain();
    EState getDomainState();
    unsigned long getDomainTimeout();
    void setDomainState(EState state);
    List(std::string) * getProposedDomainLst();

    // --- option: NTP servers ---
    bool isReqNTPServer();
    EState getNTPServerState();
    unsigned long getNTPServerTimeout();
    void setNTPServerState(EState state);
    List(TIPv6Addr) * getProposedNTPServerLst();

    // --- option: Timezone ---
    /// @todo: Once set, these are never used
    bool isReqTimezone();
    EState getTimezoneState();
    unsigned long getTimezoneTimeout();
    void setTimezoneState(EState state);
    std::string getProposedTimezone();

    // --- option: SIP servers ---
    bool isReqSIPServer();
    EState getSIPServerState();
    unsigned long getSIPServerTimeout();
    void setSIPServerState(EState state);
    List(TIPv6Addr) * getProposedSIPServerLst();

    // --- option: SIP domains ---
    bool isReqSIPDomain();
    EState getSIPDomainState();
    unsigned long getSIPDomainTimeout();
    void setSIPDomainState(EState state);
    List(std::string) * getProposedSIPDomainLst();

    // --- option: FQDN ---
    bool isReqFQDN();
    EState getFQDNState();
    unsigned long getFQDNTimeout();
    void setFQDNState(EState state);
    std::string getProposedFQDN();

    // --- option: NIS servers ---
    bool isReqNISServer();
    EState getNISServerState();
    unsigned long getNISServerTimeout();
    void setNISServerState(EState state);
    List(TIPv6Addr) * getProposedNISServerLst();

    // --- option: NIS+ servers ---
    bool isReqNISPServer();
    EState getNISPServerState();
    unsigned long getNISPServerTimeout();
    void setNISPServerState(EState state);
    List(TIPv6Addr) * getProposedNISPServerLst();

    // --- option: NIS domains ---
    bool isReqNISDomain();
    EState getNISDomainState();
    unsigned long getNISDomainTimeout();
    void setNISDomainState(EState state);
    std::string getProposedNISDomain();

    // --- option: NIS+ domains ---
    bool isReqNISPDomain();
    EState getNISPDomainState();
    unsigned long getNISPDomainTimeout();
    void setNISPDomainState(EState state);
    std::string getProposedNISPDomain();

    // --- option: Lifetime ---
    bool isReqLifetime();
    EState getLifetimeState();
    void setLifetimeState(EState state);

    // --- see strict-rfc-no-routing ---
    void setOnLinkPrefixLength(int len);
    int  getOnLinkPrefixLength();

    // --- option: VendorSpec ---
    bool isReqVendorSpec();
    void vendorSpecSupported(bool support);
    EState getVendorSpecState();
    void setVendorSpecState(EState state);
    void firstVendorSpec();
    SPtr<TOptVendorSpecInfo> getVendorSpec();
    int getVendorSpecCount();
    void setVendorSpec(List(TOptVendorSpecInfo) vendorSpecList);

    // --- custom/extra options ---
    void addExtraOption(SPtr<TOpt> extra, TOpt::EOptionLayout layout, bool sendAlways);
    void addExtraOption(int optType, TOpt::EOptionLayout layout, bool sendAlways);
    TOptionStatusLst& getExtraOptions();
    SPtr<TOptionStatus> getExtaOptionState(int type);

    // --- option: KeyGeneration ---
    EState getKeyGenerationState();
    void setKeyGenerationState(EState state);

    void setMbit(bool m_bit);
    void setObit(bool o_bit);

    /// @brief sets link-local address to bind to
    /// @param link_local the link local address the client will bind to
    void setBindToAddr(SPtr<TIPv6Addr> link_local);

    /// @brief Returns link-local address to which the client should bind (if set)
    /// @return the address to bind to (or NULL if not set)
    SPtr<TIPv6Addr> getBindToAddr();

private:
    void setDefaults();
    std::string IfaceName;
    int ID;
    bool NoConfig;
    bool Stateful_;
    bool Unicast;
    bool RapidCommit;
    int  PrefixLength; // default prefix length of the configured addresses

    List(TClntCfgIA) IALst;
    List(TClntCfgPD) PDLst;
    List(THostID) PrefSrvLst;
    List(THostID) RejectedSrvLst;

    List(TClntCfgTA) ClntCfgTALst;

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
    List(TOptVendorSpecInfo) VendorSpecLst;

    EState DNSServerState;
    EState DomainState;
    EState NTPServerState;
    EState TimezoneState;
    EState SIPServerState;
    EState SIPDomainState;
    EState FQDNState;
    EState NISServerState;
    EState NISPServerState;
    EState NISDomainState;
    EState NISPDomainState;
    EState LifetimeState;
    EState PrefixDelegationState;
    EState VendorSpecState;
    EState KeyGenerationState;
    EState RoutingEnabledState;

    /// @todo: Remove those booleans and use State directly
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
    bool RoutingEnabled;

    /// @brief Extra options to be sent to server
    TOptionStatusLst ExtraOpts;

    /// @brief Specify bind-to address (may be null)
    SPtr<TIPv6Addr> BindToAddr_;
};

#endif
