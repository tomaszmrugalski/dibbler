#include "SrvCfgAddrClass.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "Logger.h"
 
TSrvCfgAddrClass::TSrvCfgAddrClass()
{
	//FIXME:here should be set default values of all fields
	//except pool, which can be set only by the user
}


TSrvCfgAddrClass::~TSrvCfgAddrClass()
{
}

char TSrvCfgAddrClass::getPreference()
{
    return Preference;
}

bool TSrvCfgAddrClass::clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
	SmartPtr<TStationRange> range;
    RejedClnt.first();
    while(range=RejedClnt.get())
        if (range->in(duid,clntAddr))
            return false;
    if (AcceptClnt.count())
    {
        while(range=AcceptClnt.get())
        {
            if (range->in(duid,clntAddr))
                return true;
        }
        return false;
    }
    else
        return true;
}

long TSrvCfgAddrClass::getAddrCount(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
    if (MaxClientLease<MaxLease)
        return MaxLease;
    else
        return MaxClientLease;
}

long TSrvCfgAddrClass::chooseTime(long beg, long end, long clntTime)
{
    if (beg<clntTime) 
        if (clntTime<end)
            return clntTime;
        else
            return end;
    else
        return beg;
}


long TSrvCfgAddrClass::getT1(long clntT1)
{
    return chooseTime(T1Beg,T1End,clntT1);
}

long TSrvCfgAddrClass::getT2(long clntT2)
{
    return chooseTime(T2Beg,T2End,clntT2);
}

long TSrvCfgAddrClass::getPref(long clntPref)
{
    return chooseTime(PrefBeg,PrefEnd,clntPref);
}

long TSrvCfgAddrClass::getValid(long clntValid)
{
    return chooseTime(ValidBeg,ValidEnd,clntValid);
}

void TSrvCfgAddrClass::setOptions(SmartPtr<TSrvParsGlobalOpt> opt)
{
    T1Beg=opt->getT1Beg();
    T2Beg=opt->getT2Beg();
    T1End=opt->getT1End();
    T2End=opt->getT2End();
    PrefBeg=opt->getPrefBeg();
    PrefEnd=opt->getPrefEnd();
    ValidBeg=opt->getValidBeg();
    ValidEnd=opt->getValidEnd();
    
	MaxClientLease=opt->getMaxClientLease();
	MaxLease=opt->getMaxLeases();
    Preference=opt->getPreference();
    RapidCommit=opt->getRapidCommit();
    Unicast=opt->getUnicast();
	//NISServer=opt.NISServer;

	SmartPtr<TStationRange> statRange;
    opt->firstRejedClnt();
    while(statRange=opt->getRejedClnt())
        this->RejedClnt.append(statRange);
    
    opt->firstAcceptClnt();
    while(statRange=opt->getAcceptClnt())
        this->AcceptClnt.append(statRange);
	
    opt->firstPool();
    while(statRange=opt->getPool())
	    	this->Pool.append(statRange);

}

bool TSrvCfgAddrClass::addrInPool(SmartPtr<TIPv6Addr> addr)
{
    Pool.first();
    SmartPtr<TStationRange> ptrRange;
    while(ptrRange=Pool.get())
        if (ptrRange->in(addr))
            return true;
    return false;
}

unsigned long TSrvCfgAddrClass::countAddrInPool()
{
    Pool.first();
    SmartPtr<TStationRange> ptrRange;
    unsigned long sum=0;
    while(ptrRange=Pool.get())
    {
        if ((double(sum)+double(ptrRange->rangeCount()))>INFINITY)
            return INFINITY;
        else
            sum+=ptrRange->rangeCount();
    }
    return sum;
}

SmartPtr<TIPv6Addr> TSrvCfgAddrClass::getRandomAddr()
{
    int poolNr=rand()%Pool.count();
    SmartPtr<TStationRange> range;
    Pool.first();
    while((range=Pool.get())&&(poolNr>0)) poolNr--;
    return range->getRandomAddr();
}

long TSrvCfgAddrClass::getMaxLease()
{
    if (this->countAddrInPool()>MaxLease)
        return MaxLease;
    else
        return countAddrInPool();
}

long TSrvCfgAddrClass::getMaxClientLease()
{
    if (this->countAddrInPool()>MaxClientLease)
        return this->MaxClientLease;
    else
        return countAddrInPool();
}

bool TSrvCfgAddrClass::getRapidCommit()
{
    return this->RapidCommit;
}

ostream& operator<<(ostream& out,TSrvCfgAddrClass& addrClass)
{
	cout<<"T1:"<<addrClass.T1Beg<<"- "<< addrClass.T1End  << logger::endl;
    cout<<"T2:"<<addrClass.T2Beg<<"- "<< addrClass.T2End  << logger::endl;
    cout<<"Preferred:"<<addrClass.PrefBeg<<"- "<< addrClass.PrefEnd  << logger::endl;
	cout<<"Valid:"<<addrClass.ValidBeg<<"- "<< addrClass.ValidEnd  << logger::endl;
	cout<<"MaxClientLease:"<<addrClass.MaxClientLease << logger::endl;
	cout<<"MaxLease:"<<addrClass.MaxLease << logger::endl;
	    
	cout<<"Preference:"<<(int)addrClass.Preference << logger::endl;
	cout<<"RapidCommit:"<<addrClass.RapidCommit << logger::endl;
	cout<<"Unicast:"<<addrClass.Unicast << logger::endl;
//	cout<<"NISServer:"<<addrClass.NISServer << logger::endl;
	
    SmartPtr<TStationRange> statRange;
	cout<<"Liczba zakresow w puli adresowej:"<<addrClass.Pool.count() << logger::endl;
	addrClass.Pool.first();
	while(statRange=addrClass.Pool.get())
		cout<<*statRange << logger::endl;
	
    cout<<"Liczba zakresow w odrzuconych:"<<addrClass.RejedClnt.count() << logger::endl;
	addrClass.RejedClnt.first();
	while(statRange=addrClass.RejedClnt.get())
		cout<<*statRange << logger::endl;	
	
    cout<<"Liczba zakresow akceptowanych:"<<addrClass.AcceptClnt.count() << logger::endl;
	addrClass.AcceptClnt.first();
	while(statRange=addrClass.AcceptClnt.get())
		cout<<*statRange << logger::endl;
	return out;
}
