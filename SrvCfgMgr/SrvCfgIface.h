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
#include <vector>
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
    int getPreferedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);
    int getAllowedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);

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

    // CONFIRM support
    EAddrStatus confirmAddress(TIAType type, SPtr<TIPv6Addr> addr);
    bool addrInPool(SPtr<TIPv6Addr> addr);
    bool addrInTaPool(SPtr<TIPv6Addr> addr);
    bool prefixInPdPool(SPtr<TIPv6Addr> addr);

    // subnet management
    void addSubnet(SPtr<TIPv6Addr> min, SPtr<TIPv6Addr> max);
    bool addrInSubnet(SPtr<TIPv6Addr> addr);
    bool subnetDefined();

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
    unsigned int removeReservedFromCache();
    void addClientExceptionsLst(List(TSrvCfgOptions) exLst);
    SPtr<TSrvCfgOptions> getClientException(SPtr<TDUID> duid, TMsg* message, bool quiet=true);
    bool checkReservedPrefix(SPtr<TIPv6Addr> pfx,
                             SPtr<TDUID> duid,
                             SPtr<TOptVendorData> remoteID,
                             SPtr<TIPv6Addr> linkLocal);
    bool addrReserved(SPtr<TIPv6Addr> addr);
    bool prefixReserved(SPtr<TIPv6Addr> prefix);

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

    // following methods are used by out-of-pool reservations (others are using
    // methods from pool (SrvCfgAddrClass)
    uint32_t getT1(uint32_t proposal);
    uint32_t getT2(uint32_t proposal);
    uint32_t getPref(uint32_t proposal);
    uint32_t getValid(uint32_t proposal);
private:
    uint32_t chooseTime(uint32_t min, uint32_t max, uint32_t proposal);

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

    // --- subnets ---
    std::vector<THostRange> Subnets_;

    // --- relay ---
    bool Relay_;
    std::string RelayName_;     // name of the underlaying physical interface (or other relay)
    int RelayID_;          // ifindex (-1 means this is not a relay)
    SPtr<TSrvOptInterfaceID> RelayInterfaceID_; // value of interface-id option (optional)

    // --- option: FQDN ---
    List(TFQDN) FQDNLst_;
    int FQDNMode_;
    int RevDNSZoneRootLength_;
    EUnknownFQDNMode UnknownFQDN_;
    std::string FQDNDomain_;

    // --- per-client parameters (exceptions) ---
    List(TSrvCfgOptions) ExceptionsLst_;
    uint32_t T1Min_;
    uint32_t T1Max_;
    uint32_t T2Min_;
    uint32_t T2Max_;
    uint32_t PrefMin_;
    uint32_t PrefMax_;
    uint32_t ValidMin_;
    uint32_t ValidMax_;
};

#endif /* SRVCONFIFACE_H */
