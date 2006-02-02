/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#ifndef PARSIAOPT_H_
#define PARSIAOPT_H_

#include "StationID.h"

#include "Container.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "ClntParsAddrOpt.h"
#include "IPv6Addr.h"


class TClntParsIAOpt : public TClntParsAddrOpt
{
 public:
    TClntParsIAOpt();
    
    long getT1();
    void setT1(long T1);
    
    long getT2();
    void setT2(long T2);
    
    long getIAIDCnt();
    void setIAIDCnt(long cnt);
    long getIncedIAIDCnt();
    
    bool getAddrHint();
    void setAddrHint(bool addrHint);
    
    void addPrefSrv(SmartPtr<TStationID> prefSrv);
    void firstPrefSrv();
    SmartPtr<TStationID> getPrefSrv();
    void clearPrefSrv();
    void setPrefSrvLst(TContainer<SmartPtr<TStationID> > *lst);
    
    void addRejedSrv(SmartPtr<TStationID> prefSrv);
    void firstRejedSrv();
    SmartPtr<TStationID> getRejedSrv();
    void clearRejedSrv();
    void setRejedSrvLst(TContainer<SmartPtr<TStationID> > *lst);
private:
    long T1;
    long T2;
    
    long IAIDCnt;
    bool AddrHint;
    TContainer<SmartPtr<TStationID> > PrefSrv;
    TContainer<SmartPtr<TStationID> > RejedSrv;
};

#endif
