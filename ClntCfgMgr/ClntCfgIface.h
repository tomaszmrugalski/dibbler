/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgIface.h,v 1.4 2004-10-02 13:11:24 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    
    ESendOpt getDNSSendOpt();
    EReqOpt  getDNSReqOpt();

    ESendOpt getNTPSendOpt();
    EReqOpt  getNTPReqOpt();
    
    ESendOpt getDomainSendOpt();
    EReqOpt  getDomainReqOpt();
    
    ESendOpt getTimeZoneSendOpt();
    EReqOpt  getTimeZoneReqOpt();
    
    bool isReqDNSSrv();
    bool isReqNTPSrv();
    bool isReqDomainName();
    bool isReqTimeZone();
    
    EState getDNSSrvState();
    EState getNTPSrvState();
    EState getDomainNameState();
    EState getTimeZoneState();

    void setDNSSrvState(EState state);
    void setNTPSrvState(EState state);
    void setDomainNameState(EState state);
    void setTimeZoneState(EState state);

    void   setDNSSrv(TContainer<SmartPtr<TIPv6Addr> > newSrvLst,
                     SmartPtr<TDUID> duid);
    void   setNTPSrv(TContainer<SmartPtr<TIPv6Addr> > newSrvLst, 
                     SmartPtr<TDUID> duid);
    void   setDomainName(string domainName,
                     SmartPtr<TDUID> duid);
    void   setTimeZone(string timeZone,
                     SmartPtr<TDUID> duid);
    
    TContainer<SmartPtr<TIPv6Addr> > getProposedDNSSrv();
    TContainer<SmartPtr<TIPv6Addr> > getProposedNTPSrv();
    string getProposedDomainName();
    string getProposedTimeZone();

    bool onlyInformationRequest();
    bool getUnicast();
private:
    string      IfaceName;
    int	        ID;
    bool        NoConfig;
    
    EReqOpt DNSReqOpt;
    EReqOpt NTPReqOpt;
    
    ESendOpt DNSSendOpt;
    ESendOpt NTPSendOpt;
    
    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;
    TContainer<SmartPtr<TIPv6Addr> > AppDNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > AppNTPSrv;
    TContainer<SmartPtr<TIPv6Addr> > PrepDNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > PrepNTPSrv;

    bool        ReqDomainName;
    bool        ReqTimeZone;
    bool        ReqDNSSrv;
    bool        ReqNTPSrv;
    
    string      TZone;
    ESendOpt    TimeZoneSendOpt;
    EReqOpt	    TimeZoneReqOpt;
    
    string	    Domain;
    ESendOpt    DomainSendOpt;
    EReqOpt	    DomainReqOpt;
    
    EState      TimeZoneState;
    EState      DomainNameState;
    EState      DNSSrvState;
    EState      NTPSrvState;

    bool Unicast;
    
    TContainer<SmartPtr<TIPv6Addr> > ReceivedDNSSrv;
    SmartPtr<TDUID>                  GiverDNSSrvDUID;
    SmartPtr<TIPv6Addr>              GiverDNSSrvAddr;
    TContainer<SmartPtr<TIPv6Addr> > ReceivedNTPSrv;
    SmartPtr<TDUID>                  GiverNTPSrvDUID;
    SmartPtr<TIPv6Addr>              GiverNTPSrvAddr;
    string                           ReceivedDomainName;
    SmartPtr<TDUID>                  GiverDomainDUID;
    SmartPtr<TIPv6Addr>              GiverDomainAddr;
    string                           ReceivedTimeZone;
    SmartPtr<TDUID>                  GiverTimeDUID;
    SmartPtr<TIPv6Addr>              GiverTimeAddr;
    
    bool isIA;
};



#endif 
