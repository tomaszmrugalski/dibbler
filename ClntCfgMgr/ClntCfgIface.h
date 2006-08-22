/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.h,v 1.12 2006-08-22 00:01:19 thomson Exp $
 */

#ifndef CLNTCFGIFACE_H
#define CLNTCFGIFACE_H

#include <iostream>
#include <iomanip>
#include <sstream>

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

using namespace std;

class TClntCfgIface
{
	friend std::ostream& operator<<(std::ostream&,TClntCfgIface&);
public:
    TClntCfgIface(string ifaceName);
    TClntCfgIface(int ifaceNr);

    bool isServerRejected(SmartPtr<TIPv6Addr> addr,SmartPtr<TDUID> duid);

    // IA
    void firstIA();
    int countIA();
    SmartPtr<TClntCfgIA> getIA();
    void addIA(SmartPtr<TClntCfgIA> ptr);
    
    // TA
    void firstTA();
    void addTA(SmartPtr<TClntCfgTA> ta);
    SmartPtr<TClntCfgTA> getTA();
    int countTA();
#if 0
    SmartPtr<TClntCfgGroup> getLastGroup();
#endif

    string getName(void);
    string getFullName(void);
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);
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

    void setPrefixLength(int len);
    int  getPrefixLength();

private:
    string IfaceName;
    int ID;
    bool NoConfig;
    bool isIA;
    bool Unicast;
    bool RapidCommit;
    int  PrefixLength; // default prefix length of the configured addresses

    List(TClntCfgIA) IALst;

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
};

#endif 
