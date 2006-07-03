/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.h,v 1.9 2006-07-03 18:19:12 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2006-03-03 21:09:34  thomson
 * FQDN support added.
 *
 * Revision 1.7  2005/01/03 21:57:08  thomson
 * Relay support added.
 *
 * Revision 1.6  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.5  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.4  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.3  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 */

#ifndef TSRCPARSIFACEOPT_H_
#define TSRCPARSIFACEOPT_H_
#include "SrvParsClassOpt.h"
#include "FQDN.h"

class TSrvParsIfaceOpt : public TSrvParsClassOpt
{
public:
    TSrvParsIfaceOpt(void);
    ~TSrvParsIfaceOpt(void);

    bool uniAddress();
    void setUniAddress(bool isUni);
    void setClntMaxLease(long maxLeases);
    long getClntMaxLease();
    void setIfaceMaxLease(long maxLease);
    long getIfaceMaxLease();

    void setPreference(char pref);
    char getPreference();

    void setRapidCommit(bool rapidComm);
    bool getRapidCommit();

    void setUnicast(SmartPtr<TIPv6Addr> addr);
    SmartPtr<TIPv6Addr> getUnicast();

    void setRelayName(string name);
    void setRelayID(int ifindex);
    void setRelayInterfaceID(int id);
    string getRelayName();
    int getRelayID();
    int getRelayInterfaceID();
    bool isRelay();

    //-- options related methods --
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
    List(TFQDN) *getFQDNLst();
    string getFQDNName(SmartPtr<TDUID> duid);
    string getFQDNName(SmartPtr<TIPv6Addr> addr);
    string getFQDNName();
    SmartPtr<TDUID> getFQDNDuid(string name);
    void setFQDNLst(List(TFQDN) *fqdn);
    bool supportFQDN();

    int getFQDNMode();
    void setFQDNMode(int FQDNMode);

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
    char Preference;
    bool RapidCommit;
    long IfaceMaxLease;
    long ClntMaxLease;
    SmartPtr<TIPv6Addr> Unicast;

    // relay
    bool Relay;
    string RelayName;
    int RelayID;
    int RelayInterfaceID;

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
    int FQDNMode;
    unsigned int Lifetime;
};

#endif
