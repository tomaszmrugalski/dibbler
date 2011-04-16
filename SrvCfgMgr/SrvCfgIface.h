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
 * $Id: SrvCfgIface.h,v 1.31 2008-11-11 22:41:49 thomson Exp $
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

using namespace std;

class TSrvCfgIface: public TSrvCfgOptions
{
    friend ostream& operator<<(ostream& out,TSrvCfgIface& iface);
public:
    TSrvCfgIface();
    TSrvCfgIface(string ifaceName);
    TSrvCfgIface(int ifaceNr);
    virtual ~TSrvCfgIface();
    void setDefaults();

    void setName(string ifaceName);
    void setID(int ifaceID);
    int	getID();
    string getName();
    string getFullName();

    // permanent address management (IA_NA)
    void addAddrClass(SPtr<TSrvCfgAddrClass> addrClass);
    void firstAddrClass();
    bool getPreferedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    bool getAllowedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    SPtr<TSrvCfgAddrClass> getAddrClass();
    SPtr<TSrvCfgAddrClass> getClassByID(unsigned long id);
    SPtr<TSrvCfgAddrClass> getRandomClass(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr);
    long countAddrClass();

    // temporary address management (IA_TA)
    void addTA(SPtr<TSrvCfgTA> ta);
    void firstTA();
    SPtr<TSrvCfgTA> getTA();
    SPtr<TSrvCfgTA> getTA(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);

    // prefix management (IA_PD)
    void addPDClass(SPtr<TSrvCfgPD> PDClass);
    SPtr<TSrvCfgPD> getPDByID(unsigned long id);
    //SPtr<TSrvCfgPD> getRandomPrefix(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr);
    long countPD();
    void addPD(SPtr<TSrvCfgPD> pd);
    void firstPD();
    SPtr<TSrvCfgPD> getPD();
    bool addClntPrefix(SPtr<TIPv6Addr> ptrPD);
    bool delClntPrefix(SPtr<TIPv6Addr> ptrPD);
    bool supportPrefixDelegation();

    // other
    SPtr<TIPv6Addr> getUnicast();
    void setNoConfig();
    void setOptions(SPtr<TSrvParsGlobalOpt> opt);

    unsigned char getPreference();

    bool getRapidCommit();

    long getIfaceMaxLease();
    unsigned long getClntMaxLease();

    // IA address functions
    void addClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet = false);
    void delClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet = false);

    // TA address functions
    void addTAAddr();
    void delTAAddr();

    // relays
    string getRelayName();
    int getRelayID();
    SPtr<TSrvOptInterfaceID> getRelayInterfaceID();
    bool isRelay();
    void setRelayName(string name);
    void setRelayID(int id);

    // per-client parameters (exceptions)
    void addClientExceptionsLst(List(TSrvCfgOptions) exLst);
    SPtr<TSrvCfgOptions> getClientException(SPtr<TDUID> duid, SPtr<TOptVendorData> remoteID, bool quiet=true);

    // option: FQDN
    List(TFQDN) * getFQDNLst();
    SPtr<TFQDN> getFQDNName(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr, string hint);
    SPtr<TDUID> getFQDNDuid(string name);
    void setFQDNLst(List(TFQDN) * fqdn);
    int getFQDNMode();
    string getFQDNModeString();
    int  getRevDNSZoneRootLength();
    void setRevDNSZoneRootLength(int revDNSZoneRootLength);
    bool supportFQDN();
    bool leaseQuerySupport();

    void mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst);

private:
    unsigned char preference;
    int	ID;
    string Name;
    bool NoConfig;
    SPtr<TIPv6Addr> Unicast;
    unsigned long IfaceMaxLease;
    unsigned long ClntMaxLease;
    bool RapidCommit;
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst; // IA_NA list (normal addresses)
    bool LeaseQuery;

    // --- Temporary Addresses ---
    List(TSrvCfgTA) SrvCfgTALst; // IA_TA list (temporary addresses)

    // --- Prefix Delegation ---
    List(TSrvCfgPD) SrvCfgPDLst;
    bool PrefixDelegationSupport;

    // --- relay ---
    bool Relay;
    string RelayName;     // name of the underlaying physical interface (or other relay)
    int RelayID;          // ifindex
    SPtr<TSrvOptInterfaceID> RelayInterfaceID; // value of interface-id option (optional)

    // --- option: FQDN ---
    List(TFQDN) FQDNLst;
    int FQDNMode;
    int revDNSZoneRootLength;
    unsigned int PrefixLength;
    EUnknownFQDNMode UnknownFQDN;
    std::string FQDNDomain;

    // --- per-client parameters (exceptions) ---
    List(TSrvCfgOptions) ExceptionsLst;
};

#endif /* SRVCONFIFACE_H */
