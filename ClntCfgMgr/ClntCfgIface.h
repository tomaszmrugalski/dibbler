/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntCfgIface.h,v 1.20 2008-08-29 00:07:27 thomson Exp $
 */

#ifndef CLNTCFGIFACE_H
#define CLNTCFGIFACE_H
#include <list>

#include "Container.h"
#include "StationID.h"
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
	TOptionStatus(): OptionType(0), State(STATE_NOTCONFIGURED), Option(0), Always(true) {};
	unsigned short OptionType;
	EState State;
	SPtr<TOpt> Option;
	TOpt::EOptionLayout Layout;
	bool Always; // should this option be sent always? (even when already configured?)
    };
    typedef std::list< SPtr<TOptionStatus> > TOptionStatusLst;

    TClntCfgIface(string ifaceName);
    TClntCfgIface(int ifaceNr);

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

    string getName(void);
    string getFullName(void);
    void setOptions(SPtr<TClntParsGlobalOpt> opt);
    int	getID(void);
    void setNoConfig();
    void setIfaceID(int ifaceID);
    void setIfaceName(string ifaceName);
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
    List(string) * getProposedDomainLst();

    // --- option: NTP servers ---
    bool isReqNTPServer();
    EState getNTPServerState();
    unsigned long getNTPServerTimeout();
    void setNTPServerState(EState state);
    List(TIPv6Addr) * getProposedNTPServerLst();

    // --- option: Timezone ---
    bool isReqTimezone();
    EState getTimezoneState();
    unsigned long getTimezoneTimeout();
    void setTimezoneState(EState state);
    string getProposedTimezone();

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
    List(string) * getProposedSIPDomainLst();

    // --- option: FQDN ---
    bool isReqFQDN();
    EState getFQDNState();
    unsigned long getFQDNTimeout();
    void setFQDNState(EState state);
    string getProposedFQDN();

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
    string getProposedNISDomain();

    // --- option: NIS+ domains ---
    bool isReqNISPDomain();
    EState getNISPDomainState();
    unsigned long getNISPDomainTimeout();
    void setNISPDomainState(EState state);
    string getProposedNISPDomain();

    // --- option: Lifetime ---
    bool isReqLifetime();
    EState getLifetimeState();
    void setLifetimeState(EState state);

    // --- option: Prefix Delegation ---
    bool isReqPrefixDelegation();
    void setPrefixLength(int len);
    int  getPrefixLength();

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

    // --- option: Authentication ---
    EState getAuthenticationState();
    void setAuthenticationState(EState state);

private:
    void setDefaults();
    string IfaceName;
    int ID;
    bool NoConfig;
    bool isIA;
    bool Unicast;
    bool RapidCommit;
    int  PrefixLength; // default prefix length of the configured addresses

    List(TClntCfgIA) IALst;
    List(TClntCfgPD) PDLst;
    List(TStationID) PrefSrvLst;
    List(TStationID) RejectedSrvLst;

    List(TClntCfgTA) ClntCfgTALst;

    List(TIPv6Addr) DNSServerLst;
    List(string) DomainLst;
    List(TIPv6Addr) NTPServerLst;
    string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(string) SIPDomainLst;
    string FQDN;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
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
    EState AuthenticationState;

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

    TOptionStatusLst ExtraOpts; // extra options to be sent to server
};

#endif
