/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvCfgIface;
#ifndef SRVCONFIFACE_H
#define SRVCONFIFACE_H
#include "DHCPConst.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgTA.h"
#include "SrvCfgPD.h"
#include "SrvParsGlobalOpt.h"
#include <iostream>
#include <string>
#include "OptVendorSpecInfo.h"
#include "SrvCfgOptions.h"

class TSrvCfgIface: public TSrvCfgOptions
{
    friend std::ostream& operator<<(std::ostream& out, TSrvCfgIface& iface);
public:
    TSrvCfgIface(const std::string& ifaceName);
    TSrvCfgIface(int ifindex);
    virtual ~TSrvCfgIface();
    void setDefaults();
    void setName(const std::string& ifaceName);
    void setID(int ifindex);
    int	getID() const;
    std::string getName() const;
    std::string getFullName() const;

    // permanent address management (IA_NA)
    void addAddrClass(SPtr<TSrvCfgAddrClass> addrClass);
    void firstAddrClass();
    bool getPreferedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    bool getAllowedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    SPtr<TSrvCfgAddrClass> getAddrClass();
    SPtr<TSrvCfgAddrClass> getClassByID(unsigned long id);
    SPtr<TSrvCfgAddrClass> getRandomClass(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr);
    long countAddrClass() const;

    // temporary address management (IA_TA)
    void addTA(SPtr<TSrvCfgTA> ta);
    void firstTA();
    SPtr<TSrvCfgTA> getTA();
    SPtr<TSrvCfgTA> getTA(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);

    // prefix management (IA_PD)
    void addPDClass(SPtr<TSrvCfgPD> PDClass);
    SPtr<TSrvCfgPD> getPDByID(unsigned long id);
    //SPtr<TSrvCfgPD> getRandomPrefix(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr);
    long countPD() const;
    void addPD(SPtr<TSrvCfgPD> pd);
    void firstPD();
    SPtr<TSrvCfgPD> getPD();
    bool addClntPrefix(SPtr<TIPv6Addr> ptrPD, bool quiet = false);
    bool delClntPrefix(SPtr<TIPv6Addr> ptrPD, bool quiet = false);
    bool supportPrefixDelegation() const;

    // other
    SPtr<TIPv6Addr> getUnicast();
    void setNoConfig();
    void setOptions(SPtr<TSrvParsGlobalOpt> opt);

    unsigned char getPreference() const;

    bool getRapidCommit() const;

    long getIfaceMaxLease() const;
    unsigned long getClntMaxLease() const;

    // IA address functions
    void addClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet = false);
    void delClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet = false);

    // TA address functions
    void addTAAddr();
    void delTAAddr();

    // relays
    std::string getRelayName() const;
    int getRelayID() const;
    SPtr<TSrvOptInterfaceID> getRelayInterfaceID() const;
    bool isRelay() const;
    void setRelayName(const std::string& name);
    void setRelayID(int id);

    // per-client parameters (exceptions)
    void addClientExceptionsLst(List(TSrvCfgOptions) exLst);
    SPtr<TSrvCfgOptions> getClientException(SPtr<TDUID> duid, SPtr<TOptVendorData> remoteID, bool quiet=true);

    // option: FQDN
    List(TFQDN) * getFQDNLst();
    SPtr<TFQDN> getFQDNName(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr, const std::string& hint);
    SPtr<TDUID> getFQDNDuid(const std::string& name);
    void setFQDNLst(List(TFQDN) * fqdn);
    int getFQDNMode() const;
    std::string getFQDNModeString() const;
    int  getRevDNSZoneRootLength() const;
    void setRevDNSZoneRootLength(int revDNSZoneRootLength);
    bool supportFQDN() const;
    bool leaseQuerySupport() const;

    void mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst);

private:
    unsigned char Preference_;
    int	ID_;
    std::string Name_;
    bool NoConfig_;
    SPtr<TIPv6Addr> Unicast_;
    unsigned long IfaceMaxLease_;
    unsigned long ClntMaxLease_;
    bool RapidCommit_;
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst_; // IA_NA list (normal addresses)
    bool LeaseQuery_;

    // --- Temporary Addresses ---
    List(TSrvCfgTA) SrvCfgTALst_; // IA_TA list (temporary addresses)

    // --- Prefix Delegation ---
    List(TSrvCfgPD) SrvCfgPDLst_;

    // --- relay ---
    bool Relay_;
    std::string RelayName_;     // name of the underlaying physical interface (or other relay)
    int RelayID_;          // ifindex
    SPtr<TSrvOptInterfaceID> RelayInterfaceID_; // value of interface-id option (optional)

    // --- option: FQDN ---
    List(TFQDN) FQDNLst_;
    int FQDNMode_;
    int RevDNSZoneRootLength_;
    EUnknownFQDNMode UnknownFQDN_;
    std::string FQDNDomain_;

    // --- per-client parameters (exceptions) ---
    List(TSrvCfgOptions) ExceptionsLst_;
};

#endif /* SRVCONFIFACE_H */
