#ifndef TPARSEIFACEOPT_H_
#define TPARSEIFACEOPT_H_

#include "DHCPConst.h"
#include "Container.h"

#include "ClntParsIAOpt.h"
#include "StationID.h"
#include "IPv6Addr.h"

#include <iostream>
#include <string>
using namespace std;


class TClntParsIfaceOpt : public TClntParsIAOpt
{
public:
    TClntParsIfaceOpt();

    //-- DNS connected methods --
    void addDNSSrv(SmartPtr<TIPv6Addr> addr);
    void firstDNSSrv();
    SmartPtr<TIPv6Addr> getDNSSrv();
    void setDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    void addAppDNSSrv(SmartPtr<TIPv6Addr> addr);
    void firstAppDNSSrv();
    SmartPtr<TIPv6Addr> getAppDNSSrv();
    void setAppDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    void addPrepDNSSrv(SmartPtr<TIPv6Addr> addr);
    void firstPrepDNSSrv();
    SmartPtr<TIPv6Addr> getPrepDNSSrv();
    void setPrepDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    EReqOpt getDNSSrvReqOpt();
    void setDNSSrvReqOpt(EReqOpt opt);
    ESendOpt getDNSSrvSendOpt();
    void setDNSSrvSendOpt(ESendOpt opt);

    string getDomain();
    void setDomain(string Domain);
    EReqOpt getDomainReqOpt();
    void setDomainReqOpt(EReqOpt opt);
    ESendOpt getDomainSendOpt();
    void setDomainSendOpt(ESendOpt opt);

    //-- NTP connected methods --
    void addNTPSrv(SmartPtr<TIPv6Addr> addr);
    void firstNTPSrv();
    SmartPtr<TIPv6Addr> getNTPSrv();
    void setNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    void addAppNTPSrv(SmartPtr<TIPv6Addr> addr);
    void firstAppNTPSrv();
    SmartPtr<TIPv6Addr> getAppNTPSrv();
    void setAppNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    void addPrepNTPSrv(SmartPtr<TIPv6Addr> addr);
    void firstPrepNTPSrv();
    SmartPtr<TIPv6Addr> getPrepNTPSrv();
    void setPrepNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst);

    EReqOpt getNTPSrvReqOpt();
    void setNTPSrvReqOpt(EReqOpt opt);
    ESendOpt getNTPSrvSendOpt();
    void setNTPSrvSendOpt(ESendOpt opt);

    string getTimeZone();
    void setTimeZone(string timeZone);
    ESendOpt getTimeZoneSendOpt();
    void setTimeZoneSendOpt(ESendOpt opt);
    EReqOpt getTimeZoneReqOpt();
    void setTimeZoneReqOpt(EReqOpt opt);
    
    bool getReqDNSSrv();
    bool getReqNTPSrv();
    bool getReqDomainName();
    bool getReqTimeZone();

    void setReqDNSSrv(bool req);
    void setReqNTPSrv(bool req);
    void setReqDomainName(bool req);
    void setReqTimeZone(bool req);

    bool isNewGroup();
    void setNewGroup(bool newGr);
    
    bool getIsIAs();
    void setIsIAs(bool state);

private:
    //DNS connected options
    TContainer<SmartPtr<TIPv6Addr> >	DNSSrv;
    TContainer<SmartPtr<TIPv6Addr> >	AppDNSSrv;
    TContainer<SmartPtr<TIPv6Addr> >	PrepDNSSrv;

    EReqOpt								DNSSrvReqOpt;
    ESendOpt							DNSSrvSendOpt;

    string								Domain;
    ESendOpt							DomainSendOpt;
    EReqOpt								DomainReqOpt;

    //NTP connected options
    TContainer<SmartPtr<TIPv6Addr> >	NTPSrv;
    TContainer<SmartPtr<TIPv6Addr> >	AppNTPSrv;
    TContainer<SmartPtr<TIPv6Addr> >	PrepNTPSrv;
    EReqOpt								NTPSrvReqOpt;
    ESendOpt							NTPSrvSendOpt;

    string								TZone;
    ESendOpt							TimeZoneSendOpt;
    EReqOpt								TimeZoneReqOpt;

    bool                                ReqDNSSrv;
    bool                                ReqNTPSrv;
    bool                                ReqDomainName;
    bool                                ReqTimeZone;
    
    //FIXME: Maybe in DHCPv6 1.1
    //NIS connected options
    //string						    NISSrv;
    //string							NISAppSrv;
    //string							NISPrepSrv;
    //EReqOpt							NISSrvReqOpt;
    //ESendOpt							NISSrvSendOpt;

    bool NewGroup;//indicates whether new group should be created
    bool NoIAs;
};


#endif
