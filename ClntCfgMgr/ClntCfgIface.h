/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.h,v 1.7 2004-11-29 21:21:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.5  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.4  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.3  2004/05/23 20:13:12  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#ifndef CLNTCFGIFACE_H
#define CLNTCFGIFACE_H

#include <iostream>
#include <iomanip>

#include "Container.h"
#include "StationID.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "ClntCfgGroup.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "IPv6Addr.h"

using namespace std;

class TClntCfgIface
{
	friend std::ostream& operator<<(std::ostream&,TClntCfgIface&);
public:
    TClntCfgIface(string ifaceName);
    TClntCfgIface(int ifaceNr);

    TContainer< SmartPtr<TClntCfgGroup> > ClntCfgGroupLst;
    void firstGroup();
    int countGroup();
    SmartPtr<TClntCfgGroup> getGroup();
    void addGroup(SmartPtr<TClntCfgGroup> ptr);
    string getName(void);
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);
    SmartPtr<TClntCfgGroup> getLastGroup();
    int	getID(void);
    void setNoConfig();
    void setIfaceID(int ifaceID);
    void setIfaceName(string ifaceName);
    bool noConfig();

    bool stateless();
    bool getUnicast();

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

private:
    string IfaceName;
    int ID;
    bool NoConfig;
    bool isIA;
    bool Unicast;
    
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
