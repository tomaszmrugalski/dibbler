/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgIface.h,v 1.15 2006-07-03 18:13:45 thomson Exp $
 *                                                                           
 */

class TSrvCfgIface;
#ifndef SRVCONFIFACE_H
#define SRVCONFIFACE_H
#include "DHCPConst.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgTA.h"
#include "SrvParsGlobalOpt.h"
#include <iostream>
#include <string>
using namespace std;

class TSrvCfgIface
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

    void addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass);
    void firstAddrClass();
    bool getPreferedAddrClassID(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    bool getAllowedAddrClassID(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, unsigned long &classid);
    SmartPtr<TSrvCfgAddrClass> getAddrClass();
    SmartPtr<TSrvCfgAddrClass> getClassByID(unsigned long id);
    SmartPtr<TSrvCfgAddrClass> getRandomClass(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr);
    long countAddrClass();

    void addTA(SmartPtr<TSrvCfgTA> ta);
    void firstTA();
    SmartPtr<TSrvCfgTA> getTA();
    SmartPtr<TSrvCfgTA> getTA(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr);

    SmartPtr<TIPv6Addr> getUnicast();

    void setNoConfig();
    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    
    unsigned char getPreference();

    bool getRapidCommit();

    long getIfaceMaxLease();
    unsigned long getClntMaxLease();

    // IA address functions
    void addClntAddr(SmartPtr<TIPv6Addr> ptrAddr);
    void delClntAddr(SmartPtr<TIPv6Addr> ptrAddr);

    // TA address functions
    void addTAAddr();
    void delTAAddr();

    string getRelayName();
    int getRelayID();
    int getRelayInterfaceID();
    bool isRelay();
    void setRelayName(string name);
    void setRelayID(int id);

    // options
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
    List(TFQDN) * getFQDNLst();
    SPtr<TFQDN> getFQDNName(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr);
    SmartPtr<TDUID> getFQDNDuid(string name);
    void setFQDNLst(List(TFQDN) * fqdn);
    int getFQDNMode();
    void setFQDNMode(int FQDNMode);
    bool supportFQDN();

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
    
private:
    unsigned char preference;
    int	ID;
    string Name;
    bool NoConfig;
    SmartPtr<TIPv6Addr> Unicast;
    unsigned long IfaceMaxLease;
    unsigned long ClntMaxLease;
    bool RapidCommit;	
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst; // IA_NA list (normal addresses)
    List(TSrvCfgTA) SrvCfgTALst; // IA_TA list (temporary addresses)

    // relay
    bool Relay;
    string RelayName;     // name of the underlaying physical interface (or other relay)
    int RelayID;          // ifindex
    int RelayInterfaceID; // value of interface-id option (optional)

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

    List(TIPv6Addr) DNSServerLst;
    List(string) DomainLst;			
    List(TIPv6Addr) NTPServerLst;
    string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(string) SIPDomainLst;
    List(TFQDN) FQDNLst;
    int FQDNMode;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
    unsigned int Lifetime;
};

#endif /* SRVCONFIFACE_H */
