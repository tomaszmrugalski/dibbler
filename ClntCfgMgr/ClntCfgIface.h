#ifndef CLNTCFGIFACE_H_HEADER_INCLUDED_C0FD9791
#define CLNTCFGIFACE_H_HEADER_INCLUDED_C0FD9791

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
    TContainer< SmartPtr<TClntCfgGroup> > ClntCfgGroupLst;

    void firstGroup();
    int countGroup();

    SmartPtr<TClntCfgGroup> getGroup();
    void addGroup(SmartPtr<TClntCfgGroup> ptr);

	string getName(void);
    
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);
    SmartPtr<TClntCfgGroup> getLastGroup();

	int	getID(void);
	
	TClntCfgIface(string ifaceName);

    TClntCfgIface(int ifaceNr);

	void setNoConfig();

    void setIfaceID(int ifaceID);
	void setIfaceName(string ifaceName);

	bool isNoConfig() {return NoConfig;};

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
private:
    string      IfaceName;
	int	        ID;
	bool        NoConfig;

    EReqOpt DNSReqOpt;
    EReqOpt NTPReqOpt;
    //EReqOpt NISReqOpt;

    ESendOpt DNSSendOpt;
   	ESendOpt NTPSendOpt;
    //ESendOpt NISOptions;

    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;
    //string NISServer;
    TContainer<SmartPtr<TIPv6Addr> > AppDNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > AppNTPSrv;
    //string NISApp;
    TContainer<SmartPtr<TIPv6Addr> > PrepDNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > PrepNTPSrv;
    //string NISPrep;

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
