/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *changes : Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
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
    
    bool getAddrHint();
    void setAddrHint(bool addrHint);
    
    void addPrefSrv(SPtr<TStationID> prefSrv);
    void firstPrefSrv();
    SPtr<TStationID> getPrefSrv();
    void clearPrefSrv();
    void setPrefSrvLst(TContainer<SPtr<TStationID> > *lst);
    
    void addRejedSrv(SPtr<TStationID> prefSrv);
    void firstRejedSrv();
    SPtr<TStationID> getRejedSrv();
    void clearRejedSrv();
    void setRejedSrvLst(TContainer<SPtr<TStationID> > *lst);

    void setAddrParams(bool useAddrParams);
    bool getAddrParams();

private:
    long T1;
    long T2;
    
    long IAIDCnt;
    bool AddrHint;
    List(TStationID) PrefSrv;
    List(TStationID) RejedSrv;

    bool AddrParams;
};

#endif
