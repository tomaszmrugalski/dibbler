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


unsigned long TSrvCfgAddrClass::getT1(long clntT1) {
    return chooseTime(T1Beg,T1End,clntT1);
}

unsigned long TSrvCfgAddrClass::getT2(long clntT2) {
    return chooseTime(T2Beg,T2End,clntT2);
}

unsigned long TSrvCfgAddrClass::getPref(long clntPref) {
    return chooseTime(PrefBeg,PrefEnd,clntPref);
}

unsigned long TSrvCfgAddrClass::getValid(long clntValid) {
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
        if ((double(sum)+double(ptrRange->rangeCount()))>DHCPV6_INFINITY)
            return DHCPV6_INFINITY;
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

unsigned long TSrvCfgAddrClass::getMaxLease() {
    if (this->countAddrInPool()>MaxLease)
        return MaxLease;
    else
        return countAddrInPool();
}

unsigned long TSrvCfgAddrClass::getMaxClientLease()
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
    out << "    <class>" << std::endl;
    out << "      <T1 min=\"" << addrClass.T1Beg << "\" max=\"" << addrClass.T1End  
	<< "\" />" << logger::endl;
    out << "      <T2 min=\"" << addrClass.T2Beg << "\" max=\"" << addrClass.T2End  
	<< "\" />" << logger::endl;
    out << "      <pref min=\"" << addrClass.PrefBeg << "\" max=\""<< addrClass.PrefEnd  
	<< "\" />" <<logger::endl;
    out << "      <valid min=\"" << addrClass.ValidBeg << "\" max=\""<< addrClass.ValidEnd
	<< "\" />" << logger::endl;
    out << "      <MaxClientLease>" << addrClass.MaxClientLease << "</MaxClientLease>" 
	<< logger::endl;
    out << "      <MaxLease>" << addrClass.MaxLease << "</MaxLease>" 
	<< logger::endl;
    
    out << "      <preference>" << (int)addrClass.Preference << "</preference>" << logger::endl;
    if (addrClass.RapidCommit) {
	out << "      <rapid-commit/>" << logger::endl;
    }
    if (addrClass.Unicast) {
	out <<   "    <unicast/>" << logger::endl;
    }
    
    SmartPtr<TStationRange> statRange;
    out << "      <!-- ranges:" << addrClass.Pool.count() << " -->" << logger::endl;
    addrClass.Pool.first();
    while(statRange=addrClass.Pool.get())
	out << *statRange;
    
    out << "      <!-- rejected ranges:" << addrClass.RejedClnt.count() << " -->" << logger::endl;
    addrClass.RejedClnt.first();
    while(statRange=addrClass.RejedClnt.get())
	out << *statRange;	
	
    out << "      <!-- accepted ranges:" << addrClass.AcceptClnt.count() << " -->" << logger::endl;
    addrClass.AcceptClnt.first();
    while(statRange=addrClass.AcceptClnt.get())
	out << *statRange;
    out << "    </class>" << std::endl;
    return out;
}
