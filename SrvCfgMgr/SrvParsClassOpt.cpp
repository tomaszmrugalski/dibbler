#include <limits.h>
#include "SrvParsClassOpt.h"
#include "DHCPConst.h"

TSrvParsClassOpt::TSrvParsClassOpt(void)
{
    T1Beg    = SERVER_DEFAULT_MIN_T1;
    T1End    = 0x7fffffff;
    T2Beg    = 0x7fffffff;
    T2End    = 0x7fffffff;
    PrefBeg  = 0x7fffffff;
    PrefEnd  = 0x7fffffff;
    ValidBeg = 0x7fffffff;
    ValidEnd = 0x7fffffff;

    ClassMaxLease= ULONG_MAX;
}

//T1,T2,Valid,Prefered time routines
void TSrvParsClassOpt::setT1Beg(long t1)
{
    this->T1Beg=t1;
}

void TSrvParsClassOpt::setT1End(long t1)
{
    this->T1End=t1;
}

long TSrvParsClassOpt::getT1Beg()
{
    return T1Beg;
}

long TSrvParsClassOpt::getT1End()
{
    return T1End;
}

void TSrvParsClassOpt::setT2Beg(long t2)
{
    this->T2Beg=t2;
}

void TSrvParsClassOpt::setT2End(long t2)
{
    this->T2End=t2;
}

long TSrvParsClassOpt::getT2Beg()
{
    return this->T2Beg;
}

long TSrvParsClassOpt::getT2End()
{
    return this->T2End;
}

void TSrvParsClassOpt::setPrefBeg(long pref)
{
    this->PrefBeg=pref;
}

void TSrvParsClassOpt::setPrefEnd(long pref)
{
    this->PrefEnd=pref;
}

long TSrvParsClassOpt::getPrefBeg()
{
    return this->PrefBeg;
}

long TSrvParsClassOpt::getPrefEnd()
{
    return this->PrefEnd;
}

void TSrvParsClassOpt::setValidEnd(long valid)
{
    this->ValidEnd=valid;
}

void TSrvParsClassOpt::setValidBeg(long valid)
{
    this->ValidBeg=valid;
}

long TSrvParsClassOpt::getValidEnd()
{
    return this->ValidEnd;
}

long TSrvParsClassOpt::getValidBeg()
{
    return this->ValidBeg;
}

//Rejected clients access routines
void TSrvParsClassOpt::addRejedClnt(SmartPtr<TStationRange> addr)
{
    this->RejedClnt.append(addr);
}
void TSrvParsClassOpt::firstRejedClnt()
{
    this->RejedClnt.first();
}
SmartPtr<TStationRange> TSrvParsClassOpt::getRejedClnt()
{
    return this->RejedClnt.get();
}
void TSrvParsClassOpt::setRejedClnt(TContainer<SmartPtr<TStationRange> > *rejedClnt)
{
    this->RejedClnt.clear();
    rejedClnt->first();
    SmartPtr<TStationRange> addr;
    while(addr=rejedClnt->get())
        this->RejedClnt.append(addr);
}

//Accepted clients access routines
void TSrvParsClassOpt::addAcceptClnt(SmartPtr<TStationRange> addr)
{
    this->AcceptClnt.append(addr);
}

void TSrvParsClassOpt::firstAcceptClnt()
{
    this->AcceptClnt.first();
}

SmartPtr<TStationRange> TSrvParsClassOpt::getAcceptClnt()
{
    return this->AcceptClnt.get();
}

void TSrvParsClassOpt::setAcceptClnt(TContainer<SmartPtr<TStationRange> > *acceptClnt)
{
    this->AcceptClnt.clear();
    acceptClnt->first();
    SmartPtr<TStationRange> addr;
    while(addr=acceptClnt->get())
        this->AcceptClnt.append(addr);
}

//Pool access routines
void TSrvParsClassOpt::addPool(SmartPtr<TStationRange> addr)
{
    this->Pool.append(addr);
}
void TSrvParsClassOpt::firstPool()
{
    this->Pool.first();
}
SmartPtr<TStationRange> TSrvParsClassOpt::getPool()
{
    return this->Pool.get();
}

void TSrvParsClassOpt::setPool(TContainer<SmartPtr<TStationRange> > *pool)
{
    this->Pool.clear();
    pool->first();
    SmartPtr<TStationRange> addr;
    while(addr=pool->get())
        this->Pool.append(addr);
}

void TSrvParsClassOpt::setClassMaxLease(long classMaxLease) {
    this->ClassMaxLease = classMaxLease;
}


long TSrvParsClassOpt::getClassMaxLease() {
    return this->ClassMaxLease;
}

TSrvParsClassOpt::~TSrvParsClassOpt(void)
{
}

long TSrvParsClassOpt::countPool()
{
    return this->Pool.count();
}
