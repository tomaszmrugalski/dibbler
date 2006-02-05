/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include "ClntParsIAOpt.h"
#include <limits.h>

TClntParsIAOpt::TClntParsIAOpt() : TClntParsAddrOpt()
{
    T1=ULONG_MAX;
    T2=ULONG_MAX;
    IAIDCnt=0;
    AddrHint=true;
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

void TClntParsIAOpt::addPrefSrv(SmartPtr<TStationID> prefSrv)
{
    this->PrefSrv.append(prefSrv);
}

void TClntParsIAOpt::firstPrefSrv()
{
    this->PrefSrv.first();
}

SmartPtr<TStationID> TClntParsIAOpt::getPrefSrv()
{
    return this->PrefSrv.get();
}
void TClntParsIAOpt::clearPrefSrv()
{
    this->PrefSrv.clear();
}

void TClntParsIAOpt::setPrefSrvLst(TContainer<SmartPtr<TStationID> > *lst)
{
    SmartPtr<TStationID> id;
    this->PrefSrv.clear();
    lst->first();
    while(id=lst->get())
        this->PrefSrv.append(id);
}

void TClntParsIAOpt::addRejedSrv(SmartPtr<TStationID> prefSrv)
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
void TClntParsIAOpt::setRejedSrvLst(TContainer<SmartPtr<TStationID> > *lst)
{
    SmartPtr<TStationID> id;
    this->RejedSrv.clear();
    lst->first();
    while(id=lst->get())
        this->RejedSrv.append(id);
}

SmartPtr<TStationID> TClntParsIAOpt::getRejedSrv()
{
    return this->RejedSrv.get();
}
