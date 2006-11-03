/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 or later licence
 *                                                                           
 * $Id: SrvCfgPD.cpp,v 1.2 2006-11-03 20:07:07 thomson Exp $
 *
 */

#include "SrvCfgPD.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"

/*
 * static field initialization
 */
unsigned long TSrvCfgPD::staticID=0;

 
TSrvCfgPD::TSrvCfgPD() {
    this->PD_T1Beg    = 0;
    this->PD_T1End    = DHCPV6_INFINITY;
    this->PD_T2Beg    = 0;
    this->PD_T2End    = DHCPV6_INFINITY;
    this->ID = staticID++;
    this->PD_Assigned = 0;
    this->PD_Count = 0;
    this->PD_Length = 0;
}

TSrvCfgPD::~TSrvCfgPD() {
}




long TSrvCfgPD::chooseTime(unsigned long beg, unsigned long end, unsigned long clntTime)
{
    if (clntTime < beg)
	return beg;
    if (clntTime > end)
	return end;
    return clntTime;
}

unsigned long TSrvCfgPD::getPD_T1(unsigned long clntT1) {
    return chooseTime(PD_T1Beg,PD_T1End,clntT1);
}

unsigned long TSrvCfgPD::getPD_T2(unsigned long clntT2) {
    return chooseTime(PD_T2Beg,PD_T2End,clntT2);
}


void TSrvCfgPD::setOptions(SmartPtr<TSrvParsGlobalOpt> opt, int PDPrefix)
{
    Log(Debug) << "Set options for PD T1: " << opt->getT1Beg() << " T2: " << opt->getT2Beg()<< " Prefix Lenght: "<<PDPrefix <<LogEnd; 
    this->PD_T1Beg    = opt->getT1Beg();
    this->PD_T2Beg    = opt->getT2Beg();
    this->PD_T1End    = opt->getT1End();
    this->PD_T2End    = opt->getT2End();
    this->PD_Share    = opt->getShare();
    this->PD_Prefix   = PDPrefix;
    PD_MaxLease = opt->getClassMaxLease();
    
    SmartPtr<TStationRange> PD_Range;
    /* For a while white and black list will be not used
	opt->firstRejedClnt();
    while(PD_Range=opt->getRejedClnt())
        this->RejedClnt.append(PD_Range);
    
    opt->firstAcceptClnt();
    while(PD_Range=opt->getAcceptClnt())
        this->AcceptClnt.append(PD_Range);
	*/
    opt->firstPool();
    this->PD_Pool = opt->getPool();
    Log(Debug) << "Pool used for PD: " << *this->PD_Pool << LogEnd;
    if (opt->getPool()) {
	Log(Warning) << "Two or more pool defined. Only one is used." << LogEnd;
    }

    // set up prefix counter counts
    this->PD_Count = this->PD_Pool->rangeCount();
    this->PD_Assigned = 0;
    Log(Debug) << "Range of pool "/* << this->PD_Pool.getPlain()<<  */" is: " << this->PD_Count << LogEnd;
    if (this->PD_MaxLease > this->PD_Count)
	this->PD_MaxLease = this->PD_Count;
}

bool TSrvCfgPD::prefixInPool(SmartPtr<TIPv6Addr> prefix)
{
    return PD_Pool->in(prefix);
}

//unsigned long TSrvCfgPD::countPrefixesInPool()
//{
//    return this->PD_Count;
//}

SmartPtr<TIPv6Addr> TSrvCfgPD::getRandomPrefix()
{
    return PD_Pool->getRandomPrefix();
}

SmartPtr<TIPv6Addr> TSrvCfgPD::getFirstAddrInPrefix()
{
    return PD_Pool->getAddrL();
}

unsigned long TSrvCfgPD::getPD_MaxLease() {
    return PD_MaxLease;
}

unsigned long TSrvCfgPD::getID()
{
    return this->ID;
}

unsigned long TSrvCfgPD::getShare() {
    return this->PD_Share;
}

long TSrvCfgPD::incrAssigned(int count) {
    this->PD_Assigned += count;
    return this->PD_Assigned;
}

long TSrvCfgPD::decrAssigned(int count) {
    this->PD_Assigned -= count;
    return this->PD_Assigned;
}

unsigned long TSrvCfgPD::getAssignedCount() {
    return this->PD_Assigned;
}

bool TSrvCfgPD::isLinkLocal() {
    SmartPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::",true);
    if (this->prefixInPool(addr)) {
	Log(Crit) << "Link local address (fe80::) belongs to the class." << LogEnd;
	return true;
    }

    addr = new TIPv6Addr("fe80:ffff:ffff:ffff:ffff:ffff:ffff:ffff", true);
    if (this->prefixInPool(addr)) {
	Log(Crit) << "Link local address (fe80:ffff:ffff:ffff:ffff:ffff:ffff:ffff) belongs to the class." << LogEnd;
	return true;
    }

    addr = this->PD_Pool->getAddrL();
    char linklocal[] = { 0xfe, 0x80};

    if (!memcmp(addr->getAddr(), linklocal,2)) {
	Log(Crit) << "Staring address " << addr->getPlain() << " is link-local." << LogEnd;
	return true;
    }
    
    addr = this->PD_Pool->getAddrR();
    if (!memcmp(addr->getAddr(), linklocal,2)) {
	Log(Crit) << "Ending address " << addr->getPlain() << " is link-local." << LogEnd;
	return true;
    }

    return false;
}

ostream& operator<<(ostream& out,TSrvCfgPD& PD_Class)
{
    out << "    <PD id=\"" << PD_Class.ID << "\" share=\"" << PD_Class.PD_Share << "\">" << std::endl;
    out << "      <!-- total prefixes in class: " << PD_Class.PD_Count 
	<< ", prefixes assigned: " << PD_Class.PD_Assigned << " -->" << endl;
    out << "      <T1 min=\"" << PD_Class.PD_T1Beg << "\" max=\"" << PD_Class.PD_T1End  << "\" />" << endl;
    out << "      <T2 min=\"" << PD_Class.PD_T2Beg << "\" max=\"" << PD_Class.PD_T2End  << "\" />" << endl;
    out << "      <PDMaxLease>" << PD_Class.PD_MaxLease << "</PDMaxLease>" << endl;
        
    SmartPtr<TStationRange> statRange;
    out << "      <!-- prefix range -->" << endl;
    out << *PD_Class.PD_Pool;
    
    /*out << "      <!-- reject-clients ranges:" << addrClass.RejedClnt.count() << " -->" << endl;
    addrClass.RejedClnt.first();
    while(statRange=addrClass.RejedClnt.get())
	out << *statRange;	
	
    out << "      <!-- accept-only ranges:" << addrClass.AcceptClnt.count() << " -->" << endl;
    addrClass.AcceptClnt.first();
    while(statRange=addrClass.AcceptClnt.get())
	out << *statRange;*/
    out << "    </PD>" << std::endl;
    return out;
}
