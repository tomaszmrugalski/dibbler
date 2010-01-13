/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 only licence                                */

#include "ClntParsIAOpt.h"
#include <limits.h>

TClntParsIAOpt::TClntParsIAOpt() : TClntParsAddrOpt()
{
    T1=ULONG_MAX;
    T2=ULONG_MAX;
    IAIDCnt=0;
    AddrHint=true;
    AddrParams = false;
}

long TClntParsIAOpt::getT1()
{
    return T1;
}
void TClntParsIAOpt::setT1(long T1)
{
    this->T1=T1;
}

long TClntParsIAOpt::getT2()
{
    return this->T2;
}

void TClntParsIAOpt::setT2(long T2)
{
    this->T2=T2;
}

long TClntParsIAOpt::getIAIDCnt()
{
    return this->IAIDCnt;
}

void TClntParsIAOpt::setIAIDCnt(long cnt)
{
    this->IAIDCnt=cnt;
}

bool TClntParsIAOpt::getAddrHint()
{
    return this->AddrHint;
}

void TClntParsIAOpt::setAddrHint(bool addrHint)
{
    this->AddrHint=addrHint;
}

void TClntParsIAOpt::addPrefSrv(SPtr<TStationID> prefSrv)
{
    this->PrefSrv.append(prefSrv);
}

void TClntParsIAOpt::firstPrefSrv()
{
    this->PrefSrv.first();
}

SPtr<TStationID> TClntParsIAOpt::getPrefSrv()
{
    return this->PrefSrv.get();
}
void TClntParsIAOpt::clearPrefSrv()
{
    this->PrefSrv.clear();
}

void TClntParsIAOpt::setPrefSrvLst(TContainer<SPtr<TStationID> > *lst)
{
    SPtr<TStationID> id;
    this->PrefSrv.clear();
    lst->first();
    while(id=lst->get())
        this->PrefSrv.append(id);
}

void TClntParsIAOpt::addRejedSrv(SPtr<TStationID> prefSrv)
{
    this->RejedSrv.append(prefSrv);
}

void TClntParsIAOpt::firstRejedSrv()
{
    this->RejedSrv.first();
}

void TClntParsIAOpt::clearRejedSrv()
{
    this->RejedSrv.clear();
}
void TClntParsIAOpt::setRejedSrvLst(TContainer<SPtr<TStationID> > *lst)
{
    SPtr<TStationID> id;
    this->RejedSrv.clear();
    lst->first();
    while(id=lst->get())
        this->RejedSrv.append(id);
}

SPtr<TStationID> TClntParsIAOpt::getRejedSrv()
{
    return this->RejedSrv.get();
}

void TClntParsIAOpt::setAddrParams(bool useAddrParams)
{
    AddrParams = useAddrParams;
}

bool TClntParsIAOpt::getAddrParams()
{
    return AddrParams;
}
