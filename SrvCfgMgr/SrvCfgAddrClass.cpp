/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgAddrClass.cpp,v 1.7 2004-06-17 23:53:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#include "SrvCfgAddrClass.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"
 
TSrvCfgAddrClass::TSrvCfgAddrClass() {
    this->T1Beg    = 0;
    this->T1End    = DHCPV6_INFINITY;
    this->T2Beg    = 0;
    this->T2End    = DHCPV6_INFINITY;
    this->PrefBeg  = 0;
    this->PrefEnd  = DHCPV6_INFINITY;
    this->ValidBeg = 0;
    this->ValidEnd = DHCPV6_INFINITY;
}

TSrvCfgAddrClass::~TSrvCfgAddrClass() {
}

/*
 * is client allowed to use this class? (it can be rejected on DUID or address basis)
 */
bool TSrvCfgAddrClass::clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
    SmartPtr<TStationRange> range;
    RejedClnt.first();
    // is client on black list?
    while(range=RejedClnt.get())
        if (range->in(duid,clntAddr))
            return false;

    if (AcceptClnt.count()) {
	// there's white list
        while(range=AcceptClnt.get()) {
	    // is client on this white list?
            if (range->in(duid,clntAddr))
                return true;
        }
        return false;
    } else {
        return true;
    }
}

/* FIXME: is this thing work? */
long TSrvCfgAddrClass::getAddrCount(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
    if ( ClntMaxLease < ClassMaxLease )
        return ClntMaxLease;
    else
        return ClassMaxLease;
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
    
    ClntMaxLease  = opt->getClntMaxLease();
    ClassMaxLease = opt->getClassMaxLease();
    IfaceMaxLease = opt->getIfaceMaxLease();
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

SmartPtr<TIPv6Addr> TSrvCfgAddrClass::getFreeAddr(SmartPtr<TSrvAddrMgr> addrMgr,
						  SmartPtr<TDUID> clntDuid,
						  SmartPtr<TIPv6Addr> clntAddr,
						  SmartPtr<TIPv6Addr> hint) {
    long cnt=0;
    SmartPtr<TIPv6Addr> ptrAddr = hint;

    // FIXME: after 1000 tries, return 0
    while (!addrMgr->addrIsFree(ptrAddr) && (cnt<1000) ) {
	ptrAddr = this->getRandomAddr();
	cnt++;
    }
    if (cnt==1000) 
	return 0;

    return ptrAddr;
}

unsigned long TSrvCfgAddrClass::getMaxLease() {
    unsigned long addrsInPool = this->countAddrInPool();
    if (addrsInPool>ClassMaxLease)
        return ClassMaxLease;
    else
        return addrsInPool;
}

unsigned long TSrvCfgAddrClass::getMaxClientLease()
{
    if (this->countAddrInPool()>ClntMaxLease)
        return this->ClntMaxLease;
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
    out << "      <ClntMaxLease>" << addrClass.ClntMaxLease << "</ClntMaxLease>" << logger::endl;
    out << "      <ClassMaxLease>" << addrClass.ClassMaxLease << "</ClassMaxLease>" << logger::endl;
    out << "      <IFaceMaxLease>" << addrClass.IfaceMaxLease << "</IfaceMaxLease>" << logger::endl;
    
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
