/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgIface.h,v 1.12 2006-03-03 21:09:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/08/02 00:33:58  thomson
 * White-list bug fixed (bug #120),
 * Minor compilation warnings in gcc 4.0 removed.
 *
 * Revision 1.10  2005/05/02 21:48:42  thomson
 * getFullName() method implemented.
 *
 * Revision 1.9  2005/01/03 21:57:08  thomson
 * Relay support added.
 *
 * Revision 1.8  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.7  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.6  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.5  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 *                                                                           
 */

class TSrvCfgIface;
#ifndef SRVCONFIFACE_H
#define SRVCONFIFACE_H
#include "DHCPConst.h"
#include "SrvCfgAddrClass.h"
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
    SmartPtr<TSrvCfgAddrClass> getRandomClass(SmartPtr<TDUID> clntDuid, 
                                              SmartPtr<TIPv6Addr> clntAddr);
    long countAddrClass();

    SmartPtr<TIPv6Addr> getUnicast();

    void setNoConfig();
    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    
    unsigned char getPreference();

    bool getRapidCommit();

    long getIfaceMaxLease();
    unsigned long getClntMaxLease();

    // assigned address functions
    void addClntAddr(SmartPtr<TIPv6Addr> ptrAddr);
    void delClntAddr(SmartPtr<TIPv6Addr> ptrAddr);

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
    string getFQDNName(SmartPtr<TDUID> addr);
    string getFQDNName(SmartPtr<TIPv6Addr> addr);
    string getFQDNName();
    SmartPtr<TDUID> getFQDNDuid(string name);
    void setFQDNLst(List(TFQDN) * fqdn);
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
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst;

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
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
    unsigned int Lifetime;
};

#endif /* SRVCONFIFACE_H */
